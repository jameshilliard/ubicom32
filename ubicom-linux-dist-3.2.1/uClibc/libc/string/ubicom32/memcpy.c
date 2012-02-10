/* Copy memory to memory until the specified number of bytes
   has been copied.  Overlap is NOT handled correctly.
   Copyright (C) 1991, 1997, 2003 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Torbjorn Granlund (tege@sics.se).

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <string.h>

typedef unsigned long addr_t;

libc_hidden_proto(memcpy)

void *memcpy (void *dest, const void *src, size_t n)
{
	void *dest_ret = dest;
	void *aligned_start;

	if (likely((((addr_t)dest ^ (addr_t)src) & 3) == 0) && likely(n > 6)) {
		size_t m;
		n -= (4 - (addr_t)dest) & 0x03;
		m = n >> 2;
		__asm__ volatile (
		"	sub.4		d15, #0, %2		\n\t"	// set up for jump table
		"	and.4		d15, #(32-1), d15	\n\t"	// d15 = (-m) & (32 - 1)
		"	lsl.4		d15, d15, #1		\n\t"	// We need 2 instructions per word
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

		"1:	.rept		25			\n\t"
		"	movea		(%0)4++, (%1)4++	\n\t"
		"	nop					\n\t"	// nop to avoid overfilling the write buffer!
		"	.endr					\n\t"
		"	.rept		7			\n\t"
		"	move.4		(%0)4++, (%1)4++	\n\t"
		"	nop					\n\t"	// nop to avoid overfilling the write buffer!
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

	if (likely((((addr_t)dest ^ (addr_t)src) & 1) == 0) && likely(n > 2)) {
		size_t m;
		n -= (addr_t)dest & 0x01;
		m = n >> 1;
		__asm__ volatile (
		"	sub.4		d15, #0, %2		\n\t"	// set up for jump table
		"	and.4		d15, #(32-1), d15	\n\t"	// d15 = (-m) & (32 - 1)
		"	lsl.4		d15, d15, #1		\n\t"	// We need 2 instructions per word
		"	call		%4, .+4			\n\t"
		"	lea.4		%4, (%4,d15)		\n\t"	// 8 inst from here to 1f

		"	btst		%0, #0			\n\t"	// check bit 0
		"	jmpne.w.f	100f			\n\t"
		"	calli		%4, 4*8(%4)		\n\t"	// 4-byte alignment

		"100:	move.1		(%0)1++, (%1)1++	\n\t"
		"	calli		%4, 4*8(%4)		\n\t"

		"200:	move.1		(%0)1++, (%1)1++	\n\t"
		"	jmpt.w.t	2f			\n\t"

		"1:	.rept		32			\n\t"
		"	move.2		(%0)2++, (%1)2++	\n\t"
		"	nop					\n\t"	// nop to avoid overfilling the write buffer!
		"	.endr					\n\t"
		"	add.4		%2, #-32, %2		\n\t"
		"	jmpgt.w.f	1b			\n\t"

		"	and.4		%3, #1, %3		\n\t"	// check n
		"	jmpne.w.f	200b			\n\t"
		"2:						\n\t"

			: "+a" (dest), "+a" (src), "+d" (m), "+d" (n), "=a"(aligned_start)
			:
			: "d15", "memory", "cc"
		);

		return dest_ret;
	}

	__asm__ volatile (
	"	sub.4		d15, #0, %2		\n\t"
	"	jmpeq.w.f	2f			\n\t"
	"	and.4		d15, #(16-1), d15	\n\t"	// d15 = (-n) & (16 - 1)
	"	lsl.4		d15, d15, #1		\n\t"	// We need 2 instructions per word
	"	call		%3, .+4			\n\t"
	"	lea.4		%3, (%3,d15)		\n\t"	// 2 inst from here to 1f
	"	calli		%3, 8(%3)		\n\t"

	"1:	.rept		16			\n\t"
	"	move.1		(%0)1++, (%1)1++	\n\t"
	"	nop					\n\t"	// nop to avoid overfilling the write buffer!
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
libc_hidden_weak(memcpy)

