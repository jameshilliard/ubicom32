#include <linux/usb.h>
#include <linux/usb/quirks.h>
#include "usb.h"

/* List of quirky SS USB devices.  Please keep this list ordered by:
 * 	1) Vendor ID
 * 	2) Product ID
 * 	3) Class ID
 *
 */
static const struct usb_device_id ss_usb_quirk_list[] = {
#if 0
	/* My Book 1130 Western Digital */
	{ USB_DEVICE(0x1058, 0x1130), },
	/* eGo USB Iomega */
	{ USB_DEVICE(0x059b, 0x0070), },
	/*  External USB 3.0 Toshiba */
	{ USB_DEVICE(0x0480, 0xa006), },
#endif

	{ }  /* terminating entry must be last */
};

/* returns 0 if no match, 1 if match */
static int ss_usb_match_device(struct usb_device *dev, const struct usb_device_id *id)
{
	if (id->idVendor != le16_to_cpu(dev->descriptor.idVendor))
		return 0;

	if (id->idProduct != le16_to_cpu(dev->descriptor.idProduct))
		return 0;

	return 1;
}

static const struct usb_device_id *find_id(struct usb_device *udev)
{
	const struct usb_device_id *id = ss_usb_quirk_list;

	for (; id->idVendor; id++) {
		if (ss_usb_match_device(udev, id))
			return id;
	}
	return NULL;
}

/*
 * Detect any quirks the device has, and do any housekeeping for it if needed.
 */
int ubi32_ss_usb_detect_quirks(struct usb_device *udev)
{
	const struct usb_device_id *id = ss_usb_quirk_list;

#ifndef CONFIG_UBI32_SS_USB2_MODE
	return 0;
#endif
	id = find_id(udev);
	if (id)
		return 1;

	return 0;
}
