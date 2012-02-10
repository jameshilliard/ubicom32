/*
 * arch/ubicom32/mach-common/pcie.c
 *	PCI Express interface management.
 *
 * (C) Copyright 2010, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/arm
 *   arch/mips
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

#include <asm/devtree.h>
#include <asm/ubicom32.h>
#include <asm/cachectl.h>

#include "pcie.h"

struct pcie_port pcie_ports[PCIE_PORT_MAX];
static int num_pcie_ports;
static const char *port_names[PCIE_PORT_MAX] = {"pciej", "pciek"};

#ifdef PCIE_UBICOM_PROFILER
#include <asm/profile.h>
static volatile int int_total_count __attribute__((section(".ocm_data"))) = 0;
static volatile int int_inta_count __attribute__((section(".ocm_data"))) = 0;
static volatile int int_msi_count __attribute__((section(".ocm_data"))) = 0;
static volatile int int_linger_count __attribute__((section(".ocm_data"))) = 0;
extern volatile int pcie_win0_read;
extern volatile int pcie_win0_write;
extern volatile int pcie_win0_readback;
extern volatile int pcie_ctl0_rewrite;
#endif

static size_t size_to_bits(size_t sz)
{
	int i = 0;
	for (; i < 32; i++) {
		if (sz & (1 << i)) {
			break;
		}
	}
	BUG_ON(i == 32);

	/* Size must be power of 2 */
	BUG_ON(sz & ~(1 << i));
	return i;
}

/*
 * Write value to a rootport control register through BR
 */
static inline void pcie_core_reg_writel(struct pcie_port *pp, volatile u32 * addr, u32 val)
{
	/* Change access path to select BR access to core registers */
	struct ubicom32_io_port *ctl = pp->ctl;

	pcie_nbr_writel(&ctl->ctl1, PCIE_BR_ACCESS_CORE_REG);
	PCIE_BR_SWITCH_DELAY;

	pcie_br_writel(pp, addr, val);
	pcie_br_readl(pp, addr);	/* perform a read back after each write */
}

/*
 * Read a rootport control register through BR
 */
static inline u32 pcie_core_reg_readl(struct pcie_port *pp, volatile u32 * addr)
{
	/* Change access path to select BR access to core registers */
	struct ubicom32_io_port *ctl = pp->ctl;

	pcie_nbr_writel(&ctl->ctl1, PCIE_BR_ACCESS_CORE_REG);
	PCIE_BR_SWITCH_DELAY

	return pcie_br_readl(pp, addr);
}

/*
 * pcie_get_config_addr
 *	Combine the bus, dev, fn, and register offset into the address format expected by the controller.
 */
static inline u32 pcie_get_config_addr(unsigned int bus, unsigned int devfn, int where)
{
	/*
	 * For configuration transfers address register is used to identify targeted
	 * device and register as shown below:
	 *    Bits [63:32]: not used, must be zero
	 *    Bits [31:24]: targeted bus number (use 80h to access bridge configuration registers)
	 *    Bits [23:19]: targeted device number
	 *    Bits [18:16]: targeted function number
	 *    Bits [15:0]:  register number and control bits
	 *    Engineer sample chip:
	 *	Bits [15:12]: not used, must be zero
	 *      Bits [11:0]:  register or extended register number (bits [1:0] of must be set to 0)
	 *    Production chip:
	 *	Bits [15:4]:  register or extended register number (bits [5:4] of must be set to 0)
	 *      Bits [3]: not used, must be zero
	 *      Bits [2]: 0: read, 1: write
	 *      Bits [1]: start
	 *      Bits [0]: stop
	 *
	 * We only return the lower DW
	 */
#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	return (u32)((bus << 24) | (devfn << 16) | (where & 0x0ffc));
#else
	return (u32)((bus << 24) | (devfn << 16) | ((where & 0x0ffc) << 4));
#endif
}

/*
 * Dump most of the internel registers of the pcie controller
 *	Can be used even when debug is off
 */
static void pcie_dump_status_regs(struct pcie_port *pp)
{
#ifdef CONFIG_PCI_DEBUG
	int i;
	u32 *pci_reg;

	printk(KERN_INFO "pcie regdump port[%d]:\nphy->status[0]=%x, phy->control[0,3]=(%x, %x, %x, %x)\n",
			pp->port,
			pcie_nbr_readl(&pp->phy->status0),
			pcie_nbr_readl(&pp->phy->ctl0),
			pcie_nbr_readl(&pp->phy->ctl1),
			pcie_nbr_readl(&pp->phy->ctl2),
			pcie_nbr_readl(&pp->ctl->ctl3));
	printk(KERN_INFO "ctl->status[0]=%x, ctl->control[0,3]=(%x, %x, %x, %x, %x)\n",
			pcie_nbr_readl(&pp->ctl->status0),
			pcie_nbr_readl(&pp->ctl->ctl0),
			pcie_nbr_readl(&pp->ctl->ctl1),
			pcie_nbr_readl(&pp->ctl->ctl2),
			pcie_nbr_readl(&pp->ctl->ctl3),
			pcie_nbr_readl(&pp->ctl->ctl4));

	pci_reg = (u32 *)pp->core;
	for (i = 0; i < (sizeof(struct pcie_core_regs) / 4); i++) {
		printk(KERN_INFO " core REG[%08x] = 0x%08x\n",
			(u32)(&pci_reg[i]), pcie_core_reg_readl(pp, &pci_reg[i]));
	}
#endif
}

/*
 * Write value to a configuration register of the rootport core
 * Assumes spinlock of this port is held already by the caller.
 */
static inline void __pcie_core_cfg_writel(struct pcie_port *pp, int reg_offset, u32 val)
{
	u32 cfg_addr = pcie_get_config_addr(0, 0, reg_offset);
	u32 istatus;

	/* The rootport itself is using bus 0, dev 0 and func 0 */
	istatus = __pcie_cfg_writel(pp, cfg_addr, val);
	PCIE_INFO("\twrite value %x to reg %x\n", val, reg_offset);
	BUG_ON(istatus);
}

/*
 * Read a configuration register from the rootport core
 * Assumes spinlock of this port is held already by the caller.
 */
static inline u32 __pcie_core_cfg_readl(struct pcie_port *pp, int reg_offset)
{
	u32 cfg_addr = pcie_get_config_addr(0, 0, reg_offset);
	u32 val, istatus;

	/* The rootport itself is using bus 0, dev 0 and func 0 */
	istatus = __pcie_cfg_readl(pp, cfg_addr, &val);
	BUG_ON(istatus);

	PCIE_INFO("\tread value %x from reg %x\n", reg_offset, val);

	return val;
}

/*
 * ubi32_pcie_interrupt
 *
 * 	The interrupt handler for the pcie controller.
 *
 * All pcie related interrupts comes from the istatus register. We mask off DMA related
 * interrupts because we want to poll for the result, however, they may still show up in
 * the status register.
 *
 * We can not hold the spinlock while servicing the MSI interrupts which may do IO which
 * requires the spinlock again.
 */
static irqreturn_t ubi32_pcie_interrupt(int irq, void *arg)
{
	struct pcie_port *pp = (struct pcie_port *)arg;
	struct pcie_core_regs *core = pp->core;
	u32 istatus, imsistatus;

	PCIE_ASSERT(pp->ctl_irq == irq, "bad PCIe interrupt\n");

	spin_lock(&pp->conf_lock);

	/*
	 * read istatus to find out the top level interrupts
	 */
	PCIE_UBICOM_PROFILER_VCOUNT_INC(int_total_count);
#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	PCIE_ASSERT(!(pp->ctl->ctl3 & WSTALL_BIT), "PCIe DMA stalled\n");
	istatus = pcie_nbr_readl(&pp->ctl->status1);
#else
	PCIE_ASSERT(!(pp->ctl->ctl2 & WSTALL_BIT), "PCIe DMA stalled\n");
	istatus = pcie_core_reg_readl(pp, &core->istatus);
#endif

	/*
	 * Clear all but the DMA channel bits which are handled differently
	 * We also ignore some bits like doorbel, etc
	 */
	pcie_core_reg_writel(pp, &core->istatus, istatus & PCIE_INTR_CTL_MSK);

	spin_unlock(&pp->conf_lock);

#ifdef CONFIG_PCI_DEBUG
	if (!(istatus & (PCIE_INTR_MSI | PCIE_INTR_INTX))) {
		/*
		 * Print a simple warning if there is an interrupt other than MSI or INTA
		 */
		spin_lock(&pp->conf_lock);
		PCIE_INFO("pcie unexpected int, istatus=%x imask=%x\n", istatus,
			pcie_core_reg_readl(pp, &core->imask));
		spin_unlock(&pp->conf_lock);
	}
#endif

	/*
	 * Any DMA action involving the DMA engine writing something to DDR needs such
	 * a synchronization operation. This is to make sure the last DDR write, if exists,
	 * shows up properly in DDR before moving on.
	__pcie_ctl_dma_ready(pp->ctl);
	 */

	if (istatus & PCIE_INTR_MSI) {
		/*
		 * Read the MSI interrupt status out and clear them
		 */
		spin_lock(&pp->conf_lock);

		imsistatus = pcie_core_reg_readl(pp, &core->imsistatus);
		pcie_core_reg_writel(pp, &core->imsistatus, imsistatus);

		spin_unlock(&pp->conf_lock);

		/*
		 * Dispatch all MSI interrupts
		 */
		imsistatus &= (1 << PCIE_PORT_MSI_MAX) - 1;
		if (imsistatus) {
			int i = 0;
			for (; imsistatus && (i < 32); i++, imsistatus >>= 1) {
				if (imsistatus & 1) {
					PCIE_UBICOM_PROFILER_VCOUNT_INC(int_msi_count);
					PCIE_TRACE("PCIe irq relay -> msi:%d\n", pp->msi_irq_base + i);
					generic_handle_irq(pp->msi_irq_base + i);
				}
			}
		}
	}

	if (istatus & PCIE_INTR_INTX) {
		PCIE_TRACE("PCIe irq relay -> inta:%d\n", pp->inta_irq);
		PCIE_UBICOM_PROFILER_VCOUNT_INC(int_inta_count);
		generic_handle_irq(pp->inta_irq);

		/*
		 * The pcie HW has a feature in which SW is able to clear the INTx interrupt only
		 * when device send a INTx_DEASSERT message. So it is perfectly fine to "try' to
		 * clear INTx AFTER interrupt processing. NO RACE CONDITION HERE!
		 */
		spin_lock(&pp->conf_lock);
		pcie_core_reg_writel(pp, &core->istatus, PCIE_INTR_INTX);
		spin_unlock(&pp->conf_lock);
	}

#ifdef PCIE_UBICOM_PROFILER
	spin_lock(&pp->conf_lock);
	if (pcie_core_reg_readl(pp, &core->istatus) & PCIE_INTR_CTL_MSK) {
		PCIE_UBICOM_PROFILER_VCOUNT_INC(int_linger_count);
	}
	spin_unlock(&pp->conf_lock);
#endif
	return IRQ_HANDLED;
}

/*
 * One of the hw_pci methods.
 * Allocate necessary memory and IO resources for all pcie controllers.
 */
static void __init ubi32_pcie_preinit(void)
{
	int i;
	int ret;

	for (i = 0; i < num_pcie_ports; i++) {
		struct pcie_port *pp = &pcie_ports[i];

		/* Skip down links */
		if (!pp->is_up) {
			continue;
		}

		PCIE_INFO("Preinit for pcie port %d\n", pp->port);

		/* reserve IO and memory resources */
		ret = request_resource(&ioport_resource, &pp->res[0]);
		//BUG_ON(ret);
		ret = request_resource(&iomem_resource, &pp->res[1]);
		BUG_ON(ret);

	}
}

/*
 * pci_ops method to read a config register of size 1, 2, 4 bytes
 */
static int pcie_read_config(struct pci_bus *bus, unsigned int devfn,
				 int where, int size, u32 *value)
{
	struct pcie_port *pp = ((struct pci_sys_data *)bus->sysdata)->pport;
	u32 cfg_addr = pcie_get_config_addr(bus->number, devfn, where);
	u32 istatus;

	PCIE_TRACE("pcie_read_config for bus(%x) devfn(%x) where(%d) sz(%d)\n", bus->number,
			devfn, where, size);

	if (pcie_valid_config(pp, bus->number, PCI_SLOT(devfn)) == 0) {
		*value = 0xffffffff;
		PCIE_WARNING("\tdevice invalid, skip\n");
		return PCIBIOS_DEVICE_NOT_FOUND;
	}

	spin_lock_irqsave(&pp->conf_lock, pp->flags);
	istatus = __pcie_cfg_readl(pp, cfg_addr, value);
	spin_unlock_irqrestore(&pp->conf_lock, pp->flags);

	if (istatus) {
		printk(KERN_WARNING "Failed conf rd for reg %d of %d:%d:%d, istatus=%x\n",
			where, 	bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn), istatus);
		return PCIBIOS_DEVICE_NOT_FOUND;
	}

	if (size != 4) {
		int shift = (where & 0x3) << 3;
		PCIE_TRACE("Config register (4b) at %x is %08x, size=%d, shift=%d\n", where & ~0x03, *value, size, shift);
		*value = *value >> shift;
		*value = (size == 2) ? (*value & 0xffff) : (*value & 0xff);
	}

	PCIE_TRACE("\tvalue=0x%08x\n", *value);
	return PCIBIOS_SUCCESSFUL;
}

/*
 * pci_ops method to write a config register of size 1, 2, 4 bytes
 */
static int pcie_write_config(struct pci_bus *bus, unsigned int devfn,
				  int where, int size, u32 value)
{
	struct pcie_port *pp = ((struct pci_sys_data *)bus->sysdata)->pport;
	u32 cfg_addr = pcie_get_config_addr(bus->number, devfn, where);
	u32 val = value;
	u32 istatus;

	PCIE_TRACE("pcie_write_config for bus(%x) devfn(%x) where(%d) sz(%d), val=%x\n", bus->number,
			devfn, where, size, value);

	if (pcie_valid_config(pp, bus->number, PCI_SLOT(devfn)) == 0) {
		PCIE_WARNING("\tdevice invalid, skip\n");
		return PCIBIOS_DEVICE_NOT_FOUND;
	}

	spin_lock_irqsave(&pp->conf_lock, pp->flags);
	if (size != 4) {
		int shift = (where & 0x3) << 3;
		istatus = __pcie_cfg_readl(pp, cfg_addr, &val);
		if (istatus) {
			printk(KERN_WARNING "Failed conf wr first step (rd) for reg %d of %d:%d:%d, istatus=%x\n",
				where,  bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn), istatus);
			goto exit1;
		}

		if (size == 1) {
			val &= ~(0xff << shift);
		} else {
			val &= ~(0xffff << shift);
		}
		val |= (value << shift);
	}

	istatus = __pcie_cfg_writel(pp, cfg_addr, val);

exit1:
	spin_unlock_irqrestore(&pp->conf_lock, pp->flags);

	if (istatus) {
		printk(KERN_WARNING "Failed conf wr for reg %d of %d:%d:%d, istatus=%x\n",
			where,  bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn), istatus);
		return PCIBIOS_DEVICE_NOT_FOUND;
	}

	return PCIBIOS_SUCCESSFUL;
}

/*
 * pcie config space read/write operation methods.
 */
static struct pci_ops ubi32_pcie_ops = {
	.read   = pcie_read_config,
	.write  = pcie_write_config,
};

/*
 * Prevent enumeration of root complex.
 */
static void __devinit rc_pci_fixup(struct pci_dev *dev)
{
	PCIE_INFO("rc_pci_fixup is called\n");
	if (dev->bus->parent == NULL && dev->devfn == 0) {
		int i;
		for (i = 0; i < DEVICE_COUNT_RESOURCE; i++) {
			dev->resource[i].start = 0;
			dev->resource[i].end   = 0;
			dev->resource[i].flags = 0;
		}
	}
}
DECLARE_PCI_FIXUP_HEADER(PCI_VENDOR_ID_UBICOM, PCI_ANY_ID, rc_pci_fixup);

/*
 * A hw_pci method to scan all the buses of a specific root complex (pcie controller)
 */
static struct pci_bus __init *ubi32_pcie_scan_bus(int nr, struct pci_sys_data *sys)
{
	struct pcie_port *pp;
	PCIE_INFO("ubi32_pcie_scan_bus is called for nr=%d\n", nr);

	BUG_ON(nr >= num_pcie_ports);

	pp = sys->pport;
	BUG_ON(nr != pp->port);

	if (!pp->is_up) {
		return NULL;
	}

	return pci_scan_bus(sys->busnr, &ubi32_pcie_ops, sys);
}

/*
 * Set up the pciahb and ahbpci windows of a pcie controller.
 */
static void __init pcie_port_setup_wins(struct pcie_port *pp)
{
	struct pcie_core_regs *core = pp->core;
	size_t bits;

#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	/* Disable window1 of both pciahb and ahbpci windows */
	pcie_core_reg_writel(pp, &core->pciahb1_ahbbase, 0);
	pcie_core_reg_writel(pp, &core->ahbpcihi1_ahbbase, 0);
	pcie_core_reg_writel(pp, &core->ahbpcilo1_ahbbase, 0);

	/*
	 * Map PCI address 1:1 into physical memory with offset == 0, to allow PCI to access
	 * the full DDR range.
	 *
	 * Note: if we can assume the max DDR size is 512M, maybe we can reserve the 2nd half
	 * of DDR memory space to allocate to the devices connected to the root port, starting
	 * from 0xe0000000 to 0xfffffff -- another choice
	 *
	 * FIXME: find a method to get the real DDR size
	 */
	bits = size_to_bits(0xffffffff - SDRAMSTART + 1);
	pcie_core_reg_writel(pp, &core->pciahb0_ahbbase, PCIE_PCIAHB_CTL(SDRAMSTART, 1, 0, bits));
	PCIE_TRACE("pcie_port_setup_wins: pciahb %x\n", PCIE_PCIAHB_CTL(SDRAMSTART, 1, 0, bits));

	/* setup BAR0 to start of DRAM, and enable prefetch */
	__pcie_core_cfg_writel(pp, PCIE_CFG_OFFSET_BAR0,
		SDRAMSTART | PCI_BASE_ADDRESS_MEM_PREFETCH | PCI_BASE_ADDRESS_SPACE_MEMORY);

	/*
	 * Program the AHB-PCI window0 based on resource we got, which should be different
	 * for each rootport instance. We assume res[1] is mem mapped space and there is
	 * only one device attached to the root complex.
	 * FIXME: if the above assumption does not stand
	 */
	BUG_ON(pp->res[1].flags != IORESOURCE_MEM);
	bits = size_to_bits(pp->res[1].end  - pp->res[1].start + 1);
	pcie_core_reg_writel(pp, &core->ahbpcihi0_ahbbase, 0);
	pcie_core_reg_writel(pp, &core->ahbpcilo0_ahbbase, PCIE_AHBPCI_CTL_LOW(pp->res[1].start, 0, bits));
	PCIE_TRACE("pcie_port_setup_wins: ahbpci %x\n", PCIE_AHBPCI_CTL_LOW(pp->res[1].start, 0, bits));

#else	/* CONFIG_UBICOM32_IP8K_SHUTTLE */

	/* Enable master window 0 to all CPU memory */
	bits = size_to_bits(0xffffffff - SDRAMSTART + 1);
	pcie_core_reg_writel(pp, &core->axi_mas_win0_addr, SDRAMSTART);
	pcie_core_reg_writel(pp, &core->axi_mas_win0_size, (0xffffffff << bits) | 1);
	pcie_core_reg_writel(pp, &core->axi_mas_win0_lsb, SDRAMSTART);
	pcie_core_reg_writel(pp, &core->axi_mas_win0_msb, 0);
	PCIE_TRACE("pcie_port_setup_wins: master addr(%x) - size(%x)\n",
		SDRAMSTART, (0xffffffff << bits) | 1);

	/* setup BAR0 to start of DRAM, and enable prefetch */
	__pcie_core_cfg_writel(pp, PCIE_CFG_OFFSET_BAR0,
		SDRAMSTART | PCI_BASE_ADDRESS_MEM_PREFETCH | PCI_BASE_ADDRESS_SPACE_MEMORY);

	/*
	 * Program the AXI slave window[pp=>port] based on resource we got, which should be different
	 * for each rootport instance. We assume res[1] is mem mapped space and there is
	 * only one device attached to the root complex.
	 * FIXME: if the above assumption does not stand
	 */
	BUG_ON(pp->res[1].flags != IORESOURCE_MEM);
	bits = size_to_bits(pp->res[1].end  - pp->res[1].start + 1);
	pcie_core_reg_writel(pp, &core->axi_slv_win0_addr, pp->res[1].start & 0xfffff000);
	pcie_core_reg_writel(pp, &core->axi_slv_win0_size, (0xffffffff << bits) | 1);
	pcie_core_reg_writel(pp, &core->axi_slv_win0_msb, 0);
	pcie_core_reg_writel(pp, &core->axi_slv_win0_lsb, pp->res[1].start & 0xfffff000);
	PCIE_TRACE("pcie_port_setup_wins: slave addr(%x) - size(%x)\n",
		pp->res[1].start & 0xfffff000, (0xffffffff << bits) | 1);

#endif	/* CONFIG_UBICOM32_IP8K_SHUTTLE */
}

/*
 * Set up some ID related config space registers.
 */
static void __init pcie_port_setup_ids(struct pcie_port *pp)
{
	/* FIXME: Update vendor ID and device ID field. */
	u32 id = PCI_VENDOR_ID_UBICOM | (PCIE_RC_UBICOM_DEV_ID << 16);
	__pcie_core_cfg_writel(pp, PCI_VENDOR_ID, id);

	/* FIXME: class_code << 16 | revision_id, hardcode now. */
	id = PCI_CLASS_BRIDGE_PCI << 16 | 02;
	__pcie_core_cfg_writel(pp, PCI_CLASS_REVISION, id);

	/* head_type << 16 | cache_line == 32 */
	id = (PCI_HEADER_TYPE_BRIDGE << 16) | (L1_CACHE_BYTES >> 2);
	__pcie_core_cfg_writel(pp, PCI_CACHE_LINE_SIZE, id);
}

/*
 * Set up the controller and enable it.
 */
static int __init ubi32_pcie_setup(int nr, struct pci_sys_data *sys)
{
	struct pcie_port *pp;
	u32 cmd, bus;

	PCIE_INFO("ubi32_pcie_setup is called for nr=%d\n", nr);

	if (nr >= num_pcie_ports)
		return 0;

	pp = &pcie_ports[nr];
	sys->pport = pp;

	if (!pp->is_up) {
		return 0;
	}

	spin_lock_irqsave(&pp->conf_lock, pp->flags);

	pp->root_bus_nr = sys->busnr;

	/* Set up PCIe unit decode windows to DRAM space */
	pcie_port_setup_wins(pp);

	/* Set up device, vendor, class, etc */
	pcie_port_setup_ids(pp);

	/*
	 * FIXME: Master + slave enable. Do we need IO?
	 */
	cmd = __pcie_core_cfg_readl(pp, PCI_COMMAND);
	cmd |= (PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);
	__pcie_core_cfg_writel(pp,  PCI_COMMAND, cmd);

	/*
	 * Config local bus numbes
	 * Set Primary Bus Number = nr, Sec. = nr + 1 and Sub. = 0xFF
	 * It is up to the pci core enumeration code to set subordinate bus properly
	 *
	 * FIXME: once primary bus number is assigned, should we still use BDF(0, 0, 0)
	 * to access the second controller?
	 */
	bus = (0xFF << 16) | ((sys->busnr + 1) << 8) | sys->busnr;
	__pcie_core_cfg_writel(pp, PCI_PRIMARY_BUS, bus);

	/* request irq for the controller itself */
	if (request_irq(pp->ctl_irq, ubi32_pcie_interrupt, 0, port_names[pp->port], pp)) {
		BUG();
	}

	/*
	 * Enable those interesting interrupts,  assumes only one device connected
	 * to this root bridge.
	 * FIXME: add more error INTR checking
	 */
	pcie_core_reg_writel(pp, &pp->core->imask, PCIE_INTR_CTL_MSK);

	/*
	 * FIXME: switch to the above solution after it is fixed in hardware.
	 * workaround for 1st tapeout: interrupts are not delivered if we only set CTL_MSK here
	 * endianess is not the cause.
	 */
	pp->ctl->int_mask = 0xffffffff;

	/* Update resources */
	sys->resource[0] = &pp->res[0];
	sys->resource[1] = &pp->res[1];
	sys->resource[2] = NULL;

	pcie_dump_status_regs(pp);

	spin_unlock_irqrestore(&pp->conf_lock, pp->flags);

	return 1;
}

/*
 * Map a PCIe device's interrupt pin number to system interrupt number
 */
static int ubi32_pcie_map_irq(struct pci_dev *dev, u8 slot, u8 pin)
{
	struct pcie_port *pp = ((struct pci_sys_data *)dev->bus->sysdata)->pport;

	PCIE_INFO("ubi32_pcie_map_irq is called for bus(%d) slot (%d), pin(%d)\n",
			dev->bus->number, slot, pin);

	/*
	 * Force every device of a port to use the same irq. This is just a dummy
	 * place holder. Due to a hardware defect, the PLDA controller's INTx has some
	 * limitation, all devices are recommended to switch to MSI.
	 */
	return pp->inta_irq;
}

struct hw_pci ubi32_pci __initdata = {
	.nr_controllers	= 2,
	.preinit	= ubi32_pcie_preinit,
	.setup		= ubi32_pcie_setup,
	.scan		= ubi32_pcie_scan_bus,
	.map_irq	= ubi32_pcie_map_irq,
};

/*
 * Ubicom32 devtree supprot
 */
struct pcie_devtree_node {
	struct devtree_node dn;
	unsigned int pcie_dev;		/* port I/O address - either port J or K */
	unsigned int pcie_reset_port;
	unsigned int pcie_reset_pin;
};

struct pcie_devnode {
	struct 	pcie_devtree_node *pdn;
	int	inta_irq;
	int	msi_irq_base;
	struct resource res[2];
};

static struct pcie_devnode __initdata pcie_devtree[2] =
{
	{
		.pdn = NULL,
		/* use the power management intr to deliver INTa or MSI */
		.inta_irq = RJ_USB30PCIE_PWM_INT,	//should never be used
		.msi_irq_base = USB30PCIE_MSI_INT,	// we need two MSI, one for rootport, one for the device
							// this is a hack to steal reserved INTs
		.res[0] = {
			.name	= "PCIeRJ IO space",
			.start	= 0,
			.end	= 0,
			.flags	= IORESOURCE_IO,
		},
		.res[1] = {
			.name	= "PCIeRJ mem space",
			.start  = PCI_DEV_ADDR_BASE,
			.end	= PCI_DEV_ADDR_BASE + (PCI_DEV_ADDR_SIZE / 2) - 1,
			.flags	= IORESOURCE_MEM,
		},
	},
	{
		.pdn = NULL,
		/* use the power management intr to deliver INTa or MSI */
		.inta_irq = RK_USB30PCIE_PWM_INT,	//should never be used
		.msi_irq_base = (USB30PCIE_MSI_INT + PCIE_PORT_MSI_MAX),
		.res[0] = {
			.name	= "PCIeRK IO space",
			.start	= 0,
			.end	= 0,
			.flags	= IORESOURCE_IO,
		},
		.res[1] = {
			.name	= "PCIeRK mem space",
			.start  = PCI_DEV_ADDR_BASE + (PCI_DEV_ADDR_SIZE / 2),
			.end	= PCI_DEV_ADDR_BASE + PCI_DEV_ADDR_SIZE - 1,
			.flags	= IORESOURCE_MEM,
		},
	},
};

/*
 * Select a function (and keep it in reset state) for an IOPIO port
 */
static void inline __init pcie_io_port_func_select(struct ubicom32_io_port *io, int func)
{
	u32 func_val = pcie_nbr_readl(&io->function) & ~0x7;

	/* select the function */
	func_val |= IO_FUNC_SELECT(func);
	func_val |= IO_FUNC_FUNCTION_RESET(func);
	pcie_nbr_writel(&io->function, func_val);
}

/*
 * Reset a function of an IOPIO port
 */
static void inline __init pcie_io_port_func_reset(struct ubicom32_io_port *io, int func)
{
	u32 func_val = pcie_nbr_readl(&io->function);

	/* assert the reset */
	func_val |= IO_FUNC_FUNCTION_RESET(func);
	pcie_nbr_writel(&io->function, func_val);

	pcie_nbr_writel(&io->asyncerr_cause, 0);

	/* deassert the reset */
	func_val &= ~IO_FUNC_FUNCTION_RESET(func);
	pcie_nbr_writel(&io->function, func_val);
}

/*
 * A generic function to be called by board init function to use different GPIO settings if necessary
 */
static void __init ubi32_pcie_reset_endpoint(int port)
{
	u32 reset_pg = pcie_devtree[port].pdn->pcie_reset_port;
	u32 reset_pin = pcie_devtree[port].pdn->pcie_reset_pin;
	u32 clk_pg = IO_PORT_PG6;
	u32 clk_pin = port ? 1 : 0;
	printk("\tPCIe port[%d] reset: reset pin 0x%08x:%d clock pin 0x%08x:%d\n",
		port, reset_pg, reset_pin, clk_pg, clk_pin);

	/* Set reset_l bit to 0 */
	if (gpio_request((reset_pg | reset_pin) & 0xffff, "pcie_reset")) {
		printk(KERN_ERR "Can't request PCIe reset pin 0x%08x:%d\n", reset_pg, reset_pin);
		return;
	}
	gpio_direction_output((reset_pg | reset_pin) & 0xffff, 0);

	mdelay(100);

	/* Supply 100Mhz reference clock for the endpoint */
	asm volatile (
	"	or.4	"D(IO_GPIO_CTL)"(%0), "D(IO_GPIO_CTL)"(%0), %1	\n\t"
	"	or.4	"D(IO_GPIO_FN1)"(%0), "D(IO_GPIO_FN1)"(%0), %1	\n\t"
		:
		: "a" (clk_pg), "d" (1 << clk_pin)
		: "cc"
	);
	printk(KERN_INFO "\tPCIe port[%d]:  endpoint ref clock set to 100 Mhz\n", port);

	mdelay(100);

	/* Set reset_l bit to 1 */
	gpio_direction_output((reset_pg | reset_pin) & 0xffff, 1);
}

/*
 * Reset the port's phy and controller. After this function
 * the controller core is ready to be programed.
 */
static void __init pcie_port_reset(struct pcie_port *pp)
{
	struct ubicom32_io_port *phy = pp->phy;
	struct ubicom32_io_port *ctl = pp->ctl;
	spin_lock_irqsave(&pp->conf_lock, pp->flags);

	/* select the ctl with PCIe function */
	pcie_io_port_func_select(ctl, IO_PORTX_FUNC_PCIE);

	/* select & reset the phy with PCIe PHY function, and configure it */
	pcie_io_port_func_select(phy, IO_PORTX_FUNC_PCIE);
	pcie_io_port_func_reset(phy, IO_PORTX_FUNC_PCIUSB_PHY);
	pcie_phy_config(phy);
#if !defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	pcie_nbr_writel(&pp->ctl->ctl4, 0x1);
#endif

	/* remove ctl out of reset */
	pcie_io_port_func_reset(ctl, IO_PORTX_FUNC_PCIE);
#if !defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	pcie_nbr_writel(&phy->ctl2, 0x5);	// Enable PHY clock
#endif

	/* wait for phy to completely out of reset */
	PCIE_INFO("\twaiting for phy[0x%08x] out of reset\n", (u32)phy);
	while (pcie_nbr_readl(&phy->status0) & 2);
	PCIE_INFO("\tPhy out of reset, stat0=%x\n", pcie_nbr_readl(&phy->status0));

	/* wait for the ctl to completely out of reset */
#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	PCIE_INFO("\twaiting for the ctl[0x%08x] out of reset\n", (u32)ctl);
	while (!(pcie_nbr_readl(&ctl->status2) & PCIE_STATUS2_RESET_COMPLETE));
	PCIE_INFO("\tCtl out of reset, stat2=%x\n", pcie_nbr_readl(&ctl->status2));
#else
	PCIE_INFO("\tCtl out of reset, stat0=%x\n", pcie_nbr_readl(&ctl->status0));
#endif

	ubi32_pcie_reset_endpoint(pp->port);

#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	pcie_io_port_func_reset(ctl, IO_PORTX_FUNC_PCIE);

	/* wait for the ctl to completely out of reset */
	PCIE_INFO("\twaiting for the ctl[0x%08x] out of reset\n", (u32)ctl);
	while (!(pcie_nbr_readl(&ctl->status2) & PCIE_STATUS2_RESET_COMPLETE));
	PCIE_INFO("\tCtl out of reset, stat2=%x\n", pcie_nbr_readl(&ctl->status2));
#endif

	spin_unlock_irqrestore(&pp->conf_lock, pp->flags);
}

/*
 * Initialize the pcie controller core registers
 */
static void __init pcie_port_init_core(struct pcie_port *pp)
{
	/* core registers starts with BR offset 0 */
	struct pcie_core_regs *core = pp->core;
	spin_lock_irqsave(&pp->conf_lock, pp->flags);

	/* Mask off all interrupts initially */
	pcie_core_reg_writel(pp, &core->imask, 0);
	pcie_core_reg_writel(pp, &core->istatus, 0xffffffff);

	/* Write DEV_ID & Vendor ID (FIXME: we are doing it twice, again in cfg space setup_ids) */
	pcie_core_reg_writel(pp, &core->deviceid, (PCIE_RC_UBICOM_DEV_ID << 16) | PCI_VENDOR_ID_UBICOM);
	pcie_core_reg_writel(pp, &core->class_code, (0x06040002));

#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	/*
	 * ASPM must be configured, and timer is necessary to avoid blocking AHB bus
	 * Also set the proper gen2 mode
	 */
	pcie_core_reg_writel(pp, &core->ahbpci_timer, PCIE_AHBPCIE_TIMER_DATA);
	pcie_core_reg_writel(pp, &core->aspmconf, PCIE_ASPM_CONF_DATA);
	pcie_core_reg_writel(pp, &core->gen2_conf, PCIE_GEN2_CONF_DATA);
#else
	// writting BAR0 address range
	pcie_core_reg_writel(pp, &core->bar0,0xc0000000);

	// writting BAR1 address range
	pcie_core_reg_writel(pp, &core->bar1,0);

	// writting sub system ID value
	pcie_core_reg_writel(pp, &core->sub_id,CORE_SUB_SYS_ID_VAL);

	// enabling AER and ECRC check and Gen value
	pcie_core_reg_writel(pp, &core->aer_ecrc_cap,CORE_AER_ECRC_REG_VAL);

	// programming 2KB maxpaload
	pcie_core_reg_writel(pp, &core->max_payload,CORE_MAX_PAYLOAD_REG_VAL);

	// enabeling PME Ack
	pcie_core_reg_writel(pp, &core->ltssm,CORE_LTSSM_REG_VAL);

	// setting range for timeout 50us to 10ms
	pcie_core_reg_writel(pp, &core->time_out_comp,CORE_COMP_TIMEOUT_REG_VAL);

	// setting credit limit for PH=16 and PD=64
	pcie_core_reg_writel(pp, &core->fc_vc0_posted,CORE_VC0_P_CREDIT_REG_VAL);

	// setting credit limit for NPH=16 and NPD=64
	pcie_core_reg_writel(pp, &core->fc_vc0_nonposted,CORE_VC0_NP_CREDIT_REG_VAL);

	// setting slave window 0 addr(MWr32 MRd32)
	pcie_core_reg_writel(pp, &core->axi_slv_win0_addr,0);
	// setting slave window 0 size
	pcie_core_reg_writel(pp, &core->axi_slv_win0_size,0);
	// setting slave window 0 lsb of base addr PCIe window
	pcie_core_reg_writel(pp, &core->axi_slv_win0_lsb,0);
	// setting slave window 0 MSB of base addr PCIe window
	pcie_core_reg_writel(pp, &core->axi_slv_win0_msb,0);
	// setting slave window 1 addr(MWr64 MRd64)
	pcie_core_reg_writel(pp, &core->axi_slv_win1_addr,0);
	// setting slave window 1 size
	pcie_core_reg_writel(pp, &core->axi_slv_win1_size,0);
	// setting slave window 1 lsb of base addr PCIe window
	pcie_core_reg_writel(pp, &core->axi_slv_win1_lsb,0);
	// setting slave window 1 MSB of base addr PCIe window
	pcie_core_reg_writel(pp, &core->axi_slv_win1_msb,0);
	// setting slave window 2 addr(IOWr32 IoRd32)
	pcie_core_reg_writel(pp, &core->axi_slv_win2_addr,0);
	// setting slave window 2 size
	pcie_core_reg_writel(pp, &core->axi_slv_win2_size,0);
	// setting slave window 2 lsb of base addr PCIe window
	pcie_core_reg_writel(pp, &core->axi_slv_win2_lsb,0);
	// setting slave window 2 MSB of base addr PCIe window
	pcie_core_reg_writel(pp, &core->axi_slv_win2_msb,0);
	// setting slave window 3 addr(MWr32 MRd32)
	pcie_core_reg_writel(pp, &core->axi_slv_win3_addr,0);
	// setting slave window 3 size
	pcie_core_reg_writel(pp, &core->axi_slv_win3_size,0);
	// setting slave window 3 lsb of base addr PCIe window
	pcie_core_reg_writel(pp, &core->axi_slv_win3_lsb,0);
	// setting slave window 3 MSB of base addr PCIe window
	pcie_core_reg_writel(pp, &core->axi_slv_win3_msb,0);

	// setting master window 0 addr(MRd32-BAR0)
	pcie_core_reg_writel(pp, &core->axi_mas_win0_addr,0);
	// setting master window 0 size
	pcie_core_reg_writel(pp, &core->axi_mas_win0_size,0);
	// setting master window 0 lsb of base addr PCIe window
	pcie_core_reg_writel(pp, &core->axi_mas_win0_lsb,0);
	// setting master window 0 MSB of base addr PCIe window
	pcie_core_reg_writel(pp, &core->axi_mas_win0_msb,0);
	// setting master window 1 addr(MWr32 - BAR0)
	pcie_core_reg_writel(pp, &core->axi_mas_win1_addr,0);
	// setting master window 1 size
	pcie_core_reg_writel(pp, &core->axi_mas_win1_size,0);
	// setting master window 1 lsb of base addr PCIe window
	pcie_core_reg_writel(pp, &core->axi_mas_win1_lsb,0);
	// setting master window 1 MSB of base addr PCIe window
	pcie_core_reg_writel(pp, &core->axi_mas_win1_msb,0);
	// setting master window 2 addr(Mrd32 -BAR1)
	pcie_core_reg_writel(pp, &core->axi_mas_win2_addr,0);
	// setting master window 2 size
	pcie_core_reg_writel(pp, &core->axi_mas_win2_size,0);
	// setting master window 2 lsb of base addr PCIe window
	pcie_core_reg_writel(pp, &core->axi_mas_win2_lsb,0);
	// setting master window 2 MSB of base addr PCIe window
	pcie_core_reg_writel(pp, &core->axi_mas_win2_msb,0);
	// setting master window 3 addr(MWr32-BAR1)
	pcie_core_reg_writel(pp, &core->axi_mas_win3_addr,0);
	// setting master window 3 size
	pcie_core_reg_writel(pp, &core->axi_mas_win3_size,0);
	// setting master window 3 lsb of base addr PCIe window
	pcie_core_reg_writel(pp, &core->axi_mas_win3_lsb,0);
	// setting master window 3 MSB of base addr PCIe window
	pcie_core_reg_writel(pp, &core->axi_mas_win3_msb,0);
	// setting root control and status register
	pcie_core_reg_writel(pp, &core->rootcsr,CORE_PCIE_EXP_ROOT_CNT_VAL);
	// setting root control and status register
	pcie_core_reg_writel(pp, &core->cfg_pmcsr,CORE_PWR_MNGMNT_STS_CNT_VAL);


	// setting k_cnt_config register
	pcie_core_reg_writel(pp, &core->k_cnt_config_0,CORE_KCONT_CONFIG0_REG_VAL);
	pcie_core_reg_writel(pp, &core->k_cnt_config_1,CORE_KCONT_CONFIG1_REG_VAL);
	pcie_core_reg_writel(pp, &core->k_cnt_config_2,CORE_KCONT_CONFIG2_REG_VAL);
	pcie_core_reg_writel(pp, &core->k_cnt_config_3,CORE_KCONT_CONFIG3_REG_VAL);
	pcie_core_reg_writel(pp, &core->k_cnt_config_4,CORE_KCONT_CONFIG4_REG_VAL);
	pcie_core_reg_writel(pp, &core->k_cnt_config_5,CORE_KCONT_CONFIG5_REG_VAL);

	// Writing ASPM register Suggested by Rabiah
	pcie_core_reg_writel(pp, &core->aspm_l0s_cap,CORE_PCIE_ASPML0_CAP_VAL);
	pcie_core_reg_writel(pp, &core->aspm_l1_cap,CORE_PCIE_ASPML1_CAP_VAL );
	pcie_core_reg_writel(pp, &core->aspm_l0s_cap_2,CORE_PCIE_ASPML0_CAP_GEN2_VAL);
	pcie_core_reg_writel(pp, &core->pm_cap,CORE_PCIE_PM_MNGMT_CAP_VAL);
	pcie_core_reg_writel(pp, &core->msi_cnt_sts, CORE_MSI_CAPABILITY_VAL);
	//pcie_core_reg_writel(pp, &core->imsiaddr,CORE_MSI_MSG_ADDR_VAL);

	pcie_core_reg_writel(pp, &core->pcie_conf, 0x00000002);
	pcie_core_reg_writel(pp, &core->pcie_devscr_2, 0x00000001);
	pcie_core_reg_writel(pp, &core->pcie_linkscr_2, 0x00000002);

	// Writing in function control register 4, 31st bit to load written
	// value in core.
	pcie_nbr_writel(&pp->ctl->ctl4, 0x80000001);

	mdelay(1);
#endif

	PCIE_INFO("\tcore version 0x%08x\n", pcie_core_reg_readl(pp, &core->info));

	/* Set the msi report addr */
	pcie_core_reg_writel(pp, &core->imsiaddr, (u32)pp->msi_addr);

	spin_unlock_irqrestore(&pp->conf_lock, pp->flags);
	PCIE_INFO("\tcore init done\n");
}

/*
 * Keep polling link state till it is either L0 or too long
 */
static int __init pcie_port_link_state_L0(struct pcie_port *pp)
{
	u32 ltssm;
	u32 cnt = 0;

	printk(KERN_INFO "\tPolling LTSSM state ... takes up to 1/2 seconds\n");
	do {
#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
		/* LTSSM is reported by the lower 4 bits of status2 */
		ltssm = pcie_nbr_readl(&pp->ctl->status2);
		PCIE_INFO("\t\tLTSSM state = %x at trial %d\n", ltssm, cnt);
		ltssm &= 0xF;
#else
		/* LTSSM is reported by the lower 5 bits core LTSSM register */
		ltssm = pcie_core_reg_readl(pp, &pp->core->ltssm);
		PCIE_INFO("\t\tLTSSM state = %x at trial %d\n", ltssm, cnt);
		ltssm = ltssm >> 24;
#endif
		mdelay(20);
	} while ((ltssm != 0xF) && (cnt++ < 25));

	printk(KERN_INFO "\tLTSSM state of port %d is %x, %s\n", pp->port, ltssm, ltssm == 0xf? "up" : "down");
	return ltssm == 0xF;
}

/*
 * This function inits the phy and controller and brings up the link to L0 state
 * It assumes it is only called at init phase and no spinlock is needed.
 *
 * Return 0 means link is initialized properly and is up.
 */
static int __init pcie_port_init(struct pcie_port *pp)
{
	/* Update the clock source from 125 Mhz to 250 Mhz */
	BUG_ON(!(*(u32 *)(GENERAL_CFG_BASE + GEN_CLK_IO_SELECT) & (1 << pp->port)));
	PCIE_INFO("\tPCIe port %d: core clock source set to 250 Mhz\n", pp->port);

	mdelay(1);

	/* Reset and init the phy, core, wait for them to be ready */
	pcie_port_reset(pp);

	mdelay(1);

	/* Set endianess, role (RP), and mode (AER) properly, also set slotclk_cfg to jupiter common ref clock */
	pcie_nbr_writel(&pp->ctl->ctl2, 0);
	pcie_nbr_writel(&pp->ctl->ctl3, PCIE_BRIDGE_ENDIAN_RP_AER);

	mdelay(1);

#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	/* Doing crst/prst for loading new values in core reg */
	pcie_nbr_writel(&pp->ctl->int_set, 0x3);
	mdelay(10);
#endif

	/* Initialize the ctl core */
	pcie_port_init_core(pp);

	mdelay(1);

	/* poll to see if the link state moves to L0 */
	spin_lock_irqsave(&pp->conf_lock, pp->flags);
	if (!pcie_port_link_state_L0(pp)) {
		pcie_dump_status_regs(pp);
		spin_unlock_irqrestore(&pp->conf_lock, pp->flags);
		return -1;
	}
	spin_unlock_irqrestore(&pp->conf_lock, pp->flags);

	return 0;
}

/*
 * The main entrance to setup and enable a pcie controller port.
 * It does all the work before handling it to the generic pcibios code.
 */
static void __init add_pcie_port(int port, struct pcie_devnode *devnode)
{
	struct pcie_port *pp = &pcie_ports[num_pcie_ports++];

	printk(KERN_INFO "Trying add PCIe port %d: %s\n", port, devnode->pdn->dn.name);

	pp->port = port;
	/* FIXME: now we assume the device is already connected, need improve later after bringup */
	pp->is_up = 0;
	pp->root_bus_nr = -1;
	spin_lock_init(&pp->conf_lock);
	memset(pp->res, 0, sizeof(pp->res));

	pp->ctl_irq = devnode->pdn->dn.recvirq;
	pp->inta_irq = devnode->inta_irq;
	pp->msi_irq_base = devnode->msi_irq_base;
	pp->msi_irq_in_use = 0;

	pp->mem_base = devnode->res[1].start;
	pp->mem_mask = ~(devnode->res[1].end ^ devnode->res[1].start);

	/* Cache the starting address of port control registers */
	pp->phy = (struct ubicom32_io_port *)PCIE_PHY_FUNC_BASE(port);
	pp->ctl = (struct ubicom32_io_port *)PCIE_CTL_FUNC_BASE(port);
	pp->core = (struct pcie_core_regs *)PCIE_CTL_BR_BASE(port);
	BUG_ON((unsigned int)pp->ctl != devnode->pdn->pcie_dev);
	BUG_ON(pp->ctl_irq != (pp->inta_irq + 1));

	/*
	 * Setup the MSI address register, this address must not fall in the PCI-AHB window
	 * range, also it has to be 64 bit aligned.
	 *
	 * So any address not overlapping with DDR should work, for now just re-use the last
	 * qword address of the NBR region.
	 */
	pp->msi_addr = (u32)pp->core - 8;

#if !defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	pp->wr_cnt = 0;
	pp->wr_timestamp = 0;
#endif

	pp->res[0] = devnode->res[0];
	pp->res[1] = devnode->res[1];

	/* Init phy, core and make sure link is up */
	if (pcie_port_init(pp)) {
		printk(KERN_WARNING "PCIe port %d: %s init failed\n", port, devnode->pdn->dn.name);
		return;
	}

	/* Every thing is ok, mark this port up */
	pp->is_up = 1;
	printk(KERN_INFO "PCIe port %d: %s link is up\n", port, devnode->pdn->dn.name);
}

/*
 * The pcie subsystem init entry point
 */
static int __init ubi32_pcie_init(void)
{
	struct pcie_devtree_node *devnode0, *devnode1;

#ifdef PCIE_UBICOM_PROFILER
	profile_register_performance_counter(&int_total_count, "PCIe INT Total");
	profile_register_performance_counter(&int_inta_count, "PCIe INT INTA");
	profile_register_performance_counter(&int_msi_count, "PCIe INT MSI");
	profile_register_performance_counter(&int_linger_count, "PCIe INT Linger");
	profile_register_performance_counter(&pcie_win0_read, "WIN0 read");
	profile_register_performance_counter(&pcie_win0_write, "WIN0 write");
	profile_register_performance_counter(&pcie_win0_readback, "WIN0 readback");
	profile_register_performance_counter(&pcie_ctl0_rewrite, "CTL0 rewrite");
#endif

	PCIE_INFO("ubi32_pcie_init is called\n");
	devnode0 = (struct pcie_devtree_node *)devtree_find_node(port_names[0]);
	if (devnode0 != NULL) {
		printk(KERN_INFO "PCI port RJ is added\n");
		pcie_devtree[0].pdn = devnode0;
		add_pcie_port(0, &pcie_devtree[0]);
	}

	devnode1 = (struct pcie_devtree_node *)devtree_find_node(port_names[1]);
	if (devnode1 != NULL) {
		printk(KERN_INFO "PCI port RK is added\n");
		pcie_devtree[1].pdn = devnode1;
		add_pcie_port(1, &pcie_devtree[1]);
	}

	if ((devnode0 == NULL) && (devnode1 == NULL)) {
		printk(KERN_WARNING "PCI port RJ/RK init failed\n");
		return -ENOSYS;
	}

	pci_common_init(&ubi32_pci);
	PCIE_INFO("End of %s\n", __FUNCTION__);

	return 0;
}

subsys_initcall(ubi32_pcie_init);

