/*
 * arch/ubicom32/mach-common/usb_xhci.c
 *   Ubicom32 architecture usb support for xHCI.
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
 */

#ifndef __UBICOM32_XHCI_H__
#define __UBICOM32_XHCI_H__

#include <linux/kernel.h>
#include <asm/ubicom32.h>
#include <asm/ubicom32-io.h>
#include <asm/delay.h>

//#define XHCI_DEBUG

#ifdef XHCI_DEBUG
#define XHCI_TRACE(fmt, ...)			printk(KERN_DEBUG fmt, ##__VA_ARGS__)
#define XHCI_INFO(fmt, ...)			printk(KERN_INFO fmt, ##__VA_ARGS__)
#define XHCI_WARNING(fmt, ...)			printk(KERN_WARNING fmt, ##__VA_ARGS__)

#else // XHCI_DEBUG

#define XHCI_TRACE(fmt, ...)
#define XHCI_INFO(fmt, ...)
#define XHCI_WARNING(fmt, ...)

#endif // XHCI_DEBUG

/* Up to two XHCI instances */
#define XHCI_PORT_MAX			2

/*
 * PHY base addresses
 */
#define XHCI_PHY0_BASE			IO_PORT_RX
#define XHCI_PHY1_BASE			IO_PORT_RM
#define XHCI_PHY_FUNC_BASE(inst)	((inst) ? XHCI_PHY1_BASE : XHCI_PHY0_BASE)
#define XHCI_PHY_BR_BASE(inst)		(XHCI_PHY_FUNC_BASE(inst) + IO_PORT_BR_OFFSET)

/*
 * CTL FUNC base addresses
 */
#define XHCI_CTL0_BASE			IO_PORT_RW
#define XHCI_CTL1_BASE			IO_PORT_RK
#define XHCI_CTL_FUNC_BASE(inst)	((inst) ? XHCI_CTL1_BASE : XHCI_CTL0_BASE)
#define XHCI_CTL_BR_BASE(inst)		(XHCI_CTL_FUNC_BASE(inst) + IO_PORT_BR_OFFSET)

/*
 * Func reset bit of ctl
 */
#define XHCI_FN_RESET_BIT		(1 << 6)

/*
 * ctl0 register bitmap of ctl
 */
#define XHCI_BR_OFFSET_MASK		(0xFFFF)
#define XHCI_BR_OFFSET(val)        	(val & XHCI_BR_OFFSET_MASK)

/*
 * ctl1 register bitmap of ctl
 */
#define XHCI_SRST			(1 << 30)
#define XHCI_CFG_DW_CAP			(1 << 26)
#define XHCI_CFG_SIM_MODE_ON		(1 << 22)
#define USB3_CFG_NO_SS_INACTIVE		(1 << 18)
#define XHCI_CFG_U1_ENABLE		(1 << 15)
#define XHCI_CFG_U2_ENABLE		(1 << 14)

/* bit of ack pci interrupt in function set of ctl */
#define XHCI_PCI_INTX_ACK		(0x1)

/*
 * Function sel bit of phy
 */
#define XHCI_PHY_FN_SEL			1

/* bit of deassert in status 0 of phy */
#define XHCI_PHY_STATUS0_MASK		(0x2)

/* bit of wcmdq_empty in status 0 of ctl */
#define WCMDQ_EMPTY_BIT			(1 << 31)

/* bit of wstall in ctl1 of ctl */
#define WSTALL_BIT			(1 << 31)


/*
 * Bitmap definition for imask and istatus
 */
#define XHCI_INTR_INTX			(1 << 0)
#define XHCI_INTR_SMI			(1 << 1)
#define XHCI_INTR_SYS_ERR		(1 << 2)
#define XHCI_INTR_CTL_MASK		(XHCI_INTR_INTX | XHCI_INTR_SMI | XHCI_INTR_SYS_ERR)

/* bit of BIGENDIAN in ctl6 register */
#define XHCI_CTL6_DMA_AHB_BIGENDIAN	(1 << 27)
#define XHCI_CTL6_MMIO_AHB_BIGENDIAN	(1 << 28)

/*
 * Abstraction of a XHCI port
 *	port		0/1, instance number
 *
 *	phy		addr of NBR registers of the phy
 *	ctl		addr of NBR registers of the controller
 *	core		addr of BR registers of the controller
 */
struct xhci_port {
	u8                      port;

	struct ubicom32_io_port	*phy;
	struct ubicom32_io_port	*ctl;
	volatile u32 *core;
};

/*
 * xhci_nbr_readl
 *	Read a 32 bit value from the non-blocking-region
 */
static inline u32 xhci_nbr_readl(volatile u32 * addr)
{
	volatile u32 val = *addr;
 
	return val;
}

/*
 * xhci_nbr_writel
 *	Write a 32 bit value into non-blocking-region
 */
static inline void xhci_nbr_writel(volatile u32 * addr, u32 val)
{
	*addr = val;
}

extern struct xhci_port xhci_ports[XHCI_PORT_MAX];

extern void __init xhci_phy_config(struct ubicom32_io_port *phy);

#endif
