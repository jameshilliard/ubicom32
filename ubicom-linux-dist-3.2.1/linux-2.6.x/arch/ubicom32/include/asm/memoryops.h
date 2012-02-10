/*
 * arch/ubicom32/include/asm/memoryops.h
 *   Forward declarations for functions in arch/ubicom32/lib/memoryops.c
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
 *   arch/alpha
 */
#ifndef _ASM_UBICOM32_MEMORYOPS_H
#define _ASM_UBICOM32_MEMORYOPS_H

extern unsigned long __copy_from_user_zeroing(void *to, const void *from, 
					      unsigned long size);
extern long __strncpy_from_user(char *dst, const __user char *src, long count);
extern long __strnlen_user(const __user char *src, long max_n);
extern unsigned long __clear_user(__user void *to, unsigned long n);

#endif /* _ASM_UBICOM32_MEMORYOPS_H */
