/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include "asm/ip5000.h"

static ulong timestamp;

static ulong init_tv;
static ulong last_tv;
static unsigned long long high_tv;

#define READ_TV 	UBICOM32_IO_TIMER->mptval
#define TICKS_PER_SEC	GEN_CLK_MPT_FREQ
#define TICKS_PER_USEC	(GEN_CLK_MPT_FREQ/1000000)

void timer_init(void)
{
	init_tv = READ_TV;
	reset_timer();
}


void reset_timer(void)
{
	timestamp = 0;
}

unsigned long long get_ticks(void)
{
	ulong tv = READ_TV - init_tv;

	/*
 	 * This is based on the assumption that if anything needs timing
	 * it will call get_ticks at least once every 5 minutes so we can
	 * monitor for wrap arounds. That's why get_timer uses get_ticks.
	 */
	if (tv < last_tv)
		high_tv += 0x100000000ULL;
	last_tv = tv;

	return high_tv | tv;
}

ulong get_timer(ulong base)
{
	return get_ticks() * CONFIG_SYS_HZ / TICKS_PER_SEC + timestamp - base;
}

/*
 * timestamp seems to be an offset to get_timer, but unused in the main code.
 */
void set_timer(ulong t)
{
	timestamp = t;
}

/*
 * See common/main.c why this is defined the way it is.
 */
ulong get_tbclk(void)
{
	return TICKS_PER_SEC;
}

void udelay (unsigned long usec)
{
	unsigned long long ticks = TICKS_PER_USEC * usec;
	unsigned long long now = get_ticks();

	/*
	 * use <= so that we wait `at least' usec microseconds.
	 */
	while (get_ticks() - now <= ticks)
		/*NOP*/;
}

void enable_interrupts (void)
{
	/*
	 * Ubicom u-boot does not use interrupts at the moment.
	 */
}

/*
 * disable IRQ/FIQ interrupts
 * returns true if interrupts had been enabled before we disabled them
 */
int disable_interrupts (void)
{
	/* The ubicom32 architecture does not have regular interrupts. */
	/* They are simulated by using a separate hardware thread that */
	/* has to use the suspend instruction and wait (but wakes up   */
	/* within one cycle and so starts handling extremely quickly). */
	return 1;
}

int interrupt_init(void)
{
	/*
	 * Ubicom u-boot does not use interrupts at the moment.
	 */
	return 0;
}

