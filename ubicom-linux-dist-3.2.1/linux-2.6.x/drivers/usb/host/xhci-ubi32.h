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

#ifndef __XHCI_UBI32_H__
#define __XHCI_UBI32_H__

#include <asm/ubicom32.h>

#define UBI32_XHCI_VBUS_RESET_PIN	(IO_PORT_PG4 | 24)

#define BYTE_SWAP_DEFINED

#ifndef BYTE_SWAP_DEFINED

#define le16_to_host(n) (n)
#define host_to_le16(n) (n)
#define le32_to_host(n) (n)
#define host_to_le32(n) (n)
#define le64_to_host(n) (n)
#define host_to_le64(n) (n)

#else

#define le16_to_host(n) (le16_to_cpu(n))
#define host_to_le16(n) (cpu_to_le16(n))
#define le32_to_host(n) (le32_to_cpu(n))
#define host_to_le32(n) (cpu_to_le32(n))
#define le64_to_host(n) (le64_to_cpu(n))
#define host_to_le64(n) (cpu_to_le64(n))

#endif

extern void ubi32_flush(void *buf, unsigned int size);
extern void ubi32_invalidate(void *buf, unsigned int size);
extern u32 xhci_br_readl(volatile void __iomem *addr, volatile void __iomem *base);
extern void xhci_br_writel(u32 val, volatile void __iomem *addr, volatile void __iomem *base);
extern void ubi32_xhci_int_ack(volatile void __iomem *base);
extern void ubi32_xhci_clear_int(volatile void __iomem *base);
extern void ubi32_xhci_dma_ready(volatile u32 *base);

#ifdef CONFIG_UBI32_SS_USB2_MODE
extern int ubi32_xhci_port_init(void);
extern void ubi32_xhci_port_shutdown(void);
extern void ubi32_xhci_pdev_unregister(void);
#endif

#endif
