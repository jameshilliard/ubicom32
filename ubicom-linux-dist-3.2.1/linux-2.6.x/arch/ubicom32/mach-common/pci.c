/*
 * arch/ubicom32/mach-common/pci.c
 *	PCI interface management.
 *
 * (C) Copyright 2009, Ubicom, Inc.
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
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>

#include <asm/devtree.h>
#include <asm/ubicom32.h>

#include "pci_measurement.h"

#if defined(IP5000) || defined(IP7000)

/* #define PCI_USE_INTERNAL_LOCK 1 */

#ifdef PCI_USE_INTERNAL_LOCK
#define PCI_LOCK(lock, irqflag)	pci_lock_acquire(irqflag)
#define PCI_UNLOCK(lock, irqflag) pci_lock_release(irqflag)
#elif defined(CONFIG_SMP)
static DEFINE_SPINLOCK(pci_master_lock);
#define PCI_LOCK(lock, irqflag)	spin_lock_irqsave(lock, irqflag)
#define PCI_UNLOCK(lock, irqflag) spin_unlock_irqrestore(lock, irqflag)
#else
#define PCI_LOCK(lock, irqflag)		local_irq_save(irqflag)
#define PCI_UNLOCK(lock, irqflag)	local_irq_restore(irqflag)
#endif

#define PCI_DEV0_IDSEL CONFIG_PCI_DEV0_IDSEL
#define PCI_DEV1_IDSEL CONFIG_PCI_DEV1_IDSEL

/*
 * PCI commands
 */
#define PCI_CMD_INT_ACK		0x00	/* not supported */
#define PCI_CMD_SPECIAL		0x01	/* not supported */
#define PCI_CMD_IO_READ		0x02
#define PCI_CMD_IO_WRITE	0x03
#define PCI_CMD_MEM_READ	0x06
#define PCI_CMD_MEM_WRITE	0x07
#define PCI_CMD_CFG_READ	0x0a
#define PCI_CMD_CFG_WRITE	0x0b
#define PCI_CMD_MEM_READ_MULT	0x0c	/* not supported */
#define PCI_CMD_DUAL_ADDR	0x0d	/* not supported */
#define PCI_CMD_MEM_READ_LINE	0x0e	/* not supported */
#define PCI_CMD_MEM_WRITE_INVAL	0x0f	/* not supported */

/*
 * Status codes (none zero indicates error), returned by pci_read_u32() and pci_write_u32()
 */
#define PCI_RESP_OK		0
#define PCI_RESP_NO_DEVSEL		1  /* timeout before target asserted
					    * DEVSEL! */
#define PCI_RESP_LOST_DEVSEL		2  /* had DEVSEL, but went away before
					    * transfer completed! */
#define PCI_RESP_BAD_TRDY		3  /* target asserted TRDY without
					    * DEVSEL! */
#define PCI_RESP_NO_TRDY		4  /* timeout before target asserted
					    * TRDY! */
#define PCI_RESP_BAD_STOP		5  /* target asserted STOP and TRDY
					    * without DEVSEL! */
#define PCI_RESP_TARGET_ABORT		6
#define PCI_RESP_TARGET_RETRY		7
#define	PCI_RESP_TARGET_DISCONNECT	8
#define PCI_RESP_MISMATCH		9  /* data read back doesn't match data
					    * written - debug only, the core PCI
					    * routines never return this */
#define PCI_RESP_DET_SERR		10
#define PCI_RESP_DET_PERR		11
#define PCI_RESP_MALFORMED_REQ		12 /* Could be due to misaligned
					    * requests or invalid address */
#define PCI_RESP_NO_RESOURCE		13 /* Could be memory or other resourse
					    * like queue space */
#define PCI_RESP_ERROR			14 /* All emcompassing error */

struct pci_master_request {
	volatile u32_t pci_address;	/* must be 4-byte aligned */
	volatile u32_t data;		/* must be 4-byte aligned */
	volatile u8_t cmd;
	volatile u8_t byte_valid;
	volatile u8_t status;
};

struct pci_devnode {
	struct devtree_node dn;
	u32_t pci_idsel_0;
	u32_t pci_idsel_1;
	u32_t pci_cpu_address;
	struct pci_master_request volatile *volatile req;
};

static struct pci_master_request req;	/* globally used for faster master write
					 * (discarding result when possible) */
static struct pci_devnode *pci_node;


#if defined(PCI_USE_INTERNAL_LOCK)
/*
 * pci_lock_release()
 *	Release the PCI lock.
 */
static void pci_lock_release(unsigned long irqflag)
{
	UBICOM32_UNLOCK(PCI_LOCK_BIT);
}

/*
 * pci_lock_acquire()
 *	Acquire the PCI lock, spin if not available.
 */
static void pci_lock_acquire(unsigned long irqflag)
{
	UBICOM32_LOCK(PCI_LOCK_BIT);
}
#endif

/*
 * pci_set_hrt_interrupt()
 */
static inline void pci_set_hrt_interrupt(struct pci_devnode *pci_node)
{
	ubicom32_set_interrupt(pci_node->dn.sendirq);
}

/*
 * pci_read_u32()
 *	Synchronously read 32 bits from PCI space.
 */
u8 pci_read_u32(u8 pci_cmd, u32 address, u32 *data)
{
	unsigned long irqflag;

	/*
	 * Fill in the request.
	 */
	volatile struct pci_master_request lreq;
	STOPWATCH_DECLARE;

	lreq.pci_address = address;
	lreq.cmd = pci_cmd;
	lreq.byte_valid = 0xf;		/* enable all bytes */

	/*
	 * Wait for any previous request to complete and then make this request.
	 */
	STOPWATCH_START();
	PCI_LOCK(&pci_master_lock, irqflag);
	while (unlikely(pci_node->req));
	pci_node->req = &lreq;
	pci_set_hrt_interrupt(pci_node);
	PCI_UNLOCK(&pci_master_lock, irqflag);

	/*
	 * Wait for the result to show up.
	 */
	while (unlikely(pci_node->req == &lreq));
	*data = le32_to_cpu(lreq.data);
	if (unlikely(lreq.status != PCI_RESP_OK)) {
#ifdef CONFIG_PCI_DEBUG
		BUG_ON(pci_cmd != PCI_CMD_CFG_READ);
#endif
		*data = 0;
	}
	STOPWATCH_END(&pci_stopwatches[PCI_MEASUREMENT_READ32]);
	return lreq.status;
}

/*
 * pci_write_u32()
 *	Asyncrhnously or synchronously write 32 bits to PCI master space.
 */
u8 pci_write_u32(u8 pci_cmd, u32 address, u32 data)
{
	unsigned long irqflag;
	STOPWATCH_DECLARE;

	/*
	 * Wait for any previous write or pending read to complete.
	 *
	 * We use a global data block because once we write the request
	 * we do not wait for it to complete before exiting.
	 */
	STOPWATCH_START();
	PCI_LOCK(&pci_master_lock, irqflag);
	while (unlikely(pci_node->req));
	req.pci_address = address;
	req.data = cpu_to_le32(data);
	req.cmd = pci_cmd;
	req.byte_valid = 0xf;		/* enable all bytes */
	pci_node->req = &req;
	pci_set_hrt_interrupt(pci_node);

#ifdef CONFIG_PCI_DEBUG
	while (unlikely(pci_node->req));
	BUG_ON(req.status != PCI_RESP_OK);
#endif
	PCI_UNLOCK(&pci_master_lock, irqflag);
	STOPWATCH_END(&pci_stopwatches[PCI_MEASUREMENT_WRITE32]);
	return PCI_RESP_OK;
}

/*
 * pci_read_u16()
 *	Synchronously read 16 bits from PCI space.
 */
u8 pci_read_u16(u8 pci_cmd, u32 address, u16 *data)
{
	unsigned long irqflag;

	/*
	 * Fill in the request.
	 */
	volatile struct pci_master_request lreq;
	STOPWATCH_DECLARE;

	lreq.pci_address = address & ~2;
	lreq.cmd = pci_cmd;
	lreq.byte_valid = (address & 2) ? 0xc : 0x3;

	/*
	 * Wait for any previous request to complete and then make this request.
	 */
	STOPWATCH_START();
	PCI_LOCK(&pci_master_lock, irqflag);
	while (unlikely(pci_node->req));
	pci_node->req = &lreq;
	pci_set_hrt_interrupt(pci_node);
	PCI_UNLOCK(&pci_master_lock, irqflag);

	/*
	 * Wait for the result to show up.
	 */
	while (unlikely(pci_node->req == &lreq));
	lreq.data = le32_to_cpu(lreq.data);
	*data = (u16)((address & 2) ? (lreq.data >> 16) : lreq.data);
	if (unlikely(lreq.status != PCI_RESP_OK)) {
#ifdef CONFIG_PCI_DEBUG
		BUG_ON(pci_cmd != PCI_CMD_CFG_READ);
#endif
		*data = 0;
	}
	STOPWATCH_END(&pci_stopwatches[PCI_MEASUREMENT_READ16]);
	return lreq.status;
}

/*
 * pci_write_u16()
 *	Asyncrhnously or synchronously write 16 bits to PCI master space.
 */
u8 pci_write_u16(u8 pci_cmd, u32 address, u16 data)
{
	unsigned long irqflag;
	STOPWATCH_DECLARE;

	/*
	 * Wait for any previous write or pending read to complete.
	 *
	 * We use a global data block because once we write the request
	 * we do not wait for it to complete before exiting.
	 */
	STOPWATCH_START();
	PCI_LOCK(&pci_master_lock, irqflag);
	while (unlikely(pci_node->req));
	req.pci_address = address & ~2;
	req.data = (u32)data;
	req.data = cpu_to_le32((address & 2) ? (req.data << 16) : req.data);
	req.cmd = pci_cmd;
	req.byte_valid = (address & 2) ? 0xc : 0x3;
	pci_node->req = &req;
	pci_set_hrt_interrupt(pci_node);

#ifdef CONFIG_PCI_DEBUG
	while (unlikely(pci_node->req));
	BUG_ON(req.status != PCI_RESP_OK);
#endif
	PCI_UNLOCK(&pci_master_lock, irqflag);
	STOPWATCH_END(&pci_stopwatches[PCI_MEASUREMENT_WRITE16]);
	return PCI_RESP_OK;
}

/*
 * pci_read_u8()
 *	Synchronously read 8 bits from PCI space.
 */
u8 pci_read_u8(u8 pci_cmd, u32 address, u8 *data)
{
	unsigned long irqflag;

	/*
	 * Fill in the request.
	 */
	volatile struct pci_master_request lreq;
	STOPWATCH_DECLARE;

	lreq.pci_address = address & ~3;
	lreq.cmd = pci_cmd;
	lreq.byte_valid = 1 << (address & 0x3);

	/*
	 * Wait for any previous request to complete and then make this request.
	 */
	STOPWATCH_START();
	PCI_LOCK(&pci_master_lock, irqflag);
	while (unlikely(pci_node->req));
	pci_node->req = &lreq;
	pci_set_hrt_interrupt(pci_node);
	PCI_UNLOCK(&pci_master_lock, irqflag);

	/*
	 * Wait for the result to show up.
	 */
	while (unlikely(pci_node->req == &lreq));
	*data = (u8)(lreq.data >> (24 - ((address & 0x3) << 3)));
	if (unlikely(lreq.status != PCI_RESP_OK)) {
#ifdef CONFIG_PCI_DEBUG
		BUG_ON(pci_cmd != PCI_CMD_CFG_READ);
#endif
		*data = 0;
	}
	STOPWATCH_END(&pci_stopwatches[PCI_MEASUREMENT_READ8]);
	return lreq.status;
}

/*
 * pci_write_u8()
 *	Asyncrhnously or synchronously write 8 bits to PCI master space.
 */
u8 pci_write_u8(u8 pci_cmd, u32 address, u8 data)
{
	unsigned long irqflag;
	STOPWATCH_DECLARE;

	/*
	 * Wait for any previous write or pending read to complete.
	 *
	 * We use a global data block because once we write the request
	 * we do not wait for it to complete before exiting.
	 */
	STOPWATCH_START();
	PCI_LOCK(&pci_master_lock, irqflag);
	while (unlikely(pci_node->req));
	req.pci_address = address & ~3;
	req.data = ((u32)data << (24 - ((address & 0x3) << 3)));
	req.cmd = pci_cmd;
	req.byte_valid = 1 << (address & 0x3);
	pci_node->req = &req;
	pci_set_hrt_interrupt(pci_node);

#ifdef CONFIG_PCI_DEBUG
	while (unlikely(pci_node->req));
	BUG_ON(req.status != PCI_RESP_OK);
#endif
	PCI_UNLOCK(&pci_master_lock, irqflag);
	STOPWATCH_END(&pci_stopwatches[PCI_MEASUREMENT_WRITE8]);
	return PCI_RESP_OK;
}

unsigned int ubi32_pci_read_u32(const volatile void __iomem *addr)
{
	unsigned int data;
	pci_read_u32(PCI_CMD_MEM_READ, (u32)addr, &data);
	return data;
}
EXPORT_SYMBOL(ubi32_pci_read_u32);

unsigned short ubi32_pci_read_u16(const volatile void __iomem *addr)
{
	unsigned short data;
	pci_read_u16(PCI_CMD_MEM_READ, (u32)addr, &data);
	return data;
}
EXPORT_SYMBOL(ubi32_pci_read_u16);

unsigned char  ubi32_pci_read_u8(const volatile void __iomem *addr)
{
	unsigned char  data;
	pci_read_u8(PCI_CMD_MEM_READ, (u32)addr, &data);
	return data;
}
EXPORT_SYMBOL(ubi32_pci_read_u8);

void ubi32_pci_write_u32(unsigned int val, const volatile void __iomem *addr)
{
	pci_write_u32(PCI_CMD_MEM_WRITE, (u32)addr, val);
}
EXPORT_SYMBOL(ubi32_pci_write_u32);

void ubi32_pci_write_u16(unsigned short val, const volatile void __iomem *addr)
{
	pci_write_u16(PCI_CMD_MEM_WRITE, (u32)addr, val);
}
EXPORT_SYMBOL(ubi32_pci_write_u16);

void ubi32_pci_write_u8(unsigned char val, const void volatile __iomem *addr)
{
	pci_write_u8(PCI_CMD_MEM_WRITE, (u32)addr, val);
}
EXPORT_SYMBOL(ubi32_pci_write_u8);

static int ubi32_pci_read_config(struct pci_bus *bus, unsigned int devfn,
				 int where, int size, u32 *value)
{
	u8 cmd;
	u32 addr;
	u8  data8;
	u16 data16;

	u8 slot = PCI_SLOT(devfn);
	u8 fn = PCI_FUNC(devfn);

	if (slot > 1) {
		return PCIBIOS_DEVICE_NOT_FOUND;
	} else if (slot == 0) {
		addr = PCI_DEV0_IDSEL + where;
	} else {
		addr = PCI_DEV1_IDSEL + where;
	}

	addr += (fn << 8);

	cmd = PCI_CMD_CFG_READ;
	if (size == 1) {
		pci_read_u8(cmd, addr, &data8);
		*value = (u32)data8;
	} else if (size == 2) {
		pci_read_u16(cmd, addr, &data16);
		*value = (u32)data16;
	} else {
		pci_read_u32(cmd, addr, value);
	}

	return PCIBIOS_SUCCESSFUL;
}

static int ubi32_pci_write_config(struct pci_bus *bus, unsigned int devfn,
				  int where, int size, u32 value)
{
	u8 cmd;
	u32 addr;
	u8 slot = PCI_SLOT(devfn);
	u8 fn = PCI_FUNC(devfn);

	if (slot > 1) {
		return PCIBIOS_DEVICE_NOT_FOUND;
	} else if (slot == 0) {
		addr = PCI_DEV0_IDSEL + where;
	} else {
		addr = PCI_DEV1_IDSEL + where;
	}

	addr += (fn << 8);

	cmd = PCI_CMD_CFG_WRITE;
	if (size == 1) {
		pci_write_u8(cmd, addr, (u8)value);
	} else if (size == 2) {
		pci_write_u16(cmd, addr, (u16)value);
	} else {
		pci_write_u32(cmd, addr, value);
	}

	return PCIBIOS_SUCCESSFUL;
}

static struct pci_ops ubi32_pci_ops = {
	.read   = ubi32_pci_read_config,
	.write  = ubi32_pci_write_config,
};

static struct pci_bus *ubi32_pci_scan_bus(int nr, struct pci_sys_data *sys)
{
	return pci_scan_bus(sys->busnr, &ubi32_pci_ops, sys);
}

static struct resource ubi32_pci_mem = {
	.name	= "PCI memory space",
	.start	= PCI_DEV_ADDR_BASE,
	.end	= PCI_DEV_ADDR_BASE + PCI_DEV_ADDR_SIZE - 1,
	.flags	= IORESOURCE_MEM,
};

static struct resource ubi32_pci_io = {
	.name	= "PCI IO space",
	.start	= 0,
	.end	= 0,
	.flags	= IORESOURCE_IO,
};

static int __init ubi32_pci_setup(int nr, struct pci_sys_data *sys)
{
	if (nr > 0)
		return 0;

	request_resource(&iomem_resource, &ubi32_pci_mem);
	request_resource(&ioport_resource, &ubi32_pci_io);

	sys->resource[0] = &ubi32_pci_io;
	sys->resource[1] = &ubi32_pci_mem;
	sys->resource[2] = NULL;

	return 1;
}

static void __init ubi32_pci_preinit(void)
{
}

static int __init ubi32_pci_map_irq(struct pci_dev *dev, u8 slot, u8 pin)
{
	return pci_node->dn.recvirq;
}

struct hw_pci ubi32_pci __initdata = {
	.nr_controllers	= 1,
	.preinit	= ubi32_pci_preinit,
	.setup		= ubi32_pci_setup,
	.scan		= ubi32_pci_scan_bus,
	.map_irq	= ubi32_pci_map_irq,
};

static int __init ubi32_pci_init(void)
{
	pci_node = (struct pci_devnode *)devtree_find_node("pci");
	if (pci_node == NULL) {
		printk(KERN_WARNING "PCI init failed\n");
		return -ENOSYS;
	}
	pci_common_init(&ubi32_pci);
	return 0;
}

subsys_initcall(ubi32_pci_init);

/*
 * workaround for dual PCI card interrupt
 */
void ubi32_pci_int_wr(void)
{
#define PCI_COMMON_INT_BIT (1 << 19)
	volatile unsigned int pci_int_line;
	pci_int_line = UBICOM32_IO_PORT(IO_PORT_RB)->gpio_in;
	if (!(pci_int_line & PCI_COMMON_INT_BIT))
	{
		ubicom32_set_interrupt(pci_node->dn.recvirq);
	}
}
EXPORT_SYMBOL(ubi32_pci_int_wr);

#endif /* IP5000 || IP7000 */
