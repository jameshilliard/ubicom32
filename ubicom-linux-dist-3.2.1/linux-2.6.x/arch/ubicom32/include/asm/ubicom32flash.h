/*
 * arch/ubicom32/include/asm/ubicom32flash.h
 *   Ubicom32 architecture GPIO Flash driver
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
#ifndef _ASM_UBICOM32_UBICOM32FLASH_H
#define _ASM_UBICOM32_UBICOM32FLASH_H

struct ubicom32flash_platform_data {
	/*
	 * Optional mapping for oe, we, and ce
	 */
	int		has_control;
	int		oe_pin;
	int		we_pin;
	int		ce_pin;

	/*
	 * Optional timing parameters
	 */
	unsigned int	tdh;
	unsigned int	tds;
	unsigned int	twp;
	unsigned int	twc;
	unsigned int	tacc;

	/*
	 * Optional number of address bits
	 */
	int		n_addr_bits;

	/*
	 * Optional probe name, i.e. "cfi_probe", "jedec_probe", etc.
	 * Defaults to "cfi_probe".
	 */
	const char	*probe_name;
};

#endif /* _ASM_UBICOM32_UBICOM32FLASH_H */
