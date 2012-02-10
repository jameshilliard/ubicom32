/*
 * arch/ubicom32/mach-common/lcd_adapter.h
 *   LCD Adapter utility functions (8007-092x)
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

#ifndef __LCD_ADPTER_H__
#define __LCD_ADPTER_H__

#define LCD_ADAPTER_SERIAL_OFFSET	2
#define LCD_ADAPTER_SERIAL_SIZE		4

#define LCD_ADAPTER_PTYPE_OFFSET	12
enum {
	LCD_ADAPTER_PTYPE_TFT2N0369E,
	LCD_ADAPTER_PTYPE_CFAF240320KTTS,
	LCD_ADAPTER_PTYPE_CFAF240320PT,
	LCD_ADAPTER_PTYPE_CFAF240400DT,
	LCD_ADAPTER_PTYPE_CFAF320240FT,
	LCD_ADAPTER_PTYPE_AT080TN42,
	LCD_ADAPTER_PTYPE_CFAF320240FTTS,
	LCD_ADAPTER_PTYPE_LB070WV6,
	LCD_ADAPTER_PTYPE_AT07093TT,
};

#define LCD_ADAPTER_ACTIVE_W_OFFSET	19
#define LCD_ADAPTER_ACTIVE_H_OFFSET	21

#define LCD_ADAPTER_RED_BITS_OFFSET	24
#define LCD_ADAPTER_RED_SHIFT_OFFSET	25
#define LCD_ADAPTER_GREEN_BITS_OFFSET	26
#define LCD_ADAPTER_GREEN_SHIFT_OFFSET	27
#define LCD_ADAPTER_BLUE_BITS_OFFSET	28
#define LCD_ADAPTER_BLUE_SHIFT_OFFSET	29

#define LCD_ADAPTER_DTYPE_OFFSET	30
#define LCD_ADAPTER_DTYPE_SIZE		3
#define LCD_ADAPTER_DTYPE0_HV		0x01
#define LCD_ADAPTER_DTYPE0_TCON		0x02
#define LCD_ADAPTER_DTYPE0_DE		0x04
#define LCD_ADAPTER_DTYPE0_I2C		0x20
#define LCD_ADAPTER_DTYPE1_80_16	0x20
#define LCD_ADAPTER_DTYPE1_80_8		0x80
#define LCD_ADAPTER_DTYPE2_HIGH		0x01
#define LCD_ADAPTER_DTYPE2_POWER	0x02

#define LCD_ADAPTER_HFP_OFFSET		37
#define LCD_ADAPTER_HSYNC_OFFSET	39
#define LCD_ADAPTER_HBP_OFFSET		41
#define LCD_ADAPTER_VFP_OFFSET		43
#define LCD_ADAPTER_VSYNC_OFFSET	45
#define LCD_ADAPTER_VBP_OFFSET		47
#define LCD_ADAPTER_FCLK_OFFSET		66

#define LCD_ADAPTER_TOUCH_ADDR_OFFSET	53

#define LCD_ADAPTER_TOUCH_TYPE_OFFSET	52
#define LCD_ADAPTER_TOUCH_TYPE_TSC2007	0
#define LCD_ADAPTER_TOUCH_TYPE_AD7879	1

#define LCD_ADAPTER_BTYPE_OFFSET	59

#define LCD_ADAPTER_PPIN_OFFSET		65

struct lcd_adapter_info {
	u8	panel_type;
	u8	red_bits;
	u8	red_shift;
	u8	green_bits;
	u8	green_shift;
	u8	blue_bits;
	u8	blue_shift;
	u8	touch_addr;
	u8	lcd_power_pin;
	u8	backlight_type;
	u8	driver_type[LCD_ADAPTER_DTYPE_SIZE];

	u32	serial;

	u16	active_h;
	u16	active_w;

	u16	hfp;
	u16	hsync;
	u16	hbp;
	u16	vfp;
	u16	vsync;
	u16	vbp;
	u32	fclk;
};

extern int __init lcd_adapter_probe(int i2c_adapt, struct lcd_adapter_info *info);

#endif
