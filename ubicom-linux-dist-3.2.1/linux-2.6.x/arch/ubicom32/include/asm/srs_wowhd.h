/*
 * arch/ubicom32/include/asm/srs_wowhd_node.h
 *   Header file SRS Wow HD control.
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

#ifndef SRS_WOWHD_NODE_H
#define SRS_WOWHD_NODE_H

#include <asm/devtree.h>
#define SRS_WOWHD_VP_VERSION			1

#define SRS_WOWHD_FLAG_DEFINITION_ENABLE	(1 << 0)
#define SRS_WOWHD_FLAG_TRUBASS_ENABLE		(1 << 1)
#define SRS_WOWHD_FLAG_CLARITY_ENABLE		(1 << 2)
#define SRS_WOWHD_FLAG_WOWHD_ENABLE		(1 << 3)
#define SRS_WOWHD_FLAG_VOLUMECONTROL_ENABLE	(1 << 4)
#define SRS_WOWHD_FLAG_VC_NORMALIZER_ENABLE	(1 << 5)

enum srs_wowhd_modes {
	SRS_WOWHD_3D_MONO,
	SRS_WOWHD_3D_SINGLE_SPEAKER,
	SRS_WOWHD_3D_STEREO,
	SRS_WOWHD_EXTREME,
};

struct srs_wowhd_regs {
	u32_t			version;

	u32_t			flags;

	enum srs_wowhd_modes	wowhd_3d_mode;
	u16_t			wowhd_3d_space;
	u16_t			wowhd_3d_center;
	u16_t			vc_input_gain;
	u16_t			vc_output_gain;
	u16_t			vc_max_gain;
};

#define SRS_WOWHD_NODE_VERSION			1
struct srs_wowhd_node {
	struct devtree_node	dn;

	u32_t			version;

	u8_t			sendirq;

	struct srs_wowhd_regs	*regs;
};

#endif // SRS_WOWHD_NODE_H

