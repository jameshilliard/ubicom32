/*
 * drivers/video/backlight/ubicom32bl.c
 *	Backlight driver for the Ubicom32 platform
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
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/backlight.h>
#include <linux/fb.h>

#include <asm/gpio.h>
#include <asm/ubicom32bl.h>

#define DRIVER_NAME			"ubicom32bl"
#define UBICOM32BL_MAX_BRIGHTNESS	255

struct ubicom32bl_data {
	/*
	 * Pointer to the platform data structure.  Keep this around since we need values
	 * from it to set the backlight intensity.
	 */
	const struct ubicom32bl_platform_data	*pdata;

	/*
	 * Backlight device, we have to save this for use when we remove ourselves.
	 */
	struct backlight_device			*bldev;

	/*
	 * Current intensity, used for get_intensity.
	 */
	int					cur_intensity;

	/*
	 * Init function for PWM
	 */
	int (*init_fn)(struct ubicom32bl_data *);

	/*
	 * Set intensity function depending on the backlight type
	 */
	int (*set_intensity_fn)(struct ubicom32bl_data *, int);
};

/*
 * ubicom32bl_set_intensity_gpio
 */
static int ubicom32bl_set_intensity_gpio(struct ubicom32bl_data *ud, int intensity)
{
	ud->cur_intensity = intensity ? 255 : 0;

	if (intensity) {
		gpio_set_value(ud->pdata->gpio, !ud->pdata->invert);
		return 0;
	}

	gpio_set_value(ud->pdata->gpio, ud->pdata->invert);
	return 0;
}

#ifdef IP7000
#define UBICOM32BL_NUM_PWM_CHANNELS 3
static volatile u32_t *ubicom32bl_channel_ctl[] = {
	&(UBICOM32_IO_PORT(IO_PORT_RC)->ctl1),
	&(UBICOM32_IO_PORT(IO_PORT_RC)->ctl2),
	&(UBICOM32_IO_PORT(IO_PORT_RH)->ctl1),
};
#elif IP8000
#define UBICOM32BL_NUM_PWM_CHANNELS 4
static volatile u32_t *ubicom32bl_channel_ctl[] = {
	&(UBICOM32_IO_PORT(IO_PORT_RQ)->ctl1),
	&(UBICOM32_IO_PORT(IO_PORT_RR)->ctl1),
	&(UBICOM32_IO_PORT(IO_PORT_RS)->ctl1),
	&(UBICOM32_IO_PORT(IO_PORT_RT)->ctl1),
};
#else
#define UBICOM32BL_NUM_PWM_CHANNELS 0
#endif

/*
 * ubicom32bl_set_intensity_hw
 */
static int ubicom32bl_set_intensity_hw(struct ubicom32bl_data *ud, int intensity)
{
#if UBICOM32BL_NUM_PWM_CHANNELS
	u16_t period = ud->pdata->pwm_period;
	u16_t duty;
#endif

	if (ud->pdata->pwm_channel > UBICOM32BL_NUM_PWM_CHANNELS) {
		return -ENODEV;
	}

#if UBICOM32BL_NUM_PWM_CHANNELS
	/*
	 * Calculate the new duty cycle
	 */
	if (intensity == UBICOM32BL_MAX_BRIGHTNESS) {
		duty = period + 1;
	} else {
		duty = (period * intensity) / UBICOM32BL_MAX_BRIGHTNESS;
	}

	/*
	 * Set the new duty cycle
	 */
	*ubicom32bl_channel_ctl[ud->pdata->pwm_channel] = (ud->pdata->pwm_period << 16) | duty;

	ud->cur_intensity = intensity;

#endif

	return 0;
}

/*
 * ubicom32bl_set_intensity
 */
static int ubicom32bl_set_intensity(struct backlight_device *bd)
{
	struct ubicom32bl_data *ud = (struct ubicom32bl_data *)bl_get_data(bd);
	int intensity = bd->props.brightness;

	/*
	 * If we're blanked the the intensity doesn't matter.
	 */
	if ((bd->props.power != FB_BLANK_UNBLANK) || (bd->props.fb_blank != FB_BLANK_UNBLANK)) {
		intensity = 0;
	}

	/*
	 * Check for inverted backlight.
	 */
	if (ud->pdata->invert) {
		intensity = UBICOM32BL_MAX_BRIGHTNESS - intensity;
	}

	if (ud->set_intensity_fn) {
		return ud->set_intensity_fn(ud, intensity);
	}

	return -ENXIO;
}

/*
 * ubicom32bl_get_intensity
 *	Return the current intensity of the backlight.
 */
static int ubicom32bl_get_intensity(struct backlight_device *bd)
{
	struct ubicom32bl_data *ud = (struct ubicom32bl_data *)bl_get_data(bd);

	return ud->cur_intensity;
}

/*
 * ubicom32bl_init_hw_pwm
 *	Set the appropriate PWM registers
 */
static int ubicom32bl_init_hw_pwm(struct ubicom32bl_data *ud)
{
	if (ud->pdata->pwm_channel > UBICOM32BL_NUM_PWM_CHANNELS) {
		return -ENODEV;
	}

#if UBICOM32BL_NUM_PWM_CHANNELS
# ifdef IP7000
	{
		/*
		 * bit 13: enable
		 */
		u16_t pwm_cfg = (1 << 13) | (ud->pdata->pwm_prescale << 8) ;

		switch (ud->pdata->pwm_channel) {
		case 0:
			/*
			 * Channel 0 is in the lower half of PORT C ctl0 and ctl1 (PA5)
			 */
			UBICOM32_IO_PORT(IO_PORT_RC)->ctl0 &= ~0xFFFF;
			UBICOM32_IO_PORT(IO_PORT_RC)->ctl0 |= pwm_cfg;
			UBICOM32_IO_PORT(IO_PORT_RC)->ctl1 = ud->pdata->pwm_period << 16;
			
			/*
			 * If the port function is not set, set it to GPIO/PWM
			 */
			if (!UBICOM32_IO_PORT(IO_PORT_RA)->function) {
				UBICOM32_IO_PORT(IO_PORT_RA)->function = 3;
			}

			UBICOM32_GPIO_DISABLE(GPIO_RA_5);
			break;

		case 1:
			/*
			 * Channel 1 is in the upper half of PORT C ctl0 and ctl2 (PE4)
			 */
			UBICOM32_IO_PORT(IO_PORT_RC)->ctl0 &= ~0xFFFF0000;
			UBICOM32_IO_PORT(IO_PORT_RC)->ctl0 |= (pwm_cfg << 16);
			UBICOM32_IO_PORT(IO_PORT_RC)->ctl2 = ud->pdata->pwm_period << 16;

			/*
			 * If the port function is not set, set it to GPIO/ExtIOInt
			 */
			if (!UBICOM32_IO_PORT(IO_PORT_RE)->function) {
				UBICOM32_IO_PORT(IO_PORT_RE)->function = 3;
			}

			UBICOM32_GPIO_DISABLE(GPIO_RE_4);
			break;

		case 2:
			/*
			 * Channel 2 is in PORT H ctl0 and ctl1 (PD0)
			 */
			UBICOM32_IO_PORT(IO_PORT_RH)->ctl0 &= ~0xFFFF0000;
			UBICOM32_IO_PORT(IO_PORT_RH)->ctl0 = pwm_cfg;
			UBICOM32_IO_PORT(IO_PORT_RH)->ctl1 = ud->pdata->pwm_period << 16;

			/*
			 * If the port function is not set, set it to GPIO
			 */
			if (!UBICOM32_IO_PORT(IO_PORT_RD)->function) {
				UBICOM32_IO_PORT(IO_PORT_RD)->function = 3;
			}

			UBICOM32_GPIO_DISABLE(GPIO_RD_0);
			break;
		}
	}
# elif IP8000
	{
		struct ubicom32_io_port *port = UBICOM32_IO_PORT(IO_PORT_RQ + (ud->pdata->pwm_channel * 0x4000));
		struct ubicom32_gpio_port *gpio = UBICOM32_PORT_NUM_TO_GPIO(gpio_bank(ud->pdata->gpio));
		unsigned int mask = 0;

		/*
		 * Check to see if the pin is busy so we don't clobber any functions
		 */
		u32_t fn_sel = gpio->fn_sel[0] | gpio->fn_sel[1] | gpio->fn_sel[2] | gpio->fn_sel[3];
		if (fn_sel & gpio_bit(ud->pdata->gpio)) {
			printk(KERN_WARNING DRIVER_NAME ": GPIO %d in use %08x\n", 
				ud->pdata->gpio, fn_sel);
			return -EBUSY;
		}

		switch (ud->pdata->pwm_channel) {
		case 0:
			/*
			 * PWM Q can go on PG4[30](fn4), PG4[31](fn4), PG5[0](fn3), PG5[1](fn3)
			 */
			if (ud->pdata->gpio == GPIO_PG4_30) {
				mask = (1 << 30);
				gpio->fn_sel[3] |= mask;
			} else
			if (ud->pdata->gpio == GPIO_PG4_31) {
				mask = (1 << 31);
				gpio->fn_sel[3] |= mask;
			} else
			if (ud->pdata->gpio == GPIO_PG5_0) {
				mask = (1 << 0);
				gpio->fn_sel[2] |= mask;
			} else
			if (ud->pdata->gpio != GPIO_PG5_1) {
				mask = (1 << 1);
				gpio->fn_sel[2] |= mask;
			} else {
				printk(KERN_WARNING DRIVER_NAME ": GPIO %d invalid for pwm channel %d\n", 
					ud->pdata->gpio, ud->pdata->pwm_channel);
				return -EINVAL;
			}
			break;

		case 1:
			/*
			 * PWM R can go on PG5[3](fn3), PG5[2](fn3)
			 */
			if (ud->pdata->gpio == GPIO_PG5_3) {
				mask = (1 << 3);
				gpio->fn_sel[2] |= mask;
			} else
			if (ud->pdata->gpio != GPIO_PG5_2) {
				mask = (1 << 2);
				gpio->fn_sel[2] |= mask;
			} else {
				printk(KERN_WARNING DRIVER_NAME ": GPIO %d invalid for pwm channel %d\n", 
					ud->pdata->gpio, ud->pdata->pwm_channel);
				return -EINVAL;
			}
			break;

		case 2:
			/*
			 * PWM S can go on PG5[1](fn2), PG5[0](fn2)
			 */
			if (ud->pdata->gpio == GPIO_PG5_1) {
				mask = (1 << 1);
				gpio->fn_sel[1] |= mask;
			} else
			if (ud->pdata->gpio != GPIO_PG5_0) {
				mask = (1 << 0);
				gpio->fn_sel[1] |= mask;
			} else {
				printk(KERN_WARNING DRIVER_NAME ": GPIO %d invalid for pwm channel %d\n", 
					ud->pdata->gpio, ud->pdata->pwm_channel);
				return -EINVAL;
			}
			break;

		case 3:
			/*
			 * PWM T can go on PG5[3](fn2), PG5[2](fn2)
			 */
			if (ud->pdata->gpio == GPIO_PG5_3) {
				mask = (1 << 3);
				gpio->fn_sel[1] |= mask;
			} else
			if (ud->pdata->gpio != GPIO_PG5_2) {
				mask = (1 << 2);
				gpio->fn_sel[1] |= mask;
			} else {
				printk(KERN_WARNING DRIVER_NAME ": GPIO %d invalid for pwm channel %d\n", 
					ud->pdata->gpio, ud->pdata->pwm_channel);
				return -EINVAL;
			}
			break;
		}

		/*
		 * PWM is clocked at clk_core / (2 ^ pwm_prescale[3:0])
		 */
		port->function = 1<<24 | 1;
		port->ctl0 = (ud->pdata->pwm_prescale << 8) ;
		gpio->gpio_ctl |= mask;
	}
# endif
#endif

	return 0;
}

/*
 * ubicom32bl_init_gpio
 *	Allocate the appropriate GPIO
 */
static int ubicom32bl_init_gpio(struct ubicom32bl_data *ud)
{
	if (gpio_request(ud->pdata->gpio, "BLCTL GPIO")) {
		printk(KERN_WARNING DRIVER_NAME ": Could not request BLCTL gpio %d\n", ud->pdata->gpio);
		return -EBUSY;
	}
	gpio_direction_output(ud->pdata->gpio, !ud->pdata->invert);

	return 0;
}

static struct backlight_ops ubicom32bl_ops = {
	.get_brightness = ubicom32bl_get_intensity,
	.update_status  = ubicom32bl_set_intensity,
};

/*
 * ubicom32bl_probe
 */
static int ubicom32bl_probe(struct platform_device *pdev)
{
	const struct ubicom32bl_platform_data *pdata = pdev->dev.platform_data;
	struct ubicom32bl_data *ud;
	struct backlight_device *bldev;
	struct backlight_properties props;
	int retval;

	/*
	 * Check to see if we have any platform data, if we don't then the backlight is not
	 * configured on this device.
	 */
	if (!pdata) {
		return -ENODEV;
	}

	/*
	 * Allocate our private data
	 */
	ud = kzalloc(sizeof(struct ubicom32bl_data), GFP_KERNEL);
	if (!ud) {
		return -ENOMEM;
	}

	ud->pdata = pdata;

	/*
	 * Check to see that the platform data is valid for this driver
	 */
	switch (pdata->type) {
	case UBICOM32BL_TYPE_PWM:
		{
			/*
			 * Make sure we have a PWM peripheral
			 */
			if ((ubicom32_get_chip_id() != 0x00030001) &&
			    (ubicom32_get_chip_id() != 0x00040000)) {
				printk(KERN_WARNING DRIVER_NAME ": no PWM support on this chip\n");
				retval = -ENODEV;
				goto fail;
			}

			if (pdata->pwm_channel > UBICOM32BL_NUM_PWM_CHANNELS) {
				printk(KERN_WARNING DRIVER_NAME ": PWM channel %d does not exist\n", pdata->pwm_channel);
				retval = -ENODEV;
				goto fail;
			}

			if (pdata->pwm_prescale > 16) {
				printk(KERN_WARNING DRIVER_NAME ": PWM prescale must be < 16\n");
				retval = -EINVAL;
				goto fail;
			}

			ud->init_fn = ubicom32bl_init_hw_pwm;
			ud->set_intensity_fn = ubicom32bl_set_intensity_hw;
			printk(KERN_INFO DRIVER_NAME ": HW PWM\n");
			break;
		}

	case UBICOM32BL_TYPE_PWM_HRT:
		// For now, PWM HRT devices are treated as binary lights.

	case UBICOM32BL_TYPE_BINARY:
		ud->init_fn = ubicom32bl_init_gpio;
		ud->set_intensity_fn = ubicom32bl_set_intensity_gpio;
		printk(KERN_INFO DRIVER_NAME ": Binary backlight\n");
		break;
	}

	/*
	 * Register our backlight device
	 */
	memset(&props, 0, sizeof(struct backlight_properties));
	props.max_brightness = UBICOM32BL_MAX_BRIGHTNESS;	
	bldev = backlight_device_register(DRIVER_NAME, &pdev->dev, ud, 
					  &ubicom32bl_ops, &props);
	if (IS_ERR(bldev)) {
		retval = PTR_ERR(bldev);
		goto fail;
	}

	ud->bldev = bldev;
	ud->cur_intensity = pdata->default_intensity;
	platform_set_drvdata(pdev, ud);

	/*
	 * Start up the backlight at the prescribed default intensity
	 */
	bldev->props.power = FB_BLANK_UNBLANK;
	bldev->props.max_brightness = UBICOM32BL_MAX_BRIGHTNESS;
	bldev->props.brightness = pdata->default_intensity;

	if (ud->init_fn) {
		if (ud->init_fn(ud) != 0) {
			retval = -ENODEV;
			backlight_device_unregister(ud->bldev);
			goto fail;
		}
	}
	ubicom32bl_set_intensity(bldev);

	printk(KERN_INFO DRIVER_NAME ": Backlight driver started ch=%d brightness=%d/%d\n",
		pdata->pwm_channel, bldev->props.brightness, bldev->props.max_brightness);

	return 0;

fail:
	platform_set_drvdata(pdev, NULL);
	kfree(ud);
	return retval;
}

/*
 * ubicom32bl_remove
 */
static int __exit ubicom32bl_remove(struct platform_device *pdev)
{
	struct ubicom32bl_data *ud = platform_get_drvdata(pdev);

	backlight_device_unregister(ud->bldev);
	platform_set_drvdata(pdev, NULL);
	kfree(ud);

	return 0;
}

static struct platform_driver ubicom32bl_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},

	.probe = ubicom32bl_probe,
	.remove = __exit_p(ubicom32bl_remove),
};

/*
 * ubicom32bl_init
 */
static int __init ubicom32bl_init(void)
{
	return platform_driver_register(&ubicom32bl_driver);
}
module_init(ubicom32bl_init);

/*
 * ubicom32bl_exit
 */
static void __exit ubicom32bl_exit(void)
{
	platform_driver_unregister(&ubicom32bl_driver);
}
module_exit(ubicom32bl_exit);

MODULE_AUTHOR("Patrick Tjin <@ubicom.com>");
MODULE_DESCRIPTION("Ubicom32 backlight driver");
MODULE_LICENSE("GPL");
