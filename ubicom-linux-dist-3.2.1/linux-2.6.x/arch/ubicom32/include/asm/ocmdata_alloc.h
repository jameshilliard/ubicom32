/*
 * ocm_data_alloc.h
 *   Ubicom32 data allocation from OCM
 *
 * (C) Copyright 2000-2011, Ubicom, Inc.
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
#ifndef _ASM_UBICOM32_OCM_DATA_ALLOC_H
#define _ASM_UBICOM32_OCM_DATA_ALLOC_H

/*
 * Opaque reference to a heap block.
 */
struct ocmdata_block;

/*
 * Structure and functions for heap statistics
 */
#define OCMDATA_STATISTICS_VERSION_NUMBER	1

struct ocmdata_low_water {
	size_t low_water_mark;
	u32_t low_water_time;
	u32_t last_reset_time;
};

struct ocmdata_statistics {
	u16_t version;
	u32_t timestamp;
	size_t total_size;
	size_t bytes_free;
	struct ocmdata_low_water low_water;
};

/*
 * Per-pool information.
 */
struct ocmdata_info {
	void *ocmdata_start;		/* start of the ocmdata in data memory */
	u32_t ocmdata_size;		/* size of the ocmdata in data memory */
	u16_t bytes_before;		/* infrastructure bytes before each ocmdata block */
	u16_t bytes_after;		/* infrastructure bytes after each ocmdata block */
};

/*
 * Heap functions.
 */
extern void *ocmdata_alloc(size_t size);
extern void *ocmdata_alloc_and_zero(size_t size);
extern void *ocmdata_realloc(void *ptr, size_t size);
extern char *ocmdata_strdup(const char *s);
extern void ocmdata_free(void *block);
extern size_t ocmdata_get_size(void *block);
extern size_t ocmdata_get_alignment(void);
extern size_t ocmdata_get_total(void);
extern size_t ocmdata_get_free(void);
extern size_t ocmdata_get_low_water(void);
extern void ocmdata_reset_low_water(void);
extern size_t ocmdata_get_and_reset_low_water(void);
extern void ocmdata_init(unsigned long addr, size_t sz);
extern struct ocmdata_block *ocmdata_get_first_ocmdata_block(void);
extern struct ocmdata_block *ocmdata_block_get_next(struct ocmdata_block *hb);
extern size_t ocmdata_block_get_size(struct ocmdata_block *hb);
extern void *ocmdata_block_get_pointer(struct ocmdata_block *hb);
extern void ocmdata_get_info(struct ocmdata_info *info);
extern int ocmdata_get_statistics(struct ocmdata_statistics *stats);
extern int ocmdata_get_low_water_statistics(struct ocmdata_low_water *hlw);
extern int ocmdata_get_and_reset_low_water_statistics(struct ocmdata_low_water *hlw);

#endif /* _ASM_UBICOM32_OCM_DATA_ALLOC_H */
