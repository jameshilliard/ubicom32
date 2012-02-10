/*
 * drivers/serial/ubi32_mailbox.c
 * 	Ubicom32 On-Chip Mailbox Driver
 *
 * (C) Copyright 2011, Ubicom, Inc.
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

#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/sysrq.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/serial_core.h>
#include <linux/platform_device.h>
	
#define UBI32_MAILBOX_NAME	"ttyUM" 
#define UBI32_MAILBOX_MAJOR	207
#define UBI32_MAILBOX_MINOR	64
#define UBI32_MAILBOX_PORT	1235
#define UBI32_MAILBOX_NR_PORTS	1
#define UBI32_SERIAL_BAUDRATE	115200
#define UBI32_SERIAL_DATA_BIT	8
#define UBI32_MAILBOX_ATTEMPTS	1000000
#define UBI32_MAILBOX_TIMEOUTS	5
#define TRUE	1
#define FALSE	0


struct ubi32_mailbox_port {
	struct uart_port port;
	/*
	 * for future extension
	 */
};
struct ubi32_mailbox_resource {
	int uart_base_addr;
	int uart_irq;
};
struct ubi32_mailbox_resource ubi32_mailbox_resource = {
	ISD_MAILBOX_BASE,
	ISD_MAILBOX_INT
};
struct ubi32_mailbox_reg {
	volatile u32_t in;
	volatile u32_t out;
	volatile u32_t status;
};
struct ubi32_mailbox_port ubi32_mailbox_serial_port;
static volatile struct ubi32_mailbox_reg *ubi32_mailbox = 
		(struct ubi32_mailbox_reg *)ISD_MAILBOX_BASE;
static struct tasklet_struct ubi32_mailbox_tasklet;
static int mailbox_console_flg = TRUE;
static int num_timeouts = 0;

/*
 * ubi32_mailbox_get_char_avail
 *	 This function is used to check if mailbox is empty
 */
static int ubi32_mailbox_get_char_avail(void)
{
	return !(ubi32_mailbox->status & ISD_MAILBOX_STATUS_IN_EMPTY);
}
/* 
 * u32_t ubi32_mailbox_get_char
 * 	This function is used to get char from mailbox
 */
static u32_t ubi32_mailbox_get_char(void)
{
	if (mailbox_console_flg == TRUE) {
		/*
		 * Console is connected.
		 */
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
/*
 * ubi32_mailbox_put_chaR
 * 	This function puts char to mailbox
 */
static void ubi32_mailbox_put_char(u32_t v)
{
	/*
	 * Wait to be able to output.
	 */
	u32_t num_attempts = 0;

	if (mailbox_console_flg == TRUE) {
		while (num_attempts++ < UBI32_MAILBOX_ATTEMPTS) {
			if (ubi32_mailbox->status & ISD_MAILBOX_STATUS_OUT_EMPTY) {
				break;
			}
		}
		/*
	 	 * If timed out more than 5 times, console disconnected
		 */
		if (num_attempts > UBI32_MAILBOX_ATTEMPTS) {
			if (num_timeouts++ > UBI32_MAILBOX_TIMEOUTS) {
				mailbox_console_flg = FALSE;
			}
		}
	}
	ubi32_mailbox->out = v & 0xff;
}

/*
 * ubi32_mailbox_stop_tx
 * 	Required by serial core
 */
static void ubi32_mailbox_stop_tx(struct uart_port *port)
{
}

/*
 * ubi32_mailbox_stop_rx
 * 	Required by serial core
 */
static void ubi32_mailbox_stop_rx(struct uart_port *port)
{
}

/*
 * ubi32_mailbox_rx_chars
 * 	Receive and process data
 */
static void ubi32_mailbox_rx_chars(struct ubi32_mailbox_port *uart)
{
	struct uart_state *state = uart->port.state;
	struct tty_struct *tty = state->port.tty;
	unsigned int ch;

	/*
	 * Between two consecutive readings from inbox, there 
	 * are at least several instructions. So the hardware bug
	 * which results in hang or garbage data when there are 
	 * two consecutive readings from inbox within 3 clock cycles  
	 * will not have effects on this code.
	 * In other words, this code has worked-around the HW bug.
	 */
	while (ubi32_mailbox_get_char_avail()) {
		ch = ubi32_mailbox_get_char();
		if(ch == 0xff) {
			return;
		}
		uart->port.icount.rx++;
		if (uart_handle_sysrq_char(&uart->port, ch)) {
			tty_flip_buffer_push(tty);
			continue;
		}
		tty_insert_flip_char(tty, ch, TTY_NORMAL);
		tty_flip_buffer_push(tty);
	}
}

/*
 * ubi32_mailbox_tx_chars
 * 	Transmit data
 */
static void ubi32_mailbox_tx_chars(struct ubi32_mailbox_port *uart)
{
	struct circ_buf *xmit = &uart->port.state->xmit;

#if defined  CONFIG_SMP
	BUG_ON(!spin_is_locked(&uart->port.lock));
#endif
	if (uart->port.x_char) {
		ubi32_mailbox_put_char(uart->port.x_char);
		uart->port.icount.tx++;
		uart->port.x_char = 0;
		return;
	}
	if (uart_circ_empty(xmit) || uart_tx_stopped(&uart->port)) {
		return;
	}
	while (xmit->tail != xmit->head) {	
		ubi32_mailbox_put_char(xmit->buf[xmit->tail]);
		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		uart->port.icount.tx++;
	}
	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS) {
		uart_write_wakeup(&uart->port);
	}
}

/* 
 * ubi32_mailbox_start_tx
 * 	Start to transmit data
 */
static void ubi32_mailbox_start_tx(struct uart_port *port)
{
	struct ubi32_mailbox_port *uart = (struct ubi32_mailbox_port *)port;

	ubi32_mailbox_tx_chars(uart);
}

/*
 * ubi32_mailbox_rx_tasklet
 * 	Tasklet for irs to process data
 */
void ubi32_mailbox_rx_tasklet(unsigned long data)
{
	struct ubi32_mailbox_port *uart = (struct ubi32_mailbox_port*)data;

	spin_lock(&uart->port.lock);
	ubi32_mailbox_rx_chars(uart);
	spin_unlock(&uart->port.lock);
}

/*
 * irqreturn_t ubi32_mailbox_isr
 * 	Interrupt service routine
 */ 
static irqreturn_t ubi32_mailbox_isr(int irq, void *dev_id)
{
	/*
	 * ubicom32_clear_interrupt(ISD_MAILBOX_INT);
	 * No another interrupt until the data has been read
	 * since the interrupt is tied to writing data and
	 * that cannot happen until the data has been read out.
	 * Also, for mailbox, clearing interrupt
	 * has been handled by kernel code.
	 * So we do not need to clear it here.
	 */
	tasklet_schedule(&ubi32_mailbox_tasklet);
	return IRQ_HANDLED;
}

/*
 * ubi32_mailbox_tx_empty
 * 	Return TIOCSER_TEMT when transmitter is not busy.
 */
static unsigned int ubi32_mailbox_tx_empty(struct uart_port *port)
{
	return TIOCSER_TEMT;
}

/*
 * ubi32_mailbox_get_mctrl
 * 	Required by serial core
 */
static unsigned int ubi32_mailbox_get_mctrl(struct uart_port *port)
{
	return 0;
}

/*
 * ubi32_mailbox_enable_ms
 * 	Required by serial core
 */
static void ubi32_mailbox_enable_ms(struct uart_port *port)
{
}

/*
 * ubi32_mailbox_set_mctrl
 * 	Required by serial core
 */
static void ubi32_mailbox_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
}

/*
 * ubi32_mailbox_break_ctl
 * 	Required by serial core
 */
static void ubi32_mailbox_break_ctl(struct uart_port *port, int break_state)
{
}

/*
 * ubi32_mailbox_startup
 * 	Mailbox start up function
 */
static int ubi32_mailbox_startup(struct uart_port *port)
{
	struct ubi32_mailbox_port *uart = (struct ubi32_mailbox_port *)port;

	if (request_irq(uart->port.irq, ubi32_mailbox_isr,IRQF_DISABLED,
	     "ubi32_mailbox_device", uart)) {
		printk(KERN_NOTICE "Unable to attach Ubicom32 SERDES interrupt\n");
		return -EBUSY;
	}
	return 0;
}

/*
 * ubi32_mailbox_shutdown
 * 	Mailbox shut down function
 */
static void ubi32_mailbox_shutdown(struct uart_port *port)
{
	struct ubi32_mailbox_port *uart = (struct ubi32_mailbox_port *)port;

	free_irq(uart->port.irq, uart);
}

/*
 * ubi32_mailbox_set_termios
 * 	Required by serial core
 */
static void ubi32_mailbox_set_termios(struct uart_port *port, 
				       struct ktermios *termios, struct ktermios *old)
{
}

/*
 * ubi32_mailbox_type
 * 	Report port type
 */
static const char *ubi32_mailbox_type(struct uart_port *port)
{
	struct ubi32_mailbox_port *uart = (struct ubi32_mailbox_port *)port;

	return uart->port.type == UBI32_MAILBOX_PORT ? "ubi32_mailbox_device" : NULL;
}

/*
 * ubi32_mailbox_release_port
 * 	Required by serial core. Release port
 */
static void ubi32_mailbox_release_port(struct uart_port *port)
{
}

/* 
 * ubi32_mailbox_request_port
 * 	Required by serial core. Request port
 */
static int ubi32_mailbox_request_port(struct uart_port *port)
{
	return 0;
}

/*
 * ubi32_mailbox_config_port
 * 	Configure/autoconfigure the port.
 */
static void ubi32_mailbox_config_port(struct uart_port *port, int flags)
{
	struct ubi32_mailbox_port *uart = (struct ubi32_mailbox_port *)port;

	if (flags & UART_CONFIG_TYPE && !(ubi32_mailbox_request_port(&uart->port))) {
		uart->port.type = UBI32_MAILBOX_PORT;
	}
}

/*
 * ubi32_mailbox_verify_port
 * 	Verify port
 */
static int ubi32_mailbox_verify_port(struct uart_port *port, struct serial_struct *ser)
{
	return 0;
}

static struct uart_ops ubi32_mailbox_pops = {
	.tx_empty       = ubi32_mailbox_tx_empty,
	.set_mctrl      = ubi32_mailbox_set_mctrl,
	.get_mctrl      = ubi32_mailbox_get_mctrl,
	.stop_tx        = ubi32_mailbox_stop_tx,
	.start_tx       = ubi32_mailbox_start_tx,
	.stop_rx        = ubi32_mailbox_stop_rx,
	.enable_ms      = ubi32_mailbox_enable_ms,
	.break_ctl      = ubi32_mailbox_break_ctl,
	.startup        = ubi32_mailbox_startup,
	.shutdown       = ubi32_mailbox_shutdown,
	.set_termios    = ubi32_mailbox_set_termios,
	.type           = ubi32_mailbox_type,
	.release_port   = ubi32_mailbox_release_port,
	.request_port   = ubi32_mailbox_request_port,
	.config_port    = ubi32_mailbox_config_port,
	.verify_port    = ubi32_mailbox_verify_port,
};

/*
 * ubi32_mailbox_init_port
 * 	Initialize mailbox port
 */
static void __init ubi32_mailbox_init_port(void)
{
	static int first = 1;

	if (!first)
		return;
	first = 0;

	ubi32_mailbox_serial_port.port.fifosize  = 4;
	ubi32_mailbox_serial_port.port.ops       = &ubi32_mailbox_pops;
	ubi32_mailbox_serial_port.port.line      = 0;
	ubi32_mailbox_serial_port.port.iotype    = UPIO_MEM;
	ubi32_mailbox_serial_port.port.membase   =
		(void __iomem *)ubi32_mailbox_resource.uart_base_addr;
	ubi32_mailbox_serial_port.port.mapbase   =
		ubi32_mailbox_resource.uart_base_addr;
	ubi32_mailbox_serial_port.port.irq       =
		ubi32_mailbox_resource.uart_irq;
	ubi32_mailbox_serial_port.port.flags     = UPF_BOOT_AUTOCONF;
	spin_lock_init(&ubi32_mailbox_serial_port.port.lock);

}

#ifdef CONFIG_SERIAL_UBI32_MAILBOX_CONSOLE
/*
 * ubi32_mailbox_console_get_options
 * 	If the port was already initialized,
 * 	try to determine the current setup.
 */
static void __init ubi32_mailbox_console_get_options(struct ubi32_mailbox_port *uart, 
                                                      int *baud, int *parity, int *bits)
{
	*baud = UBI32_SERIAL_BAUDRATE;
	*parity = 'n';
	*bits = UBI32_SERIAL_DATA_BIT;		
	pr_debug("%s:baud = %d, parity = %c, bits= %d\n", __FUNCTION__, *baud, *parity, *bits);
}
#endif

#if defined CONFIG_SERIAL_UBI32_MAILBOX_CONSOLE
static struct uart_driver ubi32_mailbox_driver;
/*
 * ubi32_mailbox_console_setup
 * 	Set up console
 */
static int __init ubi32_mailbox_console_setup(struct console *co, char *options)
{
	struct ubi32_mailbox_port *uart;
# ifdef CONFIG_SERIAL_UBI32_MAILBOX_CONSOLE
	int baud = UBI32_SERIAL_BAUDRATE;
	int bits = UBI32_SERIAL_DATA_BIT;
	int parity = 'n';
	int flow = 'n';
# endif
	uart = &ubi32_mailbox_serial_port;

# ifdef CONFIG_SERIAL_UBI32_MAILBOX_CONSOLE
	if (options) {
		uart_parse_options(options, &baud, &parity, &bits, &flow);
	} else {
		ubi32_mailbox_console_get_options(uart, &baud, &parity, &bits);
	}
	return uart_set_options(&uart->port, co, baud, parity, bits, flow);
# else
	return 0;
# endif
}
#endif

#ifdef CONFIG_SERIAL_UBI32_MAILBOX_CONSOLE
/*
 * ubi32_mailbox_console_putchar
 * 	Console put char function
 */
static void ubi32_mailbox_console_putchar(struct uart_port *port, int ch)
{
	ubi32_mailbox_put_char(ch);
}

/*
 * ubi32_mailbox_console_write
 * 	Console write function
 */
static void ubi32_mailbox_console_write(struct console *co, const char *s, unsigned int count)
{
	struct ubi32_mailbox_port *uart = &ubi32_mailbox_serial_port;
	unsigned long flags = 0;

	spin_lock_irqsave(&uart->port.lock, flags);
	uart_console_write(&uart->port, s, count, ubi32_mailbox_console_putchar);
	spin_unlock_irqrestore(&uart->port.lock, flags);
}

static struct console ubi32_mailbox_console = {
	.name		= UBI32_MAILBOX_NAME,
	.write		= ubi32_mailbox_console_write,
	.device		= uart_console_device,
	.setup		= ubi32_mailbox_console_setup,
	.flags		= CON_PRINTBUFFER,
	.index		= -1,
	.data		= &ubi32_mailbox_driver,
};

/*
 * ubi32_mailbox_console_init
 * 	Init port and register console
 */
static int __init ubi32_mailbox_console_init(void)
{
	ubi32_mailbox_init_port();
	register_console(&ubi32_mailbox_console);
	return 0;
}
console_initcall(ubi32_mailbox_console_init);

#define UBI32_MAILBOX_CONSOLE	&ubi32_mailbox_console
#else
#define UBI32_MAILBOX_CONSOLE	NULL
#endif  

static struct uart_driver ubi32_mailbox_driver = {
	.owner			= THIS_MODULE,
	.driver_name		= "ubi32_mailbox",
	.dev_name		= UBI32_MAILBOX_NAME,
	.major			= UBI32_MAILBOX_MAJOR,
	.minor			= UBI32_MAILBOX_MINOR,
	.nr			= UBI32_MAILBOX_NR_PORTS,
	.cons			= UBI32_MAILBOX_CONSOLE,
};

/*
 * ubi32_mailbox_suspend
 * 	Suspend mailbox port
 */
static int ubi32_mailbox_suspend(struct platform_device *dev, pm_message_t state)
{
	struct ubi32_mailbox_port *uart = platform_get_drvdata(dev);

	if (uart) {
		uart_suspend_port(&ubi32_mailbox_driver, &uart->port);
	}
	return 0;
}

/*
 * ubi32_mailbox_resume
 * 	Resume mailbox port
 */
static int ubi32_mailbox_resume(struct platform_device *dev)
{
	struct ubi32_mailbox_port *uart = platform_get_drvdata(dev);

	if (uart) {
		uart_resume_port(&ubi32_mailbox_driver, &uart->port);
	}
	return 0;
}

/*
 * ubi32_mailbox_probe
 * 	Probe and add mailbox port
 */
static int ubi32_mailbox_probe(struct platform_device *dev)
{
	struct resource *res = dev->resource;
	int i;

	for (i = 0; i < dev->num_resources; i++, res++) {
		if (res->flags & IORESOURCE_MEM) {
			break;
                }
        }
	if (i < dev->num_resources) {
		if (ubi32_mailbox_serial_port.port.mapbase == res->start) {
			ubi32_mailbox_serial_port.port.dev = &dev->dev;
			uart_add_one_port(&ubi32_mailbox_driver, &ubi32_mailbox_serial_port.port);
			platform_set_drvdata(dev, &ubi32_mailbox_serial_port);
		}
	}
	return 0;
}

/*
 * ubi32_mailbox_remove
 * 	Remove mailbox port
 */
static int ubi32_mailbox_remove(struct platform_device *pdev)
{
	struct ubi32_mailbox_port *uart = platform_get_drvdata(pdev);

	platform_set_drvdata(pdev, NULL);

	if (uart) {
		uart_remove_one_port(&ubi32_mailbox_driver, &uart->port);
        }
	return 0;
}

static struct platform_driver ubi32_mailbox_platform_driver = {
	.probe		= ubi32_mailbox_probe,
	.remove		= ubi32_mailbox_remove,
	.suspend	= ubi32_mailbox_suspend,
	.resume		= ubi32_mailbox_resume,
	.driver		= {
		.name	= "ubi32-mbox",
		.owner	= THIS_MODULE,
	},
};

/*
 * ubi32_mailbox_init
 * 	Mailbox init function
 */
static int __init ubi32_mailbox_init(void)
{
	int ret;

	pr_info("Serial: Ubicom32 mailbox serial driver.\n");
	mailbox_console_flg = TRUE;
	num_timeouts = 0;
	ubi32_mailbox_init_port();
	ret = uart_register_driver(&ubi32_mailbox_driver);
	if (ret == 0) {
		ret = platform_driver_register(&ubi32_mailbox_platform_driver);
		if (ret) {
			pr_debug("uart register failed\n");
			uart_unregister_driver(&ubi32_mailbox_driver);
		}
	}
	uart_add_one_port(&ubi32_mailbox_driver, &ubi32_mailbox_serial_port.port);
	tasklet_init(&ubi32_mailbox_tasklet, ubi32_mailbox_rx_tasklet, 
			(unsigned long)&ubi32_mailbox_serial_port);
	return ret;
}

/*
 * ubi32_mailbox_exit
 * 	Mailbox exit function
 */
static void __exit ubi32_mailbox_exit(void)
{
	platform_driver_unregister(&ubi32_mailbox_platform_driver);
	uart_unregister_driver(&ubi32_mailbox_driver);
}

module_init(ubi32_mailbox_init);
module_exit(ubi32_mailbox_exit);

MODULE_ALIAS_CHARDEV_MAJOR(UBI32_MAILBOX_MAJOR);
MODULE_ALIAS("platform:ubi32_mailbox");
