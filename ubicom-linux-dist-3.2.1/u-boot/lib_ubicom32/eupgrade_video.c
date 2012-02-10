/*
 * eupgrade_video.c
 *	Emergency upgrade (video mode)
 *
 * Provides the video front end for upgrade.
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
#include <video.h>
#include "ubicom32video.h"
#include "cmd_usb_upgrade.h"
#include "pngreader.h"
#include <png_logo.h>
#include "eupgrade.h"

/*
 * Configuration:
 *	CONFIG_EUPGRADE_VIDEO_STATUS_BAR
 *		define this to draw the status bar with the following
 *		parameters:
 *			CONFIG_EUPGRADE_VIDEO_STATUS_BAR_X
 *			CONFIG_EUPGRADE_VIDEO_STATUS_BAR_Y
 *			CONFIG_EUPGRADE_VIDEO_STATUS_BAR_WIDTH
 *			CONFIG_EUPGRADE_VIDEO_STATUS_BAR_HEIGHT
 *			CONFIG_EUPGRADE_VIDEO_STATUS_BAR_BG
 *			CONFIG_EUPGRADE_VIDEO_STATUS_BAR_FG
 *			CONFIG_EUPGRADE_VIDEO_STATUS_BAR_BORDER
 *			CONFIG_EUPGRADE_VIDEO_STATUS_ROW
 *			CONFIG_EUPGRADE_VIDEO_STATUS_COL
 *			CONFIG_EUPGRADE_VIDEO_STATUS_MAXLEN
 */

/*
 * eupgrade_draw_status_bar
 */
static void eupgrade_draw_status_bar(const char *str, int step, int steps)
{
	/*
	 * Background
	 */
	ubicom32video_draw_filled_rect( CONFIG_EUPGRADE_VIDEO_STATUS_BAR_X, CONFIG_EUPGRADE_VIDEO_STATUS_BAR_Y,
					CONFIG_EUPGRADE_VIDEO_STATUS_BAR_WIDTH, CONFIG_EUPGRADE_VIDEO_STATUS_BAR_HEIGHT,
					CONFIG_EUPGRADE_VIDEO_STATUS_BAR_BG);

	/*
	 * Border
	 */
	ubicom32video_draw_rect(CONFIG_EUPGRADE_VIDEO_STATUS_BAR_X, CONFIG_EUPGRADE_VIDEO_STATUS_BAR_Y,
				CONFIG_EUPGRADE_VIDEO_STATUS_BAR_WIDTH, CONFIG_EUPGRADE_VIDEO_STATUS_BAR_HEIGHT,
				CONFIG_EUPGRADE_VIDEO_STATUS_BAR_BORDER);

	/*
	 * Foreground
	 */
	if (steps) {
		int donepx = (u32)(((u64)(CONFIG_EUPGRADE_VIDEO_STATUS_BAR_WIDTH - 2) * (u64)step) / steps);
		ubicom32video_draw_filled_rect( CONFIG_EUPGRADE_VIDEO_STATUS_BAR_X + 1, CONFIG_EUPGRADE_VIDEO_STATUS_BAR_Y + 1,
						donepx, CONFIG_EUPGRADE_VIDEO_STATUS_BAR_HEIGHT - 2,
						CONFIG_EUPGRADE_VIDEO_STATUS_BAR_FG);
	}

	/*
	 * Text
	 */
	char s[128];
	s[0] = 0;
	if (str) {
		strncpy(s, str, CONFIG_EUPGRADE_VIDEO_STATUS_MAXLEN);
	}
	int i;
	for(i = 0; i < CONFIG_EUPGRADE_VIDEO_STATUS_MAXLEN; i++) {
		if (s[i] == 0) {
			break;
		}
	}
	for(; i < CONFIG_EUPGRADE_VIDEO_STATUS_MAXLEN; i++) {
		s[i] = ' ';
	}
	s[CONFIG_EUPGRADE_VIDEO_STATUS_MAXLEN] = 0;
	video_gotorc(CONFIG_EUPGRADE_VIDEO_STATUS_ROW, CONFIG_EUPGRADE_VIDEO_STATUS_COL);
	video_puts(s);
}

/*
 * eupgrade_video_status_callback
 */
static void eupgrade_video_status_callback(void *appdata, enum eupgrade_states state, int step, int steps, int res)
{
	char s[128];

	switch (state) {
		case EUPGRADE_STATE_BEGIN:
		case EUPGRADE_STATE_START_USB:
			eupgrade_draw_status_bar("Searching...", 0, 0);
			break;
		case EUPGRADE_STATE_VERIFY:
			if (res == 0) {
				sprintf(s, "Verifying: %d/%d", step, steps);
				eupgrade_draw_status_bar(s, step, steps);
			} else {
				if (step == USB_UPGRADE_VERIFY_STEP_IDENTITY) {
					eupgrade_draw_status_bar("Verifying: Incorrect Image", step, steps);
				} else {
					eupgrade_draw_status_bar("Verifying: Corrupted Image", step, steps);
				}
				udelay(5000 * 1000);
			}
			break;
		case EUPGRADE_STATE_ERASE:
			sprintf(s, "Erasing: %d%%", (u32)(((u64)step * 100ULL) / (u64)steps));
			eupgrade_draw_status_bar(s, step, steps);
			break;
		case EUPGRADE_STATE_WRITE:
			sprintf(s, "Writing: %d%%", (u32)(((u64)step * 100ULL) / (u64)steps));
			eupgrade_draw_status_bar(s, step, steps);
			break;
		case EUPGRADE_STATE_SUCCESS:
			eupgrade_draw_status_bar("Successful!", 1, 1);
			break;

		case EUPGRADE_STATE_FAILED:
		case EUPGRADE_STATE_STOP_USB:
		case EUPGRADE_STATE_TRY_UPGRADE:
			eupgrade_draw_status_bar(NULL, 0, 0);
			break;

		case EUPGRADE_STATE_READ:
			break;
	}
}

/*
 * eupgrade_video_init
 */
int eupgrade_video_init(void)
{
	printf("[Emergency Upgrade Video Init]\n");
	if (!video_get_fb()) {
		printf("\tNo video availble!\n");
		return 0;
	}

#ifdef CONFIG_EUPGRADE_VIDEO_STATUS_BAR
	if (eupgrade_register_status_callback(eupgrade_video_status_callback, NULL)) {
		printf("\tCould not register callback\n");
	}
#endif

	pngreader_decode(video_get_fb(), png_logo, sizeof(png_logo), video_get_width(), video_get_height());

	return 0;
}

