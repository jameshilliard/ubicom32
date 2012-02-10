/*
 * arch/ubicom32/mach-common/vdc_tio_cfaf320240ft.h
 *   Initialization for CrystalFontz CFAF320240FT
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

#ifndef __VDC_TIO_CFAF320240FT_H__
#define __VDC_TIO_CFAF320240FT_H__

#if defined(CONFIG_VDC_LCD_CFAF320240FT_LANDSCAPE)
static struct vdc_tio_lcd_step cfaf320240ft_land_init[] __initdata = {
	{LCD_STEP_CMD_DATA, 0x0028, 0x0006},    // VCOM OTP                             Page 55-56 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0000, 0x0001},    // start Oscillator                     Page 36 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0010, 0x0000},    // Sleep mode                           Page 49 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0001, 0x72EF},    // Driver Output Control                Page 36-39 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0002, 0x0600},    // LCD Driving Waveform Control         Page 40-42 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0003, 0x6A38},    // Power Control 1                      Page 43-44 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0011, 0x6870},    // Entry Mode                           Page 50-52 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0X000F, 0x0000},    // Gate Scan Position                   Page 49 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0X000B, 0x5308},    // Frame Cycle Control                  Page 45 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x000C, 0x0003},    // Power Control 2                      Page 47 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x000D, 0x000A},    // Power Control 3                      Page 48 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x000E, 0x2E00},    // Power Control 4                      Page 48 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x001E, 0x00BE},    // Power Control 5                      Page 53 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0025, 0x8000},    // Frame Frequency Control              Page 53 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0026, 0x7800},    // Analog setting                       Page 54 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x004E, 0x0000},    // Ram Address Set                      Page 58 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x004F, 0x0000},    // Ram Address Set                      Page 58 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0012, 0x08D9},    // Sleep mode                           Page 49 of SSD2119 datasheet

	// Gamma Control (R30h to R3Bh) -- Page 56 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0030, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0031, 0x0104},
	{LCD_STEP_CMD_DATA, 0x0032, 0x0100},
	{LCD_STEP_CMD_DATA, 0x0033, 0x0305},
	{LCD_STEP_CMD_DATA, 0x0034, 0x0505},
	{LCD_STEP_CMD_DATA, 0x0035, 0x0305},
	{LCD_STEP_CMD_DATA, 0x0036, 0x0707},
	{LCD_STEP_CMD_DATA, 0x0037, 0x0300},
	{LCD_STEP_CMD_DATA, 0x003A, 0x1200},
	{LCD_STEP_CMD_DATA, 0x003B, 0x0800},

	{LCD_STEP_CMD_DATA, 0x0007, 0x0033},    // Display Control                       Page 45 of SSD2119 datasheet

	{LCD_STEP_CMD_DATA, 0x0044, 0xEF00},    // Vertical RAM address position         Page 57 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0045, 0x0000},    // Horizontal RAM address position       Page 57 of SSD2119 datasheet
	{LCD_STEP_CMD_DATA, 0x0046, 0x013F},    // Horizontal RAM address position       Page 57 of SSD2119 datasheet

	{LCD_STEP_SLEEP, 0, 150},

	{LCD_STEP_DONE, 0, 0},
};
#endif

#if defined(CONFIG_VDC_LCD_CFAF320240FT_LANDSCAPE)
#define VDC_TIO_LCD_PLATFORM_DATA "CFAF320240FT Landscape"
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
	.steps = cfaf320240ft_land_init,
	.disp_flags = VDCTIO_DISP_FLAG_16 | VDCTIO_DISP_FLAG_RESET | VDCTIO_DISP_FLAG_CS | VDCTIO_DISP_FLAG_RD,

	.xres = 320,
	.yres = 240,

	.height_mm = 37,
	.width_mm = 49,
};
#endif

#endif
