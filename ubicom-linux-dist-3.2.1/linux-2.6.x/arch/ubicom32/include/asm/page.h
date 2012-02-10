/*
 * arch/ubicom32/include/asm/page.h
 *   Memory page related operations and definitions.
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
#ifndef _ASM_UBICOM32_PAGE_H
#define _ASM_UBICOM32_PAGE_H

#include <asm/types.h>
#include <asm/ubicom32.h>
#include <asm/setup.h>
#include <asm/page_offset.h>
#include <asm/segment.h>

/*
 * PAGE_SHIFT determines the page size
 */
#if defined(IP5000) || defined(IP7000)
#define PAGE_SHIFT	12
#endif

#if defined(IP8000)
#define PAGE_SHIFT	14
#endif

#define PAGE_SIZE		(1 << PAGE_SHIFT)
#define PAGE_MASK		(~(PAGE_SIZE - 1))

/*
 * Virtual address of the page represented by mem_map[0].
 */
#if defined(CONFIG_UBICOM32_MEMINIT_VIRTUAL)
#define PAGE_OFFSET		(UBICOM32_FDPIC_KERNEL_START)
#else
#define PAGE_OFFSET		(PAGE_OFFSET_RAW)
#endif

/*
 * Must be below the definition of PAGE_SHIFT
 */
#ifdef __KERNEL__
#include <linux/compiler.h>
#include <asm-generic/getorder.h>
#endif /* __KERNEL__ */

#ifndef __ASSEMBLY__
extern unsigned long memory_start;
extern unsigned long memory_end;

/*
 * Lowest conversion of kernel virtual to kernel physical.
 * Make sure that PAGE_OFFSET_RAW and UBICOM32_FDPIC_KERNEL_START
 * are a power of 2 away from each other.
 */
#define ARCH_PAGE_OFFSET	(PAGE_OFFSET_RAW - PAGE_OFFSET)
#if ((ARCH_PAGE_OFFSET & (ARCH_PAGE_OFFSET - 1)) != 0)
#error "Difference must be power of 2"
#endif
#define __pa(vaddr)		((unsigned long)(vaddr) | ARCH_PAGE_OFFSET)
#define __va(paddr)		((void *)((unsigned long)paddr & ~ARCH_PAGE_OFFSET))

/*
 * Convert pages to pfn and back again.
 */
#define ARCH_PFN_OFFSET		(PAGE_OFFSET_RAW >> PAGE_SHIFT)
#define pfn_valid(pfn)	        ((unsigned long)(pfn) - ARCH_PFN_OFFSET < max_mapnr)
#define pfn_to_page(pfn)	(mem_map + ((pfn) - ARCH_PFN_OFFSET))
#define page_to_pfn(page)	((unsigned long)((page) - mem_map) + ARCH_PFN_OFFSET)

/*
 * Macros used for converting between virtual and physical mappings.
 */
#define page_to_phys(page)      (pfn_to_phys(page_to_pfn((page))))
#define page_to_virt(page)	pfn_to_virt(page_to_pfn(page))

#define pfn_to_phys(pfn)	((pfn) << PAGE_SHIFT)
#define pfn_to_virt(pfn)	(__va(pfn_to_phys((pfn))))

#define phys_to_virt(paddr)	__va(paddr)

#define virt_to_phys(vaddr)	__pa(vaddr)
#define virt_to_pfn(vaddr)	(__pa(vaddr) >> PAGE_SHIFT)
#define virt_to_page(addr)	pfn_to_page(virt_to_pfn(addr))
#define	virt_addr_valid(kaddr)	(((void *)(kaddr) >= (void *)PAGE_OFFSET) && \
				((void *)(kaddr) < (void *)memory_end))

#define VM_DATA_DEFAULT_FLAGS	(VM_READ | VM_WRITE | VM_EXEC | \
				 VM_MAYREAD | VM_MAYWRITE | VM_MAYEXEC)

#define get_user_page(vaddr)			__get_free_page(GFP_KERNEL)
#define free_user_page(page, addr)		free_page(addr)
#define clear_page(page)			memset((page), 0, PAGE_SIZE)
#define copy_page(to,from)			memcpy((to), (from), PAGE_SIZE)
#define clear_user_page(page, vaddr, pg)	clear_page(page)
#define copy_user_page(to, from, vaddr, pg)	copy_page(to, from)

#define __alloc_zeroed_user_highpage(movableflags, vma, vaddr) \
	alloc_page_vma(GFP_HIGHUSER | __GFP_ZERO | movableflags, vma, vaddr)
#define __HAVE_ARCH_ALLOC_ZEROED_USER_HIGHPAGE

/*
 * These are used to make use of C type-checking..
 */
typedef struct page *pgtable_t;
typedef struct {unsigned long pte;} pte_t;
typedef struct {unsigned long pgd;} pgd_t;
typedef struct {unsigned long pgprot;} pgprot_t;

#define pte_val(x)	((x).pte)
#define pgd_val(x)	((x).pgd)
#define pgprot_val(x)	((x).pgprot)
#define __pte(x)	((pte_t){(x)})
#define __pgd(x)	((pgd_t){(x)})
#define __pgprot(x)	((pgprot_t){(x)})

#endif /* __ASSEMBLY__ */
#endif /* _ASM_UBICOM32_PAGE_H */
