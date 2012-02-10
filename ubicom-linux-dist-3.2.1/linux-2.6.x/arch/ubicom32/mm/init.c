/*
 * arch/ubicom32/mm/init.c
 *   Ubicom32 architecture virtual memory initialization.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *  Copyright (C) 1998  D. Jeff Dionne <jeff@lineo.ca>,
 *                      Kenneth Albanowski <kjahds@kjahds.com>,
 *  Copyright (C) 2000  Lineo, Inc.  (www.lineo.com)
 *
 *  Based on:
 *
 *  linux/arch/m68k/mm/init.c
 *
 *  Copyright (C) 1995  Hamish Macdonald
 *
 *  JAN/1999 -- hacked to support ColdFire (gerg@snapgear.com)
 *  DEC/2000 -- linux 2.4 support <davidm@snapgear.com>
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

#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/ptrace.h>
#include <linux/mman.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/init.h>
#include <linux/highmem.h>
#include <linux/pagemap.h>
#include <linux/bootmem.h>
#include <linux/slab.h>
#include <linux/initrd.h>

#include <asm/setup.h>
#include <asm/segment.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/system.h>
#include <asm/machdep.h>
#include <asm/ocm-alloc.h>
#include <asm/processor.h>
#include <asm/tlb.h>
#include <asm/mmu_hardware.h>
#include <asm/mmu_context.h>
#include <asm/ocmdata_alloc.h>

#undef DEBUG

extern void free_initmem(void);
extern unsigned long memory_start;
extern unsigned long memory_end;
extern char __ocm_free_begin;
extern char __ocm_free_end;

#if defined(CONFIG_MMU)
/*
 * PMDs for the mapped kernel range.
 */
#define UBICOM32_PHYSMEM_PTES ((UBICOM32_FDPIC_KERNEL_START - UBICOM32_FDPIC_KERNEL_END) >> PAGE_SHIFT)
pte_t kernel_mapped_ptes[UBICOM32_PHYSMEM_PTES] __attribute__((aligned(PAGE_SIZE)));

/*
 * Base PGD from which all others are inherited.
 */
pgd_t swapper_pg_dir[PTRS_PER_PGD] __attribute__((aligned(PAGE_SIZE)));
#endif

DEFINE_PER_CPU(struct mmu_gather, mmu_gathers);

/*
 * ZERO_PAGE is a special page that is used for zero-initialized
 * data and COW.
 */
unsigned long empty_zero_page;

#ifdef CONFIG_UBICOM32_MEMINIT_VIRTUAL
/*
 * Indicates if cpu can be virtalized
 */
static int kernel_virtual_established = 0;
#endif

/*
 * show_mem()
 *	Show memory information.
 */
void show_mem(void)
{
	unsigned long i;
	int free = 0, total = 0, reserved = 0, shared = 0;
	int cached = 0;

	printk(KERN_INFO "\nMem-info:\n");
	show_free_areas();
	i = max_mapnr;
	while (i-- > 0) {
		total++;
		if (PageReserved(mem_map+i))
			reserved++;
		else if (PageSwapCache(mem_map+i))
			cached++;
		else if (!page_count(mem_map+i))
			free++;
		else
			shared += page_count(mem_map+i) - 1;
	}
	printk(KERN_INFO "%d pages of RAM\n",total);
	printk(KERN_INFO "%d free pages\n",free);
	printk(KERN_INFO "%d reserved pages\n",reserved);
	printk(KERN_INFO "%d pages shared\n",shared);
	printk(KERN_INFO "%d pages swap cached\n",cached);
}

#ifdef CONFIG_UBICOM32_MEMINIT_VIRTUAL
/*
 * cpu_virtual()
 *	Make this logical cpu run virtual.
 */
void cpu_virtual(void)
{
	if (!kernel_virtual_established) {
		return;
	}

	unsigned long newsp = (unsigned long)phys_to_virt(current_stack_pointer);
	asm volatile (
	"	move.4 sp, %[newsp]	\n\t"
	:
	: [newsp]"r"(newsp)
	);

	/*
	 * Since current thread info is based on the stack pointer which
	 * is now virtual, make the sw_ksp[] virtual as well.
	 */
	sw_ksp[thread_get_self()] = (unsigned int)current_thread_info();
}

/*
 * cpu_physical
 *	Convert back to physical.
 */
void cpu_physical(void)
{
	unsigned long newsp = (unsigned long)virt_to_phys(current_stack_pointer);
	asm volatile (
	"	move.4 sp, %[newsp]	\n\t"
	:
	: [newsp]"r"(newsp)
	);

	/*
	 * Since current thread info is based on the stack pointer which
	 * is now physical, make the sw_ksp[] physical as well.
	 */
	sw_ksp[thread_get_self()] = (unsigned int)current_thread_info();
}
#endif

/*
 * init_kernel_page_tables()
 *	Setup the kernel page tables and TLB.
 */
#if defined(CONFIG_MMU)
void __init init_kernel_page_tables(void)
{
	int i;
	pte_t *ptep;
#if defined(CONFIG_UBICOM32_MEMINIT_VIRTUAL)
	unsigned long addr;
#endif

	/*
	 * Initialize the TLB hardware before any of the contexts
	 * are assigned (which causes writes to the TLB hardware).
	 */
	tlb_init();

	/*
	 * Configure and assign the kernel's mm structure.
	 */
	init_mm.pgd = phys_to_virt(swapper_pg_dir);
	init_new_context(&init_task, &init_mm);
	context_assign_kernel();

	/*
	 * Initialize the default PGD for all of kernel space.
	 */
	memset(swapper_pg_dir, 0, sizeof(swapper_pg_dir));
	memset(kernel_mapped_ptes, 0, sizeof(kernel_mapped_ptes));

	/*
 	 * Create an entry in the PGD for every PMD (including vmalloc
	 * area).
	 */
	ptep = kernel_mapped_ptes;
	for (i = UBICOM32_FDPIC_KERNEL_START / (PAGE_SIZE * PTRS_PER_PTE);
			i < UBICOM32_FDPIC_KERNEL_END / (PAGE_SIZE * PTRS_PER_PTE); i++) {
		unsigned int pfn = ((unsigned int)ptep) >> PAGE_SHIFT;
		pte_t pte = (pte_t)pfn_pte(pfn, PAGE_KERNEL);
		pmd_t *pmd = (pmd_t *)(swapper_pg_dir + i);
		pmd_populate_kernel(&init_mm, pmd, &pte);
		ptep += (PAGE_SIZE / sizeof(pte_t));
	}

#if defined(CONFIG_UBICOM32_MEMINIT_VIRTUAL)
	/*
	 * Create a PTE entry for each page as a linear offset from START.
	 */
	for (addr = UBICOM32_FDPIC_KERNEL_START; addr < memory_end; addr += PAGE_SIZE) {
		phys_addr_t paddr = virt_to_phys(addr);
		unsigned int pfn = paddr >> PAGE_SHIFT;
		pte_t pte = (pte_t)pfn_pte(pfn, PAGE_KERNEL_EXEC);
		pmd_t *pmd = (pmd_t *)__pgd_offset(swapper_pg_dir, addr);
		if (unlikely(pmd_none(*pmd)) ||
		    (unlikely(pmd_bad(*pmd))) ||
		    (unlikely(pmd_present(*pmd) == 0))) {
			printk(KERN_CRIT "no pmd created for paddr: %p\n", (void *)paddr);
			THREAD_STALL;
			break;
		}

		ptep = pte_offset_map_phys(pmd, addr);
		pmd_populate_kernel(&init_mm, (pmd_t *)ptep, &pte);
		tlb_update_entry(1, UBICOM32_ASID_KERNEL, addr, pte);
	}

	/*
	 * At this point we can run virtual.
	 */
	kernel_virtual_established = 1;
	cpu_virtual();
#endif
}
#endif

/*
 * paging_init()
 *	Initializing the paging portion of the kernel memory.
 */
void __init paging_init(void)
{
	unsigned long zones_size[MAX_NR_ZONES] = {0};

	/*
	 * Configure the zone_size array for each zone and init the free
	 * memory based on the zones.  All of the zone lists are an
	 * offset from PAGE_OFFSET.
	 */
	zones_size[ZONE_NORMAL] = ((memory_end - PAGE_OFFSET) >> PAGE_SHIFT);

	free_area_init(zones_size);

	/*
	 * Initialize the zero page.
	 */
	empty_zero_page = (unsigned long)alloc_bootmem_pages(PAGE_SIZE);
	memset((void *)empty_zero_page, 0, PAGE_SIZE);
}

/*
 * mem_init()
 *	Free the boot memory into the main memory pool.
 */
void __init mem_init(void)
{
	int codek = 0, datak = 0, initk = 0;
	unsigned long tmp, ram_start, ram_end, len;
	extern char _etext, _stext, _sdata, _ebss, __init_begin, __init_end;
	unsigned long end_mem = memory_end;

#ifdef DEBUG
	unsigned long start_mem = PAGE_ALIGN(memory_start);
	printk(KERN_DEBUG "mem_init: start=%lx, end=%lx\n", start_mem, end_mem);
#endif

	/*
	 * Set num_physpages and max_mapnr for the kernel.
	 */
	end_mem &= PAGE_MASK;
	high_memory = (void *)end_mem;
	num_physpages = (((unsigned long) high_memory) - PAGE_OFFSET) >> PAGE_SHIFT;
	max_mapnr = num_physpages;

	/*
	 * Make sure that the free OCM area is marked as free.
	 */
	{
		unsigned long ocm_free_begin = (unsigned long)&__ocm_free_begin;
		unsigned long ocm_free_end = (unsigned long)&__ocm_free_end;
		ocmdata_init(ocm_free_begin, ocm_free_end - ocm_free_begin);
	}

	/* 
	 * Place remaining boot memory into the main free list.
	 */
	totalram_pages = free_all_bootmem();

	/*
	 * Standard memory available printk.
	 */
	processor_dram(&ram_start, &ram_end);
	len = (ram_end - ram_start) + OCMSIZE;
	codek = (&_etext - &_stext) >> 10;
	datak = (&_ebss - &_sdata) >> 10;
	initk = (&__init_begin - &__init_end) >> 10;
	tmp = nr_free_pages() << PAGE_SHIFT;
	printk(KERN_INFO "Memory available: %luk/%luk RAM, (%dk kernel code, %dk data)\n",
	       tmp >> 10,
	       len >> 10,
	       codek,
	       datak);
}

/*
 * free_init_pages()
 *	Common code for free_initrd_mem and free_initmem
 */
static void __init free_init_pages(const char *what, unsigned long begin, unsigned long end)
{
	unsigned long addr;
	/*
	 * The following code should be cool even if these sections
	 * are not page aligned.
	 */
	addr = PAGE_ALIGN(begin);
	/* next to check that the page we free is not a partial page */
	for (addr = begin; addr + PAGE_SIZE <= end; addr += PAGE_SIZE) {
		ClearPageReserved(virt_to_page(addr));
		init_page_count(virt_to_page(addr));
		free_page(addr);
		totalram_pages++;
	}
	printk(KERN_NOTICE "Freeing %s: %ldk freed (0x%x - 0x%x)\n",
			what,
			(addr - PAGE_ALIGN(begin)) >> 10,
			(int)(PAGE_ALIGN(begin)),
			(int)(addr - PAGE_SIZE));
}

/*
 * free_initmem()
 *	Free init memory back to heap.
 */
void free_initmem()
{
	extern char __init_begin, __init_end;
	free_init_pages("unused kernel memory", (long unsigned int)&__init_begin, (long unsigned int)&__init_end);
}

#ifdef CONFIG_BLK_DEV_INITRD
/*
 * Manage initrd
 */

/*
 * free_initrd_mem()
 *	Free the initrd memory.
 */
void free_initrd_mem(unsigned long start, unsigned long end)
{
	free_init_pages("initrd", start, end);
}

/*
 * rd_start_early()
 *	Set the start and end values for ramfs.
 */
static int __init rd_start_early(char *p)
{
	unsigned long start = memparse(p, &p);

	initrd_start = start;
	initrd_end += start;
	return 0;
}
early_param("rd_start", rd_start_early);

/*
 * rd_size_early()
 *	Set the size of ramfs.
 */
static int __init rd_size_early(char *p)
{
	initrd_end += memparse(p, &p);
	return 0;
}
early_param("rd_size", rd_size_early);
#endif /* CONFIG_BLK_DEV_INITRD */

