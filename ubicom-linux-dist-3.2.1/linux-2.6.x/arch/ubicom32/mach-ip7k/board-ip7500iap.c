/*
 * arch/ubicom32/mach-ip7k/board-ip7500iap.c
 *   Support for IP7500 Internet Audio Player
 *
 * This file supports the IP7500 Internet Audio Player:
 *	8007-1110  Rev 1.0
 *	8007-1111  Rev 1.1
 *	8007-1112  Rev 1.2
 *
 * With optional LCD Adapter board:
 *	8000-0210  Rev 1.0
 *	8000-0310  Rev 1.0
 *	8007-0920  Rev 2.0
 *	8007-0921  Rev 2.1
 *	8007-1610  Rev 1.0
 *	8007-1611  Rev 1.1
 *	8007-1710  Rev 1.0
 *	8007-2110  Rev 1.0
 *	8007-2210  Rev 1.0
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
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>

#include <asm/ubicom32sd.h>
#include <asm/sd_tio.h>
#include <asm/vdc_tio.h>
#include <linux/i2c/at24.h>

#include <asm/ubicom32bl.h>
#include <asm/ubicom32lcdpower.h>
#include <asm/lcd_adapter.h>

#include <asm/ubicom32input.h>
#include <linux/input.h>

#include <asm/flash.h>

#include <asm/machdep.h>
#include <asm/flash.h>
#include <asm/adv7393.h>
#include <asm/tfp410.h>

#ifdef CONFIG_SERIAL_UBI32_SERDES
#include <asm/ubicom32suart.h>
#endif

/******************************************************************************
 * ubicom32fb VDC platform data
 */
struct ubicom32fb_platform_data ip7500iap_ubicom32fb_platform_data __initdata;

/******************************************************************************
 * SD/IO Port F (Slot 1) platform data
 */
static struct resource ip7500iap_portf_sd_resources[] = {
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

static struct ubicom32sd_card ip7500iap_portf_sd_cards[] = {
	[0] = {
		.pin_wp		= GPIO_RF_7,
		.wp_polarity	= 1,
		.pin_pwr	= GPIO_RF_8,
		.pin_cd		= GPIO_RF_6,
	},
};

static struct ubicom32sd_platform_data ip7500iap_portf_sd_platform_data = {
	.ncards		= 1,
	.cards		= ip7500iap_portf_sd_cards,
};

static struct platform_device ip7500iap_portf_sd_device = {
	.name		= "ubicom32sd",
	.id		= 0,
	.resource	= ip7500iap_portf_sd_resources,
	.num_resources	= ARRAY_SIZE(ip7500iap_portf_sd_resources),
	.dev		= {
			.platform_data = &ip7500iap_portf_sd_platform_data,
	},

};

/*
 * ip7500iap_portf_sd_init
 */
static void ip7500iap_portf_sd_init(void)
{
	/*
	 * Check the device tree for the sd_tio
	 */
	struct sd_tio_node *sd_node = (struct sd_tio_node *)devtree_find_node("portf_sd");
	if (!sd_node) {
		printk(KERN_INFO "PortF SDTIO not found\n");
		return;
	}

	/*
	 * For IP7500IAP, if we have Port F SDTIO, we need to reclaim RF_4 which
	 * is otherwise used as ethernet phy reset.
	 */
	UBICOM32_GPIO_DISABLE(GPIO_RF_4);

	/*
	 * Fill in the resources and platform data from devtree information
	 */
	ip7500iap_portf_sd_resources[0].start = sd_node->dn.sendirq;
	ip7500iap_portf_sd_resources[1].start = sd_node->dn.recvirq;
	ip7500iap_portf_sd_resources[2].start = (u32_t)&(sd_node->regs);
	ip7500iap_portf_sd_resources[2].end = (u32_t)&(sd_node->regs) + sizeof(sd_node->regs);

	platform_device_register(&ip7500iap_portf_sd_device);
}

/******************************************************************************
 * SD/IO Port B (Slot 2) platform data
 */
static struct resource ip7500iap_portb_sd_resources[] = {
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

static struct ubicom32sd_card ip7500iap_portb_sd_cards[] = {
	[0] = {
		.pin_wp		= GPIO_RB_13,
		.wp_polarity	= 1,
		.pin_pwr	= GPIO_RB_11,
		.pin_cd		= GPIO_RB_12,
	},
};

static struct ubicom32sd_platform_data ip7500iap_portb_sd_platform_data = {
	.ncards		= 1,
	.cards		= ip7500iap_portb_sd_cards,
};

static struct platform_device ip7500iap_portb_sd_device = {
	.name		= "ubicom32sd",
	.id		= 1,
	.resource	= ip7500iap_portb_sd_resources,
	.num_resources	= ARRAY_SIZE(ip7500iap_portb_sd_resources),
	.dev		= {
			.platform_data = &ip7500iap_portb_sd_platform_data,
	},

};

/*
 * ip7500iap_portb_sd_init
 */
static void ip7500iap_portb_sd_init(void)
{
	/*
	 * Check the device tree for the sd_tio
	 */
	struct sd_tio_node *sd_node = (struct sd_tio_node *)devtree_find_node("portb_sd");
	if (!sd_node) {
		printk(KERN_INFO "PortB SDTIO not found\n");
		return;
	}

	/*
	 * Fill in the resources and platform data from devtree information
	 */
	ip7500iap_portb_sd_resources[0].start = sd_node->dn.sendirq;
	ip7500iap_portb_sd_resources[1].start = sd_node->dn.recvirq;
	ip7500iap_portb_sd_resources[2].start = (u32_t)&(sd_node->regs);
	ip7500iap_portb_sd_resources[2].end = (u32_t)&(sd_node->regs) + sizeof(sd_node->regs);

	platform_device_register(&ip7500iap_portb_sd_device);
}

/*
 * 24C16 EEPROM found on LCD Adapter board only
 */
static struct at24_platform_data ip7500iap_at24_platform_data = {
	.byte_len	= 8 * 256,
	.page_size	= 256,
};

/******************************************************************************
 * i2c devices
 *
 * DO NOT CHANGE THE ORDER HERE unless you know how this works.  There
 * are hardcoded indicies which refer to the order of drivers listed here.
 */
static struct i2c_board_info __initdata ip7500iap_i2c_board_info[] = {
	/*
	 * U6, CS4350 DAC, address 0x4B
	 */
	{
		.type		= "cs4350",
		.addr		= 0x4B,
	},

	/*
	 * U20, S35390A RTC, address 0x30
	 */
	{
		.type           = "s35390a",
		.addr           = 0x30,
	},

	/*
	 * 24C16 EEPROM found on LCD Adapter board only
	 */
	{
		.type		= "24c16",
		.addr		= 0x50,
		.platform_data	= &ip7500iap_at24_platform_data,
	},
};

/******************************************************************************
 * I2C bus on the board, SDA RF_14, SCL RF_13
 */
static struct i2c_gpio_platform_data ip7500iap_i2c_data = {
	.sda_pin		= GPIO_RF_14,
	.scl_pin		= GPIO_RF_13,
	.sda_is_open_drain	= 0,
	.scl_is_open_drain	= 0,
};

static struct platform_device ip7500iap_i2c_device = {
	.name	= "i2c-gpio",
	.id	= 0,
	.dev	= {
		.platform_data = &ip7500iap_i2c_data,
	},
};

/******************************************************************************
 * Input buttons on the board
 */
static struct ubicom32input_button ip7500iap_ubicom32input_buttons[] = {
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
		.desc		= "Power",
		.active_low	= 1,
	},
};

static struct ubicom32input_platform_data ip7500iap_ubicom32input_data = {
	.nbuttons = ARRAY_SIZE(ip7500iap_ubicom32input_buttons),
	.buttons = ip7500iap_ubicom32input_buttons,
};

static struct platform_device ip7500iap_ubicom32input_device = {
	.name	= "ubicom32input",
	.id	= 0,
	.dev	= {
		.platform_data = &ip7500iap_ubicom32input_data,
	},
};

#ifdef CONFIG_SERIAL_UBI32_SERDES
static struct resource ip7500iap_ubicom32_suart_resources[] = {
	{
		.start	= IO_PORT_RE,
		.end	= IO_PORT_RE,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= PORT_OTHER_INT(IO_PORT_RE),
		.end	= PORT_OTHER_INT(IO_PORT_RE),
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= 250000000,
		.end	= 250000000,
		.flags	= UBICOM32_SUART_IORESOURCE_CLOCK,
	},
};

static struct platform_device ip7500iap_ubicom32_suart_device = {
	.name		= "ubicom32suart",
	.id		= -1,
	.num_resources  = ARRAY_SIZE(ip7500iap_ubicom32_suart_resources),
	.resource	= ip7500iap_ubicom32_suart_resources,
};
#endif

/******************************************************************************
 * Devices on this board
 */
static struct platform_device *ip7500iap_devices[] __initdata = {
	&ip7500iap_i2c_device,
	&ip7500iap_ubicom32input_device,
#ifdef CONFIG_SERIAL_UBI32_SERDES
	&ip7500iap_ubicom32_suart_device,
#endif
};

/*
 * ip7500iap_power_off
 *	Called to turn the power off for this board
 */
static void ip7500iap_power_off(void)
{
	gpio_set_value(GPIO_RF_11, 0);
}

/*
 * ip7500iap_init
 *	Called to add the devices which we have on this board
 */
static int __init ip7500iap_init(void)
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

	/*
	 * Hold the POWER_HOLD line
	 */
	ret = gpio_request(GPIO_RF_11, "POWER_HOLD");
	if (ret) {
		printk(KERN_ERR "%s: could not request POWER_HOLD GPIO\n", __FUNCTION__);
	}
	gpio_direction_output(GPIO_RF_11, 1);
	mach_power_off = ip7500iap_power_off;

#ifndef CONFIG_DISPLAY_AUTODETECT
	/*
	 * Bring up video
	 */
	vdc_tio_fill_platform_data(&ip7500iap_ubicom32fb_platform_data);

	printk(KERN_INFO "%s: initializing video driver\n", __FUNCTION__);

	vdc_tio_init(&ip7500iap_ubicom32fb_platform_data);
#endif

	/*
	 * DAC nRESET line
	 */
	ret = gpio_request(GPIO_RE_7, "DAC_nRESET");
	if (ret) {
		printk(KERN_ERR "%s: could not request DAC_nRESET GPIO\n", __FUNCTION__);
	}
	gpio_direction_output(GPIO_RE_7, 0);
	udelay(1);
	gpio_set_value(GPIO_RE_7, 1);

	/*
	 * Bring up any SDIO slots
	 */
	ip7500iap_portb_sd_init();
	ip7500iap_portf_sd_init();

#ifdef CONFIG_SOUND
	/*
	 * Bring up audio devices
	 */
	platform_add_devices(ip7500iap_devices, ARRAY_SIZE(ip7500iap_devices));

	audio_dev = audio_device_alloc("snd-ubi32-cs4350", "audio", "audio-i2sout", 0);
	if (audio_dev) {
		ip7500iap_i2c_board_info[0].platform_data = audio_dev;
	}

	audio_dev2 = audio_device_alloc("snd-ubi32-generic", "audio", "audio-spdifout", 0);
	if (audio_dev2) {
		platform_device_register(audio_dev2);
	}

	printk(KERN_INFO "%s: registering i2c resources\n", __FUNCTION__);
	i2c_register_board_info(0, ip7500iap_i2c_board_info, ARRAY_SIZE(ip7500iap_i2c_board_info));
#endif

	printk(KERN_INFO "IP7500 Internet Audio Player\n");
	return 0;
}

arch_initcall(ip7500iap_init);

/*
 * The devices and functions below this line are for late
 * init detection of video hardware
 */

/******************************************************************************
 * LCD PON for LCD Adapter board
 */
static struct ubicom32lcdpower_platform_data ip7500iap_lcdpower_data = {
	.vgh_polarity		= true,
};

static struct platform_device ip7500iap_lcdpower_device = {
	.name	= "ubicom32lcdpower",
	.id	= -1,
	.dev	= {
		.platform_data = &ip7500iap_lcdpower_data,
	},
};

/******************************************************************************
 * Backlight on the board PD0, hardware PWM
 */
static struct ubicom32bl_platform_data ip7500iap_backlight_data = {
	.type			= UBICOM32BL_TYPE_PWM,
	.pwm_channel		= 2,
	.pwm_prescale		= 15,
	.pwm_period		= 60,
	.default_intensity	= 0x80,
};

static struct platform_device ip7500iap_backlight_device = {
	.name	= "ubicom32bl",
	.id	= -1,
	.dev	= {
		.platform_data = &ip7500iap_backlight_data,
	},
};

/******************************************************************************
 * Touch controller
 *
 * Connected via I2C bus, interrupt on PA6
 */
#include <linux/i2c/tsc2007.h>

/*
 * ip7500iap_tsc2007_exit_platform_hw
 */
static void ip7500iap_tsc2007_exit_platform_hw(void)
{
	UBICOM32_IO_PORT(IO_PORT_RA)->ctl0 &= ~(0x03 << 19);
	gpio_free(GPIO_RA_6);
}

/*
 * ip7500iap_tsc2007_init_platform_hw
 */
static int ip7500iap_tsc2007_init_platform_hw(void)
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
 * ip7500iap_tsc2007_get_pendown_state
 */
static int ip7500iap_tsc2007_get_pendown_state(void)
{
	return !gpio_get_value(GPIO_RA_6);
}

static struct tsc2007_platform_data ip7500iap_tsc2007_data = {
	.model			= 2007,
	.x_plate_ohms		= 350,
	.get_pendown_state	= ip7500iap_tsc2007_get_pendown_state,
	.init_platform_hw	= ip7500iap_tsc2007_init_platform_hw,
	.exit_platform_hw	= ip7500iap_tsc2007_exit_platform_hw,
};

static struct i2c_board_info __initdata ip7500iap_touch_i2c_board_info = {
	.type		= "tsc2007",
	.addr		= 0x49,
	.irq		= 46, // PA6
	.platform_data	= &ip7500iap_tsc2007_data,
};

#ifdef CONFIG_DISPLAY_AUTODETECT
/*
 * Virtual Frame Buffer device for use with LCD Adapter
 */
static struct platform_device ip7500iap_vfb_device = {
	.name	= "ubicom32vfb",
	.id	= -1,
};
#endif

/*
 * ip7500iap_init_offboard_video
 */
static int __init ip7500iap_init_offboard_video(struct lcd_adapter_info *info)
{
	struct i2c_adapter *adap;

#ifdef CONFIG_FB_UBICOM32
	/*
	 * If we are autodetect then bring up the display now
	 */
#ifdef CONFIG_DISPLAY_AUTODETECT
	/*
	 * Check for 8000-0210 PPI-TFP410
	 */
	if (info->panel_type == 12) {
		tfp410_init(0, 0x38, GPIO_RD_1);
	}

	/*
	 * Check for 8000-0310 PPI-ADV7393
	 */
	if (info->panel_type == 11) {
		adv7393_init(0, 0x2B, GPIO_RD_1);
	}

	/*
	 * Use the VDC for devices which are HV/TCON
	 */
	if (info->driver_type[0] & (LCD_ADAPTER_DTYPE0_HV | LCD_ADAPTER_DTYPE0_TCON | LCD_ADAPTER_DTYPE0_DE)) {
		struct ubicom32fb_platform_data *pd = &ip7500iap_ubicom32fb_platform_data;

		memset(pd, 0, sizeof(*pd));

		if (info->driver_type[0] & LCD_ADAPTER_DTYPE0_HV) {
			pd->mode = UBICOM32FB_MODE_SYNC;
		} else if (info->driver_type[0] & LCD_ADAPTER_DTYPE0_TCON) {
			pd->mode = UBICOM32FB_MODE_TCON;
		} else if (info->driver_type[0] & LCD_ADAPTER_DTYPE0_DE) {
			pd->mode = UBICOM32FB_MODE_DE;
		}

		pd->flags |= UBICOM32FB_PLATFORM_DATA_FLAG_TIMING | UBICOM32FB_PLATFORM_DATA_FLAG_MODE;
		pd->hfp = info->hfp;
		pd->hsync = info->hsync;
		pd->hbp = info->hbp;
		pd->vfp = info->vfp;
		pd->vsync = info->vsync;
		pd->vbp = info->vbp;
		pd->fclk = info->fclk;

		if (info->active_h && info->active_w) {
			pd->flags |= UBICOM32FB_PLATFORM_DATA_FLAG_DIM;
			pd->height_mm = info->active_h / 10;
			pd->width_mm = info->active_w / 10;
		}

		vdc_tio_init(pd);
	} else {
		/*
		 * Use the VFB device for other displays
		 */
		platform_device_register(&ip7500iap_vfb_device);
	}
#endif

	/*
	 * Start up the LCD Power driver if needed
	 */
	if (info->driver_type[2] & LCD_ADAPTER_DTYPE2_POWER) {
		if (info->lcd_power_pin == 38) {
			ip7500iap_lcdpower_data.vgh_gpio = GPIO_RD_2;
			platform_device_register(&ip7500iap_lcdpower_device);
		} else {
			printk(KERN_WARNING "%s: don't know about LCD power pin %u\n", __FUNCTION__, info->lcd_power_pin);
		}
	}

	/*
	 * Start up the backlight driver if needed
	 */
	if (info->backlight_type == 0x01) {
		ip7500iap_backlight_data.pwm_prescale = 7;
		platform_device_register(&ip7500iap_backlight_device);
	} else if (info->backlight_type == 0x02) {
		platform_device_register(&ip7500iap_backlight_device);
	}

	printk(KERN_INFO "%s: registering LCD Adapter Board resources\n", __FUNCTION__);

	adap = i2c_get_adapter(0);
	if (!adap) {
		printk(KERN_WARNING "%s: Failed to get i2c adapter\n", __FUNCTION__);
		return -ENODEV;
	}

	/*
	 * Offboard touch controller if needed.
	 */
	if (info->touch_addr) {
		ip7500iap_touch_i2c_board_info.addr = info->touch_addr;
		i2c_new_device(adap, &ip7500iap_touch_i2c_board_info);
	}

	i2c_put_adapter(adap);
#endif	// CONFIG_FB_UBICOM32

	return 0;
}

#ifdef CONFIG_FB_UBICOM32
/*
 * ip7500iap_init_onboard_video
 */
static int __init ip7500iap_init_onboard_video(void)
{
	struct i2c_adapter *adap;

	/*
	 * If we are autodetect then bring up the display now
	 */
#ifdef CONFIG_DISPLAY_AUTODETECT
	/*
	 * Use the VFB device for the onboard display
	 */
	platform_device_register(&ip7500iap_vfb_device);
#endif

	platform_device_register(&ip7500iap_backlight_device);

	adap = i2c_get_adapter(0);
	if (!adap) {
		printk(KERN_WARNING "%s: Failed to get i2c adapter\n", __FUNCTION__);
		return -ENODEV;
	}

	printk(KERN_INFO "%s: registering touch screen controller resources\n", __FUNCTION__);

	i2c_new_device(adap, &ip7500iap_touch_i2c_board_info);

	i2c_put_adapter(adap);

	return 0;
}

/*
 * ip7500iap_late_init
 *	Called late to determine what kind of video we have on this board
 */
static int __init ip7500iap_late_init(void)
{
	int ret;
	struct lcd_adapter_info info;

	ret = lcd_adapter_probe(0, &info);
	if (ret < 0) {
		printk(KERN_INFO "No LCD Adapter found (%d) using onboard video.\n", ret);
		return ip7500iap_init_onboard_video();
	}

	printk(KERN_INFO "LCD Adapter Board found\n");
	return ip7500iap_init_offboard_video(&info);
}
late_initcall(ip7500iap_late_init);
#endif // CONFIG_FB_UBICOM32

