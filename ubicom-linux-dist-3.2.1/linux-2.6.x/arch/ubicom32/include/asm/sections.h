/*
 * arch/ubicom32/include/asm/sections.h
 *   Generic sections.h definitions for Ubicom32 architecture.
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
#ifndef _ASM_UBICOM32_SECTIONS_H
#define _ASM_UBICOM32_SECTIONS_H

extern unsigned long __ocm_text_run_begin;
extern unsigned long __ocm_text_run_end;
extern unsigned long __ocm_data_run_begin;
extern unsigned long __ocm_data_run_end;

/*
 * arch_is_kernel_text()
 *	Returns true if addr is text but outside of .text.
 */
static inline int arch_is_kernel_text(unsigned long addr)
{
	if (addr >= (unsigned long)__ocm_text_run_begin &&
		 addr <  (unsigned long)__ocm_text_run_end) {
		return 1;
	}
	return 0;
}
#define arch_is_kernel_text(addr) arch_is_kernel_text(addr)

/*
 * arch_is_kernel_data()
 *	Returns true if the addr is a kernel address outside of .data.
 */
static inline int arch_is_kernel_data(unsigned long addr)
{
	if (addr >= (unsigned long)__ocm_data_run_begin &&
		 addr <  (unsigned long)__ocm_data_run_end) {
		return 1;
	}
	return 0;
}
#define arch_is_kernel_data(addr) arch_is_kernel_data(addr)

#include <asm-generic/sections.h>

#endif /* _ASM_UBICOM32_SECTIONS_H */
