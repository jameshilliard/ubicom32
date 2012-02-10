/*
 * Copyright (c) 2000, 2002 Red Hat Inc.
 */

/*
 * C library -- _setjmp, _longjmp
 *
 *      _longjmp(a,v)
 * will generate a "return(v?v:1)" from
 * the last call to
 *      _setjmp(a)
 * by unwinding the call stack.
 * The previous signal state is NOT restored.
 */


	.align 4
	.text
	.globl setjmp
	.globl _setjmp
	.globl longjmp
	.globl _longjmp

setjmp:
_setjmp:
        move.4  -4(sp)++,a0
	move.4	a0,d0
	move.4	(a0)4++,d1
	move.4	(a0)4++,d2
	move.4	(a0)4++,d3
	move.4	(a0)4++,d4
	move.4	(a0)4++,d5
	move.4	(a0)4++,d6
	move.4	(a0)4++,d7
	move.4	(a0)4++,d8
	move.4	(a0)4++,d9
	move.4	(a0)4++,d10
	move.4	(a0)4++,d11
	move.4	(a0)4++,d12
	move.4	(a0)4++,d13
	move.4	(a0)4++,d14
	move.4	(a0)4++,d15
	move.4	(a0)4++,a1
	move.4	(a0)4++,a2
	move.4	(a0)4++,a3
	move.4	(a0)4++,a4
	move.4	(a0)4++,a5
	move.4	(a0)4++,a6
	move.4	(a0)4++,a7
	move.4  d0,(sp)4++
	move.4	(a0)4++,d0	/* actually a0 which we store last */
	move.4	a0,d0		/* restore a0 */
	xor.4	d0,d0,d0	/* return 0 */
        ret	a5

longjmp:
_longjmp:
	move.4	a0,d0
	movei   d0,#0
	cmpi	d1,#0		/* compute v ? v : 1 for return code */
	jmpne.w label1
	movei	d0,#1
label1:
	move.4	d1,(a0)4++
	move.4	d2,(a0)4++
	move.4	d3,(a0)4++
	move.4	d4,(a0)4++
	move.4	d5,(a0)4++
	move.4	d6,(a0)4++
	move.4	d7,(a0)4++
	move.4	d8,(a0)4++
	move.4	d9,(a0)4++
	move.4	d10,(a0)4++
	move.4	d11,(a0)4++
	move.4	d12,(a0)4++
	move.4	d13,(a0)4++
	move.4	d14,(a0)4++
	move.4	d15,(a0)4++
	move.4	a1,(a0)4++
	move.4	a2,(a0)4++
	move.4	a3,(a0)4++
	move.4	a4,(a0)4++
	move.4	a5,(a0)4++
	move.4	a6,(a0)4++
	move.4	a7,(a0)4++
	move.4	a0,(a0)		/* a0 is last in list */
	ret	a5
	.end

