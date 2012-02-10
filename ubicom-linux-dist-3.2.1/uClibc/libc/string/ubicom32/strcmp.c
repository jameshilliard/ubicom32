/*
 * Copyright (C) 2009 Ubicom, Inc.
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <string.h>

libc_hidden_proto(strcmp)

int strcmp(const char *s1, const char *s2)
{
	const char *s1_c;
	const char *s2_c;
	int r;
	int t1;
	int t2;

	/*
	 * Work on copies as we'll trash the values in the asm code.
	 */
	s1_c = s1;
	s2_c = s2;

	__asm__ volatile (
	"1:	shmrg.1		%3, (%0)1++, #0		\n\t"
	"	move.1		%4, (%1)1++		\n\t"
	"	jmpeq.s.f	2f			\n\t"

	"	sub.4		%2, %3, %4		\n\t"
	"	jmpne.w.f	3f			\n\t"

	"	shmrg.1		%3, (%0)1++, #0		\n\t"
	"	move.1		%4, (%1)1++		\n\t"
	"	jmpeq.s.f	2f			\n\t"

	"	sub.4		%2, %3, %4		\n\t"
	"	jmpne.w.f	3f			\n\t"

	"	shmrg.1		%3, (%0)1++, #0		\n\t"
	"	move.1		%4, (%1)1++		\n\t"
	"	jmpeq.s.f	2f			\n\t"

	"	sub.4		%2, %3, %4		\n\t"
	"	jmpne.w.f	3f			\n\t"

	"	shmrg.1		%3, (%0)1++, #0		\n\t"
	"	move.1		%4, (%1)1++		\n\t"
	"	jmpeq.s.f	2f			\n\t"

	"	sub.4		%2, %3, %4		\n\t"
	"	jmpeq.w.t	1b			\n\t"

	"2:	sub.4		%2, %3, %4		\n\t"

	"3:						\n\t"
		: "+a" (s1_c), "+a" (s2_c), "=d" (r), "=d" (t1), "=d" (t2)
		:
		: "memory", "cc"
	);

	return r;
}
libc_hidden_def(strcmp)

#ifndef __UCLIBC_HAS_LOCALE__
libc_hidden_proto(strcoll)
strong_alias(strcmp,strcoll)
libc_hidden_def(strcoll)
#endif
