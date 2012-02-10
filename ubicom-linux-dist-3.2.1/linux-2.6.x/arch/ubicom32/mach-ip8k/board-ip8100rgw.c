/*
 * arch/ubicom32/mach-ip8k/board-ip8100rgw.c
 *   Support for the IP8100 RouterGateway evaluation board.
 *
 * This file supports the following boards:
 *	8008-0110 Rev 1.0	IP8K RGW Board
 *
 *	8008-0111 Rev 1.1	IP8K RGW Board
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
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/leds.h>
#include <linux/delay.h>
#include <linux/input.h>

#include <linux/spi/spi.h>
#include <asm/ubicom32-spi-gpio.h>
#include <asm/switch-dev.h>

#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
#include <linux/i2c/pca953x.h>
#include <linux/i2c/at24.h>

#include <asm/board.h>
#include <asm/machdep.h>
#include <asm/ubicom32input.h>
#include <asm/ubicom32input_i2c.h>

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
struct ubicom32fb_platform_data ip8100rgw_ubicom32fb_platform_data __initdata;
#endif // CONFIG_FB_UBICOM32

#if defined(CONFIG_UBICOM_SWITCH) && defined(CONFIG_UBICOM_SWITCH_BCM539X)
/*****************************************************************************
 * SPI bus over GPIO for Gigabit Ethernet Switch
 *	U58:
 *		MOSI	PG3_12
 *		MISO	PG3_15
 *		CLK	PG3_23
 *		CS	PG3_31
 *		RESET	PG3_28
 */
static struct ubicom32_spi_gpio_platform_data ip8100rgw_spi_gpio_data = {
	.pin_mosi       = GPIO_PG3_12,
	.pin_miso       = GPIO_PG3_15,
	.pin_clk        = GPIO_PG3_23,
	.bus_num        = 0,            // We'll call this SPI bus 0
	.num_chipselect = 1,            // only one device on this SPI bus
	.clk_default    = 1,
};

static struct platform_device ip8100rgw_spi_gpio_device = {
	.name   = "ubicom32-spi-gpio",
	.id     = 0,
	.dev    = {
		.platform_data = &ip8100rgw_spi_gpio_data,
	},
};

static struct ubicom32_spi_gpio_controller_data ip8100rgw_bcm539x_controller_data = {
	.pin_cs = GPIO_PG3_31,
};

static struct switch_core_platform_data ip8100rgw_bcm539x_platform_data = {
	.flags          = SWITCH_DEV_FLAG_HW_RESET,
	.pin_reset      = GPIO_PG3_28,
	.name           = "bcm539x",
};

static struct spi_board_info ip8100rgw_spi_board_info[] = {
	{
		.modalias               = "bcm539x-spi",
		.bus_num                = 0,
		.chip_select            = 0,
		.max_speed_hz           = 2000000,
		.platform_data          = &ip8100rgw_bcm539x_platform_data,
		.controller_data        = &ip8100rgw_bcm539x_controller_data,
		.mode                   = SPI_MODE_3,
	}
};
#endif

/*****************************************************************************
 * Use ubicom32input driver to monitor the various pushbuttons on this board.
 */
static struct ubicom32input_button ip8100rgw_ubicom32input_buttons[] = {
	{
		.type           = EV_KEY,
		.code           = KEY_ESC,
		.gpio           = GPIO_PG5_1,
		.desc           = "WPS",
		.active_low     = 1,
	},
};

static struct ubicom32input_platform_data ip8100rgw_ubicom32input_data = {
	.buttons	= ip8100rgw_ubicom32input_buttons,
	.nbuttons	= ARRAY_SIZE(ip8100rgw_ubicom32input_buttons),
};

static struct platform_device ip8100rgw_ubicom32input_device = {
	.name	= "ubicom32input",
	.id	= -1,
	.dev	= {
		.platform_data = &ip8100rgw_ubicom32input_data,
	},
};

static struct ubicom32input_i2c_button ip8100rgw_ubicom32input_i2c_u16_buttons[] = {
	{
		.type		= EV_KEY,
		.code		= KEY_BACK,
		.bit		= 4,
		.active_low	= 1,
        },
	{
		.type		= EV_KEY,
		.code		= KEY_MENU,
		.bit		= 5,
		.active_low	= 1,
        },
	{
		.type		= EV_KEY,
		.code		= KEY_HOME,
		.bit		= 6,
		.active_low	= 1,
        },
	{
		.type		= EV_KEY,
		.code		= KEY_SEARCH,
		.bit		= 7,
		.active_low	= 1,
        },
};

static struct ubicom32input_i2c_platform_data ip8100rgw_ubicom32input_i2c_u16_platform_data = {
	.buttons	= ip8100rgw_ubicom32input_i2c_u16_buttons,
	.nbuttons	= ARRAY_SIZE(ip8100rgw_ubicom32input_i2c_u16_buttons),
	.name		= "Ubicom32 Input I2C U16",
};

#ifdef CONFIG_SERIAL_UBI32_SERDES
static struct resource ip8100rgw_ubicom32_suart_resources[] = {
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

static struct platform_device ip8100rgw_ubicom32_suart_device = {
	.name		= "ubicom32suart",
	.id		= -1,
	.num_resources  = ARRAY_SIZE(ip8100rgw_ubicom32_suart_resources),
	.resource	= ip8100rgw_ubicom32_suart_resources,
};
#endif

/*
 * 24C16 EEPROM found on LCD Adapter board only
 */
static struct at24_platform_data ip8100rgw_at24_platform_data = {
	.byte_len	= 8 * 256,
	.page_size	= 256,
};

/******************************************************************************
 * Devices on the I2C bus #0
 *
 * BEWARE of changing the order of things in this array as we depend on
 * certain things to be in certain places.
 */
static struct i2c_board_info __initdata ip8100rgw_i2c_board_info[] = {
	/*
	 * U16, MAX7310 IO expander, 8 bits, address 0x19
	 *      IO0: D18       IO4: SW4
	 *      IO1: D19       IO5: SW5
	 *      IO2: D20       IO6: SW6
	 *      IO3: D21       IO7: SW7
	 * see gpio-ip8100rgw.h for more information
	 */
	{
	.type           = "ubicom32in_max7310",
	.addr           = 0x19,
	.platform_data  = &ip8100rgw_ubicom32input_i2c_u16_platform_data,
	},

	/*
	 * 24C16 EEPROM found on LCD Adapter board only
	 */
	{
		.type		= "24c16",
		.addr		= 0x50,
		.platform_data	= &ip8100rgw_at24_platform_data,
	},
};


/******************************************************************************
 * I2C bus on the board, SDA G4P16, SCL G4P17
 */
static struct i2c_gpio_platform_data ip8100rgw_i2c_data = {
	.sda_pin                = GPIO_PG4_17,
	.scl_pin                = GPIO_PG4_16,
	.sda_is_open_drain      = 0,
	.scl_is_open_drain      = 0,
	.udelay                 = 50,
};

static struct platform_device ip8100rgw_i2c_device = {
	.name   = "i2c-gpio",
	.id     = 0,
	.dev    = {
		.platform_data = &ip8100rgw_i2c_data,
	},
};

/*
 * List of all devices in our system
 */
static struct platform_device *ip8100rgw_devices[] __initdata = {
#ifdef CONFIG_SERIAL_UBI32_SERDES
	&ip8100rgw_ubicom32_suart_device,
#endif
	&ip8100rgw_ubicom32input_device,
#if defined(CONFIG_UBICOM_SWITCH) && defined(CONFIG_UBICOM_SWITCH_BCM539X)
	&ip8100rgw_spi_gpio_device,
#endif
	&ip8100rgw_i2c_device,
};


/*
 * ip8100rgw_nand_select
 */
static void ip8100rgw_nand_select(void *appdata)
{
	gpio_set_value(GPIO_PG5_2, 1);
}

/*
 * ip8100rgw_nor_select
 */
static void ip8100rgw_nor_select(void *appdata)
{
	gpio_set_value(GPIO_PG5_2, 0);
}

/*
 * LEDs
 *
 * WLAN1                IO0     
 * WLAN2                IO1
 * USB2.0               IO2
 * Status               IO3
 *
 */
static struct gpio_led ip8100rgw_gpio_leds[] = {
        {
                .name                   = "d18:green:WLAN1",
                .default_trigger        = "WLAN1",
                .gpio                   = IP8100RGW_IO0,
                .active_low             = 1,
        },
        {
                .name                   = "d19:green:WLAN2",
                .default_trigger        = "WLAN2",
                .gpio                   = IP8100RGW_IO1,
                .active_low             = 1,
        },
        {
                .name                   = "d20:green:USB",
                .default_trigger        = "USB",
                .gpio                   = IP8100RGW_IO2,
                .active_low             = 1,
        },
        {
                .name                   = "d21:green:Status",
                .default_trigger        = "Status",
                .gpio                   = IP8100RGW_IO3,
                .active_low             = 1,
        },
/*      {
                .name                   = "d57:green:WPS",
                .default_trigger        = "WPS",
                .gpio                   = GPIO_RD_4,
                .active_low             = 1,
        },
*/
};

static struct gpio_led_platform_data ip8100rgw_gpio_led_platform_data = {
        .num_leds       = 4,
        .leds           = ip8100rgw_gpio_leds,
};

static struct platform_device ip8100rgw_gpio_leds_device = {
        .name           = "leds-gpio",
        .id             = -1,
        .dev = {
                .platform_data = &ip8100rgw_gpio_led_platform_data,
        },
};


/*
 * ip8100rgw_init
 *	Called to add the devices which we have on this board
 */
static int __init ip8100rgw_init(void)
{
	int ret;
	void *tmp;

	board_init();

	ubi_gpio_init();

	/*
	 * Flash storage devices
	 *      NAND selected when PG5[2] is HIGH
	 *      NOR selected when PG5[2] is LOW
	 */
	ubicom32_flash_init();
	ret = gpio_request(GPIO_PG5_2, "FLASH SEL");
	if (ret) {
		printk(KERN_ERR "%s: Could not request FLASH_SEL\n", __FUNCTION__);
	}
	gpio_direction_output(GPIO_PG5_2, 0);

	tmp = ubicom32_flash_alloc(ip8100rgw_nor_select, NULL, NULL);
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

	tmp = ubicom32_flash_alloc(ip8100rgw_nand_select, NULL, NULL);
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
# ifndef CONFIG_DISPLAY_AUTODETECT
	/*
	 * Bring up video
	 */
	vdc_tio_fill_platform_data(&ip8100rgw_ubicom32fb_platform_data);

	printk(KERN_INFO "%s: initializing video driver\n", __FUNCTION__);

	vdc_tio_init(&ip8100rgw_ubicom32fb_platform_data);
# endif // CONFIG_DISPLAY_AUTODETECT
#endif // CONFIG_FB_UBICOM32

#if defined(CONFIG_UBICOM_SWITCH) && defined(CONFIG_UBICOM_SWITCH_BCM539X)
	/*
	 * Reserve switch SPI CS on behalf on switch driver
	 */
	if (gpio_request(ip8100rgw_bcm539x_controller_data.pin_cs, "switch-bcm539x-cs")) {
		printk(KERN_WARNING "Could not request cs of switch SPI I/F\n");
	} else {
		gpio_direction_output(ip8100rgw_bcm539x_controller_data.pin_cs, 1);

		printk(KERN_INFO "%s: registering SPI resources\n", __FUNCTION__);
		spi_register_board_info(ip8100rgw_spi_board_info, ARRAY_SIZE(ip8100rgw_spi_board_info));
	}
#endif

	printk(KERN_INFO "%s: registering device resources\n", __FUNCTION__);
	platform_add_devices(ip8100rgw_devices, ARRAY_SIZE(ip8100rgw_devices));

	printk(KERN_INFO "%s: registering i2c resources\n", __FUNCTION__);
	i2c_register_board_info(0, ip8100rgw_i2c_board_info, ARRAY_SIZE(ip8100rgw_i2c_board_info));


	printk(KERN_INFO "IP8100 RGW\n");
	return 0;
}
arch_initcall(ip8100rgw_init);

#ifdef CONFIG_FB_UBICOM32
/******************************************************************************
 * Touch controller
 *
 * Connected via I2C bus, interrupt on PG4_23 (EXTINT[5], uses Peripherial Int 0)
 */
#include <linux/i2c/tsc2007.h>

/*
 * ip8100rgw_tsc2007_exit_platform_hw
 */
static void ip8100rgw_tsc2007_exit_platform_hw(void)
{
	UBICOM32_IO_EXTINT_CFG &= (~0x03 << 3);
	gpio_free(GPIO_PG4_23);
}

/*
 * ip8100rgw_tsc2007_init_platform_hw
 */
static int ip8100rgw_tsc2007_init_platform_hw(void)
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
 * ip8100rgw_tsc2007_get_pendown_state
 */
static int ip8100rgw_tsc2007_get_pendown_state(void)
{
	return !gpio_get_value(GPIO_PG4_23);
}

static struct tsc2007_platform_data ip8100rgw_tsc2007_data = {
	.model			= 2007,
	.x_plate_ohms		= 350,
	.get_pendown_state	= ip8100rgw_tsc2007_get_pendown_state,
	.init_platform_hw	= ip8100rgw_tsc2007_init_platform_hw,
	.exit_platform_hw	= ip8100rgw_tsc2007_exit_platform_hw,
};

static struct i2c_board_info __initdata ip8100rgw_touch_i2c_board_info = {
	.type		= "tsc2007",
	.addr		= 0x49,
	.irq		= EXT0_INT,
	.platform_data	= &ip8100rgw_tsc2007_data,
};

/******************************************************************************
 * LCD PON for LCD Adapter board
 */
static struct ubicom32lcdpower_platform_data ip8100rgw_lcdpower_data = {
	.vgh_polarity           = true,
};

static struct platform_device ip8100rgw_lcdpower_device = {
	.name   = "ubicom32lcdpower",
	.id     = -1,
	.dev    = {
		.platform_data = &ip8100rgw_lcdpower_data,
	},
};

/******************************************************************************
 * Backlight on the board PD0, hardware PWM
 */
static struct ubicom32bl_platform_data ip8100rgw_backlight_data = {
	.type                   = UBICOM32BL_TYPE_PWM,
	.pwm_channel            = 0,
	.gpio			= GPIO_PG5_0,
	.pwm_prescale           = 15,
	.pwm_period             = 60,
	.default_intensity      = 0x80,
};

static struct platform_device ip8100rgw_backlight_device = {
	.name   = "ubicom32bl",
	.id     = -1,
	.dev    = {
		.platform_data = &ip8100rgw_backlight_data,
	},
};

#ifdef CONFIG_DISPLAY_AUTODETECT
/*
 * Virtual Frame Buffer device for use with LCD Adapter
 */
static struct platform_device ip8100rgw_vfb_device = {
	.name	= "ubicom32vfb",
	.id	= -1,
};
#endif // CONFIG_DISPLAY_AUTODETECT

/*
 * ip8100rgw_init_offboard_video
 */
static int __init ip8100rgw_init_offboard_video(struct lcd_adapter_info *info)
{
	struct i2c_adapter *adap;

	/*
	 * If we are autodetect then bring up the display now
	 */
#ifdef CONFIG_DISPLAY_AUTODETECT
	/*
	 * Use the VDC for devices which are HV/TCON
	 */
	if (info->driver_type[0] & (LCD_ADAPTER_DTYPE0_HV | LCD_ADAPTER_DTYPE0_TCON | LCD_ADAPTER_DTYPE0_DE)) {
		struct ubicom32fb_platform_data *pd = &ip8100rgw_ubicom32fb_platform_data;

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
		platform_device_register(&ip8100rgw_vfb_device);
	}
#endif // CONFIG_DISPLAY_AUTODETECT

	/*
	 * Start up the LCD Power driver if needed (!!take into account which slot it's in!!)
	 */
	if (info->driver_type[2] & LCD_ADAPTER_DTYPE2_POWER) {
		if (info->lcd_power_pin == 38) {
			ip8100rgw_lcdpower_data.vgh_gpio = GPIO_PG4_26,
			platform_device_register(&ip8100rgw_lcdpower_device);
		} else {
			printk(KERN_WARNING "%s: don't know about LCD power pin %u\n", __FUNCTION__, info->lcd_power_pin);
		}
	}

	/*
	 * Start up the backlight driver if needed
	 */
	if (info->backlight_type == 0x01) {
		ip8100rgw_backlight_data.pwm_prescale = 7;
		platform_device_register(&ip8100rgw_backlight_device);
	} else if (info->backlight_type == 0x02) {
		platform_device_register(&ip8100rgw_backlight_device);
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
		ip8100rgw_touch_i2c_board_info.addr = info->touch_addr;
		i2c_new_device(adap, &ip8100rgw_touch_i2c_board_info);
	}

	i2c_put_adapter(adap);

	return 0;
}

#endif // CONFIG_FB_UBICOM32

/******************************************************************************
 * Late Init:
 *
 * The devices and functions below this line are for late
 * init detection of video hardware
 */

/*
 * ip8100rgw_late_init
 *	Called late to determine what kind of video we have on this board
 */
static int __init ip8100rgw_late_init(void)
{
	platform_device_register(&ip8100rgw_gpio_leds_device);

#ifdef CONFIG_FB_UBICOM32
	{
		int ret;
		struct lcd_adapter_info info;
	
		ret = lcd_adapter_probe(0, &info);
		if (ret < 0) {
			printk(KERN_INFO "No LCD Adapter found (%d)\n", ret);
			return 0;
		}

		printk(KERN_INFO "LCD Adapter Board found\n");
		return ip8100rgw_init_offboard_video(&info);
	}
#else
	return 0;
#endif /* CONFIG_FB_UBICOM32 */
}
late_initcall(ip8100rgw_late_init);

