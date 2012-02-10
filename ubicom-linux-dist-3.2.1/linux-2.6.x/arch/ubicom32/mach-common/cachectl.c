/*
 * arch/ubicom32/mach-common/cachectl.c
 *   Architecture cache control support
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
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */

#include <linux/module.h>
#include <linux/signal.h>
#include <linux/mman.h>
#include <linux/mm.h>
#include <linux/hardirq.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/ptrace.h>
#include <linux/uaccess.h>

#include <asm/system.h>
#include <asm/processor.h>
#include <asm/traps.h>
#include <asm/pgtable.h>
#include <asm/mmu_hardware.h>
#include <asm/fault.h>

/*
 * The write queue flush procedure in mem_d_cache_control needs to make
 * DCACHE_WRITE_QUEUE_LENGTH writes to DDR (not OCM). Here we reserve some
 * memory for this operation.
 * Allocate array of cache lines of least DCACHE_WRITE_QUEUE_LENGTH + 1 words in
 * length rounded up to the nearest cache line.
 */
#define CACHE_WRITE_QUEUE_FLUSH_AREA_SIZE \
	ALIGN(sizeof(int) * (DCACHE_WRITE_QUEUE_LENGTH + 1), CACHE_LINE_SIZE)

#if (UBICOM32_ARCH_VERSION <= 4)
static char cache_write_queue_flush_area[CACHE_WRITE_QUEUE_FLUSH_AREA_SIZE]
	__attribute__((aligned(CACHE_LINE_SIZE)));
#endif

/*
 * ONE_CCR_ADDR_OP is a helper macro that executes a single CCR operation.
 */
#define ONE_CCR_ADDR_OP(cc, op_addr, op)				\
	do {								\
		asm volatile (						\
		"	btst	"D(CCR_CTRL)"(%0), #"D(CCR_CTRL_VALID)"				\n\t" \
		"	jmpne.f	.-4								\n\t" \
		"	move.4	"D(CCR_ADDR)"(%0), %1						\n\t" \
		"	move.1	"D(CCR_CTRL+3)"(%0), %2						\n\t" \
		";	bset	"D(CCR_CTRL)"(%0), "D(CCR_CTRL)"(%0), #"D(CCR_CTRL_VALID)"	\n\t" \
		"	cycles	2								\n\t" \
		"	btst	"D(CCR_CTRL)"(%0), #"D(CCR_CTRL_DONE)"				\n\t" \
		"	jmpeq.f	.-4								\n\t" \
			:						\
			: "a" (cc), "r" (op_addr), "r" (op | (1 << CCR_CTRL_VALID))		\
			: "cc"						\
		);							\
	} while (0)

#if (UBICOM32_ARCH_VERSION > 4)
/*
 * mem_i_cache_control()
 *	Special instruction cache control operation
 */
void mem_i_cache_control(unsigned long begin_addr,
			 unsigned long end_addr, unsigned long op)
{
	unsigned long op_addr;
	unsigned long begin_page_boundary;
	unsigned long end_page_boundary;
	UBICOM32_LOCK(ICCR_LOCK_BIT);

	/*
	 * Calculate the cache lines we need to operate on that include
	 * begin_addr though end_addr.
	 */
	begin_addr = begin_addr & ~(CACHE_LINE_SIZE - 1);
	end_addr = (end_addr + CACHE_LINE_SIZE - 1) & ~(CACHE_LINE_SIZE - 1);

	if (begin_addr >= UBICOM32_FDPIC_UNMAPPED_START) {
		/*
		 * Unmapped space.
		 */
		op_addr = begin_addr;

		do {
			ONE_CCR_ADDR_OP(ICCR_BASE, op_addr, op);
			op_addr += CACHE_LINE_SIZE;
		} while (likely(op_addr < end_addr));

		UBICOM32_UNLOCK(ICCR_LOCK_BIT);
		return;
	}

	/*
	 * Mapped space.
	 */
	begin_page_boundary = begin_addr & PAGE_MASK;
	end_page_boundary = (end_addr + PAGE_SIZE -1 ) & PAGE_MASK;

	do {
		/*
		 * Go get the physical address for this page.
		 */
		unsigned long phys_start_addr = translate_to_physical(current->mm->pgd, begin_page_boundary);
		unsigned long phys_end_addr;

		/*
		 * Increment begin_page_boundary to the next page.
		 */
		begin_page_boundary += PAGE_SIZE;

		if (begin_page_boundary < end_addr) {
			/*
			 * Process only to the end of this page.
			 */
			phys_end_addr = phys_start_addr + PAGE_SIZE;
		} else {
			/*
			 * We are processing the last page.
			 */
			phys_end_addr = phys_start_addr + (end_addr & ~PAGE_MASK);
		}

		phys_start_addr += (begin_addr & ~PAGE_MASK);

		begin_addr = 0;

		if (phys_start_addr < UBICOM32_FDPIC_UNMAPPED_START) {
			/*
			 * There is no backing store for this page and hence nothing to do.
			 * Go to next page.
			 */
			continue;
		}

		/*
		 * We need to flush the I cache from phys_start_addr to phys_end_addr.
		 */
		do {
			ONE_CCR_ADDR_OP(ICCR_BASE, phys_start_addr, op);
			phys_start_addr += CACHE_LINE_SIZE;
		} while (likely(phys_start_addr < phys_end_addr));

	} while (begin_page_boundary < end_page_boundary);

	UBICOM32_UNLOCK(ICCR_LOCK_BIT);
}
#else
/*
 * mem_i_cache_control()
 *	Special instruction cache control operation
 */
void mem_i_cache_control(unsigned long begin_addr,
			 unsigned long end_addr, unsigned long op)
{
	unsigned long op_addr;
	UBICOM32_LOCK(ICCR_LOCK_BIT);

	/*
	 * Calculate the cache lines we need to operate on that include
	 * begin_addr though end_addr.
	 */
	begin_addr = begin_addr & ~(CACHE_LINE_SIZE - 1);
	end_addr = (end_addr + CACHE_LINE_SIZE - 1) & ~(CACHE_LINE_SIZE - 1);
	op_addr = begin_addr;

	do {
		ONE_CCR_ADDR_OP(ICCR_BASE, op_addr, op);
		op_addr += CACHE_LINE_SIZE;
	} while (likely(op_addr < end_addr));

	UBICOM32_UNLOCK(ICCR_LOCK_BIT);
}
#endif
EXPORT_SYMBOL(mem_i_cache_control);

#if (UBICOM32_ARCH_VERSION > 4)
/*
 * mem_d_cache_isa_flush_invalidate()
 *	Data cache flush/invalidate op using the instruction set.
 */
void mem_d_cache_isa_flush_invalidate(unsigned long begin_addr, unsigned long end_addr)
{
	/*
	 * Calculate the cache lines we need to operate on that include
	 * begin_addr though end_addr.
	 */
	unsigned long op_addr;
	unsigned long byte_count;
	u32_t *a_tmp, d_tmp;

	begin_addr = begin_addr & ~(CACHE_LINE_SIZE - 1);
	end_addr = (end_addr + CACHE_LINE_SIZE - 1) & ~(CACHE_LINE_SIZE - 1);
	op_addr = end_addr;
	byte_count = end_addr - begin_addr;

#if (CACHE_LINE_SIZE != 32)
#error "Let me know when cache line size changed"
#endif
	asm volatile (
	"	lsr.4		%[n], %[n], #5			\n\t"	// n = number of cache lines to be flushed
	"	jmpeq.w.f	1f				\n\t"	// zero check

	"	sub.4		%[call_off], #0, %[n]		\n\t"
	"	and.4		%[call_off], #63, %[call_off]	\n\t"	// D15 = (2^N - 1) & -(n) where N = 6 here
	"	call		%[call_addr], .+4		\n\t"
	"	lea.4		%[call_addr], (%[call_addr], %[call_off])	\n\t"
	"	calli		%[call_addr], 16(%[call_addr])	\n\t"	// also skip the next two instructions

	"1:	flush		(%[addr])			\n\t"	// fluch one line as minimum
	"	jmpt.w.t	100f				\n\t"

	"10:	.rept		64				\n\t"	// repetition number: 2^N = 64 for N = 6
	"	flush		-32(%[addr])++			\n\t"
	"	.endr						\n\t"
	"	add.4		%[n], #-64, %[n]		\n\t"
	"	jmpgt.w.f	10b				\n\t"

	/*
	 * Wait for the flush operations to complete.
	 *
	 * The sync instruction can deadlock if three or more threads
	 * are issuing speculative syncs that get canceled.  We can
	 * avoid the problem by putting a number of no-ops before the
	 * sync. The few noops used do not impact performance in that
	 * on average we would wait 1/2 a cache write back cycle which
	 * is significantly longer. This delay is implemented as a
	 * pipe_flush
	 *
	 * The expectation is that the thread issuing a sync should
	 * issue another cache op (rd, wr,f lush, prefetch) after the
	 * sync and this op will be blocked till the sync operation
	 * actually completes.
	 */
	"100:							\n\t"
	"	pipe_flush	0				\n\t"
	"	sync		(%[addr])			\n\t"
	"	flush		(%[addr])			\n\t"
		: [addr]"+a" (op_addr), [n]"+d" (byte_count), [call_addr]"=&a" (a_tmp), [call_off]"=&d" (d_tmp)
		:
		: "cc", "memory"
	);
}
#else
/*
 * mem_d_cache_isa_flush_invalidate()
 *	Data cache flush/invalidate op using the cache control registers.
 */
void mem_d_cache_isa_flush_invalidate(unsigned long begin_addr, unsigned long end_addr)
{
	unsigned long op_addr;

	/*
	 * We ensure all previous writes have left the data cache write
	 * queue by sending DCACHE_WRITE_QUEUE_LENGTH writes (to
	 * different words) down the queue.  If this is not done it's
	 * possible that the data we are trying to flush hasn't even
	 * entered the data cache.
	 * The +1 ensure that the final 'flush' is actually a flush.
	 */
	int *flush_area = (int *)cache_write_queue_flush_area;
	asm volatile(
	"	.rept "D(DCACHE_WRITE_QUEUE_LENGTH + 1)"	\n\t"
	"	move.4 (%0)4++, d0				\n\t"
	"	.endr						\n\t"
		: "+a"(flush_area)
	);

	UBICOM32_LOCK(DCCR_LOCK_BIT);

	/*
	 * Calculate the cache lines we need to operate on that include
	 * begin_addr though end_addr.
	 */
	begin_addr = begin_addr & ~(CACHE_LINE_SIZE - 1);
	end_addr = (end_addr + CACHE_LINE_SIZE - 1) & ~(CACHE_LINE_SIZE - 1);
	op_addr = begin_addr;

	do {
		ONE_CCR_ADDR_OP(DCCR_BASE, op_addr, CCR_CTRL_FLUSH_ADDR);
		op_addr += CACHE_LINE_SIZE;
	} while (likely(op_addr < end_addr));

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
	ONE_CCR_ADDR_OP(DCCR_BASE, cache_write_queue_flush_area, CCR_CTRL_FLUSH_ADDR);

	UBICOM32_UNLOCK(DCCR_LOCK_BIT);
}
#endif

EXPORT_SYMBOL(mem_d_cache_isa_flush_invalidate);

/*
 * mem_d_cache_control()
 *	Special data cache control operation
 */
void mem_d_cache_control(unsigned long begin_addr,
			 unsigned long end_addr, unsigned long op)
{
	unsigned long op_addr;

	UBICOM32_LOCK(DCCR_LOCK_BIT);

	/*
	 * Calculate the cache lines we need to operate on that include
	 * begin_addr though end_addr.
	 */
	begin_addr = begin_addr & ~(CACHE_LINE_SIZE - 1);
	end_addr = (end_addr + CACHE_LINE_SIZE - 1) & ~(CACHE_LINE_SIZE - 1);
	op_addr = begin_addr;

	do {
		ONE_CCR_ADDR_OP(DCCR_BASE, op_addr, op);
		op_addr += CACHE_LINE_SIZE;
	} while (likely(op_addr < end_addr));

	UBICOM32_UNLOCK(DCCR_LOCK_BIT);
}
EXPORT_SYMBOL(mem_d_cache_control);
