/*
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
 *
 */

/* ==========================================================================
 * $File: //dwh/usb_iip/dev/software/otg/linux/drivers/dwc_otg_hcd_linux.c $
 * $Revision: #11 $
 * $Date: 2009/04/21 $
 * $Change: 1237476 $
 *
 * Synopsys HS OTG Linux Software Driver and documentation (hereinafter,
 * "Software") is an Unsupported proprietary work of Synopsys, Inc. unless
 * otherwise expressly agreed to in writing between Synopsys and you.
 *
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto. You are permitted to use and
 * redistribute this Software in source and binary forms, with or without
 * modification, provided that redistributions of source code must retain this
 * notice. You may not view, use, disclose, copy or distribute this file or
 * any information contained herein except pursuant to this license grant from
 * Synopsys. If you do not agree with this notice, including the disclaimer
 * below, then you are not authorized to use the Software.
 *
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * ========================================================================== */
#ifndef DWC_DEVICE_ONLY

#include <common.h>
#include <asm/byteorder.h>
#include <usb.h>
#include "dwc_otg_hcd_if.h"
#include "dwc_otg_hcd.h"
#include "dwc_otg_dbg.h"
#include "dwc_otg_driver.h"
#include "usb_dwc.h"

/**
 * Gets the endpoint number from a _bEndpointAddress argument. The endpoint is
 * qualified with its direction (possible 32 endpoints per device).
 */
#define dwc_ep_addr_to_endpoint(_bEndpointAddress_) ((_bEndpointAddress_ & USB_ENDPOINT_NUMBER_MASK) | \
						     ((_bEndpointAddress_ & USB_DIR_IN) != 0) << 4)

static const char dwc_otg_hcd_name[] = "dwc_otg_hcd";

static dwc_otg_hcd_t *dwc_otg_hcd;

static int32_t dwc_otg_hcd_irq(dwc_otg_hcd_t * hcd);

static int _disconnect(dwc_otg_hcd_t * hcd)
{
	return 0;
}

static int _start(dwc_otg_hcd_t * hcd)
{
	return 0;
}

static int _hub_info(dwc_otg_hcd_t * hcd, void *urb_handle, uint32_t * hub_addr,
		     uint32_t * port_addr)
{
	DWC_DEBUGPL(DBG_HCDV, "%s(%p)\n", __func__, hcd);
	*hub_addr = 0;
	*port_addr = 0;
	return 0;
}

static int _speed(dwc_otg_hcd_t * hcd, void *urb_handle)
{
	struct usb_device *dev = (struct usb_device *)urb_handle;
	return dev->speed;
}

static int _get_b_hnp_enable(dwc_otg_hcd_t * hcd)
{
	return 0;
}

/**
 * Sets the final status of an URB and returns it to the device driver. Any
 * required cleanup of the URB is performed.
 */
static int _complete(dwc_otg_hcd_t * hcd, void *urb_handle,
		     dwc_otg_hcd_urb_t * dwc_otg_urb, int32_t status)
{
	struct usb_device *dev = (struct usb_device *)urb_handle;
#ifdef CONFIG_DWC_DEBUG
	if (CHK_DEBUG_LEVEL(DBG_HCDV | DBG_HCD_URB)) {
		DWC_PRINTF("%s: otg urb %p, device %d, ep %d %s, status=%d\n",
			   __func__, dwc_otg_urb, dwc_otg_urb->pipe_info.dev_addr,
			   dwc_otg_urb->pipe_info.ep_num,
			   dwc_otg_urb->pipe_info.pipe_dir ? "IN" : "OUT", status);
	}
#endif

	dev->act_len = dwc_otg_urb->actual_length;
	/* Convert status value. */
	switch (status) {
	case -DWC_E_PROTOCOL:
	case -DWC_E_IN_PROGRESS:
	case -DWC_E_PIPE:
	case -DWC_E_IO:
	case -DWC_E_TIMEOUT:
	case -DWC_E_OVERFLOW:
		status = -1;
		break;
	default:
		if (status) {
			DWC_PRINTF("Uknown urb status %d\n", status);

		}
	}
	
	dev->status = status;
	dwc_otg_urb->finished = 1;

	return 0;
}

static struct dwc_otg_hcd_function_ops hcd_fops = {
	.start = _start,
	.disconnect = _disconnect,
	.hub_info = _hub_info,
	.speed = _speed,
	.complete = _complete,
	.get_b_hnp_enable = _get_b_hnp_enable,
};

/**
 * Initializes the HCD. This function allocates memory for and initializes the
 * static parts of the usb_hcd and dwc_otg_hcd structures. It also registers the
 * USB bus with the core and calls the hc_driver->start() function. It returns
 * a negative error on failure.
 */
int hcd_init(
	struct dwc_device *_dev
	)
{
	dwc_otg_device_t *otg_dev = dwc_get_drvdata(_dev);
	int retval = -1;

	DWC_DEBUGPL(DBG_HCD, "DWC OTG HCD INIT\n");

	/*
	 * Allocate memory for the base HCD plus the DWC OTG HCD.
	 * Initialize the base HCD.
	 */
	DWC_DEBUGPL(DBG_HCD, "DWC OTG HCD usb_create_hcd\n");

	/* Initialize the DWC OTG HCD. */
	dwc_otg_hcd = dwc_otg_hcd_alloc_hcd();
	if (!dwc_otg_hcd) {
		goto error;
	}
	otg_dev->hcd = dwc_otg_hcd;

	dwc_otg_hcd_set_priv_data(dwc_otg_hcd, otg_dev);

	dwc_otg_hcd->root_hub = 0;

	if (dwc_otg_hcd_init(dwc_otg_hcd, otg_dev->core_if)) {
		goto error;
	}

	if (dwc_otg_hcd_start(dwc_otg_hcd, &hcd_fops)) {
		goto error;	
	}

	DWC_DEBUGPL(DBG_HCD, "DWC OTG HCD irq=0x%08X \n", _dev->irq );
	return 0;

      error:
	return retval;
}

/**
 * Removes the HCD.
 * Frees memory and resources associated with the HCD and deregisters the bus.
 */
void hcd_remove(
	struct dwc_device *_dev
	)
{
	dwc_otg_device_t *otg_dev = dwc_get_drvdata(_dev);

	DWC_DEBUGPL(DBG_HCD, "DWC OTG HCD REMOVE\n");

	if (!otg_dev) {
		DWC_DEBUGPL(DBG_ANY, "%s: otg_dev NULL!\n", __func__);
		return;
	}

	dwc_otg_hcd = otg_dev->hcd;

	if (!dwc_otg_hcd) {
		DWC_DEBUGPL(DBG_ANY, "%s: otg_dev->hcd NULL!\n", __func__);
		return;
	}

	dwc_otg_hcd_remove(dwc_otg_hcd);
}

#ifdef CONFIG_DWC_DEBUG
static void dump_urb_info(dwc_otg_hcd_urb_t *dwc_otg_urb, char *fn_name)
{
	dwc_otg_hcd_pipe_info_t *pipe_info = &dwc_otg_urb->pipe_info;
	struct usb_device *dev = (struct usb_device *)dwc_otg_urb->priv;
	DWC_PRINTF("%s, urb %p\n", fn_name, dwc_otg_urb);
	DWC_PRINTF("  Device address: %d\n", pipe_info->dev_addr);
	DWC_PRINTF("  Endpoint: %d, %s\n", pipe_info->ep_num,
		   pipe_info->pipe_dir ? "IN" : "OUT");
	DWC_PRINTF("  Endpoint type: %s\n", ( {
					     char *pipetype;
					     switch (pipe_info->pipe_type) {
case USB_ENDPOINT_XFER_CONTROL:
pipetype = "CONTROL"; break; case USB_ENDPOINT_XFER_BULK:
pipetype = "BULK"; break; case USB_ENDPOINT_XFER_INT:
pipetype = "INTERRUPT"; break; case USB_ENDPOINT_XFER_ISOC:
pipetype = "ISOCHRONOUS"; break; default:
					     pipetype = "UNKNOWN"; break;};
					     pipetype;}
		   )) ;
	DWC_PRINTF("  Speed: %s\n", ( {
				     char *speed; switch (dev->speed) {
case USB_SPEED_HIGH:
speed = "HIGH"; break; case USB_SPEED_FULL:
speed = "FULL"; break; case USB_SPEED_LOW:
speed = "LOW"; break; default:
				     speed = "UNKNOWN"; break;};
				     speed;}
		   )) ;
	DWC_PRINTF("  Data buffer length: %d\n", dwc_otg_urb->length);
	DWC_PRINTF("  Transfer buffer: %p, Transfer DMA: %p\n",
		   dwc_otg_urb->buf, (void *)dwc_otg_urb->dma);
	DWC_PRINTF("  Setup buffer: %p, Setup DMA: %p\n",
		   dwc_otg_urb->setup_packet, (void *)dwc_otg_urb->setup_dma);
	DWC_PRINTF("  Interval: %d\n", dwc_otg_urb->interval);
}

#endif
/** Handles host mode interrupts for the DWC_otg controller. Returns IRQ_NONE if
 * there was no interrupt to handle. Returns IRQ_HANDLED if there was a valid
 * interrupt.
 *
 * This function is called by the USB core when an interrupt occurs */
static int32_t dwc_otg_hcd_irq(dwc_otg_hcd_t *dwc_otg_hcd)
{
	int32_t retval = dwc_otg_hcd_handle_intr(dwc_otg_hcd);
	if (retval != 0) {
		S3C2410X_CLEAR_EINTPEND();
	}
	return retval;
}

/** Handles hub class-specific requests. */
static int dwc_submit_root(struct usb_device *dev, unsigned long pipe,
		void *buf, int transfer_len, struct devrequest *cmd)
{
	int retval;
	u16 typeReq = cmd->requesttype | cmd->request << 8;
	u16 wValue = le16_to_cpu(cmd->value);
	u16 wIndex = le16_to_cpu(cmd->index);
	u16 wLength = le16_to_cpu(cmd->length);
	u16 act_len = wLength;

	DWC_DEBUGPL(DBG_HCDV, "DWC OTG %s(%p)\n", __func__, dwc_otg_hcd);
	DWC_DEBUGPL(DBG_HCDV, "%s Req=0x%x requesttype=0x%x wValue=0x%x wIndex=0x%x wLength=0x%x\n",
		   __func__, cmd->request, cmd->requesttype, wValue, wIndex, wLength);
	wLength = wLength < transfer_len ? wLength : transfer_len;
	retval = dwc_otg_hcd_hub_control(dwc_otg_hcd,
					 typeReq, wValue, wIndex, buf, wLength, &act_len); 

	if (retval) {
		retval = -1;
		dev->act_len = 0;
		dev->status = USB_ST_STALLED;
	} else {
		dev->act_len = act_len;
		dev->status = 0;
	}

	return retval;
}

/* common code for handling submit messages - used for all but root hub */
/* accesses. */
int submit_common_msg(struct usb_device *dev, unsigned long pipe, void *buffer,
		int transfer_len, struct devrequest *setup, int interval)
{
	int stat = 0;
	int retval = 0;
	int timeout;
	dwc_otg_hcd_urb_t *dwc_otg_urb;
	uint8_t ep_type = 0;
	void *buf;

	DWC_DEBUGPL(DBG_HCDV, "DWC OTG %s() %p\n", __func__, dwc_otg_hcd);
	DWC_DEBUGPL(DBG_HCDV, "DWC OTG %s() usb_device %p buffer %p setup %p transfer_len %d\n", __func__, dev, buffer, setup, transfer_len);  
	DWC_DEBUGPL(DBG_HCDV, "%s pipe type %u speed %u\n", __func__, usb_pipetype(pipe), dev->speed);	

	switch (usb_pipetype(pipe)) {
	case PIPE_CONTROL:
		ep_type = USB_ENDPOINT_XFER_CONTROL;
		break;
	case PIPE_ISOCHRONOUS:
		ep_type = USB_ENDPOINT_XFER_ISOC;
		break;
	case PIPE_BULK:
		ep_type = USB_ENDPOINT_XFER_BULK;
		break;
	case PIPE_INTERRUPT:
		ep_type = USB_ENDPOINT_XFER_INT;
		break;
	default:
		DWC_WARN("Wrong ep type\n");
	}

	dwc_otg_urb = dwc_otg_hcd_urb_alloc(dwc_otg_hcd, 0, 0);

	if(dwc_otg_urb == NULL) {
		DWC_ERROR("dwc urb allocation failed\n"); 
		stat = USB_ST_CRC_ERR;
		dev->status = stat;
		dev->act_len = 0;
		return -1;
	}
	
	dwc_otg_urb->finished = 0;

	dwc_otg_hcd_urb_set_pipeinfo(dwc_otg_urb, usb_pipedevice(pipe),
				     usb_pipeendpoint(pipe), ep_type,
				     usb_pipein(pipe),
				     usb_maxpacket(dev, pipe));

	buf = buffer;

	dwc_otg_hcd_urb_set_params(dwc_otg_urb, dev, buf,
				   (dwc_dma_t)buf,
				   transfer_len,
				   (void *)setup, 
				   (dwc_dma_t)setup,
				   0,
				   interval);

#ifdef CONFIG_DWC_DEBUG
	if (CHK_DEBUG_LEVEL(DBG_HCDV | DBG_HCD_URB)) {
		dump_urb_info(dwc_otg_urb, "urb_enqueue");
	}
#endif

	dwc_mdelay(1);
	retval = dwc_otg_hcd_urb_enqueue(dwc_otg_hcd, dwc_otg_urb, &dwc_otg_urb->hcpriv, 0);
	if (!retval) {
		timeout = 40000;
		/* wait for it to complete */
		for (;;) {
			/* check whether the controller is done */
			dwc_otg_common_irq(dwc_otg_hcd_get_priv_data(dwc_otg_hcd));
			stat = dwc_otg_hcd_irq(dwc_otg_hcd);
			
			if (dwc_otg_urb->finished) {
				DWC_DEBUGPL(DBG_HCDV, "DWC OTG %s urb %p is done\n", __func__, dwc_otg_urb);	
				break;
			}

			if (--timeout) {
				dwc_mdelay(1);
			} else {
				DWC_ERROR("TIMEOUT ");
				dwc_otg_hcd_urb_dequeue(dwc_otg_hcd, dwc_otg_urb);
				dwc_otg_urb->finished = 1;
				stat = USB_ST_CRC_ERR;
				dev->status = stat;
				dev->act_len = 0;
				break;
			}
		}
	} else {
		if (retval == -DWC_E_NO_DEVICE) {
			dev->status = DWC_E_NO_DEVICE;
			retval = -1;
		}
	}
	dwc_free(dwc_otg_urb);

	return retval;
}

/* submit routines called from usb.c */
int submit_bulk_msg(struct usb_device *dev, unsigned long pipe, void *buffer,
		int transfer_len)
{
	 DWC_DEBUGPL(DBG_HCDV, "%s\n", __func__);
	return submit_common_msg(dev, pipe, buffer, transfer_len, NULL, 0);
}

int submit_control_msg(struct usb_device *dev, unsigned long pipe, void *buffer,
		int transfer_len, struct devrequest *setup)
{
	DWC_DEBUGPL(DBG_HCDV, "%s\n", __func__);

	if (usb_pipetype(pipe) != PIPE_CONTROL) {
		DWC_ERROR("non-control pipe (type=%lu)", usb_pipetype(pipe));
		return -1;
	}

	if (usb_pipedevice(pipe) == dwc_otg_hcd->root_hub) {
		if (dwc_otg_hcd->root_hub == 0)
			dev->speed = USB_SPEED_HIGH;
		dwc_otg_hcd_irq(dwc_otg_hcd);
		return dwc_submit_root(dev, pipe, buffer, transfer_len, setup);
	}
	return submit_common_msg(dev, pipe, buffer, transfer_len, setup, 0);
}

int submit_int_msg(struct usb_device *dev, unsigned long pipe, void *buffer,
		int transfer_len, int interval)
{
	DWC_DEBUGPL(DBG_HCDV, "%s\n", __func__);
	return submit_common_msg(dev, pipe, buffer, transfer_len, NULL,
			interval);
}

#ifdef CONFIG_SYS_USB_EVENT_POLL
/*
 * This function polls for USB keyboard data.
 */
void usb_event_poll()
{
	device_t *dev;
	struct usb_device *usb_kbd_dev;
	struct usb_interface_descriptor *iface;
	struct usb_endpoint_descriptor *ep;
	int pipe;
	int maxp;

	/* Get the pointer to USB Keyboard device pointer */
	dev = device_get_by_name("usbkbd");
	usb_kbd_dev = (struct usb_device *)dev->priv;
	iface = &usb_kbd_dev->config.if_desc[0];
	ep = &iface->ep_desc[0];
	pipe = usb_rcvintpipe(usb_kbd_dev, ep->bEndpointAddress);

	/* Submit a interrupt transfer request */
	maxp = usb_maxpacket(usb_kbd_dev, pipe);
	usb_submit_int_msg(usb_kbd_dev, pipe, &new[0],
			maxp > 8 ? 8 : maxp, ep->bInterval);
}
#endif /* CONFIG_SYS_USB_EVENT_POLL */

int usb_lowlevel_init(void)
{
	ubi32_dwc_init();
	dwc_mdelay(100);
	dwc_otg_init();
	dwc_mdelay(100);
	//dwc_otg_common_irq(dwc_otg_hcd_get_priv_data(dwc_otg_hcd));
	return 0;
}

int usb_lowlevel_stop(void)
{
	dwc_otg_hcd_stop(dwc_otg_hcd);
	dwc_udelay(100);
	dwc_otg_cleanup();
	return 0;
}
#endif				/* DWC_DEVICE_ONLY */
