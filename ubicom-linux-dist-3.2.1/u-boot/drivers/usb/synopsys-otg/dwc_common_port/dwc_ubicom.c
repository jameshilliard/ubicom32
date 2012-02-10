/*
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
 *
 */

#include <common.h>
#include <asm/ip5000.h>

/*
 * dma_flush_buffer()
 *	flush and invalidate buffer.
 */
static void dma_flush_buffer(void * addr, u32 len)
{
	/*
	 * Calculate the cache lines we need to operate on that include
	 * begin_addr though end_addr.
	 */
	unsigned long op_addr, begin_addr, end_addr;
	unsigned long byte_count;
	u32_t *a_tmp, d_tmp;

	begin_addr = (unsigned long)addr & ~(CACHE_LINE_SIZE - 1);
	end_addr = ((unsigned long)addr + len + CACHE_LINE_SIZE - 1) & ~(CACHE_LINE_SIZE - 1);
	op_addr = end_addr;
	byte_count = end_addr - begin_addr;

#if (CACHE_LINE_SIZE != 32)
#error "Let me know when cache line size changed"
#endif
	asm volatile (
	"	lsr.4		%[n], %[n], #5			\n\t"	// n = number of cache lines to be flushed
	"	jmpeq.w.f	10f				\n\t"	// zero check

	"	sub.4		%[call_off], #0, %[n]		\n\t"
	"	and.4		%[call_off], #63, %[call_off]	\n\t"	// D15 = (2^N - 1) & -(n) where N = 6 here
	"	call		%[call_addr], .+4		\n\t"
	"	lea.4		%[call_addr], (%[call_addr], %[call_off])	\n\t"
	"	calli		%[call_addr], 8(%[call_addr])	\n\t"

	"1:	.rept		64				\n\t"	// repetition number: 2^N = 64 for N = 6
	"	flush		-32(%[addr])++			\n\t"
	"	.endr						\n\t"
	"	add.4		%[n], #-64, %[n]		\n\t"
	"	jmpgt.w.t	1b				\n\t"	// intentional true prediction for delay
	"	jmpt.w.t	100f				\n\t"	// these two jump combined as pipe_flush

	"10:	flush		(%[addr])			\n\t"	// fluch one line as minimum
	"	pipe_flush	0				\n\t"

	"100:							\n\t"

	/*
	 * Wait for the flush operations to complete.
	 *
	 * The sync instruction can deadlock if three or more threads
	 * are issuing speculative syncs that get canceled.  We can
	 * avoid the problem by putting a number of no-ops before the
	 * sync. The few noops used do not impact performance in that
	 * on average we would wait 1/2 a cache write back cycle which
	 * is significantly longer. This delay is implemented as a
	 * pipe_flush (in the code above)
	 *
	 * The expectation is that the thread issuing a sync should
	 * issue another cache op (rd, wr,f lush, prefetch) after the
	 * sync and this op will be blocked till the sync operation
	 * actually completes.
	 */
	"	sync		(%[addr])			\n\t"
	"	flush		(%[addr])			\n\t"
		: [addr]"+a" (op_addr), [n]"+d" (byte_count), [call_addr]"=&a" (a_tmp), [call_off]"=&d" (d_tmp)
		:
		: "cc", "memory"
	);
}

/*
 * Flush the buf out of cache
 */
void ubi32_flush(void *buf, unsigned int size)
{
	dma_flush_buffer(buf, size);
}

/*
 * Invalidate cache for the buf
 */
void ubi32_invalidate(void *buf, unsigned int size)
{
	dma_flush_buffer(buf, size);
}

void ubi32_br_writel(volatile u32 * volatile addr, u32 val)
{
        asm volatile (
                " move.4 0(%[addr]), %[val]     \n"
                " cycles        5               \n"
                :
                : [addr] "a" (addr), [val] "d" (val)
        );
}

u32 ubi32_br_readl(volatile u32 * volatile addr)
{
        volatile register u32 ret;
        asm volatile (
                " move.4 %[ret], 0(%[addr])     \n"
                " cycles        5               \n"
                : [ret] "=&d" (ret)
                : [addr] "a" (addr)
        );
        return ret;
}

