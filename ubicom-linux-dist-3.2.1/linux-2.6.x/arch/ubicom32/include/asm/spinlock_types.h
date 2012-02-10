/*
 * arch/ubicom32/include/asm/spinlock_types.h
 *   Spinlock related structure definitions for Ubicom32 architecture.
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
#ifndef _ASM_UBICOM32_SPINLOCK_TYPES_H
#define _ASM_UBICOM32_SPINLOCK_TYPES_H

typedef struct {
	volatile unsigned int lock;
} arch_spinlock_t;

typedef struct {
	arch_spinlock_t lock;
	volatile int counter;
} arch_rwlock_t;

#define __ARCH_SPIN_LOCK_UNLOCKED	{ 0 }
#define __ARCH_RW_LOCK_UNLOCKED		{ __ARCH_SPIN_LOCK_UNLOCKED, 0 }

#endif /* _ASM_UBICOM32_SPINLOCK_TYPES_H */
