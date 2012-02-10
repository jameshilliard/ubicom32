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
#ifndef _ASM_UBICOM32_GPIO_IP7145DPF_H
#define _ASM_UBICOM32_GPIO_IP7145DPF_H

/*
 * The following section defines extra GPIO available to some boards.
 * These GPIO are generally external to the processor (i.e. SPI/I2C
 * expander chips).
 *
 * Note that these defines show all possible GPIO available, however,
 * depending on the actual board configuration, some GPIO are not
 * available for use.
 */
#ifdef CONFIG_IP7145DPF
/*
 * U48
 */
#define IP7145DPF_U48_BASE	(32 * 10)
#define IP7145DPF_IO0		(IP7145DPF_U48_BASE + 0)
#define IP7145DPF_IO1		(IP7145DPF_U48_BASE + 1)
#define IP7145DPF_IO2		(IP7145DPF_U48_BASE + 2)
#define IP7145DPF_IO3		(IP7145DPF_U48_BASE + 3)
#define IP7145DPF_IO4		(IP7145DPF_U48_BASE + 4)
#define IP7145DPF_IO5		(IP7145DPF_U48_BASE + 5)
#define IP7145DPF_IO6		(IP7145DPF_U48_BASE + 6)
#define IP7145DPF_IO7		(IP7145DPF_U48_BASE + 7)

/*
 * U72
 */
#define IP7145DPF_U72_BASE	(32 * 11)
#define IP7145DPF_IOB0		(IP7145DPF_U72_BASE + 0)
#define IP7145DPF_IOB1		(IP7145DPF_U72_BASE + 1)
#define IP7145DPF_IOB2		(IP7145DPF_U72_BASE + 2)
#define IP7145DPF_IOB3		(IP7145DPF_U72_BASE + 3)
#define IP7145DPF_IOB4		(IP7145DPF_U72_BASE + 4)
#define IP7145DPF_IOB5		(IP7145DPF_U72_BASE + 5)
#define IP7145DPF_IOB6		(IP7145DPF_U72_BASE + 6)
#define IP7145DPF_IOB7		(IP7145DPF_U72_BASE + 7)
#endif

#endif /* _ASM_UBICOM32_GPIO_IP7500MEDIA_H */
