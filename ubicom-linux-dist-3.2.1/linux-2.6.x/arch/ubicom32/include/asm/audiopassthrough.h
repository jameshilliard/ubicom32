/*
 * audiopassthrough.h
 *        audio Passthrough
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
#ifndef AUDIOPASSTHROUGH_H
#define AUDIOPASSTHROUGH_H

#define AUDIO_PASSTHROUGH_REGS_VERSION	2

#define AUDIO_PASSTHROUGH_COMMAND_STOP	1
#define AUDIO_PASSTHROUGH_COMMAND_START	2
struct audio_passthrough_regs {
	u32_t				version;

	volatile u32_t			command;
	volatile u32_t			enabled;
	volatile u32_t			sample_rate;
};

#define AUDIO_PASSTHROUGH_NODE_VERSION	1
struct audio_passthrough_node {
	struct devtree_node		dn;
	u32_t				version;

	struct audio_passthrough_regs	*regs;
};

#define /* AUDIOPASSTHROUGH_H */
