/*
 * arch/ubicom32/include/asm/cacheflush.h
 *   Cache flushing definitions for Ubicom32 architecture.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 * (C) Copyright 2000-2004, Greg Ungerer <gerg@snapgear.com>
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
#ifndef _ASM_UBICOM32_CACHEFLUSH_H
#define _ASM_UBICOM32_CACHEFLUSH_H

#include <linux/mm.h>

#include <asm/page.h>
#include <asm/cachectl.h>

#define ARCH_IMPLEMENTS_FLUSH_DCACHE_PAGE 1
/*
 * mem_cache_invalidate_all()
 *	Reset a specific cache using the cache control registers.
 */
static inline void mem_cache_invalidate_all(unsigned long cc)
{
	if (cc == DCCR_BASE)
		UBICOM32_LOCK(DCCR_LOCK_BIT);
	else
		UBICOM32_LOCK(ICCR_LOCK_BIT);

	asm volatile (
	"	bset	"D(CCR_CTRL)"(%0), "D(CCR_CTRL)"(%0), #"D(CCR_CTRL_RESET)"	\n\t"
	"	nop									\n\t"
	"	bclr	"D(CCR_CTRL)"(%0), "D(CCR_CTRL)"(%0), #"D(CCR_CTRL_RESET)"	\n\t"
	"	pipe_flush 0								\n\t"
		:
		: "a" (cc)
		: "cc"
	);

	if (cc == DCCR_BASE)
		UBICOM32_UNLOCK(DCCR_LOCK_BIT);
	else
		UBICOM32_UNLOCK(ICCR_LOCK_BIT);

}

/*
 * __flush_cache_call()
 *	purge both the I & D caches.
 */
static inline void __flush_cache_all(void)
{
	/*
	 * Purge Icache
	 */
	mem_cache_invalidate_all(ICCR_BASE);

	/*
	 * Purge Dcache
	 */
	mem_cache_invalidate_all(DCCR_BASE);
}

/*
 * Linux flush macros for changing the VM.  Used on machines
 * that have virtually indexed caches.  Ubicom32 is a physically
 * indexed, physically tagged cache.
 */
#define flush_cache_all()			__flush_cache_all()
#define flush_cache_mm(mm)			do { } while (0)
#define flush_cache_dup_mm(mm)			do { } while (0)
#define flush_cache_range(vma, start, end)      do { } while (0)
#define flush_cache_page(vma, vmaddr, pfn)	do { } while (0)
#define flush_cache_vmap(start, end)		do { } while (0)
#define flush_cache_vunmap(start, end)		do { } while (0)
#define flush_dcache_page(page)			do { } while (0)
#define flush_dcache_mmap_lock(mapping)		do { } while (0)
#define flush_dcache_mmap_unlock(mapping)	do { } while (0)
#define flush_icache_user_range(vma,pg,adr,len)	do { } while (0)

/*
 * Linux flush macros for clearing a range of kernel virtual
 * or physical addresses.
 */
#define flush_dcache_range(start, end)					\
do {									\
	/* Flush the data cache. */					\
	mem_d_cache_isa_flush_invalidate(start, end);			\
} while (0)

#define flush_icache_range(start, end)					\
do {									\
	flush_dcache_range(start, end);					\
	mem_i_cache_control(start, end, CCR_CTRL_INV_ADDR);		\
} while (0)

#define flush_icache_page(vma, pg)					\
do {									\
	unsigned long start = (unsigned long)page_to_virt(pg);		\
	unsigned long end = start + PAGE_SIZE;				\
	flush_dcache_range(start, end);					\
	mem_i_cache_control(start, end, CCR_CTRL_INV_ADDR);		\
} while (0)

/*
 * copy_from_user_page()
 *	Copy from user space to kernel.
 */
#define copy_from_user_page(vma, page, vaddr, dst, src, len) \
	memcpy(dst, src, len)

/*
 * copy_to_user_page()
 *	Copy the kernel page to a user page and flush if exec.
 */
static inline void copy_to_user_page(struct vm_area_struct *vma,
				     struct page *page,
				     unsigned long vaddr,
				     void *dst,
				     const void *src,
				     unsigned long len)
{
	memcpy(dst, src, len);

	/*
	 * If the page has execute permission, flush/purge the
	 * entire page.  This is due to the fact that ptrace
	 * can bring the page into memory and this function
	 * is responsible for ensuring that the page is properly
	 * setup for user space access (not just the range
	 * that the ptrace modified).
	 */
	if (vma->vm_flags & VM_EXEC) {
		flush_icache_page(vma, page);
	}
}

#endif /* _ASM_UBICOM32_CACHEFLUSH_H */
