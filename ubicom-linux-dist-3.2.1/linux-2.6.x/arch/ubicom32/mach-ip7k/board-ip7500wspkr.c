/*
 * arch/ubicom32/mach-ip7k/board-ip7500wspkr.c
 *   Support for IP7500 Wireless Speaker board.
 *
 * This file supports the IP7500 Wireless Speaker board:
 *	8007-1210  Rev 1.0
 *
 * with optional LCD Adapter board:
 *	8007-1910 v1.0
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
 */
#include <linux/device.h>
#include <linux/gpio.h>
#include <asm/board.h>

#include <linux/platform_device.h>
#include <asm/audio.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
#include <asm/delay.h>
#include <asm/flash.h>

#include <asm/ubicom32input.h>
#include <linux/input.h>

#include <asm/flash.h>

static struct i2c_board_info __initdata ip7500wspkr_i2c_board_info[] = {
	/*
	 * U6, CS4350 DAC, address 0x4B
	 */
	{
		.type		= "cs4350",
		.addr		= 0x4B,
	},
};

/*
 * I2C bus on the board, SDA PE4, SCL PE5
 */
static struct i2c_gpio_platform_data ip7500wspkr_i2c_data = {
	.sda_pin		= GPIO_RD_5,
	.scl_pin		= GPIO_RD_6,
	.sda_is_open_drain	= 0,
	.scl_is_open_drain	= 0,
	.udelay			= 50,
};

static struct platform_device ip7500wspkr_i2c_device = {
	.name	= "i2c-gpio",
	.id	= 0,
	.dev	= {
		.platform_data = &ip7500wspkr_i2c_data,
	},
};

static struct ubicom32input_button ip7500wspkr_ubicom32input_button = {
	.type		= EV_KEY,
	.code		= KEY_FN_F1,
	.gpio		= GPIO_RE_3,
	.desc		= "WPS",
	.active_low	= 1,
};

static struct ubicom32input_platform_data ip7500wspkr_ubicom32input_data = {
	.nbuttons = 1,
	.buttons = &ip7500wspkr_ubicom32input_button,
};

static struct platform_device ip7500wspkr_ubicom32input_device = {
	.name	= "ubicom32input",
	.id	= 0,
	.dev	= {
		.platform_data = &ip7500wspkr_ubicom32input_data,
	},
};

static struct platform_device *ip7500wspkr_devices[] __initdata = {
	&ip7500wspkr_i2c_device,
	&ip7500wspkr_ubicom32input_device,
};

/*
 * ip7500wspkr_init
 *	Called to add the devices which we have on this board
 */
static int __init ip7500wspkr_init(void)
{
	struct platform_device *audio_dev;
	struct platform_device *audio_dev2;
	int ret;

	if (ubicom32_flash_single_init()) {
		printk(KERN_ERR "%s: could not initialize flash controller\n", __FUNCTION__);
	}

	board_init();

	ubi_gpio_init();

	/*
	 * Start the flash driver
	 */
	ubicom32_flash_single_init();

	platform_add_devices(ip7500wspkr_devices, ARRAY_SIZE(ip7500wspkr_devices));

	/*
	 * DAC nRESET line
	 */
	ret = gpio_request(GPIO_RD_7, "DAC_nRESET");
	if (ret) {
		printk(KERN_ERR "%s: could not request DAC_nRESET GPIO: %d\n", __FUNCTION__, ret);
	}
	gpio_direction_output(GPIO_RD_7, 0);
	udelay(1);
	gpio_direction_output(GPIO_RD_7, 1);

	audio_dev = audio_device_alloc("snd-ubi32-cs4350", "audio", "audio-i2sout", 0);
	if (audio_dev) {
		ip7500wspkr_i2c_board_info[0].platform_data = audio_dev;
	}

	audio_dev2 = audio_device_alloc("snd-ubi32-generic", "audio", "audio-spdifout", 0);
	if (audio_dev2) {
		platform_device_register(audio_dev2);
	}

	printk(KERN_INFO "%s: registering i2c resources\n", __FUNCTION__);
	i2c_register_board_info(0, ip7500wspkr_i2c_board_info, ARRAY_SIZE(ip7500wspkr_i2c_board_info));

	printk(KERN_INFO "IP7500 Wireless Speaker Board\n");

	return 0;
}

arch_initcall(ip7500wspkr_init);

