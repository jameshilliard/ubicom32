/*
 * arch/ubicom32/include/asm/vdc_tio.h
 *   Ubicom32 architecture VDC TIO definitions.
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
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#ifndef _ASM_UBICOM32_VDC_TIO_H
#define _ASM_UBICOM32_VDC_TIO_H

#include <asm/devtree.h>
#include <asm/ubicom32fb.h>

#define VDCTIO_VP_VERSION			17

#define VDCTIO_PANSTEP_X_BB			0
#define VDCTIO_PANSTEP_X			2

#define VDCTIO_SCALE_FLAG_YUV_LINE_ORDER	(1 << 10)
#define VDCTIO_SCALE_FLAG_VSUB			(1 << 9)
#define VDCTIO_SCALE_FLAG_YUV_SCAN_ORDER	(1 << 8)
#define VDCTIO_SCALE_FLAG_YUV_BLOCK_ORDER	(1 << 7)
#define VDCTIO_SCALE_FLAG_YUV			(1 << 6)
#define VDCTIO_SCALE_FLAG_VRANGE_16_255		(1 << 5)
#define VDCTIO_SCALE_FLAG_VRANGE_0_255		(1 << 4)
#define VDCTIO_SCALE_FLAG_HSUB_2_1		(1 << 3)
#define VDCTIO_SCALE_FLAG_HSUB_1_1		(1 << 2)
#define VDCTIO_SCALE_FLAG_SET_FRAME_BUFFER	(1 << 1)
#define VDCTIO_SCALE_FLAG_ENABLE		(1 << 0)

/*
 * Set this bit to make the driver pan the next frame.
 * horiz_pan and horiz_stride are used for this.
 */
#define VDCTIO_NEXT_FRAME_FLAG_PAN		(1 << (VDCTIO_NEXT_FRAME_FLAG_PAN_BIT))
#define VDCTIO_NEXT_FRAME_FLAG_PAN_BIT		1
#define VDCTIO_NEXT_FRAME_FLAG_YUV		(1 << (VDCTIO_NEXT_FRAME_FLAG_YUV_BIT))
#define VDCTIO_NEXT_FRAME_FLAG_YUV_BIT		0

#define VDCTIO_CAPS_SUPPORTS_HORIZ_PAN_BB	(1 << 7) // Horizontal panning in bit-bang mode
#define VDCTIO_CAPS_SUPPORTS_HORIZ_PAN		(1 << 6) // Horizontal panning in sync/tcon/de mode
#define VDCTIO_CAPS_SUPPORTS_OVERLAY		(1 << 5)
#define VDCTIO_CAPS_SUPPORTS_BITBANG		(1 << 4)
#define VDCTIO_CAPS_SUPPORTS_DE			(1 << 3)
#define VDCTIO_CAPS_SUPPORTS_TCON		(1 << 2)
#define VDCTIO_CAPS_SUPPORTS_HV			(1 << 1)
#define VDCTIO_CAPS_SUPPORTS_SCALING		(1 << 0)

/*
 * VDC Overlay control
 *
 * The overlay buffer is the width of the output resolution in
 * 16-bit - 565 format, 2 bytes to a pixel, 2 pixels for each
 * 32-bit word.  The number of lines needed depends on
 * the REGION flag.
 *
 * The corresponding alpha buffer is the width of the output
 * resolution in 8-bit format, 1 byte to a pixel, 4 pixels
 * for each 32-bit word.  The number of lines needed depends
 * the REGION flag.
 *
 * If ONE_LINE_ALPHA is set, then the first line of the
 * alpha buffer is applied to each line in the overlay region.
 *
 * If REGION is set, then the overlay is only done between the
 * lines specified by overlay_begin and overlay_end.  In this
 * case, line 1 of the overlay buffer will be placed at
 * overlay_begin, and so forth.  The overlay buffer only needs
 * (overlay_end - overlay_begin) lines.
 *
 * If ALPHA is set then use the alpha buffer, if not, the overlay
 * will be opaque.
 */
#define VDCTIO_OVERLAY_FLAG_ALPHA		(1 << 3)
#define VDCTIO_OVERLAY_FLAG_REGION		(1 << 2)
#define VDCTIO_OVERLAY_FLAG_ONE_LINE_ALPHA	(1 << 1)	/* Use only one line of alpha buffer */
#define VDCTIO_OVERLAY_FLAG_ENABLE		(1 << 0)

#define VDCTIO_DISP_FLAG_NO_GOTO		(1 << 13)	/* Display does not support row and column addressing */
#define VDCTIO_DISP_FLAG_NRT			(1 << 12)	/* Run DDR thread in NRT mode */
#define VDCTIO_DISP_FLAG_INV_CLOCK		(1 << 11)
#define VDCTIO_DISP_FLAG_8			(1 << 10)	/* Must be aligned to 8 bit boundary */
#define VDCTIO_DISP_FLAG_16			(1 << 9)
#define VDCTIO_DISP_FLAG_8SPLIT			(1 << 8)
#define VDCTIO_DISP_FLAG_RESET			(1 << 7)
#define VDCTIO_DISP_FLAG_RD			(1 << 6)
#define VDCTIO_DISP_FLAG_CS			(1 << 5)
#define VDCTIO_DISP_FLAG_FLIPY			(1 << 4)
#define VDCTIO_DISP_FLAG_FLIPX			(1 << 3)
#define VDCTIO_DISP_FLAG_ROTATED		(1 << 2)
#define VDCTIO_DISP_FLAG_8BIT			(1 << 1)
#define VDCTIO_DISP_FLAG_NO_PARTIAL		(1 << 0)

/*
 * These commands are only for self refreshed displays
 */
#define VDCTIO_COMMAND_AUTO_REFRESH		(1 << 8)	/* fclk = refresh interval in fps, 0 for no auto */
#define VDCTIO_COMMAND_REFRESH_REGION		(1 << 7)	/* use x_in, y_in, x_out, y_out */
#define VDCTIO_COMMAND_REFRESH			(1 << VDCTIO_COMMAND_REFRESH_BIT)
#define VDCTIO_COMMAND_REFRESH_BIT		(6)
#define VDCTIO_COMMAND_EXEC_STEPS		(1 << 5)
#define VDCTIO_COMMAND_INIT			(1 << 4)

/*
 * These commands are for generic displays
 */
#define VDCTIO_COMMAND_SET_COEFF		(1 << 2)
#define VDCTIO_COMMAND_SET_LUT			(1 << 1)
#define VDCTIO_COMMAND_SET_SCALE_MODE		(1 << 0)

/*
 * These commands are for all displays
 */
#define VDCTIO_COMMAND_START			(1 << 3)

/*
 * Status of the TIO
 */
#define VDCTIO_STATUS_FLAG_STARTED		(1 << 0)

enum vdctio_types {
	VDCTIO_TYPE_SYNC,
	VDCTIO_TYPE_DE,
	VDCTIO_TYPE_DIRECT,
	VDCTIO_TYPE_BITBANG,
};

enum vdctio_lcd_ops {
	/*
	 * Sleep for (data) ms
	 */
	LCD_STEP_SLEEP,

	/*
	 * Execute write of command
	 */
	LCD_STEP_CMD,

	/*
	 * Execute write of data
	 */
	LCD_STEP_DATA,

	/*
	 * Execute write of command/data
	 */
	LCD_STEP_CMD_DATA,

	/*
	 * Script done
	 */
	LCD_STEP_DONE,
};

struct vdc_tio_lcd_step {
	enum vdctio_lcd_ops	op;
	u16_t			cmd;
	u16_t			data;
};

struct vdc_tio_vp_regs {
	/*
	 * Version of this TIO register map
	 */
	u32_t			version;

	volatile u32_t		status;
	volatile u32_t		command;

	/*
	 * Next frame pointer.
	 *
	 * After each frame has been displayed, the VDC will check this
	 * register and take the new value if bit 0 of next_frame has
	 * been set.  Bit 0 of next_frame will be cleared once the
	 * VDC has accepted the buffer.
	 */
	void			*next_frame;
	u32_t			next_frame_flags;

	/*
	 * Current frame number, monotonically increasing number
	 */
	u32_t			frame_number;

	/*
	 * Horizontal Pan offset (in pixels)
	 */
	u32_t			horiz_pan_ofs;
	u32_t			horiz_stride;

	/*
	 * Be careful when adding registers above this line.  Registers
	 * above this line need to be under the 128 byte offset value
	 * allowed by instruction set.  Double check the file
	 * build/include/vdc-gen-datafields.h in the project directory
	 * to verify offset values.
	 *
	 * Only registers which need to be directly accessed by assembly
	 * language (typically in the data path) are subject to this
	 * restriction.
	 */

	/*
	 * Overlay buffer pointer.
	 *
	 * After each frame has been displayed, the VDC will check this
	 * register and take the new value if bit 0 of overlay_buffer has
	 * been set.  Bit 0 of overlay_buffer will be cleard once the VDC
	 * has accepted the buffer.
	 *
	 * This field is used only if overlays are enabled.
	 *
	 * Overlays may only be enabled in scaling mode.
	 *
	 * The overlay_buffer must be valid as long as overlays are enabled.
	 */
	void			*overlay_buffer;
	void			*overlay_alpha_buffer;
	u32_t			overlay_flags;
	u16_t			overlay_begin;
	u16_t			overlay_end;

	/*
	 * These map directly into the PIXP registers 0x20-0x80.
	 * DO NOT change the order of these three variables.
	 */
	u32_t			red_lut[6];
	u32_t			blue_lut[6];
	u32_t			green_lut[13];

	/*
	 * These map directly into the PIXP registers 0x04, 0x08
	 */
	u32_t			coeff0;
	u32_t			coeff1;

	/*
	 * There are used to set the scaling parameters
	 */
	u32_t			x_in;
	u32_t			x_out;
	u32_t			y_in;
	u32_t			y_out;
	u32_t			scale_flags;

	/*
	 * These variables tell the guest OS what the underlying hardware looks like
	 */
	u32_t			caps;
	u32_t			xres;
	u32_t			yres;
	u32_t			fb_align;
	u8_t			bpp;
	u8_t			rbits;
	u8_t			gbits;
	u8_t			bbits;
	u8_t			rshift;
	u8_t			gshift;
	u8_t			bshift;

	/*
	 * Display type
	 */
	enum vdctio_types	type;

	/*
	 * Timing variables (not all used in all situations)  These
	 * values must be set prior to START being issued.  (only supported
	 * on ubicom32v4)
	 */
	u16_t			hfp;		// HV
	u16_t			hsync;		// HV/DE/TCON
	u16_t			hbp;		// HV
	u16_t			vfp;		// HV
	u16_t			vsync;		// HV/DE/TCON
	u16_t			vbp;		// HV
	u32_t			fclk;		// HV/DE/TCON

	/*
	 * Steps which will be executed when command VDCTIO_COMMAND_EXEC_STEPS
	 * is received.
	 */
	struct vdc_tio_lcd_step	*steps;

	/*
	 * Parameters for self refresh displays (set msb on reset_pin to make active_high)
	 *
	 * for 8 bit mode, offset data_port to arrive at the appropriate byte offset
	 * for 16 bit mode, offset data_port to arrive at the appropriate word offset
	 */
	u32_t			disp_flags;	// Also for HV/DE/TCON
	u32_t			data_port;
	u32_t			data_mask;
	u32_t			rs_port;
	u32_t			rd_port;
	u32_t			wr_port;
	u32_t			cs_port;
	u32_t			reset_port;
	u8_t			rs_pin;
	u8_t			rd_pin;
	u8_t			wr_pin;
	u8_t			cs_pin;
	u8_t			reset_pin;
	u16_t			horz_reg;
	u16_t			vert_reg;
	u16_t			gram_reg;
	u16_t			xofs;
	u16_t			yofs;
};

/*
 * Devtree node for VDC
 */
struct vdc_tio_node {
	struct devtree_node	dn;

	struct vdc_tio_vp_regs	*regs;
};

/*
 * mach-common/vdc_tio.c
 *	Only available at init time
 */
extern void vdc_tio_init(struct ubicom32fb_platform_data *pd);
extern void vdc_tio_fill_platform_data(struct ubicom32fb_platform_data *pd);

#endif /* _ASM_UBICOM32_VDC_TIO_H */
