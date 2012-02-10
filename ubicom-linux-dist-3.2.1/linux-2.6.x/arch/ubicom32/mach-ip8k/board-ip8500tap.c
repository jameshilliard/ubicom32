/*
 * arch/ubicom32/mach-ip8k/board-ip8500tap.c
 *   Support for the IP8500 TAP board.
 *
 * This file supports the following boards:
 *	8008-0110 Rev 1.0	IP8K TAP Board
 *	8008-0111 Rev 1.1	IP8K TAP Board
 *
 * (C) Copyright 2010-2011, Ubicom, Inc.
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
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/leds.h>
#include <linux/delay.h>
#include <linux/input.h>

#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
#include <linux/i2c/pca953x.h>
#include <linux/i2c/at24.h>

#include <asm/board.h>
#include <asm/machdep.h>
#include <asm/ubicom32input.h>
#include <asm/gpio-ip8500tap.h>

#include <asm/flash.h>
#include <asm/ubi32-fc.h>
#include <asm/ubi32-nand-spi-er.h>

#ifdef CONFIG_SERIAL_UBI32_SERDES
#include <asm/ubicom32suart.h>
#endif

#ifdef CONFIG_FB_UBICOM32
#include <asm/vdc_tio.h>
#include <asm/ubicom32bl.h>
#include <asm/ubicom32lcdpower.h>
#include <asm/lcd_adapter.h>

/******************************************************************************
 * ubicom32fb VDC platform data
 */
struct ubicom32fb_platform_data ip8500tap_ubicom32fb_platform_data __initdata;
#endif // CONFIG_FB_UBICOM32

#ifdef CONFIG_SOUND
# include <asm/audio.h>
# include <asm/ubi32-pcm.h>
# if !defined(CONFIG_IP8500TAP_DAO)
#  include <asm/ubi32-cs4384.h>

/*
 * List of possible mclks we can generate.  This depends on the CPU frequency.
 */
static struct ubi32_cs4384_mclk_entry ip8500tap_cs4384_mclk_entries[] = {
	{
		.rate	=	12288000,
		.div	=	44,
	},
	{
		.rate	=	11289600,
		.div	=	48,
	},
};
# endif // !CONFIG_IP8500TAP_DAO
#endif // CONFIG_SOUND

/*****************************************************************************
 * Use ubicom32input driver to monitor the various pushbuttons on this board.
 */
static struct ubicom32input_button ip8500tap_ubicom32input_buttons[] = {
	{
		.type           = EV_KEY,
		.code           = KEY_BACK,
		.gpio           = GPIO_PG3_22,
		.desc           = "WPS",
		.active_low     = 1,
	},
};

static struct ubicom32input_platform_data ip8500tap_ubicom32input_data = {
	.buttons	= ip8500tap_ubicom32input_buttons,
	.nbuttons	= ARRAY_SIZE(ip8500tap_ubicom32input_buttons),
};

static struct platform_device ip8500tap_ubicom32input_device = {
	.name	= "ubicom32input",
	.id	= -1,
	.dev	= {
		.platform_data = &ip8500tap_ubicom32input_data,
	},
};

#ifdef CONFIG_SERIAL_UBI32_SERDES
static struct resource ip8500tap_ubicom32_suart_resources[] = {
	{
		.start	= IO_PORT_RO,
		.end	= IO_PORT_RO,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= RO_SERDES_RX_INT,
		.end	= RO_SERDES_RX_INT,
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= 250000000,
		.end	= 250000000,
		.flags	= UBICOM32_SUART_IORESOURCE_CLOCK,
	},
};

static struct platform_device ip8500tap_ubicom32_suart_device = {
	.name		= "ubicom32suart",
	.id		= -1,
	.num_resources  = ARRAY_SIZE(ip8500tap_ubicom32_suart_resources),
	.resource	= ip8500tap_ubicom32_suart_resources,
};
#endif

/*****************************************************************************
 * Additional GPIO chips
 */
static struct pca953x_platform_data ip8500tap_gpio_u35_platform_data = {
	.gpio_base = IP8500TAP_U35_BASE,
};

static struct pca953x_platform_data ip8500tap_gpio_u38_platform_data = {
	.gpio_base = IP8500TAP_U38_BASE,
};

/*
 * 24C16 EEPROM found on LCD Adapter board only
 */
static struct at24_platform_data ip8500tap_at24_platform_data = {
	.byte_len	= 8 * 256,
	.page_size	= 256,
};

/******************************************************************************
 * Devices on the I2C bus #0
 *
 * BEWARE of changing the order of things in this array as we depend on
 * certain things to be in certain places.
 */
static struct i2c_board_info __initdata ip8500tap_i2c_board_info[] = {
#if !defined(CONFIG_IP8500TAP_DAO)
	/*
	 * U21, CS4384 DAC, address 0x19 (MUST BE LISTED AT INDEX [0])
	 */
	{
		.type		= "cs4384",
		.addr		= 0x19,
	},
#endif
	/*
	 * U35, MAX7310 IO expander, 8 bits, address 0x1A
	 *      IO0: D15       IO4: PCIE-RESET
	 *      IO1: D16       IO5: SW4
	 *      IO2: D17       IO6: SW5
	 *      IO3: D18       IO7: SW6
	 * see gpio-ip8500tap.h for more information
	 */
	{
		.type           = "max7310",
		.addr           = 0x1A,
		.platform_data  = &ip8500tap_gpio_u35_platform_data,
	},

	/*
	 * U38, MAX7310 IO expander, 8 bits, address 0x1C
	 * see gpio-ip8500tap.h for more information
	 */
	{
		.type           = "max7310",
		.addr           = 0x1C,
		.platform_data  = &ip8500tap_gpio_u38_platform_data,
	},

	/*
	 * U36, S35390A RTC, address 0x30
	 */
	{
		.type           = "s35390a",
		.addr           = 0x30,
	},

	/*
	 * U22, MAX4550 Crosspoint Switch, address 0x4D
	 */
	{
		.type		= "max4550",
		.addr		= 0x4D,
	},

	/*
	 * 24C16 EEPROM found on LCD Adapter board only
	 */
	{
		.type		= "24c16",
		.addr		= 0x50,
		.platform_data	= &ip8500tap_at24_platform_data,
	},
};

/******************************************************************************
 * I2C bus #1 on the board, SDA G4P20, SCL G4P21
 */
static struct i2c_gpio_platform_data ip8500tap_i2c_data_1 = {
	.sda_pin                = GPIO_PG4_20,
	.scl_pin                = GPIO_PG4_21,
	.sda_is_open_drain      = 0,
	.scl_is_open_drain      = 0,
	.udelay                 = 100,
};

static struct platform_device ip8500tap_i2c_device_1 = {
	.name   = "i2c-gpio",
	.id     = 1,
	.dev    = {
		.platform_data = &ip8500tap_i2c_data_1,
	},
};

/******************************************************************************
 * I2C bus #0 on the board, SDA G4P16, SCL G4P17
 */
static struct i2c_gpio_platform_data ip8500tap_i2c_data = {
	.sda_pin                = GPIO_PG4_16,
	.scl_pin                = GPIO_PG4_17,
	.sda_is_open_drain      = 0,
	.scl_is_open_drain      = 0,
	.udelay                 = 50,
};

static struct platform_device ip8500tap_i2c_device = {
	.name   = "i2c-gpio",
	.id     = 0,
	.dev    = {
		.platform_data = &ip8500tap_i2c_data,
	},
};


/*
 * List of all devices in our system
 */
static struct platform_device *ip8500tap_devices[] __initdata = {
#ifdef CONFIG_SERIAL_UBI32_SERDES
	&ip8500tap_ubicom32_suart_device,
#endif
	&ip8500tap_ubicom32input_device,
	&ip8500tap_i2c_device,
	&ip8500tap_i2c_device_1,
};

/*
 * ip8500tap_nand_select
 */
static void ip8500tap_nand_select(void *appdata)
{
	gpio_set_value(GPIO_PG4_24, 1);
}

/*
 * ip8500tap_nor_select
 */
static void ip8500tap_nor_select(void *appdata)
{
	gpio_set_value(GPIO_PG4_24, 0);
}

/*
 * ip8500tap_init
 *	Called to add the devices which we have on this board
 */
static int __init ip8500tap_init(void)
{
#if defined(CONFIG_SOUND) && !defined(CONFIG_IP8500TAP_DAO)
	struct platform_device *audio_dev;
#endif
	int ret;
	void *tmp;

	board_init();

	ubi_gpio_init();

	/*
	 * Flash storage devices
	 *      NAND selected when PG4[24] is HIGH
	 *      NOR selected when PG4[24] is LOW
	 */
	ubicom32_flash_init();
	ret = gpio_request(GPIO_PG4_24, "FLASH SEL");
	if (ret) {
		printk(KERN_ERR "%s: Could not request FLASH_SEL\n", __FUNCTION__);
	}
	gpio_direction_output(GPIO_PG4_24, 0);

	tmp = ubicom32_flash_alloc(ip8500tap_nor_select, NULL, NULL);
	if (!tmp) {
		printk(KERN_ERR "%s: Could not alloc NOR flash\n", __FUNCTION__);
	} else {
		struct ubicom32fc_platform_data pd;
		memset(&pd, 0, sizeof(pd));
		pd.select = ubicom32_flash_select;
		pd.unselect = ubicom32_flash_unselect;
		pd.appdata = tmp;
		ret = ubicom32_flash_add("ubicom32fc", 0, &pd, sizeof(pd));
		if (ret) {
			printk(KERN_ERR "%s: Could not add NOR flash\n", __FUNCTION__);
		}
	}

	tmp = ubicom32_flash_alloc(ip8500tap_nand_select, NULL, NULL);
	if (!tmp) {
		printk(KERN_ERR "%s: Could not alloc NAND flash\n", __FUNCTION__);
	} else {
		struct ubi32_nand_spi_er_platform_data pd;
		memset(&pd, 0, sizeof(pd));
		pd.select = ubicom32_flash_select;
		pd.unselect = ubicom32_flash_unselect;
		pd.appdata = tmp;
		ret = ubicom32_flash_add("ubi32-nand-spi-er", 0, &pd, sizeof(pd));
		if (ret) {
			printk(KERN_ERR "%s: Could not add NAND flash\n", __FUNCTION__);
		}
	}

#ifdef CONFIG_FB_UBICOM32
# ifndef CONFIG_IP8500TAP_AUTODETECT
	/*
	 * Bring up video
	 */
	vdc_tio_fill_platform_data(&ip8500tap_ubicom32fb_platform_data);

	printk(KERN_INFO "%s: initializing video driver\n", __FUNCTION__);

	vdc_tio_init(&ip8500tap_ubicom32fb_platform_data);
# endif // CONFIG_IP8500TAP_AUTODETECT
#endif // CONFIG_FB_UBICOM32

	printk(KERN_INFO "%s: registering device resources\n", __FUNCTION__);
	platform_add_devices(ip8500tap_devices, ARRAY_SIZE(ip8500tap_devices));

#if defined(CONFIG_SOUND) && !defined(CONFIG_IP8500TAP_DAO)
	/*
	 * CS4384 DAC
	 */
	audio_dev = audio_device_alloc("snd-ubi32-cs4384", "audio", "audio-i2sout", 
			sizeof(struct ubi32_cs4384_platform_data));
	if (audio_dev) {
		/*
		 * Attempt to figure out a good divisor.  This will only work
		 * assuming the core frequency is compatible.
		 */
		struct ubi32_cs4384_platform_data *cs4384_pd;
		int i;
		unsigned int freq = processor_frequency();
		for (i = 0; i < ARRAY_SIZE(ip8500tap_cs4384_mclk_entries); i++) {
			unsigned int div;
			unsigned int rate = ip8500tap_cs4384_mclk_entries[i].rate / 1000;
			div = ((freq / rate) + 500) / 1000;
			ip8500tap_cs4384_mclk_entries[i].div = div;
			printk("CS4384 mclk %d rate %u000Hz div %u act %u\n", i, rate, div, freq / div);
		}

		cs4384_pd = audio_device_priv(audio_dev);
		cs4384_pd->mclk_src = UBI32_CS4384_MCLK_PWM_S_G5_1;
		cs4384_pd->n_mclk = ARRAY_SIZE(ip8500tap_cs4384_mclk_entries);
		cs4384_pd->mclk_entries = ip8500tap_cs4384_mclk_entries;
		ip8500tap_i2c_board_info[0].platform_data = audio_dev;

		/*
		 * Reset the DAC
		 */
		if (gpio_request(GPIO_PG0_15, "DAC Reset") == 0) {
			gpio_direction_output(GPIO_PG0_15, 0);
			udelay(1);
			gpio_direction_output(GPIO_PG0_15, 1);
		} else {
			printk("Unable to request DAC reset GPIO\n");
		}
	}
#endif

	printk(KERN_INFO "%s: registering i2c resources\n", __FUNCTION__);
	i2c_register_board_info(0, ip8500tap_i2c_board_info, ARRAY_SIZE(ip8500tap_i2c_board_info));

#if defined(CONFIG_IP8500TAP_DAO)
	printk(KERN_INFO "IP8500 TAP DAO\n");
#elif defined(CONFIG_IP8500TAP_ES)
	printk(KERN_INFO "IP8500 TAP ES\n");
#else
	printk(KERN_INFO "IP8500 TAP\n");
#endif
	return 0;
}
arch_initcall(ip8500tap_init);

#ifdef CONFIG_FB_UBICOM32
/******************************************************************************
 * Late Init:
 *
 * The devices and functions below this line are for late
 * init detection of video hardware
 */

/******************************************************************************
 * Touch controller
 *
 * Connected via I2C bus, interrupt on PG4_23 (EXTINT[5], uses Peripherial Int 0)
 */
#include <linux/i2c/tsc2007.h>

/*
 * ip8500tap_tsc2007_exit_platform_hw
 */
static void ip8500tap_tsc2007_exit_platform_hw(void)
{
	UBICOM32_IO_EXTINT_CFG &= (~0x03 << 3);
	gpio_free(GPIO_PG4_23);
}

/*
 * ip8500tap_tsc2007_init_platform_hw
 */
static int ip8500tap_tsc2007_init_platform_hw(void)
{
	int res = gpio_request(GPIO_PG4_23, "TSC2007_IRQ");
	if (res) {
		return res;
	}

	UBICOM32_IO_EXTINT_CFG &= (~0x03 << 3);
	UBICOM32_IO_EXTINT_CFG |= (0x02 << 3) | 5;
	return 0; 
}

/*
 * ip8500tap_tsc2007_get_pendown_state
 */
static int ip8500tap_tsc2007_get_pendown_state(void)
{
	return !gpio_get_value(GPIO_PG4_23);
}

static struct tsc2007_platform_data ip8500tap_tsc2007_data = {
	.model			= 2007,
	.x_plate_ohms		= 350,
	.get_pendown_state	= ip8500tap_tsc2007_get_pendown_state,
	.init_platform_hw	= ip8500tap_tsc2007_init_platform_hw,
	.exit_platform_hw	= ip8500tap_tsc2007_exit_platform_hw,
};

static struct i2c_board_info __initdata ip8500tap_touch_i2c_board_info = {
	.type		= "tsc2007",
	.addr		= 0x49,
	.irq		= EXT0_INT,
	.platform_data	= &ip8500tap_tsc2007_data,
};

/******************************************************************************
 * LCD PON for LCD Adapter board
 */
static struct ubicom32lcdpower_platform_data ip8500tap_lcdpower_data = {
	.vgh_polarity           = true,
};

static struct platform_device ip8500tap_lcdpower_device = {
	.name   = "ubicom32lcdpower",
	.id     = -1,
	.dev    = {
		.platform_data = &ip8500tap_lcdpower_data,
	},
};

/******************************************************************************
 * Backlight on the board PD0, hardware PWM
 */
static struct ubicom32bl_platform_data ip8500tap_backlight_data = {
	.type                   = UBICOM32BL_TYPE_PWM,
	.pwm_channel            = 0,
	.gpio			= GPIO_PG5_0,
	.pwm_prescale           = 15,
	.pwm_period             = 60,
	.default_intensity      = 0x80,
};

static struct platform_device ip8500tap_backlight_device = {
	.name   = "ubicom32bl",
	.id     = -1,
	.dev    = {
		.platform_data = &ip8500tap_backlight_data,
	},
};

#ifdef CONFIG_IP8500TAP_AUTODETECT
/*
 * Virtual Frame Buffer device for use with LCD Adapter
 */
static struct platform_device ip8500tap_vfb_device = {
	.name	= "ubicom32vfb",
	.id	= -1,
};
#endif // CONFIG_IP8500TAP_AUTODETECT

/*
 * ip8500tap_init_offboard_video
 */
static int __init ip8500tap_init_offboard_video(struct lcd_adapter_info *info)
{
	struct i2c_adapter *adap;

	/*
	 * If we are autodetect then bring up the display now
	 */
#ifdef CONFIG_IP8500TAP_AUTODETECT
	/*
	 * Use the VDC for devices which are HV/TCON
	 */
	if (info->driver_type[0] & (LCD_ADAPTER_DTYPE0_HV | LCD_ADAPTER_DTYPE0_TCON | LCD_ADAPTER_DTYPE0_DE)) {
		struct ubicom32fb_platform_data *pd = &ip8500tap_ubicom32fb_platform_data;

		printk(KERN_INFO "HVSync mode display detected\n");

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
		platform_device_register(&ip8500tap_vfb_device);
	}
#endif // CONFIG_IP8500TAP_AUTODETECT

	/*
	 * Start up the LCD Power driver if needed (!!take into account which slot it's in!!)
	 */
	if (info->driver_type[2] & LCD_ADAPTER_DTYPE2_POWER) {
		if (info->lcd_power_pin == 38) {
			ip8500tap_lcdpower_data.vgh_gpio = GPIO_PG4_26,
			platform_device_register(&ip8500tap_lcdpower_device);
		} else {
			printk(KERN_WARNING "%s: don't know about LCD power pin %u\n", __FUNCTION__, info->lcd_power_pin);
		}
	}

	/*
	 * Start up the backlight driver if needed
	 */
	if (info->backlight_type == 0x01) {
		ip8500tap_backlight_data.pwm_prescale = 7;
		platform_device_register(&ip8500tap_backlight_device);
	} else if (info->backlight_type == 0x02) {
		platform_device_register(&ip8500tap_backlight_device);
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
		ip8500tap_touch_i2c_board_info.addr = info->touch_addr;
		i2c_new_device(adap, &ip8500tap_touch_i2c_board_info);
	}

	i2c_put_adapter(adap);

	return 0;
}

/*
 * ip8500tap_init_onboard_video
 */
static int __init ip8500tap_init_onboard_video(void)
{
	struct i2c_adapter *adap;

	/*
	 * If we are autodetect then bring up the display now
	 */
	vdc_tio_init(NULL);

	platform_device_register(&ip8500tap_backlight_device);

	adap = i2c_get_adapter(0);
	if (!adap) {
		printk(KERN_WARNING "%s: Failed to get i2c adapter\n", __FUNCTION__);
		return -ENODEV;
	}

	printk(KERN_INFO "%s: registering touch screen controller resources\n", __FUNCTION__);

	i2c_new_device(adap, &ip8500tap_touch_i2c_board_info);

	i2c_put_adapter(adap);

	return 0;
}

/*
 * ip8500tap_late_init
 *	Called late to determine what kind of video we have on this board
 */
static int __init ip8500tap_late_init(void)
{
	int ret;
	struct lcd_adapter_info info;
	
	ret = lcd_adapter_probe(0, &info);
	if (ret < 0) {
		printk(KERN_INFO "No LCD Adapter found (%d) using onboard video.\n", ret);
		return ip8500tap_init_onboard_video();
	}

	printk(KERN_INFO "LCD Adapter Board found\n");
	return ip8500tap_init_offboard_video(&info);
}
late_initcall(ip8500tap_late_init);

#endif // CONFIG_FB_UBICOM32

#if defined(CONFIG_SOUND) && defined(CONFIG_IP8500TAP_DAO)
/*
 * CS4380 DAC, address 0x48 found on daughter card 8000-1010
 */
static struct i2c_board_info __initdata ip8500tap_i2c_audio_board_info = {
	.type		= "cs4350",
	.addr		= 0x48,
};

/*
 * ip8500tap_audio_late_init
 *	late init for 8000-1010
 */
static int __init ip8500tap_audio_late_init(void) {
	struct platform_device *audio_dev;
	struct i2c_adapter *adap = i2c_get_adapter(0);

	if (!adap) {
		printk(KERN_WARNING "%s: Failed to get i2c adapter\n", __FUNCTION__);
		return -ENODEV;
	}

	audio_dev = audio_device_alloc("snd-ubi32-cs4350", "audio", "audio-i2sout", 0);
	if (audio_dev) {
		ip8500tap_i2c_audio_board_info.platform_data = audio_dev;

		/*
		 * Reset the DAC
		 */
		if (gpio_request(IP8500TAP_IO1_3, "G3P25SEL") == 0) {
			gpio_direction_output(IP8500TAP_IO1_3, 0);
		} else {
			printk("Unable to request DAC reset SELECT\n");
		}
		if (gpio_request(GPIO_PG3_25, "DAC Reset") == 0) {
			gpio_direction_output(GPIO_PG3_25, 0);
			udelay(1);
			gpio_direction_output(GPIO_PG3_25, 1);
		} else {
			printk("Unable to request DAC reset GPIO\n");
		}
		i2c_new_device(adap, &ip8500tap_i2c_audio_board_info);
	}
	i2c_put_adapter(adap);

	return 0;
}
late_initcall(ip8500tap_audio_late_init);

#endif // defined(CONFIG_SOUND) && defined(CONFIG_IP8500TAP_DAO)
