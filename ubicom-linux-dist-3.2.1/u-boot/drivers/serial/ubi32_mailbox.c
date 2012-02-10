/*
 *   Ubicom32 mailbox driver
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
 */

#include <common.h>
#include <watchdog.h>
#include <asm/ip5000.h>
#include <asm/ubicom32suart.h>

#define TRUE 1
#define FALSE 0

volatile struct ubicom32_isd_mailbox {
    u32_t in;
    u32_t out;
    u32_t status;
} *ubi32_mailbox = (struct ubicom32_isd_mailbox *)ISD_MAILBOX_BASE;

static int num_timeouts = 0;
int mailbox_console_flg = TRUE;

/*
 * ubi32_mailbox_get_char_avail()
 */
static u32_t ubi32_mailbox_get_char_avail(void)
{
    return !(ubi32_mailbox->status & ISD_MAILBOX_STATUS_IN_EMPTY);
}
 
/*
 * ubi32_mailbox_get_char()
 */
static u32_t ubi32_mailbox_get_char(void)
{
	if (mailbox_console_flg == TRUE) {
		/*
		 * Mailbox console is connected.
		 */
		while (ubi32_mailbox->status & ISD_MAILBOX_STATUS_IN_EMPTY) {
			udelay(1000);
			WATCHDOG_RESET();
		}
		return ubi32_mailbox->in & 0xff;
	}

	/*
	 * Mailbox console was not connected.
	 */
	if (ubi32_mailbox->status & ISD_MAILBOX_STATUS_IN_EMPTY) {
		return 0xff;
	}

	/*
	 * Mailbox console is connecting.
	 */
	mailbox_console_flg = TRUE;
	num_timeouts = 0;
	return ubi32_mailbox->in & 0xff;
}

#define MAILBOX_MAX_ATTEMPTS 1000000
#define MAILBOX_MAX_TIMEOUTS 5
/*
 * ubi32_mailbox_put_char()
 */
static void ubi32_mailbox_put_char(char v)
{
	/*
	 * Wait to be able to output.
	 */
	u32_t num_attempts = 0;

	if(mailbox_console_flg == TRUE) {
		while(num_attempts++ < MAILBOX_MAX_ATTEMPTS) {
			if(ubi32_mailbox->status & ISD_MAILBOX_STATUS_OUT_EMPTY) {
				break;
			}
		}

		/*
		 * If timed out more than 5 times on send, mailbox console is disconnected now.
		 */
		if (num_attempts > MAILBOX_MAX_ATTEMPTS) {
			if (num_timeouts++ > MAILBOX_MAX_TIMEOUTS) {
				mailbox_console_flg = FALSE;
			}
		}
	}
	
	asm volatile(
		"pipe_flush 0	\n\t"
		"pipe_flush 0	\n\t"
		"pipe_flush 0	\n\t"
		"pipe_flush 0	\n\t"
		"pipe_flush 0	\n\t"
		"pipe_flush 0	\n\t"
		"pipe_flush 0	\n\t"
	);

	ubi32_mailbox->out = v & 0xff;
}

#if !defined(CONFIG_UBICOM_SERDES_UART)

void serial_setbrg (void)
{
}

int serial_getc (void)
{
	int val = 0;
	val = (int) ubi32_mailbox_get_char();
	return val;
}

int serial_init(void)
{
	mailbox_console_flg = TRUE;
	num_timeouts = 0;

	return 0;
}

void serial_putc (const char c)
{
	/* If \n, also do \r */
	if (c == '\n')
		serial_putc ('\r');

	ubi32_mailbox_put_char(c);
		
}

/*
 * Test whether a character is in the RX buffer
 */
int serial_tstc (void)
{
	return (int) ubi32_mailbox_get_char_avail();
}

void serial_puts (const char *s)
{
	while (*s) {
		serial_putc (*s++);
	}
}

#else

int ubi32_mailbox_getc (void)
{
	int val = 0;
	val = (int) ubi32_mailbox_get_char();
	return val;
}

int ubi32_mailbox_init(void)
{
	mailbox_console_flg = TRUE;
	num_timeouts = 0;

	return 0;
}

void ubi32_mailbox_putc (char c)
{
	/* If \n, also do \r */
	if (c == '\n')
		ubi32_mailbox_putc ('\r');

	ubi32_mailbox_put_char(c);
		
}

/*
 * Test whether a character is in the RX buffer
 */
int ubi32_mailbox_tstc (void)
{
	return (int) ubi32_mailbox_get_char_avail();
}

#endif
