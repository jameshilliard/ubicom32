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
 *  linux/arch/frv/mm/elf-fdpic.c
 *
 * Copyright (C) 2004 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
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
 *   arch/frv
 *
 */

#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/elf-fdpic.h>

#include <asm/segment.h>
#include <asm/mman.h>
#include <asm/elf.h>

// #define DEBUG_UBICOM32_FDPIC 1
#if defined(DEBUG_UBICOM32_FDPIC)
#define kdebug(fmt, ...) printk(KERN_INFO "[LAYOUT] "fmt, ##__VA_ARGS__ )
#else
#define kdebug(fmt, ...) do {} while(0)
#endif

/*
 * We use the ELF page size to make sure that we round
 * to the proper sizes when computing data sections.
 */
#define ELF_EXEC_PAGE_ALIGN(addr) ALIGN((addr), ELF_EXEC_PAGESIZE)

/*
 * elf_fdpic_debug_print()
 *	Print the general params and header information.
 */
#if defined(DEBUG_UBICOM32_FDPIC)
static void elf_fdpic_debug_print(const char *s, struct elf_fdpic_params *params)
{
	int i;
	struct elf32_phdr *phdr;

	switch (params->flags & ELF_FDPIC_FLAG_ARRANGEMENT) {
	case ELF_FDPIC_FLAG_INDEPENDENT:
		kdebug("%s: flag = INDEPENDENT\n", s);
		break;
	case ELF_FDPIC_FLAG_HONOURVADDR:
		kdebug("%s: flag = HONOURVADDR\n", s);
		break;
	case ELF_FDPIC_FLAG_CONSTDISP:
		kdebug("%s: flag = CONSTDISP\n", s);
		break;
	case ELF_FDPIC_FLAG_CONTIGUOUS:
		kdebug("%s: flag = CONTIGUOUS\n", s);
		break;
	default:
		kdebug("%s: flag = UNKNOWN\n", s);
		break;
	}

	kdebug("%s: dynamic_addr: %lx\n", s, params->entry_addr);
	kdebug("%s: entry_addr: %lx\n", s, params->entry_addr);
	kdebug("%s: e_entry: %lx\n", s, (unsigned long)params->hdr.e_entry);

	phdr = params->phdrs;
	for (i = 0; i < params->hdr.e_phnum; i++, phdr++) {
		kdebug("%s: type: %lx, flags: %lx, va: %lx, of: %lx, fs: %lx, ms: %lx\n",
		       s,
		       (unsigned long)phdr->p_type,
		       (unsigned long)phdr->p_flags,
		       (unsigned long)phdr->p_vaddr,
		       (unsigned long)phdr->p_offset,
		       (unsigned long)phdr->p_filesz,
		       (unsigned long)phdr->p_memsz);

		/*
		 * Make sure that the page size of the program header
		 * is a multiple of the system page size.  Report an error
		 * if there is a mismatch.  The load will ultimately fail.
		 */
		if (phdr->p_type == PT_LOAD) {
			unsigned long disp = (phdr->p_vaddr & ~PAGE_MASK);
			unsigned long offset = phdr->p_offset - disp;
			if ((offset & ~PAGE_MASK) != 0) {
				kdebug("ERROR: offset[%lx] not a multiple of page size: %lx\n\n",
				       (unsigned long)offset, 
				       (unsigned long)PAGE_SIZE);
			}
		}
	}
}
#endif

/*
 * elf_fdpic_total_size()
 *	Compute the total size of the elf loadable sections.
 */
static unsigned long elf_fdpic_total_size(struct elf_fdpic_params *params)
{
	int i;
	int cnt = 0;
	unsigned long sz = 0;
	struct elf32_phdr *phdr = params->phdrs;

	for (i = 0; i < params->hdr.e_phnum; i++, phdr++) {
		if (phdr->p_type != PT_LOAD) {
			continue;
		}
		sz += ELF_EXEC_PAGE_ALIGN(phdr->p_memsz);
		cnt++;
	}

	/*
	 * Each section can rounded up to another page; so we need
	 * to account for at least PAGE_SIZE round up on each 
	 * section.
	 */
	sz += (cnt << (ELF_EXEC_PAGESHIFT));
	return sz;
}

#if defined(HAVE_ARCH_UNMAPPED_AREA)
/*
 * arch_get_unmapped_area()
 *
 * NOTE: The caller of this function expects either an address or
 * -<error>.  It assumes that the address returned will be page aligned
 * and thus negative error values will not be.
 *
 * NOTE: We supply our own function here because the generic version would
 * allocate mappings before MAPPED_START.
 */
unsigned long arch_get_unmapped_area(struct file *f, unsigned long addr, unsigned long len,
				     unsigned long pgoff, unsigned long flags)
{
	struct vm_area_struct *vma;
	unsigned long limit;

	kdebug("request: addr[%lx], len[%lx], pgoff[%lx], flags[%lx]: %s\n",
	       addr, len, pgoff, flags, 
	       f ? (char *)f->f_path.dentry->d_name.name : "");

	if ((addr + len) > TASK_SIZE) {
		kdebug("too big: request[%lx], task[%lx]\n", 
		       addr + len, TASK_SIZE);
		return -ENOMEM;
	}

	/*
	 * Compute the size of the file and make sure that we stay within it.
	 */
	if (f) {
		struct address_space *mapping = f->f_mapping;
		struct inode *inode = mapping->host;
		pgoff_t size = (pgoff + (PAGE_ALIGN(len) >> PAGE_SHIFT));
		limit = (PAGE_ALIGN(i_size_read(inode)) >> PAGE_SHIFT);
		if (limit && (size > limit)) {
			kdebug("request[%lx] exceeds file size[%lx]\n",
			      (unsigned long)size, (unsigned long)limit);
			return ((flags & MAP_FIXED) ? addr : -EINVAL);
		}
	}

	/* 
	 * handle MAP_FIXED 
	 */
	if (flags & MAP_FIXED) {
		kdebug("fixed addr: addr[%lx]\n", addr);
		return addr;
	}

	/* 
	 * Without MAP_FIXED, but with addr this is a hint.  Only honor
	 * a hint if we're not going to clobber something doing so.
	 */
	if (addr) {
		addr = PAGE_ALIGN(addr);
		vma = find_vma(current->mm, addr);
		if (!vma || ((addr + len) <= vma->vm_start)) {
			goto success;
		}
		kdebug("address[%lx] hint overlays existing vma: %p, ignored\n", 
		       addr, vma);
	}

	/* 
	 * Perform a search of the reserved area for mapped
	 * addresses.
	 *
	 * TODO: If this fails, do we want to try again over the 
	 * entire mapped space?
	 */
	addr = PAGE_ALIGN(UBICOM32_FDPIC_MAPPED_START);
	limit = PAGE_ALIGN(UBICOM32_FDPIC_STACK_END);
	vma = find_vma(current->mm, addr);
	while (vma) {
		/*
		 * Stop searching if we reach the limit.
		 */
		if (addr > limit) {
			break;
		}

		/*
		 * We found a hole where the request will
		 * fit.
		 */
		if (addr + len <= vma->vm_start) {
			goto success;
		}

		/*
		 * Try searching from the end of this vma.
		 */
		addr = vma->vm_end;
		vma = vma->vm_next;
	}

	/*
	 * Is there room at the end?
	 */
	if (!vma && ((addr + len) <= limit)) {
		goto success;
	}

	kdebug("failed\n");
	return -ENOMEM;

success:
       kdebug("success: %lx\n", addr);
	return addr;
}
#endif

/*
 * elf_fdpic_arch_lay_out_mm()
 *	Layout memory management for the new FDPIC based executable.
 *
 * TODO: The long term goal is to keep the executable within a single
 * 64MB range (if possible).  This limits the number of page tables that
 * are necessary to represent a file.
 */
void elf_fdpic_arch_lay_out_mm(struct elf_fdpic_params *exec_params,
			       struct elf_fdpic_params *interp_params,
			       unsigned long *start_stack,
			       unsigned long *start_brk)
{
#if defined(DEBUG_UBICOM32_FDPIC)
	elf_fdpic_debug_print("exec", exec_params);
	elf_fdpic_debug_print("interp", interp_params);
#endif

	/*
	 * Set the stack address and data addresses.
	 */
	*start_stack = UBICOM32_FDPIC_STACK_END;
	*start_brk = ELF_EXEC_PAGE_ALIGN(UBICOM32_FDPIC_TEXT_START + 
			elf_fdpic_total_size(exec_params));

	/* 
	 * If the only executable is a shared object, assume that it is
	 * an interpreter rather than a true executable, and map it such
	 * that "ld.so --list" comes out right
	 */
	if (!(interp_params->flags & ELF_FDPIC_FLAG_PRESENT) &&
	    exec_params->hdr.e_type != ET_EXEC) {
		exec_params->load_addr = PAGE_SIZE;
		kdebug("Using ld.so section\n");
		return;
	}

	/*
	 * True executable, set the load addr and indicate constant
	 * displacement for its address assignment.
	 */
	exec_params->load_addr = UBICOM32_FDPIC_TEXT_START;
	if ((exec_params->flags & ELF_FDPIC_FLAG_ARRANGEMENT) ==
	    ELF_FDPIC_FLAG_INDEPENDENT) {
		exec_params->flags &= ~ELF_FDPIC_FLAG_ARRANGEMENT;
		exec_params->flags |= ELF_FDPIC_FLAG_CONSTDISP;
	}

	kdebug("stack[%lx], brk[%lx], load[%lx], flags[%lx]\n",
	       *start_stack,
	       *start_brk,
	       exec_params->load_addr,
	       exec_params->flags); 
}
