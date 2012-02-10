/* 
 * __malloc_lock and __malloc_unlock for the Scenix VPE4K processor.
 * 
 * Inputs: 
 *      d1 - address of reentrant structure to be used as lock id
 * Outputs:
 *      None
 *
 * Copyright (C) 2000, 2002 by Red Hat Incorporated. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 */

	.text
	.global __malloc_lock
	.global __malloc_unlock
	.extern __mlock

__malloc_lock:
        move.4 d1,d0
	move.4 d0,a0
loop:
	moveai a0,#%hi(__mlock)
	lea.4 a0,%lo(__mlock)(a0)
	bset (a0),(a0),#0	; attempt to acquire lock
	jmpeq.w.t locked	; branch if lock freshly acquired
	sub.4 #0,4(a0),d1	; otherwise check if we already have lock
	jmpne.w.t loop		; if not, continue trying

locked:
	bset (a0),(a0),#1	; acquire counter lock
	jmpne.w.f locked
	move.4 4(a0),d1		; use reentrant pointer as lock id
	movei d2,#1
	add.4 8(a0),8(a0),d2	; increment lock counter
	bclr (a0),(a0),#1	; release counter lock

	move.4 a0,d0
	ret a5

	.align 2	
__malloc_unlock:
        move.4 d1,d0
	move.4 d0,a0
uloop:
	moveai a0,#%hi(__mlock)
	lea.4 a0,%lo(__mlock)(a0)
	sub.4 #0,4(a0),d1	; confirm that the id already owns the lock
	jmpne.w.t uloop	 	; if we don't already hold the lock, loop
.L0:
	bset (a0),(a0),#1	; acquire counter lock
	jmpne.w.f .L0
	move.4 d2,8(a0)
	move.4 d3,#1
	sub.4 8(a0),d2,d3	; decrement counter
	jmpgt.w.f .L1		; check if lock counter is <= 0
	xor.4 4(a0),d2,d2	; if yes, clear out owning context
	xor.4 8(a0),d2,d2	; clear counter
	bclr (a0),(a0),#0	; and release main lock
.L1:
	bclr (a0),(a0),#1	; release counter lock
	move.4 a0,d0
	ret a5
