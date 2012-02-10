/*
 * arch/ubicom32/include/asm/pic.h
 *   Ubicom32 PIC interface definitions.
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
#ifndef _ASM_UBICOM32_PIC_H
#define _ASM_UBICOM32_PIC_H

#include <asm/ubicom32.h>
#include <asm/types.h>
#include <asm/thread.h>

#if defined(CONFIG_PREEMPT)
#define PIC_PREEMPT_ROSR_ACQUIRE() pic_lock_acquire();
#define PIC_PREEMPT_ROSR_RELEASE() pic_lock_release();
#else
#define PIC_PREEMPT_ROSR_ACQUIRE()
#define PIC_PREEMPT_ROSR_RELEASE()
#endif

/*
 * For all of the asm blocks in this file, we need to ensure the
 * compiler can not optimize out the code (volatile) and that it does
 * not "cache" values around the interrupt state change (memory).  This
 * ensures that interrupt changes are treated as a critical section.
 */

/*
 * Enabling irqs uses the pic_soft_irq_mask to wake
 * the pic.
 */
extern unsigned int pic_soft_irq_mask;

/*
 * pic_lock_release()
 *	Release the PIC lock.
 */
static inline void pic_lock_release(void)
{
	UBICOM32_UNLOCK(PIC_LOCK_BIT);
}

/*
 * pic_lock_acquire()
 *	Acquire the PIC lock, spin if not available.
 */
static inline void pic_lock_acquire(void)
{
	UBICOM32_LOCK(PIC_LOCK_BIT);
}

/*
 * pic_local_irq_is_disabled()
 *	Test if interrupts are disabled for this thread?
 */
static inline int pic_local_irq_is_disabled(void)
{
	volatile int ret;
	thread_t self;
	unsigned int mask;

	PIC_PREEMPT_ROSR_ACQUIRE();
	self = thread_get_self();
	mask = (1 << self);
	asm volatile (
	"	and.4	%0, scratchpad1, %1	\n\t"
		: "=r" (ret)
		: "d" (mask)
		: "cc"
	);

	/*
	 *  We return a simple 1 == disabled, 0 == enabled
	 *  losing which tid this is for, because Linux
	 *  can restore interrupts on a different thread.
	 */
	ret >>= self;
	PIC_PREEMPT_ROSR_RELEASE();
	return ret;
}

/*
 * pic_local_irq_save()
 *	Get the current interrupt state and disable interrupts.
 *
 *  We return a simple 1 == disabled, 0 == enabled losing which tid this
 *  is for, because Linux can restore interrupts on a different thread.
 */
static inline unsigned int pic_local_irq_save(void)
{
	int ret;

#if (UBICOM32_ARCH_VERSION <= 4)
	thread_t self;
	unsigned int mask;

	PIC_PREEMPT_ROSR_ACQUIRE();
	self= thread_get_self();
	mask = (1 << self);

	asm volatile (
	"	and.4	%0, scratchpad1, %1		\n\t"
	"	or.4	scratchpad1, scratchpad1, %1	\n\t"
		: "=&r" (ret)
		: "d" (mask)
		: "cc", "memory"
	);
	PIC_PREEMPT_ROSR_RELEASE();
	return ret >> self;
#else
	asm volatile (
	"	movei	%0, #0				\n\t"
	"	tbset	scratchpad1, scratchpad1	\n\t"
	"	jmpeq.t 1f				\n\t"
	"	movei	%0, #1				\n\t"
	"1:						\n\t"
		: "=r" (ret)
		:
		: "cc", "memory"
	);
	return ret;
#endif

}

/*
 * pic_local_irq_restore()
 * 	Restore this cpu's interrupt enable/disable state.
 *
 * Note: flags is either 0 or 1.
 */
static inline void pic_local_irq_restore(unsigned int flags)
{
	unsigned int temp;
	thread_t self;
	unsigned int mask;

	PIC_PREEMPT_ROSR_ACQUIRE();
	self= thread_get_self();
	mask = (1 << self);
	flags = (flags << self);

	/*
	 * Atomic change to our bit in scratchpad1 without
	 * causing any temporary glitch in the value and
	 * without effecting other values.  Also this uses
	 * no branches so no penalties.
	 */
	asm volatile (
	"	xor.4	%0, scratchpad1, %1		\n\t"
	"	and.4	%0, %2, %0			\n\t"
	"	xor.4	scratchpad1, scratchpad1, %0	\n\t"
	"	move.4	int_set0, %3			\n\t"
		: "=&d" (temp)
		: "d" (flags), "r" (mask), "m" (pic_soft_irq_mask)
		: "cc", "memory"
	);
	PIC_PREEMPT_ROSR_RELEASE();
}

/*
 * pic_local_irq_disable()
 * 	Disable ints for this thread.
 */
static inline void pic_local_irq_disable(void)
{
#if (UBICOM32_ARCH_VERSION <= 4)
	unsigned int mask;

	PIC_PREEMPT_ROSR_ACQUIRE();
	mask = (1 << thread_get_self());
	asm volatile (
	"	or.4	scratchpad1, scratchpad1, %0	\n\t"
		:
		: "d" (mask)
		: "cc", "memory"
	);
	PIC_PREEMPT_ROSR_RELEASE();
#else
	asm volatile (
	"	tbset	scratchpad1, scratchpad1	\n\t"
		:
		:
		: "cc"
	);

#endif
}

/*
 * pic_local_irq_enable_interrupt
 * 	Enable ints for this thread.
 */
static inline void pic_local_irq_enable(void)
{

#if (UBICOM32_ARCH_VERSION <= 4)
	unsigned int mask;

	PIC_PREEMPT_ROSR_ACQUIRE();
	mask = (1 << thread_get_self());
	asm volatile (
	"	and.4	scratchpad1, scratchpad1, %0	\n\t"
	"	move.4	int_set0, %1			\n\t"
		:
		: "d" (~mask), "m" (pic_soft_irq_mask)
		: "cc", "memory"
	);
	PIC_PREEMPT_ROSR_RELEASE();
#else	
	asm volatile (
	"	tbclr	scratchpad1, scratchpad1	\n\t"
	"	move.4	int_set0, %[soft_mask]		\n\t"
		:
		: [soft_mask]"m" (pic_soft_irq_mask)
		: "cc", "memory"
	);

#endif

}

extern void pic_init(void);
extern void pic_set_trap_irq(unsigned int irq);
extern void pic_set_mmu_irq(unsigned int irq);
extern void pic_mask_vector(unsigned int vector);
extern void pic_unmask_vector(unsigned int vector);
extern void pic_enable_vector(unsigned int vector);
extern void pic_disable_vector(unsigned int vector);
extern thread_t pic_get_threadid(void);

#endif /* _ASM_UBICOM32_PIC_H */
