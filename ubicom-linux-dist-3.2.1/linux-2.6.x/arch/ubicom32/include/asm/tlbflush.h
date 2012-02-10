/*
 * arch/ubicom32/include/asm/tlbflush.h
 *   TLB operations for Ubicom32 architecture.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 * Copyright (C) 2000 Lineo, David McCullough <davidm@uclinux.org>
 * Copyright (C) 2000-2002, Greg Ungerer <gerg@snapgear.com>
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
#ifndef _ASM_UBICOM32_TLB_FLUSH_H
#define _ASM_UBICOM32_TLB_FLUSH_H

#include <asm/setup.h>

extern struct   mm_struct init_mm;

extern void tlb_purge_all(void);
extern void tlb_purge_page(unsigned int asid, unsigned long address, int execute);
extern void tlb_purge_range(unsigned int asid, unsigned long start, unsigned long end, int execute);
extern void tlb_purge_asid(unsigned int asid);
extern void tlb_update_entry(int target, unsigned int asid, unsigned long addr, pte_t pte);
extern void tlb_asid_set_bank(int bits, int bank);

/*
 * flush_tlb_all()
 *	Flush all entries from the TLB.
 *
 */
static inline void flush_tlb_all(void)
{
	tlb_purge_all();
}

/*
 * flush_tlb_mm()
 *	Flush the specified memory map from the TLB.
 */
static inline void flush_tlb_mm(struct mm_struct *mm)
{
	tlb_purge_asid(mm->context.asid);
}

/*
 * flush_tlb_range()
 *	Flush the specified range from the TLB.
 */
static inline void flush_tlb_range(struct vm_area_struct *vma, unsigned long start, unsigned long end)
{
	tlb_purge_range(vma->vm_mm->context.asid, start, end, vma->vm_flags & VM_EXEC);
}

/*
 * flush_tlb_page()
 *	Flush the specific page from the TLB.
 */
static inline void flush_tlb_page(struct vm_area_struct *vma, unsigned long addr)
{
	tlb_purge_page(vma->vm_mm->context.asid, addr, vma->vm_flags & VM_EXEC);
}

/*
 * flush_tlb_kernel_range()
 *	Flush the kernel range from the TLB.
 *
 * Catch both D and I TLB.
 */
static inline void flush_tlb_kernel_range(unsigned long start, unsigned long end)
{
	tlb_purge_range(init_mm.context.asid, start, end, 1);
}

/*
 * flush_tlb_kernel_page()
 *	Flush the specified kernel page from the TLB.
 *
 * Catch both D and I TLB.
 */
static inline void flush_tlb_kernel_page(unsigned long addr)
{
	tlb_purge_page(init_mm.context.asid, addr, 1);
}

/*
 * update_mmu_cache()
 *	Update the recently modified  PTE entry in the MMU cache.
 */
static inline void update_mmu_cache(struct vm_area_struct *vma, unsigned long addr, pte_t *pte)
{
	unsigned long flags;
	int target;

	raw_local_irq_save(flags);
	target = (vma->vm_flags & VM_EXEC) ? 0 : 1;
	tlb_update_entry(target, vma->vm_mm->context.asid, addr, *pte);
	raw_local_irq_restore(flags);
}

#endif /* _ASM_UBICOM32_TLB_FLUSH_H */
