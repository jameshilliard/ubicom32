/*
 * arch/ubicom32/include/asm/cachectl.h
 *   Ubicom32 cache control definitions.
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
#ifndef _ASM_UBICOM32_CACHECTL_H
#define _ASM_UBICOM32_CACHECTL_H

#include <asm/ubicom32.h>

extern void mem_d_cache_isa_flush_invalidate(unsigned long begin_addr, unsigned long end_addr);
extern void mem_i_cache_control(unsigned long begin_addr, unsigned long end_addr, unsigned long op);
extern void mem_d_cache_control(unsigned long begin_addr, unsigned long end_addr, unsigned long op);
#endif /* _ASM_UBICOM32_CACHECTL_H */
