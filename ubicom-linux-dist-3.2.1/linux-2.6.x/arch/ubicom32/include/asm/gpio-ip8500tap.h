/*
 * arch/ubicom32/include/asm/gpio-ip8500tap.h
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
#ifndef _ASM_UBICOM32_GPIO_IP8500TAP_H
# define _ASM_UBICOM32_GPIO_IP8500TAP_H

# if defined(CONFIG_IP8500TAP) || defined(CONFIG_IP8500TAP_ES) || defined(CONFIG_IP8500TAP_DAO)

/*
 * U38
 */
#  define IP8500TAP_U38_BASE	(32 * 10)
#  define IP8500TAP_IO1_0	(IP8500TAP_U38_BASE + 0)	// U6 S1
#  define IP8500TAP_IO1_1	(IP8500TAP_U38_BASE + 1)	// U6 S2
#  define IP8500TAP_IO1_2	(IP8500TAP_U38_BASE + 2)	// U8 G4P25 -> 0=PLIO1_CTL3, 1=ADC_SDOUT
#  define IP8500TAP_IO1_3	(IP8500TAP_U38_BASE + 3)	// U5 G3P25 -> 0=PLIO2_CTL3, 1=VideoADC_RESET
#  define IP8500TAP_IO1_4	(IP8500TAP_U38_BASE + 4)	// MUTE_CTL_IO
#  define IP8500TAP_IO1_5	(IP8500TAP_U38_BASE + 5)	// 0 = VideoADC on PLIO2, 1=PLIO2 on PPI2 (J7)

/*
 * U35
 */
#  define IP8500TAP_U35_BASE	(32 * 11)
#  define IP8500TAP_IO2_0	(IP8500TAP_U35_BASE + 0)	// D15
#  define IP8500TAP_IO2_1	(IP8500TAP_U35_BASE + 1)	// D16
#  define IP8500TAP_IO2_2	(IP8500TAP_U35_BASE + 2)	// D27
#  define IP8500TAP_IO2_3	(IP8500TAP_U35_BASE + 3)	// D28
#  define IP8500TAP_IO2_4	(IP8500TAP_U35_BASE + 4)	// PCIE-RESET
#  define IP8500TAP_IO2_5	(IP8500TAP_U35_BASE + 5)	// SW4
#  define IP8500TAP_IO2_6	(IP8500TAP_U35_BASE + 6)	// SW5
#  define IP8500TAP_IO2_7	(IP8500TAP_U35_BASE + 7)	// SW6

# endif /* defined(CONFIG_IP8500TAP) || defined(CONFIG_IP8500TAP_ES) || defined(CONFIG_IP8500TAP_I2S) */

#endif /* _ASM_UBICOM32_GPIO_IP8500TAP_H */

