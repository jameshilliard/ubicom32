/*
 * arch/ubicom32/include/asm/segment.h
 *   Memory segment definitions for Ubicom32 architecture.
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
#ifndef _ASM_UBICOM32_SEGMENT_H
#define _ASM_UBICOM32_SEGMENT_H

/*
 * Ubicom32 shares a 32 bit address space between mapped and unmapped
 * space.  The use of the mapped region is entirely software driven.
 * The following segmentation defines the layout of memory.
 *
 * NOTE: If you change this layout, you might need to update:
 *
 * include/asm/pgtable.h: VMALLOC_START value
 * include/asm/processor.h: TASK_SIZE
 * mm/context.c: ASID mask and compare registers.
 *
 * 0x00000000		- PAGE_SIZE  	- Dead Page
 * PAGE_SIZE		- End Data	- Executable + Data
 * End Data		- MAPPED_START	- Break Area
 * MAPPED_START		- Stack	End	- Mapped Object Area
 * STACK_END		- Negative	- Stack
 *
 * KERNEL_START		- KERNEL_END	- Kernel Virtual Space
 * 0xb0000000		- 0xffffffff	- Unmapped Space
 */
#if defined(CONFIG_UBICOM32_MEMINIT_VIRTUAL)
#define UBICOM32_FDPIC_TEXT_START	(PAGE_SIZE)
#define UBICOM32_FDPIC_MAPPED_START	0x40000000
#define UBICOM32_FDPIC_STACK_END	0x80000000
#define UBICOM32_FDPIC_KERNEL_START	0x80100000
#define UBICOM32_FDPIC_KERNEL_END	UBICOM32_FDPIC_UNMAPPED_START
#define UBICOM32_FDPIC_UNMAPPED_START	0xb0000000
#else
#define UBICOM32_FDPIC_TEXT_START	(PAGE_SIZE)
#define UBICOM32_FDPIC_MAPPED_START	0x70000000
#define UBICOM32_FDPIC_STACK_END	UBICOM32_FDPIC_KERNEL_START
#define UBICOM32_FDPIC_KERNEL_START	0xa0000000
#define UBICOM32_FDPIC_KERNEL_END	UBICOM32_FDPIC_UNMAPPED_START
#define UBICOM32_FDPIC_UNMAPPED_START	0xb0000000
#endif

#ifndef __ASSEMBLY__

/*
 * ASID values shared for kernel, unique for user.
 */
#define UBICOM32_ASID_KERNEL 1
#define UBICOM32_ASID_START 2

/*
 * For non-MMU systems, all of physical memory is in the same segment.
 *
 * For MMU, the mapped space is divided into user and kernel space.
 * The unmapped space is also used for kernel.
 */
typedef struct {
	unsigned long seg;
} mm_segment_t;

#define MAKE_MM_SEG(s)	((mm_segment_t) { (s) })

#if !defined(CONFIG_MMU)
#define USER_DS		MAKE_MM_SEG(-1UL)
#define KERNEL_DS	MAKE_MM_SEG(-1UL)
#else
#define USER_DS		MAKE_MM_SEG(UBICOM32_FDPIC_KERNEL_START)
#define KERNEL_DS	MAKE_MM_SEG(-1UL)
#endif

#define get_ds()	(KERNEL_DS)
#define get_fs()	(current_thread_info()->addr_limit)
#define set_fs(x)	(current_thread_info()->addr_limit = (x))
#define segment_eq(a,b)	((a).seg == (b).seg)

#endif /* __ASSEMBLY__ */

#endif /* _ASM_UBICOM32_SEGMENT_H */
