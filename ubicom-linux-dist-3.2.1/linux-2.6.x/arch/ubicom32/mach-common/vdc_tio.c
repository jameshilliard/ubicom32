/*
 * arch/ubicom32/mach-common/vdc_tio.c
 *   Generic initialization for VDC
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

#include <linux/platform_device.h>
#include <linux/types.h>

#include <asm/devtree.h>
#include <asm/vdc_tio.h>
#include <asm/ubicom32fb.h>

#include "vdc_tio_at070tn93.h"
#include "vdc_tio_at080tn42.h"
#include "vdc_tio_cfaf240320kt.h"
#include "vdc_tio_cfaf240400dt.h"
#include "vdc_tio_cfaf320240f.h"
#include "vdc_tio_lb070wv6.h"
#include "vdc_tio_tft1n5046e.h"
#include "vdc_tio_tft1n5046e_hv.h"
#include "vdc_tio_hx8347.h"

/*
 * Resources that this driver uses
 */
static struct resource __initdata vdc_tio_resources[] = {
	/*
	 * Send IRQ
	 */
	[0] = {
		/*
		 * The init routine will query the devtree and fill this in
		 */
		.flags	= IORESOURCE_IRQ,
	},

	/*
	 * Receive IRQ (optional)
	 */
	[1] = {
		/*
		 * The init routine will query the devtree and fill this in
		 */
		.flags	= IORESOURCE_IRQ,
	},

	/*
	 * Memory Mapped Registers
	 */
	[2] = {
		/*
		 * The init routine will query the devtree and fill this in
		 */
		.flags	= IORESOURCE_MEM,
	},
};


/*
 * The platform_device structure which is passed to the driver
 */
static struct platform_device vdc_tio_platform_device = {
	.name		= "ubicom32fb",
	.id		= -1,
	.resource	= vdc_tio_resources,
	.num_resources	= ARRAY_SIZE(vdc_tio_resources),
};

/*
 * vdc_tio_fill_platform_data
 *	Fills in relevant information into ubicom32fb_platform_data
 *	based on the selected panel type.
 *
 * Board files should call this function and then add or change
 * any board specific settings as needed.
 */
void __init vdc_tio_fill_platform_data(struct ubicom32fb_platform_data *pd)
{
	memset(pd, 0, sizeof(struct ubicom32fb_platform_data));

#ifdef VDC_TIO_LCD_PLATFORM_DATA
	printk(KERN_INFO "Load VDC Platform data: " VDC_TIO_LCD_PLATFORM_DATA "\n");
	memcpy(pd, &vdc_tio_ubicom32fb_platform_data, sizeof(struct ubicom32fb_platform_data));
#endif
}

/*
 * vdc_tio_init
 *	Checks the device tree and instantiates the driver if found
 */
void __init vdc_tio_init(struct ubicom32fb_platform_data *pd)
{
	/*
	 * Check the device tree for the vdc_tio
	 */
	struct vdc_tio_node *vdc_node =
		(struct vdc_tio_node *)devtree_find_node("vdctio");
	if (!vdc_node) {
		printk(KERN_WARNING "No vdc_tio found\n");
		return;
	}

	vdc_tio_platform_device.dev.platform_data = pd;

	/*
	 * Fill in the resources and platform data from devtree information
	 */
	vdc_tio_resources[0].start = vdc_node->dn.sendirq;
	vdc_tio_resources[1].start = vdc_node->dn.recvirq;
	vdc_tio_resources[2].start = (u32_t)vdc_node->regs;
	vdc_tio_resources[2].end = (u32_t)vdc_node->regs +
		sizeof(struct vdc_tio_vp_regs);

#ifdef CONFIG_VDC_LCD_AUTO_REFRESH_RATE
	pd->fclk = CONFIG_VDC_LCD_AUTO_REFRESH_RATE;
#endif

	/*
	 * Try to get the device registered
	 */
	if (platform_device_register(&vdc_tio_platform_device) < 0) {
		printk(KERN_WARNING "VDC failed to register\n");
	}
}

