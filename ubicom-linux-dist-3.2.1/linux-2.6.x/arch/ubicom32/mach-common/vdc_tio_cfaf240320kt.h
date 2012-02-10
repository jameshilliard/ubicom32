/*
 * arch/ubicom32/mach-common/vdc_tio_cfaf240320kt.h
 *   Initialization for CrystalFontz CFAF240320KTTS
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

#ifndef __VDC_TIO_CFAF240320KT_H__
#define __VDC_TIO_CFAF240320KT_H__

#if defined(CONFIG_VDC_LCD_CFAF240320KT_PORTRAIT)
static struct vdc_tio_lcd_step cfaf240320kt_port_init[] __initdata = {
	{LCD_STEP_CMD_DATA, 0x0001, 0x0000,}, // Driver Output Control Register (R01h)          Page 14, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0002, 0x0700,}, // LCD Driving Waveform Control (R02h)            Page 15, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0003, 0x50A0,}, // Entry Mode (R03h)                              0 degrees
	{LCD_STEP_CMD_DATA, 0x0004, 0x0000,}, // Scaling Control register (R04h)                Page 16, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0008, 0x0207,}, // Display Control 2 (R08h)                       Page 17, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0009, 0x0000,}, // Display Control 3 (R09h)                       Page 18, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x000A, 0x0000,}, // Frame Cycle Control (R0Ah)                     Page 19, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x000C, 0x0000,}, // External Display Interface Control 1 (R0Ch)    Page 20, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x000D, 0x0000,}, // Frame Maker Position (R0Dh)                    Page 21, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x000F, 0x0000,}, // External Display Interface Control 2 (R0Fh)    Page 21, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0010, 0x0000,}, // Power Control 1 (R10h)                         Page 22, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0011, 0x0007,}, // Power Control 2 (R11h)                         Page 23, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0012, 0x0000,}, // Power Control 3 (R12h)                         Page 24, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0013, 0x0000,}, // Power Control 4 (R13h)                         Page 25, SPFD5408B Datasheet
	{LCD_STEP_SLEEP, 0, 200},
	{LCD_STEP_CMD_DATA, 0x0007, 0x0101,}, // Display Control (R07h)                         Page 16, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0010, 0x12B0,}, // Power Control 1 (R10h)                         Page 22, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0011, 0x0007,}, // Power Control 2 (R11h)                         Page 23, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0012, 0x01BB,}, // Power Control 3 (R12h)                         Page 24, SPFD5408B Datasheet
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0013, 0x1300,}, // Power Control 4 (R13h)                         Page 25, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0029, 0x0010,}, // NVM read data 2 (R29h)                         Page 30, SPFD5408B Datasheet
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0030, 0x000A,}, // Gamma Control 1                                Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0031, 0x1326,}, // Gamma Control 2                                Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0032, 0x0A29,}, // Gamma Control 3                                Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0033, 0x290A,}, // Gamma Control 4                                Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0034, 0x2613,}, // Gamma Control 5                                Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0035, 0x0A0A,}, // Gamma Control 6                                Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0036, 0x1E03,}, // Gamma Control 7                                Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0037, 0x031E,}, // Gamma Control 8                                Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0038, 0x0706,}, // Gamma Control 9                                Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0039, 0x0303,}, // Gamma Control 10                               Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003A, 0x0E04,}, // Gamma Control 11                               Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003B, 0x0E01,}, // Gamma Control 12                               Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003C, 0x010E,}, // Gamma Control 13                               Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003D, 0x040E,}, // Gamma Control 14                               Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003E, 0x0303,}, // Gamma Control 15                               Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003F, 0x0607,}, // Gamma Control 16                               Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0050, 0x0000,}, // Window Horizontal RAM Address Start (R50h)     Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0051, 0x00EF,}, // Window Horizontal RAM Address End (R51h)       Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0052, 0x0000,}, // Window Vertical RAM Address Start (R52h)       Page 33, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0053, 0x013F,}, // Window Vertical RAM Address End (R53h)         Page 33, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0060, 0x2700,}, // Driver Output Control (R60h)                   Page 33, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0061, 0x0001,}, // Driver Output Control (R61h)                   Page 35, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x006A, 0x0000,}, // Vertical Scroll Control (R6Ah)                 Page 35, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0080, 0x0000,}, // Display Position - Partial Display 1 (R80h)    Page 35, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0081, 0x0000,}, // RAM Address Start - Partial Display 1 (R81h)   Page 35, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0082, 0x0000,}, // RAM Address End - Partial Display 1 (R82h)     Page 36, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0083, 0x0000,}, // Display Position - Partial Display 2 (R83h)    Page 36, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0084, 0x0000,}, // RAM Address Start - Partial Display 2 (R84h)   Page 36, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0085, 0x0000,}, // RAM Address End - Partial Display 2 (R85h)     Page 36, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0090, 0x0010,}, // Panel Interface Control 1 (R90h)               Page 36, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0092, 0x0000,}, // Panel Interface Control 2 (R92h)               Page 37, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0093, 0x0103,}, // Panel Interface control 3 (R93h)               Page 38, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0095, 0x0210,}, // Panel Interface control 4 (R95h)               Page 38, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0097, 0x0000,}, // Panel Interface Control 5 (R97h)               Page 40, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0098, 0x0000,}, // Panel Interface Control 6 (R98h)               Page 41, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0007, 0x0173,}, // Display Control (R07h)                         Page 16, SPFD5408B Datasheet
	{LCD_STEP_DONE, 0, 0},
};
#endif

#if defined(CONFIG_VDC_LCD_CFAF240320KT_PORTRAIT_180)
static struct vdc_tio_lcd_step cfaf240320kt_port_init[] __initdata = {
	{LCD_STEP_CMD_DATA, 0x0001, 0x0000,}, // Driver Output Control Register (R01h)          Page 14, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0002, 0x0700,}, // LCD Driving Waveform Control (R02h)            Page 15, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0003, 0x5000,}, // Entry Mode (R03h)                              180 degrees
	{LCD_STEP_CMD_DATA, 0x0004, 0x0000,}, // Scaling Control register (R04h)                Page 16, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0008, 0x0207,}, // Display Control 2 (R08h)                       Page 17, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0009, 0x0000,}, // Display Control 3 (R09h)                       Page 18, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x000A, 0x0000,}, // Frame Cycle Control (R0Ah)                     Page 19, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x000C, 0x0000,}, // External Display Interface Control 1 (R0Ch)    Page 20, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x000D, 0x0000,}, // Frame Maker Position (R0Dh)                    Page 21, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x000F, 0x0000,}, // External Display Interface Control 2 (R0Fh)    Page 21, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0010, 0x0000,}, // Power Control 1 (R10h)                         Page 22, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0011, 0x0007,}, // Power Control 2 (R11h)                         Page 23, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0012, 0x0000,}, // Power Control 3 (R12h)                         Page 24, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0013, 0x0000,}, // Power Control 4 (R13h)                         Page 25, SPFD5408B Datasheet
	{LCD_STEP_SLEEP, 0, 200},
	{LCD_STEP_CMD_DATA, 0x0007, 0x0101,}, // Display Control (R07h)                         Page 16, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0010, 0x12B0,}, // Power Control 1 (R10h)                         Page 22, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0011, 0x0007,}, // Power Control 2 (R11h)                         Page 23, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0012, 0x01BB,}, // Power Control 3 (R12h)                         Page 24, SPFD5408B Datasheet
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0013, 0x1300,}, // Power Control 4 (R13h)                         Page 25, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0029, 0x0010,}, // NVM read data 2 (R29h)                         Page 30, SPFD5408B Datasheet
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0030, 0x000A,}, // Gamma Control 1                                Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0031, 0x1326,}, // Gamma Control 2                                Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0032, 0x0A29,}, // Gamma Control 3                                Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0033, 0x290A,}, // Gamma Control 4                                Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0034, 0x2613,}, // Gamma Control 5                                Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0035, 0x0A0A,}, // Gamma Control 6                                Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0036, 0x1E03,}, // Gamma Control 7                                Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0037, 0x031E,}, // Gamma Control 8                                Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0038, 0x0706,}, // Gamma Control 9                                Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0039, 0x0303,}, // Gamma Control 10                               Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003A, 0x0E04,}, // Gamma Control 11                               Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003B, 0x0E01,}, // Gamma Control 12                               Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003C, 0x010E,}, // Gamma Control 13                               Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003D, 0x040E,}, // Gamma Control 14                               Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003E, 0x0303,}, // Gamma Control 15                               Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x003F, 0x0607,}, // Gamma Control 16                               Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0050, 0x0000,}, // Window Horizontal RAM Address Start (R50h)     Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0051, 0x00EF,}, // Window Horizontal RAM Address End (R51h)       Page 32, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0052, 0x0000,}, // Window Vertical RAM Address Start (R52h)       Page 33, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0053, 0x013F,}, // Window Vertical RAM Address End (R53h)         Page 33, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0060, 0x2700,}, // Driver Output Control (R60h)                   Page 33, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0061, 0x0001,}, // Driver Output Control (R61h)                   Page 35, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x006A, 0x0000,}, // Vertical Scroll Control (R6Ah)                 Page 35, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0080, 0x0000,}, // Display Position - Partial Display 1 (R80h)    Page 35, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0081, 0x0000,}, // RAM Address Start - Partial Display 1 (R81h)   Page 35, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0082, 0x0000,}, // RAM Address End - Partial Display 1 (R82h)     Page 36, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0083, 0x0000,}, // Display Position - Partial Display 2 (R83h)    Page 36, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0084, 0x0000,}, // RAM Address Start - Partial Display 2 (R84h)   Page 36, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0085, 0x0000,}, // RAM Address End - Partial Display 2 (R85h)     Page 36, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0090, 0x0010,}, // Panel Interface Control 1 (R90h)               Page 36, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0092, 0x0000,}, // Panel Interface Control 2 (R92h)               Page 37, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0093, 0x0103,}, // Panel Interface control 3 (R93h)               Page 38, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0095, 0x0210,}, // Panel Interface control 4 (R95h)               Page 38, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0097, 0x0000,}, // Panel Interface Control 5 (R97h)               Page 40, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0098, 0x0000,}, // Panel Interface Control 6 (R98h)               Page 41, SPFD5408B Datasheet
	{LCD_STEP_CMD_DATA, 0x0007, 0x0173,}, // Display Control (R07h)                         Page 16, SPFD5408B Datasheet
	{LCD_STEP_DONE, 0, 0},
};
#endif

#if defined(CONFIG_VDC_LCD_CFAF240320KT_PORTRAIT) || defined(CONFIG_VDC_LCD_CFAF240320KT_PORTRAIT_180)
#define VDC_TIO_LCD_PLATFORM_DATA "CFAF240320KT Portrait"
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
	.horz_reg = 0x20,
	.vert_reg = 0x21,
	.gram_reg = 0x22,
	.steps = cfaf240320kt_port_init,
	.disp_flags = VDCTIO_DISP_FLAG_NO_PARTIAL | VDCTIO_DISP_FLAG_16 | VDCTIO_DISP_FLAG_RESET | VDCTIO_DISP_FLAG_CS | VDCTIO_DISP_FLAG_RD,

	.xres = 240,
	.yres = 320,

	.height_mm = 37,
	.width_mm = 49,
};
#endif

#endif
