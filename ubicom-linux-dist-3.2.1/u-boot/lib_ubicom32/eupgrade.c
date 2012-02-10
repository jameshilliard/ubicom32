/*
 * eupgrade.c
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
#include <common.h>
#include <malloc.h>
#include <asm/errno.h>
#include "cmd_usb_upgrade.h"
#include "eupgrade.h"

#define endtick(seconds) (get_ticks() + (uint64_t)(seconds) * get_tbclk())

struct eupgrade_callback {
	struct eupgrade_callback	*next;
	eupgrade_status_callback_t	cb;
	void				*appdata;
};
static struct eupgrade_callback *eupgrade_callbacks = NULL;

/*
 * eupgrade_call_status_callbacks
 */
static void eupgrade_call_status_callbacks(enum eupgrade_states state, int step, int steps, int status)
{
	printf("\tStatus state:%04x step:%d/%d res:%d\n", state, step, steps, status);

	struct eupgrade_callback *u = eupgrade_callbacks;
	while (u) {
		u->cb(u->appdata, state, step, steps, status);
		u = u->next;
	}
}

/*
 * eupgrade_register_status_callback
 */
int eupgrade_register_status_callback(eupgrade_status_callback_t cb, void *appdata)
{
	struct eupgrade_callback *u = calloc(1, sizeof(struct eupgrade_callback));
	if (!u) {
		return -ENOMEM;
	}

	u->next = eupgrade_callbacks;
	eupgrade_callbacks = u;

	u->cb = cb;
	u->appdata = appdata;

	return 0;
}

/*
 * eupgrade_usb_upgrade_status_callback
 */
static void eupgrade_usb_upgrade_status_callback(enum usb_upgrade_states state, int step, int steps, int status)
{
	enum eupgrade_states mystate = 0;

	switch (state) {
	case USB_UPGRADE_STATE_READ:
		mystate = EUPGRADE_STATE_READ;
		break;

	case USB_UPGRADE_STATE_VERIFY:
		mystate = EUPGRADE_STATE_VERIFY;
		break;

	case USB_UPGRADE_STATE_ERASE:
		mystate = EUPGRADE_STATE_ERASE;
		break;

	case USB_UPGRADE_STATE_WRITE:
		mystate = EUPGRADE_STATE_WRITE;
		break;
	}

	eupgrade_call_status_callbacks(mystate, step, steps, status);
}

/*
 * eupgrade_init
 */
int eupgrade_init(void)
{
	usb_upgrade_status_callback = eupgrade_usb_upgrade_status_callback;

	puts("[Emergency Upgrade]\nPlease insert USB stick - SPACE to abort\n");
	while (1) {
		eupgrade_call_status_callbacks(EUPGRADE_STATE_BEGIN, 0, 0, 0);

		/*
		 * Check to see if the user is aborting the process
		 */
		uint64_t etime = endtick(2);
		do {
			if (tstc()) {
				if (getc() == 0x20) {
					puts("Emergency Upgrade aborted...\n");
					return 0;
				}
			}
		} while (get_ticks() <= etime);

		/*
		 * Start up the USB
		 */
		eupgrade_call_status_callbacks(EUPGRADE_STATE_START_USB, 0, 0, 0);
		int res = run_command("usb start", 0);
		if (res < 0) {
			puts("Failed to start USB...restarting\n");
			reset_cpu(0);
			return 0;
		}

		run_command("usb part", 0);

		/*
		 * Test USB0 for upgrade file
		 */
		eupgrade_call_status_callbacks(EUPGRADE_STATE_TRY_UPGRADE, 0, 0, 0);
		do {
			res = run_command("usb_upgrade 0:1 upgrade.ub", 0);
			if (res >= 0) {
				break;
			}
			res = run_command("usb_upgrade 0:2 upgrade.ub", 0);
			if (res >= 0) {
				break;
			}
			res = run_command("usb_upgrade 0:3 upgrade.ub", 0);
			if (res >= 0) {
				break;
			}
			res = run_command("usb_upgrade 0:4 upgrade.ub", 0);
		} while (0);

		if (res < 0) {
			puts("Upgrade failed, try again\n");
			eupgrade_call_status_callbacks(EUPGRADE_STATE_FAILED, 0, 0, 0);
		} else {
			puts("Upgrade succeeded, booting\n");
			eupgrade_call_status_callbacks(EUPGRADE_STATE_SUCCESS, 0, 0, 0);
			return 0;
		}

		/*
		 * Stop the usb
		 */
		eupgrade_call_status_callbacks(EUPGRADE_STATE_STOP_USB, 0, 0, 0);
		run_command("usb stop", 0);
	}

	return 0;
}

