/*
 * arch/ubicom32/include/asm/ubi32-cs4525.h
 *   Ubicom32 architecture CS4525 driver platform data definitions.
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
 */
#ifndef _ASM_UBICOM32_UBI32_CS4525_H
#define _ASM_UBICOM32_UBI32_CS4525_H

struct ubi32_cs4525_platform_data {
	/*
	 * Pin used to enable high voltage power supply to DAC
	 */
	int	power_enable_pin;

	/*
	 * Set to 0 for active low enable of power supply,
	 * 1 for active high enable of power supply.
	 */
	int	power_enable_polarity;

	int	reset_pin;
};

#endif /* _ASM_UBICOM32_UBI32_CS4525_H */

