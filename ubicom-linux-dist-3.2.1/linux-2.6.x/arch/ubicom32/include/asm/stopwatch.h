/*
 * arch/ubicom32/kernel/stopwatch.h
 *   Creates /proc/stopwatch/ and associated functions
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
 */
#ifndef _ASM_UBICOM32_STOPWATCH_H
#define _ASM_UBICOM32_STOPWATCH_H

#include <linux/seq_file.h>

/*
 * Create a false set of defines and functions enabling users
 * of the stopwatch to leave the code in place.
 */
#if !defined(__STOPWATCH_USE__)

struct stopwatch_instance;
#define DEFINE_STATIC_STOPWATCH(x)
#define DEFINE_STATIC_STOPWATCH_ARRAY(x, y)
#define STOPWATCH_DECLARE
#define STOPWATCH_START()
#define STOPWATCH_END(si)
#define stopwatch_register(name, count, show) (0)
#define stopwatch_unregister(name) (0)
#define stopwatch_update(si, sample)
#define stopwatch_show(si, p, s, scale)

#else /* __STOPWATCH_USE__ */

#define DEFINE_STATIC_STOPWATCH(x) static struct stopwatch_instance x;
#define DEFINE_STATIC_STOPWATCH_ARRAY(x, y) static struct stopwatch_instance x[y];

#define STOPWATCH_SHOW_MICRO	1
#define STOPWATCH_SHOW_NANO	1000

#define STOPWATCH_DECLARE	\
	int __diff;		\
	unsigned int __tstart;

#define STOPWATCH_START()		\
	__tstart = UBICOM32_IO_TIMER->sysval;

#define STOPWATCH_END(si)		\
	__diff = (int)UBICOM32_IO_TIMER->sysval - (int)__tstart; \
	stopwatch_update((si), __diff);

struct stopwatch_instance {
	volatile unsigned long long min;
	volatile unsigned long long avg;
	volatile unsigned long long max;
};

typedef int (*stopwatch_show_t)(struct seq_file *f, void *v);

extern unsigned int stopwatch_register(const char *name, int count, 
				       stopwatch_show_t show);

extern void stopwatch_unregister(const char *name);

extern void stopwatch_update(struct stopwatch_instance *si,
			     unsigned int sample);

extern void stopwatch_show(struct stopwatch_instance *si,
			   struct seq_file *p, 
			   const char *s,
			   unsigned int scale);

#endif /* __STOPWATCH_USE__ */
#endif /* _ASM_UBICOM32_STOPWATCH_H */
