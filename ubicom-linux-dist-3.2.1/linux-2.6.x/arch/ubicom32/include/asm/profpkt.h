
/*
 * arch/ubicom32/include/asm/profpkt.c
 *   Ubicom32 Profiler packet formats for communication between the linux proc driver and the profiler display tool
 *
 * IMPORTANT!  There is a parallel verison of this file in the tools in tools/profiler/src.  They must agree
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

#define PROFILE_PORT 51080
#define PROFILE_CONTROL_PORT 51081
#define PROFILE_POSIX_NAME_LENGTH 32

/*
 * profile UDP packet format for communicating between ip3k and host
 *
 * every packet starts with a header, followed by samples.
 * samples are only taken for non-hrt threads that are
 * active
 */
#define PROF_MAGIC 0x3ea0
#define PROF_MAGIC_COUNTERS 0x9ea0
#define PROF_MAGIC_MAPS 0xaea0

/*
 * Versions (31 max):
 * 1 to 4 were before 6.0 release,  development versions
 * 5 was forward compatible version, shipped with 6.0 and 6.1
 * 6 adds heap packets, and clock_freq to header, shipped with 6.2
 * 7 adds a sequence numbers to check for dropped packets, shipped with 6.3.5
 * 8 adds mqueue timing information, shipped with 6.3.5
 * 9 adds sdram heap size information, shipped with 6.4
 * 10 adds heapmem heap callers and long latency stack traces.  shipped with 6.4
 * 11 adds support for Mars (IP5K).  shipped with 6.10
 * 12 adds more support for Mars.  Shipped with 7.0
 * 13 adds per sample latency measurement.  Shipped with 7.2
 * 14 changes the heap format and adds a string packet.  Shipped with 7.4
 * 15 adds dsr stats and posix.  shipped with 7.6
 * 16 corrects maximum packet count for Ares.  ships with 7.9
 * 17 adds a5 register value to sample
 * 18 adds counter support and removes unused header fields
 * 19 adds PID support for MMU profiling
 * 20 changes the protocol for transmitting map PID maps automatically
 * 21 adds support for multiple possible parents, configurable
 */

#define PROFILE_VERSION 21
#define PROFILE_MAX_PACKET_SIZE 1440

#define PROFILE_MAX_THREADS 12

#define PROFILE_COUNTERS 8

/*
 * each packet starts with a profile_header, then sample_count samples
 * samples are gprof samples of pc, the return address, condition codes, and
 * active threads
 */
struct profile_header {
	unsigned short magic;			/* magic number and version */
	unsigned char header_size;		/* number of bytes in profile header */
	unsigned char sample_count;		/* number of samples in the packet */
	unsigned int clocks;			/* clock counter value */
	unsigned int instruction_count[PROFILE_MAX_THREADS];
					/* instructions executed per thread */
	unsigned int profile_instructions;	/* instructions executed by profiler mainline */
	unsigned short enabled;			/* which threads are enabled */
	unsigned short hrt;			/* which threads are hrt */
	unsigned short high;			/* which threads are high priority */
	unsigned short profiler_thread;		/* which thread runs the profiler */
	unsigned int clock_freq;		/* clock frequency (Hz) of system being analyzed */
	unsigned int seq_num;			/* to detect dropped profiler packets */
	unsigned int cpu_id;			/* CHIP_ID register contents */
	unsigned int ddr_freq;			/* DDR clock frequency */
	unsigned int perf_counters[PROFILE_COUNTERS];		/* contents of the CPU performance counters */
	unsigned char perf_config[PROFILE_COUNTERS];		/* what is being counted */
	unsigned int sample_stack_words;	/* number of stack words in the sample */
};

struct profile_header_counters {
	unsigned short magic;
	unsigned short ultra_count;		/* how many ultra counters follow this */
	unsigned int ultra_sample_time;	/* in chip clocks */
	unsigned int linux_count;		/* how many linux counters follow this */
	unsigned int linux_sample_time;
};

/*
 * values chosen so all counter values fit in a single 1400 byte UDP packet
 */
#define PROFILE_COUNTER_NAME_LENGTH 20
#define PROFILE_MAX_COUNTERS ((PROFILE_MAX_PACKET_SIZE - sizeof(struct profile_header_counters)) / (PROFILE_COUNTER_NAME_LENGTH + 4))

struct profile_counter_pkt {
	char name[PROFILE_COUNTER_NAME_LENGTH];
	unsigned int value;
};

/*
 * send memory maps from linux to profiler tool
 */

struct profile_header_maps {
	unsigned short magic;			/* magic number and last packet bit */
	unsigned short count;
	unsigned int page_shift;
};

#define PROFILE_MAP_NUM_TYPES 16

/* size field is pages.  True size in bytes is (1 << PAGE_SHIFT) * size */
#define PROFILE_MAP_TYPE_FREE 0
#define PROFILE_MAP_TYPE_SMALL 1
#define PROFILE_MAP_TYPE_FS 2
#define PROFILE_MAP_TYPE_UNKNOWN_USED 4
#define PROFILE_MAP_TYPE_TEXT 5
#define PROFILE_MAP_TYPE_STACK 6
#define PROFILE_MAP_TYPE_APP_DATA 7
#define PROFILE_MAP_TYPE_ASHMEM 8
#define PROFILE_MAP_TYPE_READ_SHARED 9
#define PROFILE_MAP_TYPE_CACHE 10
#define PROFILE_MAP_TYPE_VMA_WASTE 11
#define PROFILE_MAP_RESERVED 15

#define PROFILE_MAP_TYPE_SHIFT 12
#define PROFILE_MAP_SIZE_MASK 0xfff

struct profile_map {
	unsigned short start;		/* start page number of segment, relative to start of OCM (Max 256 MB on IP7K or 1 GB on IP8K, plus 256 KB OCM) */
	unsigned short type_size;	/* type (4 bits) of the segment and size (12 bits) in pages. A size of 0 means 4K pages */
};

#define PROFILE_MAX_MAPS (PROFILE_MAX_PACKET_SIZE - sizeof(struct profile_header_maps)) / sizeof(struct profile_map)
