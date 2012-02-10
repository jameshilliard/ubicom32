/*
 * arch/ubicom32/mach-common/vdc_tio_hx8347.h
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

#ifndef __VDC_TIO_HX8347_H__
#define __VDC_TIO_HX8347_H__

/*
 * The datasheet seems incorrect when specifying BGR and RGB...
 */
#if defined(CONFIG_VDC_LCD_HX8347_LANDSCAPE)
static struct vdc_tio_lcd_step hx8347_land_init[] __initdata = {
	{LCD_STEP_CMD_DATA, 0x0046, 0x0095},
	{LCD_STEP_CMD_DATA, 0x0047, 0x0051},
	{LCD_STEP_CMD_DATA, 0x0048, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0049, 0x0036},
	{LCD_STEP_CMD_DATA, 0x004A, 0x0011},
	{LCD_STEP_CMD_DATA, 0x004B, 0x0066},
	{LCD_STEP_CMD_DATA, 0x004C, 0x0014},
	{LCD_STEP_CMD_DATA, 0x004D, 0x0077},
	{LCD_STEP_CMD_DATA, 0x0050, 0x0046},
	{LCD_STEP_CMD_DATA, 0x0051, 0x0046},

	/*
	 * 320x240 window setting
	 */
	{LCD_STEP_CMD_DATA, 0x0002, 0x0000}, // Column address start2
	{LCD_STEP_CMD_DATA, 0x0003, 0x0000}, // Column address start1
	{LCD_STEP_CMD_DATA, 0x0004, 0x0001}, // Column address end2
	{LCD_STEP_CMD_DATA, 0x0005, 0x003F}, // Column address end1
	{LCD_STEP_CMD_DATA, 0x0006, 0x0000}, // Row address start2
	{LCD_STEP_CMD_DATA, 0x0007, 0x0000}, // Row address start1
	{LCD_STEP_CMD_DATA, 0x0008, 0x0000}, // Row address end2
	{LCD_STEP_CMD_DATA, 0x0009, 0x00EF}, // Row address end1
	{LCD_STEP_CMD_DATA, 0x0016, 0x00A8}, // Rotate 90 degress

	/*
	 * Display Setting
	 */
	{LCD_STEP_CMD_DATA, 0x0001, 0x0006}, // IDMON=0, INVON=1, NORON=1, PTLONLCD_STEP_CMD_DATA, =0
	{LCD_STEP_CMD_DATA, 0x0023, 0x0095}, // N_DC=1001 0101
	{LCD_STEP_CMD_DATA, 0x0024, 0x0095}, // PI_DC=1001 0101
	{LCD_STEP_CMD_DATA, 0x0025, 0x00FF}, // I_DC=1111 1111
	{LCD_STEP_CMD_DATA, 0x0027, 0x0002}, // N_BP=0000 0010
	{LCD_STEP_CMD_DATA, 0x0028, 0x0002}, // N_FP=0000 0010
	{LCD_STEP_CMD_DATA, 0x0029, 0x0002}, // PI_BP=0000 0010
	{LCD_STEP_CMD_DATA, 0x002A, 0x0002}, // PI_FP=0000 0010
	{LCD_STEP_CMD_DATA, 0x002C, 0x0002}, // I_BP=0000 0010
	{LCD_STEP_CMD_DATA, 0x002D, 0x0002}, // I_FP=0000 0010
	{LCD_STEP_CMD_DATA, 0x003A, 0x0001}, // N_RTN=0000, N_NW=001
	{LCD_STEP_CMD_DATA, 0x003B, 0x0001}, // PI_RTN=0000, PI_NW=001
	{LCD_STEP_CMD_DATA, 0x003C, 0x00F0}, // I_RTN=1111, I_NW=000
	{LCD_STEP_CMD_DATA, 0x003D, 0x0000}, // DIV=00
	{LCD_STEP_SLEEP, 0, 20},
	{LCD_STEP_CMD_DATA, 0x0035, 0x0038}, // EQS=38h
	{LCD_STEP_CMD_DATA, 0x0036, 0x0078}, // EQP=78h
	{LCD_STEP_CMD_DATA, 0x003E, 0x0038}, // SON=38h
	{LCD_STEP_CMD_DATA, 0x0040, 0x000F}, // GDON=0Fh
	{LCD_STEP_CMD_DATA, 0x0041, 0x00F0}, // GDOFF

	/*
	 * Power Supply Setting
	 */
	{LCD_STEP_CMD_DATA, 0x0019, 0x007F},
	{LCD_STEP_CMD_DATA, 0x0093, 0x000C}, // RADJ=1100, //HX8347
	{LCD_STEP_SLEEP, 0, 10},
	{LCD_STEP_CMD_DATA, 0x0020, 0x0040}, // BT=0100
	{LCD_STEP_CMD_DATA, 0x0021, 0x0000}, //add  by  solo
	{LCD_STEP_CMD_DATA, 0x001D, 0x0007}, // VC1=111
	{LCD_STEP_CMD_DATA, 0x001E, 0x0000}, // VC3=000
	{LCD_STEP_CMD_DATA, 0x001F, 0x0004}, // VRH=0100  4.12V
	{LCD_STEP_CMD_DATA, 0x0044, 0x0045}, // VCM=101 0000   3.21V
	{LCD_STEP_CMD_DATA, 0x0045, 0x0011}, // VDV=1 0001   -1.19V
	{LCD_STEP_SLEEP, 0, 10},
	{LCD_STEP_CMD_DATA, 0x001C, 0x0004}, // AP=100
	{LCD_STEP_SLEEP, 0, 10},
	{LCD_STEP_CMD_DATA, 0x0043, 0x0080}, //set VCOMG=1
	{LCD_STEP_SLEEP, 0, 5},
	{LCD_STEP_CMD_DATA, 0x001B, 0x0018}, // GASENB=0, PON=0, DK=1, XDK=0, VLCD_TRI=0, STB=0
	{LCD_STEP_SLEEP, 0, 40},
	{LCD_STEP_CMD_DATA, 0x001B, 0x0014}, // GASENB=0, PON=1, DK=0, XDK=0, VLCD_TRI=0, STB=0
	{LCD_STEP_SLEEP, 0, 40},

	/*
	 * Turn on Display
	 */
	{LCD_STEP_CMD_DATA, 0x0019, 0x007F},//{0x0019, 0x0049},// OSCADJ=10 0000, OSD_EN=1 //60Hz   //add  by  solo
	{LCD_STEP_SLEEP, 0, 20},
	{LCD_STEP_CMD_DATA, 0x001B, 0x0014}, // GASENB=0, PON=1, DK=0, XDK=0, VLCD_TRI=0, STB=0
	{LCD_STEP_SLEEP, 0, 40},
	{LCD_STEP_CMD_DATA, 0x0090, 0x007F}, //SAP=0111 1111
	{LCD_STEP_CMD_DATA, 0x0026, 0x0004}, //GON=0, DTE=0, D=01
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0026, 0x0024}, //GON=1, DTE=0, D=01
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0026, 0x002C}, //GON=1, DTE=0, D=11
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0026, 0x003C}, //GON=1, DTE=1, D=11
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_DONE, 0, 0},
};
#endif

#if defined(CONFIG_VDC_LCD_HX8347_PORTRAIT)
static struct vdc_tio_lcd_step hx8347_port_init[] __initdata = {
	{LCD_STEP_CMD_DATA, 0x0046, 0x0095},
	{LCD_STEP_CMD_DATA, 0x0047, 0x0051},
	{LCD_STEP_CMD_DATA, 0x0048, 0x0000},
	{LCD_STEP_CMD_DATA, 0x0049, 0x0036},
	{LCD_STEP_CMD_DATA, 0x004A, 0x0011},
	{LCD_STEP_CMD_DATA, 0x004B, 0x0066},
	{LCD_STEP_CMD_DATA, 0x004C, 0x0014},
	{LCD_STEP_CMD_DATA, 0x004D, 0x0077},
	{LCD_STEP_CMD_DATA, 0x004E, 0x0013},
	{LCD_STEP_CMD_DATA, 0x004F, 0x004C},
	{LCD_STEP_CMD_DATA, 0x0050, 0x0046},
	{LCD_STEP_CMD_DATA, 0x0051, 0x0046},

	/*
	 * 240x320 window setting
	 */
	{LCD_STEP_CMD_DATA, 0x0002, 0x0000}, // Column address start2
	{LCD_STEP_CMD_DATA, 0x0003, 0x0000}, // Column address start1
	{LCD_STEP_CMD_DATA, 0x0004, 0x0000}, // Column address end2
	{LCD_STEP_CMD_DATA, 0x0005, 0x00EF}, // Column address end1
	{LCD_STEP_CMD_DATA, 0x0006, 0x0000}, // Row address start2
	{LCD_STEP_CMD_DATA, 0x0007, 0x0000}, // Row address start1
	{LCD_STEP_CMD_DATA, 0x0008, 0x0001}, // Row address end2
	{LCD_STEP_CMD_DATA, 0x0009, 0x003F}, // Row address end1

	/*
	 * Display Setting
	 */
	{LCD_STEP_CMD_DATA, 0x0001, 0x0006}, // IDMON=0, INVON=1, NORON=1, PTLONLCD_STEP_CMD_DATA, =0
	{LCD_STEP_CMD_DATA, 0x0016, 0x00c8}, // MY=0, MX=0, MV=0, ML=1, BGR=0, TEON=LCD_STEP_CMD_DATA, 0
	{LCD_STEP_CMD_DATA, 0x0023, 0x0095}, // N_DC=1001 0101
	{LCD_STEP_CMD_DATA, 0x0024, 0x0095}, // PI_DC=1001 0101
	{LCD_STEP_CMD_DATA, 0x0025, 0x00FF}, // I_DC=1111 1111
	{LCD_STEP_CMD_DATA, 0x0027, 0x0002}, // N_BP=0000 0010
	{LCD_STEP_CMD_DATA, 0x0028, 0x0002}, // N_FP=0000 0010
	{LCD_STEP_CMD_DATA, 0x0029, 0x0002}, // PI_BP=0000 0010
	{LCD_STEP_CMD_DATA, 0x002A, 0x0002}, // PI_FP=0000 0010
	{LCD_STEP_CMD_DATA, 0x002C, 0x0002}, // I_BP=0000 0010
	{LCD_STEP_CMD_DATA, 0x002D, 0x0002}, // I_FP=0000 0010
	{LCD_STEP_CMD_DATA, 0x003A, 0x0001}, // N_RTN=0000, N_NW=001
	{LCD_STEP_CMD_DATA, 0x003B, 0x0001}, // PI_RTN=0000, PI_NW=001
	{LCD_STEP_CMD_DATA, 0x003C, 0x00F0}, // I_RTN=1111, I_NW=000
	{LCD_STEP_CMD_DATA, 0x003D, 0x0000}, // DIV=00
	{LCD_STEP_SLEEP, 0, 20},
	{LCD_STEP_CMD_DATA, 0x0035, 0x0038}, // EQS=38h
	{LCD_STEP_CMD_DATA, 0x0036, 0x0078}, // EQP=78h
	{LCD_STEP_CMD_DATA, 0x003E, 0x0038}, // SON=38h
	{LCD_STEP_CMD_DATA, 0x0040, 0x000F}, // GDON=0Fh
	{LCD_STEP_CMD_DATA, 0x0041, 0x00F0}, // GDOFF
	
	/*
	 * Power Supply Setting
	 */
	{LCD_STEP_CMD_DATA, 0x0019, 0x007F},
	{LCD_STEP_CMD_DATA, 0x0093, 0x000C}, // RADJ=1100, //HX8347
	{LCD_STEP_SLEEP, 0, 10},
	{LCD_STEP_CMD_DATA, 0x0020, 0x0040}, // BT=0100
	{LCD_STEP_CMD_DATA, 0x0021, 0x0000}, //add  by  solo
	{LCD_STEP_CMD_DATA, 0x001D, 0x0007}, // VC1=111
	{LCD_STEP_CMD_DATA, 0x001E, 0x0000}, // VC3=000
	{LCD_STEP_CMD_DATA, 0x001F, 0x0004}, // VRH=0100  4.12V
	{LCD_STEP_CMD_DATA, 0x0044, 0x0045}, // VCM=101 0000   3.21V
	{LCD_STEP_CMD_DATA, 0x0045, 0x0011}, // VDV=1 0001   -1.19V
	{LCD_STEP_SLEEP, 0, 10},
	{LCD_STEP_CMD_DATA, 0x001C, 0x0004}, // AP=100
	{LCD_STEP_SLEEP, 0, 10},
	{LCD_STEP_CMD_DATA, 0x0043, 0x0080}, //set VCOMG=1
	{LCD_STEP_SLEEP, 0, 5},
	{LCD_STEP_CMD_DATA, 0x001B, 0x0018}, // GASENB=0, PON=0, DK=1, XDK=0, VLCD_TRI=0, STB=0
	{LCD_STEP_SLEEP, 0, 40},
	{LCD_STEP_CMD_DATA, 0x001B, 0x0014}, // GASENB=0, PON=1, DK=0, XDK=0, VLCD_TRI=0, STB=0
	{LCD_STEP_SLEEP, 0, 40},

	/*
	 * Turn on Display
	 */
	{LCD_STEP_CMD_DATA, 0x0019, 0x007F}, //{0x0019, 0x0049},// OSCADJ=10 0000, OSD_EN=1 //60Hz   //add  by  solo
	{LCD_STEP_SLEEP, 0, 20},
	{LCD_STEP_CMD_DATA, 0x001B, 0x0014}, // GASENB=0, PON=1, DK=0, XDK=0, VLCD_TRI=0, STB=0
	{LCD_STEP_SLEEP, 0, 40},
	{LCD_STEP_CMD_DATA, 0x0090, 0x007F}, //SAP=0111 1111
	{LCD_STEP_CMD_DATA, 0x0026, 0x0004}, //GON=0, DTE=0, D=01
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0026, 0x0024}, //GON=1, DTE=0, D=01
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0026, 0x002C}, //GON=1, DTE=0, D=11
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_CMD_DATA, 0x0026, 0x003C}, //GON=1, DTE=1, D=11
	{LCD_STEP_SLEEP, 0, 50},
	{LCD_STEP_DONE, 0, 0},
};
#endif

#ifdef CONFIG_VDC_LCD_HX8347_LANDSCAPE
#define VDC_TIO_LCD_PLATFORM_DATA "HX8347 Landscape"
static struct ubicom32fb_platform_data vdc_tio_ubicom32fb_platform_data __initdata = {
	.flags		= UBICOM32FB_PLATFORM_DATA_FLAG_MODE | UBICOM32FB_PLATFORM_DATA_FLAG_BITBANG | UBICOM32FB_PLATFORM_DATA_FLAG_STEPS | UBICOM32FB_PLATFORM_DATA_FLAG_RESOLUTION,
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
	.gram_reg = 0x22,
	.steps = hx8347_land_init,
	.disp_flags = VDCTIO_DISP_FLAG_16 | VDCTIO_DISP_FLAG_RESET | VDCTIO_DISP_FLAG_CS | VDCTIO_DISP_FLAG_RD | VDCTIO_DISP_FLAG_NO_GOTO,
	.xres = 320,
	.yres = 240,
	.width_mm = 65,
	.height_mm = 49,
};
#endif

#ifdef CONFIG_VDC_LCD_HX8347_PORTRAIT
#define VDC_TIO_LCD_PLATFORM_DATA "HX8347 Portrait"
static struct ubicom32fb_platform_data vdc_tio_ubicom32fb_platform_data __initdata = {
	.flags		= UBICOM32FB_PLATFORM_DATA_FLAG_MODE | UBICOM32FB_PLATFORM_DATA_FLAG_BITBANG | UBICOM32FB_PLATFORM_DATA_FLAG_DIM |
			  UBICOM32FB_PLATFORM_DATA_FLAG_SHIFT | UBICOM32FB_PLATFORM_DATA_FLAG_STEPS |
			  UBICOM32FB_PLATFORM_DATA_FLAG_RESOLUTION,

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
	.gram_reg = 0x22,
	.steps = hx8347_port_init,
	.disp_flags = VDCTIO_DISP_FLAG_16 | VDCTIO_DISP_FLAG_RESET | VDCTIO_DISP_FLAG_CS | VDCTIO_DISP_FLAG_RD | VDCTIO_DISP_FLAG_NO_GOTO,

	.xres = 240,
	.yres = 320,

	.width_mm = 49,
	.height_mm = 65,
};
#endif

#endif
