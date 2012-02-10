/*
 * arch/ubicom32/include/asm/spinlock.h
 *   Spinlock related definitions for Ubicom32 architecture.
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
#ifndef _ASM_UBICOM32_SPINLOCK_H
#define _ASM_UBICOM32_SPINLOCK_H

#include <asm/system.h>
#include <asm/processor.h>
#include <asm/spinlock_types.h>

/*
 * arch_spin_lock()
 *	Lock the lock.
 */
static inline void arch_spin_lock(arch_spinlock_t *x)
{
	__builtin_ubicom32_spinlock_lock(&x->lock, 0);
	barrier();
}

/*
 * arch_spin_unlock()
 *	Unlock the lock.
 */
static inline void arch_spin_unlock(arch_spinlock_t *x)
{
	__builtin_ubicom32_spinlock_unlock(&x->lock, 0);
	barrier();
}

/*
 * arch_spin_is_locked()
 *	Test if the lock is locked.
 */
static inline int arch_spin_is_locked(arch_spinlock_t *x)
{
	return (int)__builtin_ubicom32_spinlock_is_locked(&x->lock, 0);
}

/*
 * arch_spin_unlock_wait()
 *	Wait for the lock to be unlocked.
 *
 * Note: the caller has no guarantee that the lock will not
 * be acquired before they get to it.
 */
static inline void arch_spin_unlock_wait(arch_spinlock_t *x)
{
	do {
		cpu_relax();
	} while (arch_spin_is_locked(x));
}

/*
 * arch_spin_trylock()
 *	Try the lock, return 0 on failure, 1 on success.
 */
static inline int arch_spin_trylock(arch_spinlock_t *x)
{
	bool ret = __builtin_ubicom32_spinlock_try_lock(&x->lock, 0);
	barrier();
	return (int)ret;
}

/*
 * arch_spin_lock_flags()
 *	Spin waiting for the lock.
 */
#define arch_spin_lock_flags(lock, flags) arch_spin_lock(lock)

/*
 * Read-write spinlocks, allowing multiple readers but only one writer.
 * Linux rwlocks are unfair to writers; they can be starved for an indefinite
 * time by readers.  With care, they can also be taken in interrupt context.
 *
 * In Ubicom32 architecture implementation, we have a spinlock and a counter.
 * Readers use the lock to serialise their access to the counter (which
 * records how many readers currently hold the lock).
 * Writers hold the spinlock, preventing any readers or other writers from
 * grabbing the rwlock.
 */

/*
 * arch_read_lock()
 *	Increment the counter in the rwlock.
 *
 * Note that we have to ensure interrupts are disabled in case we're
 * interrupted by some other code that wants to grab the same read lock
 */
static inline void arch_read_lock(arch_rwlock_t *rw)
{
	unsigned long flags;
	raw_local_irq_save(flags);
	arch_spin_lock_flags(&rw->lock, flags);
	rw->counter++;
	arch_spin_unlock(&rw->lock);
	raw_local_irq_restore(flags);
}

/*
 * arch_read_unlock()
 *	Decrement the counter.
 *
 * Note that we have to ensure interrupts are disabled in case we're
 * interrupted by some other code that wants to grab the same read lock
 */
static inline void arch_read_unlock(arch_rwlock_t *rw)
{
	unsigned long flags;
	raw_local_irq_save(flags);
	arch_spin_lock_flags(&rw->lock, flags);
	rw->counter--;
	arch_spin_unlock(&rw->lock);
	raw_local_irq_restore(flags);
}

/*
 * arch_read_trylock()
 *	Increment the counter if we can.
 *
 * Note that we have to ensure interrupts are disabled in case we're
 * interrupted by some other code that wants to grab the same read lock
 */
static inline int arch_read_trylock(arch_rwlock_t *rw)
{
	unsigned long flags;
 retry:
	raw_local_irq_save(flags);
	if (arch_spin_trylock(&rw->lock)) {
		rw->counter++;
		arch_spin_unlock(&rw->lock);
		raw_local_irq_restore(flags);
		return 1;
	}

	raw_local_irq_restore(flags);

	/*
	 * If write-locked, we fail to acquire the lock
	 */
	if (rw->counter < 0) {
		return 0;
	}

	/*
	 * Wait until we have a realistic chance at the lock
	 */
	while (arch_spin_is_locked(&rw->lock) && rw->counter >= 0) {
		cpu_relax();
	}

	goto retry;
}

/*
 * arch_write_lock()
 *
 * Note that we have to ensure interrupts are disabled in case we're
 * interrupted by some other code that wants to read_trylock() this lock
 */
static inline void arch_write_lock(arch_rwlock_t *rw)
{
	unsigned long flags;
retry:
	raw_local_irq_save(flags);
	arch_spin_lock_flags(&rw->lock, flags);

	if (rw->counter != 0) {
		arch_spin_unlock(&rw->lock);
		raw_local_irq_restore(flags);

		while (rw->counter != 0)
			cpu_relax();

		goto retry;
	}

	rw->counter = -1; /* mark as write-locked */
	mb();
	raw_local_irq_restore(flags);
}

static inline void arch_write_unlock(arch_rwlock_t *rw)
{
	rw->counter = 0;
	arch_spin_unlock(&rw->lock);
}

/* Note that we have to ensure interrupts are disabled in case we're
 * interrupted by some other code that wants to read_trylock() this lock */
static inline int arch_write_trylock(arch_rwlock_t *rw)
{
	unsigned long flags;
	int result = 0;

	raw_local_irq_save(flags);
	if (arch_spin_trylock(&rw->lock)) {
		if (rw->counter == 0) {
			rw->counter = -1;
			result = 1;
		} else {
			/* Read-locked.  Oh well. */
			arch_spin_unlock(&rw->lock);
		}
	}
	raw_local_irq_restore(flags);

	return result;
}

/*
 * read_can_lock - would read_trylock() succeed?
 * @lock: the rwlock in question.
 */
static inline int arch_read_can_lock(arch_rwlock_t *rw)
{
	return rw->counter >= 0;
}

/*
 * write_can_lock - would write_trylock() succeed?
 * @lock: the rwlock in question.
 */
static inline int arch_write_can_lock(arch_rwlock_t *rw)
{
	return !rw->counter;
}

#define arch_read_lock_flags(lock, flags) arch_read_lock(lock)
#define arch_write_lock_flags(lock, flags) arch_write_lock(lock)
#define arch_spin_relax(lock)	cpu_relax()
#define arch_read_relax(lock)	cpu_relax()
#define arch_write_relax(lock)	cpu_relax()

#endif /* _ASM_UBICOM32_SPINLOCK_H */
