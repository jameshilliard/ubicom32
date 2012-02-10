/* Machine-dependent pthreads configuration and inline functions.

   Copyright (C) 1996, 1997, 1998, 2000, 2002, 2003, 2004
   Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ralf Baechle <ralf@gnu.org>.
   Based on the Alpha version by Richard Henderson <rth@tamu.edu>.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If
   not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef _PT_MACHINE_H
#define _PT_MACHINE_H   1

#include <features.h>

/*
 * XXX try to make this inline
 */
extern long int testandset (int *spinlock);

#ifndef PT_EI
#define PT_EI
#else
/* Spinlock implementation; required.  */

/*
 * testandset() is the basis for the pthread spin lock.
 *
 * This implementation only requires that we 'set' the state of *spinlock. As
 * bset is allows us to atomically 'testandset' a single bit define our 'set' is
 * such that we will set bit 0 (ignoring all other bits, which should also be
 * zero).
 * Return value of 1 implies that the bit was already set and is still
 * set.
 * Return value of 0 implies that the bit was not previously set but it
 * now is.
 */
long int
testandset (int *spinlock)
{
	int ret; 
	__asm__ volatile (
		"	move.4	%0, #0		\n\t" /* Assume that the bit is not currently set */
		"	bset	%1, %1, #0	\n\t" /* Attempt to 'set' bit 0 */
		"	jmpeq.t	1f		\n\t"
		"	move.4	%0, #1		\n\t" /* Bit was already set, so return 1 */
		" 1:				\n\t"
		: "=r"(ret), "+U4"(*spinlock)
		:
		: "memory", "cc"
		);
	return ret;
}
#endif
#endif /* pt-machine.h */
