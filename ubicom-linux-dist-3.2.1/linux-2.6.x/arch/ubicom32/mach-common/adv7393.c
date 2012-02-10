/*
 * arch/ubicom32/mach-common/adv7393.c
 *   Common initialization routines for ADV7393
 *
 * The init function should be called during late_init.
 *
 * (C) Copyright 2009-2010, Ubicom, Inc.
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
#include <linux/i2c.h>
#include <linux/delay.h>
#include <asm/gpio.h>
#include <asm/adv7393.h>

/*
 * adv7393_write
 */
static int __init adv7393_write(int i2c_adapt, int i2c_addr, int reg, int val)
{
	struct i2c_adapter *adap;
	struct i2c_msg msg[1];
	unsigned char data[2];
	int err;

	adap = i2c_get_adapter(i2c_adapt);
	if (!adap) {
		printk(KERN_WARNING "%s: failed to get i2c adapter\n", __FUNCTION__);
		return -ENODEV;
	}
	msg->addr = i2c_addr;
	msg->flags = 0;
	msg->len = 2;
	msg->buf = data;
	data[0] = reg;
	data[1] = val;
	err = i2c_transfer(adap, msg, 1);
	i2c_put_adapter(adap);
	if (err >= 0) {
		return 0;
	}
	return err;
}

/*
 * adv7393_init
 *	Initializes the video DAC via I2C
 *
 * Equivalent mode line: 720x480p = 27 Mhz, 720 736 800 858 480 484 492 525
 */
int __init adv7393_init(int i2c_adapt, int i2c_addr, int vdac_reset_gpio)
{
	int err;

	printk(KERN_INFO "Initializing ADV7393 DAC\n");

	/*
	 * Reset the VDAC
	 */
	if (vdac_reset_gpio) {
		if (gpio_request(vdac_reset_gpio, "VDAC Reset")) {
			printk(KERN_WARNING "%s: failed to allocate VDAC Reset\n", __FUNCTION__);
			return -EBUSY;
		}
		gpio_direction_output(vdac_reset_gpio, 0);
		udelay(1);
		gpio_set_value(vdac_reset_gpio, 1);
		udelay(1);
	}

	/*
	 * See table 100 of ADV7393 data sheet: 16-bit 525p YCrCb In, YPbPr Out
	 */
	err = adv7393_write(i2c_adapt, i2c_addr, 0x17, 0x02);
	if (err) {
		printk(KERN_WARNING "%s: failed to write VDAC\n", __FUNCTION__);
		return err;
	}
	err = adv7393_write(i2c_adapt, i2c_addr, 0x00, 0x1c);
	if (err) {
		printk(KERN_WARNING "%s: failed to write VDAC\n", __FUNCTION__);
		return err;
	}
	err = adv7393_write(i2c_adapt, i2c_addr, 0x01, 0x10);
	if (err) {
		printk(KERN_WARNING "%s: failed to write VDAC\n", __FUNCTION__);
		return err;
	}
	err = adv7393_write(i2c_adapt, i2c_addr, 0x31, 0x01);
	if (err) {
		printk(KERN_WARNING "%s: failed to write VDAC\n", __FUNCTION__);
		return err;
	}
#ifdef ADV7393_SWAP_PBPR
	err = adv7393_write(i2c_adapt, i2c_addr, 0x35, 0x08);
	if (err) {
		printk(KERN_WARNING "%s: failed to write VDAC\n", __FUNCTION__);
		return err;
	}
#endif
#ifdef ADV7393_FULL_RANGE
	err = adv7393_write(i2c_adapt, i2c_addr, 0x30, 0x02);
	if (err) {
		printk(KERN_WARNING "%s: failed to write VDAC\n", __FUNCTION__);
		return err;
	}
#endif
	return 0;
}

