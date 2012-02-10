/*
 * ubicom32video.h
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

#ifndef __UBICOM32VIDEO_H__
#define __UBICOM32VIDEO_H__

extern void ubicom32video_draw_vline(int x, int y, int h, int val);
extern void ubicom32video_draw_hline(int x, int y, int w, int val);
extern void ubicom32video_draw_rect(int x, int y, int w, int h, int val);
extern void ubicom32video_draw_filled_rect(int x, int y, int w, int h, int val);
extern int video_get_width(void);
extern int video_get_height(void);
extern void *video_get_fb(void);

#endif
