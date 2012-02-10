/*
 * arch/ubicom32/mach-common/lcd_adapter.c
 *   LCD Adapter utility routines (8007-092x)
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

#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <asm/lcd_adapter.h>

#include <linux/i2c.h>

/*
 * lcd_adapter_probe
 *	Must be called in/after late_init so we can use the i2c bus
 */
int __init lcd_adapter_probe(int i2c_adapt, struct lcd_adapter_info *info)
{
	struct i2c_adapter *adap;
	struct i2c_msg msg[1];
	unsigned char *data;
	unsigned char checksum;
	int err;
	int i;
	u16 tmp;

	printk(KERN_INFO "Probe for LCD Adapter (i2c_adapt=%d)\n", i2c_adapt);

	/*
	 * Check to see if there is an EEPROM out there.  If we see an
	 * EEPROM then we will assume a LCD Adapter Board (8007-092x)
	 * exists.
	 */
	data = kmalloc(256, GFP_KERNEL);
	if (!data) {
		printk(KERN_WARNING "%s: Failed to allocate memory\n", __FUNCTION__);
		return -ENOMEM;
	}

	adap = i2c_get_adapter(i2c_adapt);
	if (!adap) {
		printk(KERN_WARNING "%s: Failed to get i2c adapter\n", __FUNCTION__);
		kfree(data);
		return -ENODEV;
	}
	data[0] = 0;
	msg->addr = 0x50;
	msg->flags = 0;
	msg->len = 1;
	msg->buf = data;
	err = i2c_transfer(adap, msg, 1);
	if (err < 0) {
		return err;
	}

	msg->addr = 0x50;
	msg->flags = I2C_M_RD;
	msg->len = 256;
	msg->buf = data;
	err = i2c_transfer(adap, msg, 1);
	if (err < 0) {
		return err;
	}

	i2c_put_adapter(adap);

	/*
	 * Verify the checksum
	 */
	err = 0;
	checksum = 0xff;
	for (i = 0; i < 255; i++) {
		checksum ^= data[i];
	}
	if (checksum != data[255]) {
		printk(KERN_WARNING "%s: Checksum mismatch\n", __FUNCTION__);
		err = -EINVAL;
		goto done;
	}

	if (!info) {
		goto done;
	}

	/*
	 * Pull out the relevant data
	 */
	info->panel_type = data[LCD_ADAPTER_PTYPE_OFFSET];
	info->backlight_type = data[LCD_ADAPTER_BTYPE_OFFSET];
	info->lcd_power_pin = data[LCD_ADAPTER_PPIN_OFFSET];
	info->red_bits = data[LCD_ADAPTER_RED_BITS_OFFSET];
	info->red_shift = data[LCD_ADAPTER_RED_SHIFT_OFFSET];
	info->green_bits = data[LCD_ADAPTER_GREEN_BITS_OFFSET];
	info->green_shift = data[LCD_ADAPTER_GREEN_SHIFT_OFFSET];
	info->blue_bits = data[LCD_ADAPTER_BLUE_BITS_OFFSET];
	info->blue_shift = data[LCD_ADAPTER_BLUE_SHIFT_OFFSET];
	info->touch_addr = data[LCD_ADAPTER_TOUCH_ADDR_OFFSET];
	memcpy(&info->serial, &data[LCD_ADAPTER_SERIAL_OFFSET], LCD_ADAPTER_SERIAL_SIZE);
	memcpy(info->driver_type, &data[LCD_ADAPTER_DTYPE_OFFSET], LCD_ADAPTER_DTYPE_SIZE);
	memcpy(&info->hfp, &data[LCD_ADAPTER_HFP_OFFSET], 2);
	memcpy(&info->hsync, &data[LCD_ADAPTER_HSYNC_OFFSET], 2);
	memcpy(&info->hbp, &data[LCD_ADAPTER_HBP_OFFSET], 2);
	memcpy(&info->vfp, &data[LCD_ADAPTER_VFP_OFFSET], 2);
	memcpy(&info->vsync, &data[LCD_ADAPTER_VSYNC_OFFSET], 2);
	memcpy(&info->vbp, &data[LCD_ADAPTER_VBP_OFFSET], 2);
	memcpy(&info->active_h, &data[LCD_ADAPTER_ACTIVE_H_OFFSET], 2);
	memcpy(&info->active_w, &data[LCD_ADAPTER_ACTIVE_W_OFFSET], 2);

	memcpy(&tmp, &data[LCD_ADAPTER_FCLK_OFFSET], 2);
	info->fclk = (u32_t)tmp * 10000UL;

done:
	kfree(data);

	return err;
}

