/* Source for:
 * Cypress ZY070 Prototype touchscreen driver.
 * drivers/input/touchscreen/zy070.c
 *
 * Copyright (C) 2009, 2010 Cypress Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2, and only version 2, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Cypress reserves the right to make changes without further notice
 * to the materials described herein. Cypress does not assume any
 * liability arising out of the application described herein.
 *
 * Contact Cypress Semiconductor at www.cypress.com
 *
 * History:
 *			(C) 2010 Cypress - Update for GPL distribution
 *			(C) 2009 Cypress - Assume maintenance ownership
 *			(C) 2009 Enea - Original prototype
 *
 */
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/byteorder/generic.h>
#include <linux/bitops.h>
#include <linux/interrupt.h>
#include <linux/i2c/zy070.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("ZY070 touch driver");
MODULE_AUTHOR("Cypress");

#define TS_POLL_DELAY			5 /* ms delay between samples */
#define ZY070_NUM_BLK_BYTES 8
#define ZY070_NUM_BLK_READS ZY070_NUM_BLK_BYTES * 2
//#define ZY070_DEBUG 1

struct zy070 {
	struct i2c_client *client;
	int irq;
	struct input_dev *input;
	//  struct work_struct work;

	struct delayed_work	work;

	struct timer_list timer;
	struct mutex mutex;
	char phys[32];
	struct zy070_platform_data *platform_data;
	u8 prev_touch;
};

/* 
 * Structure used to read X,Y and status
 */
struct xydata_t {
	u8 stat;
	u16 x;
	u16 y;
	u8 btn;
	u8 zoom;
	u8 gest;
};

/* ****************************************************************************
 * Prototypes for static functions
 * ************************************************************************** */
static irqreturn_t zy070_irq(int irq, void *handle);
static void zy070_xy_worker(struct work_struct *work);
static int __devinit zy070_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int __devexit zy070_remove(struct i2c_client *client);
static int zy070_resume(struct i2c_client *client);
static int zy070_suspend(struct i2c_client *client, pm_message_t message);

/* *************************************************************************
 * Static variables below
 * ************************************************************************* */
static const struct i2c_device_id zy070_id[] = {
	{ "zy070", 0 },  { }
};

MODULE_DEVICE_TABLE(i2c, zy070_id);

static struct i2c_driver zy070_driver = {
	.driver = {
		.name = "zy070",
		.owner = THIS_MODULE,
	},
	.probe = zy070_probe,
	.remove = __devexit_p(zy070_remove),
	.suspend = zy070_suspend,
	.resume = zy070_resume,
	.id_table = zy070_id,
};

/*
 * smbus protocol says that length must be less than 32
 */
static inline int zy070_xfer(struct zy070 *ts, u8 cmd, int length, struct xydata_t *data)
{
	s32 ret;
	u8 *buf;
	u16 temp;
	if(unlikely(length > 32)) {
		length = 32;
	}

	ret = i2c_smbus_read_byte_data(ts->client, cmd);
	if (ret < 0) {
		dev_err(&ts->client->dev, "i2c io error: %d\n", ret);
		return ret;
	}
	return ret;
}

static s32 zy070_read_values(struct zy070 *ts, u8 base, int length, struct xydata_t *xy_data)
{
	s32 data;
	data = zy070_xfer(ts, REG_STATUS, 1, xy_data);
	if(data > -1) {
		xy_data->stat = data;
	}
	else {
		goto read_exit;
	}

	/* read high and low x values */
	data = zy070_xfer(ts, REG_POS_XH, 1, xy_data);
	if(data > -1) {
		xy_data->x = data << 8;
	}
	else {
		goto read_exit;
	}
	data = zy070_xfer(ts, REG_POS_XL, 1, xy_data);
	if(data > -1) {
		xy_data->x |= data;
	}
	else {
		goto read_exit;
	}
  
	/* read high and low y values */
	data = zy070_xfer(ts, REG_POS_YH, 1, xy_data);
	if(data > -1) {
		xy_data->y = data << 8;
	}
	else {
		goto read_exit;
	}
	data = zy070_xfer(ts, REG_POS_YL, 1, xy_data);
	if(data > -1) {
		xy_data->y |= data;
	}
	else {
		goto read_exit;
	}

 read_exit:
	return data;
}

static s32 zy070_read_version(struct zy070 *ts, struct xydata_t *xy_data)
{
	s32 data;
	//  data = zy070_xfer(ts, REG_FW_VER);
	if(data > 0) {
		xy_data->stat = data;
	}
	return data;
}

/* ************************************************************************
 * The zy070_xy_worker function reads the XY coordinates and sends them to
 * the input layer
 * ************************************************************************ */
void zy070_xy_worker(struct work_struct *work)
{
	struct xydata_t xy_data;
	struct zy070 *ts = container_of(to_delayed_work(work), struct zy070, work);
	u16 tmp;
	s32 retval;

	retval = zy070_read_values(ts, REG_STATUS, ZY070_NUM_BLK_BYTES, &xy_data);
	if (retval < 0) {
		printk(KERN_WARNING "Failed to read coordinates x:%d y:%d\n", xy_data.x, xy_data.y);
		ts->prev_touch = 1;
		goto exit_xy_worker;
	}

	if(ts->platform_data->flags) {
		if(ts->platform_data->flags & ZY070_FLIP_Y) {
			xy_data.y = ts->platform_data->maxy - xy_data.y;
		}
		if(ts->platform_data->flags & ZY070_FLIP_X) {
			xy_data.x = ts->platform_data->maxx - xy_data.x;
		}
		if(ts->platform_data->flags & ZY070_SWAP_XY) {
			tmp = xy_data.x;
			xy_data.x = xy_data.y;
			xy_data.y = tmp;
		}
	}

	xy_data.x = be16_to_cpu(xy_data.x);
	xy_data.y = be16_to_cpu(xy_data.y);

	/* We only report data when there is exactly one finger touch to report and no
	 * Large area has been detected.
	 *
	 * TODO:
	 * 1. Add multi touch functionality
	 * 2. Add press/release events if necessary
	 */

	/* Ignoring multi-touch */
	if ( xy_data.stat == REG_STAT_ONE_FINGER && xy_data.x < ts->platform_data->maxx + 1 && xy_data.y < ts->platform_data->maxy + 1) {
#ifdef ZY070_DEBUG
		printk(KERN_ALERT "stat: %d X: %x Y: %x (x,y): (%d,%d)\n",xy_data.stat, xy_data.x,xy_data.y,xy_data.x,xy_data.y);
#endif
		ts->prev_touch = 1;
		input_report_abs(ts->input, ABS_X, xy_data.x);
		input_report_abs(ts->input, ABS_Y, xy_data.y);
		input_report_key(ts->input, BTN_TOUCH, 1);
		input_sync(ts->input);
	} else if ( (xy_data.stat != REG_STAT_ONE_FINGER) && (ts->prev_touch == 1) ) {
#ifdef ZY070_DEBUG
		printk(KERN_ALERT "detected finger up\n");
#endif
		ts->prev_touch = 0;
		input_report_key(ts->input, BTN_TOUCH, 0);
		input_sync(ts->input);
	} else {
#ifdef ZY070_DEBUG
		printk(KERN_ALERT "bad stat %d or xy data (%d,%d)\n", xy_data.stat, xy_data.x, xy_data.y);
#endif
		/* schedule if touched or wait until next touch */
		if(xy_data.stat == REG_STAT_ONE_FINGER) {
			ts->prev_touch = 1;
		} else {
			ts->prev_touch = 0;
		}
	}

 exit_xy_worker:
	if (ts->client->irq == 0) {
		/* We need to restart the timer as we do not use IRQs */
		mod_timer(&ts->timer, jiffies+ TOUCHSCREEN_TIMEOUT);
	}
#ifdef ZY070_DEBUG
	printk(KERN_ALERT "prev_touch   : %d\n", ts->prev_touch);
#endif

	if(ts->prev_touch) {
		schedule_delayed_work(&ts->work,
				      msecs_to_jiffies(TS_POLL_DELAY));
	}
	else {
		enable_irq(ts->irq);
	}

	return;
}

/* ************************************************************************
 * Timer function used as dummy interrupt driver
 * ************************************************************************ */
static void zy070_timer(unsigned long handle)
{
	struct zy070 *ts = (struct zy070 *) handle;
	schedule_work(&ts->work);
	return;
}

/* ************************************************************************
 * ISR function. This function is general, initialized in drivers init
 * function
 * ************************************************************************ */
static irqreturn_t zy070_irq(int irq, void *handle)
{
	struct zy070 *ts = (struct zy070 *) handle;
#ifdef ZY070_DEBUG
	printk (KERN_ALERT "Zy070: Got IRQ\n");
#endif
	disable_irq_nosync(ts->irq);
	schedule_delayed_work(&ts->work,
			      msecs_to_jiffies(TS_POLL_DELAY));
	return IRQ_HANDLED;

}

/* zy070_initialize: Bus independent initialization. This function takes
 * care of the following tasks:
 * 1. Creating an input device and registering the device with the
 *    input layer
 * 2. Setting up IRQs  (TBD)
 * 3. Setting up SysFS (TBD)
 * 4. Initializing ZY070
 * 5. Starting any timers/Work queues.  */

static int zy070_initialize(struct i2c_client *client, struct zy070 *ts)
{

	struct input_dev *input_device;
	int error=0;
	int retval=0;
	struct xydata_t xy_data;

	if(!ts->platform_data) {
		printk(KERN_ERR "platform data is required!\n");
		return -EINVAL;
	}

	if (!i2c_check_functionality(client->adapter,
				     //				I2C_FUNC_SMBUS_READ_WORD_DATA)) {
				     I2C_FUNC_SMBUS_READ_I2C_BLOCK)) {
		printk(KERN_ERR "adapter does not support SMBUS_READ_I2C_BLOCK");
		return -EIO;
	}

	/* Create the input device and register it. */
	input_device = input_allocate_device();
	if (!input_device) {
		error = -ENOMEM;
		goto error_free_device;
	}

	ts->input = input_device;
	input_device->name = "ZY070 Touchscreen";
	input_device->phys = ts->phys;

	input_device->id.bustype = BUS_I2C;

	input_device->dev.parent = &client->dev;

	set_bit(EV_SYN, input_device->evbit);
	set_bit(ABS_X, input_device->absbit);
	set_bit(ABS_Y, input_device->absbit);
	set_bit(BTN_TOUCH, input_device->keybit);
	set_bit(EV_KEY, input_device->evbit);
	set_bit(EV_ABS, input_device->evbit);

	input_set_abs_params(input_device, ABS_X, 0, ts->platform_data->maxx, 0, 0);
	input_set_abs_params(input_device, ABS_Y, 0, ts->platform_data->maxy, 0, 0);

	if (ts->platform_data->init_platform_hw)
		ts->platform_data->init_platform_hw();

	/* Report MAXX MAXY here */

	error = input_register_device(input_device);
	if (error) {
		printk(KERN_ERR "zy070.c: Failed to register input device\n");
		retval = error;
		goto error_free_device;
	}

	/* Prepare our worker structure prior to setting up the timer/ISR */
	INIT_DELAYED_WORK(&ts->work,zy070_xy_worker);

	/* Interrupt setup: if client->irq is zero we use the polling
	 * version instead */

	if (client->irq !=0) {
		printk(KERN_ALERT"Using IRQ %d\n", client->irq);
		error = request_irq (client->irq, zy070_irq, 0,
				     client->dev.driver->name, ts);
		if(error) {
			printk(KERN_ALERT "error: could not request irq\n");
			retval = error;
			goto error_free_irq;
		}
	}

	else {
		printk(KERN_ALERT "Setting up timer\n");
		setup_timer(&ts->timer, zy070_timer, (unsigned long) ts);
		mod_timer(&ts->timer,jiffies + TOUCHSCREEN_TIMEOUT);
	}

#if 0 //version currently not working in FW
	if(zy070_read_version(ts, &xy_data)) {
		printk(KERN_ALERT "FW Ver: %x\n", xy_data.stat);
	}
	else {
		goto error_free_irq;
	} 
#endif    

	/*    Initialize the chip - nothing needed in initial version */

	goto success;

	/* error_free_sysfs: */
	printk(KERN_ALERT "error: should remove sysfs group here\n");
	/*sysfs_remove_group(&bus->dev.kobj, attr_group);*/

 error_free_irq:
	printk(KERN_ERR "Error: Failed to register IRQ handler\n");
	free_irq(client->irq,ts);
	if(ts->platform_data->exit_platform_hw)
		ts->platform_data->exit_platform_hw();

 error_free_device:
	input_free_device(input_device);

 success:
	return retval;
}

/* Function to manage power-on resume (still to be implemented in full) */
static int zy070_resume(struct i2c_client *client)
{
	/* dev_get_drvdata + device enable */
	printk(KERN_ALERT "zy070_resume\n");
	return 0;
}

/* Function to manage power-off suspend (still to be implemented in full) */
static int zy070_suspend(struct i2c_client *client, pm_message_t message)
{
	/* Dummy function for now. We need to do more... */
	printk(KERN_ALERT "zy070_suspend\n");
	return 0;
}

/* Are you sure that your driver's ids match one that is present in the
   system at the time?  That is what is needed for the probe to be called. */
static int __devinit zy070_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct zy070 *ts;
	int error;

	printk(KERN_INFO "ZY070 Entering: %s\n",__FUNCTION__);

	ts = kzalloc (sizeof(struct zy070),GFP_KERNEL);
	if (!ts) {
		return -ENOMEM;
	}

	/* register driver_data */
	ts->client = client;
	ts->irq = client->irq;


	ts->platform_data = client->dev.platform_data;
	i2c_set_clientdata(client,ts);

	/* bus-independent initialization of zy070 below */
	error = zy070_initialize(client, ts);
	if (error) {
		i2c_del_driver(&zy070_driver);
	}

	return 0;

}

/* registered in driver struct */
static int __devexit zy070_remove(struct i2c_client *client)
{
	struct zy070 *ts;
	int err;
	printk(KERN_ALERT "Entering: %s\n",__FUNCTION__);

	/* clientdata registered on probe */
	ts = i2c_get_clientdata(client);

	/* Start cleaning up by removing any delayed work and the timer */
	if (cancel_delayed_work((struct delayed_work *)&ts->work)<0) {
		printk(KERN_ALERT "error: could not remove work from workqueue");
	}
	err = del_timer(&ts->timer);
	if (err < 0) {
		printk(KERN_ALERT "error: failed to delete timer\n");
	}

	if(ts->platform_data->exit_platform_hw)
		ts->platform_data->exit_platform_hw();

	/* add bus independent remove */
	kfree(ts);
	printk(KERN_ALERT "Leaving: %s\n",__FUNCTION__);
	return 0;
}

static int zy070_init(void)
{
	int ret;

	printk(KERN_INFO "ZY070 Driver (Built %s @ %s)\n",__DATE__,__TIME__);
	ret = i2c_add_driver(&zy070_driver);
	return ret;
}

static void zy070_exit(void)
{
	printk(KERN_ALERT "Goodbye cruel world\n");
	return i2c_del_driver(&zy070_driver);
}

module_init(zy070_init);
module_exit(zy070_exit);
