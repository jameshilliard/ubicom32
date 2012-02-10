/*
 * arch/ubicom32/include/asm/mmu_ptec.h
 *   Interface to the Page Table Cache that Hardware "walks".
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
#ifndef _ASM_UBICOM32_MMU_PTEC_H
#define _ASM_UBICOM32_MMU_PTEC_H

/*
 * What size PTEC do we want?
 */
#define TLB_PTEC_ENTRIES_PER_WAY_BITS	8
#define TLB_PTEC_ENTRIES_PER_WAY	(1 << TLB_PTEC_ENTRIES_PER_WAY_BITS)
#define TLB_PTEC_ENTRIES_PER_WAY_MASK	(TLB_PTEC_ENTRIES_PER_WAY - 1)

/*
 * Some of the MMU fields are computed based on the size of the 
 * software PTEC.  The residual tag size is a function of the bits
 * that can fit in the abbreviated tag and the residual tag.
 */
#define MMU_ATAG_BITS		(16 - MMU_ASID_BITS)
#define MMU_ATAG_MASK		((1 << MMU_ATAG_BITS) - 1)
#define MMU_RTAG_BITS		(32 - MMU_ATAG_BITS - (TLB_PTEC_ENTRIES_PER_WAY_BITS + PAGE_SHIFT))
#define MMU_RTAG_MASK		((1 << MMU_RTAG_BITS) - 1)

/*
 * If we underflow the MMU_RTAG_BITS, we reset the values to 0.
 */
#if (MMU_RTAG_BITS < 0)
#undef MMU_RTAG_BITS
#undef MMU_RTAG_MASK
#define MMU_RTAG_BITS		(0)
#define MMU_RTAG_MASK		(0)
#endif

/*
 * A PTEC entry
 */
#define PTEC_ENTRY_PFN_SHIFT 12
#define PTEC_ENTRY_PFN_BITS 20
#if defined(CONFIG_UBICOM32_IP8K_SHUTTLE)
#define PTEC_ENTRY_RTAG_SHIFT 5
#define PTEC_ENTRY_RTAG_BITS 7
#else
#define PTEC_ENTRY_RTAG_SHIFT 6
#define PTEC_ENTRY_RTAG_BITS 6
#endif
#define PTEC_ENTRY_NONCACHE_SHIFT 5
#define PTEC_ENTRY_NONCACHE_BITS 1
#define PTEC_ENTRY_READ_SHIFT 4
#define PTEC_ENTRY_READ_BITS 1
#define PTEC_ENTRY_WRITE_SHIFT 3
#define PTEC_ENTRY_WRITE_BITS 1
#define PTEC_ENTRY_EXEC_SHIFT 2
#define PTEC_ENTRY_EXEC_BITS 1
#define PTEC_ENTRY_USER_SHIFT 1
#define PTEC_ENTRY_USER_BITS 1
#define PTEC_ENTRY_PAD_SHIFT 0
#define PTEC_ENTRY_PAD_BITS 1

/*
 * Mask used to clear RTAG bits from a PTE the bits are likely used by
 * Linux and must be cleared before loading into TLB.  Create a 16 bit
 * value for use with movei.
 */
#define PTEC_ENTRY_RTAG_MASK (((1 << PTEC_ENTRY_RTAG_BITS) - 1) << PTEC_ENTRY_RTAG_SHIFT)

#if !defined(__ASSEMBLY__)
struct ptec_tag {
	unsigned short tag0;
	unsigned short tag1;
};
#endif

#endif /* _ASM_UBICOM32_MMU_PTEC_H */
