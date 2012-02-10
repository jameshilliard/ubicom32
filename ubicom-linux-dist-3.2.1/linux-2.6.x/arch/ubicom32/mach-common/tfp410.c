/*
 * arch/ubicom32/mach-common/tfp410.c
 *   Common initialization routines for tfp410
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
#include <asm/tfp410.h>

/*
 * tfp410_write
 */
static int __init tfp410_write(int i2c_adapt, int i2c_addr, int reg, int val)
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
 * tfp410_init
 *	Initializes TFP410 via I2C
 */
int __init tfp410_init(int i2c_adapt, int i2c_addr, int reset_gpio)
{
	int err;

	printk(KERN_INFO "Initializing TFP410\n");

	/*
	 * Reset the TFP410
	 */
	if (reset_gpio) {
		if (gpio_request(reset_gpio, "TFP410 Reset")) {
			printk(KERN_WARNING "%s: failed to allocate TFP410 Reset\n", __FUNCTION__);
			return -EBUSY;
		}
		gpio_direction_output(reset_gpio, 0);
		udelay(1);
		gpio_set_value(reset_gpio, 1);
		udelay(1);
	}

	/*
	 * Turn on transmitter.
	 */
	err = tfp410_write(i2c_adapt, i2c_addr, 0x08, 0xBD);
	if (err) {
		printk(KERN_WARNING "%s: failed to write TFP410\n", __FUNCTION__);
		return err;
	}
	err = tfp410_write(i2c_adapt, i2c_addr, 0x09, 0x21);
	if (err) {
		printk(KERN_WARNING "%s: failed to write TFP410\n", __FUNCTION__);
		return err;
	}
	return 0;
}

