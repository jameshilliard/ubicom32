/*
 * arch/ubicom32/lib/memoryops.c
 *   Memory functions.
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
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/compiler.h>
#include <asm/uaccess.h>
#include <asm/memoryops.h>

/*
 * __clear_user()
 *	Zero Userspace
 *
 * Returns the number of bytes that could not be cleared.
 */
unsigned long __clear_user(__user void *dest, unsigned long size)
{
	void *dest_orig = dest;
	void *aligned_start;
	size_t n = size;

	if (unlikely(n == 0)) {
		return 0;
	}
	
	if (likely(n > 6)) {
		size_t m;
		
		n -= (4 - (unsigned long)dest) & 0x03;
		m = n >> 2;
		
		asm volatile (
		/*
		 * Setup for the jump table.
		 * d15 = ((-m) & (32 - 1)) * 2 instructions per entry
		 * This code relies on the knowledge that there are 18 instructions
		 * from the lea.4 to the label "1f"
		 */
		"	sub.4		d15, #0, %[m]			\n\t"
		"	and.4		d15, #(32-1), d15		\n\t"
		"	lsl.4		d15, d15, #1			\n\t"
		"	call		%[aligned_start], .+4		\n\t"
		"	lea.4		%[aligned_start], (%[aligned_start],d15)\n\t"

		/*
		 * If (dest & 3) == 0, then our pointer is 32-bit aligned
		 */
		"	bfextu		d15, %[dest], #2		\n\t"
		"	jmpne.w.f	100f				\n\t"
		"	calli		%[aligned_start], 4*18(%[aligned_start])\n\t"

		/*
		 * Check for 16-bit aligned
		 */
		"100:	cmpi		d15, #2				\n\t"
		"	jmpne.s.f	101f				\n\t"
		"103:	move.2		(%[dest])2++, #0		\n\t"
		"	calli		%[aligned_start], 4*18(%[aligned_start])\n\t"

		/*
		 * Handle odd alignment (1-byte or 3-byte)
		 * The move.2 in this block is in the same page as the move.1
		 */
		"101:	move.1		(%[dest])1++, #0		\n\t"
		"	jmpgt.s.f	102f				\n\t"
		"	move.2		(%[dest])2++, #0		\n\t"
		"102:	calli		%[aligned_start], 4*18(%[aligned_start])\n\t"

		/*
		 * Post block copy leftover bytes.  Somewhere between 1 and 3.
		 * These moves are guaranteed to be in a page already accessed.
		 */
		"200:	cmpi		%[n], #2			\n\t"
		"	jmplt.s.f	201f				\n\t"
		"202:	move.2		(%[dest])2++, #0		\n\t"
		"	jmpeq.s.t	202f				\n\t"
		"201:	move.1		(%[dest])1++, #0		\n\t"
		"	jmpt.w.t	202f				\n\t"

		/*
		 * nop is inserted between each move to avoid the situation where the
		 * write buffer can't keep up and we take a hazard.
		 */
		"1:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"2:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"3:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"4:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"5:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"6:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"7:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"8:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"9:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"10:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"11:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"12:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"13:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"14:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"15:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"16:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"17:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"18:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"19:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"20:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"21:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"22:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"23:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"24:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
 		"25:	movea		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
		"26:	move.4		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
		"27:	move.4		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
		"28:	move.4		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
		"29:	move.4		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
		"30:	move.4		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
		"31:	move.4		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
		"32:	move.4		(%[dest])4++, #0		\n\t"
		"	nop						\n\t"
		"	add.4		%[m], #-32, %[m]		\n\t"
		"	jmpgt.w.f	1b				\n\t"

		/*
		 * Block copy completed.  Determine if there are any left over
		 * bytes that still need to be copied.
		 */
		"	and.4		%[n], #3, %[n]			\n\t"
		"	jmpne.w.f	200b				\n\t"
		"202:							\n\t"

		/*
		 * The ex_table contains pairs of addresses.  The first
		 * address is the location of the potential fault.  The
		 * second address is the location of the handler after a
		 * fault occurs.
		 */
		"	.section __ex_table, \"ax\"			\n\t"
		"	.word		1b				\n\t"
		"	.word		202b				\n\t"
		"	.word		2b				\n\t"
		"	.word		202b				\n\t"
		"	.word		3b				\n\t"
		"	.word		202b				\n\t"
		"	.word		4b				\n\t"
		"	.word		202b				\n\t"
		"	.word		5b				\n\t"
		"	.word		202b				\n\t"
		"	.word		6b				\n\t"
		"	.word		202b				\n\t"
		"	.word		7b				\n\t"
		"	.word		202b				\n\t"
		"	.word		8b				\n\t"
		"	.word		202b				\n\t"
		"	.word		9b				\n\t"
		"	.word		202b				\n\t"
		"	.word		10b				\n\t"
		"	.word		202b				\n\t"
		"	.word		11b				\n\t"
		"	.word		202b				\n\t"
		"	.word		12b				\n\t"
		"	.word		202b				\n\t"
		"	.word		13b				\n\t"
		"	.word		202b				\n\t"
		"	.word		14b				\n\t"
		"	.word		202b				\n\t"
		"	.word		15b				\n\t"
		"	.word		202b				\n\t"
		"	.word		16b				\n\t"
		"	.word		202b				\n\t"
		"	.word		17b				\n\t"
		"	.word		202b				\n\t"
		"	.word		18b				\n\t"
		"	.word		202b				\n\t"
		"	.word		19b				\n\t"
		"	.word		202b				\n\t"
		"	.word		20b				\n\t"
		"	.word		202b				\n\t"
		"	.word		21b				\n\t"
		"	.word		202b				\n\t"
		"	.word		22b				\n\t"
		"	.word		202b				\n\t"
		"	.word		23b				\n\t"
		"	.word		202b				\n\t"
		"	.word		24b				\n\t"
		"	.word		202b				\n\t"
		"	.word		25b				\n\t"
		"	.word		202b				\n\t"
		"	.word		26b				\n\t"
		"	.word		202b				\n\t"
		"	.word		27b				\n\t"
		"	.word		202b				\n\t"
		"	.word		28b				\n\t"
		"	.word		202b				\n\t"
		"	.word		29b				\n\t"
		"	.word		202b				\n\t"
		"	.word		30b				\n\t"
		"	.word		202b				\n\t"
		"	.word		31b				\n\t"
		"	.word		202b				\n\t"
		"	.word		32b				\n\t"
		"	.word		202b				\n\t"
		"	.word		101b				\n\t"
		"	.word		202b				\n\t"
		"	.word		103b				\n\t"
		"	.word		202b				\n\t"
		"	.previous					\n\t"
			: [dest] "+a" (dest), [m] "+d" (m), 
			  [n] "+d" (n), [aligned_start] "=a" (aligned_start)
			:
			: "d15", "memory", "cc"
		);

		/*
		 * We return the number of bytes that have not been copied.
		 */
		return size - ((unsigned long) dest - (unsigned long) dest_orig);
	}

	asm volatile (
	/*
	 * Setup for the jump table.
	 * d15 = ((-m) & (16 - 1)) * 2 instructions per entry
	 * This code relies on the knowledge that there are 2 instructions
	 * from the lea.4 to the label "1f"
	 */
	"	sub.4		d15, #0, %[n]			\n\t"
	"	jmpeq.w.f	202f				\n\t"
	"	and.4		d15, #(16-1), d15		\n\t"
	"	call		%[aligned_start], .+4		\n\t"
	"	lea.4		%[aligned_start], (%[aligned_start],d15)\n\t"
	"	calli		%[aligned_start], 8(%[aligned_start])\n\t"

	"1:	move.1		(%[dest])1++, #0		\n\t"
	"2:	move.1		(%[dest])1++, #0		\n\t"
	"3:	move.1		(%[dest])1++, #0		\n\t"
	"4:	move.1		(%[dest])1++, #0		\n\t"
	"5:	move.1		(%[dest])1++, #0		\n\t"
	"6:	move.1		(%[dest])1++, #0		\n\t"
	"7:	move.1		(%[dest])1++, #0		\n\t"
	"8:	move.1		(%[dest])1++, #0		\n\t"
	"9:	move.1		(%[dest])1++, #0		\n\t"
	"10:	move.1		(%[dest])1++, #0		\n\t"
	"11:	move.1		(%[dest])1++, #0		\n\t"
	"12:	move.1		(%[dest])1++, #0		\n\t"
	"13:	move.1		(%[dest])1++, #0		\n\t"
	"14:	move.1		(%[dest])1++, #0		\n\t"
	"15:	move.1		(%[dest])1++, #0		\n\t"
	"16:	move.1		(%[dest])1++, #0		\n\t"
	"	add.4		%[n], #-16, %[n]		\n\t"
	"	jmpgt.w.f	1b				\n\t"
	"202:							\n\t"

	/*
	 * The ex_table contains pairs of addresses.  The first
	 * address is the location of the potential fault.  The
	 * second address is the location of the handler after a
	 * fault occurs.
	 */
	"	.section __ex_table, \"ax\"			\n\t"
	"	.word		1b				\n\t"
	"	.word		202b				\n\t"
	"	.word		2b				\n\t"
	"	.word		202b				\n\t"
	"	.word		3b				\n\t"
	"	.word		202b				\n\t"
	"	.word		4b				\n\t"
	"	.word		202b				\n\t"
	"	.word		5b				\n\t"
	"	.word		202b				\n\t"
	"	.word		6b				\n\t"
	"	.word		202b				\n\t"
	"	.word		7b				\n\t"
	"	.word		202b				\n\t"
	"	.word		8b				\n\t"
	"	.word		202b				\n\t"
	"	.word		9b				\n\t"
	"	.word		202b				\n\t"
	"	.word		10b				\n\t"
	"	.word		202b				\n\t"
	"	.word		11b				\n\t"
	"	.word		202b				\n\t"
	"	.word		12b				\n\t"
	"	.word		202b				\n\t"
	"	.word		13b				\n\t"
	"	.word		202b				\n\t"
	"	.word		14b				\n\t"
	"	.word		202b				\n\t"
	"	.word		15b				\n\t"
	"	.word		202b				\n\t"
	"	.word		16b				\n\t"
	"	.word		202b				\n\t"
	"	.previous					\n\t"
		: [dest] "+a" (dest), [n] "+d" (n), 
		  [aligned_start] "=a" (aligned_start)
		:
		: "d15", "memory", "cc"
	);

	/*
	 * We return the number of bytes that have not been copied.
	 */
	return size - ((unsigned long) dest - (unsigned long) dest_orig);
}
EXPORT_SYMBOL(__clear_user);

/*
 * __strnlen_user()
 *	Determine the length of a string in user memory.
 *
 * Returns number characters in the string including the NULL at the
 * end.  Returns 0 if it fails.
 */
long __strnlen_user(const __user char *src, long max_n)
{
	long n = 0;
	long retval = 0;
	long tmp;

	if (max_n == 0) {
		return 0;
	}
	
	max_n--;

	asm volatile (
	";                                        		\n\t"
	"; Basic loop to determine length of string.		\n\t"
	";                                        		\n\t"
	"1:	lsl.1	%[tmp], (%[sptr])1++, #0		\n\t"
	"	jmpeq.f	2f					\n\t"
	"	add.4	%[cnt], #1, %[cnt]			\n\t"
	"	sub.4	%[tmp], %[max_n], %[cnt]		\n\t"
	"	jmpne.t	1b					\n\t"
	"	nop						\n\t"
	"2:	add.4	%[ret], #1, %[cnt]			\n\t"
	"3:							\n\t"
	";                                        		\n\t"
	"; The ex_table contains pairs of addresses.  The first	\n\t"
	"; address is the location of the potential fault the  	\n\t"
	"; second address is the new location to jump to.      	\n\t"
	";                                        		\n\t"
	"	.section __ex_table, \"ax\"			\n\t"
	"	.word	1b                      		\n\t"
	"	.word	3b                      		\n\t"
	"	.previous	                      		\n\t"
	: [tmp] "=&d" (tmp), [sptr] "+a" (src), [cnt] "+&d" (n),
	  [ret] "+r" (retval) 
	: [max_n] "r" (max_n)
	: "memory", "cc"
	);

	return retval;
}
EXPORT_SYMBOL(__strnlen_user);

/*
 * __strncpy_from_user()
 *	Copy from string in user space to a buffer in kernel space.
 *
 * Returns number of non-zero characters copied or -EFAULT if protection 
 * error when accessing user space.
 */
long __strncpy_from_user(char *dst, const __user char *src, long count)
{
	long orig_count = count;
	long retval = -EFAULT;
	long zero = 0;

	if (count == 0) {
		return 0;
	}
	
	/*
	 * The following implements the strncpy() function.  It is
	 * very important to note the constraints below are NOT
	 * arbitrary.  "+&d" constraint for the variable "count" is
	 * an absolute necessity.  It indicates that its value is
	 * changing as the assembly code is executing.  Failure to
	 * use the "&" in the constraint WILL produce the wrong
	 * code.
	 */
	asm volatile (
	";                                        		\n\t"
	"; Basic loop to copy 1 byte at a time.			\n\t"
	";                                        		\n\t"
	"1:	add.1	(%[dptr])1++, (%[sptr])1++, %[zero]	\n\t"
	"	jmpeq.f	2f					\n\t"
	"	add.4	%[cnt], #-1, %[cnt]			\n\t"
	"	jmpne.t	1b					\n\t"
	"2:	sub.4	%[ret], %[len], %[cnt]			\n\t"
	"3:							\n\t"
	";                                        		\n\t"
	"; The ex_table contains pairs of addresses.  The first	\n\t"
	"; address is the location of the potential fault the  	\n\t"
	"; second address is the new location to jump to.      	\n\t"
	";                                        		\n\t"
	"	.section __ex_table, \"ax\"			\n\t"
	"	.word	1b                      		\n\t"
	"	.word	3b                      		\n\t"
	"	.previous	                      		\n\t"
	: [dptr] "+a" (dst), [sptr] "+a" (src), [cnt] "+&d" (count),
	  [ret] "+r" (retval)
	: [len] "r" (orig_count), [zero] "d" (zero)
	: "memory", "cc"
	);

	return retval;
}
EXPORT_SYMBOL(__strncpy_from_user);

/*
 * __copy_user()
 *	Copy the data either to or from user space.
 *
 * Return the number of bytes NOT copied.
 */
unsigned long __copy_user(void *dest, const void *src, unsigned long size)
{
	void *dest_orig = dest;
	void *aligned_start;
	size_t n = size;

	if (likely((((unsigned long)dest ^ (unsigned long)src) & 3) == 0) && likely(n > 6)) {
		size_t m;
		
		n -= (4 - (unsigned long)dest) & 0x03;
		m = n >> 2;
		
		asm volatile (
		/*
		 * Setup for the jump table.
		 * d15 = ((-m) & (32 - 1)) * 2 instructions per entry
		 * This code relies on the knowledge that there are 18 instructions
		 * from the lea.4 to the label "1f"
		 */
		"	sub.4		d15, #0, %[m]			\n\t"
		"	and.4		d15, #(32-1), d15		\n\t"
		"	lsl.4		d15, d15, #1			\n\t"
		"	call		%[aligned_start], .+4		\n\t"
		"	lea.4		%[aligned_start], (%[aligned_start],d15)\n\t"

		/*
		 * If (dest & 3) == 0, then our pointers are 32-bit aligned
		 */
		"	bfextu		d15, %[dest], #2		\n\t"
		"	jmpne.w.f	100f				\n\t"
		"	calli		%[aligned_start], 4*18(%[aligned_start])\n\t"

		/*
		 * Check for 16-bit aligned
		 */
		"100:	cmpi		d15, #2				\n\t"
		"	jmpne.s.f	101f				\n\t"
		"103:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	calli		%[aligned_start], 4*18(%[aligned_start])\n\t"

		/*
		 * Handle odd alignment (1-byte or 3-byte)
		 * The move.2 in this block is in the same page as the move.1
		 */
		"101:	move.1		(%[dest])1++, (%[src])1++	\n\t"
		"	jmpgt.s.f	102f				\n\t"
		"	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"102:	calli		%[aligned_start], 4*18(%[aligned_start])\n\t"

		/*
		 * Post block copy leftover bytes.  Somewhere between 1 and 3.
		 * These moves are guaranteed to be in a page already accessed.
		 */
		"200:	cmpi		%[n], #2			\n\t"
		"	jmplt.s.f	201f				\n\t"
		"202:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	jmpeq.s.t	202f				\n\t"
		"201:	move.1		(%[dest])1++, (%[src])1++	\n\t"
		"	jmpt.w.t	202f				\n\t"

		/*
		 * nop is inserted between each move to avoid the situation where the
		 * write buffer to the DDR can't keep up and we take a hazard.
		 */
		"1:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"2:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"3:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"4:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"5:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"6:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"7:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"8:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"9:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"10:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"11:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"12:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"13:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"14:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"15:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"16:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"17:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"18:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"19:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"20:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"21:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"22:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"23:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"24:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
 		"25:	movea		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
		"26:	move.4		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
		"27:	move.4		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
		"28:	move.4		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
		"29:	move.4		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
		"30:	move.4		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
		"31:	move.4		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
		"32:	move.4		(%[dest])4++, (%[src])4++	\n\t"
		"	nop						\n\t"
		"	add.4		%[m], #-32, %[m]		\n\t"
		"	jmpgt.w.f	1b				\n\t"

		/*
		 * Block copy completed.  Determine if there are any left over
		 * bytes that still need to be copied.
		 */
		"	and.4		%[n], #3, %[n]			\n\t"
		"	jmpne.w.f	200b				\n\t"
		"202:							\n\t"

		/*
		 * The ex_table contains pairs of addresses.  The first
		 * address is the location of the potential fault.  The
		 * second address is the location of the handler after a
		 * fault occurs.
		 */
		"	.section __ex_table, \"ax\"			\n\t"
		"	.word		1b				\n\t"
		"	.word		202b				\n\t"
		"	.word		2b				\n\t"
		"	.word		202b				\n\t"
		"	.word		3b				\n\t"
		"	.word		202b				\n\t"
		"	.word		4b				\n\t"
		"	.word		202b				\n\t"
		"	.word		5b				\n\t"
		"	.word		202b				\n\t"
		"	.word		6b				\n\t"
		"	.word		202b				\n\t"
		"	.word		7b				\n\t"
		"	.word		202b				\n\t"
		"	.word		8b				\n\t"
		"	.word		202b				\n\t"
		"	.word		9b				\n\t"
		"	.word		202b				\n\t"
		"	.word		10b				\n\t"
		"	.word		202b				\n\t"
		"	.word		11b				\n\t"
		"	.word		202b				\n\t"
		"	.word		12b				\n\t"
		"	.word		202b				\n\t"
		"	.word		13b				\n\t"
		"	.word		202b				\n\t"
		"	.word		14b				\n\t"
		"	.word		202b				\n\t"
		"	.word		15b				\n\t"
		"	.word		202b				\n\t"
		"	.word		16b				\n\t"
		"	.word		202b				\n\t"
		"	.word		17b				\n\t"
		"	.word		202b				\n\t"
		"	.word		18b				\n\t"
		"	.word		202b				\n\t"
		"	.word		19b				\n\t"
		"	.word		202b				\n\t"
		"	.word		20b				\n\t"
		"	.word		202b				\n\t"
		"	.word		21b				\n\t"
		"	.word		202b				\n\t"
		"	.word		22b				\n\t"
		"	.word		202b				\n\t"
		"	.word		23b				\n\t"
		"	.word		202b				\n\t"
		"	.word		24b				\n\t"
		"	.word		202b				\n\t"
		"	.word		25b				\n\t"
		"	.word		202b				\n\t"
		"	.word		26b				\n\t"
		"	.word		202b				\n\t"
		"	.word		27b				\n\t"
		"	.word		202b				\n\t"
		"	.word		28b				\n\t"
		"	.word		202b				\n\t"
		"	.word		29b				\n\t"
		"	.word		202b				\n\t"
		"	.word		30b				\n\t"
		"	.word		202b				\n\t"
		"	.word		31b				\n\t"
		"	.word		202b				\n\t"
		"	.word		32b				\n\t"
		"	.word		202b				\n\t"
		"	.word		101b				\n\t"
		"	.word		202b				\n\t"
		"	.word		103b				\n\t"
		"	.word		202b				\n\t"
		"	.previous					\n\t"
			: [dest] "+a" (dest), [src] "+a" (src), [m] "+d" (m), 
			  [n] "+d" (n), [aligned_start] "=a" (aligned_start)
			:
			: "d15", "memory", "cc"
		);

		/*
		 * We return the number of bytes that have not been copied.
		 */
		return size - ((unsigned long) dest - (unsigned long) dest_orig);
	}

	if (likely((((unsigned long)dest ^ (unsigned long)src) & 1) == 0) && likely(n > 2)) {
		size_t m;
		n -= (unsigned long)dest & 0x01;
		m = n >> 1;
		asm volatile (
		/*
		 * Setup for the jump table.
		 * d15 = ((-m) & (32 - 1)) * 2 instructions per entry
		 * This code relies on the knowledge that there are 8 instructions
		 * from the lea.4 to the label "1f"
		 */
		"	sub.4		d15, #0, %[m]			\n\t"
		"	and.4		d15, #(32-1), d15		\n\t"
		"	lsl.4		d15, d15, #1			\n\t"
		"	call		%[aligned_start], .+4		\n\t"
		"	lea.4		%[aligned_start], (%[aligned_start],d15)\n\t"

		/*
		 * Are the pointers 16-bit aligned?
		 */
		"	btst		%[dest], #0			\n\t"
		"	jmpne.w.f	100f				\n\t"
		"	calli		%[aligned_start], 4*8(%[aligned_start])	\n\t"

		/*
		 * Handle odd alignment.
		 */
		"100:	move.1		(%[dest])1++, (%[src])1++	\n\t"
		"	calli		%[aligned_start], 4*8(%[aligned_start])\n\t"

		/*
		 * Post block copy leftover byte.
		 * This move is guaranteed to be in a page already accessed.
		 */
		"200:	move.1		(%[dest])1++, (%[src])1++	\n\t"
		"	jmpt.w.t	202f				\n\t"

		/*
		 * nop is inserted between each move to avoid the situation where the
		 * write buffer to the DDR can't keep up and we take a hazard.
		 */
		"1:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"2:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"3:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"4:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"5:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"6:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"7:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"8:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"9:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"10:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"11:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"12:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"13:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"14:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"15:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"16:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"17:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"18:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"19:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"20:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"21:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"22:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"23:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"24:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
 		"25:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
		"26:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
		"27:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
		"28:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
		"29:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
		"30:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
		"31:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
		"32:	move.2		(%[dest])2++, (%[src])2++	\n\t"
		"	nop						\n\t"
		"	add.4		%[m], #-32, %[m]		\n\t"
		"	jmpgt.w.f	1b			\n\t"

		/*
		 * Block copy completed.  Determine if there is a left over
		 * byte that still need to be copied.
		 */
		"	and.4		%[n], #1, %[n]		\n\t"
		"	jmpne.w.f	200b			\n\t"
		"202:						\n\t"

		/*
		 * The ex_table contains pairs of addresses.  The first
		 * address is the location of the potential fault.  The
		 * second address is the location of the handler after a
		 * fault occurs.
		 */
		"	.section __ex_table, \"ax\"			\n\t"
		"	.word		1b				\n\t"
		"	.word		202b				\n\t"
		"	.word		2b				\n\t"
		"	.word		202b				\n\t"
		"	.word		3b				\n\t"
		"	.word		202b				\n\t"
		"	.word		4b				\n\t"
		"	.word		202b				\n\t"
		"	.word		5b				\n\t"
		"	.word		202b				\n\t"
		"	.word		6b				\n\t"
		"	.word		202b				\n\t"
		"	.word		7b				\n\t"
		"	.word		202b				\n\t"
		"	.word		8b				\n\t"
		"	.word		202b				\n\t"
		"	.word		9b				\n\t"
		"	.word		202b				\n\t"
		"	.word		10b				\n\t"
		"	.word		202b				\n\t"
		"	.word		11b				\n\t"
		"	.word		202b				\n\t"
		"	.word		12b				\n\t"
		"	.word		202b				\n\t"
		"	.word		13b				\n\t"
		"	.word		202b				\n\t"
		"	.word		14b				\n\t"
		"	.word		202b				\n\t"
		"	.word		15b				\n\t"
		"	.word		202b				\n\t"
		"	.word		16b				\n\t"
		"	.word		202b				\n\t"
		"	.word		17b				\n\t"
		"	.word		202b				\n\t"
		"	.word		18b				\n\t"
		"	.word		202b				\n\t"
		"	.word		19b				\n\t"
		"	.word		202b				\n\t"
		"	.word		20b				\n\t"
		"	.word		202b				\n\t"
		"	.word		21b				\n\t"
		"	.word		202b				\n\t"
		"	.word		22b				\n\t"
		"	.word		202b				\n\t"
		"	.word		23b				\n\t"
		"	.word		202b				\n\t"
		"	.word		24b				\n\t"
		"	.word		202b				\n\t"
		"	.word		25b				\n\t"
		"	.word		202b				\n\t"
		"	.word		26b				\n\t"
		"	.word		202b				\n\t"
		"	.word		27b				\n\t"
		"	.word		202b				\n\t"
		"	.word		28b				\n\t"
		"	.word		202b				\n\t"
		"	.word		29b				\n\t"
		"	.word		202b				\n\t"
		"	.word		30b				\n\t"
		"	.word		202b				\n\t"
		"	.word		31b				\n\t"
		"	.word		202b				\n\t"
		"	.word		32b				\n\t"
		"	.word		202b				\n\t"
		"	.word		100b				\n\t"
		"	.word		202b				\n\t"
		"	.previous					\n\t"
			: [dest] "+a" (dest), [src] "+a" (src), [m] "+d" (m), 
			  [n] "+d" (n), [aligned_start] "=a" (aligned_start)
			:
			: "d15", "memory", "cc"
		);

		/*
		 * We return the number of bytes that have not been copied.
		 */
		return size - ((unsigned long) dest - (unsigned long) dest_orig);
	}

	asm volatile (
	/*
	 * Setup for the jump table.
	 * d15 = ((-m) & (16 - 1)) * 2 instructions per entry
	 * This code relies on the knowledge that there are 2 instructions
	 * from the lea.4 to the label "1f"
	 */
	"	sub.4		d15, #0, %[n]			\n\t"
	"	jmpeq.w.f	202f				\n\t"
	"	and.4		d15, #(16-1), d15		\n\t"
	"	call		%[aligned_start], .+4		\n\t"
	"	lea.4		%[aligned_start], (%[aligned_start],d15)\n\t"
	"	calli		%[aligned_start], 8(%[aligned_start])\n\t"

	"1:	move.1		(%[dest])1++, (%[src])1++	\n\t"
	"2:	move.1		(%[dest])1++, (%[src])1++	\n\t"
	"3:	move.1		(%[dest])1++, (%[src])1++	\n\t"
	"4:	move.1		(%[dest])1++, (%[src])1++	\n\t"
	"5:	move.1		(%[dest])1++, (%[src])1++	\n\t"
	"6:	move.1		(%[dest])1++, (%[src])1++	\n\t"
	"7:	move.1		(%[dest])1++, (%[src])1++	\n\t"
	"8:	move.1		(%[dest])1++, (%[src])1++	\n\t"
	"9:	move.1		(%[dest])1++, (%[src])1++	\n\t"
	"10:	move.1		(%[dest])1++, (%[src])1++	\n\t"
	"11:	move.1		(%[dest])1++, (%[src])1++	\n\t"
	"12:	move.1		(%[dest])1++, (%[src])1++	\n\t"
	"13:	move.1		(%[dest])1++, (%[src])1++	\n\t"
	"14:	move.1		(%[dest])1++, (%[src])1++	\n\t"
	"15:	move.1		(%[dest])1++, (%[src])1++	\n\t"
	"16:	move.1		(%[dest])1++, (%[src])1++	\n\t"
	"	add.4		%[n], #-16, %[n]		\n\t"
	"	jmpgt.w.f	1b				\n\t"
	"202:							\n\t"

	/*
	 * The ex_table contains pairs of addresses.  The first
	 * address is the location of the potential fault.  The
	 * second address is the location of the handler after a
	 * fault occurs.
	 */
	"	.section __ex_table, \"ax\"			\n\t"
	"	.word		1b				\n\t"
	"	.word		202b				\n\t"
	"	.word		2b				\n\t"
	"	.word		202b				\n\t"
	"	.word		3b				\n\t"
	"	.word		202b				\n\t"
	"	.word		4b				\n\t"
	"	.word		202b				\n\t"
	"	.word		5b				\n\t"
	"	.word		202b				\n\t"
	"	.word		6b				\n\t"
	"	.word		202b				\n\t"
	"	.word		7b				\n\t"
	"	.word		202b				\n\t"
	"	.word		8b				\n\t"
	"	.word		202b				\n\t"
	"	.word		9b				\n\t"
	"	.word		202b				\n\t"
	"	.word		10b				\n\t"
	"	.word		202b				\n\t"
	"	.word		11b				\n\t"
	"	.word		202b				\n\t"
	"	.word		12b				\n\t"
	"	.word		202b				\n\t"
	"	.word		13b				\n\t"
	"	.word		202b				\n\t"
	"	.word		14b				\n\t"
	"	.word		202b				\n\t"
	"	.word		15b				\n\t"
	"	.word		202b				\n\t"
	"	.word		16b				\n\t"
	"	.word		202b				\n\t"
	"	.previous					\n\t"
		: [dest] "+a" (dest), [src] "+a" (src), [n] "+d" (n), 
		  [aligned_start] "=a" (aligned_start)
		:
		: "d15", "memory", "cc"
	);

	/*
	 * We return the number of bytes that have not been copied.
	 */
	return size - ((unsigned long) dest - (unsigned long) dest_orig);
}
EXPORT_SYMBOL(__copy_user);

/*
 * __copy_from_user_zeroing()
 *	Copy the data from user space zeroing if failure.
 *
 * Return the number of bytes NOT copied.
 */
unsigned long __copy_from_user_zeroing(void *to, const void *from, 
				       unsigned long size)
{
	unsigned long remaining;
	
	remaining = __copy_user(to, from, size);
	
	if (remaining > 0) {
		memset((char *) to + (size - remaining), 0, remaining);
	}

	return remaining;
}
EXPORT_SYMBOL(__copy_from_user_zeroing);

/*
 * memcpy()
 */
void *memcpy(void *dest, const void *src, size_t n)
{
	void *dest_ret = dest;
	void *aligned_start;

	/*
	 * Standard Linux code occasionally likes to call memcpy() with src and dest
	 * pointed at the same place.  One specific place this happens is skbuff.c:778.
	 * This is the second place to do this in Linux and there may be more hidden
	 * and waiting to trigger the BUG_ON().  Instead we will check for this
	 * condition and return safely.  No copying needs to be done if dest and src
	 * are the same.
	 */
	if (src == dest)
		return dest_ret;
	
	BUG_ON((src < (dest + n)) && (dest < (src + n)));

	if (likely((((unsigned long)dest ^ (unsigned long)src) & 3) == 0) && likely(n > 6)) {
		size_t m;
		n -= (4 - (unsigned long)dest) & 0x03;
		m = n >> 2;
		asm volatile (
		"	sub.4		d15, #0, %2		\n\t"	// set up for jump table
		"	and.4		d15, #(32-1), d15	\n\t"	// d15 = (-m) & (32 - 1)
		"	lsl.4		d15, d15, #1		\n\t"	// 2 instructions per word
		"	call		%4, .+4			\n\t"
		"	lea.4		%4, (%4,d15)		\n\t"	// 18 inst from here to 1f

		"	bfextu		d15, %0, #2		\n\t"	// d15 = (dest & 3)
		"	jmpne.w.f	100f			\n\t"
		"	calli		%4, 4*18(%4)		\n\t"	// 4-byte alignment

		"100:	cmpi		d15, #2			\n\t"
		"	jmpne.s.f	101f			\n\t"
		"	move.2		(%0)2++, (%1)2++	\n\t"
		"	calli		%4, 4*18(%4)		\n\t"	// 2-byte alignment

		"101:	move.1		(%0)1++, (%1)1++	\n\t"
		"	jmpgt.s.f	102f			\n\t"	// 3-byte alignment
		"	move.2		(%0)2++, (%1)2++	\n\t"	// 1-byte alignment
		"102:	calli		%4, 4*18(%4)		\n\t"

		"200:	cmpi		%3, #2			\n\t"
		"	jmplt.s.f	201f			\n\t"
		"	move.2		(%0)2++, (%1)2++	\n\t"
		"	jmpeq.s.t	2f			\n\t"
		"201:	move.1		(%0)1++, (%1)1++	\n\t"
		"	jmpt.w.t	2f			\n\t"

		/*
		 * nop is inserted between each move to avoid the situation where the
		 * write buffer to the DDR can't keep up and we take a hazard.
		 */
		"1:	.rept	"D(33 - (CACHE_LINE_SIZE/4))"	\n\t"
 		"	movea		(%0)4++, (%1)4++	\n\t"
		"	nop					\n\t"
		"	.endr					\n\t"
		"	.rept	"D((CACHE_LINE_SIZE/4) - 1)"	\n\t"
		"	move.4		(%0)4++, (%1)4++	\n\t"
		"	nop					\n\t"
		"	.endr					\n\t"
		"	add.4		%2, #-32, %2		\n\t"
		"	jmpgt.w.f	1b			\n\t"

		"	and.4		%3, #3, %3		\n\t"	// check n
		"	jmpne.w.f	200b			\n\t"
		"2:						\n\t"
			: "+a" (dest), "+a" (src), "+d" (m), "+d" (n), "=a" (aligned_start)
			:
			: "d15", "memory", "cc"
		);

		return dest_ret;
	}

	if (likely((((unsigned long)dest ^ (unsigned long)src) & 1) == 0) && likely(n > 2)) {
		size_t m;
		n -= (unsigned long)dest & 0x01;
		m = n >> 1;
		asm volatile (
		"	sub.4		d15, #0, %2		\n\t"	// set up for jump table
		"	and.4		d15, #(32-1), d15	\n\t"	// d15 = (-m) & (32 - 1)
		"	lsl.4		d15, d15, #1		\n\t"	// 2 instructions per word
		"	call		%4, .+4			\n\t"
		"	lea.4		%4, (%4,d15)		\n\t"	// 8 inst from here to 1f

		"	btst		%0, #0			\n\t"	// check bit 0
		"	jmpne.w.f	100f			\n\t"
		"	calli		%4, 4*8(%4)		\n\t"	// 4-byte alignment

		"100:	move.1		(%0)1++, (%1)1++	\n\t"
		"	calli		%4, 4*8(%4)		\n\t"

		"200:	move.1		(%0)1++, (%1)1++	\n\t"
		"	jmpt.w.t	2f			\n\t"

		/*
		 * nop is inserted between each move to avoid the situation where the
		 * write buffer to the DDR can't keep up and we take a hazard.
		 */
		"1:	.rept		32			\n\t"
		"	move.2		(%0)2++, (%1)2++	\n\t"
		"	nop					\n\t"
		"	.endr					\n\t"
		"	add.4		%2, #-32, %2		\n\t"
		"	jmpgt.w.f	1b			\n\t"

		"	and.4		%3, #1, %3		\n\t"	// check n
		"	jmpne.w.f	200b			\n\t"
		"2:						\n\t"
			: "+a" (dest), "+a" (src), "+d" (m), "+d" (n), "=a" (aligned_start)
			:
			: "d15", "memory", "cc"
		);

		return dest_ret;
	}

	asm volatile (
	"	sub.4		d15, #0, %2		\n\t"
	"	jmpeq.w.f	2f			\n\t"
	"	and.4		d15, #(16-1), d15	\n\t"	// d15 = (-n) & (16 - 1)
	"	call		%3, .+4			\n\t"
	"	lea.4		%3, (%3,d15)		\n\t"	// 2 inst from here to 1f
	"	calli		%3, 8(%3)		\n\t"

	"1:	.rept		16			\n\t"
	"	move.1		(%0)1++, (%1)1++	\n\t"
	"	.endr					\n\t"
	"	add.4		%2, #-16, %2		\n\t"
	"	jmpgt.w.f	1b			\n\t"
	"2:						\n\t"
		: "+a" (dest), "+a" (src), "+d" (n), "=a" (aligned_start)
		:
		: "d15", "memory", "cc"
	);

	return dest_ret;
}

/*
 * memset()
 */
void *memset(void *s, int c, size_t n)
{
	void *s_ret = s;
	void *aligned_start;

	if (likely(n > 6)) {
		size_t m;
		n -= (4 - (unsigned long)s) & 0x03;
		m = n >> 2;
		asm volatile (
		"	sub.4		d15, #0, %2		\n\t"	// set up for jump table
		"	and.4		d15, #(32-1), d15	\n\t"	// d15 = (-m) & (32 - 1)
		"	shmrg.1		%1, %1, %1		\n\t"
		"	shmrg.2		%1, %1, %1		\n\t"	// %1 = (c<<24)|(c<<16)|(c<<8)|c
		"	call		%4, .+4			\n\t"
		"	lea.4		%4, (%4,d15)		\n\t"	// 18 inst from here to 1f

		"	bfextu		d15, %0, #2		\n\t"	// d15 = (s & 3)
		"	jmpne.w.f	100f			\n\t"
		"	calli		%4, 4*18(%4)		\n\t"	// 4-byte alignment

		"100:	cmpi		d15, #2			\n\t"
		"	jmpne.s.f	101f			\n\t"
		"	move.2		(%0)2++, %1		\n\t"
		"	calli		%4, 4*18(%4)		\n\t"	// 2-byte alignment

		"101:	move.1		(%0)1++, %1		\n\t"
		"	jmpgt.s.f	102f			\n\t"	// 3-byte alignment
		"	move.2		(%0)2++, %1		\n\t"	// 1-byte alignment
		"102:	calli		%4, 4*18(%4)		\n\t"

		"200:	cmpi		%3, #2			\n\t"
		"	jmplt.s.f	201f			\n\t"
		"	move.2		(%0)2++, %1		\n\t"
		"	jmpeq.s.t	2f			\n\t"
		"201:	move.1		(%0)1++, %1		\n\t"
		"	jmpt.w.t	2f			\n\t"

		"1:	.rept	"D(33 - (CACHE_LINE_SIZE/4))"	\n\t"
		"	movea		(%0)4++, %1		\n\t"
		"	.endr					\n\t"
		"	.rept	"D((CACHE_LINE_SIZE/4) - 1)"	\n\t"
		"	move.4		(%0)4++, %1		\n\t"
		"	.endr					\n\t"
		"	add.4		%2, #-32, %2		\n\t"
		"	jmpgt.w.f	1b			\n\t"

		"	and.4		%3, #3, %3		\n\t"	// test bit 1 of n
		"	jmpne.w.f	200b			\n\t"
		"2:						\n\t"
			: "+a" (s), "+d" (c), "+d" (m), "+d" (n), "=a" (aligned_start)
			:
			: "d15", "memory", "cc"
		);

		return s_ret;
	}

	asm volatile (
	"	sub.4		d15, #0, %2		\n\t"
	"	jmpeq.w.f	2f			\n\t"
	"	and.4		d15, #(8-1), d15	\n\t"	// d15 = (-%2) & (16 - 1)
	"	call		%3, .+4			\n\t"
	"	lea.4		%3, (%3,d15)		\n\t"	// 2 inst from here to 1f
	"	calli		%3, 8(%3)		\n\t"

	"1:	.rept		8			\n\t"
	"	move.1		(%0)1++, %1		\n\t"
	"	.endr					\n\t"
	"2:						\n\t"
		: "+a" (s), "+d" (c), "+d" (n), "=a" (aligned_start)
		:
		: "d15", "memory", "cc"
	);

	return s_ret;
}

/*
 * memmove()
 */
void *memmove(void *dest, const void *src, size_t n)
{
	char *tmp;
	const char *s;
	void *aligned_start;

	if (unlikely(n == 0))
		return dest;

	if (((dest + n) <= src) || ((src + n) <= dest))
		return memcpy(dest, src, n);

	tmp = dest;
	s = src;

	/*
	 * Will perform 16-bit move if possible
	 */
	if (likely((((u32)dest | (u32)src | n) & 1) == 0)) {
		if (dest <= src) {
			asm volatile (
			"	sub.4		d15, #0, %2		\n\t"	// set up for jump table
			"	and.4		d15, #(32-2), d15	\n\t"	// d15 = (- count) & (32 - 2)
			"	call		%3, .+4			\n\t"
			"	lea.2		%3, (%3,d15)		\n\t"	// 2 inst from here to 1f
			"	calli		%3, 8(%3)		\n\t"

			"1:	.rept		16			\n\t"
			"	move.2		(%0)2++, (%1)2++	\n\t"
			"	.endr					\n\t"
			"	add.4		%2, #-32, %2		\n\t"
			"	jmpgt.w.f	1b			\n\t"
				: "+a" (tmp), "+a" (s), "+d" (n), "=a" (aligned_start)
				:
				: "d15", "memory", "cc"
			);
		} else {
			tmp += n;
			s += n;
			asm volatile (
			"	sub.4		d15, #0, %2		\n\t"	// set up for jump table
			"	and.4		d15, #(32-2), d15	\n\t"	// d15 = (- count) & (32 - 2)
			"	call		%3, .+4			\n\t"
			"	lea.2		%3, (%3,d15)		\n\t"	// 2 inst from here to 1f
			"	calli		%3, 8(%3)		\n\t"

			"1:	.rept		16			\n\t"
			"	move.2		-2(%0)++, -2(%1)++	\n\t"
			"	.endr					\n\t"
			"	add.4		%2, #-32, %2		\n\t"
			"	jmpgt.w.f	1b			\n\t"
				: "+a" (tmp), "+a" (s), "+d" (n), "=a" (aligned_start)
				:
				: "d15", "memory", "cc"
			);
		}
		return dest;
	}

	if (dest <= src) {
		asm volatile (
		"	sub.4		d15, #0, %2		\n\t"	// set up for jump table
		"	and.4		d15, #(16-1), d15	\n\t"	// d15 = (- count) & (16 - 1)
		"	call		%3, .+4			\n\t"
		"	lea.4		%3, (%3,d15)		\n\t"	// 2 inst from here to 1f
		"	calli		%3, 8(%3)		\n\t"

		"1:	.rept		16			\n\t"
		"	move.1		(%0)1++, (%1)1++	\n\t"
		"	.endr					\n\t"
		"	add.4		%2, #-16, %2		\n\t"
		"	jmpgt.w.f	1b			\n\t"
			: "+a" (tmp), "+a" (s), "+d" (n), "=a" (aligned_start)
			:
			: "d15", "memory", "cc"
		);
	} else {
		tmp += n;
		s += n;
		asm volatile (
		"	sub.4		d15, #0, %2		\n\t"	// set up for jump table
		"	and.4		d15, #(16-1), d15	\n\t"	// d15 = (- count) & (16 - 1)
		"	call		%3, .+4			\n\t"
		"	lea.4		%3, (%3,d15)		\n\t"	// 2 inst from here to 1f
		"	calli		%3, 8(%3)		\n\t"

		"1:	.rept		16			\n\t"
		"	move.1		-1(%0)++, -1(%1)++	\n\t"
		"	.endr					\n\t"
		"	add.4		%2, #-16, %2		\n\t"
		"	jmpgt.w.f	1b			\n\t"
			: "+a" (tmp), "+a" (s), "+d" (n), "=a" (aligned_start)
			:
			: "d15", "memory", "cc"
		);
	}
	return dest;
}

/*
 * strncpy()
 */
char *strncpy(char *dest, const char *src, size_t n)
{
	char *tmp = dest;

	if (n) {
		do {
			if ((*tmp++ = *src) != 0)
				src++;
		} while (--n);
	}
	return dest;
}
EXPORT_SYMBOL(strncpy);

