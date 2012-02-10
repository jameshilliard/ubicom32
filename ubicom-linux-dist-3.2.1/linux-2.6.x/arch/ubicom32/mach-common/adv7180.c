/*
 * arch/ubicom32/mach-common/adv7180.c
 *   Common initialization routines for adv7180
 *
 * The init function should be called during late_init.
 *
 * This setup is for NTSC CVBS only
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
#include <linux/i2c.h>
#include <linux/delay.h>
#include <asm/gpio.h>

/*
 * adv7180_write
 */
static int __init adv7180_write(int i2c_adapt, int i2c_addr, int reg, int val)
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
 * adv7180_init
 *	Initializes adv7180 via I2C
 */
int __init adv7180_init(int i2c_adapt, int i2c_addr, int reset_gpio, int input)
{
	int err = 0;

	printk(KERN_INFO "Initializing ADV7180\n");

	/*
	 * Reset the adv7180
	 */
	if (reset_gpio) {
		if (gpio_request(reset_gpio, "ADV7180 Reset")) {
			printk(KERN_WARNING "%s: failed to allocate ADV7180 Reset\n", __FUNCTION__);
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
	err = adv7180_write(i2c_adapt, i2c_addr, 0x00, 0x10 | input);
	err = adv7180_write(i2c_adapt, i2c_addr, 0x03, 0x08);
	err = adv7180_write(i2c_adapt, i2c_addr, 0x04, 0x5d);
	err = adv7180_write(i2c_adapt, i2c_addr, 0x17, 0x41);
	err = adv7180_write(i2c_adapt, i2c_addr, 0x31, 0x12);
	err = adv7180_write(i2c_adapt, i2c_addr, 0x3d, 0xa2);
	err = adv7180_write(i2c_adapt, i2c_addr, 0x3e, 0x6a);
	err = adv7180_write(i2c_adapt, i2c_addr, 0x3f, 0xa0);
	err = adv7180_write(i2c_adapt, i2c_addr, 0x0e, 0x80);
	err = adv7180_write(i2c_adapt, i2c_addr, 0x55, 0x81);
	err = adv7180_write(i2c_adapt, i2c_addr, 0x0e, 0x00);
	err = adv7180_write(i2c_adapt, i2c_addr, 0x8f, 0x50);
	err = adv7180_write(i2c_adapt, i2c_addr, 0xe6, 0x10);
	err = adv7180_write(i2c_adapt, i2c_addr, 0x34, 0x00);
	err = adv7180_write(i2c_adapt, i2c_addr, 0x35, 240);
	err = adv7180_write(i2c_adapt, i2c_addr, 0x36, 0x00);
	if (err) {
		printk(KERN_WARNING "%s: failed to write ADV7180\n", __FUNCTION__);
		return err;
	}

	return 0;
}

