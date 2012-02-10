/*
 * arch/ubicom32/mach-ip7k/board-ip7500module.c
 *   Support for IP7500 CPU module board.
 *
 * This file supports the IP7500 CPU module board:
 *	8007-0510  Rev 1.0
 *	8007-0510A Rev 1.0 (with ethernet)
 *	8007-0511  Rev 1.1
 *	8007-0511A Rev 1.1 (with ethernet)
 *
 * DIP Switch SW2 configuration: (*) default
 *	POS 1: on(*) = PCI enabled, off = PCI disabled
 *	POS 2: on(*) = TTYX => PA6, off = TTYX => PF12
 *	POS 3: on(*) = TTYY => PA7, off = TTYY => PF15
 *	POS 4: unused
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
#include <linux/gpio.h>
#include <asm/board.h>

#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
#include <linux/i2c/at24.h>

#include <asm/vdc_tio.h>
#include <asm/flash.h>

#include <asm/flash.h>

/******************************************************************************
 * Touch controller present on LCD Adapter board
 *
 * Connected via I2C bus, interrupt on PD1
 */
#include <linux/i2c/tsc2007.h>

/*
 * ip7500media_tsc2007_exit_platform_hw
 */
static void ip7500media_tsc2007_exit_platform_hw(void)
{
	UBICOM32_IO_PORT(IO_PORT_RD)->int_mask &= ~(1 << 11);
	UBICOM32_IO_PORT(IO_PORT_RD)->ctl2 &= ~(0x03 << 16);
	gpio_free(GPIO_RD_1);
}

/*
 * ip7500media_tsc2007_init_platform_hw
 */
static int ip7500media_tsc2007_init_platform_hw(void)
{
	int res = gpio_request(GPIO_RD_1, "TSC2007_IRQ");
	if (res) {
		return res;
	}
	UBICOM32_IO_PORT(IO_PORT_RD)->function = 0;
	UBICOM32_IO_PORT(IO_PORT_RD)->int_mask = (1 << 11);
	UBICOM32_IO_PORT(IO_PORT_RD)->ctl2 &= ~(0x03 << 16);
	UBICOM32_IO_PORT(IO_PORT_RD)->ctl2 |= (0x02 << 16);

	return 0;
}

/*
 * ip7500media_tsc2007_clear_penirq
 */
static void ip7500media_tsc2007_clear_penirq(void)
{
	UBICOM32_IO_PORT(IO_PORT_RD)->int_clr = (1 << 11);
}

/*
 * ip7500media_tsc2007_get_pendown_state
 */
static int ip7500media_tsc2007_get_pendown_state(void)
{
	return !gpio_get_value(GPIO_RD_1);
}

static struct tsc2007_platform_data ip7500media_tsc2007_data = {
	.model			= 2007,
	.x_plate_ohms		= 350,
	.get_pendown_state	= ip7500media_tsc2007_get_pendown_state,
	.init_platform_hw	= ip7500media_tsc2007_init_platform_hw,
	.exit_platform_hw	= ip7500media_tsc2007_exit_platform_hw,
	.clear_penirq		= ip7500media_tsc2007_clear_penirq,
};

/*
 * I2C bus on the board, SDA PE4, SCL PE5
 */
static struct i2c_gpio_platform_data ip7500module_i2c_data = {
	.sda_pin		= GPIO_RA_5,
	.scl_pin		= GPIO_RA_4,
	//.sda_pin		= GPIO_RE_4,
	//.scl_pin		= GPIO_RE_5,
	.sda_is_open_drain	= 0,
	.scl_is_open_drain	= 0,
	.udelay			= 50,
};

static struct platform_device ip7500module_i2c_device = {
	.name	= "i2c-gpio",
	.id	= 0,
	.dev	= {
		.platform_data = &ip7500module_i2c_data,
	},
};

static struct at24_platform_data ip7500module_at24_platform_data = {
	.byte_len	= 8 * 256,
	.page_size	= 256,
};

static struct i2c_board_info __initdata ip7500module_i2c_board_info[] = {
	{
		.type		= "24c16",
		.addr		= 0x50,
		.platform_data  = &ip7500module_at24_platform_data,
	},
	{
		I2C_BOARD_INFO("tsc2007", 0x48),
		.irq = PORT_OTHER_INT(IO_PORT_RD),
		.platform_data = &ip7500media_tsc2007_data,
	},
};


static struct platform_device *ip7500module_devices[] __initdata = {
	&ip7500module_i2c_device,
};

/*
 * ip7500module_init
 *	Called to add the devices which we have on this board
 */
static int __init ip7500module_init(void)
{
	if (ubicom32_flash_single_init()) {
		printk(KERN_ERR "%s: could not initialize flash controller\n", __FUNCTION__);
	}

	board_init();

	ubi_gpio_init();

	/*
	 * Start the flash driver
	 */
	ubicom32_flash_single_init();

	platform_add_devices(ip7500module_devices, ARRAY_SIZE(ip7500module_devices));

	printk(KERN_INFO "%s: registering i2c resources\n", __FUNCTION__);
	i2c_register_board_info(0, ip7500module_i2c_board_info, ARRAY_SIZE(ip7500module_i2c_board_info));

	//vdc_tio_init(NULL);

	return 0;
}

arch_initcall(ip7500module_init);

