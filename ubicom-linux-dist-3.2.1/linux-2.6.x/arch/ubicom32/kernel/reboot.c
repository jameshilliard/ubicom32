/*
 * arch/ubicom32/kernel/reboot.c
 *
 * (C) Copyright 2011, Ubicom, Inc.
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
#include <linux/kernel.h>
#include <linux/reboot.h>
#include <linux/module.h>
#include <linux/pm.h>

#include <asm/system.h>

#define WITH_REBOOT true
#define NO_REBOOT false

void (*pm_power_off)(void) = machine_power_off;
EXPORT_SYMBOL(pm_power_off);

/*
 * ubicom32_machine_restart()
 *	Restart/Halt the machine according watchdog_enabled.
 */
static void ubicom32_machine_restart(bool reboot)
{
	/*
	 * Disable all threads except myself. We can do this
	 * directly without needing to call smp_send_stop
	 * because we have a unique architecture where
	 * one thread can disable one or more other threads.
	 */
	thread_disable_others();

	if (reboot) {
		/*
		 * Set watchdog to trigger (after 1ms delay) (12 Mhz is the fixed OSC)
		 */
		UBICOM32_IO_TIMER->tkey = TIMER_TKEYVAL;
		UBICOM32_IO_TIMER->wdcom = UBICOM32_IO_TIMER->mptval +
			(GEN_CLK_MPT_FREQ / 1000);
		UBICOM32_IO_TIMER->wdcfg = 0;
		UBICOM32_IO_TIMER->tkey = 0;

		/*
		 * Wait for watchdog
		 */
		asm volatile (
			"	move.4		MT_EN, #0		\n\t"
			"	pipe_flush	0			\n\t"
		);
	}

	local_irq_disable();
	for (;;) {
		thread_suspend();
	}

}

/*
 * machine_restart()
 *	Resets the system.
 */
void machine_restart(char *__unused)
{
	printk(KERN_EMERG "Machine Restart\n");

	ubicom32_machine_restart(WITH_REBOOT);
}

/*
 * machine_halt()
 *	Halt the machine.
 *
 * Similar to machine_power_off, but don't shut off power.  Add code
 * here to freeze the system for e.g. post-mortem debug purpose when
 * possible.  This halt has nothing to do with the idle halt.
 */
void machine_halt(void)
{
	printk(KERN_EMERG "Machine Halt\n");

	ubicom32_machine_restart(NO_REBOOT);
}

/*
 * machine_power_off()
 *	Turn the power off, if a power off handler is defined, otherwise, spin
 *	endlessly.
 */
void machine_power_off(void)
{
	printk(KERN_EMERG "Machine Power Off\n");

	ubicom32_machine_restart(NO_REBOOT);
}

/*
 * machine_emergency_restart()
 *	Emergency restart.
 */
void machine_emergency_restart(void)
{
	printk(KERN_EMERG "Machine Emergency Restart\n");

#if defined(CONFIG_UBICOM32_STALL_ON_EMERGENCY_REBOOT)
	printk(KERN_NOTICE "Stalling all threads...\n");
	THREAD_STALL;
#endif
	ubicom32_machine_restart(WITH_REBOOT);
}

