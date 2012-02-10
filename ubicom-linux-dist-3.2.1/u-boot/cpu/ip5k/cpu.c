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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * CPU specific code
 */

#include <common.h>
#include <command.h>
#include <watchdog.h>
#include "asm/ip5000.h"
#include <asm/UbicomSDK.h>


unsigned long jiffies;

void reset_cpu (ulong ignored)
{
	/*
	 * Set watchdog to trigger (after 1ms delay)
	 */
	ubicom32_write_reg(&UBICOM32_IO_TIMER->tkey, TIMER_TKEYVAL);
	ubicom32_write_reg(&UBICOM32_IO_TIMER->wdcom, UBICOM32_IO_TIMER->mptval + (GEN_CLK_MPT_FREQ / 1000));
	ubicom32_write_reg(&UBICOM32_IO_TIMER->wdcfg, 0);
	ubicom32_write_reg(&UBICOM32_IO_TIMER->tkey, 0);

	/*
	 * Wait for watchdog
	 */
	asm volatile (
	"	move.4		MT_EN, #0		\n\t"
	"	pipe_flush	0			\n\t"
	);

	while (1);
}

int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	disable_interrupts ();
	reset_cpu (0);
	/*NOTREACHED*/
	return (0);
}

#ifdef CONFIG_HW_WATCHDOG
void hw_watchdog_reset (void)
{
	/*
 	 * Check to see if weatchdog is running
	 */
	unsigned int wcfg = ubicom32_read_reg(&UBICOM32_IO_TIMER->wdcfg);
	if (wcfg == TIMER_WATCHDOG_DISABLE) {
		/*
		 * The HW watchdog is not running. Skip it.
		 */
		return;
	}

	/*
	 * The HW watchdog is running. Reset watchdog now (timeout = quite long).
	 */
	ubicom32_write_reg(&UBICOM32_IO_TIMER->tkey, TIMER_TKEYVAL);
	ubicom32_write_reg(&UBICOM32_IO_TIMER->wdcom, UBICOM32_IO_TIMER->mptval + (GEN_CLK_MPT_FREQ * 60));
	ubicom32_write_reg(&UBICOM32_IO_TIMER->tkey, 0);
}
#endif
