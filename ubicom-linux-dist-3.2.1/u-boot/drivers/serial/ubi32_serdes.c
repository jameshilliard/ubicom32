/*
 *   Ubicom32 On-Chip Serial Driver
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
#include <asm/spinlock.h>
#include <asm/ip5000.h>
#include <asm/ubicom32suart.h>

#define SERIAL_UBICOM_PIN_RXD	(1 << 0)
#define SERIAL_UBICOM_PIN_TXD	(1 << 6)
#define SERIAL_UBICOM_CTL0	0x8b300000
#define SERIAL_UBICOM_CTL1	0x00000009

#define SERIAL_UBICOM_DATA_BIT	8	/* Fixed parameter - do not change */
#define SERIAL_UBICOM_PAR_BIT	0	/* Fixed parameter - do not change */
#define SERIAL_UBICOM_STOP_BIT	1	/* Fixed parameter - do not change */

#define TRUE 1
#define FALSE 0

struct uart_port {
	struct ubi32_spinlock_t lock;			/* port lock */
	unsigned char 		*membase;		/* read/write[bwl] */
	unsigned int		irq;			/* irq number */
	unsigned int		uartclk;		/* base uart clock */
	unsigned int		line;			/* port index */
	unsigned long		flags;
//	unsigned char		*name;
//	void			*private_data;		/* generic platform data pointer */
};

struct uart_port serial_io_port;

extern int mailbox_console_flg;

/* UART name and device definitions */
#if 1
// Port E
#define UBI32_UART_BASE_ADDR 0x02004000
#define UBI32_UART_IRQ 61
#define UBI32_UART_CLOCK 250000000
#define UBI32_UART_PORT_NO 5
#define UBI32_SERDES_NAME	"ttyUS1"

#else
//port D
#define UBI32_UART_BASE_ADDR 0x02003000
#define UBI32_UART_IRQ 60
#define UBI32_UART_CLOCK 240000000
#define UBI32_UART_PORT_NO 4
#define UBI32_SERDES_NAME	"ttyUS0"
#endif

/*
 * Can get overridden by 'serdes=' kernel command line.
 */
static int ubi32_serdes_default_baud_rate = 115200;

#define IO_PORT(port) ((struct ubicom32_io_port *)(port)->membase)
#define IO_PORT_INT_STATUS(port) (IO_PORT(port)->int_status)
#define IO_PORT_INT_MASK(port) (IO_PORT(port)->int_mask)
#define IO_PORT_INT_CLR(port) (IO_PORT(port)->int_clr)

#define ULITE_STATUS_RXVALID IO_PORTX_INT_SERDES_RXBF
#define ULITE_STATUS_OVERRUN 0
#define ULITE_STATUS_FRAME 0
#define ULITE_STATUS_PARITY 0
#define ULITE_STATUS_TXEMPTY IO_PORTX_INT_SERDES_TXBE
#define ULITE_STATUS_TXFULL 0

#define IO_PORTX_INT_SERDES_RX_FIFO_RESET (1 << 30)
#define IO_PORTX_INT_SERDES_RX_FIFO_WM (1 << 12)

/*
 * ubi32_serdes_get_char()
 */
static u8_t ubi32_serdes_get_char(struct ubicom32_io_port *io_port)
{
	/*
	 * Read from hardware (forced 32-bit atomic read).
	 */
	u32_t data = 0;

	if ( io_port ) {
		while(!(io_port->int_status & IO_PORTX_INT_SERDES_RX_FIFO_WM)) {
			udelay(1000);
			WATCHDOG_RESET();
		}
	
		io_port->int_clr = IO_PORTX_INT_SERDES_RXBF;
		asm volatile (
			"move.4		%0, %1		\n\t"
			: "=r" (data)
			: "m" (*(u32_t *)&(io_port->rx_fifo))
			);
	}

	return (u8_t)(data & 0x000000ff);
}

/*
 * ubi32_serdes_put_char()
 */
static void ubi32_serdes_put_char(struct ubicom32_io_port *io_port, u8_t c)
{
	u32_t data = 0x0000fe00 | (c << 1);
	u32_t i;
	
	if ( io_port ) {
		/*
		 * Fixed data format: 
		 * [LSB]1 start bit - 8 data bits - no parity - 1 stop bit[MSB]
		 */
		 
		io_port->int_clr = IO_PORTX_INT_SERDES_TXBE;
		io_port->ctl2 = data;		
		io_port->int_set = IO_PORTX_INT_SERDES_TXBUF_VALID;
		
		for(i = 0; i < 50000; i++){
			if(io_port->int_status & IO_PORTX_INT_SERDES_TXBE)	
				return;
		}

	}
}

static void ubi32_serdes_hw_init(struct uart_port *port, int baud)
{
	struct ubicom32_io_port *io_port = IO_PORT(port);

	if ( io_port ) {
	
		/*
		 * Put port functions 1-4 into reset state.
		 * Function 0 (GPIO) does not need or have a reset bit.
		 * Select SERDES function for restart below.
		 */
		io_port->function = IO_FUNC_FUNCTION_CLK(IO_PORTX_FUNC_SERDES) |
			IO_FUNC_FUNCTION_RESET(1) | IO_FUNC_FUNCTION_RESET(2) |
			IO_FUNC_FUNCTION_RESET(3) | IO_FUNC_FUNCTION_RESET(4) |
			IO_PORTX_FUNC_SERDES;

		/*
		 * Configure SERDES baudrate
		 */
		if ( baud == 0 ) {
			baud = ubi32_serdes_default_baud_rate;
		}

		io_port->ctl0 =
			SERIAL_UBICOM_CTL0 | 
			((port->uartclk / (16 * baud)) - 1);

		io_port->ctl1 =
			SERIAL_UBICOM_CTL1;

		/*
		 * don't interrupt until startup and start_tx
		 */
		io_port->int_mask = 0;
		
		io_port->fifo_watermark = 1;

		/*
		 * Set TXD pin output, RXD input and prevent GPIO
		 * override on the TXD & RXD pins
		 */
		io_port->gpio_ctl &= ~SERIAL_UBICOM_PIN_RXD;
		io_port->gpio_ctl |= SERIAL_UBICOM_PIN_TXD;
		io_port->gpio_mask &= ~(SERIAL_UBICOM_PIN_RXD | SERIAL_UBICOM_PIN_TXD);

		/*o
		 * Restart (un-reset) the port's SERDES function.
		 */
		io_port->function &= ~(IO_FUNC_FUNCTION_RESET(IO_PORTX_FUNC_SERDES));
	}
}

static void ubi32_serdes_init_ports(void)
{
	serial_io_port.uartclk   = UBI32_UART_CLOCK;
	serial_io_port.line      = UBI32_UART_PORT_NO;
	serial_io_port.membase   = (unsigned char *)UBI32_UART_BASE_ADDR;
	serial_io_port.irq       = UBI32_UART_IRQ;
	serial_io_port.flags     = 0;

	ubi32_serdes_hw_init(&serial_io_port, 0);

	printf("Serial: Ubicom32 serdes uart serial driver initialized %p\n", serial_io_port.membase);
}

#if !defined(CONFIG_UBICOM_UART_MAILBOX)

void serial_setbrg(void)
{
	ubi32_serdes_init_ports();
}

int serial_init(void)
{
	ubi32_serdes_init_ports();

	return 0;
}

void serdes_serial_putc(char c)
{
	ubi32_serdes_put_char(IO_PORT(&serial_io_port), c);
}

void serial_putc(char c)
{
	if (c == '\n')
		serial_putc('\r');
	ubi32_serdes_put_char(IO_PORT(&serial_io_port), c);

}


void serial_puts(const char *s)
{
	while (*s)
		serial_putc(*s++);
}

int serial_getc(void)
{
	return ubi32_serdes_get_char(IO_PORT(&serial_io_port));
}

int serial_tstc(void)
{
	return IO_PORT_INT_STATUS(&serial_io_port) & ULITE_STATUS_RXVALID;
}
#else

void serial_setbrg(void)
{
	ubi32_serdes_init_ports();
}

int serial_init(void)
{
	ubi32_serdes_init_ports();

	return 0;
}

void serial_putc(char c)
{
	if (c == '\n')
		serial_putc('\r');

	if(mailbox_console_flg == FALSE) {
	      ubi32_serdes_put_char(IO_PORT(&serial_io_port), c);
	} else {
	      ubi32_mailbox_putc(c);
	}
}

void serial_puts(const char *s)
{
	while (*s)
		serial_putc(*s++);
}

int serial_getc(void)
{
	int val = 0;

	val = ubi32_mailbox_getc();

	if(mailbox_console_flg == FALSE) {
		val = (int) ubi32_serdes_get_char(IO_PORT(&serial_io_port));
	}
	
	return val;
}

int serial_tstc(void)
{
	if(mailbox_console_flg == FALSE) {
		return IO_PORT_INT_STATUS(&serial_io_port) & ULITE_STATUS_RXVALID;
	} else {
		return ubi32_mailbox_tstc();
	}
}

#endif
