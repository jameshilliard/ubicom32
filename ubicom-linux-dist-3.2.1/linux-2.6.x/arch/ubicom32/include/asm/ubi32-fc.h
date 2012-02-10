/*
 * arch/ubicom32/include/asm/ubi32-fc.h
 *   Ubicom32 architecture FC Flash driver.
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
#ifndef __UBICOM32FC_H__
#define __UBICOM32FC_H__

#ifdef CONFIG_MTD_PARTITIONS
#include <linux/mtd/partitions.h>
#endif

typedef void (*ubicom32fc_select_fn)(void *appdata);

/**
 * struct ubicom32fc_platform_data: board-specific flash data
 * @name: optional flash device name (eg, as used with mtdparts=)
 * @parts: optional array of mtd_partitions for static partitioning
 * @nr_parts: number of mtd_partitions for static partitoning
 * @type: optional flash device type (e.g. m25p80 vs m25p64), for use
 *	with chips that can't be queried for JEDEC or other IDs
 *
 * Board init code (in arch/.../mach-xxx/board-yyy.c files) can
 * provide information about SPI flash parts (such as DataFlash) to
 * help set up the device and its appropriate default partitioning.
 *
 * Note that for DataFlash, sizes for pages, blocks, and sectors are
 * rarely powers of two; and partitions should be sector-aligned.
 */
struct ubicom32fc_platform_data {
	/*
	 * Optional, Name to give the chip
	 */
	char			*name;

	/*
	 * Optional, Type of the chip, eg M25P80
	 */
	char			*type;

	/*
	 * Optional, function to call to select/unselect the NOR flash chip
	 * Must take into account multiple selects, i.e. select, select,
	 * unselect = still selected.
	 */
	ubicom32fc_select_fn	select;
	ubicom32fc_select_fn	unselect;
	void			*appdata;

#ifdef CONFIG_MTD_PARTITIONS
	/*
	 * Optional, partition info
	 */
	unsigned int		nr_parts;
	struct mtd_partition	*parts;
#endif
};

#endif
