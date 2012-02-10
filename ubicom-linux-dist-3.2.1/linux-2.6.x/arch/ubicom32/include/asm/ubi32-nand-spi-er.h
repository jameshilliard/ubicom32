/*
 * arch/ubicom32/include/asm/ubi32-nand-spi-er.h
 *   Ubicom32 architecture NAND SPI-ER Flash driver.
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
#ifndef __UBI32_NAND_SPI_ER_H__
#define __UBI32_NAND_SPI_ER_H__

typedef void (*ubi32_nand_spi_er_select_fn)(void *appdata);

struct ubi32_nand_spi_er_platform_data {
	/*
	 * Optional, function to call to select/unselect the NAND flash chip.
	 * Must take into account multiple selects, i.e. select, select,
	 * unselect = still selected.
	 */
	ubi32_nand_spi_er_select_fn	select;
	ubi32_nand_spi_er_select_fn	unselect;
	void				*appdata;

	/*
	 * Optional, name of the device
	 */
	const char			*name;

#ifdef CONFIG_MTD_PARTITIONS
	/*
	 * Optional, partition info
	 */
	unsigned int		nr_parts;
	struct mtd_partition	*parts;
#endif
};

#endif
