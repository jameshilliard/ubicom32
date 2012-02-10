/*
 * libc/sysdeps/linux/ubicom32/clone.S -- 'clone' syscall for linux/ubicom32
 *
 * Copyright (C) 2009, Ubicom, Inc.
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>

	.global __syscall_error

/* int _clone(int (*fn)(void *arg), void *child_stack, int flags, void *arg); */
	.text
	.type   clone,@function
	.global clone
clone:
	/* Sanity check arguments.  */
	sub.4	#0, #0, d0	/* Test if fn is zero. */
	jmpeq.f .Leinval	/* arg bad */
	sub.4	#0, #0, d1	/* Test if child_stack is zero. */
	jmpeq.f	.Leinval	/* arg bad */

	move.4	-4(sp)++, a5	/* push a5 to our stack */
	move.4	-4(sp)++, a1	/* push a1 */
	move.4	a1, d0		/* save fn in a1 */
	/*
	 * The syscall clone is clone(int flags, void *child_stack) so we set
	 * that up now
	 */
	move.4	d0, d2		/* Move flags to d0 */
				/* child stack is already in d1 */

	/* Do Clone syscall */
	movei	d8, #__NR_clone
	__syscall

	/* Clone Complete */
	cmpi	d0, #0		/* Test if d0 is less than zero. If it is we
				 * return the error */
	jmpeq.f	.Lchild_thread	/* If return is 0 we are in the clone, jump to
				 * thread start */
	move.4	a1, (sp)4++	/* pop a1 from stack */
	move.4	a5, (sp)4++	/* pop a5 from stack */
	jmplt.f	.Lerror		/* If return is less than we had an error */
	ret	a5		/* d0 is pid */

	/* Invalid Value */
.Leinval:
	movei   d0, #-22	/* EINVAL */

	/* Call syscall Error (it will return through a5) */
.Lerror:
	sub.4	d0, #0, d0	/* d0 = -d0 */
#if defined(__UBICOM32_FDPIC__)
	call	a3, __syscall_error
#else
	moveai	a3, #%hi(__syscall_error)
	calli	a3, %lo(__syscall_error)(a3)
#endif

	/* Child Thread Start */
.Lchild_thread:
	move.4	d0, d3		/* copy 'arg' to d0 (this is because ubicom32
				   syscall doesn't clobber d0-d5,d8 */
#if defined(__UBICOM32_FDPIC__)
	/* a5 actually contains a function descriptor for fdpic */
	move.4	a0, 4(a1)	/* set GOT for this function */
	move.4	a5, 0(a1)	/* get address of entry point */
	calli	a5, 0(a5)
#else
	calli	a5, 0(a1)
#endif
	movei	d8, #__NR_exit
	__syscall

	.size clone, . - clone
	.global __GI_clone
	.hidden __GI_clone
	.set __GI_clone,clone
