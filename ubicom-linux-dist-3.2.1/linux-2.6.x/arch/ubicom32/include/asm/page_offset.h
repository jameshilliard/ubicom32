/*
 * arch/ubicom32/include/asm/page_offset.h
 *   Definition of PAGE_OFFSET_RAW for Ubicom32 architecture.
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

#ifndef _ASM_UBICOM32_PAGE_OFFSET_H
#define _ASM_UBICOM32_PAGE_OFFSET_H

#include <asm/ubicom32.h>

/*
 * The PAGE_OFFSET_RAW is the physical offset (from 0) where
 * memory starts.  For Ubicom32, this is either the beginning
 * of OCM or if running virtually, it is the DDR address where
 * the kernel is loaded.
 */
#if defined(CONFIG_UBICOM32_MEMINIT_VIRTUAL)
#define	PAGE_OFFSET_RAW		KERNELSTART
#else
#define PAGE_OFFSET_RAW		(OCMSTART)
#endif

#endif /* _ASM_UBICOM32_PAGE_OFFSET_H */
