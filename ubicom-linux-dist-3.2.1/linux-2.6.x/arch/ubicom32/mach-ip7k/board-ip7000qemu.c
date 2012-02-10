/*
 * arch/ubicom32/mach-ip7k/board-ip7000qemu.c
 *   Platform initialization for ip7000qemu board.
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
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/delay.h>

#include <asm/board.h>
#include <asm/machdep.h>

/*
 * List of all devices in our system
 */
static struct platform_device *ip7000qemu_devices[] __initdata = {
};

/*
 * ip7000qemu_init
 *	Called to add the devices which we have on this board
 */
static int __init ip7000qemu_init(void)
{
	board_init();

	printk(KERN_INFO "%s: registering device resources\n", __FUNCTION__);
	platform_add_devices(ip7000qemu_devices, ARRAY_SIZE(ip7000qemu_devices));

	printk(KERN_INFO "IP7000 QEMU\n");
	return 0;
}

arch_initcall(ip7000qemu_init);
