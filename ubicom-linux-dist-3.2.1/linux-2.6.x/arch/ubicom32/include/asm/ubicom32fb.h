/*
 * arch/ubicom32/include/asm/ubicom32fb.h
 *   Ubicom32 architecture video frame buffer definitions.
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
 */
#ifndef _ASM_UBICOM32_UBICOM32FB_H
#define _ASM_UBICOM32_UBICOM32FB_H

#include <linux/ioctl.h>

/*
 * Set next frame
 */
#define UBICOM32FB_IOCTL_SET_NEXT_FRAME			_IOW('r',  1, void *) 
#define UBICOM32FB_IOCTL_SET_NEXT_FRAME_SYNC		_IOW('r',  2, void *) 
#define UBICOM32FB_IOCTL_SET_NEXT_FRAME_EX		_IOW('r',  10, void *) 		// only if the driver supports panning
struct ubicom32fb_next_frame_ex {
	unsigned long	stride;
	unsigned long	x_offset;
	unsigned long	y_offset;
	unsigned long	flags;
	void		*next_frame;
};
#define UBICOM32FB_IOCTL_SET_NEXT_FRAME_EX_FLAG_SYNC	(1 << 0)

/*
 * Set Mode
 */
#define UBICOM32FB_IOCTL_SET_MODE			_IOW('r',  3, void *)
struct ubicom32fb_mode {
	unsigned long	width;
	unsigned long	height;
	unsigned long	flags;
	void		*next_frame;
};

#define UBICOM32FB_IOCTL_SET_MODE_FLAG_YUV_LINE_ORDER	(1 << 9)
#define UBICOM32FB_IOCTL_SET_MODE_FLAG_YUV_SCAN_ORDER	(1 << 8)
#define UBICOM32FB_IOCTL_SET_MODE_FLAG_YUV_BLOCK_ORDER	(1 << 7)
#define UBICOM32FB_IOCTL_SET_MODE_FLAG_YUV		(1 << 6)
#define UBICOM32FB_IOCTL_SET_MODE_FLAG_VSUB		(1 << 5)
#define UBICOM32FB_IOCTL_SET_MODE_FLAG_VRANGE_16_255	(1 << 4)

#define UBICOM32FB_IOCTL_SET_MODE_FLAG_VRANGE_0_255	(1 << 3)
#define UBICOM32FB_IOCTL_SET_MODE_FLAG_HSUB_2_1		(1 << 2)
#define UBICOM32FB_IOCTL_SET_MODE_FLAG_HSUB_1_1		(1 << 1)
#define UBICOM32FB_IOCTL_SET_MODE_FLAG_SCALE_ENABLE	(1 << 0)

/*
 * Bitbang display IOCTLs
 */
#define UBICOM32FB_IOCTL_REFRESH			_IO('r',  4)
#define UBICOM32FB_IOCTL_REFRESH_SYNC			_IO('r',  5)

/*
 * Overlay control
 */
#define UBICOM32FB_IOCTL_SET_OVERLAY_BUFFER		_IOW('r', 6, void *)
#define UBICOM32FB_IOCTL_SET_OVERLAY_ALPHA_BUFFER	_IOW('r', 7, void *)
#define UBICOM32FB_IOCTL_SET_OVERLAY_REGION		_IOW('r', 8, unsigned int)
#define UBICOM32FB_IOCTL_SET_OVERLAY_FLAGS		_IOW('r', 9, unsigned int)

#define UBICOM32FB_IOCTL_OVERLAY_FLAG_ALPHA		(1 << 3)
#define UBICOM32FB_IOCTL_OVERLAY_FLAG_REGION		(1 << 2)
#define UBICOM32FB_IOCTL_OVERLAY_FLAG_ONE_LINE_ALPHA	(1 << 1)
#define UBICOM32FB_IOCTL_OVERLAY_FLAG_ENABLE		(1 << 0)

/*
 * Lock
 *	Prevent the framebuffer from being changed, does not
 *	lock out the overlay or alpha buffers through.
 */
#define UBICOM32FB_IOCTL_SET_LOCK			_IOW('r',11, unsigned int)
enum ubicom32fb_locks {
	UBICOM32FB_LOCK_DISABLED,
	UBICOM32FB_LOCK_ENABLED,
};

/*
 * Operating modes
 */
enum ubicom32fb_modes {
	UBICOM32FB_MODE_SYNC,
	UBICOM32FB_MODE_TCON,
	UBICOM32FB_MODE_DE,
	UBICOM32FB_MODE_BITBANG,
};

/*
 * External step function
 *	If set, the ubicom32fb driver will call this function
 *	before starting the display
 */
typedef void (*ubicom32fb_step_fn)(void *stepdata);

/*
 * Platform data can override the defaults in some cases
 */
#define UBICOM32FB_PLATFORM_DATA_FLAG_SHIFT		0x01
#define UBICOM32FB_PLATFORM_DATA_FLAG_TIMING		0x02
#define UBICOM32FB_PLATFORM_DATA_FLAG_DIM		0x04
#define UBICOM32FB_PLATFORM_DATA_FLAG_BITBANG		0x08
#define UBICOM32FB_PLATFORM_DATA_FLAG_MODE		0x10
#define UBICOM32FB_PLATFORM_DATA_FLAG_RESOLUTION	0x20
#define UBICOM32FB_PLATFORM_DATA_FLAG_STEPS		0x40
#define UBICOM32FB_PLATFORM_DATA_FLAG_DISP_FLAGS	0x80

/*
 * This structure is not referenced past init time
 */
struct ubicom32fb_platform_data {
	u32_t			flags;

	/*
	 * UBICOM32FB_PLATFORM_DATA_RESOLUTION
	 */
	u16_t			xres;
	u16_t			yres;

	/*
	 * UBICOM32FB_PLATFORM_DATA_FLAG_MODE
	 */
	enum ubicom32fb_modes	mode;

	/*
	 * UBICOM32FB_PLATFORM_DATA_FLAG_SHIFT
	 */
	u8_t			rbits;
	u8_t			gbits;
	u8_t			bbits;
	u8_t			rshift;
	u8_t			gshift;
	u8_t			bshift;

	/*
	 * UBICOM32FB_PLATFORM_DATA_FLAG_TIMING
	 */
	u16_t			hfp;		// HV
	u16_t			hsync;		// HV/DE/TCON
	u16_t			hbp;		// HV
	u16_t			vfp;		// HV
	u16_t			vsync;		// HV/DE/TCON
	u16_t			vbp;		// HV
	u32_t			fclk;		// HV/DE/TCON

	/*
	 * UBICOM32FB_PLATFORM_DATA_FLAG_DIM
	 */
	u16_t			height_mm;
	u16_t			width_mm;

	/*
	 * UBICOM32FB_PLATFORM_DATA_FLAG_BITBANG
	 *	See vdc_tio.h
	 */
	ubicom32fb_step_fn	step_fn;
	void			*steps;
	u32_t			disp_flags;
	u32_t			data_port;
	u32_t			data_mask;
	u32_t			wr_port;
	u32_t			rs_port;
	u32_t			rd_port;
	u32_t			cs_port;
	u32_t			reset_port;
	u8_t			wr_pin;
	u8_t			rs_pin;
	u8_t			rd_pin;
	u8_t			cs_pin;
	u8_t			reset_pin;
	u16_t			horz_reg;
	u16_t			vert_reg;
	u16_t			gram_reg;
	u16_t			xofs;
	u16_t			yofs;

};

#endif /* _ASM_UBICOM32_UBICOM32FB_H */
