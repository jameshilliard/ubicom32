/*
 * eupgrade_charlcd.c
 *	Emergency upgrade (character lcd mode)
 *
 * Character LCD driver for emergency upgrade
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
 *	CONFIG_EUPGRADE_CHARLCD_DATA_PORT
 *	CONFIG_EUPGRADE_CHARLCD_RS_PORT
 *	CONFIG_EUPGRADE_CHARLCD_RS_PIN
 *	CONFIG_EUPGRADE_CHARLCD_RW_PORT
 *	CONFIG_EUPGRADE_CHARLCD_RW_PIN
 *	CONFIG_EUPGRADE_CHARLCD_E_PORT
 *	CONFIG_EUPGRADE_CHARLCD_E_PIN
 *
 * example:
 *	#define CONFIG_EUPGRADE_CHARLCD_DATA_PORT	IO_PORT_RI
 *	#define CONFIG_EUPGRADE_CHARLCD_RS_PORT		IO_PORT_RI
 *	#define CONFIG_EUPGRADE_CHARLCD_RS_BIT		8
 *	#define CONFIG_EUPGRADE_CHARLCD_RW_PORT		IO_PORT_RI
 *	#define CONFIG_EUPGRADE_CHARLCD_RW_BIT		9
 *	#define CONFIG_EUPGRADE_CHARLCD_E_PORT		IO_PORT_RI
 *	#define CONFIG_EUPGRADE_CHARLCD_E_BIT		10
 */

#define eupgrade_charlcd_wait(ns)	\
	if (ns < 1000) {		\
		udelay(1);		\
	} else {			\
		udelay(ns / 1000);	\
	}

/*
 * eupgrade_charlcd_get_data
 */
static inline unsigned char eupgrade_charlcd_get_data(unsigned long port)
{
	unsigned char data;

	asm volatile ("move.1 %[data], 0x0F(%[port])" : [data] "=d" (data) : [port] "a" (port));

	return data;
}

/*
 * eupgrade_charlcd_set_data
 */
static inline void eupgrade_charlcd_set_data(unsigned long port, unsigned char data)
{
	asm volatile ("move.1 0x0B(%[port]), %[data]" :: [port] "a" (port), [data] "d" (data));
}

/*
 * eupgrade_charlcd_set_port_high
 */
static inline void eupgrade_charlcd_set_port_high(unsigned long port, unsigned int bit)
{
	asm volatile ("or.4 0x08(%[port]), 0x08(%[port]), %[mask]" :: [port] "a" (port), [mask] "d" (1 << bit) : "cc");
}

/*
 * eupgrade_charlcd_set_port_low
 */
static inline void eupgrade_charlcd_set_port_low(unsigned long port, unsigned int bit)
{
	asm volatile ("and.4 0x08(%[port]), 0x08(%[port]), %[mask]" :: [port] "a" (port), [mask] "d" (~(1 << bit)) : "cc");
}

/*
 * eupgrade_charlcd_set_port
 */
static inline void eupgrade_charlcd_set_port(unsigned long port, unsigned int bit, int value)
{
	if (value) {
		eupgrade_charlcd_set_port_high(port, bit);
		return;
	}
	eupgrade_charlcd_set_port_low(port, bit);
}

/*
 * eupgrade_charlcd_set_idle
 */
static void eupgrade_charlcd_set_idle(void)
{
	// set input
	UBICOM32_IO_PORT(CONFIG_EUPGRADE_CHARLCD_DATA_PORT)->gpio_ctl &= ~(0xFF << 0);

	eupgrade_charlcd_set_port_low(CONFIG_EUPGRADE_CHARLCD_E_PORT, CONFIG_EUPGRADE_CHARLCD_E_BIT);
	eupgrade_charlcd_set_port_low(CONFIG_EUPGRADE_CHARLCD_RW_PORT, CONFIG_EUPGRADE_CHARLCD_RW_BIT);
}

/*
 * eupgrade_chareupgrade_charlcd_read_data
 */
static unsigned char eupgrade_charlcd_read_data(unsigned char rs)
{
	unsigned char res;

	eupgrade_charlcd_set_port(CONFIG_EUPGRADE_CHARLCD_RS_PORT, CONFIG_EUPGRADE_CHARLCD_RS_BIT, rs);
	eupgrade_charlcd_set_port_high(CONFIG_EUPGRADE_CHARLCD_RW_PORT, CONFIG_EUPGRADE_CHARLCD_RW_BIT);

	// wait Tsu = 100ns
	eupgrade_charlcd_wait(100);

	eupgrade_charlcd_set_port_high(CONFIG_EUPGRADE_CHARLCD_E_PORT, CONFIG_EUPGRADE_CHARLCD_E_BIT);

	// wait Td = 60ns
	eupgrade_charlcd_wait(60 + 100);

	res = eupgrade_charlcd_get_data(CONFIG_EUPGRADE_CHARLCD_DATA_PORT);

	// wait Tw = 300ns
	eupgrade_charlcd_wait(300 - 100);

	// wait Tdh + Tc = 20ns + 200ns
	eupgrade_charlcd_wait(20 + 200);

	eupgrade_charlcd_set_idle();

	return res;
}

/*
 * eupgrade_charlcd_send_data
 */
static void eupgrade_charlcd_send_data(unsigned char data, unsigned char rs)
{
	eupgrade_charlcd_set_port(CONFIG_EUPGRADE_CHARLCD_RS_PORT, CONFIG_EUPGRADE_CHARLCD_RS_BIT, rs);
	eupgrade_charlcd_set_port_low(CONFIG_EUPGRADE_CHARLCD_RW_PORT, CONFIG_EUPGRADE_CHARLCD_RW_BIT);

	// wait Tsu = 100ns
	eupgrade_charlcd_wait(100);

	// set output
	UBICOM32_IO_PORT(CONFIG_EUPGRADE_CHARLCD_DATA_PORT)->gpio_ctl |= (0xFF << 0);

	eupgrade_charlcd_set_port_high(CONFIG_EUPGRADE_CHARLCD_E_PORT, CONFIG_EUPGRADE_CHARLCD_E_BIT);

	// wait Td = 60ns
	eupgrade_charlcd_wait(60);

	eupgrade_charlcd_set_data(CONFIG_EUPGRADE_CHARLCD_DATA_PORT, data);

	// wait Tw = 300ns
	eupgrade_charlcd_wait(300);

	eupgrade_charlcd_set_port_low(CONFIG_EUPGRADE_CHARLCD_E_PORT, CONFIG_EUPGRADE_CHARLCD_E_BIT);

	// wait Tdh = 20ns + Tdh = 200ns
	eupgrade_charlcd_wait(220);

	eupgrade_charlcd_set_idle();
}

/*
 * eupgrade_charlcd_clear
 */
static void eupgrade_charlcd_clear(void)
{
	eupgrade_charlcd_send_data(0x01, 0);
	// must wait 1.53ms
}

/*
 * eupgrade_charlcd_return_home
 */
static void eupgrade_charlcd_return_home(void)
{
	eupgrade_charlcd_send_data(0x02, 0);
	// must wait 1.53ms
}

/*
 * eupgrade_charlcd_set_entry_mode
 */
static void eupgrade_charlcd_set_entry_mode(unsigned char id, unsigned sh)
{
	eupgrade_charlcd_send_data(0x04 | (id ? 2 : 0) | (sh ? 1 : 0), 0);
	// must wait 39us
}

/*
 * eupgrade_charlcd_display_control
 */
static void eupgrade_charlcd_display_control(unsigned char disp, unsigned char cursor, unsigned char blink)
{
	eupgrade_charlcd_send_data(0x08 | (disp ? 4 : 0) | (cursor ? 2 : 0) | (blink ? 1 : 0), 0);
}

/*
 * eupgrade_charlcd_cursor_shift
 */
static void eupgrade_charlcd_cursor_shift(unsigned char sc, unsigned char rl) __attribute__((unused));
static void eupgrade_charlcd_cursor_shift(unsigned char sc, unsigned char rl)
{
	eupgrade_charlcd_send_data(0x10 | (sc ? 8 : 0) | (rl ? 4 : 0), 0);
	// must wait 39us
}

/*
 * eupgrade_charlcd_function_set
 */
static void eupgrade_charlcd_function_set(unsigned char dl, unsigned char n, unsigned char f)
{
	eupgrade_charlcd_send_data(0x20 | (dl ? 0x10 : 0) | (n ? 0x08 : 0) | (f ? 0x04 : 0), 0);
	// must wait 39us
}

/*
 * eupgrade_charlcd_set_cgram_addr
 */
static void eupgrade_charlcd_set_cgram_addr(unsigned char addr) __attribute__((unused));
static void eupgrade_charlcd_set_cgram_addr(unsigned char addr)
{
	eupgrade_charlcd_send_data(0x40 | (addr & 0x3f), 0);
	// must wait 39us
}

/*
 * eupgrade_charlcd_set_ddram_addr
 */
static void eupgrade_charlcd_set_ddram_addr(unsigned char addr)
{
	eupgrade_charlcd_send_data(0x80 | (addr & 0x7f), 0);
	// must wait 39us
}

/*
 * eupgrade_charlcd_is_busy(void)
 */
static unsigned char eupgrade_charlcd_is_busy(void)
{
	return eupgrade_charlcd_read_data(0) & 0x80;
}

/*
 * eupgrade_charlcd_read
 */
static unsigned char eupgrade_charlcd_read(void) __attribute__((unused));
static unsigned char eupgrade_charlcd_read(void)
{
	return eupgrade_charlcd_read_data(1);
	// must wait 43us
}

/*
 * eupgrade_charlcd_busy_wait
 */
static void eupgrade_charlcd_busy_wait(void)
{
	while (eupgrade_charlcd_is_busy());
	udelay(2000);
}

/*
 * eupgrade_charlcd_write_string
 */
static void eupgrade_charlcd_write_string(char *str)
{
	while (*str) {
		eupgrade_charlcd_send_data(*str, 1);
		str++;
		eupgrade_charlcd_busy_wait();
	}
	// must wait 43us
}

/*
 * eupgrade_charlcd_write
 */
static void eupgrade_charlcd_write(unsigned char data) __attribute__((unused));
static void eupgrade_charlcd_write(unsigned char data)
{
	eupgrade_charlcd_send_data(data, 1);
	// must wait 43us
}

/*
 * eupgrade_charlcd_init
 */
static void eupgrade_charlcd_hw_init(void)
{
	UBICOM32_IO_PORT(CONFIG_EUPGRADE_CHARLCD_DATA_PORT)->gpio_ctl &= ~(0xFF << 0);
	UBICOM32_IO_PORT(CONFIG_EUPGRADE_CHARLCD_DATA_PORT)->gpio_mask |= (0xFF << 0);

	UBICOM32_IO_PORT(CONFIG_EUPGRADE_CHARLCD_RS_PORT)->gpio_ctl |= (1 << CONFIG_EUPGRADE_CHARLCD_RS_BIT);
	UBICOM32_IO_PORT(CONFIG_EUPGRADE_CHARLCD_RS_PORT)->gpio_mask |= (1 << CONFIG_EUPGRADE_CHARLCD_RS_BIT);

	UBICOM32_IO_PORT(CONFIG_EUPGRADE_CHARLCD_RW_PORT)->gpio_ctl |= (1 << CONFIG_EUPGRADE_CHARLCD_RW_BIT);
	UBICOM32_IO_PORT(CONFIG_EUPGRADE_CHARLCD_RW_PORT)->gpio_mask |= (1 << CONFIG_EUPGRADE_CHARLCD_RW_BIT);

	UBICOM32_IO_PORT(CONFIG_EUPGRADE_CHARLCD_E_PORT)->gpio_ctl |= (1 << CONFIG_EUPGRADE_CHARLCD_E_BIT);
	UBICOM32_IO_PORT(CONFIG_EUPGRADE_CHARLCD_E_PORT)->gpio_out &= ~(1 << CONFIG_EUPGRADE_CHARLCD_E_BIT);
	UBICOM32_IO_PORT(CONFIG_EUPGRADE_CHARLCD_E_PORT)->gpio_mask |= (1 << CONFIG_EUPGRADE_CHARLCD_E_BIT);

	eupgrade_charlcd_function_set(1, 1, 0);
	eupgrade_charlcd_busy_wait();

	eupgrade_charlcd_display_control(1, 0, 0);
	eupgrade_charlcd_busy_wait();

	eupgrade_charlcd_function_set(1, 1, 0);
	eupgrade_charlcd_busy_wait();

	eupgrade_charlcd_clear();
	eupgrade_charlcd_busy_wait();

	eupgrade_charlcd_return_home();
	eupgrade_charlcd_busy_wait();

	eupgrade_charlcd_set_entry_mode(1, 0);
	eupgrade_charlcd_busy_wait();
}

/*
 * eupgrade_charlcd_status_callback
 *	Callback from eupgrade which details the state of the upgrade procedure.
 */
static void eupgrade_charlcd_status_callback(void *appdata, enum eupgrade_states state, int step, int steps, int res)
{
	char s[40];

	switch (state) {
	case EUPGRADE_STATE_BEGIN:
		eupgrade_charlcd_clear();
		eupgrade_charlcd_set_ddram_addr(0);
		eupgrade_charlcd_busy_wait();
		eupgrade_charlcd_write_string("Insert USB Stick");
		break;

	case EUPGRADE_STATE_VERIFY:
		eupgrade_charlcd_clear();
		eupgrade_charlcd_set_ddram_addr(0);
		eupgrade_charlcd_busy_wait();
		eupgrade_charlcd_write_string("Verifying Image");
		if (res) {
			eupgrade_charlcd_set_ddram_addr(0x40);
			eupgrade_charlcd_busy_wait();
			if (step == USB_UPGRADE_VERIFY_STEP_IDENTITY) {
				eupgrade_charlcd_write_string("Incorrect Image");
			} else {
				eupgrade_charlcd_write_string("Corrupted Image");
			}
			udelay(5000 * 1000);
		}
		break;

	case EUPGRADE_STATE_ERASE:
		eupgrade_charlcd_clear();
		eupgrade_charlcd_busy_wait();
		eupgrade_charlcd_set_ddram_addr(0);
		eupgrade_charlcd_busy_wait();
		sprintf(s, "Erasing: %d%%   ", (step * 100) / steps);
		eupgrade_charlcd_write_string(s);
		break;

	case EUPGRADE_STATE_WRITE:
		eupgrade_charlcd_clear();
		eupgrade_charlcd_busy_wait();
		eupgrade_charlcd_set_ddram_addr(0);
		eupgrade_charlcd_busy_wait();
		sprintf(s, "Writing: %d%%   ", (step * 100) / steps);
		eupgrade_charlcd_write_string(s);
		break;

	case EUPGRADE_STATE_SUCCESS:
		eupgrade_charlcd_clear();
		eupgrade_charlcd_busy_wait();
		eupgrade_charlcd_set_ddram_addr(0);
		eupgrade_charlcd_busy_wait();
		eupgrade_charlcd_write_string("Success!");
		break;

	default:
		break;
	}
}

/*
 * eupgrade_charlcd_init
 */
int eupgrade_charlcd_init(void)
{
	printf("[Emergency Upgrade CHARLCD Init]\n");

	eupgrade_charlcd_hw_init();

	if (eupgrade_register_status_callback(eupgrade_charlcd_status_callback, NULL)) {
		printf("\tCould not register callback\n");
	}

	return 0;
}

