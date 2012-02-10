/*
 * arch/ubicom32/include/asm/profile.h
 *	Sample format for profiling
 *
 * must be synchronized with the version at ultra/pkg/ipProfile/include/profilesample.h
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
 */

#ifndef _PROFILESAMPLE_H_
#define _PROFILESAMPLE_H_

#define PROFILE_STACK_WORDS 4

/*
 * Each sample is for an enabled thread, not including the profiling thread.
 * HRT thread sampling is optional.
 * Sampled threads may be active or inactive.  Samples are included in thread number
 * order, so each sample interval has a set of samples starting with one from thread 0
 *
 * Samples include bits indicating if this thread is blocked
 */
#define PROFILE_I_BLOCKED_BIT 5
#define PROFILE_I_BLOCKED (1 << PROFILE_I_BLOCKED_BIT)
#define PROFILE_D_BLOCKED_BIT 4
#define PROFILE_D_BLOCKED (1 << PROFILE_D_BLOCKED_BIT)
#define PROFILE_BTB_SHIFT 6

/*
 * a sample taken by the ipProfile package for sending to the profiler tool
 */
struct profile_sample {
	unsigned int pc;			/* PC value of the last instruction executed before the sample time */
	unsigned int pid;			/* pid for the current process, or 0 if NOMMU or unmapped space */
	unsigned short active;			/* which threads are active - for accurate counting */
	unsigned short d_blocked;		/* which threads are blocked due to D cache misses */
	unsigned short i_blocked;		/* which threads are blocked due to I cache misses */
	unsigned char cond_codes;		/* for branch prediction */
	unsigned char thread;			/* 4-bit thread number and I/D blocked status and BTB status */
	unsigned int a_reg;			/* source An if PC points to a calli.  Otherwise a5 contents for parent of leaf function */
	unsigned int parent[PROFILE_STACK_WORDS];			/* return address from stack, to find the caller */
};

#endif
