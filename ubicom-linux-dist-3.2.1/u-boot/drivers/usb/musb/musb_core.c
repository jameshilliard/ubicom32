/*
 * Mentor USB OTG Core functionality common for both Host and Device
 * functionality.
 *
 * Copyright (c) 2008 Texas Instruments
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Author: Thomas Abraham t-abraham@ti.com, Texas Instruments
 */

#include <common.h>

#include "musb_core.h"
struct musb_regs *musbr;

/*
 * program the mentor core to start (enable interrupts, dma, etc.)
 */
void musb_start(void)
{
	u8 devctl;

	/* disable all interrupts */
	musb_writew(0, &musbr->intrtxe);
	musb_writew(0, &musbr->intrrxe);
	musb_writeb(0, &musbr->intrusbe);
	musb_writeb(0, &musbr->testmode);

	/* put into basic highspeed mode and start session */
	musb_writeb(MUSB_POWER_HSENAB, &musbr->power);

#if defined(CONFIG_MUSB_HCD)
	devctl = musb_readb(&musbr->devctl);
	musb_writeb(devctl | MUSB_DEVCTL_SESSION, &musbr->devctl);
#endif
}

/*
 * This function configures the endpoint configuration. The musb hcd or musb
 * device implementation can use this function to configure the endpoints
 * and set the FIFO sizes. Note: The summation of FIFO sizes of all endpoints
 * should not be more than the available FIFO size.
 *
 * epinfo	- Pointer to EP configuration table
 * cnt		- Number of entries in the EP conf table.
 */
void musb_configure_ep(struct musb_epinfo *epinfo, u8 cnt)
{
#if !defined(__UBICOM32__)
	u16 csr;
	u16 fifoaddr = 64; /* First 64 bytes of FIFO reserved for EP0 */
	u32 fifosize;
	u8  idx;

	while (cnt--) {
		/* prepare fifosize to write to register */
		fifosize = epinfo->epsize >> 3;
		idx = ffs(fifosize) - 1;

		musb_writeb(epinfo->epnum, &musbr->index);
		if (epinfo->epdir) {
			/* Configure fifo size and fifo base address */
			musb_writeb(idx, &musbr->txfifosz);
			musb_writew(fifoaddr >> 3, &musbr->txfifoadd);
#if defined(CONFIG_MUSB_HCD)
			/* clear the data toggle bit */
			csr = musb_readw(&musbr->txcsr);
			musb_writew(csr | MUSB_TXCSR_CLRDATATOG, &musbr->txcsr);
#endif
			/* Flush fifo if required */
			if (csr & MUSB_TXCSR_TXPKTRDY)
				musb_writew(csr | MUSB_TXCSR_FLUSHFIFO,
					&musbr->txcsr);
		} else {
			/* Configure fifo size and fifo base address */
			musb_writeb(idx, &musbr->rxfifosz);
			musb_writew(fifoaddr >> 3, &musbr->rxfifoadd);
#if defined(CONFIG_MUSB_HCD)
			/* clear the data toggle bit */
			csr = musb_readw(&musbr->rxcsr);
			musb_writew(csr | MUSB_RXCSR_CLRDATATOG, &musbr->rxcsr);
#endif
			/* Flush fifo if required */
			if (csr & MUSB_RXCSR_RXPKTRDY)
				musb_writew(csr | MUSB_RXCSR_FLUSHFIFO,
					&musbr->rxcsr);
		}
		fifoaddr += epinfo->epsize;
		epinfo++;
	}
#endif // !defined(__UBICOM32__)
}

/*
 * This function writes data to endpoint fifo
 *
 * ep		- endpoint number
 * length	- number of bytes to write to FIFO
 * fifo_data	- Pointer to data buffer that contains the data to write
 */
void write_fifo(u8 ep, u32 length, void *fifo_data)
{
	u8  *data = (u8 *)fifo_data;
#if defined(__UBICOM32__)
	u32 data32;
#endif // defined(__UBICOM32__)

	/* select the endpoint index */
	musb_writeb(ep, &musbr->index);

	/* write the data to the fifo */
#if defined(__UBICOM32__)
	while (length>3) {
		memcpy(&data32, data, 4);
		writel(data32, &musbr->fifox[ep]);
		data+=4;
		length-=4;
	}
#endif // defined(__UBICOM32__)
	while (length--)
		musb_writeb(*data++, &musbr->fifox[ep]);
}

/*
 * This function reads data from endpoint fifo
 *
 * ep           - endpoint number
 * length       - number of bytes to read from FIFO
 * fifo_data    - pointer to data buffer into which data is read
 */
void read_fifo(u8 ep, u32 length, void *fifo_data)
{
	u8  *data = (u8 *)fifo_data;
#if defined(__UBICOM32__)
	u32 data32;
#endif // defined(__UBICOM32__)

	/* select the endpoint index */
	musb_writeb(ep, &musbr->index);

	/* read the data from the fifo */
#if defined(__UBICOM32__)
	while (length>3) {
		data32 = readl(&musbr->fifox[ep]);
		memcpy(data, &data32, 4);
		data+=4;
		length-=4;
	}
#endif // defined(__UBICOM32__)
	while (length--)
		*data++ = musb_readb(&musbr->fifox[ep]);
}
