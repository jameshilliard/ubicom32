/*
 * driver/usb/host/xhci-ubi32.c
 *   Ubicom32 architecture platform driver for xhci.
 *
 * (C) Copyright 2010, Ubicom, Inc.
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
#include <linux/dma-mapping.h>
#include <linux/gpio.h>
#include <linux/kobject.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <asm/cacheflush.h>
#include <asm/ubicom32.h>

#include "xhci.h"

static const char hcd_name[] = "xhci_hcd";

/*
 * Flush the buf out of cache
 */
void ubi32_flush(void *buf, unsigned int size)
{
        flush_dcache_range((u32)buf, (u32)buf + size);
}
EXPORT_SYMBOL(ubi32_flush);

/*
 * Invalidate cache for the buf
 */
void ubi32_invalidate(void *buf, unsigned int size)
{
	//flush_dcache_range((u32)buf, (u32)buf + size);
	mem_d_cache_control((u32)buf, (u32)buf + size, CCR_CTRL_INV_ADDR);
}
EXPORT_SYMBOL(ubi32_invalidate);

static void ubi32_vbus_reset(struct xhci_hcd *xhci)
{
	xhci_dbg(xhci, "vbus gpio 0x%x\n", UBI32_XHCI_VBUS_RESET_PIN);
	/* Set reset_l bit to 0 */
	if (gpio_request(UBI32_XHCI_VBUS_RESET_PIN & 0xffff, "ubi32_xhci_vbus_reset")) {
		xhci_dbg(xhci, "Can't request  ubi32_xhci vbus reset pin 0x%08x\n", UBI32_XHCI_VBUS_RESET_PIN);
	}

	gpio_direction_output(UBI32_XHCI_VBUS_RESET_PIN & 0xffff, 1);
	mdelay(600);

	/* Set reset_l bit to 1 */
	gpio_direction_output(UBI32_XHCI_VBUS_RESET_PIN & 0xffff, 0);

	xhci_dbg(xhci, "%s gpio set vbus is done\n", __func__);
}

/* called during probe() after chip reset completes */
static int ubi32_xhci_drv_setup(struct usb_hcd *hcd)
{
	struct xhci_hcd		*xhci;
	int			retval;
	u32			temp;

	hcd->self.sg_tablesize = TRBS_PER_SEGMENT - 2;

	if (usb_hcd_is_primary_hcd(hcd)) {
		xhci = kzalloc(sizeof(struct xhci_hcd), GFP_KERNEL);
		if (!xhci)
			return -ENOMEM;
		*((struct xhci_hcd **) hcd->hcd_priv) = xhci;
		xhci->main_hcd = hcd;
		/* Mark the first roothub as being USB 2.0.
		 * The xHCI driver will register the USB 3.0 roothub.
		 */
		hcd->speed = HCD_USB2;
		hcd->self.root_hub->speed = USB_SPEED_HIGH;
		/*
		 * USB 2.0 roothub under xHCI has an integrated TT,
		 * (rate matching hub) as opposed to having an OHCI/UHCI
		 * companion controller.
		 */
		hcd->has_tt = 1;
	} else {
		/* xHCI private pointer was set in xhci_pci_probe for the second
		 * registered roothub.
		 */
		xhci = hcd_to_xhci(hcd);
		temp = xhci_readl(xhci, &xhci->cap_regs->hcc_params);
		if (HCC_64BIT_ADDR(temp)) {
			xhci_dbg(xhci, "Enabling 64-bit DMA addresses.\n");
			dma_set_mask(hcd->self.controller, DMA_BIT_MASK(64));
		} else {
			dma_set_mask(hcd->self.controller, DMA_BIT_MASK(32));
		}
		return 0;
	}

	xhci->cap_regs = hcd->regs;
	xhci->op_regs = hcd->regs +
		HC_LENGTH(xhci_readl(xhci, &xhci->cap_regs->hc_capbase));
	xhci->run_regs = hcd->regs +
		(xhci_readl(xhci, &xhci->cap_regs->run_regs_off) & RTSOFF_MASK);
	/* Cache read-only capability registers */
	xhci->hcs_params1 = xhci_readl(xhci, &xhci->cap_regs->hcs_params1);
	xhci->hcs_params2 = xhci_readl(xhci, &xhci->cap_regs->hcs_params2);
	xhci->hcs_params3 = xhci_readl(xhci, &xhci->cap_regs->hcs_params3);
	xhci->hcc_params = xhci_readl(xhci, &xhci->cap_regs->hc_capbase);
	xhci->hci_version = HC_VERSION(xhci->hcc_params);
	xhci->hcc_params = xhci_readl(xhci, &xhci->cap_regs->hcc_params);
	xhci_print_registers(xhci);

	/* Make sure the HC is halted. */
	retval = xhci_halt(xhci);
	if (retval)
		goto error;

	xhci_dbg(xhci, "Resetting HCD\n");
	/* Reset the internal HC memory state and registers. */
	retval = xhci_reset(xhci);
	if (retval)
		goto error;
	xhci_dbg(xhci, "Reset complete\n");

	temp = xhci_readl(xhci, &xhci->cap_regs->hcc_params);
	if (HCC_64BIT_ADDR(temp)) {
		xhci_dbg(xhci, "Enabling 64-bit DMA addresses.\n");
		dma_set_mask(hcd->self.controller, DMA_BIT_MASK(64));
	} else {
		dma_set_mask(hcd->self.controller, DMA_BIT_MASK(32));
	}

	xhci_dbg(xhci, "Calling HCD init\n");
	/* Initialize HCD and host controller data structures. */
	retval = xhci_init(hcd);
	if (retval)
		goto error;
	xhci_dbg(xhci, "Called HCD init\n");

	if (!retval)
		return retval;
error:
	kfree(xhci);
	return retval;
}


#ifdef CONFIG_UBI32_SS_USB2_MODE
int ubi32_xhci_shutdown_hcd_io(struct usb_hcd *hcd);
void ubi32_xhci_reset_hcd_and_io(struct usb_hcd *hcd);
int ubi32_xhci_hcd_is_reset(struct usb_hcd *hcd);
#endif

static const struct hc_driver ubi32_h_driver = {
	.description =		hcd_name,
	.product_desc =		"xHCI Host Controller",
	.hcd_priv_size =	sizeof(struct xhci_hcd *),

	/*
	 * generic hardware linkage
	 */
	.irq =			xhci_irq,
	.flags =		HCD_MEMORY | HCD_USB3 | HCD_SHARED,

	/*
	 * basic lifecycle operations
	 */
	.reset =		ubi32_xhci_drv_setup,
	.start =		xhci_run,
	.stop =			xhci_stop,
	.shutdown =		xhci_shutdown,

	/*
	 * managing i/o requests and associated device resources
	 */
	.urb_enqueue =		xhci_urb_enqueue,
	.urb_dequeue =		xhci_urb_dequeue,
	.alloc_dev =		xhci_alloc_dev,
	.free_dev =		xhci_free_dev,
	.alloc_streams =	xhci_alloc_streams,
	.free_streams =		xhci_free_streams,
	.add_endpoint =		xhci_add_endpoint,
	.drop_endpoint =	xhci_drop_endpoint,
	.endpoint_reset =	xhci_endpoint_reset,
	.check_bandwidth =	xhci_check_bandwidth,
	.reset_bandwidth =	xhci_reset_bandwidth,
	.address_device =	xhci_address_device,
	.update_hub_device =	xhci_update_hub_device,
	.reset_device =		xhci_discover_or_reset_device,

	/*
	 * scheduling support
	 */
	.get_frame_number =	xhci_get_frame,

	/* Root hub support */
	.hub_control =		xhci_hub_control,
	.hub_status_data =	xhci_hub_status_data,
	.bus_suspend =		xhci_bus_suspend,
	.bus_resume =		xhci_bus_resume,
#ifdef CONFIG_UBI32_SS_USB2_MODE
	.shutdown_hcd_io =	ubi32_xhci_shutdown_hcd_io,
	.reset_hcd_and_io =	ubi32_xhci_reset_hcd_and_io,
	.hcd_is_reset =		ubi32_xhci_hcd_is_reset,
#endif
};

static int ubi32_xhci_drv_probe(struct platform_device *pdev)
{
	struct xhci_hcd *xhci;
	struct usb_hcd *hcd;
	struct device	*dev = &pdev->dev;
	int irq = platform_get_irq(pdev, 0);
	struct resource	*iomem;
	int ret;

	printk(KERN_DEBUG "%s\n", __func__);
	if (usb_disabled())
		return -ENODEV;

	iomem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!iomem || irq == 0) {
		ret = -ENODEV;
		goto init_fail;
	}

	hcd = usb_create_hcd(&ubi32_h_driver, dev, dev_name(dev));
	if (!hcd) {
		dev_err(dev, "create hcd %s fail\n", dev_name(dev));
		ret = -ENOMEM;
		goto init_fail;
	}

	hcd->rsrc_start = iomem->start;
	hcd->rsrc_len = resource_size(iomem);

	hcd->regs = ioremap(iomem->start, resource_size(iomem));
	if (!hcd->regs) {
		dev_err(dev, "ioremap failed\n");
		ret = -EFAULT;
		goto put_hcd;
	}

	ret = usb_add_hcd(hcd, irq, IRQF_DISABLED | IRQF_SHARED);
	if (ret != 0) {
		dev_err(dev, "add hcd failed\n");
		goto unmap_registers;
	}

	platform_set_drvdata(pdev, hcd);

	xhci = hcd_to_xhci(hcd);
#ifdef CONFIG_UBI32_SS_USB2_MODE
	xhci->usb3_io_reset = 0;
#endif
	xhci->shared_hcd = usb_create_shared_hcd(&ubi32_h_driver, dev,
				dev_name(dev), hcd);
	if (!xhci->shared_hcd) {
		dev_err(dev, "create shared_hcd failed\n");
		ret = -ENOMEM;
		goto unmap_registers;
	}

	/* Set the xHCI pointer before ubi32_xhci_setup() (aka hcd_driver.reset)
	 * is called by usb_add_hcd().
	 */
	*((struct xhci_hcd **) xhci->shared_hcd->hcd_priv) = xhci;

	ret = usb_add_hcd(xhci->shared_hcd, irq,
			IRQF_DISABLED | IRQF_SHARED);
	if (ret)
		goto put_usb3_hcd;

	/* Reset vbus to make cold plug work */
	ubi32_vbus_reset(xhci);

	/* Roothub already marked as USB 3.0 speed */
	return 0;

put_usb3_hcd:
	usb_put_hcd(xhci->shared_hcd);
unmap_registers:
	iounmap(hcd->regs);
put_hcd:
	usb_put_hcd(hcd);
init_fail:
	dev_err(dev, "init %s fail, %d\n", dev_name(dev), ret);
	return ret;
}

static int ubi32_xhci_drv_remove(struct platform_device *pdev)
{
	struct xhci_hcd *xhci;
	struct usb_hcd *hcd = platform_get_drvdata(pdev);

	printk(KERN_DEBUG "%s\n", __func__);
	xhci = hcd_to_xhci(hcd);

	if (xhci->shared_hcd) {
		usb_remove_hcd(xhci->shared_hcd);
		usb_put_hcd(xhci->shared_hcd);
	}
	usb_remove_hcd(hcd);
	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
	usb_put_hcd(hcd);
	platform_set_drvdata(pdev, NULL);
	kfree(xhci);

	return 0;
}

#ifdef CONFIG_PM
static int ubi32_xhci_drv_suspend(struct device *dev)
{
	struct usb_hcd *hcd = dev_get_drvdata(dev);
	struct xhci_hcd	*xhci = hcd_to_xhci(hcd);
	int	retval = 0;

	if (hcd->state != HC_STATE_SUSPENDED ||
			xhci->shared_hcd->state != HC_STATE_SUSPENDED)
		return -EINVAL;

	retval = xhci_suspend(xhci);

	return retval;
}

static int ubi32_xhci_drv_resume(struct device *dev)
{
	struct usb_hcd		*hcd = dev_get_drvdata(dev);
	struct xhci_hcd		*xhci = hcd_to_xhci(hcd);
	int			retval = 0;

	retval = xhci_resume(xhci, 0);
	return retval;
}

static const struct dev_pm_ops ubi32_xhci_dev_pm_ops = {
	.suspend	= ubi32_xhci_drv_suspend,
	.resume		= ubi32_xhci_drv_resume,
};

#define UBI32_XHCI_DEV_PM_OPS	(&ubi32_xhci_dev_pm_ops)
#else
#define	UBI32_XHCI_DEV_PM_OPS	NULL
#endif /* CONFIG_PM */

static struct platform_driver ubi32_xhci_driver = {
	.driver = {
		.name		= "ubi32_xhci_ctl",
		.bus		= &platform_bus_type,
		.owner		= THIS_MODULE,
		.pm		= UBI32_XHCI_DEV_PM_OPS,
	},
	.probe = ubi32_xhci_drv_probe,
	.remove = ubi32_xhci_drv_remove,
	.shutdown = usb_hcd_platform_shutdown,
};

int ubi32_xhci_register_drv(void)
{
	return platform_driver_register(&ubi32_xhci_driver);
}

void ubi32_xhci_unregister_drv(void)
{
	platform_driver_unregister(&ubi32_xhci_driver);
}

#ifdef CONFIG_UBI32_SS_USB2_MODE

int ubi32_xhci_shutdown_hcd_io(struct usb_hcd *hcd)
{
	struct xhci_hcd *xhci = hcd_to_xhci(hcd);

	printk(KERN_DEBUG "%s\n", __func__);
	xhci_halt(xhci);
	xhci_reset(xhci);
	ubi32_xhci_port_shutdown();
	xhci->usb3_io_reset = 1;

	ubi32_vbus_reset(xhci);

	return 0;
}

void ubi32_xhci_reset_hcd_and_io(struct usb_hcd *hcd)
{
	printk(KERN_DEBUG "%s\n", __func__);
	ubi32_xhci_unregister_drv();
	ubi32_xhci_port_init();
	ubi32_xhci_register_drv();
}

int ubi32_xhci_hcd_is_reset(struct usb_hcd *hcd)
{
	struct xhci_hcd *xhci = hcd_to_xhci(hcd);

	return xhci->usb3_io_reset;
}

#endif
