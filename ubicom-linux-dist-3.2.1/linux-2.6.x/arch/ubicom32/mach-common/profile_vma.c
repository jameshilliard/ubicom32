/*
 * arch/ubicom32/mach-common/profile_vma.c
 *   Implementation for Ubicom32 Profiler vma walker
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
#include <linux/mm.h>
#include <asm/profpkt.h>

int profile_get_vma(struct profile_map *profile_pm, int num, int profile_num_maps)
{
#if !defined(CONFIG_MMU)
	struct rb_node *rb __attribute__ ((unused));
	struct vm_region *region __attribute__ ((unused));
	int type __attribute__ ((unused)) = PROFILE_MAP_TYPE_UNKNOWN_USED;
	int flags __attribute__ ((unused));
	bool f_read, f_write, f_exec, f_shared, f_mayshare;
	down_read(&nommu_region_sem);
	for (rb = rb_first(&nommu_region_tree); rb && num < profile_num_maps - 1; rb = rb_next(rb)) {
		region = rb_entry(rb, struct vm_region, vm_rb);
		profile_pm[num].start = (region->vm_start - SDRAMSTART) >> PAGE_SHIFT;
		profile_pm[num].type_size = (region->vm_end - region->vm_start + (1 << PAGE_SHIFT) - 1) >> PAGE_SHIFT;
		flags = region->vm_flags;
		f_read = flags & VM_READ;
		f_write = flags & VM_WRITE;
		f_exec = flags & VM_EXEC;
		f_shared = flags & VM_SHARED;
		f_mayshare = flags & VM_MAYSHARE;
		if (f_read && !f_write && f_exec && f_mayshare) {
			type = PROFILE_MAP_TYPE_TEXT;
		} else if (f_read && f_write && f_exec  && !f_mayshare) {
			type = PROFILE_MAP_TYPE_STACK;
		} else if (f_read && f_write && !f_exec && !f_mayshare) {
			type = PROFILE_MAP_TYPE_APP_DATA;
		} else if (f_read && !f_write && !f_exec && f_mayshare) {
			type = PROFILE_MAP_TYPE_READ_SHARED;
		} else if (f_read && f_write && !f_exec && f_shared) {
			type = PROFILE_MAP_TYPE_ASHMEM;
		} else {
			type = PROFILE_MAP_TYPE_UNKNOWN_USED;
		}
		profile_pm[num].type_size |= type << PROFILE_MAP_TYPE_SHIFT;
		num++;
		if (region->vm_top - region->vm_end >= 1 << PAGE_SHIFT) {
			profile_pm[num].start = (region->vm_end - SDRAMSTART) >> PAGE_SHIFT;
			profile_pm[num].type_size = (region->vm_top - region->vm_end + (1 << PAGE_SHIFT) - 1) >> PAGE_SHIFT;
			profile_pm[num].type_size |= PROFILE_MAP_TYPE_VMA_WASTE << PROFILE_MAP_TYPE_SHIFT;
		}
	}
	up_read(&nommu_region_sem);
	if (rb) {
		return -1;
	}

#endif
	return num;
}
