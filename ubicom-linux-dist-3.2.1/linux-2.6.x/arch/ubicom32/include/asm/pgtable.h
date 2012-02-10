/*
 * arch/ubicom32/include/asm/pgtable.h
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
#ifndef _ASM_UBICOM32_PGTABLE_H
#define _ASM_UBICOM32_PGTABLE_H

/*
 * The Ubicom32 architecture does not define a specific page table
 * implementation.  Given the page size (8K or 16K) and the 2.75 Gigabyte
 * VM address range, we will use a 2 level forward mapped page table.
 * This is accomplished by folding away the PMD.  It means some PMD
 * macros are still defined but fold away so that you just have a PGD
 * and a PTE page.
 */
#include <asm-generic/pgtable-nopmd.h>

/*
 * If the is NOMMU, redirect to the NOMMU pgtable.h
 */
#if !defined(CONFIG_MMU)
#include <asm/pgtable_nommu.h>
#endif

#if defined(CONFIG_MMU)

/*
 * These 2 have to be defined before linux/mm.h gets pulled in otherwise
 * is_vmalloc_addr will fail.
 */
#if defined(CONFIG_UBICOM32_MEMINIT_VIRTUAL)
#define VMALLOC_START ((unsigned long)memory_end)
#else
#define VMALLOC_START ((unsigned long)UBICOM32_FDPIC_KERNEL_START)
#endif
#define VMALLOC_END UBICOM32_FDPIC_KERNEL_END

#include <linux/mm.h>
#include <asm/segment.h>
#include <asm/page.h>
#include <asm/io.h>

#define PTE_ORDER 1
#define PTE_SHIFT	(PAGE_SHIFT)
#define BITS_PER_PTE	(PAGE_SHIFT - 2)
#define PTRS_PER_PTE	(1 << BITS_PER_PTE)

#define PGD_ORDER	1
#define PGDIR_SHIFT	(BITS_PER_PTE + PTE_SHIFT)
#define PGDIR_SIZE	(1 << PGDIR_SHIFT)
#define PGDIR_MASK	(~(PGDIR_SIZE - 1))
#define BITS_PER_PGD	(32 - PGDIR_SHIFT)
#define PTRS_PER_PGD	(1 << BITS_PER_PGD)

/*
 * Most values come from segment.h
 */
#define FIRST_USER_ADDRESS   0UL

#define kern_addr_valid(addr)   (1)

/*
 * Ubicom32 based PTE is a 32 bit entry.
 *	[16K Page size] pfn:18, zero: 2, soft/hard bits:12
 *      [ 8K Page size] pfn:19, zero: 1, soft/hard bits:12
 *	[ 4K Page size] pfn:20, zero: 0, soft/hard bits:12
 *
 * The combination of hardware and software bits is limited to 12
 * bits.
 */
#define PTE_VALID	(1 << 0)
#define PTE_USER	(1 << 1)
#define PTE_EXECUTE	(1 << 2)
#define PTE_WRITE	(1 << 3)
#define PTE_READ	(1 << 4)
#define PTE_NONCACHED	(1 << 5)
#define PTE_PTEC_FILL	(1 << 6)

/*
 * These are the Linux based PTE fields.  They include both
 * the hardware bits and additional software only bits.
 */
#define L_PTE_PRESENT		(PTE_VALID)
#define L_PTE_FILE		(PTE_USER) /* valid must be 0, 30 bits of offset */
#define L_PTE_CACHEABLE		(PTE_NONCACHED)
#define L_PTE_YOUNG		(PTE_PTEC_FILL)
#define L_PTE_BUFFERABLE	(1 << 7)
#define L_PTE_DIRTY		(1 << 8)
#define L_PTE_READ		(PTE_READ)
#define L_PTE_WRITE		(PTE_WRITE)
#define L_PTE_USER		(PTE_USER)
#define L_PTE_EXEC		(PTE_EXECUTE)
#define L_PTE_SHARED		(1 << 9)

/*
 * These are the Linux based PAGE types.
 */
#define PAGE_KERNEL		__pgprot(L_PTE_PRESENT | L_PTE_READ | L_PTE_WRITE)
#define PAGE_KERNEL_EXEC	__pgprot(L_PTE_PRESENT | L_PTE_READ | L_PTE_WRITE | L_PTE_EXEC)
#define PAGE_SHARED		__pgprot(L_PTE_PRESENT | L_PTE_USER | L_PTE_READ | L_PTE_WRITE)
#define PAGE_SHARED_EXEC	__pgprot(L_PTE_PRESENT | L_PTE_USER | L_PTE_READ | L_PTE_WRITE | L_PTE_EXEC)

#define _PAGE_CHG_MASK	(PAGE_MASK | L_PTE_DIRTY | L_PTE_YOUNG | L_PTE_CACHEABLE)

/*
 * The following form useful combinations of user based page protections.
 */
#define __PAGE_U___		__pgprot(L_PTE_PRESENT | L_PTE_USER)
#define __PAGE_U__R		__pgprot(L_PTE_PRESENT | L_PTE_USER | L_PTE_READ)
#define __PAGE_U_W_		__pgprot(L_PTE_PRESENT | L_PTE_USER | L_PTE_WRITE)
#define __PAGE_U_WR		__pgprot(L_PTE_PRESENT | L_PTE_USER | L_PTE_WRITE | L_PTE_READ)
#define __PAGE_UX__		__pgprot(L_PTE_PRESENT | L_PTE_USER | L_PTE_EXEC)
#define __PAGE_UX_R		__pgprot(L_PTE_PRESENT | L_PTE_USER | L_PTE_EXEC | L_PTE_READ)
#define __PAGE_UXW_		__pgprot(L_PTE_PRESENT | L_PTE_USER | L_PTE_EXEC | L_PTE_WRITE)
#define __PAGE_UXWR		__pgprot(L_PTE_PRESENT | L_PTE_USER | L_PTE_EXEC | L_PTE_WRITE | L_PTE_READ)

/*
 * See mm/mmap.c for the use of the protection bits. The following defines
 * are formed into an array which is indexed by VM_READ, VM_WRITE, VM_EXEC
 * VM_SHARED (see include/linux/mm.h).
 */
	/* xwr */
#define __P000 __PAGE_U___
#define __P001 __PAGE_U__R
#define __P010 __PAGE_U___ /* Copy On Write */
#define __P011 __PAGE_U__R /* Copy On Write */
#define __P100 __PAGE_UX__
#define __P101 __PAGE_UX_R
#define __P110 __PAGE_UX__ /* Copy On Write */
#define __P111 __PAGE_UX_R /* Copy On Write */

#define __S000 __PAGE_U___
#define __S001 __PAGE_U__R
#define __S010 __PAGE_U_W_
#define __S011 __PAGE_U_WR
#define __S100 __PAGE_UX__
#define __S101 __PAGE_UX_R
#define __S110 __PAGE_UXW_
#define __S111 __PAGE_UXWR

extern unsigned long empty_zero_page;
#define ZERO_PAGE(vaddr) (virt_to_page(empty_zero_page))

/*
 * set_pte()
 *	Write a PTE entry.
 *
 * Linux relies on the fact that writing a PTE will be atomic across all
 * logical CPUs.
 */
static inline void set_pte(pte_t *ptep, unsigned long pteval)
{
	ubicom32_write_reg((volatile unsigned long *)ptep, (unsigned long)pteval);
}

#define set_pte_at(mm, addr, ptep, pte) do { \
	set_pte(ptep, pte_val(pte)); \
 } while (0)

#define pte_ERROR(e) \
	printk("%s:%d: bad pte %08lx.\n", __FILE__, __LINE__, pte_val(e))

#define pfn_pte(pfn, prot)	__pte(((pfn) << (PAGE_SHIFT)) | pgprot_val(prot))
#define mk_pte(page, pgprot)    pfn_pte(page_to_pfn(page), (pgprot))
#define pte_page(x)		pfn_to_page(pte_pfn(x))
#define pte_pfn(x)		(pte_val(x) >> PAGE_SHIFT)
#define pte_none(x)     	(pte_val(x) == 0)
#define pte_present(x)		(pte_val(x) & L_PTE_PRESENT)
#define pte_clear(mm, addr, xp)	set_pte((xp), 0)
#define pte_special(pte) 	(0)
#define pte_bad(pte) 		(0)

#define pte_unmap(pte)           do { } while (0)
#define pte_unmap_nested(pte)    do { } while (0)

static inline pte_t pte_mkclean(pte_t pte)
{
	pte_val(pte) &= ~L_PTE_DIRTY;
	return pte;
}

static inline pte_t pte_mkold(pte_t pte)
{
	pte_val(pte) &= ~L_PTE_YOUNG;
	return pte;
}

static inline pte_t pte_wrprotect(pte_t pte)
{
	pte_val(pte) &= ~L_PTE_WRITE;
	return pte;
}

static inline pte_t pte_mkexec(pte_t pte)
{
	pte_val(pte) |= L_PTE_EXEC;
	return pte;
}

static inline pte_t pte_mkdirty(pte_t pte)
{
	pte_val(pte) |= L_PTE_DIRTY;
	return pte;
}

static inline pte_t pte_mkyoung(pte_t pte)
{
	pte_val(pte) |= L_PTE_YOUNG;
	return pte;
}

static inline pte_t pte_mkwrite(pte_t pte)
{
	pte_val(pte) |= L_PTE_WRITE;
	return pte;
}

static inline pte_t pte_modify(pte_t pte, pgprot_t newprot)
{
	pte_val(pte) = (pte_val(pte) & _PAGE_CHG_MASK) | pgprot_val(newprot);
	return pte;
}

static inline int pte_young(pte_t pte)
{
	return pte_val(pte) & L_PTE_YOUNG;
}

static inline int pte_file(pte_t pte)
{
	return pte_val(pte) & L_PTE_FILE;
}

static inline int pte_write(pte_t pte)
{
	return pte_val(pte) & L_PTE_WRITE;
}

static inline int pte_dirty(pte_t pte)
{
	return pte_val(pte) & L_PTE_DIRTY;
}

static inline pte_t pte_mkspecial(pte_t pte)
{
	return pte;
}


/*
 * This section provides macros for dealing with file PTE(s).
 *
 * Format of file pte:
 *	bit 	0	: valid bit (must be zero)
 *	bit	1	: L_PTE_FILE (must be one)
 *	bits	2-31	: page offset
 */
#define PTE_FILE_MAX_BITS	30
#define pte_to_pgoff(__pte)	((__pte).pte >> (32 - PTE_FILE_MAX_BITS))
#define pgoff_to_pte(off)	__pte((off) << (32 - PTE_FILE_MAX_BITS) | L_PTE_FILE)

/*
 * This section provides macros for dealing with swap PTEs.
 *
 * Format of file pte:
 *	bit 	0	: valid bit (must be zero)
 *	bit	1	: L_PTE_FILE (must be zero)
 *	bits	3-8	: swap type
 *	bits	9-31	: swap offset
 */
#define PTE_SWAP_TYPE_SHIFT 3
#define PTE_SWAP_OFFSET_SHIFT (PTE_SWAP_TYPE_SHIFT + 5)

#define __swp_type(x)		(((x).val >> PTE_SWAP_TYPE_SHIFT) & 0x1f)
#define __swp_offset(x)		((x).val >> PTE_SWAP_OFFSET_SHIFT)
#define __swp_entry(type,offset)	\
	((swp_entry_t) {(((offset) << PTE_SWAP_OFFSET_SHIFT)  | ((type) << PTE_SWAP_TYPE_SHIFT))})

#define __pte_to_swp_entry(pte)	((swp_entry_t){pte_val(pte)})
#define __swp_entry_to_pte(x)	((pte_t){(x).val})

/*
 * This section provides the macros for manipulating the root
 * page table.
 */
#define pgd_ERROR(e) \
	printk("%s:%d: bad pgd %08lx.\n", __FILE__, __LINE__, pgd_val(e))

#define pgd_none(pgd)		(pgd_val(pgd) == 0)
#define pgd_bad(pgd)		(0)
#define pgd_present(pgd)	(pgd_val(pgd) & PTE_VALID)
#define pgd_clear(pgd)		set_pte((pte_t *)pgd, 0)
#define pgd_index(addr)		((addr) >> PGDIR_SHIFT)
#define __pgd_offset(pgd, addr)	(((pgd_t *)(pgd)) + pgd_index(addr))
#define pgd_offset(mm, addr)	__pgd_offset((mm)->pgd, (addr))
#define pgd_offset_k(addr)	pgd_offset(&init_mm, (addr))

/*
 * This section provides the macros for manipulating a page of pointers
 * to a page of PTEs (middle page in a 3 tier page table).  It is folded
 * into our 2 level page table.
 */
#define pmd_none(pmd)		(pmd_val(pmd) == 0)
#define pmd_bad(pmd)		(0)
#define pmd_clear(pmd)		set_pte((pte_t *)pmd, 0)
#define pmd_present(pmd)	(pmd_val(pmd) & PTE_VALID)
#define pmd_pfn(x)		(pmd_val(x) >> PAGE_SHIFT)
#define pmd_address(x)		(pmd_pfn(x) << PAGE_SHIFT)
#define pmd_page(pmd)		(pfn_to_page(pmd_pfn(pmd)))
#define pmd_page_vaddr(pmd)	((unsigned long) __va(pmd_address(pmd)))

/*
 * This section provides the macros for manipulating a page of PTEs.
 */
#define __pte_index(addr)		(((addr) >> PTE_SHIFT) & (PTRS_PER_PTE - 1))
#define pte_offset_kernel(dir, addr)	(((pte_t *)pmd_page_vaddr(*(dir))) + __pte_index(addr))
#define pte_offset_map(dir, addr)	(((pte_t *)pmd_page_vaddr(*(dir))) + __pte_index(addr))
#define pte_offset_map_nested(dir, addr)(((pte_t *)pmd_page_vaddr(*(dir))) + __pte_index(addr))
#define pte_offset_map_phys(dir, addr)	(((pte_t *)pmd_address(*(dir))) + __pte_index(addr))

/*
 * Reference to our prototypical pgd.
 */
extern pgd_t swapper_pg_dir[PTRS_PER_PGD];

/*
 * io related page mapping.
 */
#define io_remap_pfn_range(vma, vaddr, pfn, size, prot)		\
		remap_pfn_range(vma, vaddr, pfn, size, prot)

#include <asm-generic/pgtable.h>

#define pgtable_cache_init() do { } while (0)

/*
 * externs and function dummies.
 */
static inline void check_pgt_cache(void) { };
extern void paging_init(void);
extern void init_kernel_page_tables(void);

#ifdef CONFIG_UBICOM32_MEMINIT_VIRTUAL
extern void cpu_virtual(void);
extern void cpu_physical(void);
#else
#define cpu_virtual(...) do {} while (0)
#define cpu_physical(...) do {} while (0)
#endif

/*
 * See arch/ubicom32/mm/elf-fdpic.c for implementation
 * of the mmap area selector.
 */
#define HAVE_ARCH_UNMAPPED_AREA 1

#endif /* CONFIG_MMU */
#endif	/* _ASM_UBICOM32_PGTABLE_H */
