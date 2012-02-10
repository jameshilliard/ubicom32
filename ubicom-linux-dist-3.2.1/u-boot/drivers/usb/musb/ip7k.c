/*
 * Ubicom ip7k platform specific USB wrapper functions.
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
#include <asm/io.h>
#include "ip7k.h"

/* MUSB platform configuration */
struct musb_config musb_cfg = {
	(struct	musb_regs *)MENTOR_USB0_BASE,
	IP7K_USB_TIMEOUT,
	0
};

extern void board_usb_deinit(void);
extern int board_usb_init(void);

/*
 * Enable the USB phy
 */
u8 phy_on(void)
{
	if (board_usb_init()) {
		printf("Failed to initialize USB\n");
		return 0;
	}

#ifdef IP7000
	u32 chip_id;
	volatile u32 phy_test = readl(GENERAL_CFG_BASE + GEN_USB_PHY_TEST);
	volatile u32 phy_cfg;
	asm volatile (
		      "move.4	%0, CHIP_ID	\n\t"
		      : "=r" (chip_id)
	);
	if (chip_id == 0x30001) {
		phy_test &= ~(1<<30);
		writel(phy_test, GENERAL_CFG_BASE + GEN_USB_PHY_TEST);
		udelay(1000);
		phy_test &= ~(1<<31);
		writel(phy_test, GENERAL_CFG_BASE + GEN_USB_PHY_TEST);
	} else {
		phy_test &= ~(1<<17);
		writel(phy_test, GENERAL_CFG_BASE + GEN_USB_PHY_TEST);
		udelay(1000);
		phy_test &= ~(1<<14);
		writel(phy_test, GENERAL_CFG_BASE + GEN_USB_PHY_TEST);
	}
#endif
	/* Wait until the USB phy is turned on */
	phy_cfg = readl(GENERAL_CFG_BASE + GEN_USB_PHY_CFG);
	phy_cfg |= ((1 << 14) | (1 <<15));
	writel(phy_cfg, GENERAL_CFG_BASE + GEN_USB_PHY_CFG);
	/* USB phy is on */

	return 1;
}

/*
 * Disable the USB phy
 */
static void phy_off(void)
{
	board_usb_deinit();

	/* Wait until the USB phy is turned on */
	volatile u32 phy_cfg = readl(GENERAL_CFG_BASE + GEN_USB_PHY_CFG);
	phy_cfg &= ~((1 << 14) | (1 <<15));
	writel(phy_cfg, GENERAL_CFG_BASE + GEN_USB_PHY_CFG);
}

static void dump_regs(void) __attribute__((unused));
static void dump_regs(void)
{
	int i;
	for (i=0; i<sizeof(struct musb_regs); i++) {
		printf("offset %d, value %02x \n",i,
		readb(MENTOR_USB0_BASE+i));
	}
}

static void enable_vbus(void) __attribute__((unused));
static void enable_vbus(void)
{
	u8		devctl;
	/* HDRC controls CPEN, but beware current surges during device
	 * connect.  They can trigger transient overcurrent conditions
	 * that must be ignored.
	 */

	devctl = musb_readb(MENTOR_USB0_BASE+offsetof(struct musb_regs,devctl));

	devctl |= MUSB_DEVCTL_SESSION;

	//musb_writeb(MUSB_TEST_FORCE_HOST | MUSB_TEST_FORCE_HS, MENTOR_USB0_BASE+offsetof(struct musb_regs,testmode));
	musb_writeb(devctl, MENTOR_USB0_BASE+offsetof(struct musb_regs,devctl));

#if XDEBUG
	printf("testmode %02x, devctl %02x "
		/* otg %3x conf %08x prcm %08x */ "\n",
		musb_readb(MENTOR_USB0_BASE+offsetof(struct musb_regs,testmode)),
		musb_readb(MENTOR_USB0_BASE+offsetof(struct musb_regs,devctl)));
#endif
}

/*
 * This function performs Davinci platform specific initialization for usb0.
 */
int musb_platform_init(void)
{
	/* start the on-chip USB phy and its pll */
	if (!phy_on())
		return -1;
	udelay(100);

	/* enable USB VBUS */
#if 0
	enable_vbus();
	udelay(100);
#endif

#if XDEBUG
	printf("2 devctl:%x, phy cfg: %x, phy test: %x\n",
		musb_readb(MENTOR_USB0_BASE+offsetof(struct musb_regs,devctl)),
		readl(GENERAL_CFG_BASE + GEN_USB_PHY_CFG),
		readl(GENERAL_CFG_BASE + GEN_USB_PHY_TEST));
#endif
	return 0;
}

/*
 * This function performs Davinci platform specific deinitialization for usb0.
 */
void musb_platform_deinit(void)
{
	/* Turn of the phy */
	phy_off();

	/* flush any interrupts */
	struct ubicom32_io_port *io_port = UBICOM32_IO_PORT(IO_PORT_RJ);
	io_port->int_mask = 0;
	io_port->int_clr = -1;
}
