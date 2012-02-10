/* Copyright (C) 1991, 1997, 2003 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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

libc_hidden_proto(memset)
void *memset (void *s, int c, size_t n)
{
	void *s_ret = s;
	void *aligned_start;

	if (likely(n > 6)) {
		size_t m;
		n -= (4 - (addr_t)s) & 0x03;
		m = n >> 2;
		__asm__ volatile (
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

		"1:	.rept		25			\n\t"
		"	movea		(%0)4++, %1		\n\t"
		"	.endr					\n\t"
		"	.rept		7			\n\t"
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

	__asm__ volatile (
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

		: "+a" (s), "+d" (c), "+d" (n), "=a"(aligned_start)
		:
		: "d15", "memory", "cc"
	);

	return s_ret;
}
libc_hidden_weak(memset)

