/*
 * arch/ubicom32/include/asm/videopassthrough.h
 *   Header file for video passthrough control.
 *
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
 */

#ifndef VIDEOPASSTHROUGH_H
#define VIDEOPASSTHROUGH_H

#define VIDEO_PASSTHROUGH_REGS_VERSION	2

#define VIDEO_PASSTHROUGH_COMMAND_STOP	1

/*
 * Start the video passthrough, the buffer must be assigned or
 * the passthrough will not be enabled
 */
#define VIDEO_PASSTHROUGH_COMMAND_START	2
struct video_passthrough_regs {
	u32_t				version;

	volatile u32_t			command;
	volatile u32_t			enabled;

	/* 
	 * Minimum buffer size required, read only
	 */
	u32_t				minimum_buffer_size;
	u32_t				buffer_alignment;

	/* 
	 * Pointer to buffer, must be aligned and at least minimum_buffer_size large
	 */
	volatile void			*buffer;
};

#define VIDEO_PASSTHROUGH_NODE_VERSION	3
struct video_passthrough_node {
	struct devtree_node		dn;
	u32_t				version;

	struct video_passthrough_regs	*regs;
};

#endif // VIDEOPASSTHROUGH_H

