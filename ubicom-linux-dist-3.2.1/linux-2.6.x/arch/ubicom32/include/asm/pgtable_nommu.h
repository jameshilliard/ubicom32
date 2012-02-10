/*
 * arch/ubicom32/include/asm/pgtable_nommu.h
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
#ifndef _ASM_UBICOM32_PGTABLE_NOMMU_H
#define _ASM_UBICOM32_PGTABLE_NOMMU_H

#define VMALLOC_START   0
#define VMALLOC_END     0xffffffff
#define PAGE_KERNEL	__pgprot(0)

extern unsigned long empty_zero_page;
#define ZERO_PAGE(vaddr) (virt_to_page(empty_zero_page))

#define kern_addr_valid(addr)   (1)

#define swapper_pg_dir ((pgd_t *) 0)
#define pgtable_cache_init() do { } while (0)

#define pte_file(pte) 0

#define __swp_type(x)		(((x).val >> 10) & 0x1f)
#define __swp_offset(x)		((x).val >> 15)
#define __swp_entry(type, offset)	\
	((swp_entry_t) { ((type) << 10) | ((offset) << 15) })
#define __pte_to_swp_entry(pte)	((swp_entry_t) { pte_val(pte) })
#define __swp_entry_to_pte(x)	((pte_t) { (x).val })

#define io_remap_pfn_range(vma, vaddr, pfn, size, prot)		\
		remap_pfn_range(vma, vaddr, pfn, size, prot)

#include <asm-generic/pgtable.h>

static inline void check_pgt_cache(void) {};
extern void paging_init(void);

#endif	/* _ASM_UBICOM32_PGALLOC_NOMMU_H */
