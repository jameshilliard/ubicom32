/*
 * Micron SPI-ER NAND Flash Memory
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
*/

#ifndef __LINUX_MTD_NAND_SPI_ER__
#define __LINUX_MTD_NAND_SPI_ER__

#if defined(CONFIG_CMD_NAND_SPI_ER_UBI32)
extern unsigned int nand_spi_er_switch_port;
extern unsigned int nand_spi_er_switch_pin;
#endif

#define NAND_SPI_ER_BLOCK_FROM_ROW(row)		(row >> 6)

#define NAND_SPI_ER_STATUS_P_FAIL		(1 << 3)
#define NAND_SPI_ER_STATUS_E_FAIL		(1 << 2)
#define NAND_SPI_ER_STATUS_OIP			(1 << 0)

#define NAND_SPI_ER_LAST_ROW_INVALID		0xFFFFFFFF
#define	NAND_SPI_ER_BAD_BLOCK_MARK_OFFSET	0x08

struct nand_spi_er_device {
	const char		*name;

	uint16_t		id;

	unsigned int		blocks;
	unsigned int		pages_per_block;
	unsigned int		page_size;
	unsigned int		write_size;
	unsigned int		erase_size;
};

struct nand_spi_er {
	char				name[24];

	const struct nand_spi_er_device	*device;

	//struct mutex			lock;

	unsigned int			last_row;	/* the last row we fetched */

	/*
	 * Bad block table (MUST be last in strcuture)
	 */
	unsigned long			nbb;
	unsigned long			bbt[0];
};

extern int nand_spi_er_probe(struct mtd_info *mtd);
extern int nand_spi_er_remove(struct mtd_info *mtd);
#endif
