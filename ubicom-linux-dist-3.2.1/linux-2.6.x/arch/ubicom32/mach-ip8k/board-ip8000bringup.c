/*
 * arch/ubicom32/mach-ip8k/board-ip8000bringup.c
 *   Support for the IP8000 bringup board.
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
#include <linux/gpio.h>
#include <linux/leds.h>
#include <linux/delay.h>
#include <linux/input.h>

#include <asm/ubicom32sd.h>
#include <asm/sd_tio.h>

#include <asm/board.h>
#include <asm/machdep.h>
#include <asm/ubicom32input.h>

#ifdef CONFIG_SERIAL_UBI32_SERDES
#include <asm/ubicom32suart.h>
#endif

/******************************************************************************
 * SD/IO Port D (Slot 0) platform data
 */
static struct resource ip8000bringup_portd_sd_resources[] = {
	/*
	 * Send IRQ
	 */
	[0] = {
		/* 
		 * The init routine will query the devtree and fill this in
		 */
		.flags	= IORESOURCE_IRQ,
	},

	/*
	 * Receive IRQ
	 */
	[1] = {
		/* 
		 * The init routine will query the devtree and fill this in
		 */
		.flags	= IORESOURCE_IRQ,
	},

	/*
	 * Memory Mapped Registers
	 */
	[2] = {
		/* 
		 * The init routine will query the devtree and fill this in
		 */
		.flags	= IORESOURCE_MEM,
	},
};

static struct ubicom32sd_card ip8000bringup_portd_sd_cards[] = {
	[0] = {
		.pin_wp		= -1,
		.wp_polarity	= 0,
		.pin_pwr	= GPIO_PG4_9,
		.pin_cd		= GPIO_PG4_24,
	},
};

static struct ubicom32sd_platform_data ip8000bringup_portd_sd_platform_data = {
	.ncards		= 1,
	.cards		= ip8000bringup_portd_sd_cards, 
};

static struct platform_device ip8000bringup_portd_sd_device = {
	.name		= "ubicom32sd",
	.id		= 0,
	.resource	= ip8000bringup_portd_sd_resources,
	.num_resources	= ARRAY_SIZE(ip8000bringup_portd_sd_resources),
	.dev		= {
			.platform_data = &ip8000bringup_portd_sd_platform_data,
	},

};

/*
 * ip8000bringup_portd_sd_init
 */
static void ip8000bringup_portd_sd_init(void)
{
	/*
	 * Check the device tree for the sd_tio
	 */
	struct sd_tio_node *sd_node = (struct sd_tio_node *)devtree_find_node("portd_sd");
	if (!sd_node) {
		printk(KERN_INFO "PortD SDTIO not found\n");
		return;
	}

	/*
	 * Fill in the resources and platform data from devtree information
	 */
	ip8000bringup_portd_sd_resources[0].start = sd_node->dn.sendirq;
	ip8000bringup_portd_sd_resources[1].start = sd_node->dn.recvirq;
	ip8000bringup_portd_sd_resources[2].start = (u32_t)&(sd_node->regs);
	ip8000bringup_portd_sd_resources[2].end = (u32_t)&(sd_node->regs) + sizeof(sd_node->regs);
	platform_device_register(&ip8000bringup_portd_sd_device);
}

/******************************************************************************
 * SD/IO Port E (Slot 1) platform data
 */
static struct resource ip8000bringup_porte_sd_resources[] = {
	/*
	 * Send IRQ
	 */
	[0] = {
		/* 
		 * The init routine will query the devtree and fill this in
		 */
		.flags	= IORESOURCE_IRQ,
	},

	/*
	 * Receive IRQ
	 */
	[1] = {
		/* 
		 * The init routine will query the devtree and fill this in
		 */
		.flags	= IORESOURCE_IRQ,
	},

	/*
	 * Memory Mapped Registers
	 */
	[2] = {
		/* 
		 * The init routine will query the devtree and fill this in
		 */
		.flags	= IORESOURCE_MEM,
	},
};

static struct ubicom32sd_card ip8000bringup_porte_sd_cards[] = {
	[0] = {
		.pin_wp		= -1,
		.wp_polarity	= 0,
		.pin_pwr	= GPIO_PG4_26,
		.pin_cd		= GPIO_PG4_28,
	},
};

static struct ubicom32sd_platform_data ip8000bringup_porte_sd_platform_data = {
	.ncards		= 1,
	.cards		= ip8000bringup_porte_sd_cards, 
};

static struct platform_device ip8000bringup_porte_sd_device = {
	.name		= "ubicom32sd",
	.id		= 0,
	.resource	= ip8000bringup_porte_sd_resources,
	.num_resources	= ARRAY_SIZE(ip8000bringup_porte_sd_resources),
	.dev		= {
			.platform_data = &ip8000bringup_porte_sd_platform_data,
	},

};

/*
 * ip8000bringup_porte_sd_init
 */
static void ip8000bringup_porte_sd_init(void)
{
	/*
	 * Check the device tree for the sd_tio
	 */
	struct sd_tio_node *sd_node = (struct sd_tio_node *)devtree_find_node("porte_sd");
	if (!sd_node) {
		printk(KERN_INFO "PortE SDTIO not found\n");
		return;
	}

	/*
	 * Fill in the resources and platform data from devtree information
	 */
	ip8000bringup_porte_sd_resources[0].start = sd_node->dn.sendirq;
	ip8000bringup_porte_sd_resources[1].start = sd_node->dn.recvirq;
	ip8000bringup_porte_sd_resources[2].start = (u32_t)&(sd_node->regs);
	ip8000bringup_porte_sd_resources[2].end = (u32_t)&(sd_node->regs) + sizeof(sd_node->regs);
	platform_device_register(&ip8000bringup_porte_sd_device);
}

/*
 * Use ubicom32input driver to monitor the various pushbuttons on this board.
 *
 * TODO: pick some ubicom understood EV_xxx define for WPS and Fact Default
 */
static struct ubicom32input_button ip8000bringup_ubicom32input_buttons[] = {
};

static struct ubicom32input_platform_data ip8000bringup_ubicom32input_data = {
	.buttons	= ip8000bringup_ubicom32input_buttons,
	.nbuttons	= ARRAY_SIZE(ip8000bringup_ubicom32input_buttons),
};

static struct platform_device ip8000bringup_ubicom32input_device = {
	.name	= "ubicom32input",
	.id	= -1,
	.dev	= {
		.platform_data = &ip8000bringup_ubicom32input_data,
	},
};

#ifdef CONFIG_SERIAL_UBI32_SERDES
static struct resource ip8000bringup_ubicom32_suart_resources[] = {
	{
		.start	= IO_PORT_RO,
		.end	= IO_PORT_RO,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= RO_SERDES_RX_INT,
		.end	= RO_SERDES_INT,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= 250000000,
		.end	= 250000000,
		.flags	= UBICOM32_SUART_IORESOURCE_CLOCK,
	},
};

static struct platform_device ip8000bringup_ubicom32_suart_device = {
	.name		= "ubicom32suart",
	.id		= -1,
	.num_resources  = ARRAY_SIZE(ip8000bringup_ubicom32_suart_resources),
	.resource	= ip8000bringup_ubicom32_suart_resources,
};
#endif

/*
 * List of all devices in our system
 */
static struct platform_device *ip8000bringup_devices[] __initdata = {
#ifdef CONFIG_SERIAL_UBI32_SERDES
	&ip8000bringup_ubicom32_suart_device,
#endif
	&ip8000bringup_ubicom32input_device,
};

/*
 * ip8000bringup_init
 *	Called to add the devices which we have on this board
 */
static int __init ip8000bringup_init(void)
{
	board_init();

	ubi_gpio_init();

	/*
	 * Initialize SD slots.  Only top slots are supported.
	 */
	ip8000bringup_portd_sd_init();
	ip8000bringup_porte_sd_init();

	printk(KERN_INFO "%s: registering device resources\n", __FUNCTION__);
	platform_add_devices(ip8000bringup_devices, ARRAY_SIZE(ip8000bringup_devices));

	printk(KERN_INFO "IP8000 Bringup\n");

	return 0;
}

arch_initcall(ip8000bringup_init);
