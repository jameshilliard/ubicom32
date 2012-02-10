/*
 * videointio.h
 *	Video Input Virtual Peripherial
 *
 * Copyright © 2010 Ubicom Inc. <www.ubicom.com>.  All Rights Reserved.
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

#ifndef __VIDEOINTIO_H__
#define __VIDEOINTIO_H__

/*
 * Command register
 */
#define VIDEOINTIO_CMD_STOP			(1 << VIDEOINTIO_CMD_STOP_BIT)
#define VIDEOINTIO_CMD_STOP_BIT			0
#define VIDEOINTIO_CMD_VALID			(1 << VIDEOINTIO_CMD_VALID_BIT)
#define VIDEOINTIO_CMD_VALID_BIT		31

/*
 * Status register
 */
#define VIDEOINTIO_STAT_STOPPED		(1 << VIDEOINTIO_STAT_STOPPED_BIT)
#define VIDEOINTIO_STAT_STOPPED_BIT		0
#define VIDEOINTIO_STAT_READY			(1 << VIDEOINTIO_STAT_READY_BIT)
#define VIDEOINTIO_STAT_READY_BIT		1

/*
 * Ring buffer entry status bits
 */
#define	VIDEOINTIO_RING_STATUS_VALID		(1 << VIDEOINTIO_RING_STATUS_VALID_BIT)
#define	VIDEOINTIO_RING_STATUS_VALID_BIT	0
#define	VIDEOINTIO_RING_STATUS_LOOP		(1 << VIDEOINTIO_RING_STATUS_LOOP_BIT)
#define	VIDEOINTIO_RING_STATUS_LOOP_BIT		1

/*
 * NOTICE: When changing any of these structures, make sure to change the version
 * number associated with the structure.
 */
#define VIDEOINTIO_VP_VERSION		2
struct videointio_regs {
	u32_t				version;

	/*
	 * Command register
	 */
	u32_t				command;

	/*
	 * Status register
	 */
	u32_t				status;

	/*
	 * Data register
	 */
	u32_t				data;

	/*
	 * Vertical lines / Horizontal pixels
	 */
	u32_t				vlines;
	u32_t				hpixels;

	/*
	 * Ring buffer:
	 *	ring_begin points to the first entry in the ring buffer
	 *	ring_end points to the last entry in the ring buffer
	 */
	void				*ring_begin;
	void				*ring_end;
};

#define VIDEOINTIO_NODE_VERSION		1
struct videointio_node {
	struct devtree_node		dn;

	u32_t				version;

	struct videointio_regs		*regs;
};

struct videointio_ring_entry {
	/*
	 * Entry status (MUST BE FIRST)
	 */
	u32_t				status;

	/*
	 * Buffer
	 */
	void				*buffer;
};

#endif

