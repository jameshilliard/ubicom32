/*
 * arch/ubicom32/kernel/switch_to.S
 *	Implements context switch function.
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
 * __switch_to()
 *
 * Call with:
 *	void *__switch_to(struct task_struct *prev, 
 *			  struct thread_struct *prev_switch,
 *			  struct thread_struct *next_switch)
 */
	.section .text.__switch_to, "ax", @progbits
	.global __switch_to
__switch_to:

	/*
	 * Set up register a3 to point to save area.
	 */
	movea	a3, d1			; a3 now holds prev_switch
	move.4	(a3)4++, d10
	move.4	(a3)4++, d11
	move.4	(a3)4++, d12
	move.4	(a3)4++, d13
	move.4	(a3)4++, a1
	move.4	(a3)4++, a2
	move.4	(a3)4++, a5
	move.4	(a3)4++, a6
	move.4	(a3)4++, a7

	/*
	 * Set up register a3 to point to restore area.
	 */
	movea	a3, d2			; a3 now holds next_switch
	move.4	d10 , (a3)4++
	move.4	d11 , (a3)4++
	move.4	d12 , (a3)4++
	move.4	d13 , (a3)4++
	move.4	a1 , (a3)4++
	move.4	a2 , (a3)4++
	move.4	a5 , (a3)4++
	move.4	a6 , (a3)4++
	move.4	a7 , (a3)4++

	/*
	 * Load the sw_ksp with the proper thread_info pointer.
	 * Accessing ROSR is safe since __switch_to() is always
	 * called with preemption disabled.
	 */
	movei	d15, #(~(ASM_THREAD_SIZE-1))
	and.4	a3, sp, d15		; a3 now has the thread info pointer
	moveai	a4, #%hi(sw_ksp)
	lea.1	a4, %lo(sw_ksp)(a4)	; a4 now has the base address of sw_ksp array
	lsr.4	d15, ROSR, #2		; Thread number
	and.4	d15, #0x1f, d15		; Thread number masked
	move.4	(a4, d15), a3		; Load the thread info pointer into the sw_ksp array..

	/*
	 * We are done with context switch. Time to return..
	 */
	calli	a5, 0(a5)
	.size __switch_to, . - __switch_to
