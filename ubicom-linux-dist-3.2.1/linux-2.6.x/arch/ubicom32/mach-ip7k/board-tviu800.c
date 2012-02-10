
/*
 * arch/ubicom32/mach-ip7k/board-tviu800.c
 *   Support for TVIU800 Internet Audio Player
 *
 * This file supports the TVIU800 Internet Audio Player:
 *	8007-1110  Rev 1.0
 *
 * With optional LCD Adapter board:
 *	8007-0920  Rev 2.0
 *	8007-0921  Rev 2.1
 *	8007-1610  Rev 1.0
 *	8007-1710  Rev 1.0
 *	8007-2110  Rev 1.0
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

#include <linux/delay.h>

#include <linux/platform_device.h>
#include <asm/audio.h>
#include <asm/ubi32-pcm.h>
#include <asm/ubi32-wm8988.h>

#include <linux/i2c.h>
#include <linux/i2c-gpio.h>

#include <asm/ubicom32sd.h>
#include <asm/sd_tio.h>
#include <asm/vdc_tio.h>
#include <linux/i2c/at24.h>

#include <asm/ubicom32bl.h>
#include <asm/ubicom32lcdpower.h>
#include <asm/lcd_adapter.h>

#include <asm/ubicom32jog.h>
#include <asm/ubicom32input.h>
#include <linux/input.h>

#include <asm/machdep.h>
#include <asm/flash.h>

#include <asm/flash.h>

/******************************************************************************
 * i2c devices
 *
 * DO NOT CHANGE THE ORDER HERE unless you know how this works.  There
 * are hardcoded indicies which refer to the order of drivers listed here.
 */
static struct i2c_board_info __initdata tviu800_i2c_board_info[] = {
	/*
	 * U6, CS4350 DAC, address 0x4B
	 */
	{
		.type		= "wm8988",
		.addr		= 0x1a,
	},

};

/******************************************************************************
 * I2C bus on the board, SDA RF_14, SCL RF_13
 */
static struct i2c_gpio_platform_data tviu800_i2c_data = {
	.sda_pin		= GPIO_RF_14,
	.scl_pin		= GPIO_RF_13,
	.sda_is_open_drain	= 0,
	.scl_is_open_drain	= 0,
};

static struct platform_device tviu800_i2c_device = {
	.name	= "i2c-gpio",
	.id	= 0,
	.dev	= {
		.platform_data = &tviu800_i2c_data,
	},
};

/******************************************************************************
 * Jog Dial platform data
 */
static struct ubicom32jog_platform_data tviu800_jog_platform_data = {
	.has_button		= 1,
	.button_active_low	= 0,
	.button_poll_interval	= 40,
	.button_gpio		= GPIO_RB_4,
	.jog_gpio_a		= GPIO_RB_10,
	.jog_gpio_b		= GPIO_RB_9,
};

static struct platform_device tviu800_jog_device = {
	.name		= "ubicom32jog",
	.dev		= {
			.platform_data = &tviu800_jog_platform_data,
	},

};

/******************************************************************************
 * Input buttons on the board
 */
static struct ubicom32input_button tviu800_ubicom32input_buttons[] = {
	{
		.type		= EV_KEY,
		.code		= KEY_FN_F1,
		.gpio		= GPIO_RE_5,
		.desc		= "WPS",
		.active_low	= 1,
	},
	{
		.type		= EV_KEY,
		.code		= KEY_POWER,
		.gpio		= GPIO_RF_10,
		.desc		= "Reset",
		.active_low	= 1,
	},
	{
		.type		= EV_KEY,
		.code		= KEY_LEFT,
		.gpio		= GPIO_RC_1,
		.desc		= "Standby_dec",
		.active_low	= 1,
	},
	{
		.type		= EV_KEY,
		.code		= KEY_RIGHT,
		.gpio		= GPIO_RC_6,
		.desc		= "GPIO_VOL+_PCE",
		.active_low	= 1,
	},
	{
		.type		= EV_KEY,
		.code		= KEY_ENTER,
		.gpio		= GPIO_RC_4,
		.desc		= "GPIO_VOL-_PCF",
		.active_low	= 1,
	},
};

static struct ubicom32input_platform_data tviu800_ubicom32input_data = {
	.nbuttons = ARRAY_SIZE(tviu800_ubicom32input_buttons),
	.buttons = tviu800_ubicom32input_buttons,
};

static struct platform_device tviu800_ubicom32input_device = {
	.name	= "ubicom32input",
	.id	= 0,
	.dev	= {
		.platform_data = &tviu800_ubicom32input_data,
	},
};

/******************************************************************************
 * Devices on this board
 */
static struct platform_device *tviu800_devices[] __initdata = {
	&tviu800_i2c_device,
	&tviu800_ubicom32input_device,
	&tviu800_jog_device,
};

/*
 * tviu800_power_off
 *	Called to turn the power off for this board
 */
static void tviu800_power_off(void)
{
	gpio_set_value(GPIO_RF_11, 0);
}

/*
 * List of possible mclks we can generate.  This depends on the CPU frequency.
 */
static struct ubi32_wm8988_mclk_entry tviu800_wm8988_mclk_entries[] = {
	{
		.rate	=	12288000,
		.div	=	40,
	},
};

/*
 * tviu800_init
 *	Called to add the devices which we have on this board
 */
static int __init tviu800_init(void)
{
	struct platform_device *audio_dev;
	struct ubi32_wm8988_platform_data *wm8988_pd;
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

	/*
	 * Hold the POWER_HOLD line
	 */
	ret = gpio_request(GPIO_RF_11, "POWER_HOLD");
	if (ret) {
		printk(KERN_ERR "%s: could not request POWER_HOLD GPIO\n", __FUNCTION__);
	}
	gpio_direction_output(GPIO_RF_11, 1);
	mach_power_off = tviu800_power_off;

	/*
	 * Bring up audio devices
	 */
	platform_add_devices(tviu800_devices, ARRAY_SIZE(tviu800_devices));

	audio_dev = audio_device_alloc("snd-ubi32-wm8988", "audio", "audio-i2sout", sizeof(struct ubi32_wm8988_platform_data));
	if (audio_dev) {
		printk(KERN_INFO "%s: setting audio_dev properties\n", __FUNCTION__);
		wm8988_pd = audio_device_priv(audio_dev);
		wm8988_pd->mclk_src = UBI32_WM8988_MCLK_CLKDIV_1;
		wm8988_pd->n_mclk = ARRAY_SIZE(tviu800_wm8988_mclk_entries);
		wm8988_pd->mclk_entries = tviu800_wm8988_mclk_entries;
		tviu800_i2c_board_info[0].platform_data = audio_dev;
	}

	printk(KERN_INFO "%s: registering i2c resources\n", __FUNCTION__);
	i2c_register_board_info(0, tviu800_i2c_board_info, ARRAY_SIZE(tviu800_i2c_board_info));

	printk(KERN_INFO "TVIU800 TechVision DLNA board\n");

	return 0;
}
arch_initcall(tviu800_init);

/* The devices and functions below this line are for late
 * init detection of video hardware
 */

/******************************************************************************
 * LCD PON for LCD Adapter board
 */
static struct ubicom32lcdpower_platform_data tviu800_lcdpower_data = {
	.vgh_gpio		= GPIO_RC_24,
	.vgh_polarity		= true,
};

static struct platform_device tviu800_lcdpower_device = {
	.name	= "ubicom32lcdpower",
	.id	= -1,
	.dev	= {
		.platform_data = &tviu800_lcdpower_data,
	},
};

/******************************************************************************
 * Backlight on the board PD0, hardware PWM
 */
static struct ubicom32bl_platform_data tviu800_backlight_data = {
	.type			= UBICOM32BL_TYPE_PWM,
	.pwm_channel		= 2,
	.pwm_prescale		= 15,
	.pwm_period		= 60,
	.default_intensity	= 0x80,
};

static struct platform_device tviu800_backlight_device = {
	.name	= "ubicom32bl",
	.id	= -1,
	.dev	= {
		.platform_data = &tviu800_backlight_data,
	},
};

/******************************************************************************
 * Touch controller
 *
 * Connected via I2C bus, interrupt on PA6
 */
#include <linux/i2c/tsc2007.h>

/*
 * tviu800_tsc2007_exit_platform_hw
 */
static void tviu800_tsc2007_exit_platform_hw(void)
{
	UBICOM32_IO_PORT(IO_PORT_RA)->ctl0 &= ~(0x03 << 19);
	gpio_free(GPIO_RA_6);
}

/*
 * tviu800_tsc2007_init_platform_hw
 */
static int tviu800_tsc2007_init_platform_hw(void)
{
	int res = gpio_request(GPIO_RA_6, "TSC2007_IRQ");
	if (res) {
		return res;
	}

	UBICOM32_IO_PORT(IO_PORT_RA)->ctl0 &= ~(0x03 << 19);
	UBICOM32_IO_PORT(IO_PORT_RA)->ctl0 |= (0x02 << 19);
	return 0;
}

/*
 * tviu800_tsc2007_get_pendown_state
 */
static int tviu800_tsc2007_get_pendown_state(void)
{
	return !gpio_get_value(GPIO_RA_6);
}

static struct tsc2007_platform_data tviu800_tsc2007_data = {
	.model			= 2007,
	.x_plate_ohms		= 350,
	.get_pendown_state	= tviu800_tsc2007_get_pendown_state,
	.init_platform_hw	= tviu800_tsc2007_init_platform_hw,
	.exit_platform_hw	= tviu800_tsc2007_exit_platform_hw,
};

static struct i2c_board_info __initdata tviu800_touch_i2c_board_info = {
	.type		= "tsc2007",
	.addr		= 0x49,
	.irq		= 46, // PA6
	.platform_data	= &tviu800_tsc2007_data,
};

/*
 * Virtual Frame Buffer device for use with LCD Adapter
 */
static struct platform_device tviu800_vfb_device = {
	.name	= "ubicom32vfb",
	.id	= -1,
};

/*
 * tviu800_init_onboard_video
 */
static int __init tviu800_init_onboard_video(void)
{
	struct i2c_adapter *adap;

	/*
	 * Use the VFB device for the onboard display
	 */
	platform_device_register(&tviu800_vfb_device);

	platform_device_register(&tviu800_backlight_device);

	/* turn on LCD power */
	tviu800_lcdpower_data.vgh_gpio = GPIO_RC_24;
	platform_device_register(&tviu800_lcdpower_device);

	adap = i2c_get_adapter(0);
	if (!adap) {
		printk(KERN_WARNING "%s: Failed to get i2c adapter\n", __FUNCTION__);
		return -ENODEV;
	}

	printk(KERN_INFO "%s: registering touch screen controller resources\n", __FUNCTION__);

	i2c_new_device(adap, &tviu800_touch_i2c_board_info);

	i2c_put_adapter(adap);

	return 0;
}
late_initcall(tviu800_init_onboard_video);

