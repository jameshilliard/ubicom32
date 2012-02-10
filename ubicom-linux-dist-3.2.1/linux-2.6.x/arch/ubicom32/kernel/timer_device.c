/*
 * arch/ubicom32/kernel/timer_device.c
 *   Implements a Ubicom32 clock device and event devices.
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
#include <linux/types.h>
#include <linux/clockchips.h>
#include <linux/clocksource.h>
#include <asm/ubicom32.h>
#include <asm/machdep.h>

#if defined(CONFIG_SMP)
#include <asm/smp.h>
#endif

#if defined(CONFIG_GENERIC_CLOCKEVENTS_BROADCAST)
#define MAX_TIMERS (2 + CONFIG_TIMER_EXTRA_ALLOC)
#else
#define MAX_TIMERS (NR_CPUS + CONFIG_TIMER_EXTRA_ALLOC)
#endif

#if (MAX_TIMERS > 10)
#error "Ubicom32 only has 10 timers"
#endif

static unsigned int frequency;
static struct clock_event_device timer_device_devs[MAX_TIMERS];
static struct irqaction timer_device_irqs[MAX_TIMERS];
static int timer_device_next_timer = 0;

/*
 * Software implementation of a 64 bit high resolution timer.
 */
static volatile cycle_t timer_device_high_res;
static volatile unsigned long timer_device_last_sysval;

/*
 * timer_device_lock_release()
 *	Release the timer lock.
 *
 * We use a global lock for the timer because Linux
 * lock proofing code uses the timer.
 */
static void timer_device_lock_release(void)
{
	UBICOM32_UNLOCK(TIMER_LOCK_BIT);
}

/*
 * timer_device_lock_acquire()
 *	Acquire the timer lock, spin if not available.
 *
 * We use a global lock for the timer because Linux
 * lock proofing code uses the timer.
 */
static void timer_device_lock_acquire(void)
{
	UBICOM32_LOCK(TIMER_LOCK_BIT);
}

/*
 * timer_device_clockbase_read()
 *	Read and update the high resolution 64 bit timer.
 *
 * This timer is artificially created in software by noticing if the low
 * 32 bits of sysval are now less than the last time the timer was read
 * and incrementing the high 32 bits.  For this to work, the timer must
 * be read before it is allowed to rollover.
 *
 * The system guarantees (even when compiled tickless) that it will read
 * the timer (either by calling sched_clock() or by taking a timer
 * interrupt) before it rolls over.

 * The only exception would be if some part of the system held
 * interrupts off (and did not call sched_clock()) for more than 2^32
 * cycles.  This is considered highly unlikely.  At 500Mhz, 2^32 cycles
 * is approximately 8 seconds.
 *
 * The value returned is a 64 bit cycle timer.
 */
static cycle_t timer_device_clockbase_read(struct clocksource *cs)
{
	unsigned long sysval;
	cycle_t ret;

	timer_device_lock_acquire();
	sysval = UBICOM32_IO_TIMER->sysval;
	if (unlikely(sysval < timer_device_last_sysval)) {
		timer_device_high_res += 0x100000000LL;
	}
	timer_device_last_sysval = sysval;
	ret = timer_device_high_res | sysval;
	timer_device_lock_release();
	return ret;
}

/*
 * timer_device_set_next_event()
 *	Cause the timer to go off "cycles" from now.
 */
static int timer_device_set_next_event(unsigned long cycles, struct clock_event_device *dev)
{
	timer_set(dev->irq, cycles);
	return 0;
}

/*
 * timer_device_set_mode()
 *	Handle the mode switch for a clock event device.
 */
static void timer_device_set_mode(enum clock_event_mode mode, struct clock_event_device *dev)
{
	switch (mode) {
	case CLOCK_EVT_MODE_SHUTDOWN:
		/*
		 * Make sure the vector is disabled
		 * until the next event is set.
		 */
		printk(KERN_NOTICE "timer[%d]: shutdown\n", dev->irq);
		pic_disable_vector(dev->irq);
		break;

	case CLOCK_EVT_MODE_ONESHOT:
		/*
		 * Make sure the vector is disabled
		 * until the next event is set.
		 */
		printk(KERN_NOTICE "timer[%d]: oneshot\n", dev->irq);
		pic_disable_vector(dev->irq);
		break;

	case CLOCK_EVT_MODE_PERIODIC:
		/*
		 * The periodic request is 1 per jiffies
		 */
		printk(KERN_NOTICE "timer[%d]: periodic: %d cycles\n",
			dev->irq, frequency / CONFIG_HZ);
		timer_set(dev->irq, frequency / CONFIG_HZ);
		break;

	case CLOCK_EVT_MODE_UNUSED:
	case CLOCK_EVT_MODE_RESUME:
		printk(KERN_WARNING "timer[%d]: unimplemented mode: %d\n",
			dev->irq, mode);
		break;
	}
}

/*
 * Primary Clock Source Description
 *
 * Notes:
 *
 * 1) mult is overwritten by the init below.
 *
 * 2) See include/linux/clocksource.h for ratings.
 *
 * 3) The scale of the arithmetic in cyc2ns is 1 / 2 ^ (.shift).  A
 * shift factor of 10 is accurate to about 1/1000 of a nsec.  This
 * yields a clock rollover in 2^54 nanoseconds ~= 180 days.
 */
static struct clocksource timer_device_clockbase = {
	.name	= "sysval",
	.rating	= 400,
	.read	= timer_device_clockbase_read,
	.flags	= CLOCK_SOURCE_IS_CONTINUOUS,
	.mask	= CLOCKSOURCE_MASK(32),
	.mult	= 0,				/* To be filled in by init */
	.shift	= 10,
};

/*
 * timer_device_event()
 *	Call the device's event handler.
 *
 * The pointer is initialized by the generic Linux code
 * to the function to be called.
 */
static irqreturn_t timer_device_event(int irq, void *dev_id)
{
	struct clock_event_device *dev = (struct clock_event_device *)dev_id;

	/*
	 * Read the high resolution timer on each interrupt to make sure
	 * that the high resolution timer is properly maintained.
	 */
	(void)timer_device_clockbase_read(&timer_device_clockbase);

	if (dev->mode == CLOCK_EVT_MODE_PERIODIC) {
		/*
		 * The periodic request is 1 per jiffies
		 */
		timer_reset(dev->irq, frequency / CONFIG_HZ);
	} else {
		/*
		 * The timer will go off again at the rollover
		 * point.  We must disable the IRQ to prevent
		 * getting a spurious interrupt.
		 */
		pic_disable_vector(dev->irq);
	}

	if (!dev->event_handler) {
		printk(KERN_CRIT "no registered event handler\n");
		return IRQ_HANDLED;
	}

	dev->event_handler(dev);
	return IRQ_HANDLED;
}

/*
 * timer_device_alloc_event()
 * 	Allocate a timer device event.
 */
static int timer_device_alloc_event(const char *name, int cpuid, const cpumask_t *mask)
{
	struct clock_event_device *dev;
	struct irqaction *action;

	/*
	 * Are we out of configured timers?
	 */
	timer_device_lock_acquire();
	if (timer_device_next_timer >= MAX_TIMERS) {
		timer_device_lock_release();
		printk(KERN_WARNING "out of timer event entries\n");
		return -1;
	}
	dev = &timer_device_devs[timer_device_next_timer];
	action = &timer_device_irqs[timer_device_next_timer];
	timer_device_next_timer++;
	timer_device_lock_release();

	/*
	 * Now allocate a timer to ourselves.
	 */
	dev->irq = timer_alloc();
	if (dev->irq == -1) {
		timer_device_lock_acquire();
		timer_device_next_timer--;
		timer_device_lock_release();
		printk(KERN_WARNING "out of hardware timers\n");
		return -1;
	}

	/*
	 * Init the IRQ action structure.  Make sure
	 * this in place before you register the clock
	 * event device.
	 */
	action->name = name;
	action->flags = IRQF_DISABLED | IRQF_TIMER;
	action->handler = timer_device_event;
	action->dev_id = dev;
	setup_irq(dev->irq, action);
	irq_set_affinity(dev->irq, mask);
	pic_disable_vector(dev->irq);

	/*
	 * init clock dev structure.
	 *
	 * The max_delta_ns needs to be less than a full timer's
	 * resolution to ensure that with overhead, we will be able to
	 * service the timer.  The usual approach is to use 31 bits
	 * instead of 32 for a 32 bit timer.
	 *
	 * The min_delta_ns is chosen to ensure that setting next event
	 * will never be requested with too small of value.
	 */
	dev->name = name;
	dev->rating = timer_device_clockbase.rating;
	dev->shift = timer_device_clockbase.shift;
	dev->features = CLOCK_EVT_FEAT_PERIODIC | CLOCK_EVT_FEAT_ONESHOT;
	dev->set_mode = timer_device_set_mode;
	dev->set_next_event = timer_device_set_next_event;
	dev->mult = div_sc(frequency, NSEC_PER_SEC, dev->shift);
	dev->max_delta_ns = clockevent_delta2ns(0x7fffffff, dev);
	dev->min_delta_ns = clockevent_delta2ns(0xf, dev);
	dev->cpumask = mask;
	printk(KERN_NOTICE "timer[%d]: %s - created\n", dev->irq, dev->name);

	/*
	 * Now register the device.
	 */
	clockevents_register_device(dev);
	return dev->irq;
}

#if defined(CONFIG_LOCAL_TIMERS)
/*
 * local_timer_setup()
 * 	Allocation function for creating a per cpu local timer.
 */
int __cpuinit local_timer_setup(unsigned int cpu)
{
	const cpumask_t *mask = cpumask_of(cpu);
	return timer_device_alloc_event("timer-cpu", cpu, mask);
}
#endif

/*
 * sched_clock()
 *	Returns current time in nano-second units.
 *
 * Notes:
 * 1) This is an override for the weak alias in
 * kernel/sched_clock.c.
 * 2) Do not use xtime_lock as this function is
 * sometimes called with xtime_lock held.
 * 3) This approach allows us to perform sched_clock() calls with interrupts
 * disabled, since our cycle counter moves forward no matter what.
 */
unsigned long long sched_clock(void)
{
	return clocksource_cyc2ns(timer_device_clockbase_read(&timer_device_clockbase), 
				  timer_device_clockbase.mult, 
				  timer_device_clockbase.shift);
}

/*
 * timer_device_init()
 * 	Create and init a generic clock driver for Ubicom32.
 */
void timer_device_init(void)
{
	int i;
	const cpumask_t mask = (CPU_MASK_ALL);
	/*
	 * Get the frequency from the processor device tree node or use
	 * the default if not available. We will store this as the frequency
	 * of the timer to avoid future calculations.
	 */
	frequency = processor_frequency();
	if (frequency == 0) {
		frequency = CLOCK_TICK_RATE;
	}

	/*
	 * Setup the primary clock source around sysval.  Linux does not
	 * supply a Mhz multiplier so convert down to khz.
	 */
	timer_device_clockbase.mult =
		clocksource_khz2mult(frequency / 1000,
			timer_device_clockbase.shift);
	if (clocksource_register(&timer_device_clockbase)) {
		printk(KERN_ERR "timer: clocksource failed to register\n");
		return;
	}

	/*
	 * Always allocate a primary timer.
	 */
	timer_device_alloc_event("timer-primary", -1, &mask);

	/*
	 * Initialize the high resolution interface.
	 */
	timer_device_lock_acquire();
	timer_device_high_res = 0;
	timer_device_last_sysval = UBICOM32_IO_TIMER->sysval;
	timer_device_lock_release();

#if defined(CONFIG_GENERIC_CLOCKEVENTS_BROADCAST)
	/*
	 * If BROADCAST is selected we need to add a broadcast timer.
	 */
	timer_device_alloc_event("timer-broadcast", -1, CPU_MASK_ALL);
#endif

	/*
	 * Allocate extra timers that are requested.
	 */
	for (i = 0; i < CONFIG_TIMER_EXTRA_ALLOC; i++) {
		timer_device_alloc_event("timer-extra", -1, &mask);
	}
}

