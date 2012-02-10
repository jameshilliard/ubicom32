/*
 * arch/ubicom32/include/asm/pgalloc.h
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
#ifndef _ASM_UBICOM32_PGALLOC_H
#define _ASM_UBICOM32_PGALLOC_H
#if defined(CONFIG_MMU)

#include <linux/mm.h>

#include <asm/pgtable.h>

/*
 * pte_free_kernel()
 *	Free a kernel page of page tables.
 */
static inline void pte_free_kernel(struct mm_struct *mm, pte_t *pte)
{
	free_pages((unsigned long)pte, PTE_ORDER);
}

/*
 * pte_alloc_one_kernel()
 *	Allocate a free page for kernel ptes.
 */
static inline pte_t *pte_alloc_one_kernel(struct mm_struct *mm,	unsigned long address)
{
	pte_t *pte;
	pte = (pte_t *)__get_free_pages(GFP_KERNEL|__GFP_REPEAT|__GFP_ZERO, PTE_ORDER);
	return pte;
}

/*
 * pte_free()
 *	Free a page of PTEs.
 */
static inline void pte_free(struct mm_struct *mm, struct page *p)
{
	pgtable_page_dtor(p);
	__free_pages(p, PTE_ORDER);
}

/*
 * pte_alloc_one()
 *	Allocate and zero out a page for use as a PTE page.
 */
static inline struct page *pte_alloc_one(struct mm_struct *mm, unsigned long address)
{
	struct page *p;
	void *addr;

	p = alloc_pages(GFP_KERNEL | __GFP_REPEAT, PTE_ORDER);
	if (!p) {
		return p;
	}

	/*
	 * We can access every page using unmapped space.
	 */
	addr = (void *)page_to_phys(p);
	memset(addr, 0, PAGE_SIZE);
	pgtable_page_ctor(p);
	return p;
}

#define pmd_pgtable(pmd) 	pfn_to_page(pmd_pfn(pmd))
#define pmd_free(mm, x)		do { } while (0)

/*
 * __pmd_populate()
 *	Atomically update the pmd entry with pmdval.
 */
static inline void __pmd_populate(pmd_t *pmd, unsigned long pmdval)
{
	ubicom32_write_reg((volatile unsigned *)pmd, pmdval);
}

/*
 * pmd_populate_kernel()
 *	Write a single pmd entry with the value of the pte.
 */
static inline void pmd_populate_kernel(struct mm_struct *mm, pmd_t *pmd,
	pte_t *ptep)
{
	__pmd_populate(pmd, (unsigned long)pte_val(*ptep));
}

/*
 * pmd_populate()
 *	Write a single pmd entry for the newly assigned page of ptes.
 */
static inline void pmd_populate(struct mm_struct *mm, pmd_t *pmd,
	pgtable_t page)
{
	__pmd_populate(pmd, pte_val(mk_pte(page, PAGE_KERNEL)));
}

/*
 * pgd_free()
 *	Free the page directory root page table.
 */
static inline void pgd_free(struct mm_struct *mm, pgd_t *pgd)
{
	free_pages((unsigned long)pgd, PTE_ORDER);
}

/*
 * pgd_alloc()
 *	Allocate and zero a root page directory.
 *
 * TODO: Instead of allocating a whole page, we should use the kernel
 * allocator and allocate a small 64 entry (256 byte chunk) of memory.
 * Because of the MISSQ register the chunk must be aligned on 2^8
 * boundary.
 */
static inline pgd_t *pgd_alloc(struct mm_struct *mm)
{
	pgd_t *pgd;
	pgd = (pgd_t *)__get_free_pages(GFP_KERNEL|__GFP_REPEAT, PTE_ORDER);
	if (!pgd) {
		return NULL;
	}

	memcpy(pgd, swapper_pg_dir, PTRS_PER_PGD * sizeof(pgd_t));

	return pgd;
}

#endif /* CONFIG_MMU */
#endif	/* _ASM_UBICOM32_PGALLOC_H */
