/*
 * TI's Davinci platform specific USB wrapper functions.
 *
 * Copyright (c) 2008 Texas Instruments
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Author: Thomas Abraham t-abraham@ti.com, Texas Instruments
 */

#ifndef __IP7K_USB_H__
#define __IP7K_USB_H__

#include <asm/ip5000.h>
#include "musb_core.h"

/* Base address of IP7K usb0 wrapper */
#define IP7K_USB0_BASE RJ

/* Base address of IP7K musb core */
#define MENTOR_USB0_BASE (IP7K_USB0_BASE+0x800)

#define IP7K_USB_TX_ENDPTS_MASK	0x1f /* ep0 + 4 tx */
#define IP7K_USB_RX_ENDPTS_MASK	0x1e /* 4 rx */
#define IP7K_USB_USBINT_SHIFT	16
#define IP7K_USB_TXINT_SHIFT 	0
#define IP7K_USB_RXINT_SHIFT 	8
#define IP7K_INTR_DRVVBUS		0x0100

#define IP7K_USB_USBINT_MASK 	0x01ff0000	/* 8 Mentor, DRVVBUS */
#define IP7K_USB_TXINT_MASK \
		(IP7K_USB_TX_ENDPTS_MASK << IP7K_USB_TXINT_SHIFT)
#define IP7K_USB_RXINT_MASK \
		(IP7K_USB_RX_ENDPTS_MASK << IP7K_USB_RXINT_SHIFT)
#define MGC_BUSCTL_OFFSET(_bEnd, _bOffset) \
		(0x80 + (8*(_bEnd)) + (_bOffset))

/* Integrated highspeed/otg PHY */
#define USBPHY_CTL_CFG	(GENERAL_CFG_BASE + GEN_USB_PHY_CFG)
#define USBPHY_CTL_CFG2	(GENERAL_CFG_BASE + GEN_USB_PHY_TEST)
#define USBPHY_PHYCLKGD 	(1 << 8)
#define USBPHY_SESNDEN		(1 << 7)	/* v(sess_end) comparator */
#define USBPHY_VBDTCTEN 	(1 << 6)	/* v(bus) comparator */
#define USBPHY_PHYPLLON 	(1 << 4)	/* override pll suspend */
#define USBPHY_CLKO1SEL 	(1 << 3)
#define USBPHY_OSCPDWN		(1 << 2)
#define USBPHY_PHYPDWN		(1 << 0)

/* Timeout for ip7k USB module */
#define IP7K_USB_TIMEOUT 0x7FFFFF

#endif	/* __IP7K_USB_H__ */
