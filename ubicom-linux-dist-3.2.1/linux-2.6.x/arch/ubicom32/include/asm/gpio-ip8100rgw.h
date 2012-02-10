/*
 * arch/ubicom32/include/asm/gpio-ip8100rgw.h
 *   Definitions for GPIO operations on Ubicom32 architecture.
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
#ifndef _ASM_UBICOM32_GPIO_IP8100RGW_H
#define _ASM_UBICOM32_GPIO_IP8100RGW_H

#if defined(CONFIG_IP8100RGW_ES) || defined(CONFIG_IP8100RGW)
/*
 * U16
 */
#define IP8100RGW_U16_BASE	(32 * 10)
#define IP8100RGW_IO0		(IP8100RGW_U16_BASE + 0)	// D18
#define IP8100RGW_IO1		(IP8100RGW_U16_BASE + 1)	// D19
#define IP8100RGW_IO2		(IP8100RGW_U16_BASE + 2)	// D20
#define IP8100RGW_IO3		(IP8100RGW_U16_BASE + 3)	// D21
#define IP8100RGW_IO4		(IP8100RGW_U16_BASE + 4)	// SW4
#define IP8100RGW_IO5		(IP8100RGW_U16_BASE + 5)	// SW5
#define IP8100RGW_IO6		(IP8100RGW_U16_BASE + 6)	// SW6
#define IP8100RGW_IO7		(IP8100RGW_U16_BASE + 7)	// SW7
#endif

#endif /* _ASM_UBICOM32_GPIO_IP8100RGW_H */

