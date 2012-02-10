#include <linux/sys.h>
#include <linux/linkage.h>
#include <linux/unistd.h>

#include <asm/ubicom32.h>
#include <asm/thread_info.h>
#include <generated/asm-offsets.h>
#include <asm/range-protect.h>
#include <asm/ubicom32-asm.h>
#include <asm/segment.h>

#ifdef CONFIG_SUPPORT_LIGHTWEIGHT_ATOMICS
	
.macro	__syscall_ret
1:	bset scratchpad5, scratchpad5, #1
	jmpne.f	1b
	bset	csr, csr, #UBICOM32_CSR_PREV_PRIV_BIT
	setcsr_flush 0
	move.4	scratchpad4, a5
	sysret	scratchpad5, scratchpad4	; return and clear the lock
.endm
	
/* 
 * __lw_atomic_cmpxchg
 * Input:
 *	int old
 *	int new
 *	int *addr
 *
 * Ouput:
 */
	.section .text.__lw_atomic_cmpxchg, "ax", @progbits
	.global __lw_atomic_cmpxchg
__lw_atomic_cmpxchg:
	and.4	#0, #3, d2	; Test for word alignment
	jmpne.f	4f		; We have to fix the misaligned address
5:	sub.4	#0, a3, d2	; Test if address is at kernel line or above
	jmpls.f	2f		;  if address invalid exit with error
	movea	a3, d2		; Load destination address into a3
	move.4	(a3), (a3)	; Test for bad address, Copy on write etc.

	;; Acquire the lock
1:	bset 	scratchpad1, scratchpad1, #USER_ATOMIC_LOCK_BIT
	jmpne.f	1b

	;; We own the lock
	move.4	d3, (a3)	; Read the current value
	sub.4	#0, d0, d3	; Compare old with current value.
	jmpne.f	2f
	move.4	(a3), d1	; Old == current value. Replace with new
	move.4	d0, #0		; Return success

3:	bclr	scratchpad1, scratchpad1, #USER_ATOMIC_LOCK_BIT ; Release the lock and return
	__syscall_ret
	
2:	move.4	d0, #1		; Return failure
	jmpt.t	3b		; Release lock and return

4:	and.4	d2, #-4, d2	; Align the destination address in d2
	jmpt.t	5b
	
	.size __lw_atomic_cmpxchg, . - __lw_atomic_cmpxchg

/* 
 * __lw_atomic_dec
 * Input:
 *	int *addr
 *
 * Ouput:
 *	Return previous value
 */
	.section .text.__lw_atomic_dec, "ax", @progbits
	.global __lw_atomic_dec
__lw_atomic_dec:
	and.4	#0, #3, d0	; Test for word alignment
	jmpne.f	2f		; We have to fix the misaligned address
3:	sub.4	#0, a3, d0	; Test if address is at kernel line or above
	jmpls.f	4f		;  if address invalid exit
	movea	a3, d0		; Load the destination address into a3
	move.4	(a3), (a3)	; Test for bad address, copy on write etc.

	;; Acquire the lock
1:	bset 	scratchpad1, scratchpad1, #USER_ATOMIC_LOCK_BIT
	jmpne.f	1b

	;; We own the lock
	move.4	d0, (a3)	;Read the current value
	move.4	d1, #1
	sub.4	(a3), (a3), d1	;Decrement
	bclr	scratchpad1, scratchpad1, #USER_ATOMIC_LOCK_BIT ;Release the lock and return

4:	__syscall_ret

2:	and.4	d0, #-4, d0	; Align the destination address in d0
	jmpt.t	3b

	.size __lw_atomic_dec, . - __lw_atomic_dec

/* 
 * __lw_atomic_inc
 * Input:
 *	int *addr
 *
 * Ouput:
 *	Return previous value
 */
	.section .text.__lw_atomic_inc, "ax", @progbits
	.global __lw_atomic_inc
__lw_atomic_inc:
	and.4	#0, #3, d0	; Test for word alignment
	jmpne.f	2f		; We have to fix the misaligned address
3:	sub.4	#0, a3, d0	; Test if address is at kernel line or above
	jmpls.f	4f		;  if address invalid exit
	movea	a3, d0		; Load the destination address into a3
	move.4	(a3), (a3)	; Test for bad address, copy on write etc.

	;; Acquire the lock
1:	bset 	scratchpad1, scratchpad1, #USER_ATOMIC_LOCK_BIT
	jmpne.f	1b

	;; We own the lock
	move.4	d0, (a3)	;Read the current value
	move.4	d1, #1
	add.4	(a3), (a3), d1	;Increment
	bclr	scratchpad1, scratchpad1, #USER_ATOMIC_LOCK_BIT ;Release the lock and return
4:	__syscall_ret

2:	and.4	d0, #-4, d0	; Align the destination address in d0
	jmpt.t	3b
		
	.size __lw_atomic_inc, . - __lw_atomic_inc


/* 
 * __lw_atomic_swap
 * Input:
 *	int new
 *	int *addr
 *
 * Ouput:
 *	Return previous value
 */
	.section .text.__lw_atomic_swap, "ax", @progbits
	.global __lw_atomic_swap
__lw_atomic_swap:
	and.4	#0, #3, d1	; Test for word alignment
	jmpne.f	2f		; We have to fix the misaligned address
3:	sub.4	#0, a3, d1	; Test if address is at kernel line or above
	jmpls.f	4f		;  if address invalid exit
	movea	a3, d1		; Load the destination address into a3
	move.4	(a3), (a3)	; Test for bad address, copy on write etc.

	;; Acquire the lock
1:	bset 	scratchpad1, scratchpad1, #USER_ATOMIC_LOCK_BIT
	jmpne.f	1b

	;; We own the lock
	move.4	d2, (a3)	;Read the current value
	move.4	(a3), d0	;Replace destination with new value
	move.4	d0, d2		;Return current value.
	bclr	scratchpad1, scratchpad1, #USER_ATOMIC_LOCK_BIT ;Release the lock and return
4:	__syscall_ret

2:	and.4	d1, #-4, d1	; Align the destination address in d1
	jmpt.t	3b
		
	.size __lw_atomic_swap, . - __lw_atomic_swap


/* 
 * __lw_atomic_add
 * Input:
 *	int new
 *	int *addr
 *
 * Ouput:
 *	Return previous value
 */
	.section .text.__lw_atomic_add, "ax", @progbits
	.global __lw_atomic_add
__lw_atomic_add:
	and.4	#0, #3, d1	; Test for word alignment
	jmpne.f	2f		; We have to fix the misaligned address
3:	sub.4	#0, a3, d1	; Test if address is at kernel line or above
	jmpls.f	4f		;  if address invalid exit
	movea	a3, d1		; Load the destination address into a3
	move.4	(a3), (a3)	; Test for bad address, copy on write etc.

	;; Acquire the lock
1:	bset 	scratchpad1, scratchpad1, #USER_ATOMIC_LOCK_BIT
	jmpne.f	1b

	;; We own the lock
	move.4	d2, (a3)	;Read the current value
	add.4	(a3), d2, d0	;Add values
	move.4	d0, d2		;Return current value.
	bclr	scratchpad1, scratchpad1, #USER_ATOMIC_LOCK_BIT ;Release the lock and return
4:	__syscall_ret

2:	and.4	d1, #-4, d1	; Align the destination address in d1
	jmpt.t	3b
		
	.size __lw_atomic_add, . - __lw_atomic_add


/* 
 * __lw_atomic_and
 * Input:
 *	int new
 *	int *addr
 *
 * Ouput:
 *	Return previous value
 */
	.section .text.__lw_atomic_and, "ax", @progbits
	.global __lw_atomic_and
__lw_atomic_and:
	and.4	#0, #3, d1	; Test for word alignment
	jmpne.f	2f		; We have to fix the misaligned address
3:	sub.4	#0, a3, d1	; Test if address is at kernel line or above
	jmpls.f	4f		;  if address invalid exit
	movea	a3, d1		; Load the destination address into a3
	move.4	(a3), (a3)	; Test for bad address, copy on write etc.

	;; Acquire the lock
1:	bset 	scratchpad1, scratchpad1, #USER_ATOMIC_LOCK_BIT
	jmpne.f	1b

	;; We own the lock
	move.4	d2, (a3)	;Read the current value
	and.4	(a3), d2, d0	;And values
	move.4	d0, d2		;Return current value.
	bclr	scratchpad1, scratchpad1, #USER_ATOMIC_LOCK_BIT ;Release the lock and return
4:	__syscall_ret

		
2:	and.4	d1, #-4, d1	; Align the destination address in d1
	jmpt.t	3b

	.size __lw_atomic_and, . - __lw_atomic_and


/* 
 * __lw_atomic_or
 * Input:
 *	int new
 *	int *addr
 *
 * Ouput:
 *	Return previous value
 */
	.section .text.__lw_atomic_or, "ax", @progbits
	.global __lw_atomic_or
__lw_atomic_or:
	and.4	#0, #3, d1	; Test for word alignment
	jmpne.f	2f		; We have to fix the misaligned address
3:	sub.4	#0, a3, d1	; Test if address is at kernel line or above
	jmpls.f	4f		;  if address invalid exit
	movea	a3, d1		; Load the destination address into a3
	move.4	(a3), (a3)	; Test for bad address, copy on write etc.

	;; Acquire the lock
1:	bset 	scratchpad1, scratchpad1, #USER_ATOMIC_LOCK_BIT
	jmpne.f	1b

	;; We own the lock
	move.4	d2, (a3)	;Read the current value
	or.4	(a3), d2, d0	;Or values
	move.4	d0, d2		;Return current value.
	bclr	scratchpad1, scratchpad1, #USER_ATOMIC_LOCK_BIT ;Release the lock and return
4:	__syscall_ret

2:	and.4	d1, #-4, d1	; Align the destination address in d1
	jmpt.t	3b
		
	.size __lw_atomic_or, . - __lw_atomic_or


/* 
 * __lw_atomic_invalid
 * Input:
 *
 * Ouput:
 */
	.section .text.__lw_atomic_invalid, "ax", @progbits
	.global __lw_atomic_invalid
__lw_atomic_invalid:
	movei	d0, #0
	__syscall_ret

		
	.size __lw_atomic_invalid, . - __lw_atomic_invalid


	.section .text.ubicom32_lw_syscall
	.global	ubicom32_lw_syscall
ubicom32_lw_syscall:
	/*
	 * We got here because this is a light weight system call.
	 */
	and.4	d8, #7, d8			; Prevent accesses beyond end of table
	moveai	a3, #%hi(lw_sys_call_table)	;
	lea.1	a3, %lo(lw_sys_call_table)(a3)	; a3 = sys_call_table
	move.4	a4, (a3, d8)			; a4 = sys_call_table[d8]

	/*
	 * Put the kernel memory boundary in a3.  The system calls
	 * will compare against this to validate pointer arguments.
	 * NOTE: We are assuming that the low 7 bits of the kernel
	 * start address are all 0.  If they are not, then this code
	 * will need to be changed.
	 */
	moveai	a3, #%hi(UBICOM32_FDPIC_KERNEL_START)

	calli	a4, 0(a4)			; Jump to the system call.

#endif /* CONFIG_SUPPORT_LIGHTWEIGHT_ATOMICS */
