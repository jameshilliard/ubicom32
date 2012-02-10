/*
 * arch/ubicom32/mach-common/vdc_tio_tft1n5046e_hv.h
 *   Initialization for Truly TFT1N5046-E in HV mode
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

#ifndef __VDC_TIO_TFT1N5046E_HV_H__
#define __VDC_TIO_TFT1N5046E_HV_H__

#include <linux/delay.h>
#include <asm/gpio.h>

#if defined(CONFIG_VDC_LCD_TFT1N5046E_HV)
static struct vdc_tio_lcd_step tft1n5046e_sync_init[] __initdata = {
	{LCD_STEP_CMD_DATA, 0x0028, 0x0006},
	{LCD_STEP_CMD_DATA, 0x0000, 0x0001},
	{LCD_STEP_SLEEP, 0, 15},
	{LCD_STEP_CMD_DATA, 0x002B, 0x9532},
	{LCD_STEP_CMD_DATA, 0x0003, 0xAAAC},
	{LCD_STEP_CMD_DATA, 0x000C, 0x0002},
	{LCD_STEP_CMD_DATA, 0x000D, 0x000A},
	{LCD_STEP_CMD_DATA, 0x000E, 0x2C00},
	{LCD_STEP_CMD_DATA, 0x001E, 0x00AA},
	{LCD_STEP_CMD_DATA, 0x0025, 0x8000},
	{LCD_STEP_SLEEP, 0, 15},
	{LCD_STEP_CMD_DATA, 0x0001, 0x2B3F},
	{LCD_STEP_CMD_DATA, 0x0002, 0x0600},
	{LCD_STEP_CMD_DATA, 0x0010, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0011, 0x6930},
	{LCD_STEP_SLEEP, 0, 20},
	{LCD_STEP_CMD_DATA, 0x0005, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0006, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0016, 0xEF1C},
	{LCD_STEP_CMD_DATA, 0x0017, 0x0103},
	{LCD_STEP_CMD_DATA, 0x0007, 0x0233},
	{LCD_STEP_CMD_DATA, 0x000B, 0x5312},
	{LCD_STEP_CMD_DATA, 0x000F, 0x0000},
	{LCD_STEP_SLEEP, 0, 20},
	{LCD_STEP_CMD_DATA, 0x0041, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0042, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0048, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0049, 0x013F},
	{LCD_STEP_CMD_DATA, 0x0044, 0xEF00},
	{LCD_STEP_CMD_DATA, 0x0045, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0046, 0x013F},
	{LCD_STEP_CMD_DATA, 0x004A, 0x0000},
	{LCD_STEP_CMD_DATA, 0x004B, 0x0000},
	{LCD_STEP_SLEEP, 0, 20},
	{LCD_STEP_CMD_DATA, 0x0030, 0x0707},
	{LCD_STEP_CMD_DATA, 0x0031, 0x0704},
	{LCD_STEP_CMD_DATA, 0x0032, 0x0204},
	{LCD_STEP_CMD_DATA, 0x0033, 0x0201},
	{LCD_STEP_CMD_DATA, 0x0034, 0x0203},
	{LCD_STEP_CMD_DATA, 0x0035, 0x0204},
	{LCD_STEP_CMD_DATA, 0x0036, 0x0204},
	{LCD_STEP_CMD_DATA, 0x0037, 0x0502},
	{LCD_STEP_CMD_DATA, 0x003A, 0x0302},
	{LCD_STEP_CMD_DATA, 0x003B, 0x0500},
	{LCD_STEP_SLEEP, 0, 20},
	{LCD_STEP_CMD_DATA, 0x0044, 239 << 8 | 0},
	{LCD_STEP_CMD_DATA, 0x0045, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0046, 319},
	{LCD_STEP_DONE, 0, 0},
};

#define CS      GPIO_RD_1
#define SCK     GPIO_RD_2
#define SDI     GPIO_RD_4
#define RS      GPIO_RD_3

static __init void tft1n5046e_spi_write(u8_t data)
{
	int i;

	UBICOM32_GPIO_SET_PIN_LOW(CS);
	/*
	 * Clock starts High
	 */
	for (i = 0; i < 8; i++) {
		/*
		 * Clock Low
		 */
		UBICOM32_GPIO_SET_PIN_TOGGLE(SCK);

		if (data & 0x80) {
			UBICOM32_GPIO_SET_PIN_HIGH(SDI);
		} else {
			UBICOM32_GPIO_SET_PIN_LOW(SDI);
		}
		data <<= 1;
		udelay(10);

		/*
		 * Clock High
		 */
		UBICOM32_GPIO_SET_PIN_TOGGLE(SCK);
		udelay(10);
	}
	UBICOM32_GPIO_SET_PIN_HIGH(CS);
	udelay(10);
}

static __init void tft1n5046e_write_command(u16_t cmd)
{
	UBICOM32_GPIO_SET_PIN_LOW(RS);
	tft1n5046e_spi_write(cmd >> 8);
	tft1n5046e_spi_write(cmd & 0xFF);
}

static __init void tft1n5046e_write_data(u16_t data)
{
	UBICOM32_GPIO_SET_PIN_HIGH(RS);
	tft1n5046e_spi_write(data >> 8);
	tft1n5046e_spi_write(data & 0xFF);
}

static __init void tft1n5046e_spi_step(void *data)
{
	const struct vdc_tio_lcd_step *step = data;

	printk(KERN_INFO "Stepping TFT1N5046-E Sync\n");

	UBICOM32_GPIO_ENABLE(CS);
	UBICOM32_GPIO_ENABLE(SCK);
	UBICOM32_GPIO_ENABLE(SDI);
	UBICOM32_GPIO_ENABLE(RS);
	UBICOM32_GPIO_SET_PIN_HIGH(CS);
	UBICOM32_GPIO_SET_PIN_HIGH(SCK);
	UBICOM32_GPIO_SET_PIN_HIGH(SDI);
	UBICOM32_GPIO_SET_PIN_HIGH(RS);
	UBICOM32_GPIO_SET_PIN_OUTPUT(CS);
	UBICOM32_GPIO_SET_PIN_OUTPUT(SCK);
	UBICOM32_GPIO_SET_PIN_OUTPUT(SDI);
	UBICOM32_GPIO_SET_PIN_OUTPUT(RS);

	while (1) {
		switch (step->op) {
		case LCD_STEP_CMD:
			tft1n5046e_write_command(step->cmd);
			break;
		case LCD_STEP_CMD_DATA:
			tft1n5046e_write_command(step->cmd);
			/*
			 * Fall Though
			 */
		case LCD_STEP_DATA:
			tft1n5046e_write_data(step->data);
			break;
		case LCD_STEP_SLEEP:
			udelay(step->data);
			break;
		case LCD_STEP_DONE:
			goto done;
		}
		step++;
	}
done:
	UBICOM32_GPIO_DISABLE(SDI);
}

#define VDC_TIO_LCD_PLATFORM_DATA "TFT1N5046E_HV"
static struct ubicom32fb_platform_data vdc_tio_ubicom32fb_platform_data __initdata = {
	.flags		= UBICOM32FB_PLATFORM_DATA_FLAG_MODE | UBICOM32FB_PLATFORM_DATA_FLAG_TIMING | UBICOM32FB_PLATFORM_DATA_FLAG_DIM |
			  UBICOM32FB_PLATFORM_DATA_FLAG_SHIFT | UBICOM32FB_PLATFORM_DATA_FLAG_STEPS |
			  UBICOM32FB_PLATFORM_DATA_FLAG_RESOLUTION,

	.rshift		= 11,
	.gshift		= 5,
	.bshift		= 0,
	.rbits		= 5,
	.gbits		= 6,
	.bbits		= 5,

	.mode		= UBICOM32FB_MODE_SYNC,

	.hfp = 10,
	.hsync = 20,
	.hbp = 10,
	.vfp = 1,
	.vsync = 2,
	.vbp = 2,
	.fclk = 5500000,

	.disp_flags = VDCTIO_DISP_FLAG_INV_CLOCK,

	.step_fn = tft1n5046e_spi_step,
	.steps = tft1n5046e_sync_init,

	.xres = 240,
	.yres = 320,

	.width_mm = 49,
	.height_mm = 65,
};
#endif

#endif
