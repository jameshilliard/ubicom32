/*
 * Micron SPI-ER NAND Flash Memory U-Boot driver
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

#include <common.h>
#include <linux/mtd/compat.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand-spi-er.h>

#include <asm/ip5000.h>
#include <asm/UbicomSDK.h>

struct mtd_info nand_spi_er_mtd;
static const char *dev_name = "spier0";

void nand_spi_er_init(void)
{
	memset(&nand_spi_er_mtd, 0, sizeof(struct mtd_info));

	int res = nand_spi_er_probe(&nand_spi_er_mtd);
	if (res < 0) {
		printf("No SPI-ER NAND detected\n");
		return;
	}

	puts(dev_name);	//nand_spi_er_mtd.name);
	puts(": ");
	print_size(nand_spi_er_mtd.size, "\n");

#ifdef CONFIG_MTD_PARTITIONS
	nand_spi_er_mtd.name = dev_name;
	add_mtd_device(&nand_spi_er_mtd);
#endif
}

