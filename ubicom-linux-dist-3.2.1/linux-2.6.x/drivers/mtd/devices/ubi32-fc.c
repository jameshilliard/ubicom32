/*
 * drivers/mtd/devices/ubi32-fc.c
 *   Ubicom32 architecture FC Flash driver.
 *
 * Some parts based on m25p80.c by Mike Lavender
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
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#ifdef CONFIG_MTD_PARTITIONS
#include <linux/mtd/partitions.h>
#endif
#include <asm/ubi32-fc.h>
#include <linux/mtd/physmap.h>

#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/mutex.h>

#include <asm/ubicom32.h>

#define DRIVER_NAME "ubicom32fc"

/*
 * UBICOM32FC_AUTO_LOAD:
 *	Define to allow this driver to "auto load", however, this
 *	type of behavior is discouraged.
 */
//#undef UBICOM32FC_AUTO_LOAD

#define FLASH_PAGESIZE		256

/* Flash opcodes. */
#define	OPCODE_WREN		0x06	/* Write enable */
#define	OPCODE_RDSR		0x05	/* Read status register */
#define	OPCODE_READ		0x03	/* Read data bytes (low frequency) */
#define	OPCODE_FAST_READ	0x0b	/* Read data bytes (high frequency) */
#define	OPCODE_PP		0x02	/* Page program (up to 256 bytes) */
#define	OPCODE_BE_4K		0x20	/* Erase 4KiB block */
#define	OPCODE_BE_32K		0x52	/* Erase 32KiB block */
#define	OPCODE_SE		0xd8	/* Sector erase (usually 64KiB) */
#define	OPCODE_RDID		0x9f	/* Read JEDEC ID */

/* Status Register bits. */
#define	SR_WIP			1	/* Write in progress */
#define	SR_WEL			2	/* Write enable latch */
/* meaning of other SR_* bits may differ between vendors */
#define	SR_BP0			4	/* Block protect 0 */
#define	SR_BP1			8	/* Block protect 1 */
#define	SR_BP2			0x10	/* Block protect 2 */
#define	SR_SRWD			0x80	/* SR write protect */

/* Define max times to check status register before we give up. */
#define	MAX_READY_WAIT_COUNT	100000

/*
 * Ubicom32 FLASH Command Set
 */
#define FLASH_FC_INST_CMD	0x00	/* for pdev command only transaction */
#define FLASH_FC_INST_WR	0x01	/* for pdev write transaction */
#define FLASH_FC_INST_RD	0x02	/* for pdev read transaction */

#define ALIGN_DOWN(v, a) ((v) & ~((a) - 1))
#define ALIGN_UP(v, a) (((v) + ((a) - 1)) & ~((a) - 1))

#define	FLASH_COMMAND_KICK_OFF(io)							\
	asm volatile(									\
	"	bset	"D(IO_INT_CLR)"(%0), #0, #%%bit("D(IO_XFL_INT_DONE)")	\n\t"	\
	"	jmpt.t	.+4							\n\t"	\
	"	bset	"D(IO_INT_SET)"(%0), #0, #%%bit("D(IO_XFL_INT_START)")	\n\t"	\
		:									\
		: "a"(io)								\
		: "cc"							\
	);

#define	FLASH_COMMAND_WAIT_FOR_COMPLETION(io)						\
	asm volatile(									\
	"	btst	"D(IO_INT_STATUS)"(%0), #%%bit("D(IO_XFL_INT_DONE)")	\n\t"	\
	"	jmpeq.f	.-4							\n\t"	\
		:									\
		: "a"(io)								\
		: "cc"									\
	);

#define	FLASH_COMMAND_EXEC(io)								\
	FLASH_COMMAND_KICK_OFF(io)							\
	FLASH_COMMAND_WAIT_FOR_COMPLETION(io)

#define TEN_MICRO_SECONDS (GEN_CLK_MPT_FREQ * 10 / 1000000)

/*
 * We will have to eventually replace this null definition with the real thing.
 */
#define WATCHDOG_RESET()

#define EXTFLASH_WRITE_FIFO_SIZE 32
#define EXTFLASH_WRITE_BLOCK_SIZE EXTFLASH_WRITE_FIFO_SIZE /* limit the size to
							    * FIFO capacity, so
							    * the thread can be
							    * suspended. */

/****************************************************************************/
struct ubicom32fc_flash_type {
	char		*name;

	/* JEDEC id zero means "no ID" (most older chips); otherwise it has
	 * a high byte of zero plus three data bytes: the manufacturer id,
	 * then a two byte device id.
	 */
	u32		jedec_id;

	/* The size listed here is what works with OPCODE_SE, which isn't
	 * necessarily called a "sector" by the vendor.
	 */
	unsigned	sector_size;
	u16		n_sectors;

	u16		flags;
#define	SECT_4K		0x01		/* OPCODE_BE_4K works uniformly */
};

/*
 * NOTE: double check command sets and memory organization when you add
 * more flash chips.  This current list focusses on newer chips, which
 * have been converging on command sets which including JEDEC ID.
 */
static struct ubicom32fc_flash_type __devinitdata ubicom32fc_flash_types[] = {
	/*
	 * Atmel -- some are (confusingly) marketed as "DataFlash"
	 */
	{ "at25fs010",  0x1f6601, 32 * 1024, 4, SECT_4K, },
	{ "at25fs040",  0x1f6604, 64 * 1024, 8, SECT_4K, },

	{ "at25df041a", 0x1f4401, 64 * 1024, 8, SECT_4K, },

	{ "at26f004",   0x1f0400, 64 * 1024, 8, SECT_4K, },
	{ "at26df081a", 0x1f4501, 64 * 1024, 16, SECT_4K, },
	{ "at26df161a", 0x1f4601, 64 * 1024, 32, SECT_4K, },
	{ "at26df321",  0x1f4701, 64 * 1024, 64, SECT_4K, },

	/*
	 * Spansion -- single (large) sector size only, at least
	 * for the chips listed here (without boot sectors).
	 */
	{ "s25sl004a", 0x010212, 64 * 1024, 8, },
	{ "s25sl008a", 0x010213, 64 * 1024, 16, },
	{ "s25sl016a", 0x010214, 64 * 1024, 32, },
	{ "s25sl032a", 0x010215, 64 * 1024, 64, },
	{ "s25sl064a", 0x010216, 64 * 1024, 128, },

	/*
	 * SST -- large erase sizes are "overlays", "sectors" are 4K
	 */
	{ "sst25vf040b", 0xbf258d, 64 * 1024, 8, SECT_4K, },
	{ "sst25vf080b", 0xbf258e, 64 * 1024, 16, SECT_4K, },
	{ "sst25vf016b", 0xbf2541, 64 * 1024, 32, SECT_4K, },
	{ "sst25vf032b", 0xbf254a, 64 * 1024, 64, SECT_4K, },

	/*
	 * ST Microelectronics -- newer production may have feature updates
	 */
	{ "m25p05",  0x202010,  32 * 1024, 2, },
	{ "m25p10",  0x202011,  32 * 1024, 4, },
	{ "m25p20",  0x202012,  64 * 1024, 4, },
	{ "m25p40",  0x202013,  64 * 1024, 8, },
	{ "m25p80",         0,  64 * 1024, 16, },
	{ "m25p16",  0x202015,  64 * 1024, 32, },
	{ "m25p32",  0x202016,  64 * 1024, 64, },
	{ "m25p64",  0x202017,  64 * 1024, 128, },
	{ "m25p128", 0x202018, 256 * 1024, 64, },

	{ "m45pe80", 0x204014,  64 * 1024, 16, },
	{ "m45pe16", 0x204015,  64 * 1024, 32, },

	{ "m25pe80", 0x208014,  64 * 1024, 16, },
	{ "m25pe16", 0x208015,  64 * 1024, 32, SECT_4K, },

	/*
	 * Winbond -- w25x "blocks" are 64K, "sectors" are 4KiB
	 */
	{ "w25x10", 0xef3011, 64 * 1024, 2, SECT_4K, },
	{ "w25x20", 0xef3012, 64 * 1024, 4, SECT_4K, },
	{ "w25x40", 0xef3013, 64 * 1024, 8, SECT_4K, },
	{ "w25x80", 0xef3014, 64 * 1024, 16, SECT_4K, },
	{ "w25x16", 0xef3015, 64 * 1024, 32, SECT_4K, },
	{ "w25x32", 0xef3016, 64 * 1024, 64, SECT_4K, },
	{ "w25x64", 0xef3017, 64 * 1024, 128, SECT_4K, },

	/*
	 * Macronix -- mx25lxxx
	 */
	{ "mx25l32",  0xc22016, 64 * 1024,  64, },
	{ "mx25l64",  0xc22017, 64 * 1024, 128, },
	{ "mx25l128", 0xc22018, 64 * 1024, 256, },

};

/****************************************************************************/

struct ubicom32fc_info {
	struct mutex		lock;
	struct mtd_info		mtd;
	u8			erase_opcode;
	char			dev_name[32];
	//u8			command[4];

	ubicom32fc_select_fn	select;
	ubicom32fc_select_fn	unselect;
	void			*appdata;
};

/****************************************************************************/

/*
 * ubicom32fc_read_status
 *	Read the status register, returning its value in the location
 */
static int ubicom32fc_read_status(struct ubicom32fc_info *flash)
{
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)IO_PORT_RA;

	io->ctl1 &= ~IO_XFL_CTL1_MASK;
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_RD) | IO_XFL_CTL1_FC_DATA(1);
	io->ctl2 = IO_XFL_CTL2_FC_CMD(OPCODE_RDSR);
	FLASH_COMMAND_EXEC(io);

	return io->status1 & 0xff;
}

/*
 * ubicom32fc_read_u32
 */
static u32 ubicom32fc_read_u32(u32 addr)
{
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)IO_PORT_RA;
	io->ctl1 &= ~IO_XFL_CTL1_MASK;
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_RD) | IO_XFL_CTL1_FC_DATA(4) |
		    IO_XFL_CTL1_FC_DUMMY(1) | IO_XFL_CTL1_FC_ADDR;
	io->ctl2 = IO_XFL_CTL2_FC_CMD(OPCODE_FAST_READ) | IO_XFL_CTL2_FC_ADDR(addr);
	FLASH_COMMAND_EXEC(io);
	return io->status1;
}

/*
 * ubicom32fc_read_u8
 */
static u8 ubicom32fc_read_u8(u32 addr)
{
	u32 tmp_addr = ALIGN_DOWN(addr, 4);
	u32 tmp_data = ubicom32fc_read_u32(tmp_addr);
	u8 *ptr = (u8 *)&tmp_data;
	return ptr[addr & 0x3];
}

/*
 * ubicom32fc_read()
 */
static void ubicom32fc_read(u32 addr, void *dst, size_t length)
{
	/*
	 * Range check
	 */
	/*
	 * Fix source alignment.
	 */
	while (addr & 0x03) {
		if (length == 0) {
			return;
		}
		*((u8 *)dst) = ubicom32fc_read_u8(addr++);
		dst++;
		length--;
	}

	while (length >= 4) {
		u32 tmp_data = ubicom32fc_read_u32(addr);
		addr += 4;
		length -= 4;

		/*
		 * Send the data to the destination.
		 */
		memcpy((void *)dst, (void *)&tmp_data, 4);
		dst += 4;
	}

	while (length--) {
		*((u8 *)dst) = ubicom32fc_read_u8(addr++);
		dst++;
	}
}

/*
 * ubicom32fc_wait_until_complete()
 */
static void ubicom32fc_wait_until_complete(void)
{
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)IO_PORT_RA;

	do {
		/*
		 * Put a delay here to deal with flash programming problem.
		 */
		u32 mptval = UBICOM32_IO_TIMER->mptval + TEN_MICRO_SECONDS;
		while (UBICOM32_IO_TIMER->mptval < mptval);

		io->ctl1 &= ~IO_XFL_CTL1_MASK;
		io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_RD) | IO_XFL_CTL1_FC_DATA(1);
		io->ctl2 = IO_XFL_CTL2_FC_CMD(OPCODE_RDSR);
		FLASH_COMMAND_EXEC(io);
	} while (io->status1 & SR_WIP);
}

/*
 * ubicom32fc_write_next()
 */
static size_t ubicom32fc_write_next(u32 addr, u8 *buf, size_t length)
{
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)IO_PORT_RA;
	u32 data_start = addr;
	u32 data_end = addr + length;
	size_t count;
	u32 i, j;

	/*
	 * Top limit address.
	 */
	u32 block_start = ALIGN_DOWN(data_start, 4);
	u32 block_end = block_start + EXTFLASH_WRITE_BLOCK_SIZE;

	union {
		u8 byte[EXTFLASH_WRITE_BLOCK_SIZE];
		u32 word[EXTFLASH_WRITE_BLOCK_SIZE / 4];
	} write_buf;

	u32 *flash_addr = (u32 *)block_start;

	/*
	 * The write block must be limited by FLASH internal buffer.
	 */
	u32 block_end_align = ALIGN_DOWN(block_end, 256);
	bool write_needed;

	block_end = (block_end_align > block_start) ? block_end_align : block_end;
	data_end = (data_end <= block_end) ? data_end : block_end;
	block_end = ALIGN_UP(data_end, 4);
	count = data_end - data_start;

	/*
	 * Transfer data to a buffer.
	 */
	for (i = 0; i < (block_end - block_start) / 4; i++) {
		/*
		 * The FLASH read can hold D-cache for a long time.
		 * Use I/O operation to read FLASH to avoid starving other
		 * threads, especially HRT.  (Do this for application only)
		 */
		write_buf.word[i] = ubicom32fc_read_u32(
			(u32)(&flash_addr[i]));
	}

	write_needed = false;
	for (i = 0, j = (data_start - block_start); i < (data_end - data_start); i++, j++) {
		write_needed = write_needed || (write_buf.byte[j] != buf[i]);
		write_buf.byte[j] &= buf[i];
	}


	/*
	 * If the data in FLASH is identical to what to be written. Then skip
	 * it.
	 */
	if (write_needed) {
		/*
		 * Write to flash.
		 */
		void *tmp __attribute__((unused));
		s32 extra_words;

		asm volatile(
		"	move.4	%0, %2									\n\t"
		"	bset	"D(IO_INT_SET)"(%1), #0, #%%bit("D(IO_PORTX_INT_FIFO_TX_RESET)")	\n\t"
		"	pipe_flush 0									\n\t"
		"	.rept	"D(EXTFLASH_WRITE_FIFO_SIZE / 4)"					\n\t"
		"	move.4	"D(IO_TX_FIFO)"(%1), (%0)4++						\n\t"
		"	.endr										\n\t"
			: "=&a" (tmp)
			: "a" (io), "r" (&write_buf.word[0])
			: "memory", "cc"
		);

		/*
		 * Lock FLASH for write access.
		 */
		io->ctl0 |= IO_XFL_CTL0_MCB_LOCK;

		/*
		 * Command: WREN
		 */
		io->ctl1 &= ~IO_XFL_CTL1_MASK;
		io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_CMD);
		io->ctl2 = IO_XFL_CTL2_FC_CMD(OPCODE_WREN);
		FLASH_COMMAND_EXEC(io);

		/*
		 * Command: BYTE PROGRAM
		 */
		io->ctl1 &= ~IO_XFL_CTL1_MASK;
		io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_WR) |
			    IO_XFL_CTL1_FC_DATA(block_end - block_start) |
			    IO_XFL_CTL1_FC_ADDR;
		io->ctl2 = IO_XFL_CTL2_FC_CMD(OPCODE_PP) | IO_XFL_CTL2_FC_ADDR(block_start);
		FLASH_COMMAND_KICK_OFF(io);

		extra_words = (s32)(block_end - block_start - EXTFLASH_WRITE_FIFO_SIZE) / 4;
		if (extra_words > 0) {
			asm volatile(
			"	move.4		%0, %3				\n\t"
			"1:	cmpi		"D(IO_FIFO_LEVEL)"(%1), #4	\n\t"
			"	jmpgt.s.t	1b				\n\t"
			"	move.4		"D(IO_TX_FIFO)"(%1), (%0)4++	\n\t"
			"	add.4		%2, #-1, %2			\n\t"
			"	jmpgt.t		1b				\n\t"
				: "=&a" (tmp)
				: "a" (io), "d" (extra_words),
				  "r" (&write_buf.word[EXTFLASH_WRITE_FIFO_SIZE / 4])
				: "memory", "cc"
			);
		}
		FLASH_COMMAND_WAIT_FOR_COMPLETION(io);

		ubicom32fc_wait_until_complete();

		/*
		 * Unlock FLASH for cache access.
		 */
		io->ctl0 &= ~IO_XFL_CTL0_MCB_LOCK;
	}

	/*
	 * Complete.
	 */
	return count;
}

/*
 * ubicom32fc_write()
 */
static void ubicom32fc_write(u32 addr, const void *src, size_t length)
{
	/*
	 * Write data
	 */
	u8_t *ptr = (u8_t *)src;
	while (length) {
		size_t count = ubicom32fc_write_next(addr, ptr, length);
		addr += count;
		ptr += count;
		length -= count;
	}
}

/*
 * Service routine to read status register until ready, or timeout occurs.
 * Returns non-zero if error.
 */
static int wait_till_ready(struct ubicom32fc_info *flash)
{
	int count;
	int sr;

	/*
	 * one chip guarantees max 5 msec wait here after page writes,
	 * but potentially three seconds (!) after page erase.
	 */
	for (count = 0; count < MAX_READY_WAIT_COUNT; count++) {
		u32 mptval;
		sr = ubicom32fc_read_status(flash);
		if (sr < 0)
			break;
		else if (!(sr & SR_WIP))
			return 0;

		/*
		 * Put a 10us delay here to deal with flash programming problem.
		 */
		mptval = UBICOM32_IO_TIMER->mptval + TEN_MICRO_SECONDS;
		while ((s32)(mptval - UBICOM32_IO_TIMER->mptval) > 0) {
			WATCHDOG_RESET();
		}
		/* REVISIT sometimes sleeping would be best */
	}

	return 1;
}

/*
 * ubicom32fc_erase_page()
 */
static void ubicom32fc_erase_page(u32 addr)
{
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)IO_PORT_RA;

	/*
	 * Lock FLASH for write access.
	 */
	io->ctl0 |= IO_XFL_CTL0_MCB_LOCK;

	/*
	 * Command: WREN
	 */
	io->ctl1 &= ~IO_XFL_CTL1_MASK;
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_CMD);
	io->ctl2 = IO_XFL_CTL2_FC_CMD(OPCODE_WREN);
	FLASH_COMMAND_EXEC(io);

	/*
	 * Command: ERASE
	 */
	io->ctl1 &= ~IO_XFL_CTL1_MASK;
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_CMD) | IO_XFL_CTL1_FC_ADDR;
	io->ctl2 = IO_XFL_CTL2_FC_CMD(OPCODE_SE) | IO_XFL_CTL2_FC_ADDR(addr);
	FLASH_COMMAND_EXEC(io);

	ubicom32fc_wait_until_complete();

	/*
	 * Unlock FLASH for cache access.
	 */
	io->ctl0 &= ~IO_XFL_CTL0_MCB_LOCK;
}

/*
 * ubicom32fc_erase()
 */
static u32 ubicom32fc_erase(u32 addr, u32 length)
{
	/*
	 * Calculate the endaddress to be the first address of the page
	 * just beyond this erase section of pages.
	 */
	u32 endaddr = addr + length;

	/*
	 * Erase.
	 */
	while (addr < endaddr) {
		u32 test_addr = addr;
		ubicom32fc_erase_page(addr);

		/*
		 * Test how much was erased as actual flash page at this address
		 * may be smaller than the expected page size.
		 */
		while (test_addr < endaddr) {
			/*
			 * The FLASH read can hold D-cache for a long time.  Use
			 * I/O operation to read FLASH to avoid starving other
			 * threads, especially HRT.  (Do this for application
			 * only)
			 */
			if (ubicom32fc_read_u32(test_addr) != 0xFFFFFFFF) {
				break;
			}
			test_addr += 4;
		}
		if (test_addr == addr) {
			printk("erase failed at address 0x%x, skipping", test_addr);
			test_addr += 4;
			return 1;
		}
		addr = test_addr;
	}
	return 0;
}

/*
 * ubicom32fc_driver_erase
 *	Erase an address range on the flash chip.  The address range may extend
 *	one or more erase sectors.  Return an error is there is a problem erasing.
 */
static int ubicom32fc_driver_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	struct ubicom32fc_info *info = (struct ubicom32fc_info *)mtd->priv;
	DEBUG(MTD_DEBUG_LEVEL2, "%s: erase %08x, len %lld\n",
		info->mtd.name, (u32)instr->addr, instr->len);

	if ((instr->addr + instr->len) > info->mtd.size) {
		return -EINVAL;
	}
	if (((instr->addr % mtd->erasesize) != 0) || ((instr->len % mtd->erasesize) != 0)) {
		return -EINVAL;
	}

	if (info->select) {
		info->select(info->appdata);
	}

	mutex_lock(&info->lock);

	/*
	 * REVISIT in some cases we could speed up erasing large regions
	 * by using OPCODE_SE instead of OPCODE_BE_4K
	 */

	if (ubicom32fc_erase(instr->addr, instr->len)) {
		instr->state = MTD_ERASE_FAILED;
		mutex_unlock(&info->lock);

		if (info->unselect) {
			info->unselect(info->appdata);
		}
		return -EIO;
	}

	mutex_unlock(&info->lock);

	if (info->unselect) {
		info->unselect(info->appdata);
	}

	instr->state = MTD_ERASE_DONE;
	mtd_erase_callback(instr);
	return 0;
}

/*
 * ubicom32fc_driver_read
 *	Read an address range from the flash chip.  The address range
 *	may be any size provided it is within the physical boundaries.
 */
static int ubicom32fc_driver_read(struct mtd_info *mtd, loff_t from,
				  size_t len, size_t *retlen, u_char *buf)
{
	struct ubicom32fc_info *info = (struct ubicom32fc_info *)mtd->priv;
	DEBUG(MTD_DEBUG_LEVEL2, "%s: read from %08x to %p, len %d\n",
		info->mtd.name, (u32)from, buf, len);

	if (!len) {
		return 0;
	}

	if ((from + len) > info->mtd.size) {
		return -EINVAL;
	}

	/*
	 * Byte count starts at zero.
	 */
	if (retlen) {
		*retlen = 0;
	}

	if (info->select) {
		info->select(info->appdata);
	}

	mutex_lock(&info->lock);

	/*
	 * Wait till previous write/erase is done.
	 */
	if (wait_till_ready(info)) {
		/*
		 * REVISIT status return??
		 */
		mutex_unlock(&info->lock);

		if (info->unselect) {
			info->unselect(info->appdata);
		}
		return 1;
	}

	ubicom32fc_read(from, (void *)buf, len);

	if (retlen) {
		*retlen = len;
	}

	mutex_unlock(&info->lock);

	if (info->unselect) {
		info->unselect(info->appdata);
	}

	return 0;
}

/*
 * ubicom32fc_driver_write
 *	Write an address range to the flash chip.  Data must be written in
 *	FLASH_PAGESIZE chunks.  The address range may be any size provided
 *	it is within the physical boundaries.
 */
static int ubicom32fc_driver_write(struct mtd_info *mtd, loff_t to,
				   size_t len, size_t *retlen, const u_char *buf)
{
	struct ubicom32fc_info *info = (struct ubicom32fc_info *)mtd->priv;
	DEBUG(MTD_DEBUG_LEVEL2, "%s: write from %p to %08x, len %d\n", info->mtd.name, buf, (u32)to, len);

	if (retlen) {
		*retlen = 0;
	}

	if (!len) {
		return 0;
	}

	if ((to + len) > info->mtd.size) {
		return -EINVAL;
	}

	if (info->select) {
		info->select(info->appdata);
	}

	mutex_lock(&info->lock);

	ubicom32fc_write(to, (void *)buf, len);

	/*
	 * Wait until finished previous write command.
	 */
	if (wait_till_ready(info)) {
		mutex_unlock(&info->lock);

		if (info->unselect) {
			info->unselect(info->appdata);
		}
		return 1;
	}

	if (retlen) {
		*retlen = len;
	}

	mutex_unlock(&info->lock);

	if (info->unselect) {
		info->unselect(info->appdata);
	}
	return 0;
}

/*
 * ubicom32fc_jedec_probe
 *	Must be called with chip selected.
 */
static struct ubicom32fc_flash_type *__devinit ubicom32fc_jedec_probe(struct platform_device *pdev)
{
	int tmp;
	u32 jedec;
	struct ubicom32fc_flash_type *type;
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)IO_PORT_RA;

	/*
	 * Setup and run RDID command on the flash.
	 */
	io->ctl1 &= ~IO_XFL_CTL1_MASK;
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_RD) | IO_XFL_CTL1_FC_DATA(3);
	io->ctl2 = IO_XFL_CTL2_FC_CMD(OPCODE_RDID);
	FLASH_COMMAND_EXEC(io);

	jedec = io->status1 & 0x00ffffff;

	for(tmp = 0, type = ubicom32fc_flash_types; tmp < ARRAY_SIZE(ubicom32fc_flash_types); tmp++, type++) {
		if (type->jedec_id == jedec)
			return type;
	}

	dev_err(&pdev->dev, "unrecognized JEDEC id %06x\n", jedec);
	return NULL;
}

/*
 * ubicom32fc_probe
 */
static int __devinit ubicom32fc_probe(struct platform_device *pdev)
{
	struct ubicom32fc_platform_data *pdata;
	struct ubicom32fc_info *info;
	struct ubicom32fc_flash_type *type;
	unsigned int i;
#ifdef CONFIG_MTD_PARTITIONS
	struct mtd_partition	*parts = NULL;
	int			nr_parts = 0;
	static const char *part_probes[] = { "cmdlinepart", NULL, };
#endif

	/*
	 * Platform data helps sort out which chip type we have, as
	 * well as how this board partitions it.  If we don't have
	 * a chip ID, try the JEDEC id commands; they'll work for most
	 * newer chips, even if we don't recognize the particular chip.
	 */
	pdata = pdev->dev.platform_data;
	if (pdata && pdata->type) {
		/*
		 * See if the type specified by the platform_data matches
		 * one of the devices we have in our list.
		 */
		for (i = 0, type = ubicom32fc_flash_types; i < ARRAY_SIZE(ubicom32fc_flash_types); i++, type++) {
			if (strcmp(pdata->type, type->name) == 0)
				break;
		}

		/*
		 * If we didn't find it in our list, we're out
		 */
		if (i == ARRAY_SIZE(ubicom32fc_flash_types)) {
//			DEBUG(MTD_DEBUG_LEVEL0, "%s: unrecognized id %s\n", pdev->dev.bus_id, pdata->type);
			return -ENODEV;
		}

		/*
		 * Check to see if the JEDEC ID matches
		 */
		if (type->jedec_id) {
			struct ubicom32fc_flash_type *chip;

			if (pdata->select) {
				pdata->select(pdata->appdata);
			}

			chip = ubicom32fc_jedec_probe(pdev);

			if (pdata->unselect) {
				pdata->unselect(pdata->appdata);
			}

			if (!chip || chip != type) {
				dev_warn(&pdev->dev, "found %s, expected %s\n", chip ? chip->name : "UNKNOWN", type->name);
				return -ENODEV;
			}
		}
	} else {
		if (pdata && pdata->select) {
			pdata->select(pdata->appdata);
		}

		type = ubicom32fc_jedec_probe(pdev);

		if (pdata && pdata->unselect) {
			pdata->unselect(pdata->appdata);
		}

		if (!type) {
			return -ENODEV;
		}
	}

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (!info) {
		return -ENOMEM;
	}

	mutex_init(&info->lock);
	dev_set_drvdata(&pdev->dev, info);

	if (pdata) {
		info->select = pdata->select;
		info->unselect = pdata->unselect;
		info->appdata = pdata->appdata;
	}

	if (pdata && pdata->name) {
		strncpy(info->dev_name, pdata->name, sizeof(info->dev_name));
	} else {
		strncpy(info->dev_name, dev_name(&pdev->dev), sizeof(info->dev_name));
	}
	info->dev_name[sizeof(info->dev_name) - 1] = 0;
	info->mtd.name = info->dev_name;
	info->mtd.type = MTD_NORFLASH;
	info->mtd.writesize = 1;
	info->mtd.flags = MTD_CAP_NORFLASH;
	info->mtd.size = type->sector_size * type->n_sectors;
	info->mtd.erase = ubicom32fc_driver_erase;
	info->mtd.read = ubicom32fc_driver_read;
	info->mtd.write = ubicom32fc_driver_write;
	info->mtd.priv = info;

	/*
	 * The Ubicom erase code does not use the opcode for smaller sectors,
	 * so disable that functionality and keep erasesize == sector_size
	 * so that the test in ubicom32fc_driver_erase works properly.
	 *
	 * This was: `if (type->flags & SECT_4K) {' infoead of `if (0) {'
	 */
	if (0) {
		info->erase_opcode = OPCODE_BE_4K;
		info->mtd.erasesize = 4096;
	} else {
		info->erase_opcode = OPCODE_SE;
		info->mtd.erasesize = type->sector_size;
	}

	dev_info(&pdev->dev, "%s (%lld Kbytes)\n", type->name, info->mtd.size / 1024);

	DEBUG(MTD_DEBUG_LEVEL2, "mtd .name = %s, .size = 0x%.8llx (%lluMiB) "
				".erasesize = 0x%.8x (%uKiB) .numeraseregions = %d\n",
				info->mtd.name,
				info->mtd.size, info->mtd.size / (1024*1024),
				info->mtd.erasesize, info->mtd.erasesize / 1024,
				info->mtd.numeraseregions);

	if (info->mtd.numeraseregions) {
		for (i = 0; i < info->mtd.numeraseregions; i++) {
			DEBUG(MTD_DEBUG_LEVEL2, "mtd.eraseregions[%d] = { .offset = 0x%.8llx, "
						".erasesize = 0x%.8x (%uKiB), "
						".numblocks = %d }\n",
						i, info->mtd.eraseregions[i].offset,
						info->mtd.eraseregions[i].erasesize,
						info->mtd.eraseregions[i].erasesize / 1024,
						info->mtd.eraseregions[i].numblocks);
		}
	}


	/*
	 * partitions should match sector boundaries; and it may be good to
	 * use readonly partitions for writeprotected sectors (BP2..BP0).
	 */
#ifdef CONFIG_MTD_PARTITIONS
	nr_parts = parse_mtd_partitions(&info->mtd, part_probes, &parts, 0);

	if (nr_parts <=0 && pdata && pdata->parts) {
		parts = pdata->parts;
		nr_parts = pdata->nr_parts;
	}

	if (nr_parts > 0) {
		for (i = 0; i < nr_parts; i++) {
			DEBUG(MTD_DEBUG_LEVEL2, "%s: partitions[%d] = "
						"{.name = %s, .offset = 0x%.8llx, "
						".size = 0x%.8llx (%lluKiB) }\n",
						info->mtd.name, i, parts[i].name,
						parts[i].offset,
						parts[i].size,
						parts[i].size / 1024);
		}
		return add_mtd_partitions(&info->mtd, parts, nr_parts);
	} else
#endif
	{
		dev_warn(&pdev->dev, "%s: no partition info, using whole device\n", info->mtd.name);
		add_mtd_device(&info->mtd);
	}
	platform_set_drvdata(pdev, info);

	return 0;
}

/*
 * ubicom32fc_remove
 */
static int __devexit ubicom32fc_remove(struct platform_device *pdev)
{
	struct ubicom32fc_info *info = dev_get_drvdata(&pdev->dev);
	int status;

#ifdef CONFIG_MTD_PARTITIONS
	status = del_mtd_partitions(&info->mtd);
#else
	status = del_mtd_device(&info->mtd);
#endif

	if (status == 0) {
		kfree(info);
	}
	return status;
}

#ifdef UBICOM32FC_AUTO_LOAD
static struct platform_device *ubicom32fc_platform_device;
#endif
static struct platform_driver ubicom32fc_platform_driver = {
	.driver = {
		.name	= DRIVER_NAME,
		.owner	= THIS_MODULE,
	},
	.probe	= ubicom32fc_probe,
	.remove	= __devexit_p(ubicom32fc_remove),
};

/*
 * ubicom32fc_init
 */
static int ubicom32fc_init(void)
{
	int ret;
	ret = platform_driver_register(&ubicom32fc_platform_driver);
#ifdef UBICOM32FC_AUTO_LOAD
	if (ret) {
		return ret;
	}

	ubicom32fc_platform_device = platform_device_alloc(DRIVER_NAME, -1);
	if (!ubicom32fc_platform_device) {
		platform_driver_unregister(&ubicom32fc_platform_driver);
		return -ENOMEM;
	}

	ret = platform_device_add(ubicom32fc_platform_device);
	if (ret) {
		platform_device_put(ubicom32fc_platform_device);
		platform_driver_unregister(&ubicom32fc_platform_driver);
	}
#endif
	return ret;
}
module_init(ubicom32fc_init);

/*
 * ubicom32fc_exit
 */
static void __exit ubicom32fc_exit(void)
{
#ifdef UBICOM32FC_AUTO_LOAD
	platform_device_unregister(ubicom32fc_platform_device);
#endif
	platform_driver_unregister(&ubicom32fc_platform_driver);
}
module_exit(ubicom32fc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Tjin");
MODULE_DESCRIPTION("Ubicom32 built in flash controller MTD driver");
