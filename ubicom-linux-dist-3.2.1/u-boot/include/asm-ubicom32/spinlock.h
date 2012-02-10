/*
 * (C) Copyright 2009
 * Ubicom, Inc. www.ubicom.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef UBICOM32_SPINLOCK_H
#define UBICOM32_SPINLOCK_H

struct ubi32_spinlock_t {
	volatile unsigned int lock;
};


/*
 * spin_lock()
 *	Lock the lock.
 */
static inline void ubi32_spin_lock(struct ubi32_spinlock_t *x)
{
	asm volatile (
	"1:	bset	%0, %0, #0	\n\t"
	"	jmpne.f	1b		\n\t"
		: "+U4" (x->lock)
		:
		: "memory", "cc"
	);
}

/*
 * spin_unlock()
 *	Unlock the lock.
 */
static inline void ubi32_spin_unlock(struct ubi32_spinlock_t *x)
{
	asm volatile (
	"	bclr	%0, %0, #0	\n\t"
		: "+U4" (x->lock)
		:
		: "memory", "cc"
	);
}

/*
 * spin_is_locked()
 *	Test if the lock is locked.
 */
static inline int ubi32_spin_is_locked(struct ubi32_spinlock_t *x)
{
	return x->lock;
}

/*
 * ubi32_spin_lock_init()
 *	Test if the lock is locked.
 */
static inline int ubi32_spin_lock_init(struct ubi32_spinlock_t *x)
{
	x->lock = 0;
	return x->lock;
}



#endif /* _ASM_UBICOM32_SPINLOCK_H */
