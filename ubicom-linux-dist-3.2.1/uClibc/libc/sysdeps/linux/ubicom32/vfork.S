/*
 * libc/sysdeps/linux/ubicom32/vfork.S -- 'vfork' syscall for linux/ubicom32
 *
 * Copyright (C) 2009, Ubicom, Inc.
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <sys/syscall.h>

	.text
	.type   vfork,@function
	.global vfork
vfork:
	move.4	-4(sp)++, a5	; Save return address on the stack.
	movei	d8, #__NR_vfork
	__syscall

	/*
	 * You get here only if the syscall bombed. If things had worked out the
	 * parent and child would have both returned to the instruction just after
	 * the call to this function.  See the implementation in 
	 * arch/ubicom32/kernel/process.c.
	 */
	move.4	a5, (sp)4++	; Pop the return address off the stack.
;	movei	d1, #-125
;	sub.4	#0, d0, d1
;	jmplt.t	1f

	;; We have an error.
	sub.4	d0, #0, d0	; d0 = -res. Call __set_errno with that.
#if defined(__UBICOM32_FDPIC__)
	call	a3, __syscall_error
#else
	moveai	a3, #%hi(__syscall_error)
	calli	a3, %lo(__syscall_error)(a3)	; __syscall_error will return -1 and not come back here.
#endif
1:
	calli	a5, 0(a5)
	.size vfork, . - vfork
	.global __GI_vfork
	.hidden __GI_vfork
	.set __GI_vfork,vfork
