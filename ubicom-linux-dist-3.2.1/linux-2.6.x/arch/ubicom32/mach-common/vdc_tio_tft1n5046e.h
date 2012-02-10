/*
 * arch/ubicom32/mach-common/vdc_tio_tft1n5046e.h
 *   Initialization for Truly Semiconductor TFT1N5046-E
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

#ifndef __VDC_TIO_TFT1N5046E_H__
#define __VDC_TIO_TFT1N5046E_H__

/*
 * The datasheet seems incorrect when specifying BGR and RGB...
 */
#if defined(CONFIG_VDC_LCD_TFT1N5046E_LANDSCAPE)	|| \
    defined(CONFIG_VDC_LCD_TFT1N5046E_LANDSCAPE_8)	|| \
    defined(CONFIG_VDC_LCD_TFT1N5046E_LANDSCAPE_8_SPLIT)
static struct vdc_tio_lcd_step tft1n5046e_land_init[] __initdata = {
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
// RGB	{LCD_STEP_CMD_DATA, 0x0001, 0x2B3F},
// BGR	{LCD_STEP_CMD_DATA, 0x0001, 0x233F},
	{LCD_STEP_CMD_DATA, 0x0001, 0x2B3F},
	{LCD_STEP_CMD_DATA, 0x0002, 0x0600},
	{LCD_STEP_CMD_DATA, 0x0010, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0011, 0x6028},
	{LCD_STEP_SLEEP, 0, 20},
	{LCD_STEP_CMD_DATA, 0x0005, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0006, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0016, 0xEF1C},
	{LCD_STEP_CMD_DATA, 0x0017, 0x0003},
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
#endif

#if defined(CONFIG_VDC_LCD_TFT1N5046E_PORTRAIT)		|| \
    defined(CONFIG_VDC_LCD_TFT1N5046E_PORTRAIT_8)	|| \
    defined(CONFIG_VDC_LCD_TFT1N5046E_PORTRAIT_8_SPLIT)
static struct vdc_tio_lcd_step tft1n5046e_port_init[] __initdata = {
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
// RGB	{LCD_STEP_CMD_DATA, 0x0001, 0x2B3F},
// BGR	{LCD_STEP_CMD_DATA, 0x0001, 0x233F},
	{LCD_STEP_CMD_DATA, 0x0001, 0x2B3F},
	{LCD_STEP_CMD_DATA, 0x0002, 0x0600},
	{LCD_STEP_CMD_DATA, 0x0010, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0011, 0x6010},
	{LCD_STEP_SLEEP, 0, 20},
	{LCD_STEP_CMD_DATA, 0x0005, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0006, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0016, 0xEF1C},
	{LCD_STEP_CMD_DATA, 0x0017, 0x0003},
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
#endif

#ifdef CONFIG_VDC_LCD_TFT1N5046E_LANDSCAPE
#define VDC_TIO_LCD_PLATFORM_DATA "TFT1N5046E Landscape"
static struct ubicom32fb_platform_data vdc_tio_ubicom32fb_platform_data __initdata = {
	.flags		= UBICOM32FB_PLATFORM_DATA_FLAG_MODE | UBICOM32FB_PLATFORM_DATA_FLAG_BITBANG | UBICOM32FB_PLATFORM_DATA_FLAG_DIM |
			  UBICOM32FB_PLATFORM_DATA_FLAG_SHIFT | UBICOM32FB_PLATFORM_DATA_FLAG_STEPS |
			  UBICOM32FB_PLATFORM_DATA_FLAG_RESOLUTION,

	.rshift		= 0,
	.gshift		= 5,
	.bshift		= 11,
	.rbits		= 5,
	.gbits		= 6,
	.bbits		= 5,

	.mode		= UBICOM32FB_MODE_BITBANG,

#ifdef IP8000
	.data_port = IO_PORT_PG4 + 2,
	.data_mask = 0x0000FFFF,
	.rs_port = IO_PORT_PG4,
	.rs_pin = 27,
	.rd_port = IO_PORT_PG4,
	.rd_pin = 28,
	.wr_port = IO_PORT_PG4,
	.wr_pin = 26,
	.cs_port = IO_PORT_PG4,
	.cs_pin = 29,
	.reset_port = IO_PORT_PG4,
	.reset_pin = 31,
#else
	.data_port = IO_PORT_RI + 2,
	.data_mask = 0x0000FFFF,
	.rs_port = IO_PORT_RD,
	.rs_pin = 3,
	.rd_port = IO_PORT_RD,
	.rd_pin = 5,
	.wr_port = IO_PORT_RD,
	.wr_pin = 2,
	.cs_port = IO_PORT_RD,
	.cs_pin = 4,
	.reset_port = IO_PORT_RD,
	.reset_pin = 7,
#endif
	.horz_reg = 0x4e,
	.vert_reg = 0x4f,
	.gram_reg = 0x22,
	.steps = tft1n5046e_land_init,
	.xofs = 239,
	.disp_flags = VDCTIO_DISP_FLAG_ROTATED | VDCTIO_DISP_FLAG_FLIPX | VDCTIO_DISP_FLAG_16 | VDCTIO_DISP_FLAG_RESET | VDCTIO_DISP_FLAG_CS | VDCTIO_DISP_FLAG_RD,

	.xres = 320,
	.yres = 240,

	.width_mm = 65,
	.height_mm = 49,
};
#endif

#ifdef CONFIG_VDC_LCD_TFT1N5046E_PORTRAIT
#define VDC_TIO_LCD_PLATFORM_DATA "TFT1N5046E Portrait"
static struct ubicom32fb_platform_data vdc_tio_ubicom32fb_platform_data __initdata = {
	.flags		= UBICOM32FB_PLATFORM_DATA_FLAG_MODE | UBICOM32FB_PLATFORM_DATA_FLAG_BITBANG | UBICOM32FB_PLATFORM_DATA_FLAG_DIM |
			  UBICOM32FB_PLATFORM_DATA_FLAG_SHIFT | UBICOM32FB_PLATFORM_DATA_FLAG_STEPS |
			  UBICOM32FB_PLATFORM_DATA_FLAG_RESOLUTION,

	.rshift		= 11,
	.gshift		= 5,
	.bshift		= 0,
	.rbits		= 5,
	.gbits		= 6,
	.bbits		= 5,

	.mode		= UBICOM32FB_MODE_BITBANG,

	.data_port = IO_PORT_RI + 2,
	.data_mask = 0x0000FFFF,
	.rs_port = IO_PORT_RD,
	.rs_pin = 3,
	.rd_port = IO_PORT_RD,
	.rd_pin = 5,
	.wr_port = IO_PORT_RD,
	.wr_pin = 2,
	.cs_port = IO_PORT_RD,
	.cs_pin = 4,
	.reset_port = IO_PORT_RD,
	.reset_pin = 7,
	.horz_reg = 0x4e,
	.vert_reg = 0x4f,
	.gram_reg = 0x22,
	.steps = tft1n5046e_port_init,
	.disp_flags = VDCTIO_DISP_FLAG_16 | VDCTIO_DISP_FLAG_RESET | VDCTIO_DISP_FLAG_CS | VDCTIO_DISP_FLAG_RD,

	.xres = 240,
	.yres = 320,

	.width_mm = 49,
	.height_mm = 65,
};
#endif

#ifdef CONFIG_VDC_LCD_TFT1N5046E_LANDSCAPE_8
#define VDC_TIO_LCD_PLATFORM_DATA "TFT1N5046E Landscape 8-bit"
static struct ubicom32fb_platform_data vdc_tio_ubicom32fb_platform_data __initdata = {
	.flags		= UBICOM32FB_PLATFORM_DATA_FLAG_MODE | UBICOM32FB_PLATFORM_DATA_FLAG_BITBANG | UBICOM32FB_PLATFORM_DATA_FLAG_DIM |
			  UBICOM32FB_PLATFORM_DATA_FLAG_SHIFT | UBICOM32FB_PLATFORM_DATA_FLAG_STEPS |
			  UBICOM32FB_PLATFORM_DATA_FLAG_RESOLUTION,

	.rshift		= 11,
	.gshift		= 5,
	.bshift		= 0,
	.rbits		= 5,
	.gbits		= 6,
	.bbits		= 5,

	.mode		= UBICOM32FB_MODE_BITBANG,

	.data_port = IO_PORT_RI + 3,
	.data_mask = 0x000000FF,
	.rs_port = IO_PORT_RD,
	.rs_pin = 3,
	.rd_port = IO_PORT_RD,
	.rd_pin = 5,
	.wr_port = IO_PORT_RD,
	.wr_pin = 2,
	.cs_port = IO_PORT_RD,
	.cs_pin = 4,
	.reset_port = IO_PORT_RD,
	.reset_pin = 7,
	.horz_reg = 0x4e,
	.vert_reg = 0x4f,
	.gram_reg = 0x22,
	.steps = tft1n5046e_land_init,
	.xofs = 239,
	.disp_flags = VDCTIO_DISP_FLAG_ROTATED | VDCTIO_DISP_FLAG_FLIPX | VDCTIO_DISP_FLAG_8 | VDCTIO_DISP_FLAG_RESET | VDCTIO_DISP_FLAG_CS | VDCTIO_DISP_FLAG_RD,

	.xres = 320,
	.yres = 240,

	.width_mm = 65,
	.height_mm = 49,
};
#endif

#ifdef CONFIG_VDC_LCD_TFT1N5046E_LANDSCAPE_8_SPLIT
#define VDC_TIO_LCD_PLATFORM_DATA "TFT1N5046E Landscape 8-bit Split"
static struct ubicom32fb_platform_data vdc_tio_ubicom32fb_platform_data __initdata = {
	.flags		= UBICOM32FB_PLATFORM_DATA_FLAG_MODE | UBICOM32FB_PLATFORM_DATA_FLAG_BITBANG | UBICOM32FB_PLATFORM_DATA_FLAG_DIM |
			  UBICOM32FB_PLATFORM_DATA_FLAG_SHIFT | UBICOM32FB_PLATFORM_DATA_FLAG_STEPS |
			  UBICOM32FB_PLATFORM_DATA_FLAG_RESOLUTION,

	.rshift		= 11,
	.gshift		= 5,
	.bshift		= 0,
	.rbits		= 5,
	.gbits		= 6,
	.bbits		= 5,

	.mode		= UBICOM32FB_MODE_BITBANG,

	.data_port = IO_PORT_RI,
	.data_mask = 0x0F0F,
	.rs_port = IO_PORT_RI,
	.rs_pin = 5,
	.wr_port = IO_PORT_RI,
	.wr_pin = 7,
	.horz_reg = 0x4e,
	.vert_reg = 0x4f,
	.gram_reg = 0x22,
	.steps = tft1n5046e_land_init,
	.xofs = 239,
	.disp_flags = VDCTIO_DISP_FLAG_ROTATED | VDCTIO_DISP_FLAG_FLIPX | VDCTIO_DISP_FLAG_8SPLIT,

	.xres = 320,
	.yres = 240,

	.width_mm = 65,
	.height_mm = 49,
};
#endif

#ifdef CONFIG_VDC_LCD_TFT1N5046E_PORTRAIT_8
#define VDC_TIO_LCD_PLATFORM_DATA "TFT1N5046E Portrait 8-bit"
static struct ubicom32fb_platform_data vdc_tio_ubicom32fb_platform_data __initdata = {
	.flags		= UBICOM32FB_PLATFORM_DATA_FLAG_MODE | UBICOM32FB_PLATFORM_DATA_FLAG_BITBANG | UBICOM32FB_PLATFORM_DATA_FLAG_DIM |
			  UBICOM32FB_PLATFORM_DATA_FLAG_SHIFT | UBICOM32FB_PLATFORM_DATA_FLAG_STEPS |
			  UBICOM32FB_PLATFORM_DATA_FLAG_RESOLUTION,

	.rshift		= 11,
	.gshift		= 5,
	.bshift		= 0,
	.rbits		= 5,
	.gbits		= 6,
	.bbits		= 5,

	.mode		= UBICOM32FB_MODE_BITBANG,

	.data_port = IO_PORT_RI + 3,
	.data_mask = 0x000000FF,
	.rs_port = IO_PORT_RD,
	.rs_pin = 3,
	.rd_port = IO_PORT_RD,
	.rd_pin = 5,
	.wr_port = IO_PORT_RD,
	.wr_pin = 2,
	.cs_port = IO_PORT_RD,
	.cs_pin = 4,
	.reset_port = IO_PORT_RD,
	.reset_pin = 7,
	.horz_reg = 0x4e,
	.vert_reg = 0x4f,
	.gram_reg = 0x22,
	.disp_flags = VDCTIO_DISP_FLAG_8 | VDCTIO_DISP_FLAG_RESET | VDCTIO_DISP_FLAG_CS | VDCTIO_DISP_FLAG_RD,

	.steps = tft1n5046e_port_init,

	.xres = 240,
	.yres = 320,

	.width_mm = 49,
	.height_mm = 65,
};
#endif

#ifdef CONFIG_VDC_LCD_TFT1N5046E_PORTRAIT_8_SPLIT
#define VDC_TIO_LCD_PLATFORM_DATA "TFT1N5046E Portrait 8-bit Split"
static struct ubicom32fb_platform_data vdc_tio_ubicom32fb_platform_data __initdata = {
	.flags		= UBICOM32FB_PLATFORM_DATA_FLAG_MODE | UBICOM32FB_PLATFORM_DATA_FLAG_BITBANG | UBICOM32FB_PLATFORM_DATA_FLAG_DIM |
			  UBICOM32FB_PLATFORM_DATA_FLAG_SHIFT | UBICOM32FB_PLATFORM_DATA_FLAG_STEPS |
			  UBICOM32FB_PLATFORM_DATA_FLAG_RESOLUTION,

	.rshift		= 11,
	.gshift		= 5,
	.bshift		= 0,
	.rbits		= 5,
	.gbits		= 6,
	.bbits		= 5,

	.mode		= UBICOM32FB_MODE_BITBANG,

	.data_port = IO_PORT_RI,
	.data_mask = 0x0F0F,
	.rs_port = IO_PORT_RI,
	.rs_pin = 5,
	.wr_port = IO_PORT_RI,
	.wr_pin = 7,
	.horz_reg = 0x4e,
	.vert_reg = 0x4f,
	.gram_reg = 0x22,
	.disp_flags = VDCTIO_DISP_FLAG_8SPLIT;

	.steps = tft1n5046e_port_init,

	.xres = 240,
	.yres = 320,

	.width_mm = 49,
	.height_mm = 65,
};
#endif

#endif
