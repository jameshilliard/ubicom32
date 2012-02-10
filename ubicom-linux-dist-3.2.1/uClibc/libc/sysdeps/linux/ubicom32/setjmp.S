/*
 * libc/sysdeps/linux/ubicom32/setjmp.S -- setjmp/sigsetjmp/longjmp for linux/ubicom32
 *
 * Copyright (C) 2009, Ubicom, Inc.
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

/*
 * _setjmp() for Ubicom32
 *	This just does a tail-call to `__sigsetjmp (ARG, 0)'.  We cannot
 *	do it in C because it must be a tail-call, so frame-unwinding in
 *	setjmp doesn't clobber the state restored by longjmp.
 */
	.text
	.global _setjmp
	.type   _setjmp,@function
_setjmp:
	movei	d1, #0		/* savemask=0, don't save the mask */
	jmpt.t	.L__sigsetjmp	/* don't use the public symbol as the
				   link will jump through the plt */
	.size _setjmp, . - _setjmp

/*
 * setjmp() for Ubicom32
 *	This just does a tail-call to `__sigsetjmp (env, 1)'.  We cannot
 *	do it in C because it must be a tail-call, so frame-unwinding in
 *	setjmp doesn't clobber the state restored by longjmp.
 */
	.global setjmp
	.type   setjmp,@function
setjmp:
	movei	d1, #1		/* savemask=1, to save the mask */
	jmpt.t	.L__sigsetjmp	/* don't use the public symbol as the
				   link will jump through the plt */
	.size setjmp, . - setjmp

/*
 * __sigsetjmp() for Ubicom32
 *     d0 holds sigjmp_buf and d1 holds the savemask. We will save the
 *     frame and then just call __sigjmp_save to do the mask save.
 */
	.global __sigsetjmp
	.type   __sigsetjmp,@function
__sigsetjmp:
.L__sigsetjmp:			/* private version used by setjmp, _setjmp */
	movea	a3, d0		; A3 now holds the jmp buf that was passed in.
	move.4	(a3)4++, d10
	move.4	(a3)4++, d11
	move.4	(a3)4++, d12
	move.4	(a3)4++, d13
	move.4  (a3)4++, a1
	move.4  (a3)4++, a2
	move.4  (a3)4++, a5
	move.4  (a3)4++, a6
	move.4  (a3)4++, a7

	/*
	 * The frame has been saved. Call _sigjmp_save to get sigmask saved. a5
	 * still has the return address and it will go back to that.
	 */
#if defined(__UBICOM32_FDPIC__)
	call	a3, __sigjmp_save
#else
	moveai	a3, #%hi(__sigjmp_save)
	calli	a3, %lo(__sigjmp_save)(a3)
#endif
	.size __sigsetjmp, . - __sigsetjmp

/*
 * __longjmp() for Ubicom32
 */
	.global	__longjmp
	.type   __longjmp,@function
__longjmp:
	movea	a3, d0		; A3 now holds the jmp buf that was passed in.
	move.4  d10, (a3)4++
	move.4  d11, (a3)4++
	move.4  d12, (a3)4++
	move.4  d13, (a3)4++
	move.4  a1 , (a3)4++
	move.4  a2 , (a3)4++
	move.4  a5 , (a3)4++
	move.4  a6 , (a3)4++
	move.4  a7 , (a3)4++

	/*
	 * Test if d1 is zero. If it is we have to return 1 other wise return
	 * content of d1
	 */
	sub.4	#0, #0, d1
	jmpeq.t	2f
	move.4	d0, d1		; d1 is non zero load it into d0
	jmpt.t	3f

2:	move.4	d0, #1
3:	calli	a5, 0(a5)
	.size __longjmp, . - __longjmp
	.global __GI___longjmp
	.hidden __GI___longjmp
	.set __GI___longjmp,__longjmp
