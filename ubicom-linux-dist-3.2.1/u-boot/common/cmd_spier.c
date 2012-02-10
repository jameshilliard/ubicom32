/*
 *  U-Boot command for SPI-ER NAND support
 *
 *  Copyright (C) 2010 Ubicom, Inc.
 *  Patrick Tjin <pat.tjin@ubicom.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <common.h>
#include <command.h>
#include <malloc.h>

#include <linux/mtd/compat.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand-spi-er.h>
#include <nand-spi-er-uboot.h>

#include <asm/io.h>

static struct mtd_info *mtd;

static loff_t next_ofs;
static loff_t skip_ofs;

static inline int str2long(char *p, ulong *num)
{
	char *endptr;

	*num = simple_strtoul(p, &endptr, 16);
	return (*p != '\0' && *endptr == '\0') ? 1 : 0;
}

static int arg_off_size(int argc, char *argv[], ulong *off, size_t *size)
{
	if (argc >= 1) {
		if (!(str2long(argv[0], off))) {
			printf("'%s' is not a number\n", argv[0]);
			return -1;
		}
	} else {
		*off = 0;
	}

	if (argc >= 2) {
		if (!(str2long(argv[1], (ulong *)size))) {
			printf("'%s' is not a number\n", argv[1]);
			return -1;
		}
	} else {
		*size = mtd->size - *off;
	}

	if ((*off + *size) > mtd->size) {
		printf("total chip size (0x%x) exceeded!\n", mtd->size);
		return -1;
	}

	if (*size == mtd->size)
		puts("whole chip\n");
	else
		printf("offset 0x%lx, size 0x%x\n", *off, *size);

	return 0;
}

static int spier_block_read(loff_t from, size_t len, size_t *retlen, u_char *buf)
{
	struct nand_spi_er *this = mtd->priv;
	int blocks;
	int blocksize = this->device->erase_size;
	loff_t ofs = from;
	int ret;

	if (!len) {
		return 0;
	}

	blocks = (int)len / blocksize;
	if ((blocks == 0) || (len % blocksize)) {
		blocks++;
	}

	while (blocks) {
		size_t _retlen = 0;

		ret = mtd->block_isbad(mtd, ofs);
		if (ret) {
			printk("Skip: bad block at 0x%x\n", (u32)ofs);
			ofs += blocksize;
			continue;
		}

		ret = mtd->read(mtd, ofs, blocksize, &_retlen, buf);
		if (ret) {
			printk("Skip: read failed 0x%x, err=%d\n", (u32)ofs, ret);
			ofs += blocksize;
			continue;
		}
		ofs += blocksize;
		buf += blocksize;
		blocks--;
		*retlen += _retlen;
	}

	return 0;
}

static int spier_block_write(loff_t to, size_t len,
			       size_t *retlen, const u_char * buf)
{
	struct nand_spi_er *this = mtd->priv;
	int blocks;
	int blocksize = this->device->write_size;
	loff_t ofs;
	int ret;

	blocks = (int)len / blocksize;
	if ((blocks == 0) || (len % blocksize)) {
		blocks++;
	}

	if (to == next_ofs) {
		next_ofs = to + len;
		to += skip_ofs;
	} else {
		next_ofs = to + len;
		skip_ofs = 0;
	}
	ofs = to;

	while (blocks) {
		size_t _retlen = 0;

		ret = mtd->block_isbad(mtd, ofs);
		if (ret) {
			printk("Skip: bad block at 0x%x\n", (u32)ofs);
			skip_ofs += blocksize;
			goto next;
		}

		ret = mtd->write(mtd, ofs, blocksize, &_retlen, buf);
		if (ret) {
			printk("Skip: write failed 0x%x, err=%d", (u32)ofs, ret);
			skip_ofs += blocksize;
			goto next;
		}

		buf += blocksize;
		blocks--;
		*retlen += _retlen;
next:
		ofs += blocksize;
	}

	return 0;
}

static int spier_block_erase(u32 start, u32 size, int force)
{
	struct nand_spi_er *this = mtd->priv;
	struct erase_info instr = {
		.callback	= NULL,
	};
	loff_t ofs;
	int ret;
	int blocksize = this->device->erase_size;

	for (ofs = start; ofs < (start + size); ofs += blocksize) {
		ret = mtd->block_isbad(mtd, ofs);
		if (ret && !force) {
			printf("Skip: erase bad block at 0x%x\n", (u32)ofs);
			continue;
		}

		instr.addr = ofs;
		instr.len = blocksize;
		instr.priv = force;
		instr.mtd = mtd;
		ret = mtd->erase(mtd, &instr);
		if (ret) {
			printf("Failed to erase block at 0x%x\n", (u32)ofs);
			continue;
		}
	}

	return 0;
}

static int spier_block_test(u32 start, u32 size)
{
	struct nand_spi_er *this = mtd->priv;
	struct erase_info instr = {
		.callback	= NULL,
		.priv		= 0,
	};

	int blocks;
	loff_t ofs;
	int blocksize = this->device->erase_size;
	int start_block, end_block;
	u_char *buf;
	u_char *verify_buf;
	int ret;

	buf = malloc(blocksize);
	if (!buf) {
		printf("Not enough malloc space available!\n");
		return -1;
	}

	verify_buf = malloc(blocksize);
	if (!verify_buf) {
		printf("Not enough malloc space available!\n");
		return -1;
	}

	start_block = start / blocksize;
	end_block = (start + size) / blocksize;

	if (end_block > (mtd->size / blocksize)) {
		end_block = mtd->size / blocksize;
	}

	blocks = start_block;
	ofs = start;
	while (blocks < end_block) {
		size_t retlen = 0;

		printf("\rTesting block at 0x%x", (u32)ofs);

		ret = mtd->block_isbad(mtd, ofs);
		if (ret) {
			printf("Skip: not erasing bad block at 0x%x\n", (u32)ofs);
			goto next;
		}

		instr.addr = ofs;
		instr.len = blocksize;
		ret = mtd->erase(mtd, &instr);
		if (ret) {
			printk("Erase failed 0x%x, err=%d\n", (u32)ofs, ret);
			goto next;
		}

		ret = mtd->write(mtd, ofs, blocksize, &retlen, buf);
		if (ret) {
			printk("Write failed 0x%x, err=%d\n", (u32)ofs, ret);
			goto next;
		}

		ret = mtd->read(mtd, ofs, blocksize, &retlen, verify_buf);
		if (ret) {
			printk("Read failed 0x%x, err=%d\n", (u32)ofs, ret);
			goto next;
		}

		if (memcmp(buf, verify_buf, blocksize))
			printk("\nCompare failed 0x%x\n", (u32)ofs);

next:
		ofs += blocksize;
		blocks++;
	}
	printf("...Done\n");

	free(buf);
	free(verify_buf);

	return 0;
}

static int spier_dump(struct mtd_info *mtd, ulong off, int width)
{
	int i;
	u_char *datbuf, *p;
	loff_t addr;
	size_t retlen = 0;
	int n = 0;

	datbuf = malloc(mtd->writesize);
	if (!datbuf) {
		puts("No memory for page buffer\n");
		return 1;
	}
	off &= ~(mtd->writesize - 1);
	addr = (loff_t) off;

	i = mtd->read(mtd, addr, mtd->writesize, &retlen, datbuf);
	if (i < 0) {
		printf("Error (%d) reading page %08lx\n", i, off);
		free(datbuf);
		return 1;
	}
	printf("Page %08lx:\n", off / mtd->writesize);
	p = datbuf;

	if (width) {
		width = 32;
		i = mtd->writesize >> 5;
	} else {
		width = 16;
		i = mtd->writesize >> 4;
	}
	while (i--) {
		int j;
		printf("\t%08x: ", (unsigned int)(addr + n));
		for (j = 0; j < width; j++) {
			if ((j % 8) == 0) {
				printf(" ");
			}
			printf("%02x ", p[j]);
		}

		printf("|");
		for (j = 0; j < width; j++) {
			if ((j % 8) == 0) {
				printf(" ");
			}
			if ((p[j] > 0x20) && (p[j] < 0x7E)) {
				printf("%c", p[j]);
			} else {
				printf(".");
			}
		}
		printf("|\n");
		p += width;
		n += width;
	}
	free(datbuf);

	return 0;
}

int do_spier(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	struct nand_spi_er *this;
	ulong addr, ofs;
	size_t len, retlen = 0;
	int ret;
	char *cmd;

	mtd = &nand_spi_er_mtd;
	this = mtd->priv;
	if (!this) {
		printf("No SPI-ER device detected\n");
		return 1;
	}

	cmd = argv[1];

	switch (argc) {
	case 0:
	case 1:
		goto usage;

	case 2:
		if (strcmp(cmd, "info") == 0) {
			printf("%s\n", mtd->name);
			printf(" Chip type:   %s\n", this->name);
			printf(" # blocks:    %u\n", this->device->blocks);
			printf(" pages/block: %u\n", this->device->pages_per_block);
			printf(" page size:   %u\n", this->device->page_size);
			printf(" write size:  %u\n", this->device->write_size);
			printf(" erase size:  %u\n", this->device->erase_size);

			return 0;
		}

		if (strcmp(cmd, "bad") == 0) {
			int j;

			printf("%s: Bad Block Table:", mtd->name);
			for (j = 0; j < this->device->blocks; j++) {
				if ((j % 64) == 0) {
					printf("\n%s: block %03x: ", this->name, j);
				}
				printf("%c", test_bit(j, this->bbt) ? 'X' : '.');
			}
			printf("\n%s: Bad Block Numbers: ", mtd->name);
			for (j = 0; j < this->device->blocks; j++) {
				if (test_bit(j, this->bbt)) {
					printf("%x ", j);
				}
			}
			printf("\n");
			return 0;
		}

	default:
		/* At least 4 args */

		/*
		 * Syntax is:
		 *   0       1     2       3    4
		 *   spier erase [force] [off size]
		 */
		if ((strcmp(cmd, "erase") == 0) || (strcmp(cmd, "test") == 0)) {
			int force = argc > 2 && !strcmp("force", argv[2]);
			int o = force ? 3 : 2;
			int erase;

			erase = strcmp(cmd, "erase") == 0; /* 1 = erase, 0 = test */
			printf("\nSPI-ER NAND %s: ", erase ? "erase" : "test");

			/* skip first two or three arguments, look for offset and size */
			if (arg_off_size(argc - o, argv + o, &ofs, &len) != 0)
				return 1;

			if (erase)
				ret = spier_block_erase(ofs, len, force);
			else
				ret = spier_block_test(ofs, len);

			printf("%s\n", ret ? "ERROR" : "OK");

			return ret == 0 ? 0 : 1;
		}

		if (strncmp(cmd, "read", 4) == 0 || strncmp(cmd, "write", 5) == 0) {
			int read;

			if (argc < 4)
				goto usage;

			addr = (ulong)simple_strtoul(argv[2], NULL, 16);

			read = strncmp(cmd, "read", 4) == 0; /* 1 = read, 0 = write */
			printf("\nSPI-ER NAND %s: ", read ? "read" : "write");
			if (arg_off_size(argc - 3, argv + 3, &ofs, &len) != 0)
				return 1;

			if (read) {
				ret = spier_block_read(ofs, len, &retlen, (u8 *)addr);
			} else {
				ret = spier_block_write(ofs, len, &retlen, (u8 *)addr);
			}

			printf(" %d bytes %s: %s\n", retlen,
			       read ? "read" : "written", ret ? "ERROR" : "OK");

			return ret == 0 ? 0 : 1;
		}

		if (strcmp(cmd, "markbad") == 0) {
			addr = (ulong)simple_strtoul(argv[2], NULL, 16);

			int ret = mtd->block_markbad(mtd, addr);
			if (ret == 0) {
				printf("block 0x%08lx successfully marked as bad\n",
						(ulong) addr);
				return 0;
			} else {
				printf("block 0x%08lx NOT marked as bad! ERROR %d\n",
						(ulong) addr, ret);
			}
			return 1;
		}

		if (strncmp(cmd, "dump", 4) == 0) {
			if (argc < 3)
				goto usage;

			ofs = (int)simple_strtoul(argv[2], NULL, 16);

			ret = spier_dump(mtd, ofs, (argc > 3));

			return ret == 0 ? 1 : 0;
		}

		break;
	}

	return 0;

usage:
	cmd_usage(cmdtp);
	return 1;
}

U_BOOT_CMD(
	spier,	6,	1,	do_spier,
	"SPI-ER NAND sub-system",
	"info - show available SPI-ER NAND devices\n"
	"spier bad - show bad blocks\n"
	"spier read addr off size\n"
	"spier write addr off size\n"
	"    read/write 'size' bytes starting at offset 'off'\n"
	"    to/from memory address 'addr', skipping bad blocks.\n"
	"spier erase [force] [off size] - erase 'size' bytes from\n"
	"spier test [off size] - test 'size' bytes from\n"
	"    offset 'off' (entire device if not specified)\n"
	"spier dump off [wide]- dump page, 'wide' for 32 byte output\n"
	"spier markbad off - mark bad block at offset (UNSAFE)\n"
);
