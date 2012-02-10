/*
 * arch/ubicom32/mach-common/pcie_io.c
 *      PCI Express interface management.
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
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <asm/ubicom32.h>
#include <asm/cachectl.h>

#include "pcie.h"

#ifdef PCIE_UBICOM_PROFILER
volatile int pcie_win0_read __attribute__((section(".ocm_data"))) = 0;
volatile int pcie_win0_write __attribute__((section(".ocm_data"))) = 0;
volatile int pcie_win0_readback __attribute__((section(".ocm_data"))) = 0;
volatile int pcie_ctl0_rewrite __attribute__((section(".ocm_data"))) = 0;
#endif

#define USE_PCIE_AHBPCI_WIN0 1
//#define USE_PCIE_BE 1

#define ADDR_ALIGN(addr) ((void *)((u32)addr & ~0x03))
#define ADDR_SHIFT(addr) (((u32)addr & 0x03) << 3)

/*
 * The controller has the capability of read/write byte, word.
 * For bringup, let's just read DW through PCI. Improve later.
 */

/*
 * Helper function to find the corresponding pcie port of a pci memory mapped I/O address
 */
static inline struct pcie_port *addr_to_port(const volatile void __iomem *addr)
{
	PCIE_ASSERT(((pcie_ports[0].mem_mask & (u32)addr) == pcie_ports[0].mem_base) ||
		    ((pcie_ports[1].mem_mask & (u32)addr) == pcie_ports[1].mem_base),
		    "bad PCIe address\n");
	return ((pcie_ports[0].mem_mask & (u32)addr) == pcie_ports[0].mem_base) ?
		&pcie_ports[0] : &pcie_ports[1];
}

/*
 * Helper function to find the corresponding pcie port of a pci I/O port address
 */
static inline struct pcie_port *ioport_to_port(unsigned long addr)
{
#if 0
	PCIE_ASSERT(((pcie_ports[0].io_mask & addr) == pcie_ports[0].io_base) ||
		    ((pcie_ports[1].io_mask & addr) == pcie_ports[1].io_base),
		    "bad PCIe IO address\n");
	return ((pcie_ports[0].io_mask & addr) == pcie_ports[0].io_base) ?
		&pcie_ports[0] : &pcie_ports[1];
#endif
	return addr_to_port((const volatile void __iomem *)addr);
}

/*
 * DMA / interrupt synchronization helper
 * Assumes spinlock of this port is held already by the caller.
 */
static inline void __pcie_ctl_dma_ready(struct ubicom32_io_port *ctl)
{
	/* polls dma_wcmdq_empty bit of STATUS0 */
	if (likely(ctl->status0 & WCMDQ_EMPTY_BIT)) {
		return;
	}

	PCIE_TRACE("dma is not ready, set stall bit and wait\n");
	/* tell the device stop feeding till the current written data is ready */
#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	ctl->ctl3 |= WSTALL_BIT;
#else
	ctl->ctl2 |= WSTALL_BIT;
#endif

	/* keep polling till it is ready */
	while (!(ctl->status0 & WCMDQ_EMPTY_BIT));

	/* clear the stall bit */
#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	ctl->ctl3 &= ~WSTALL_BIT;
#else
	ctl->ctl2 &= ~WSTALL_BIT;
#endif
}

#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
/*
 * Ask the internal DMA engine of the controller to make a 32 bit read (configure, mem, or IO)
 *
 *	core: 		the starting address of the controller registers
 *	addr_lo:	low 32 bit of a possible 64 address
 *	val:		the place to keep the read value
 *	dma_type:	configure space, memory, IO, value defined by the datasheet
 *	be:		byte enable		// FIXME: not working due to the endian issue in the first tape-out
 *	swap:		do endian swap		// FIXME: may not be unnecessary to have this flexibility
 */
static u32 __pcie_dma_readl(struct pcie_port *pp, u32 addr_lo, u32 *val, u32 dma_type, u32 be)
{
	struct pcie_core_regs *core = pp->core;
	u32 istatus;
	int cnt = 0;

	PCIE_TRACE("read DMA: REG[%x]\n", addr_lo);
	PCIE_UBICOM_PROFILER_VCOUNT_INC(pcie_win0_read);

	asm volatile (
	"	flush		(%0)		\n\t"
		:
		: "a" (&pp->buf)
	);

	/*
	 * Clear possible stale istatus bits. There might take effect immediately,
	 * but should be faster than the following writes.
	 */
	pcie_nbr_writel(&pp->ctl->ctl1, PCIE_BR_ACCESS_CORE_REG);
	PCIE_BR_SWITCH_DELAY;
	pcie_br_writel(pp, &core->istatus, PCIE_INTR_RD_DMA_MSK);

	/* Set up DMA descriptors */
	pcie_br_writel(pp, &core->rdma_pci_addr_0, addr_lo);
	pcie_br_writel(pp, &core->rdma_pci_addr_1, 0);
	pcie_br_writel(pp, &core->rdma_ahb_addr, (u32)(&pp->buf));

	/* Kick off the DMA action */
	pcie_br_writel(pp, &core->rdma_cntl, PCIE_DMA_CTL(4, be, dma_type, 0, 1));

	/* Poll the the interrupt status register till it is complete */
	do {
		istatus = pcie_br_readl(pp, &core->istatus);
		if (cnt++ == PCIE_DMA_MAX_LOOP) {
			PCIE_WARNING("\tread DMA incomplete\n");
			/* Stop the action */
			pcie_br_writel(pp, &core->rdma_cntl, PCIE_DMA_CTL(4, be, dma_type, 1, 0));
			break;
		}
		PCIE_TRACE("\tread DMA retry: istatus=0x%x, MSK=0x%x\n", istatus, PCIE_INTR_RD_DMA_MSK);
	} while (!(istatus & PCIE_INTR_RD_DMA_MSK));

	/*
	 * Any DMA action involving the DMA engine writing something to DDR needs such
	 * a synchronization operation.
	 */
	istatus &= (PCIE_INTR_RD_PCI_ERR | PCIE_INTR_RD_AHB_ERR);
	if (istatus) {
		PCIE_ASSERT((dma_type == PCIE_DMA_RD_CFG_REG), "read DMA error: istatus = %x\n", istatus);
		*val = 0;
		return istatus;
	}

	/*
	 * make sure that DMA is completed
	 */
	__pcie_ctl_dma_ready(pp->ctl);

	*val = pp->buf;
#ifdef PCIE_SWAP_ENDIAN
	*val = le32_to_cpu(*val);
#endif

	PCIE_TRACE("\tread DMA completed: REG[%x] => %x\n", addr_lo, *val);

	return istatus;
}

/*
 * Ask the internal DMA engine of the controller to make a 32 bit write (configure, mem, or IO)
 *
 *	core: 		the starting address of the controller registers
 *	addr_lo:	low 32 bit of a possible 64 address
 *	val:		the value to write
 *	dma_type:	configure space, memory, IO, value defined by the datasheet
 *	be:		byte enable		// FIXME: not working due to the endian issue in the first tape-out
 *	swap:		do endian swap		// FIXME: may not be unnecessary to have this flexibility
 */
static u32 __pcie_dma_writel(struct pcie_port *pp, u32 addr_lo, u32 val, u32 dma_type, u32 be)
{
	struct pcie_core_regs *core = pp->core;
	u32 istatus;
	int cnt = 0;

	PCIE_TRACE("write DMA: REG[%x] <= %x\n", addr_lo, val);
	PCIE_UBICOM_PROFILER_VCOUNT_INC(pcie_win0_write);

	/* Make sure the right value to be DMAed is in memory */
#ifdef PCIE_SWAP_ENDIAN
	val = cpu_to_le32(val);
#endif

	asm volatile (
	"	move.4		(%0), %1	\n\t"
	"	flush		(%0)		\n\t"
	"	pipe_flush	0		\n\t"
	"	sync		(%0)		\n\t"
	"	flush		(%0)		\n\t"
		:
		: "a" (&pp->buf), "r" (val)
	);

	/*
	 * Clear possible stale istatus bits. There might take effect immediately,
	 * but should be faster than the following writes.
	 */
	pcie_nbr_writel(&pp->ctl->ctl1, PCIE_BR_ACCESS_CORE_REG);
	PCIE_BR_SWITCH_DELAY;
	pcie_br_writel(pp, &core->istatus, PCIE_INTR_WR_DMA_MSK);

	/* Set up DMA descriptors */
	pcie_br_writel(pp, &core->wdma_pci_addr_0, addr_lo);
	pcie_br_writel(pp, &core->wdma_pci_addr_1, 0);
	pcie_br_writel(pp, &core->wdma_ahb_addr, (u32)(&pp->buf));

	/* Kick off the DMA action */
	pcie_br_writel(pp, &core->wdma_cntl, PCIE_DMA_CTL(4, be, dma_type, 0, 1));

	/* Poll the the interrupt status register till it is complete */
	do {
		istatus = pcie_br_readl(pp, &core->istatus);
		if (cnt++ == PCIE_DMA_MAX_LOOP) {
			PCIE_WARNING("\twrite DMA incomplete\n");
			/* Stop the action */
			pcie_br_writel(pp, &core->wdma_cntl, PCIE_DMA_CTL(4, be, dma_type, 1, 0));
			break;
		}
	} while (!(istatus & PCIE_INTR_WR_DMA_MSK));

	istatus &= (PCIE_INTR_WR_PCI_ERR | PCIE_INTR_WR_AHB_ERR);
	PCIE_ASSERT(!istatus, "\twrite DMA error: istatus=%x\n", istatus);
	PCIE_TRACE("\twrite DMA completed\n");

	return istatus;
}

#ifdef USE_PCIE_AHBPCI_WIN0
/*
 * Internal function to read a 32 bit value from a memory mapped space
 */
static u32 __pcie_win0_readl(struct pcie_port *pp, const volatile void __iomem *pci_addr, u32 *val)
{
	struct ubicom32_io_port *ctl = pp->ctl;
	u32 istatus;

	/* Where to read the BR register */
	u32 *br_addr = (u32 *)((u32)pp->core + ((u32)pci_addr & 4095));

	PCIE_TRACE("read WIN0: REG[%x]\n", (u32)pci_addr);
	PCIE_UBICOM_PROFILER_VCOUNT_INC(pcie_win0_read);

	/* Clear possible stale istatus bits. */
	pcie_nbr_writel(&ctl->ctl1, PCIE_BR_ACCESS_CORE_REG);
	PCIE_BR_SWITCH_DELAY;
	pcie_br_writel(pp, &pp->core->istatus, PCIE_INTR_WIN_ERR_MSK);

	/* Set high bits then read the value */
	pcie_nbr_writel(&ctl->ctl0, (u32)pci_addr);

	/* Select window0 */
	pcie_nbr_writel(&ctl->ctl1, PCIE_BR_ACCESS_AHBPCI_WIN0);
	PCIE_BR_SWITCH_DELAY;

	*val = pcie_br_readl(pp, br_addr);

	pcie_nbr_writel(&ctl->ctl1, PCIE_BR_ACCESS_CORE_REG);
	PCIE_BR_SWITCH_DELAY;
	istatus = pcie_br_readl(pp, &pp->core->istatus)
		& PCIE_INTR_WIN_ERR_MSK;
	if (istatus) {
		PCIE_ASSERT(0, "\tread WIN0 error: istatus = %x\n", istatus);
		*val = 0;
	}

	PCIE_TRACE("\tread WIN0 completed: REG[%x] => %x\n", (u32)pci_addr, *val);

	return istatus;
}

/*
 * Internal function to write a 32 bit value into a memory mapped space
 */
static u32 __pcie_win0_writel(struct pcie_port *pp, u32 val, const volatile void __iomem *pci_addr)
{
	struct ubicom32_io_port *ctl = pp->ctl;
	u32 istatus;

	/* Where to write the BR register */
	u32 *br_addr = (u32 *)((u32)pp->core + ((u32)pci_addr & 4095));

	PCIE_TRACE("write WIN0: REG[%x] <= %x\n", (u32)pci_addr, val);
	PCIE_UBICOM_PROFILER_VCOUNT_INC(pcie_win0_write);

	/* Clear possible stale istatus bits. */
	pcie_nbr_writel(&ctl->ctl1, PCIE_BR_ACCESS_CORE_REG);
	PCIE_BR_SWITCH_DELAY;
	pcie_br_writel(pp, &pp->core->istatus, PCIE_INTR_WIN_ERR_MSK);

	/* Set high bits then read the value */
	pcie_nbr_writel(&ctl->ctl0, (u32)pci_addr);

	/* Select window0 */
	pcie_nbr_writel(&ctl->ctl1, PCIE_BR_ACCESS_AHBPCI_WIN0);
	PCIE_BR_SWITCH_DELAY;

	pcie_br_writel(pp, br_addr, val);
	asm volatile ("cycles 128");

	pcie_nbr_writel(&ctl->ctl1, PCIE_BR_ACCESS_CORE_REG);
	PCIE_BR_SWITCH_DELAY;
	istatus = pcie_br_readl(pp, &pp->core->istatus)
		& PCIE_INTR_WIN_ERR_MSK;
	if (istatus) {
		PCIE_ASSERT(!(istatus & PCIE_INTR_WIN_FETCH), "\twrite WIN0 error: istatus = %x\n", istatus);
	}

	PCIE_TRACE("\twrite WIN0 completed\n");

	return istatus;
}
#endif
#else	/* CONFIG_UBICOM32_IP8K_SHUTTLE */

/*
 * Internal function to read a 32 bit value from a memory mapped space
 */
static u32 __pcie_win0_readl(struct pcie_port *pp, const volatile void __iomem *pci_addr, u32 *val)
{
	struct ubicom32_io_port *ctl = pp->ctl;
	u32 readback_val;

	/* Where to read the BR register */
	u32 *br_addr = (u32 *)((u32)pp->core + ((u32)pci_addr & 4095));

	PCIE_TRACE("read WIN0: REG[%x]\n", (u32)pci_addr);
	PCIE_UBICOM_PROFILER_VCOUNT_INC(pcie_win0_read);

	/* 
	 * Set high bits then read the value.
	 *
	 * WARNING: The following code is written to create a workaround for an issue
	 * seen at high frequency.  Sometimes when this register is written, the wrong
	 * value ends up in the register.  We will readback the value written and if
	 * it is incorrect, then we will write it again.  The NOPs are required to
	 * prevent the CPU bypass path from fooling us.  Without the NOPs, the CPU does
	 * not actually perform the device register read.
	 */
	do {
		asm volatile (
			"move.4	%[ctl0], %[pci_addr]	\n\t" 
			"nop				\n\t" 
			"nop				\n\t" 
			"nop				\n\t" 
			"nop				\n\t" 
			"nop				\n\t" 
			"move.4	%[rbv], %[ctl0]		\n\t" 
			: [rbv] "=d" (readback_val) 
			: [ctl0] "m" (ctl->ctl0), [pci_addr] "r" ((u32)pci_addr));
		if (readback_val != (u32)pci_addr) {
			PCIE_UBICOM_PROFILER_VCOUNT_INC(pcie_ctl0_rewrite);
			PCIE_TRACE("pcie ctl0 retry: expected %x, got %x\n", 
			       (u32)pci_addr, readback_val);
		}
	} while (readback_val != (u32)pci_addr);

	/* Select window0 */
	pcie_nbr_writel(&ctl->ctl1, PCIE_BR_ACCESS_PCIE_REG);
	PCIE_BR_SWITCH_DELAY;

	*val = pcie_br_readl(pp, br_addr);

	PCIE_TRACE("\tread WIN0 completed: REG[%x] => %x\n", (u32)pci_addr, *val);
	pp->wr_cnt = 0;

	return 0;
}

/*
 * Internal function to write a 32 bit value into a memory mapped space
 */
static u32 __pcie_win0_writel(struct pcie_port *pp, u32 val, const volatile void __iomem *pci_addr)
{
	struct ubicom32_io_port *ctl = pp->ctl;
	u32 readback_val;

	/* Where to read the BR register */
	u32 *br_addr = (u32 *)((u32)pp->core + ((u32)pci_addr & 4095));

	PCIE_TRACE("write WIN0: REG[%x] <= %x\n", (u32)pci_addr, val);
	PCIE_UBICOM_PROFILER_VCOUNT_INC(pcie_win0_write);

	/*
	 * When doing consecutive write, the HW may enter deadlock.
	 * Perform a read in case of too many write being done consecutively.
	 */
	if (((s32)(UBICOM32_IO_TIMER->mptval - pp->wr_timestamp) < 0) && (pp->wr_cnt > 0)) {
		u32 istatus;

		pcie_nbr_writel(&ctl->ctl1, PCIE_BR_ACCESS_CORE_REG);
		PCIE_BR_SWITCH_DELAY;

		pcie_br_writel(pp, &pp->core->istatus, PCIE_INTR_CFG_END | PCIE_INTR_CFG_ERROR);
		pcie_br_writel(pp, &pp->core->cfg_fbe, 0xF);
		pcie_br_writel(pp, &pp->core->cfg_control, PCIE_REG_RD | 0x01000000);
		do {
			istatus = pcie_br_readl(pp, &pp->core->istatus);
		} while (!(istatus & (PCIE_INTR_CFG_END | PCIE_INTR_CFG_ERROR)));
		PCIE_ASSERT(!(istatus & PCIE_INTR_CFG_ERROR), "write WIN0: read back error: istatus = %x\n", istatus);
		PCIE_UBICOM_PROFILER_VCOUNT_INC(pcie_win0_readback);
		pp->wr_cnt = 0;
	}

	/* 
	 * Set high bits then read the value.
	 *
	 * WARNING: The following code is written to create a workaround for an issue
	 * seen at high frequency.  Sometimes when this register is written, the wrong
	 * value ends up in the register.  We will readback the value written and if
	 * it is incorrect, then we will write it again.  The NOPs are required to
	 * prevent the CPU bypass path from fooling us.  Without the NOPs, the CPU does
	 * not actually perform the device register read.
	 */
	do {
		asm volatile (
			"move.4	%[ctl0], %[pci_addr]	\n\t" 
			"nop				\n\t" 
			"nop				\n\t" 
			"nop				\n\t" 
			"nop				\n\t" 
			"nop				\n\t" 
			"move.4	%[rbv], %[ctl0]		\n\t" 
			: [rbv] "=d" (readback_val) 
			: [ctl0] "m" (ctl->ctl0), [pci_addr] "r" ((u32)pci_addr));
		if (readback_val != (u32)pci_addr) {
			PCIE_UBICOM_PROFILER_VCOUNT_INC(pcie_ctl0_rewrite);
			PCIE_TRACE("pcie ctl0 retry: expected %x, got %x\n", 
			       (u32)pci_addr, readback_val);
		}
	} while (readback_val != (u32)pci_addr);

	/* Select window0 */
	pcie_nbr_writel(&ctl->ctl1, PCIE_BR_ACCESS_PCIE_REG);
	PCIE_BR_SWITCH_DELAY;

	pcie_br_writel(pp, br_addr, val);
	asm volatile ("cycles 33");

	PCIE_TRACE("\twrite WIN0 completed\n");
	pp->wr_cnt++;
	pp->wr_timestamp = UBICOM32_IO_TIMER->mptval + 3;

	return 0;
}
#endif	/* CONFIG_UBICOM32_IP8K_SHUTTLE */

/*
 * Public function to read a 32 bit value from a PCI memory address
 */
unsigned int ubi32_pci_read_u32(const volatile void __iomem *addr)
{
	unsigned int val;
	struct pcie_port *pp = addr_to_port(addr);

	spin_lock_irqsave(&pp->conf_lock, pp->flags);
#if defined(USE_PCIE_AHBPCI_WIN0) || !defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	__pcie_win0_readl(pp, addr, &val);
#else
	__pcie_dma_readl(pp, (u32)addr, &val, PCIE_DMA_RD_MEM_DW, 0xf);
#endif
	spin_unlock_irqrestore(&pp->conf_lock, pp->flags);
	PCIE_TRACE(" -- read32 for addr %p, val=%x\n", addr, val);
	return val;
}

/*
 * Public function to read a 16 bit value from a PCI memory address
 */
unsigned short ubi32_pci_read_u16(const volatile void __iomem *addr)
{
	unsigned int val, shift;
	struct pcie_port *pp = addr_to_port(addr);

	/* Bit shift count, then find the 32bit aligned addr */
	shift = ADDR_SHIFT(addr);

	spin_lock_irqsave(&pp->conf_lock, pp->flags);
#if defined(USE_PCIE_AHBPCI_WIN0) || !defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	__pcie_win0_readl(pp, ADDR_ALIGN(addr), &val);
#else
	__pcie_dma_readl(pp, (u32)ADDR_ALIGN(addr), &val, PCIE_DMA_RD_MEM_DW, 0xf);
#endif
	spin_unlock_irqrestore(&pp->conf_lock, pp->flags);

	val = (val >> shift) & 0xffff;
	PCIE_TRACE(" -- read16 for addr %p, val=%x\n", addr, val);
	return val;
}

/*
 * Public function to read a 8 bit value from a PCI memory address
 */
unsigned char ubi32_pci_read_u8	(const volatile void __iomem *addr)
{
	unsigned int val, shift;
	struct pcie_port *pp = addr_to_port(addr);

	/* Bit shift count, then find the 32bit aligned addr */
	shift = ADDR_SHIFT(addr);

	spin_lock_irqsave(&pp->conf_lock, pp->flags);
#if defined(USE_PCIE_AHBPCI_WIN0) || !defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	__pcie_win0_readl(pp, ADDR_ALIGN(addr), &val);
#else
	__pcie_dma_readl(pp, (u32)ADDR_ALIGN(addr), &val, PCIE_DMA_RD_MEM_DW, 0xf);
#endif
	spin_unlock_irqrestore(&pp->conf_lock, pp->flags);

	val = (val >> shift) & 0xff;
	PCIE_TRACE(" -- read8 for addr %p, val=%x\n", addr, val);
	return val;
}

/*
 * Public function to write a 32 bit value to a PCI memory address
 */
void ubi32_pci_write_u32(unsigned int value, const volatile void __iomem *addr)
{
	struct pcie_port *pp = addr_to_port(addr);

	PCIE_TRACE(" -- write32 for addr %p, val=%x\n", addr, value);
	spin_lock_irqsave(&pp->conf_lock, pp->flags);
#if defined(USE_PCIE_AHBPCI_WIN0) || !defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	__pcie_win0_writel(pp, value, addr);
#else
	__pcie_dma_writel(pp, (u32)addr, value, PCIE_DMA_WR_MEM_DW, 0xf);
#endif
	spin_unlock_irqrestore(&pp->conf_lock, pp->flags);
}

/*
 * Public function to write a 16 bit value to a PCI memory address
 */
void ubi32_pci_write_u16(unsigned short value, const volatile void __iomem *addr)
{
	unsigned int shift;
	struct pcie_port *pp = addr_to_port(addr);
	unsigned int val;

	PCIE_TRACE(" -- write16 for addr %p, val=%x\n", addr, value);
	shift = ADDR_SHIFT(addr);

	spin_lock_irqsave(&pp->conf_lock, pp->flags);
#if defined(USE_PCIE_AHBPCI_WIN0) || !defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	__pcie_win0_readl(pp, ADDR_ALIGN(addr), &val);
	val &= ~(0xffff << shift);
	val |= value << shift;
	__pcie_win0_writel(pp, val, ADDR_ALIGN(addr));
#else
#ifdef USE_PCIE_BE
	val = value << shift;
	__pcie_dma_writel(pp, (u32)ADDR_ALIGN(addr), val, PCIE_DMA_WR_MEM_DW,
			(((u32)addr & 0x02) ? (0x3 << 2) : 0x3));
#else
	__pcie_dma_readl(pp, (u32)ADDR_ALIGN(addr), &val, PCIE_DMA_RD_MEM_DW, 0xf);
	val &= ~(0xffff << shift);
	val |= value << shift;
	__pcie_dma_writel(pp, (u32)ADDR_ALIGN(addr), val, PCIE_DMA_WR_MEM_DW, 0xf);
#endif
#endif
	spin_unlock_irqrestore(&pp->conf_lock, pp->flags);
}

/*
 * Public function to write a 8 bit value to a PCI memory address
 */
void ubi32_pci_write_u8(unsigned char value, const volatile void __iomem *addr)
{
	unsigned int shift;
	struct pcie_port *pp = addr_to_port(addr);
	unsigned int val;

	PCIE_TRACE(" -- write8 for addr %p, val=%x\n", addr, value);
	shift = ADDR_SHIFT(addr);

	spin_lock_irqsave(&pp->conf_lock, pp->flags);
#if defined(USE_PCIE_AHBPCI_WIN0) || !defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	__pcie_win0_readl(pp, ADDR_ALIGN(addr), &val);
	val &= ~(0xff << shift);
	val |= value << shift;
	__pcie_win0_writel(pp, val, ADDR_ALIGN(addr));
#else
#ifdef USE_PCIE_BE
	val = value << shift;
	__pcie_dma_writel(pp, (u32)ADDR_ALIGN(addr), val, PCIE_DMA_WR_MEM_DW,
			(1 << ((u32)addr & 0x03)));
#else
	__pcie_dma_readl(pp, (u32)ADDR_ALIGN(addr), &val, PCIE_DMA_RD_MEM_DW, 0xf);
	val &= ~(0xff << shift);
	val |= value << shift;
	__pcie_dma_writel(pp, (u32)ADDR_ALIGN(addr), val, PCIE_DMA_WR_MEM_DW, 0xf);
#endif
#endif
	spin_unlock_irqrestore(&pp->conf_lock, pp->flags);
}

EXPORT_SYMBOL(ubi32_pci_write_u32);
EXPORT_SYMBOL(ubi32_pci_write_u16);
EXPORT_SYMBOL(ubi32_pci_write_u8);

EXPORT_SYMBOL(ubi32_pci_read_u32);
EXPORT_SYMBOL(ubi32_pci_read_u16);
EXPORT_SYMBOL(ubi32_pci_read_u8);

/*
 * Public function to write a 8 bit value to a PCI memory address
 */
static inline u32 __pcie_port_inl(struct pcie_port *pp, const volatile void __iomem *port_addr, u32 *val)
{
#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	__pcie_dma_readl(pp, (u32)port_addr, val, PCIE_DMA_RD_IO, 0xf);
#else
	u32 istatus;
	struct ubicom32_io_port *ctl = pp->ctl;
	struct pcie_core_regs *core = pp->core;

	PCIE_TRACE("read IO: REG[%x]\n", (u32)port_addr);
	pcie_nbr_writel(&ctl->ctl1, PCIE_BR_ACCESS_CORE_REG);
	PCIE_BR_SWITCH_DELAY;

	pcie_br_writel(pp, &core->istatus, PCIE_INTR_IO_END | PCIE_INTR_IO_ERROR);
	pcie_br_writel(pp, &core->io_addr, (u32)port_addr);
	pcie_br_writel(pp, &core->io_control, PCIE_REG_RD | (0xf << 4));
	do {
		istatus = pcie_br_readl(pp, &core->istatus);
	} while (!(istatus & (PCIE_INTR_IO_END | PCIE_INTR_IO_ERROR)));

	istatus &= PCIE_INTR_IO_ERROR;
	PCIE_ASSERT(!istatus, "read IO error: istatus = %x\n", istatus);
	if (istatus) {
		*val = 0;
		return istatus;
	}

	*val = pcie_br_readl(pp, &core->io_rd_data);
#ifdef PCIE_SWAP_ENDIAN
	*val = le32_to_cpu(*val);
#endif

	PCIE_TRACE("\tread IO completed: REG[%x] => %x\n", (u32)port_addr, *val);
	return istatus;
#endif
	return *val;
}

static inline void __pcie_port_outl(struct pcie_port *pp, u32 val, const volatile void __iomem *port_addr, u32 be)
{
#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	__pcie_dma_writel(pp, (u32)port_addr, val, PCIE_DMA_WR_IO, be);
#else
	u32 istatus;
	struct ubicom32_io_port *ctl = pp->ctl;
	struct pcie_core_regs *core = pp->core;

	PCIE_TRACE("write IO: REG[%x:%x] <= %x\n", (u32)port_addr, be, val);
#ifdef PCIE_SWAP_ENDIAN
	val = cpu_to_le32(val);
#endif
	pcie_nbr_writel(&ctl->ctl1, PCIE_BR_ACCESS_CORE_REG);
	PCIE_BR_SWITCH_DELAY;

	pcie_br_writel(pp, &core->istatus, PCIE_INTR_IO_END | PCIE_INTR_IO_ERROR);
	pcie_br_writel(pp, &core->io_addr, (u32)port_addr);
	pcie_br_writel(pp, &core->io_wr_data, val);		// Configuration data
	pcie_br_writel(pp, &core->io_control, PCIE_REG_WR | ((be & 0xf) << 4));
	do {
		istatus = pcie_br_readl(pp, &core->istatus);
	} while (!(istatus & (PCIE_INTR_IO_END | PCIE_INTR_IO_ERROR)));

	istatus &= PCIE_INTR_IO_ERROR;
	PCIE_ASSERT(!istatus, "\twrite IO error: istatus = %x\n", istatus);
	PCIE_TRACE("\twrite IO completed\n");
#endif
}

/*
 * Public function to read a 8 bit value from a PCI I/O address
 */
u8 ubi32_pci_inb(unsigned long addr)
{
	unsigned int shift;
	struct pcie_port *pp = ioport_to_port(addr);
	unsigned int val;

	shift = ADDR_SHIFT(addr);

	spin_lock_irqsave(&pp->conf_lock, pp->flags);
	__pcie_port_inl(pp, ADDR_ALIGN(addr), &val);
	spin_unlock_irqrestore(&pp->conf_lock, pp->flags);

	val = (val >> shift) & 0xff;
	PCIE_INFO("inb for port %d, val=%x\n", (unsigned int)addr, val);

	return val;
}

/*
 * Public function to write a 8 bit value to a PCI I/O address
 */
void ubi32_pci_outb(u8 value, unsigned long addr)
{
	unsigned int shift;
	struct pcie_port *pp = ioport_to_port(addr);
	unsigned int val;

	shift = ADDR_SHIFT(addr);

	spin_lock_irqsave(&pp->conf_lock, pp->flags);
#ifdef USE_PCIE_BE
	val = value << shift;
	__pcie_port_outl(pp, val, ADDR_ALIGN(addr), (1 << ((u32)addr & 0x03)));
#else
	__pcie_port_inl(pp, ADDR_ALIGN(addr), &val);
	val &= ~(0xff << shift);
	val |= value << shift;
	__pcie_port_outl(pp, val, ADDR_ALIGN(addr), 0xf);
#endif
	spin_unlock_irqrestore(&pp->conf_lock, pp->flags);
}

/*
 * Public function to read a 16 bit value from a PCI I/O address
 */
u16 ubi32_pci_inw(unsigned long addr)
{
	unsigned int shift;
	struct pcie_port *pp = ioport_to_port(addr);
	unsigned int val;

	shift = ADDR_SHIFT(addr);

	spin_lock_irqsave(&pp->conf_lock, pp->flags);
	__pcie_port_inl(pp, ADDR_ALIGN(addr), &val);
	spin_unlock_irqrestore(&pp->conf_lock, pp->flags);

	val = (val >> shift) & 0xffff;
	PCIE_INFO("inb for port %d, val=%x\n", (unsigned int)addr, val);

	return val;
}

/*
 * Public function to write a 16 bit value to a PCI I/O address
 */
void ubi32_pci_outw(u16 value, unsigned long addr)
{
	unsigned int shift;
	struct pcie_port *pp = ioport_to_port(addr);
	unsigned int val;

	shift = ADDR_SHIFT(addr);

	spin_lock_irqsave(&pp->conf_lock, pp->flags);
#ifdef USE_PCIE_BE
	val = value << shift;
	__pcie_port_outl(pp, val, ADDR_ALIGN(addr), (((u32)addr & 0x02) ? (0x3 << 2) : 0x3));
#else
	__pcie_port_inl(pp, ADDR_ALIGN(addr), &val);
	val &= ~(0xffff << shift);
	val |= value << shift;
	__pcie_port_outl(pp, val, ADDR_ALIGN(addr), 0xf);
#endif
	spin_unlock_irqrestore(&pp->conf_lock, pp->flags);
}

/*
 * Public function to read a 32 bit value from a PCI I/O address
 */
u32 ubi32_pci_inl(unsigned long addr)
{
	struct pcie_port *pp = ioport_to_port(addr);
	u32 val;

	spin_lock_irqsave(&pp->conf_lock, pp->flags);
	__pcie_port_inl(pp, (void __iomem *)addr, &val);
	spin_unlock_irqrestore(&pp->conf_lock, pp->flags);
	return val;
}

/*
 * Public function to write a 32 bit value to a PCI I/O address
 */
void ubi32_pci_outl(u32 value, unsigned long addr)
{
	struct pcie_port *pp = ioport_to_port(addr);

	spin_lock_irqsave(&pp->conf_lock, pp->flags);
	__pcie_port_outl(pp, value, (void __iomem *)addr, 0xf);
	spin_unlock_irqrestore(&pp->conf_lock, pp->flags);
}

EXPORT_SYMBOL(ubi32_pci_outl);
EXPORT_SYMBOL(ubi32_pci_outw);
EXPORT_SYMBOL(ubi32_pci_outb);

EXPORT_SYMBOL(ubi32_pci_inl);
EXPORT_SYMBOL(ubi32_pci_inw);
EXPORT_SYMBOL(ubi32_pci_inb);

/*
 * Write a 32bit value into a PCI device's config space register
 * Assumes spinlock of this port is held already by the caller.
 */
u32 __pcie_cfg_writel(struct pcie_port *pp, u32 cfg_addr, u32 val)
{
#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	return __pcie_dma_writel(pp, cfg_addr, val, PCIE_DMA_WR_CFG_REG, 0xf);
#else
	u32 istatus;
	struct ubicom32_io_port *ctl = pp->ctl;
	struct pcie_core_regs *core = pp->core;

	PCIE_TRACE("write DMA: REG[%x] <= %x\n", cfg_addr, val);
#ifdef PCIE_SWAP_ENDIAN
	val = cpu_to_le32(val);
#endif
	pcie_nbr_writel(&ctl->ctl1, PCIE_BR_ACCESS_CORE_REG);
	PCIE_BR_SWITCH_DELAY;

	pcie_br_writel(pp, &core->istatus, PCIE_INTR_CFG_END | PCIE_INTR_CFG_ERROR);
	pcie_br_writel(pp, &core->cfg_wr_data, val);	// Configuration data
	pcie_br_writel(pp, &core->cfg_fbe, 0xf);	// DW Alligned
	pcie_br_writel(pp, &core->cfg_control, PCIE_REG_WR | cfg_addr);
	do {
		istatus = pcie_br_readl(pp, &core->istatus);
	} while (!(istatus & (PCIE_INTR_CFG_END | PCIE_INTR_CFG_ERROR)));

	istatus &= PCIE_INTR_CFG_ERROR;
	PCIE_ASSERT(!istatus, "\twrite CFG error: istatus = %x\n", istatus);
	PCIE_TRACE("\twrite DMA completed\n");

	return istatus;
#endif
}

/*
 * Read a DW from a PCI config space register
 * Assumes spinlock of this port is held already by the caller.
 */
u32 __pcie_cfg_readl(struct pcie_port *pp, u32 cfg_addr, u32 *val)
{
#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	return __pcie_dma_readl(pp, cfg_addr, val, PCIE_DMA_RD_CFG_REG, 0xf);
#else
	u32 istatus;
	struct ubicom32_io_port *ctl = pp->ctl;
	struct pcie_core_regs *core = pp->core;

	PCIE_TRACE("read DMA: REG[%x]\n", cfg_addr);
	pcie_nbr_writel(&ctl->ctl1, PCIE_BR_ACCESS_CORE_REG);
	PCIE_BR_SWITCH_DELAY;

	pcie_br_writel(pp, &core->istatus, PCIE_INTR_CFG_END | PCIE_INTR_CFG_ERROR);
	pcie_br_writel(pp, &core->cfg_fbe, 0xf);	// DW Alligned
	pcie_br_writel(pp, &core->cfg_control, PCIE_REG_RD | cfg_addr);
	do {
		istatus = pcie_br_readl(pp, &core->istatus);
	} while (!(istatus & (PCIE_INTR_CFG_END | PCIE_INTR_CFG_ERROR)));

	istatus &= PCIE_INTR_CFG_ERROR;
	PCIE_ASSERT(!istatus, "read DMA error: istatus = %x\n", istatus);
	if (istatus) {
		*val = 0;
		return istatus;
	}

	*val = pcie_br_readl(pp, &core->cfg_rd_data);
#ifdef PCIE_SWAP_ENDIAN
	*val = le32_to_cpu(*val);
#endif

	PCIE_TRACE("\tread DMA completed: REG[%x] => %x\n", cfg_addr, *val);
	return istatus;
#endif
}

/*
 * APIs to make sure DMA device is not updating DDR while host is accessing it.
 * User must be careful enough not issuing any PCI bus access (config space and MMIO space)
 * while holding the dma lock, otherwise there will be deadlock.
 */

/*
 * Lock the DMA engine from making more write transactions to the platform memory
 */
void ubi32_pcie_dma_lock(void __iomem *addr)
{
	struct pcie_port *pp = addr_to_port(addr);
	struct ubicom32_io_port *ctl = pp->ctl;

	spin_lock_irqsave(&pp->conf_lock, pp->flags);
#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	ctl->ctl3 |= WSTALL_BIT;
#else
	ctl->ctl2 |= WSTALL_BIT;
#endif
	while (!(ctl->status0 & WCMDQ_EMPTY_BIT));
}

/*
 * Unlock the DMA engine to allow more write transactions to the platform memory
 */
void ubi32_pcie_dma_unlock(void __iomem *addr)
{
	struct pcie_port *pp = addr_to_port(addr);
	struct ubicom32_io_port *ctl = pp->ctl;

#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
	ctl->ctl3 &= ~WSTALL_BIT;
#else
	ctl->ctl2 &= ~WSTALL_BIT;
#endif
	spin_unlock_irqrestore(&pp->conf_lock, pp->flags);
}

EXPORT_SYMBOL(ubi32_pcie_dma_lock);
EXPORT_SYMBOL(ubi32_pcie_dma_unlock);

