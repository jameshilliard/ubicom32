/*
 *  Ubicom32 Jogdial interface driver
 *
 *  This driver works with a gray code rotary controller.  If interrupts are
 *  used, the inputs should be connected as follows:
 *	INT:	A XOR B
 *	GPIO:	B
 *  If polling mode is used:
 *	GPIO: A
 *	GPIO: B
 *
 *  Copyright (C) 2009-2010 Ubicom, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include <linux/init.h>
#include <linux/input.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/list.h>
#include <linux/gpio.h>
#include <asm/ubicom32jog.h>

//#define UBICOM32_JOG_DEBUG
#ifdef UBICOM32_JOG_DEBUG
#define DEBUG(x...) printk(x)
#else
#define DEBUG(...)
#endif

struct ubicom32jog_dev {
	struct input_dev *idev;
	int refcount;
	int button_type;
	int button_code;
	int button_state;
	int button_poll_interval;
	int rotary_irq;
	int rotary_state;
	int rotary_count;
	int rotary_left_count;
	int rotary_max;
	int rotary_min;
	int rotary_abs_code;
	int rotary_rel_code;
	struct tasklet_struct rotary_task;
	struct timer_list button_timer;
	struct ubicom32jog_platform_data *pdata;
};

/*
 * ubicom32jog_rotary_gray_tasklet
 *
 * Expects a code [AB]:  10-00-11-01-10-00-11-01...
 * where A = a XOR b (int)
 * where B = b
 */
static void ubicom32jog_rotary_gray_tasklet(unsigned long appdata)
{
	struct ubicom32jog_dev *ujd = (struct ubicom32jog_dev *)appdata;
	int a;
	int b;
	int i;
	int bounce_count;
	int read;
	int skip = 0;
	int left = 0;

	/*
	 * Get initial reading
	 */
	a = gpio_get_value(ujd->pdata->jog_gpio_a);
	b = gpio_get_value(ujd->pdata->jog_gpio_b);

	if(!ujd->rotary_irq) {
		a = a ^ b;
	}

	/*
	 * Debounce
	 */
	bounce_count = 0;
	for (i = 0; i < 50; i++) {
		int na = gpio_get_value(ujd->pdata->jog_gpio_a);
		int nb = gpio_get_value(ujd->pdata->jog_gpio_b);

		if(!ujd->rotary_irq) {
			na = na ^ nb;
		}

		if ((na != a) || (nb != b)) {
			bounce_count = 0;
			a = na;
			b = nb;
		} else {
			bounce_count++;
		}
		if (bounce_count == 10) {
			break;
		}
		udelay(100);
	}

	/*
	 * Too bouncy to do anything
	 */
	if (bounce_count < 10) {
		DEBUG("Too bouncy\n");
		goto done;
	}

	read = (a << 1) | b;
	switch (ujd->rotary_state) {
	case 0:
		if (read == 3) {
			// right
			break;
		}
		if (read == 2) {
			left = 1;
			break;
		}
		skip = 1;
		break;
	case 1:
		if (read == 2) {
			//right
			break;
		}
		if (read == 3) {
			left = 1;
			break;
		}
		skip = 1;
		break;
	case 2:
		if (read == 0) {
			// right
			break;
		}
		if (read == 1) {
			left = 1;
			break;
		}
		skip = 1;
		break;
	case 3:
		if (read == 1) {
			// right
			break;
		}
		if (read == 0) {
			left = 1;
			break;
		}
		skip = 1;
		break;
	}

	ujd->rotary_state = read;

	if (skip) {
		DEBUG("SKIP\n");
		goto done;
	}

	if (ujd->pdata->jog_polarity) {
		left = !left;
	}

	if (left) {
		ujd->rotary_left_count++;
		if (ujd->rotary_left_count > ujd->pdata->jog_debounce) {
			ujd->rotary_left_count = ujd->pdata->jog_debounce;
			if (ujd->rotary_count != ujd->rotary_min) {
				ujd->rotary_count--;
			}
			input_report_rel(ujd->idev, ujd->rotary_rel_code, -1);
			input_report_abs(ujd->idev, ujd->rotary_abs_code, ujd->rotary_count);
		}
	} else {
		ujd->rotary_left_count--;
		if (ujd->rotary_left_count < -ujd->pdata->jog_debounce) {
			ujd->rotary_left_count = -ujd->pdata->jog_debounce;
			if (ujd->rotary_count != ujd->rotary_max) {
				ujd->rotary_count++;
			}
			input_report_rel(ujd->idev, ujd->rotary_rel_code, 1);
			input_report_abs(ujd->idev, ujd->rotary_abs_code, ujd->rotary_count);
		}
	}

done:
	DEBUG("a:%u b:%u read:%u ns:%u left:%d:%d count:%d %c\n", a, b, read, ujd->rotary_state, left, ujd->rotary_left_count, ujd->rotary_count, skip ? '*' : ' ');

	if(ujd->rotary_irq) {
		enable_irq(ujd->rotary_irq);
	}
}

/*
 * ubicom32jog_rotary_interrupt
 */
static irqreturn_t ubicom32jog_rotary_interrupt(int irq, void *appdata)
{
	struct ubicom32jog_dev *ujd = (struct ubicom32jog_dev *)appdata;
	disable_irq(ujd->rotary_irq);
	tasklet_schedule(&ujd->rotary_task);
	return IRQ_HANDLED;
}

/*
 * ubicom32jog_button_poll
 */
static void ubicom32jog_button_poll(unsigned long appdata)
{
	struct ubicom32jog_dev *ujd = (struct ubicom32jog_dev *)appdata;
	int state = 0;
	int val = 0; 

	/*
	 * Poll rotary gpio
	 */
	if(!ujd->rotary_irq) {
		ubicom32jog_rotary_gray_tasklet((unsigned long)ujd);
	}

	/*
	 * Check button
	 */
	val = gpio_get_value(ujd->pdata->button_gpio);

	/*
	 * State changed?
	 */
	if (val == ujd->button_state) {
		goto done;
	}

	/*
	 * check for up or down
	 */
	ujd->button_state = val;
	if ((!val && ujd->pdata->button_active_low) || (val && !ujd->pdata->button_active_low)) {
		state = 1;
	}

	input_event(ujd->idev, ujd->button_type, ujd->button_code, state);
	input_sync(ujd->idev);

done:
	/*
	 * Continue polling
	 */
	ujd->button_timer.expires = jiffies + ujd->button_poll_interval;
	add_timer(&ujd->button_timer);
}

/*
 * ubicom32jog_open
 */
static int ubicom32jog_open(struct input_dev *dev)
{
	struct ubicom32jog_dev *ujd = input_get_drvdata(dev);

	ujd->refcount++;
	if (ujd->refcount > 1) {
		return 0;
	}

	/*
	 * Start polling
	 */
	ujd->button_timer.expires = jiffies + ujd->button_poll_interval;
	add_timer(&ujd->button_timer);

	return 0;
}

/*
 * ubicom32jog_close
 */
static void ubicom32jog_close(struct input_dev *dev)
{
	struct ubicom32jog_dev *ujd = input_get_drvdata(dev);

	ujd->refcount--;
	if (ujd->refcount) {
		return;
	}

	/*
	 * Stop polling
	 */
	del_timer_sync(&ujd->button_timer);
}

/*
 * ubicom32jog_probe
 */
static int __devinit ubicom32jog_probe(struct platform_device *pdev)
{
	struct ubicom32jog_dev *ujd;
	struct ubicom32jog_platform_data *pdata;
	struct input_dev *idev;
	int res;
	int a;
	int b;
	int use_irq;

	pdata = pdev->dev.platform_data;
	if (!pdata) {
		return -EINVAL;
	}

	ujd = kzalloc(sizeof(struct ubicom32jog_dev), GFP_KERNEL);
	if (!ujd) {
		return -ENOMEM;
	}
	ujd->pdata = pdata;

	/*
	 * Allocate the input device
	 */
	idev = input_allocate_device();
	if (!idev) {
		res = -ENOMEM;
		goto fail_idev;
	}

	platform_set_drvdata(pdev, ujd);
	input_set_drvdata(idev, ujd);
	ujd->idev = idev;

	/*
	 * Get the jog gpio
	 */
	res = gpio_request(pdata->jog_gpio_a, "ubicom32jog-a");
	if (res < 0) {
		printk(KERN_ERR "ubicom32jog: Could not alloc GPIO %d\n", pdata->jog_gpio_a);
		goto fail_gpio_1;
	}

	res = gpio_request(pdata->jog_gpio_b, "ubicom32jog-b");
	if (res < 0) {
		printk(KERN_ERR "ubicom32jog: Could not alloc GPIO %d\n", pdata->jog_gpio_b);
		goto fail_gpio_2;
	}

	gpio_direction_input(pdata->jog_gpio_a);
	gpio_direction_input(pdata->jog_gpio_b);

	a = gpio_get_value(pdata->jog_gpio_a);
	b = gpio_get_value(pdata->jog_gpio_b);

	if (a && b) {
		ujd->rotary_state = 0;
	} else if (a && !b) {
		ujd->rotary_state = 1;
	} else if (!a && b) {
		ujd->rotary_state = 2;
	} else {
		ujd->rotary_state = 3;
	}

	/*
	 * Determine whether to use interrupt or poll
	 */
	use_irq = 0;
#if defined(IP7000) || defined(IP7000_REV2)

	switch (pdata->jog_gpio_a) {
		case GPIO_RA_4:
			*((uint32_t *)(IO_PORT_RA + IO_CTL0)) |= (3 << 15);
			use_irq = 1;
			break;
		case GPIO_RA_5:
			*((uint32_t *)(IO_PORT_RA + IO_CTL0)) |= (3 << 17);
			use_irq = 1;
			break;
		case GPIO_RA_6:
			*((uint32_t *)(IO_PORT_RA + IO_CTL0)) |= (3 << 19);
			use_irq = 1;
			break;
	}
#endif

	/*
	 * Set flag for interrupt or poll
	 */
	if(use_irq) {
		if (request_irq(ujd->rotary_irq, ubicom32jog_rotary_interrupt, IRQF_DISABLED, "ubicom32jog", (void *)ujd)) {
			printk(KERN_ERR "ubicom32jog: Could not alloc irq %d\n", ujd->rotary_irq);
			if (pdata->has_button) {
				gpio_free(pdata->button_gpio);
			}
			goto fail_irq;
		}
		res = gpio_to_irq(pdata->jog_gpio_a);
		if (res < 0) {
			printk(KERN_ERR "ubicom32jog: jog_a is not an IRQ capable pin\n");
			goto fail_irq;
		}
		ujd->rotary_irq = res;
		tasklet_init(&ujd->rotary_task, ubicom32jog_rotary_gray_tasklet, (unsigned long)ujd);
	}

	/*
	 * Set gpio polling parameters if needed for rotary encoder or button
	 */
	if (pdata->has_button || !use_irq) {
		idev->open = ubicom32jog_open;
		idev->close = ubicom32jog_close;

		/*
		 * Default polling of 20 times a second
		 */
		if (!pdata->button_poll_interval) {
			ujd->button_poll_interval = HZ / 20;
		} else {
			ujd->button_poll_interval = HZ / pdata->button_poll_interval;
		}

		ujd->button_timer.function = ubicom32jog_button_poll;
		ujd->button_timer.data = (unsigned long)ujd;

		if (pdata->has_button) {
			res = gpio_request(pdata->button_gpio, "ubicom32jog-button");
			if (res < 0) {
				printk(KERN_ERR "ubicom32jog: Could not alloc GPIO %d\n", pdata->button_gpio);
				goto fail_gpio_3;
			}
			gpio_direction_input(pdata->button_gpio);

			if (pdata->button_types) {
				ujd->button_type = pdata->button_type;
				ujd->button_code = pdata->button_code;
			} else {
				ujd->button_type = EV_KEY;
				ujd->button_code = KEY_MUTE;
			}
			input_set_capability(idev, ujd->button_type, ujd->button_code);
			ujd->button_state = pdata->button_active_low;
		}
		init_timer(&ujd->button_timer);
	}

	if (pdata->jog_limits) {
		ujd->rotary_max = pdata->jog_max;
		ujd->rotary_min = pdata->jog_min;
	} else {
		ujd->rotary_max = INT_MAX;
		ujd->rotary_min = INT_MIN;
	}

	if (pdata->jog_codes) {
		ujd->rotary_abs_code = pdata->jog_abs_code;
		ujd->rotary_rel_code = pdata->jog_rel_code;
	} else {
		ujd->rotary_abs_code = ABS_VOLUME;
		ujd->rotary_rel_code = REL_DIAL;
	}

	input_set_capability(idev, EV_REL, ujd->rotary_rel_code);
	input_set_capability(idev, EV_ABS, ujd->rotary_abs_code);
	input_set_abs_params(idev, ujd->rotary_abs_code, ujd->rotary_min, ujd->rotary_max, 0, 0);

	idev->name = pdata->name ? pdata->name : "Ubicom32 Jog";
	idev->phys = "ubicom32jog/input0";
	idev->dev.parent = &pdev->dev;
	idev->id.bustype = BUS_HOST;

	res = input_register_device(idev);
	if (res) {
		goto fail;
	}
	
	if (use_irq) {	
		printk(KERN_INFO "ubicom32jog: button: %d irq: %d\n", pdata->button_gpio, ujd->rotary_irq);
	} else {
		printk(KERN_INFO "ubicom32jog: button: %d, poll rotary: %d %d\n", pdata->button_gpio, pdata->jog_gpio_a, pdata->jog_gpio_b);
	}

	return 0;

fail:
	if (pdata->has_button) {
		gpio_free(pdata->button_gpio);
	}
fail_gpio_3:
	free_irq(ujd->rotary_irq, pdev);
fail_irq:
	gpio_free(pdata->jog_gpio_b);
fail_gpio_2:
	gpio_free(pdata->jog_gpio_a);
fail_gpio_1:
	input_free_device(idev);
fail_idev:
	if (ujd) {
		kfree(ujd);
	}
	dev_set_drvdata(&pdev->dev, NULL);

	return res;
}

/*
 * ubicom32jog_remove
 */
static int __devexit ubicom32jog_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct ubicom32jog_dev *ujd = dev_get_drvdata(dev);

	/*
	 * Release the GPIOs
	 */
	if (ujd->pdata->has_button) {
		gpio_free(ujd->pdata->button_gpio);
	}
	gpio_free(ujd->pdata->jog_gpio_a);
	gpio_free(ujd->pdata->jog_gpio_b);

	tasklet_kill(&ujd->rotary_task);
	del_timer_sync(&ujd->button_timer);

	/*
	 * Free the IRQ
	 */
	free_irq(ujd->rotary_irq, dev);

	input_set_drvdata(ujd->idev, NULL);
	input_unregister_device(ujd->idev);
	input_free_device(ujd->idev);
	kfree(ujd);
	dev_set_drvdata(dev, NULL);

	return 0;
}

static struct platform_driver ubicom32jog_driver = {
	.driver	= {
		.name	= "ubicom32jog",
		.owner	= THIS_MODULE,
	},
	.probe	= ubicom32jog_probe,
	.remove	= __devexit_p(ubicom32jog_remove),
};

/*
 * ubicom32jog_init
 */
static int __init ubicom32jog_init(void)
{
	return platform_driver_register(&ubicom32jog_driver);
}

/*
 * ubicom32jog_exit
 */
static void __exit ubicom32jog_exit(void)
{
	platform_driver_unregister(&ubicom32jog_driver);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pat Tjin <pat.tjin@ubicom.com>");
MODULE_DESCRIPTION("Ubicom32 Jog Dial");
module_init(ubicom32jog_init);
module_exit(ubicom32jog_exit);
