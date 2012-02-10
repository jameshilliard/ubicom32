/*
 * usb.c
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

/*
 * This file provides a hook for the board to do anything special
 * for bringing up and down the usb controller.
 *
 * Each board must provide the following two functions:
 *	void board_usb_deinit(void)
 *		stop the usb controller, however, beware that certain
 *		drivers (musb) attempt to use the controller even after
 *		calling this function!
 *	void board_usb_init(void)
 *		start the usb controller.  It would be wise to stop
 *		it and completely reset the controller if it's already
 *		running.
 */

#include <common.h>
#include <asm/ip5000.h>

#ifdef IP7000
/*
 * board_usb_reset
 *	reset the USB controller
 */
static void board_usb_reset(void)
{
	/*
	 * Put the port into its default function and put the PLLs into reset.
	 */
	struct ubicom32_io_port *io_port = UBICOM32_IO_PORT(IO_PORT_RJ);
	io_port->function =
			IO_FUNC_FUNCTION_RESET(1) | IO_FUNC_FUNCTION_RESET(2) |
			IO_FUNC_FUNCTION_RESET(3) | IO_FUNC_FUNCTION_RESET(4);
	io_port->ctl0 = 0;
	ubicom32_disable_interrupt(PORT_OTHER_INT(IO_PORT_RJ));
	ubicom32_clear_interrupt(PORT_OTHER_INT(IO_PORT_RJ));

	udelay(1000);
}

/*
 * board_usb_deinit
 *	stop the USB controller
 *
 * For IP7K this function is empty since the musb driver attempts to write to
 * the musb device after platform_deinit (!!!).  We'll just use board_usb_reset
 * to clobber the controller on restart.
 */
void board_usb_deinit(void)
{
}

/*
 * board_usb_init
 *	start up the USB controller
 */
int board_usb_init(void)
{
	board_usb_reset();

	/*
	 * Configure the port. Set the thread that will access the port, reset the function,
	 * pull PLL out of reset and deassert function reset
	 */
	struct ubicom32_io_port *io_port = UBICOM32_IO_PORT(IO_PORT_RJ);
	io_port->function =
			IO_FUNC_FUNCTION_RESET(1) | IO_FUNC_FUNCTION_RESET(2) |
			IO_FUNC_FUNCTION_RESET(3) | IO_FUNC_FUNCTION_RESET(4) |
			IO_PORTX_FUNC_USB2_0;
	io_port->ctl0 = (1 << 5) | (1 << 4) | 1;
	udelay(1000);

	/*
	 * Restart (un-reset) the port's USB function.
	 */
	io_port->function &= ~(IO_FUNC_FUNCTION_RESET(IO_PORTX_FUNC_USB2_0));
	io_port->int_clr = (1 << 3);
	io_port->int_mask = (1 << 3);
	ubicom32_enable_interrupt(PORT_OTHER_INT(IO_PORT_RJ));
	return 0;
}

#endif

