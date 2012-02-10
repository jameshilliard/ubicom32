/*
 * cmd_usb_upgrade.h
 *	Command for doing upgrades from usb
 *
 * Copyright (C) 2010 Ubicom, Inc.
 * http://www.ubicom.com
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

#ifndef __CMD_USB_UPGRADE_H__
#define __CMD_USB_UPGRADE_H__

enum usb_upgrade_states {
	USB_UPGRADE_STATE_READ,
	USB_UPGRADE_STATE_VERIFY,
	USB_UPGRADE_STATE_ERASE,
	USB_UPGRADE_STATE_WRITE,
};

enum usb_upgrade_verify_steps {
	USB_UPGRADE_VERIFY_STEP_HEADER,
	USB_UPGRADE_VERIFY_STEP_IDENTITY,
	USB_UPGRADE_VERIFY_STEP_HEADER_CRC,
	USB_UPGRADE_VERIFY_STEP_DATA_CRC,
};

typedef void (*usb_upgrade_status_callback_t)(enum usb_upgrade_states state, int step, int steps, int status);

extern usb_upgrade_status_callback_t usb_upgrade_status_callback;

#endif
