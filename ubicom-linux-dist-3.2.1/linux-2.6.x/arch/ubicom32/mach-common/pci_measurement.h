/*
 * arch/ubicom32/mach-common/pci_measurement.h
 *	PCI performance measurement
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

#ifndef __UBI32_PCI_MEASURMENT_H__
#define __UBI32_PCI_MEASURMENT_H__ 

#include <asm/ubicom32.h>

#if defined(CONFIG_UBICOM32_STOPWATCH_PCI)
#define __STOPWATCH_USE__ 1
#endif
#include <asm/stopwatch.h>

enum pci_measurement_list {
	PCI_MEASUREMENT_READ32,
	PCI_MEASUREMENT_WRITE32,
	PCI_MEASUREMENT_READ16,
	PCI_MEASUREMENT_WRITE16,
	PCI_MEASUREMENT_READ8,
	PCI_MEASUREMENT_WRITE8,
	PCI_MEASUREMENT_LAST,
};

#if defined(CONFIG_UBICOM32_STOPWATCH_PCI)
extern struct stopwatch_instance pci_stopwatches[PCI_MEASUREMENT_LAST];
#endif

#endif
