/*
 * arch/ubicom32/include/asm/gpio-ip7500media.h
 *   Definitions for GPIO operations on Ubicom32 architecture.
 *
 * (C) Copyright 2009-2010, Ubicom, Inc.
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
#ifndef _ASM_UBICOM32_GPIO_IP7500MEDIA_H
#define _ASM_UBICOM32_GPIO_IP7500MEDIA_H

/*
 * The following section defines extra GPIO available to some boards.
 * These GPIO are generally external to the processor (i.e. SPI/I2C
 * expander chips).
 *
 * Note that these defines show all possible GPIO available, however,
 * depending on the actual board configuration, some GPIO are not
 * available for use.
 */
#ifdef CONFIG_IP7500MEDIA
/*
 * U15
 */
#define IP7500MEDIA_U15_BASE	(32 * 10)
#define IP7500MEDIA_IO0		(IP7500MEDIA_U15_BASE + 0)
#define IP7500MEDIA_IO1		(IP7500MEDIA_U15_BASE + 1)
#define IP7500MEDIA_IO2		(IP7500MEDIA_U15_BASE + 2)
#define IP7500MEDIA_IO3		(IP7500MEDIA_U15_BASE + 3)
#define IP7500MEDIA_IO4		(IP7500MEDIA_U15_BASE + 4)
#define IP7500MEDIA_IO5		(IP7500MEDIA_U15_BASE + 5)
#define IP7500MEDIA_IO6		(IP7500MEDIA_U15_BASE + 6)
#define IP7500MEDIA_IO7		(IP7500MEDIA_U15_BASE + 7)

/*
 * U16
 */
#define IP7500MEDIA_U16_BASE	(32 * 11)
#define IP7500MEDIA_IO8		(IP7500MEDIA_U16_BASE + 0)
#define IP7500MEDIA_IO9		(IP7500MEDIA_U16_BASE + 1)
#define IP7500MEDIA_IO10	(IP7500MEDIA_U16_BASE + 2)
#define IP7500MEDIA_IO11	(IP7500MEDIA_U16_BASE + 3)
#define IP7500MEDIA_IO12	(IP7500MEDIA_U16_BASE + 4)
#define IP7500MEDIA_IO13	(IP7500MEDIA_U16_BASE + 5)
#define IP7500MEDIA_IO14	(IP7500MEDIA_U16_BASE + 6)
#define IP7500MEDIA_IO15	(IP7500MEDIA_U16_BASE + 7)

/*
 * U17
 */
#define IP7500MEDIA_U17_BASE	(32 * 12)
#define IP7500MEDIA_IO16	(IP7500MEDIA_U17_BASE + 0)
#define IP7500MEDIA_IO17	(IP7500MEDIA_U17_BASE + 1)
#define IP7500MEDIA_IO18	(IP7500MEDIA_U17_BASE + 2)
#define IP7500MEDIA_IO19	(IP7500MEDIA_U17_BASE + 3)
#define IP7500MEDIA_IO20	(IP7500MEDIA_U17_BASE + 4)
#define IP7500MEDIA_IO21	(IP7500MEDIA_U17_BASE + 5)
#define IP7500MEDIA_IO22	(IP7500MEDIA_U17_BASE + 6)
#define IP7500MEDIA_IO23	(IP7500MEDIA_U17_BASE + 7)

/*
 * U18
 */
#define IP7500MEDIA_U18_BASE	(32 * 13)
#define IP7500MEDIA_IO24	(IP7500MEDIA_U18_BASE + 0)
#define IP7500MEDIA_IO25	(IP7500MEDIA_U18_BASE + 1)
#define IP7500MEDIA_IO26	(IP7500MEDIA_U18_BASE + 2)
#define IP7500MEDIA_IO27	(IP7500MEDIA_U18_BASE + 3)
#define IP7500MEDIA_IO28	(IP7500MEDIA_U18_BASE + 4)
#define IP7500MEDIA_IO29	(IP7500MEDIA_U18_BASE + 5)
#define IP7500MEDIA_IO30	(IP7500MEDIA_U18_BASE + 6)
#define IP7500MEDIA_IO31	(IP7500MEDIA_U18_BASE + 7)
#endif
#endif /* _ASM_UBICOM32_GPIO_IP7500MEDIA_H */
