/*
 * arch/ubicom32/mach-common/pci_stopwatch.c
 *	PCI operation measurement.
 *
 * (C) Copyright 2010, Ubicom, Inc.
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
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/init.h>
#include "pci_measurement.h"

#define __STOPWATCH_USE__ 1
#include <asm/stopwatch.h>


static const char *pci_stopwatch_name_list[PCI_MEASUREMENT_LAST] = {
	"READ32",
	"WRITE32",
	"READ16",
	"WRITE16",
	"READ8",
	"WRITE8"
};

struct stopwatch_instance pci_stopwatches[PCI_MEASUREMENT_LAST];

/*
 * pci_stopwatch_show()
 *	Print out the min, avg, max values for each PCI transaction type.
 *
 * By request, the max value is reset after each dump.
 */
static int pci_stopwatch_show(struct seq_file *p, void *v)
{
	int trans = *((loff_t *) v);

	if (trans == 0) {
		seq_puts(p, "min\tavg\tmax\t(nano-seconds)\n");
	}

	stopwatch_show(&pci_stopwatches[trans], p, pci_stopwatch_name_list[trans], STOPWATCH_SHOW_NANO);
	return 0;
}

static int __init pci_stopwatch_init(void)
{
	return stopwatch_register("pci", PCI_MEASUREMENT_LAST, pci_stopwatch_show);
}

module_init(pci_stopwatch_init);

