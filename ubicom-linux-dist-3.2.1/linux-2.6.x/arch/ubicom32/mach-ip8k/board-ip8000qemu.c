/*
 * arch/ubicom32/mach-ip7k/board-ip8000qemu.c
 *   Platform initialization for ip8000qemu board.
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

#ifdef CONFIG_SERIAL_UBI32_SERDES
#include <asm/ubicom32suart.h>
#endif

#ifdef CONFIG_SERIAL_UBI32_SERDES
static struct resource ip8000_ubicom32_suart_resources[] = {
	{
		.start	= IO_PORT_RO,
		.end	= IO_PORT_RO,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= RO_SERDES_RX_INT,
		.end	= RO_SERDES_INT),
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= 250000000,
		.end	= 250000000,
		.flags	= UBICOM32_SUART_IORESOURCE_CLOCK,
	},
};

static struct platform_device ip8000_ubicom32_suart_device = {
	.name		= "ubicom32suart",
	.id		= -1,
	.num_resources  = ARRAY_SIZE(ip8000_ubicom32_suart_resources),
	.resource	= ip8000_ubicom32_suart_resources,
};
#endif

/*
 * List of all devices in our system
 */
static struct platform_device *ip8000_devices[] __initdata = {
#ifdef CONFIG_SERIAL_UBI32_SERDES
	&ip8000_ubicom32_suart_device,
#endif
};

/*
 * ip8000_init
 *	Called to add the devices which we have on this board
 */
static int __init ip8000_init(void)
{
	board_init();

	printk(KERN_INFO "%s: registering device resources\n", __FUNCTION__);
	platform_add_devices(ip8000_devices, ARRAY_SIZE(ip8000_devices));

	return 0;
}

arch_initcall(ip8000_init);
