/*
 * Flash memory functions
 *
 * (C) Copyright 2009-2010
 * Ubicom, Inc. www.ubicom.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <common.h>
#include "asm/ip5000.h"
#include "asm/UbicomSDK.h"

/*
 * Define the filename to be used for assertions.
 */
THIS_FILE("mem_flash");

/*
 * The write queue flush procedure in mem_cache_control needs to make
 * DCACHE_WRITE_QUEUE_LENGTH writes to DDR (not OCM). Here we reserve some
 * memory for this operation.
 * Allocate array of cache lines of least DCACHE_WRITE_QUEUE_LENGTH + 1 words in
 * length rounded up to the nearest cache line.
 */
#define CACHE_WRITE_QUEUE_FLUSH_AREA_SIZE \
	((sizeof(int) * (DCACHE_WRITE_QUEUE_LENGTH + 1) + CACHE_LINE_SIZE - 1) & ~(CACHE_LINE_SIZE - 1))

static u8_t cache_write_queue_flush_area[CACHE_WRITE_QUEUE_FLUSH_AREA_SIZE]
	__attribute__((aligned(CACHE_LINE_SIZE)));

/*
 * ONE_CCR_ADDR_OP is a helper macro that executes a single CCR operation.
 */
#define ONE_CCR_ADDR_OP(cc, op_addr, op)								\
	do {												\
		asm volatile (										\
		"	btst	"D(CCR_CTRL)"(%0), #"D(CCR_CTRL_VALID)"				\n\t"	\
		"	jmpne.f	.-4								\n\t"	\
		"										\n\t"	\
		"	move.4	"D(CCR_ADDR)"(%0), %1						\n\t"	\
		"	move.1	"D(CCR_CTRL+3)"(%0), %2						\n\t"	\
		"	bset	"D(CCR_CTRL)"(%0), "D(CCR_CTRL)"(%0), #"D(CCR_CTRL_VALID)"	\n\t"	\
		"	cycles	2								\n\t"	\
		"	btst	"D(CCR_CTRL)"(%0), #"D(CCR_CTRL_DONE)"				\n\t"	\
		"	jmpeq.f	.-4								\n\t"	\
			:										\
			: "a" (cc), "r" (op_addr), "r" (op & 0xff)					\
			: "cc"										\
		);											\
	} while (0)

/*
 * mem_cache_control()
 *	Special cache control operation
 */
void mem_cache_control(mem_addr_t cc, mem_addr_t begin_addr, mem_addr_t end_addr, u8_t op)
{
	if (cc == DCCR_BASE && op == CCR_CTRL_FLUSH_ADDR) {
		/*
		 * We ensure all previous writes have left the data cache write
		 * queue by sending DCACHE_WRITE_QUEUE_LENGTH writes (to
		 * different words) down the queue.  If this is not done it's
		 * possible that the data we are trying to flush hasn't even
		 * entered the data cache.
		 * The +1 ensure that the final 'flush' is actually a flush.
		 */
		void *flush_area = cache_write_queue_flush_area;
		asm volatile (
		"	.rept "D(DCACHE_WRITE_QUEUE_LENGTH + 1)"	\n\t"
		"	move.4 (%0)4++, d0				\n\t"
		"	.endr						\n\t"
			: "+a" (flush_area)
		);
	}

	/*
	 * Calculate the cache lines we need to operate on that include
	 * begin_addr though end_addr.
	 */
	begin_addr = begin_addr & ~(CACHE_LINE_SIZE - 1);
	end_addr = (end_addr + CACHE_LINE_SIZE - 1) & ~(CACHE_LINE_SIZE - 1);
	mem_addr_t op_addr = begin_addr;

	do {
		ONE_CCR_ADDR_OP(cc, op_addr, op);
		op_addr += CACHE_LINE_SIZE;
	} while (op_addr < end_addr);

	if (cc == DCCR_BASE && op == CCR_CTRL_FLUSH_ADDR) {
		/*
		 * It turns out that when flushing the data cache the last flush
		 * isn't actually complete at this point. This is because there
		 * is another write buffer on the DDR side of the cache that is
		 * arbitrated with the I-Cache.
		 *
		 * The only foolproof method that ensures that the last data
		 * cache flush *actually* completed is to do another flush on a
		 * dirty cache line. This flush will block until the DDR write
		 * buffer is empty.
		 *
		 * Rather than creating a another dirty cache line, we use the
		 * flush_area above as we know that it is dirty from previous
		 * writes.
		 */
		ONE_CCR_ADDR_OP(cc, cache_write_queue_flush_area, op);
	}
}

/*
 * Ubicom32 FLASH Command Set
 */
#define FLASH_FC_INST_CMD	0x00	// for SPI command only transaction
#define FLASH_FC_INST_WR	0x01	// for SPI write transaction
#define FLASH_FC_INST_RD	0x02	// for SPI read transaction

/*
 * Flash opcodes.
 */
#define FLASH_SPICMD_NULL	0x00	// invalid
#define FLASH_SPICMD_WRSR	0x01	// write status: FC_WRITE - 0(addr) - 0(dummy) - 1(data)
#define FLASH_SPICMD_PROGRAM	0x02	// write data: FC_WRITE - 3(addr) - 0(dummy) - N(data N=1 for SST)
#define FLASH_SPICMD_RD		0x03	// read data: FC_READ - 3(addr) - 0(dummy) - N(data)
#define FLASH_SPICMD_WRDI	0x04	// write disable: FC_CMD - 0(addr) - 0(dummy) - 0(data)
#define FLASH_SPICMD_RDSR	0x05	// read status: FC_READ - 0(addr) - 0(dummy) - 1(data)
#define FLASH_SPICMD_WREN	0x06	// write enable: FC_CMD - 0(addr) - 0(dummy) - 0(data)
#define FLASH_SPICMD_FAST_RD	0x0b	// fast read(not for ATMEL): FC_READ - 3(addr) - 1(dummy) - N(data)
#define FLASH_SPICMD_RDID	0x9f	// FC_READ - 0(addr) - 0(dummy) - 3(data id-dev-cap)
#define FLASH_SPICMD_PAGE_ERASE	0xd8	// sector/page erase: FC_CMD - 3(addr) - 0(dummy) - 0(data)
#define FLASH_SPICMD_CHIP_ERASE	0xc7	// whole chip erase: FC_CMD - 0(addr) - 0(dummy) - 0(data)

// SST specific commands
#define FLASH_SPICMD_SST_AAI 0xad	// write data: FC_WRITE - 3(addr) - 0(dummy) - N(data N=2) SST only

// ATMEL specific commands
#define FLASH_SPICMD_RDID_ATMEL 0x15	// FC_READ - 0(addr) - 0(dummy) - 2(data 0x1f-dev)
#define FLASH_SPICMD_PAGE_ERASE_ATMEL 0x52
#define FLASH_SPICMD_CHIP_ERASE_ATMEL 0x62

/*
 * Status Register bits.
 */
#define FLASH_STATUS_WIP	(1 << 0)
#define FLASH_STATUS_WEL	(1 << 1)
#define FLASH_STATUS_BP_MASK	(7 << 2)
#define FLASH_STATUS_SRWD	(1 << 7)

/*
 * Flash type
 */
#define EXTFLASH_TYPE_UNKNOWN	0x00
#define EXTFLASH_TYPE_AMD	0x01	// Include Spansion type
#define EXTFLASH_TYPE_ATMEL	0x1f	// Untested
#define EXTFLASH_TYPE_INTEL	0x89
#define EXTFLASH_TYPE_MXIC	0xc2
#define EXTFLASH_TYPE_SST	0xbf	// Untested
#define EXTFLASH_TYPE_ST	0x20
#define EXTFLASH_TYPE_WINBOND	0xef

//uboot - this was static!
u8_t mem_flash_type = EXTFLASH_TYPE_UNKNOWN;

/*
 * Actual total/page size
 */
//uboot - these were static!
mem_addr_t mem_flash_actual_total_size;
mem_addr_t mem_flash_actual_page_size;

#define ALIGN_DOWN(v, a)	((v) & ~((a) - 1))
#define ALIGN_UP(v, a)		(((v) + ((a) - 1)) & ~((a) - 1))

#define	FLASH_COMMAND_KICK_OFF(io)							\
	asm volatile(									\
	"	bset	"D(IO_INT_CLR)"(%0), #0, #%%bit("D(IO_XFL_INT_DONE)")	\n\t"	\
	"	jmpt.t	.+4							\n\t"	\
	"	bset	"D(IO_INT_SET)"(%0), #0, #%%bit("D(IO_XFL_INT_START)")	\n\t"	\
		:									\
		: "a"(io)								\
		: "cc"									\
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

/*
 * Write buffer (TX_FIFO size is 8x32)
 */
#define EXTFLASH_WRITE_FIFO_SIZE  32
#define EXTFLASH_WRITE_BLOCK_SIZE EXTFLASH_WRITE_FIFO_SIZE /* limit the size to
							    * FIFO capacity, so
							    * the thread can be
							    * suspended. */

#define TEN_MICRO_SECONDS (GEN_CLK_MPT_FREQ * 10 / 1000000)

/*
 * mem_flash_io_read_u32()
 */
static u32_t mem_flash_io_read_u32(mem_addr_t addr)
{
	DEBUG_ASSERT(!(addr & 3), "Aligned read only for FLASH I/O read");

	struct ubicom32_io_port *io = (struct ubicom32_io_port *)IO_PORT_RA;
	io->ctl1 &= ~IO_XFL_CTL1_MASK;
	if (mem_flash_type == EXTFLASH_TYPE_ATMEL) {
		io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_RD) | IO_XFL_CTL1_FC_DATA(4) | IO_XFL_CTL1_FC_ADDR;
		io->ctl2 = IO_XFL_CTL2_FC_CMD(FLASH_SPICMD_RD) | IO_XFL_CTL2_FC_ADDR(addr);
	} else {
		io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_RD) | IO_XFL_CTL1_FC_DATA(4) | IO_XFL_CTL1_FC_DUMMY(1) | IO_XFL_CTL1_FC_ADDR;
		io->ctl2 = IO_XFL_CTL2_FC_CMD(FLASH_SPICMD_FAST_RD) | IO_XFL_CTL2_FC_ADDR(addr);
	}
	FLASH_COMMAND_EXEC(io);
	return io->status1;
}

/*
 * mem_flash_wait_until_complete()
 */
static void mem_flash_wait_until_complete(void)
{
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)IO_PORT_RA;

	do {
		/*
		 * Put a delay here to deal with flash programming problem.
		 */
		u32_t mptval = UBICOM32_IO_TIMER->mptval + TEN_MICRO_SECONDS;
		while (UBICOM32_IO_TIMER->mptval < mptval);

		io->ctl1 &= ~IO_XFL_CTL1_MASK;
		io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_RD) | IO_XFL_CTL1_FC_DATA(1);
		io->ctl2 = IO_XFL_CTL2_FC_CMD(FLASH_SPICMD_RDSR);
		FLASH_COMMAND_EXEC(io);
	} while (io->status1 & FLASH_STATUS_WIP);
}

/*
 * mem_flash_write_next()
 */
static size_t mem_flash_write_next(mem_addr_t addr, const u8_t *buf, size_t length)
{
	DEBUG_ASSERT(length > 0, "invalid flash write size");

	struct ubicom32_io_port *io = (struct ubicom32_io_port *)IO_PORT_RA;
	mem_addr_t data_start = addr;
	mem_addr_t data_end = addr + length;
	size_t count;

	if (mem_flash_type == EXTFLASH_TYPE_SST) {
		/*
		 * Top limit address.
		 */
		mem_addr_t block_start = ALIGN_DOWN(data_start, 2);
		mem_addr_t block_end = block_start + 2;
		data_end = (data_end <= block_end) ? data_end : block_end;
		count = data_end - data_start;

		/*
		 * Transfer data to a buffer.
		 */
		union {
			u8_t byte[2];
			u16_t hword;
		} write_buf;

		/*
		 * The FLASH read can hold D-cache for a long time.
		 * Use I/O operation to read FLASH to avoid starving other threads, especially HRT.
		 */
		write_buf.hword = (u16_t)mem_flash_io_read_u32(block_start);

		bool_t write_needed = FALSE;
		for (u32_t i = 0, j = (data_start - block_start); i < (data_end - data_start); i++, j++) {
			write_needed = write_needed || (write_buf.byte[j] != buf[i]);
			write_buf.byte[j] &= buf[i];
			DEBUG_ASSERT(write_buf.byte[j] == buf[i],
				"Can't program flash[0x%x]: %hhx -> %hhx", (data_start + i), write_buf.byte[j], buf[i]);
		}

		/*
		 * If the data in FLASH is identical to what to be written. Then skip it.
		 */
		if (write_needed) {
			/*
			 * Write to flash.
			 */
			asm volatile(
			"	bset	"D(IO_INT_SET)"(%0), #0, #%%bit("D(IO_PORTX_INT_FIFO_TX_RESET)")	\n\t"
			"	pipe_flush 0									\n\t"
			"	move.4	"D(IO_TX_FIFO)"(%0), %1							\n\t"
				:
				: "a"(io), "r"(write_buf.hword << 16)
				: "cc"
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
			io->ctl2 = IO_XFL_CTL2_FC_CMD(FLASH_SPICMD_WREN);
			FLASH_COMMAND_EXEC(io);

			/*
			 * Command: BYTE PROGRAM
			 */
			io->ctl1 &= ~IO_XFL_CTL1_MASK;
			io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_WR) | IO_XFL_CTL1_FC_DATA(2) | IO_XFL_CTL1_FC_ADDR;
			io->ctl2 = IO_XFL_CTL2_FC_CMD(FLASH_SPICMD_SST_AAI) | IO_XFL_CTL2_FC_ADDR(block_start);
			FLASH_COMMAND_EXEC(io);

			/*
			 * Command: WRDI
			 */
			io->ctl1 &= ~IO_XFL_CTL1_MASK;
			io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_CMD);
			io->ctl2 = IO_XFL_CTL2_FC_CMD(FLASH_SPICMD_WRDI);
			FLASH_COMMAND_EXEC(io);

			mem_flash_wait_until_complete();

			mem_cache_control(DCCR_BASE, data_start, data_end, CCR_CTRL_INV_ADDR);
			mem_cache_control(ICCR_BASE, data_start, data_end, CCR_CTRL_INV_ADDR);

			/*
			 * Unlock FLASH for cache access.
			 */
			io->ctl0 &= ~IO_XFL_CTL0_MCB_LOCK;
		}
	} else {
		/*
		 * Top limit address.
		 */
		mem_addr_t block_start = ALIGN_DOWN(data_start, 4);
		mem_addr_t block_end = block_start + EXTFLASH_WRITE_BLOCK_SIZE;

		/*
		 * The write block must be limited by FLASH internal buffer.
		 */
		mem_addr_t block_end_align = ALIGN_DOWN(block_end, 256);
		block_end = (block_end_align > block_start) ? block_end_align : block_end;
		data_end = (data_end <= block_end) ? data_end : block_end;
		block_end = ALIGN_UP(data_end, 4);
		count = data_end - data_start;

		/*
		 * Transfer data to a buffer.
		 */
		union {
			u8_t byte[EXTFLASH_WRITE_BLOCK_SIZE];
			u32_t word[EXTFLASH_WRITE_BLOCK_SIZE / 4];
		} write_buf;

		u32_t *flash_addr = (u32_t *)block_start;
		for (u32_t i = 0; i < (block_end - block_start) / 4; i++) {
			/*
			 * The FLASH read can hold D-cache for a long time.
			 * Use I/O operation to read FLASH to avoid starving other threads, especially HRT.
			 */
			write_buf.word[i] = mem_flash_io_read_u32((addr_t)(&flash_addr[i]));
		}

		bool_t write_needed = FALSE;
		for (u32_t i = 0, j = (data_start - block_start); i < (data_end - data_start); i++, j++) {
			write_needed = write_needed || (write_buf.byte[j] != buf[i]);
			write_buf.byte[j] &= buf[i];
			DEBUG_ASSERT(write_buf.byte[j] == buf[i],
				"Can't program flash[0x%x]: %hhx -> %hhx", (data_start + i), write_buf.byte[j], buf[i]);
		}


		/*
		 * If the data in FLASH is identical to what to be written. Then skip it.
		 */
		if (write_needed) {
			/*
			 * Write to flash.
			 */
			void *tmp __attribute__((unused));
			asm volatile(
			"	move.4	%0, %2									\n\t"
			"	bset	"D(IO_INT_SET)"(%1), #0, #%%bit("D(IO_PORTX_INT_FIFO_TX_RESET)")	\n\t"
			"	pipe_flush 0									\n\t"
			"	.rept	"D(EXTFLASH_WRITE_FIFO_SIZE / 4)"					\n\t"
			"	move.4	"D(IO_TX_FIFO)"(%1), (%0)4++						\n\t"
			"	.endr										\n\t"
				: "=&a" (tmp)
				: "a" (io), "r" (&write_buf.word[0])
				: "cc"
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
			io->ctl2 = IO_XFL_CTL2_FC_CMD(FLASH_SPICMD_WREN);
			FLASH_COMMAND_EXEC(io);

			/*
			 * Command: BYTE PROGRAM
			 */
			io->ctl1 &= ~IO_XFL_CTL1_MASK;
			io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_WR) |
				    IO_XFL_CTL1_FC_DATA(block_end - block_start) |
				    IO_XFL_CTL1_FC_ADDR;
			io->ctl2 = IO_XFL_CTL2_FC_CMD(FLASH_SPICMD_PROGRAM) | IO_XFL_CTL2_FC_ADDR(block_start);
			FLASH_COMMAND_KICK_OFF(io);

#if 0
			s32_t extra_words = (s32_t)(block_end - block_start - EXTFLASH_WRITE_FIFO_SIZE) / 4;
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
					: "cc"
				);
			}
#endif
			FLASH_COMMAND_WAIT_FOR_COMPLETION(io);

			mem_flash_wait_until_complete();

			mem_cache_control(DCCR_BASE, data_start, data_end, CCR_CTRL_INV_ADDR);
			mem_cache_control(ICCR_BASE, data_start, data_end, CCR_CTRL_INV_ADDR);

			/*
			 * Unlock FLASH for cache access.
			 */
			io->ctl0 &= ~IO_XFL_CTL0_MCB_LOCK;
		}
	}

	/*
	 * Complete.
	 */
	return count;
}

/*
 * mem_flash_write()
 */
void mem_flash_write(mem_addr_t addr, const void *src, size_t length)
{
	DEBUG_TRACE("flash_write: 0x%lx 0x%lx", addr, length);

	/*
	 * Range check
	 */
	if ((addr < FLASHSTART) || ((addr + length) > (FLASHSTART + EXTFLASH_TOTAL_SIZE))) {
		DEBUG_ASSERT(0, "invalid flash address (0x%x) to write", addr);
		return;
	}

	/*
	 * Write data
	 */
	const u8_t *ptr = src;
	while (length) {
		size_t count = mem_flash_write_next(addr, ptr, length);
		addr += count;
		ptr += count;
		length -= count;
	}
}

/*
 * mem_flash_erase_page()
 */
static void mem_flash_erase_page(mem_addr_t addr)
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
	io->ctl2 = IO_XFL_CTL2_FC_CMD(FLASH_SPICMD_WREN);
	FLASH_COMMAND_EXEC(io);

	/*
	 * Command: ERASE
	 */
	io->ctl1 &= ~IO_XFL_CTL1_MASK;
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_CMD) | IO_XFL_CTL1_FC_ADDR;
	if (mem_flash_type == EXTFLASH_TYPE_ATMEL) {
		io->ctl2 = IO_XFL_CTL2_FC_CMD(FLASH_SPICMD_PAGE_ERASE_ATMEL) | IO_XFL_CTL2_FC_ADDR(addr);
	} else {
		io->ctl2 = IO_XFL_CTL2_FC_CMD(FLASH_SPICMD_PAGE_ERASE) | IO_XFL_CTL2_FC_ADDR(addr);
	}
	FLASH_COMMAND_EXEC(io);

	mem_flash_wait_until_complete();

	mem_cache_control(DCCR_BASE, addr, addr + EXTFLASH_PAGE_SIZE, CCR_CTRL_INV_ADDR);
	mem_cache_control(ICCR_BASE, addr, addr + EXTFLASH_PAGE_SIZE, CCR_CTRL_INV_ADDR);

	/*
	 * Unlock FLASH for cache access.
	 */
	io->ctl0 &= ~IO_XFL_CTL0_MCB_LOCK;
}

/*
 * mem_flash_erase()
 */
void mem_flash_erase(mem_addr_t addr, mem_addr_t length)
{
	DEBUG_INFO("flash_erase: 0x%lx 0x%lx", addr, length);

	/*
	 * Range check
	 */
	if ((addr < FLASHSTART) || ((addr + length) > (FLASHSTART + EXTFLASH_TOTAL_SIZE))) {
		DEBUG_ASSERT(0, "invalid flash address (0x%x) to erase", addr);
		return;
	}

	/*
	 * Adjust start address to nearest reverse page boundary.
	 * Calculate the endaddress to be the first address of the page
	 * just beyond this erase section of pages.
	 */
	mem_addr_t endaddr = ALIGN_UP(addr + length, EXTFLASH_PAGE_SIZE);
	addr = ALIGN_DOWN(addr, EXTFLASH_PAGE_SIZE);

	/*
	 * Erase.
	 */
	while (addr < endaddr) {
		mem_flash_erase_page(addr);

		/*
		 * Test how much was erased as actual flash page at this address
		 * may be smaller than the expected page size.
		 */
		mem_addr_t test_addr = addr;
		while (test_addr < endaddr) {
			/*
			 * The FLASH read can hold D-cache for a long time.
			 * Use I/O operation to read FLASH to avoid starving other threads, especially HRT.
			 */
			if (mem_flash_io_read_u32(test_addr) != 0xFFFFFFFF) {
				break;
			}
			test_addr += 4;
		}
		if (test_addr == addr) {
			DEBUG_ERROR("erase failed at address 0x%lx, skipping", test_addr);
			test_addr += 4;
		}
		addr = test_addr;
	}
}

/*
 * mem_flash_query()
 */
//uboot - this was static
u32_t mem_flash_query(void)
{
	struct ubicom32_io_port *io = (struct ubicom32_io_port *)IO_PORT_RA;

	/*
	 * Setup and run RDID command on the flash.
	 */
	io->ctl1 &= ~IO_XFL_CTL1_MASK;
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_RD) | IO_XFL_CTL1_FC_DATA(3);
	io->ctl2 = IO_XFL_CTL2_FC_CMD(FLASH_SPICMD_RDID);
	FLASH_COMMAND_EXEC(io);
	u8_t type = (u8_t)(io->status1 >> 16);

	if ((type != EXTFLASH_TYPE_AMD) &&
			(type != EXTFLASH_TYPE_INTEL) &&
			(type != EXTFLASH_TYPE_MXIC) &&
			(type != EXTFLASH_TYPE_SST) &&
			(type != EXTFLASH_TYPE_ST) &&
			(type != EXTFLASH_TYPE_WINBOND)) {
		/*
		 * Try for ATMEL FLASH now
		 */
		io->ctl1 &= ~IO_XFL_CTL1_MASK;
		io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_RD) | IO_XFL_CTL1_FC_DATA(2);
		io->ctl2 = IO_XFL_CTL2_FC_CMD(FLASH_SPICMD_RDID_ATMEL);
		FLASH_COMMAND_EXEC(io);
		type = (u8_t)(io->status1 >> 8);
	}

	/*
	 * Return known supported type of FLASH only
	 */
	u32_t flash_timing;
	switch (type) {
	case EXTFLASH_TYPE_AMD:		// (fast read / 50MHz clock / 100ns idle)
	case EXTFLASH_TYPE_INTEL:	// (fast read / 50MHz clock / 100ns idle)
	case EXTFLASH_TYPE_MXIC:	// (fast read / 50MHz clock / 100ns idle)
	case EXTFLASH_TYPE_SST:		// (fast read / 50MHz clock / 50ns idle)
	case EXTFLASH_TYPE_ST:		// (fast read / 50MHz clock / 100ns idle)
	case EXTFLASH_TYPE_WINBOND:	// (fast read / 50MHz clock / 100ns idle)
		/*
		 * Configure the flash timing with FC_CLK_WIDTH = Calculated value + 2
		 * (Done to fix the flash write issue that we are unable to root cause yet)
		 * and FC_CE_WAIT = 12
		 */
		flash_timing = IO_XFL_CTL0_FAST_VALUE(
			(((SYSTEM_FREQ / 1000000) * EXTFLASH_MAX_ACCESS_TIME - 1) / 1000 +2),
			12);
		flash_timing = (flash_timing ^ io->ctl0) & IO_XFL_CTL0_MASK;
#if 0	// Should be already done
		asm volatile (
		"	xor.4	(%0), (%0), %1"
			:
			: "a" (&io->ctl0), "d" (flash_timing)
			: "cc"
		);
#endif
		return type;

	case EXTFLASH_TYPE_ATMEL:	// (normal read / 33MHz clock / 25ns idle)
		/*
		 * Configure the flash timing (normal read / 33MHz clock / 25ns idle)
		 */
		flash_timing = IO_XFL_CTL0_VALUE(
			(((SYSTEM_FREQ / 1000000) * EXTFLASH_MAX_ACCESS_TIME - 1) / 1000 + 2),
			((100 - 1) / EXTFLASH_MAX_ACCESS_TIME + 2));
		flash_timing = (flash_timing ^ io->ctl0) & IO_XFL_CTL0_MASK;
#if 0	// Should be already done
		asm volatile (
		"	xor.4	(%0), (%0), %1"
			:
			: "a" (&io->ctl0), "d" (flash_timing)
			: "cc"
		);
#endif
		return type;

	default:
		return EXTFLASH_TYPE_UNKNOWN;
	}
}

/*
 * mem_flash_init()
 */
void mem_flash_init(void)
{
	/*
	 * The FLASH must be enabled by default.
	 */
	struct ubicom32_io_port *flash_port __attribute__((unused)) = (struct ubicom32_io_port *)IO_PORT_RA;
	DEBUG_ASSERT((flash_port->function == IO_PORTX_FUNC_XFL) && (flash_port->ctl0 & IO_XFL_CTL0_ENABLE),
			"FLASH must be enabled by default");

	/*
	 * Query flash device.
	 */
	mem_flash_type = mem_flash_query();
	mem_flash_actual_total_size = EXTFLASH_TOTAL_SIZE;
	mem_flash_actual_page_size = EXTFLASH_PAGE_SIZE;

	/*
	 * If the flash is Intel the BP bits will come up high. We need to drive them to zero
	 * by writing a 0 to the status register.
	 */
	if (mem_flash_type == EXTFLASH_TYPE_INTEL) {
		/*
		 * Command: WREN. This will let us write the Status Register.
		 */
		struct ubicom32_io_port *io = flash_port;

		io->ctl1 &= ~IO_XFL_CTL1_MASK;
		io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_CMD);
		io->ctl2 = IO_XFL_CTL2_FC_CMD(FLASH_SPICMD_WREN);
		FLASH_COMMAND_EXEC(io);

		/*
		 * Write 0 to SR.
		 */
		io->ctl1 &= ~IO_XFL_CTL1_MASK;
		io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_WR) | IO_XFL_CTL1_FC_DATA(1);
		io->ctl2 = IO_XFL_CTL2_FC_CMD(FLASH_SPICMD_WRSR);
		io->tx_fifo = 0;
		FLASH_COMMAND_EXEC(io);
	}
}

/*
 * mem_flash_start()
 */
void mem_flash_start(void)
{

	struct ubicom32_io_port *io = (struct ubicom32_io_port *)IO_PORT_RA;
	io->ctl1 &= ~IO_XFL_CTL1_MASK;
	io->ctl1 |= IO_XFL_CTL1_FC_INST(FLASH_FC_INST_RD) | IO_XFL_CTL1_FC_DATA(1);
	io->ctl2 = IO_XFL_CTL2_FC_CMD(FLASH_SPICMD_RDSR);
	FLASH_COMMAND_EXEC(io);
	DEBUG_ASSERT(!(io->status1 & (FLASH_STATUS_WIP | FLASH_STATUS_WEL | FLASH_STATUS_BP_MASK)),
			"FLASH status ( = %hhx) is unexpected", io->status1 & 0xff);

	/*
	 * Check flash type.
	 */
	switch (mem_flash_type) {
	case EXTFLASH_TYPE_AMD:
		DEBUG_INFO("flash type: AMD(0x%hhx)", mem_flash_type);
		break;

	case EXTFLASH_TYPE_INTEL:
		DEBUG_INFO("flash type: INTEL(0x%hhx)", mem_flash_type);
		break;

	case EXTFLASH_TYPE_MXIC:
		DEBUG_INFO("flash type: MXIC(0x%hhx)", mem_flash_type);
		break;

	case EXTFLASH_TYPE_SST:
		DEBUG_INFO("flash type: SST(0x%hhx)", mem_flash_type);
		break;

	case EXTFLASH_TYPE_ST:
		DEBUG_INFO("flash type: STMicro(0x%hhx)", mem_flash_type);
		break;

	case EXTFLASH_TYPE_WINBOND:
		DEBUG_INFO("flash type: WINBOND(0x%hhx)", mem_flash_type);
		break;

	case EXTFLASH_TYPE_ATMEL:
		DEBUG_INFO("flash type: ATMEL(0x%hhx)", mem_flash_type);
		break;

	default:
		DEBUG_INFO("flash type: UNKNOWN(0x%hhx)", mem_flash_type);
		if (mem_flash_type == EXTFLASH_TYPE_UNKNOWN) {
#if !defined(EXTFLASH_ALLOW_UNIDENTIFIED)
			DEBUG_ASSERT(FALSE, "flash type is not recoganized");
#endif
			return;
		}
		break;
	}
	DEBUG_INFO("flash configured size: %dk", EXTFLASH_TOTAL_SIZE / 1024);
	DEBUG_INFO("flash actual size: %dk", mem_flash_actual_total_size / 1024);
	DEBUG_INFO("flash configured page size: %dk", EXTFLASH_PAGE_SIZE / 1024);
	DEBUG_INFO("flash native page size: %dk", mem_flash_actual_page_size / 1024);
}

/*
 * mem_read_u8()
 */
u8_t mem_read_u8(mem_addr_t addr)
{
	/*
	 * Range check
	 */
	if ((addr < FLASHSTART) || (addr > (FLASHSTART + EXTFLASH_TOTAL_SIZE))) {
		return *(u8_t *)addr;
	}

	mem_addr_t tmp_addr = ALIGN_DOWN(addr, 4);
	u32_t tmp_data = mem_flash_io_read_u32(tmp_addr);
	u8_t *ptr = (u8_t *)&tmp_data;
	return ptr[addr & 0x3];
}

/*
 * mem_read_u16()
 */
u16_t mem_read_u16(mem_addr_t addr)
{
	DEBUG_ASSERT(!(addr & 1), "Aligned read only for FLASH read");

	/*
	 * Range check
	 */
	if ((addr < FLASHSTART) || (addr > (FLASHSTART + EXTFLASH_TOTAL_SIZE))) {
		return *(u16_t *)addr;
	}

	union data {
		u32_t longs;
		u16_t shorts[2];
	} tmp_data;

	mem_addr_t tmp_addr = ALIGN_DOWN(addr, 4);
	tmp_data.longs = mem_flash_io_read_u32(tmp_addr);
	return tmp_data.shorts[((addr & 0x3) >> 1)];
}

/*
 * mem_read_u32()
 */
u32_t mem_read_u32(mem_addr_t addr)
{
	DEBUG_ASSERT(!(addr & 3), "Aligned read only for FLASH read");

	/*
	 * Range check
	 */
	if ((addr < FLASHSTART) || (addr > (FLASHSTART + EXTFLASH_TOTAL_SIZE))) {
		return *(u32_t *)addr;
	}

	mem_addr_t tmp_addr = ALIGN_DOWN(addr, 4);
	u32_t tmp_data = mem_flash_io_read_u32(tmp_addr);
	return tmp_data;
}

/*
 * mem_read()
 */
void mem_read(void *dst, mem_addr_t addr, size_t length)
{
	/*
	 * Range check
	 */
	if ((addr < FLASHSTART) || ((addr + length) > (FLASHSTART + EXTFLASH_TOTAL_SIZE))) {
		memcpy((void *)dst, (void *)addr, length);
		return;
	}

	/*
	 * Fix source alignment.
	 */
	while (addr & 0x03) {
		if (length == 0) {
			return;
		}
		*((u8_t *)dst) = mem_read_u8(addr++);
		dst++;
		length--;
	}

	while (length >= 4) {
		u32_t tmp_data = mem_flash_io_read_u32(addr);
		addr += 4;
		length -= 4;

		/*
		 * Send the data to the destination.
		 */
		memcpy((void *)dst, (void *)&tmp_data, 4);
		dst +=4;
	}

	while (length--) {
		*((u8_t *)dst) = mem_read_u8(addr++);
		dst++;
	}
}
