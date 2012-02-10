/*
 * eupgrade_led.c
 *	Emergency upgrade (led mode)
 *
 * Blink a LED when in emergency upgrade
 *
 * (C) Copyright 2010
 * Ubicom, Inc. www.ubicom.com
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <asm/gpio.h>
#include "cmd_usb_upgrade.h"
#include "eupgrade.h"

/*
 * Configuration:
 *	CONFIG_EUPGRADE_LED_GPIO
 *	CONFIG_EUPGRADE_LED_ACTIVE_HIGH
 *	CONFIG_EUPGRADE_LED_BLINK_COUNT
 *	CONFIG_EUPGRADE_LED_BLINK_INTERVAL_OFF (in uS)
 *	CONFIG_EUPGRADE_LED_BLINK_INTERVAL_ON (in uS)
 *
 * example:
 *	#define CONFIG_EUPGRADE_LED
 *	#define CONFIG_EUPGRADE_LED_GPIO                GPIO_RD_0
 *	#define CONFIG_EUPGRADE_LED_ACTIVE_HIGH
 *	#define CONFIG_EUPGRADE_LED_BLINK_INTERVAL_ON   (250 * 1000)
 *	#define CONFIG_EUPGRADE_LED_BLINK_INTERVAL_OFF  (250 * 1000)
 *	#define CONFIG_EUPGRADE_LED_BLINK_COUNT         5
 * 
 */

/*
 * eupgrade_led_status_callback
 *	Callback from eupgrade which details the state of the upgrade procedure.
 *
 * Add code in this function to blink the LED in response to the different states.
 */
static void eupgrade_led_status_callback(void *appdata, enum eupgrade_states state, int step, int steps, int res)
{
	int i;
	switch (state) {
	case EUPGRADE_STATE_BEGIN:
		/*
		 * Make sure the LED is OFF
		 */
#ifdef CONFIG_EUPGRADE_LED_ACTIVE_HIGH
		UBICOM32_GPIO_SET_PIN_LOW(CONFIG_EUPGRADE_LED_GPIO);
#else
		UBICOM32_GPIO_SET_PIN_HIGH(CONFIG_EUPGRADE_LED_GPIO);
#endif
		UBICOM32_GPIO_SET_PIN_OUTPUT(CONFIG_EUPGRADE_LED_GPIO);
		UBICOM32_GPIO_ENABLE(CONFIG_EUPGRADE_LED_GPIO);

		/*
		 * Blink the led a few times
		 */
		for (i = 0; i < CONFIG_EUPGRADE_LED_BLINK_COUNT; i++) {
			/*
			 * Turn it on
			 */
			UBICOM32_GPIO_SET_PIN_TOGGLE(CONFIG_EUPGRADE_LED_GPIO);
			udelay(CONFIG_EUPGRADE_LED_BLINK_INTERVAL_ON);

			/*
			 * Turn it off
			 */
			UBICOM32_GPIO_SET_PIN_TOGGLE(CONFIG_EUPGRADE_LED_GPIO);
			udelay(CONFIG_EUPGRADE_LED_BLINK_INTERVAL_OFF);
		}
		break;

	case EUPGRADE_STATE_ERASE:
	case EUPGRADE_STATE_WRITE:
		UBICOM32_GPIO_SET_PIN_TOGGLE(CONFIG_EUPGRADE_LED_GPIO);
		/*
		 * Turn it off when we're done
		 */
		if (step == steps) {
#ifdef CONFIG_EUPGRADE_LED_ACTIVE_HIGH
			UBICOM32_GPIO_SET_PIN_LOW(CONFIG_EUPGRADE_LED_GPIO);
#else
			UBICOM32_GPIO_SET_PIN_HIGH(CONFIG_EUPGRADE_LED_GPIO);
#endif
		}
		break;

	case EUPGRADE_STATE_SUCCESS:
		/*
		 * Done, blink at 2x the rate for the same amount of time to signal success
		 */
		for (i = 0; i < CONFIG_EUPGRADE_LED_BLINK_COUNT * 2; i++) {
			/*
			 * Turn it on
			 */
			UBICOM32_GPIO_SET_PIN_TOGGLE(CONFIG_EUPGRADE_LED_GPIO);
			udelay(CONFIG_EUPGRADE_LED_BLINK_INTERVAL_ON / 2);

			/*
			 * Turn it off
			 */
			UBICOM32_GPIO_SET_PIN_TOGGLE(CONFIG_EUPGRADE_LED_GPIO);
			udelay(CONFIG_EUPGRADE_LED_BLINK_INTERVAL_OFF / 2);
		}
		break;

	default:
		break;
	}
}

/*
 * eupgrade_led_init
 */
int eupgrade_led_init(void)
{
	printf("[Emergency Upgrade LED Init]\n");

	if (eupgrade_register_status_callback(eupgrade_led_status_callback, NULL)) {
		printf("\tCould not register callback\n");
	}

	return 0;
}

