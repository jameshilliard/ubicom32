/*
 * arch/ubicom32/kernel/returns_v34.S
 *	Implements returns functions for all architectures < 5.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#include <linux/sys.h>
#include <linux/linkage.h>
#include <generated/asm-offsets.h>
#include <asm/ubicom32.h>
#include <asm/range-protect.h>

/*
 * __begin_restore_context()
 *	Restore most of the context from sp (struct pt_reg *)
 *
 * This *can* be called without the global atomic lock. (because sp is
 * not restored!)  Only d15 and a3 are allowed to be used after this
 * before calling __complete_restore_context
 */
.macro __begin_restore_context
	move.4	d0, PT_D0(sp)
	move.4	d1, PT_D1(sp)
	move.4	d2, PT_D2(sp)
	move.4	d3, PT_D3(sp)
	move.4	d4, PT_D4(sp)
	move.4	d5, PT_D5(sp)
	move.4	d6, PT_D6(sp)
	move.4	d7, PT_D7(sp)
	move.4	d8, PT_D8(sp)
	move.4	d9, PT_D9(sp)
	move.4	d10, PT_D10(sp)
	move.4	d11, PT_D11(sp)
	move.4	d12, PT_D12(sp)
	move.4	d13, PT_D13(sp)
	move.4	d14, PT_D14(sp)
;	move.4	d15, PT_D15(sp)
	move.4	a0, PT_A0(sp)
	move.4	a1, PT_A1(sp)
	move.4	a2, PT_A2(sp)
;	move.4	a3, PT_A3(sp)
	move.4	a4, PT_A4(sp)
	move.4	a5, PT_A5(sp)
	move.4	a6, PT_A6(sp)
	move.4	acc0_hi, PT_ACC0HI(sp)
	move.4	acc0_lo, PT_ACC0LO(sp)
	move.4	mac_rc16, PT_MAC_RC16(sp)
	move.4	acc1_hi, PT_ACC1HI(sp)
	move.4	acc1_lo, PT_ACC1LO(sp)
	move.4	source3, PT_SOURCE3(sp)
	move.4	int_mask0, PT_INT_MASK0(sp)
	move.4	int_mask1, PT_INT_MASK1(sp)
.endm

/*
 * pic_thread_enable_interrupts()
 *	An assembly version of the enable interrupts function.
 *
 * The stack is fair game but all registers MUST be preserved.
 *
 * We don't know if interrupts are disabled or not so we 
 * must protect the code in the CONFIG_PREEMPT case.
 */
.macro pic_thread_enable_interrupts
	move.4	-4(sp)++, d3	; Push d3
	move.4	-4(sp)++, a3	; Push a3

	/*
	 * Read the ROSR and obtain ~(1 << tid)
	 */
	lsr.4	d3, rosr, #0x2	; Move the thread portion of ROSR into d3
	lsl.4	d3, #1, d3	; perform a (1 << tid)
	not.4	d3, d3		; Negate the value of d3 == ~(1 << threadid)

	/*
	 * Get the value of the pic_soft_irq_mask
	 */
	moveai	a3, #%hi(pic_soft_irq_mask)
	move.4	a3, %lo(pic_soft_irq_mask)(a3)

	/*
	 * Now re-enable interrupts for this thread and then
	 * wakeup the PIC.
	 */
	and.4	scratchpad1, scratchpad1, d3
	move.4	int_set0, a3

	/*
	 * Restore the registers.
	 */
	move.4	a3, (sp)4++
	move.4	d3, (sp)4++
.endm

/*
 * __complete_restore_context()
 *	Completely restore the context from sp (struct pt_reg *)
 *
 * Note: Recovered PC and CSR are saved on the stack and are to be
 * popped off before returning.
 */
.macro __complete_restore_context
	move.4	a3, sp
	move.4	d15, PT_D15(sp)
	move.4	sp, PT_SP(a3)		; Recover Stack pointer from save area
	move.4	-4(sp)++, PT_PC(a3)	; Recover saved PC and save to stack
	move.4	-4(sp)++, PT_CSR(a3)	; Recover saved csr and save to stack
	move.4	a3, PT_A3(a3)
.endm

/*
 * __complete_and_return_to_userspace
 *
 * restores the second half of the context and returns
 * You must have the atomic lock when you call this function
 */
	.section .kernel_unprotected, "ax", @progbits
__complete_and_return_to_userspace:
#ifdef CONFIG_PROTECT_KERNEL
	disable_kernel_ranges_for_current d15	; disable kernel ranges
#endif
	__complete_restore_context		; restore previous context
	atomic_lock_release			; Leave critical section
	move.4	csr, (sp)4++			; Restore csr from the stack
	ret	(sp)4++

/*
 * ret_from_interrupt_to_kernel()
 *	RFI function that is where PIC preempted tasks return to if the thread was
 *	in kernel space.
 */
	.section .text.ret_from_interrupt_to_kernel, "ax", @progbits
	.global ret_from_interrupt_to_kernel
ret_from_interrupt_to_kernel:
#if defined(CONFIG_PREEMPT)
	/*
	 * Set a1 to the thread info pointer, no need to save it as we are
	 * restoring from an interrupt and will never return
	 */
	movei	d0, #(~(ASM_THREAD_SIZE-1))
	and.4	a1, sp, d0

	/*
	 * Test if my preemption count is to set to avoid scheduling.
	 */
	lsl.4 d0, TI_PREEMPTCOUNT(a1), #0
	jmpne.f 1f

	/*
	 * Test if the scheduler needs to be called.
	 */
	btst	TI_FLAGS(a1), #ASM_TIF_NEED_RESCHED
	jmpeq.t	1f

	/*
	 * Call the scheduler for preemption telling it that irqs are off.
	 */
	call	a5, preempt_schedule_irq
1:
#endif
	__begin_restore_context		; Restore the thread context
	atomic_lock_acquire		; Enter critical section
	__complete_restore_context	; Restore the thread context
	pic_thread_enable_interrupts	; enable the threads interrupts
	atomic_lock_release		; Leave critical section
	move.4	csr, (sp)4++		; Restore csr from the stack
	ret	(sp)4++

/*
 * ret_from_interrupt_to_user()
 *	RFI function that is where PIC preempted tasks return to if the thread was
 *	in user space.
 *
 * TODO: Do we really need the critical section handling in this code?
 *
 */
	.section .text.ret_from_interrupt_to_user, "ax", @progbits
	.global ret_from_interrupt_to_user
ret_from_interrupt_to_user:
	atomic_lock_acquire		; Enter critical section
	pic_thread_enable_interrupts	; enable the threads interrupts
	atomic_lock_release		; Leave critical section

	/*
	 * Set a1 to the thread info pointer, no need to save it as we are
	 * restoring userspace and will never return
	 */
	movei	d0, #(~(ASM_THREAD_SIZE-1))
	and.4	a1, sp, d0

	/*
	 * Test if the scheduler needs to be called.
	 */
	btst	TI_FLAGS(a1), #ASM_TIF_NEED_RESCHED
	jmpeq.t	2f
	call	a5, schedule			; Call the scheduler. I will come back here.

	/*
	 * See if we have pending signals and call do_signal
	 * if needed.
	 */
2:
	btst	TI_FLAGS(a1), #ASM_TIF_SIGPENDING	; Any signals needed?
	jmpeq.t	1f

	/*
	 * Now call do_signal()
	 */
	move.4	d0, #0					; oldset pointer is NULL
	move.4	d1, sp					; d1 is the regs pointer
	call	a5, do_signal				; Call do_signal()

	/*
	 * Back from do_signal(), re-enter critical section.
	 */
1:
	__begin_restore_context				; Restore the thread context
	atomic_lock_acquire				; Enter critical section
	call a3, __complete_and_return_to_userspace	; jump to unprotected section

/*
 * ret_from_signal_to_preempted_user()
 *
 * restore_all_registers will be the alternate exit route for
 * preempted processes that have called a signal handler
 * and are returning back to user space.
 */
	.section .text.ret_from_signal_to_preempted_user, "ax", @progbits
	.global ret_from_signal_to_preempted_user
ret_from_signal_to_preempted_user:
	__begin_restore_context			; Restore the thread context
	atomic_lock_acquire			; Enter critical section
	call a3, __complete_and_return_to_userspace

/*
 * ret_from_fork_to_user()
 *	Called on the child's return from fork system call.
 */
	.section .text.ret_from_fork_to_user, "ax", @progbits
	.global ret_from_fork_to_user
ret_from_fork_to_user:
	;;;  d0 contains the arg for schedule_tail
	;;;  the others we don't care about as they are in PT_REGS (sp)
	call   a5, schedule_tail

	move.4	a3, sp
	move.4	d0, PT_D0(sp)		; Restore D0
	move.4	d1, PT_D1(sp)		; Restore D1
	move.4	d2, PT_D2(sp)		; Restore D2
	move.4	d3, PT_D3(sp)		; Restore D3
	move.4	d10, PT_D10(sp)		; Restore D10
	move.4	d11, PT_D11(sp)		; Restore D11
	move.4	d12, PT_D12(sp)		; Restore D12
	move.4	d13, PT_D13(sp)		; Restore D13
	move.4	d14, PT_D14(sp)		; Restore D14
	move.4	a1, PT_A1(sp)		; Restore A1
	move.4	a2, PT_A2(sp)		; Restore A2
	move.4	a5, PT_A5(sp)		; Restore A5
	move.4	a6, PT_A6(sp)		; Restore A6

	atomic_lock_acquire		; Enter critical section

	move.4	sp, PT_SP(a3)		; Restore sp
	move.4	a4, PT_PC(a3)		; Restore pc in register a4
	move.4	PT_FRAME_TYPE(a3), #0	; Clear frame_type to indicate it is invalid.

#ifdef CONFIG_PROTECT_KERNEL
	call a3, __ret_from_fork_bottom_half
	.section .kernel_unprotected, "ax", @progbits
__ret_from_fork_bottom_half:
	disable_kernel_ranges_for_current d15
#endif
	atomic_lock_release		; Leave critical section
	calli	a4, 0(a4)		; Return.


/*
 * ret_from_fork_to_kernel()
 *	Called on the child's return from fork system call.
 */
	.section .text.ret_from_fork_to_kernel, "ax", @progbits
	.global ret_from_fork_to_kernel
ret_from_fork_to_kernel:
	;;;  d0 contains the arg for schedule_tail
	;;;  the others we don't care about as they are in PT_REGS (sp)
	call   a5, schedule_tail
	move.4	a3, sp
	move.4	d0, PT_D0(sp)		; Restore D0
	move.4	d1, PT_D1(sp)		; Restore D1
	move.4	d2, PT_D2(sp)		; Restore D2
	move.4	d3, PT_D3(sp)		; Restore D3
	move.4	d10, PT_D10(sp)		; Restore D10
	move.4	d11, PT_D11(sp)		; Restore D11
	move.4	d12, PT_D12(sp)		; Restore D12
	move.4	d13, PT_D13(sp)		; Restore D13
	move.4	d14, PT_D14(sp)		; Restore D14
	move.4	a1, PT_A1(sp)		; Restore A1
	move.4	a2, PT_A2(sp)		; Restore A2
	move.4	a5, PT_A5(sp)		; Restore A5
	move.4	a6, PT_A6(sp)		; Restore A6

	atomic_lock_acquire		; Enter critical section

	move.4	sp, PT_SP(a3)		; Restore sp
	move.4	a4, PT_PC(a3)		; Restore pc in register a4
	move.4	PT_FRAME_TYPE(a3), #0	; Clear frame_type to indicate it is invalid.

	atomic_lock_release		; Leave critical section
	calli	a4, 0(a4)		; Return.
