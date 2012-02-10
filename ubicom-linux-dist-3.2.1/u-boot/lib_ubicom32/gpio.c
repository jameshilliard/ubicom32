/*
 *   Ubicom32 family general purpose I/O support
 *
 * (C) Copyright 2009
 * Ubicom, Inc. www.ubicom.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <common.h>
#include <asm/gpio.h>

int __gpio_get_value(unsigned gpio)
{
	return 0;
}

void __gpio_set_value(unsigned gpio, int value)
{
}

int __gpio_cansleep(unsigned gpio)
{
	return 0;
}

int __gpio_to_irq(unsigned gpio)
{
	return 0;
}

int gpio_request(unsigned gpio, const char *label)
{
	return 0;
}

void gpio_free(unsigned gpio)
{
}

int gpio_direction_input(unsigned gpio)
{
	UBICOM32_GPIO_ENABLE(gpio);
	UBICOM32_GPIO_SET_PIN_INPUT(gpio);
	return 0;
}

int gpio_direction_output(unsigned gpio, int value)
{
	UBICOM32_GPIO_ENABLE(gpio);
	UBICOM32_GPIO_SET_PIN_OUTPUT(gpio);
	UBICOM32_GPIO_SET_PIN(gpio, value);
	return 0;
}

