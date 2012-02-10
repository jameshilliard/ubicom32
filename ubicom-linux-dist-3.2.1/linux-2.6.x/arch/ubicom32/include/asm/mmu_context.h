/*
 * arch/ubicom32/include/asm/mmu_context.h
 *   Ubicom32 architecture core file definitions.
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
#ifndef _ASM_UBICOM32_MMU_CONTEXT_H
#define _ASM_UBICOM32_MMU_CONTEXT_H

#include <linux/compiler.h>
#include <asm/cacheflush.h>
#include <asm/tlbflush.h>
#include <asm-generic/mm_hooks.h>

#if !defined(CONFIG_MMU)
/*
 * Make all this disappear if we don't have an mmu.
 */
#define init_new_context(tsk, mm)	0
#define switch_mm(prev, next, tsk)	do {} while(0)
#define enter_lazy_tlb(mm, tsk)		do {} while(0)
#define destroy_context(mm)		do { } while(0)
#define deactivate_mm(tsk, mm)		do { } while (0)
#define activate_mm(prev, next)		switch_mm(prev, next, NULL)

#else

/*
 * Real implementation of context handling with an MMU.
 */
extern int init_new_context(struct task_struct *tsk, struct mm_struct *mm);
extern void context_assign(struct mm_struct *mm);
extern void context_assign_kernel(void);

/*
 * switch_mm()
 *	Linux is context switching the memory management.
 */
static inline void switch_mm(struct mm_struct *prev, struct mm_struct *next, struct task_struct *tsk)
{
	context_assign(next);
}

/*
 * enter_lazy_tlb()
 *	Make sure the kernel task can only access kernel space.
 *
 * This is called by the kernel when it is context switching to a kernel
 * task.  Kernel tasks have no mm data structure and should not be able
 * to access the previous users context.
 */
#define enter_lazy_tlb(mm, tsk) context_assign_kernel();

#define destroy_context(mm)	do { } while(0)
#define deactivate_mm(tsk, mm)	do { } while (0)
#define activate_mm(prev, next)	switch_mm(prev, next, NULL)

#endif /* CONFIG_MMU */
#endif	/* _ASM_UBICOM32_MMU_CONTEXT_H */
