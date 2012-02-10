/*
 * arch/ubicom32/mach-common/vdc_tio_cfaf240400dt.h
 *   Initialization for Crystalfontz CFAF240400DT
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

#ifndef __VDC_TIO_CFAF240400DT_H__
#define __VDC_TIO_CFAF240400DT_H__

#if defined(CONFIG_VDC_LCD_CFAF240400DT_PORTRAIT)
static const struct vdc_tio_lcd_step cfaf240400dt_port_init[] = {
	{LCD_STEP_CMD_DATA, 0x0606, 0x0000},    // Pin Control (R606h)          // Page 41 of SPFD5420A Datasheet
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0007, 0x0001},    // Display Control 1 (R007h)    // Page 16 of SPFD5420A Datasheet
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0110, 0x0001},    // Power Control 6(R110h)       // Page 30 of SPFD5420A Datasheet
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0100, 0x17B0},    // Power Control 1 (R100h)      // Page 26 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0101, 0x0147},    // Power Control 2 (R101h)      // Page 27 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0102, 0x019D},    // Power Control 3 (R102h)      // Page 28 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0103, 0x3600},    // Power Control 4 (R103h)      // Page 29 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0281, 0x0010},    // NVM read data 2 (R281h)      // Page 34 of SPFD5420A Datasheet
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0102, 0x01BD},    // Power Control 3 (R102h)      // Page 28 of SPFD5420A Datasheet
	{LCD_STEP_SLEEP, 0, 50},

	//--------------- Power control 1~6 ---------------//
	{LCD_STEP_CMD_DATA, 0x0100, 0x16B0},    // Power Control 1 (R100h)      // Page 26 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0101, 0x0147},    // Power Control 2 (R101h)      // Page 27 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0102, 0x01BD},    // Power Control 3 (R102h)      // Page 28 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0103, 0x2d00},    // Power Control 4 (R103h)      // Page 29 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0107, 0x0000},    // Power Control 5 (R107h)      // Page 30 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0110, 0x0001},    // Power Control 6(R110h)       // Page 30 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0280, 0x0000},    // NVM read data 1 (R280h)      // Page 33 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0281, 0x0006},    // NVM read data 2 (R281h)      // Page 34 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0282, 0x0000},    // NVM read data 3 (R282h)      // Page 34 of SPFD5420A Datasheet

	//------- Gamma 2.2 control (R300h to R30Fh) ------//
	{LCD_STEP_CMD_DATA, 0x0300, 0x0101},
	{LCD_STEP_CMD_DATA, 0x0301, 0x0b27},
	{LCD_STEP_CMD_DATA, 0x0302, 0x132a},
	{LCD_STEP_CMD_DATA, 0x0303, 0x2a13},
	{LCD_STEP_CMD_DATA, 0x0304, 0x270b},
	{LCD_STEP_CMD_DATA, 0x0305, 0x0101},
	{LCD_STEP_CMD_DATA, 0x0306, 0x1205},
	{LCD_STEP_CMD_DATA, 0x0307, 0x0512},
	{LCD_STEP_CMD_DATA, 0x0308, 0x0005},
	{LCD_STEP_CMD_DATA, 0x0309, 0x0003},
	{LCD_STEP_CMD_DATA, 0x030A, 0x0f04},
	{LCD_STEP_CMD_DATA, 0x030B, 0x0f00},
	{LCD_STEP_CMD_DATA, 0x030C, 0x000f},
	{LCD_STEP_CMD_DATA, 0x030D, 0x040f},
	{LCD_STEP_CMD_DATA, 0x030E, 0x0300},
	{LCD_STEP_CMD_DATA, 0x030F, 0x0500},

	{LCD_STEP_CMD_DATA, 0x0400, 0x3500},    // Base Image Number of Line (R400h)            // Page 36 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0401, 0x0001},    // Base Image Display Control (R401h)           // Page 39 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0404, 0x0000},    // Based Image Vertical Scroll Control (R404h)  // Page 40 of SPFD5420A Datasheet

	//--------------- Normal set ---------------//
	{LCD_STEP_CMD_DATA, 0x0000, 0x0000},    // ID Read Register (R000h)                     // Page 13 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0001, 0x0100},    // Driver Output Control Register (R001h)       // Page 14 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0002, 0x0100},    // LCD Driving Waveform Control (R002h)         // Page 14 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0003, 0x1030},    // Entry Mode (R003h)                           // Page 15 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0006, 0x0000},    // Display Control 1 (R007h)                    // Page 16 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0008, 0x0808},    // Display Control 2 (R008h)                    // Page 17 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0009, 0x0001},    // Display Control 3 (R009h)                    // Page 18 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x000B, 0x0010},    // Low Power Control (R00Bh)                    // Page 19 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x000C, 0x0000},    // External Display Interface Control 1 (R00Ch) // Page 19 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x000F, 0x0000},    // External Display Interface Control 2 (R00Fh) // Page 20 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0007, 0x0001},    // Display Control 1 (R007h)                    // Page 16 of SPFD5420A Datasheet

	//--------------- Panel interface control 1~6 ---------------//
	{LCD_STEP_CMD_DATA, 0x0010, 0x0012},    // Panel Interface Control 1 (R010h)            // Page 20 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0011, 0x0202},    // Panel Interface Control 2 (R011h)            // Page 21 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0012, 0x0300},    // Panel Interface control 3 (R012h)            // Page 22 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0020, 0x021E},    // Panel Interface control 4 (R020h)            // Page 22 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0021, 0x0202},    // Panel Interface Control 5 (021Rh)            // Page 24 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0022, 0x0100},    // Panel Interface Control 6 (R022h)            // Page 25 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0090, 0x8000},    // Frame Marker Control (R090h)                 // Page 25 of SPFD5420A Datasheet

	//--------------- Partial display ---------------//
	{LCD_STEP_CMD_DATA, 0x0210, 0x0000},    // Window Horizontal RAM Address Start (R210h)  // Page 35 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0211, 0x00EF},    // Window Horziontal RAM Address End (R211h)    // Page 35 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0212, 0x0000},    // Window Vertical RAM Address Start (R212h)    // Page 35 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0213, 0x018F},    // Window Vertical RAM Address End (R213h)      // Page 35 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0500, 0x0000},    // Display Position - Partial Display 1 (R500h) // Page 40 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0501, 0x0000},    // RAM Address Start - Partial Display 1 (R501h)// Page 40 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0502, 0x0000},    // RAM Address End - Partail Display 1 (R502h)  // Page 40 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0503, 0x0000},    // Display Position - Partial Display 2 (R503h) // Page 40 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0504, 0x0000},    // RAM Address Start . Partial Display 2 (R504h)// Page 41 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0505, 0x0000},    // RAM Address End . Partial Display 2 (R505h)  // Page 41 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0606, 0x0000},    // Pin Control (R606h)                          // Page 41 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x06F0, 0x0000},    // NVM Access Control (R6F0h)                   // Page 41 of SPFD5420A Datasheet
	{LCD_STEP_CMD_DATA, 0x0007, 0x0173},    // Display Control 1 (R007h)                    // Page 16 of SPFD5420A Datasheet
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0007, 0x0171},    // Display Control 1 (R007h)                    // Page 16 of SPFD5420A Datasheet
	{LCD_STEP_SLEEP, 0, 10},
	{LCD_STEP_CMD_DATA, 0x0007, 0x0173},    // Display Control 1 (R007h)                    // Page 16 of SPFD5420A Datasheet
	{LCD_STEP_DONE, 0, 0},
};
#endif

#ifdef CONFIG_VDC_LCD_CFAF240400DT_PORTRAIT
#define VDC_TIO_LCD_PLATFORM_DATA "CFAF240400DT Portrait"
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
	.horz_reg = 0x0200,
	.vert_reg = 0x0201,
	.gram_reg = 0x0202,
	.steps = cfaf240400dt_port_init,
	.disp_flags = VDCTIO_DISP_FLAG_16 | VDCTIO_DISP_FLAG_RESET | VDCTIO_DISP_FLAG_CS | VDCTIO_DISP_FLAG_RD,

	.xres = 240,
	.yres = 400,

	.width_mm = 39,
	.height_mm = 65,
};
#endif

#endif
