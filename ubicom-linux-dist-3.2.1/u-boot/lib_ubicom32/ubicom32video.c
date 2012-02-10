/*
 * ubicom32video.c
 *   Ubicom32 Video Display Controller support for u-boot
 *
 * (C) Copyright 2010
 * Ubicom, Inc. www.ubicom.com
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
 *
 */

#include <common.h>
#include <video_fb.h>
#include <asm/ip5000.h>
#include "ubicom32video.h"
#include "vdc.h"

#define VDC_DEBUG(x...)	printf(x)

/*
 * Graphic device which describes the VDC
 */
static GraphicDevice gdev;

static int vdctio_int = 0;

/*
 * ubicom32video_draw_vline
 */
void ubicom32video_draw_vline(int x, int y, int h, int val)
{
	if ((gdev.gdfIndex != GDF_16BIT_565RGB) || !h) {
		return;
	}

	unsigned short *p = (unsigned short *)gdev.frameAdrs;
	p += x + (y * gdev.winSizeX);
	for (int j = 0; j < h; j++) {
		*p = val;
		p += gdev.winSizeX;
	}
}

/*
 * ubicom32video_draw_hline
 */
void ubicom32video_draw_hline(int x, int y, int w, int val)
{
	if ((gdev.gdfIndex != GDF_16BIT_565RGB) || !w) {
		return;
	}

	unsigned short *p = (unsigned short *)gdev.frameAdrs;
	p += x + (y * gdev.winSizeX);
	for (int j = 0; j < w; j++) {
		*p++ = val;
	}
}

/*
 * ubicom32video_draw_rect
 */
void ubicom32video_draw_rect(int x, int y, int w, int h, int val)
{
	if ((gdev.gdfIndex != GDF_16BIT_565RGB) || !w || !h) {
		return;
	}

	ubicom32video_draw_hline(x, y, w, val);		// top
	ubicom32video_draw_hline(x, y + h - 1, w, val);	// bottom
	ubicom32video_draw_vline(x, y, h, val);		// left
	ubicom32video_draw_vline(x + w - 1, y, h, val);	// right
}

/*
 * ubicom32video_draw_filled_rect
 */
void ubicom32video_draw_filled_rect(int x, int y, int w, int h, int val)
{
	if ((gdev.gdfIndex != GDF_16BIT_565RGB) || !w || !h) {
		return;
	}

	for (int i = 0; i < h; i++) {
		ubicom32video_draw_hline(x, y, w, val);
		y++;
	}
}

/*
 * ubicom32video_send_command
 *	Sends a command/data pair to the VDC
 */
static void ubicom32video_send_command(struct vdc_tio_vp_regs *regs, u16_t command, u8_t block)
{
	regs->command = command;
	ubicom32_set_interrupt(vdctio_int);
	while (block && regs->command);
}

/*
 * video_set_lut
 */
void video_set_lut(unsigned int index, unsigned char r, unsigned char g, unsigned char b)
{
}

/*
 * video_get_width
 */
int video_get_width(void)
{
	return gdev.winSizeX;
}

/*
 * video_get_height
 */
int video_get_height(void)
{
	return gdev.winSizeY;
}

/*
 * video_get_fb
 */
void *video_get_fb(void)
{
	return (void *)gdev.frameAdrs;
}

/*
 * video_hw_init
 */
void *video_hw_init(void)
{
	struct vdc_tio_node *vn = (struct vdc_tio_node *)devtree_find_node("vdctio");
	if (!vn) {
		VDC_DEBUG("No VDC node\n");
		return NULL;
	}
	vdctio_int = vn->dn.sendirq;

	struct vdc_tio_vp_regs *regs = vn->regs;

	if (regs->version != VDCTIO_VP_VERSION) {
		VDC_DEBUG("Version mismatch in VDC\n");
		return NULL;
	}

	VDC_DEBUG("Using VDC @%p irq:%d fb:%p x:%d y:%d bpp:%d\n", regs, 
		  vdctio_int, regs->next_frame, regs->xres, regs->yres, regs->bpp);

	/*
	 * u-boot depends on the vdc being pre-initialized by
	 * the starting process. (i.e. bootexec)
	 */
	if (!(regs->status & VDCTIO_STATUS_FLAG_STARTED)) {
		VDC_DEBUG("VDC not pre-initialized\n");
		return NULL;

	}

	/*
	 * Clear the screen
	 */
	//memset(regs->next_frame, 0, regs->xres * regs->yres * regs->bpp / 8);

	/*
	 * Turn on timer refresh if needed
	 */
	if (!regs->fclk) {
		regs->fclk = 15;
		ubicom32video_send_command(regs, VDCTIO_COMMAND_AUTO_REFRESH, 1);
	}

	/*
	 * Fill in the gdev
	 */
	gdev.frameAdrs = (unsigned int)regs->next_frame;
	gdev.winSizeX = regs->xres;
	gdev.winSizeY = regs->yres;
	gdev.gdfBytesPP = regs->bpp / 8;
	gdev.gdfIndex = GDF_16BIT_565RGB;

	/* 
	 * Success 
	 */
	return &gdev;
}


