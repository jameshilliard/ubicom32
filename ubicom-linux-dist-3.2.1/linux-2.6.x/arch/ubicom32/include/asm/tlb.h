/*
 * arch/ubicom32/include/asm/tlb.h
 *   Ubicom32 architecture TLB operations.
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
#ifndef _ASM_UBICOM32_TLB_H
#define _ASM_UBICOM32_TLB_H

#include <linux/pagemap.h>

#include <asm/pgalloc.h>
#include <asm/tlbflush.h>
#include <asm/mmu_hardware.h>
#include <asm/mmu_ptec.h>

/*
 * Maintain an array of misses for each thread.  Each
 * tid can have exactly one outstanding miss being
 * processed in software.
 */
struct tlb_miss_per_thread {
	unsigned int missqw0;
	unsigned int missqw1;
}; 

extern volatile struct tlb_miss_per_thread tlb_missq[];
extern volatile unsigned int tlb_faulted_thread_mask;

/*
 * tlb_threads_blocked_not_interruptable()
 *	Decide if a thread can handle an interrupt.
 *
 * Thread that are blocked by the MMU can not be interrupted.
 *
 * Threads that are blocked by I/O can not be interrupted.
 *
 * The hardware does not differentiate between I/O blocked and OCM
 * blocked so sometimes we will avoid interrupting a thread that is
 * OCM blocked instead of MMU or I/O blocked.
 */
static inline unsigned long tlb_threads_blocked_not_interruptable(void)
{
	/*
	 * Bus_state0 is blocked or unblocked. Bus_state3 is blocked
	 * on the cache.  If we are not blocked on the cache, we are
	 * blocked on MMU, OCM or I/O.
	 */
	unsigned int bus_state0 = ubicom32_read_reg((volatile int *)MMU_BUS_ST0);
	unsigned int bus_state1 = ubicom32_read_reg((volatile int *)MMU_BUS_ST1);
	unsigned int bus_state2 = ubicom32_read_reg((volatile int *)MMU_BUS_ST2);
	unsigned int bus_state3 = ubicom32_read_reg((volatile int *)MMU_BUS_ST3);
	return bus_state0 & ((bus_state1 & ~bus_state3) | (~bus_state1 & bus_state2));
}

/*
 * tlb_faulted_threads()
 *	Returns the threads that are currently held for faults.
 */
static inline unsigned int tlb_faulted_threads(void)
{
	return ubicom32_read_reg(&tlb_faulted_thread_mask);
}

/*
 * tlb_faulted_clear()
 *	Remove the thread from the list of faulted threads.
 *
 * Use assembly to ensure that the inst is atomic.
 */
static inline void tlb_faulted_clear(thread_t tid)
{
	asm volatile (
	"	and.4	(%[faulted]), (%[faulted]), %[tmask]	\n\t"
		:
		: [faulted]"a" (&tlb_faulted_thread_mask), [tmask]"r" (~(1 << tid))
		: "cc", "memory"
		);
}

/*
 * tlb_faulted_set()
 *	Set the faulted thread bit.
 *
 * Use assembly to ensure that the inst is atomic.
 */
static inline void tlb_faulted_set(thread_t tid)
{
	asm volatile (
	"	or.4	(%[faulted]), (%[faulted]), %[tmask]	\n\t"
		:
		: [faulted]"a" (&tlb_faulted_thread_mask), [tmask]"r" (1 << tid)
		: "cc", "memory"
	);
}

/*
 * tlb_faulted_missq()
 *	Obtain the missq information for the faulted thread.
 */
static inline void tlb_faulted_missq(thread_t tid, unsigned int *missqw0, unsigned int *missqw1)
{
	/*
	 * The entry can only be called from the thread that owns the
	 * entry and the entry will not be written again by any thread
	 * until the faulted bit is clear.  Thus no locking is
	 * necessary.
	 */
	*missqw0 = tlb_missq[tid].missqw0;
	*missqw1 = tlb_missq[tid].missqw1;
}

/*
 * Per logical CPU data structure used to minimize
 * tlb changes by delaying requests on a per-page
 * basis.  Do not bother purging kernel space
 * vmas.
 */
struct mmu_gather {
	struct mm_struct	*mm;
	unsigned int		fullmm;
	unsigned int		kernel;
	unsigned long		range_start;
	unsigned long		range_end;
};

DECLARE_PER_CPU(struct mmu_gather, mmu_gathers);

/*
 * tlb_gather_mmu()
 *	The specified mmu is tied to this cpu until finished.
 */
static inline struct mmu_gather *
tlb_gather_mmu(struct mm_struct *mm, unsigned int fullmm)
{
	struct mmu_gather *tlb = &get_cpu_var(mmu_gathers);
	tlb->mm = mm;
	tlb->fullmm = fullmm;
	return tlb;
}

/*
 * tlb_finish_mmu()
 *	The tlb structure is released.
 *
 */
static inline void
tlb_finish_mmu(struct mmu_gather *tlb, unsigned long start, unsigned long end)
{
	/*
	 * If fullmmu was specified at binding time, it means that this mm
	 * structure is no longer in use and the user asid should be purged.
	 */
	if (tlb->fullmm) {
		flush_tlb_mm(tlb->mm);
	}
	put_cpu_var(mmu_gathers);
}

/*
 * tlb_remove_tlb_entry()
 *	Remove the tlb entry.
 *
 * Record the range for this vma.  The actual removal of the TLB
 * entries is delayed until tlb_end_vma() or tlb_finish_mmu().
 */
static inline void
tlb_remove_tlb_entry(struct mmu_gather *tlb, pte_t *ptep, unsigned long addr)
{
	/*
	 * No need to record anything, the entire mm (asid) will be
	 * purged on tlb_finish_mmu()
	 */
	if ((tlb->fullmm) || (tlb->kernel)) {
		return;
	}

	/*
	 * Record the range associated with the tlb entries to be purged.
	 * They will be purged at tlb_end_vma() time.
	 */
	if (addr < tlb->range_start) {
		tlb->range_start = addr;
	}

	if (addr + PAGE_SIZE > tlb->range_end) {
		tlb->range_end = addr + PAGE_SIZE;
	}
}


/*
 * tlb_start_vma()
 *	We are starting a removal for this vma.
 *
 */
static inline void
tlb_start_vma(struct mmu_gather *tlb, struct vm_area_struct *vma)
{
	/*
	 * No need to record anything, the entire mm (asid) will be
	 * purged on tlb_finish_mmu()
	 */
	if (tlb->fullmm) {
		return;
	}

	/*
	 * If this is a kernel space vma, do not bother
	 * tracking entries.  We never have a single vma
	 * that crosses the user/kernel space boundary.
	 */
	tlb->kernel = 0;
	if (vma->vm_start > TASK_SIZE) {
		tlb->kernel = 1;
		return;
	}

	/*
	 * TODO: The arm flushes the cache range here, do
	 * we need to do this?
	 */
	flush_cache_range(vma, vma->vm_start, vma->vm_end);
	tlb->range_start = TASK_SIZE;
	tlb->range_end = 0;
}

/*
 * tlb_end_vma()
 *	This vma is completely removed from the address space.
 */
static inline void
tlb_end_vma(struct mmu_gather *tlb, struct vm_area_struct *vma)
{
	/*
	 * No need to record anything, the entire mm (asid) will be
	 * purged on tlb_finish_mmu()
	 */
	if ((tlb->fullmm) || (tlb->kernel)) {
		return;
	}

	/*
	 * We are doing a partial address space remove, purge the
	 * saved range.
	 */
	if (tlb->range_end > 0) {
		flush_tlb_range(vma, tlb->range_start, tlb->range_end);
	}
}

#define tlb_remove_page(tlb,page)	free_page_and_swap_cache(page)
#define pte_free_tlb(tlb, ptep, addr)	pte_free((tlb)->mm, ptep)
#define pmd_free_tlb(tlb, pmdp, addr)	pmd_free((tlb)->mm, pmdp)
#define pud_free_tlb(tlb, pmdp, addr)	do { } while (0)
#define tlb_migrate_finish(mm)		do { } while (0)

extern void tlb_init(void);

#endif /* _ASM_UBICOM32_TLB_H */
