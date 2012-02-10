/*
 * eupgrade.h
 *	Emergency upgrade
 *
 * Scans for USB insertion and kicks off the upgrade process
 *
 * (C) Copyright 2010
 * Ubicom, Inc. www.ubicom.com
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef __EUPGRADE_H__
#define __EUPGRADE_H__

#include "cmd_usb_upgrade.h"

extern int eupgrade_init(void);

enum eupgrade_states {
	EUPGRADE_STATE_BEGIN,
	EUPGRADE_STATE_START_USB,
	EUPGRADE_STATE_TRY_UPGRADE,
	EUPGRADE_STATE_READ,
	EUPGRADE_STATE_VERIFY,
	EUPGRADE_STATE_ERASE,
	EUPGRADE_STATE_WRITE,
	EUPGRADE_STATE_STOP_USB,
	EUPGRADE_STATE_SUCCESS,
	EUPGRADE_STATE_FAILED,
};

typedef void (*eupgrade_status_callback_t)(void *appdata, enum eupgrade_states state, int step, int steps, int status);

extern int eupgrade_register_status_callback(eupgrade_status_callback_t cb, void *appdata);

#endif
