/*
 * arch/ubicom32/include/asm/ubicom32videoin.h
 *   Ubicom32 architecture video input
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
#ifndef _ASM_UBICOM32_UBICOM32VIDEOIN_H
#define _ASM_UBICOM32_UBICOM32VIDEOIN_H

#include <linux/ioctl.h>

/* 
 * Set Ring Buffer
 */
#define UBICOM32VIDEOIN_IOCTL_SET_RING_BUFFER_BEGIN	_IOW('w',  1, void *) 
#define UBICOM32VIDEOIN_IOCTL_SET_RING_BUFFER_END	_IOW('w',  2, void *) 

/*
 * Stop
 */
#define UBICOM32VIDEOIN_IOCTL_STOP			_IOW('w',  3, void *) 

/*
 * Start
 */
#define UBICOM32VIDEOIN_IOCTL_START			_IOW('w',  4, void *) 

/*
 * Get status
 */
#define UBICOM32VIDEOIN_IOCTL_GET_STATUS		_IOR('r',  5, unsigned int)

/*
 * Sampling parameters
 */
#define UBICOM32VIDEOIN_IOCTL_SET_HPIXELS		_IOR('r',  6, unsigned int)
#define UBICOM32VIDEOIN_IOCTL_SET_VLINES		_IOR('r',  7, unsigned int)

/*
 * Ring entry status bits
 */
#define	VIDEOINTIO_RING_STATUS_VALID		(1 << VIDEOINTIO_RING_STATUS_VALID_BIT)
#define	VIDEOINTIO_RING_STATUS_VALID_BIT	0
#define	VIDEOINTIO_RING_STATUS_LOOP		(1 << VIDEOINTIO_RING_STATUS_LOOP_BIT)
#define	VIDEOINTIO_RING_STATUS_LOOP_BIT		1

struct ubicom32videoin_ring_entry {
	/*
	 * Entry status
	 */
	u32_t				status;

	/*
	 * Buffer
	 */
	void				*buffer;
};

#endif /* _ASM_UBICOM32_UBICOM32VIDEOIN_H */
