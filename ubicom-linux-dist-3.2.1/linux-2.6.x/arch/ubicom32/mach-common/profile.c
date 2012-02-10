/*
 * arch/ubicom32/mach-common/profile.c
 *   Implementation for Ubicom32 Profiler
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

#include <linux/platform_device.h>
#include "profilenode.h"
#include <asm/profilesample.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/mmzone.h>
#include <linux/fs.h>
#include <linux/page-flags.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <asm/devtree.h>
#include <asm/page.h>
#include <asm/profile.h>
#include <asm/thread_info.h>
#include <asm/thread.h>

/*
 * This is the driver for the Ubicom system profiler.  The system interface to the driver is
 *      profile_register_performance_counter(), defined in <asm/profile.>
 *      a set of proc files (proc/profile/<*>), used by the profiler daemon
 *
 * communication between the profiler components is described in a set of header files.  There are
 * multiple versions of these files that must be kept synchronized:
 *	in ultra/pkg/ipProfile
 *	in tools/profiler
 *	in linux-2.6.x/arch/ubicom32/include/asm
 *
 * profilesample.h specifies the sample format used by ipProfile, profile driver, and ip3kprof (two versions)
 * profilenode.h specifies the driver node communication between ipProfile and the profile driver. (two versions)
 * profpkt.h specifies the network packet format between the profile driver, profile daemon, and ip3kprof (two versions)
 *
 *
 * ipProfile: the ultra profile sampler
 *	pkg/ipProfile/src/profile.c
 *	pkg/ipProfile/include/profilenode.h
 *	pkg/ipProfile/include/profilesample.h
 *
 * profile driver: this code
 *	linux-2.6.x/arch/ubicom32/mach-common/profile.c
 *	linux-2.6.x/arch/ubicom32/include/asm/profilesample.h
 *	
 * profilerd: the user daemon that sends data to the tool
 *	uClinux/user/profilerd/profilerd.c
 *
 * ip3kprof: the Windows tool
 *	tools/profiler/src/(many files)
 *
 */
extern int profile_get_vma(struct profile_map *, int, int);

/*
 * LINUX and Ultra counters must all fit in one packet
 */
#define PROFILE_LINUX_MAX_COUNTERS 40
static int profile_num_counters = 0;
static volatile unsigned int *profile_counter[PROFILE_LINUX_MAX_COUNTERS];
static char profile_name[PROFILE_LINUX_MAX_COUNTERS][PROFILE_COUNTER_NAME_LENGTH];

int profile_register_performance_counter(volatile unsigned int *counter, char *name)
{
	static DEFINE_SPINLOCK(profile_counter_lock);
	unsigned long flags;
 
	spin_lock_irqsave(&profile_counter_lock, flags);
	if (profile_num_counters >= PROFILE_LINUX_MAX_COUNTERS) {
		spin_unlock_irqrestore(&profile_counter_lock, flags);
		return 0;
	}
	profile_counter[profile_num_counters] = counter;
	strncpy(profile_name[profile_num_counters], name, PROFILE_COUNTER_NAME_LENGTH);
	profile_name[profile_num_counters][PROFILE_COUNTER_NAME_LENGTH - 1] = 0;
	profile_num_counters++;
	spin_unlock_irqrestore(&profile_counter_lock, flags);
	return 1;
}
EXPORT_SYMBOL(profile_register_performance_counter);

/*
 * space for all memory blocks so we can hold locks for short time when walking tables
 */
#define PROFILE_NUM_MAPS 5000
#define PROFILE_TMP_NUM_MAPS 1000
static struct profile_map profile_pm[PROFILE_NUM_MAPS];
static struct kmem_cache_size_info profile_pm_tmp[PROFILE_TMP_NUM_MAPS];

static struct profilenode *node = NULL;
static int profile_first_packet = 1;

static int profile_open(struct inode *inode, struct file *filp)
{
	if (!node) {
		return -ENOENT;
	}
	node->busy = 1;
	if (!node->enabled) {
		node->enabled = 1;
		node->busy = 0;
		profile_first_packet = 1;
		return 0;
	}
	node->busy = 0;
	return -EBUSY;
}

static int profile_sequence_num;

/*
 * make a packet full of sample data
 */
static int profile_make_data_packet(char *buf, int count)
{
	int samples;		/* number of samples requested */
	int i;
	struct profile_header ph;
	char *ptr;

	if (count < sizeof(struct profile_header) + sizeof(struct profile_sample)) {
		return -EINVAL;
	}

	/*  
	 * fill in the packet header
	 */
	memset(&ph, 0, sizeof(struct profile_header));
	ph.magic = PROF_MAGIC + PROFILE_VERSION;
	ph.header_size = sizeof(struct profile_header);
	ph.clocks = node->clocks;
	for (i = 0; i < PROFILE_MAX_THREADS; ++i) {
		ph.instruction_count[i] = node->inst_count[i];
	}
	ph.profile_instructions = 0;
	ph.enabled = node->enabled_threads;
	ph.hrt = node->hrt;
	ph.high = 0;
	ph.profiler_thread = node->profiler_thread;
	ph.clock_freq = processor_frequency();
	ph.seq_num = profile_sequence_num;
	ph.cpu_id = node->cpu_id;
	ph.perf_counters[0] = node->stats[0];
	ph.perf_counters[1] = node->stats[1];
	ph.perf_counters[2] = node->stats[2];
	ph.perf_counters[3] = node->stats[3];
	ph.perf_counters[4] = node->stats[4];
	ph.perf_counters[5] = node->stats[5];
	ph.perf_counters[6] = node->stats[6];
	ph.perf_counters[7] = node->stats[7];
	ph.ddr_freq = processor_ddr_frequency();
	ph.sample_stack_words = PROFILE_STACK_WORDS;

	ptr = buf + sizeof(struct profile_header);

	samples = (count - sizeof(struct profile_header)) / sizeof(struct profile_sample);
	for (i = 0; i < samples && node->count; ++i) {
		if (copy_to_user(ptr, &node->samples[node->tail], sizeof(struct profile_sample)) != 0) {
			return -EFAULT;
		}
		node->count--;
		node->tail++;
		if (node->tail >= node->max_samples) {
			node->tail = 0;
		}
		ptr += sizeof(struct profile_sample);
	}
	ph.sample_count = i;
	if (copy_to_user(buf, &ph, sizeof(struct profile_header)) != 0) {
		return -EFAULT;
	}
	if (ph.sample_count == 0) {
		return 0;
	} else {
		profile_sequence_num++;
		return sizeof(struct profile_header) + ph.sample_count * sizeof(struct profile_sample);
	}
}

static void profile_get_memory_stats(unsigned int *total_free, unsigned int *max_free)
{
	struct zone *zone;
	unsigned int size;
#ifdef CONFIG_PAGE_ALLOC2
	struct page *page;
#else
	struct list_head *p;
#endif

	*total_free = 0;
	*max_free = 0;

	/*
	 * get all the free regions.  In each zone, the array of free_area lists contains 
         * the first page of each frame of size 1 << order
	 */
	for_each_zone(zone) {
#ifndef CONFIG_PAGE_ALLOC2
		unsigned long order, i;
#endif
		unsigned long flags;

		if (!populated_zone(zone))
			continue;
		
		if (!is_normal(zone))
			continue;

#ifndef CONFIG_PAGE_ALLOC2
		spin_lock_irqsave(&zone->lock, flags);
		for_each_migratetype_order(order, i) {
			size = ((1 << order) << PAGE_SHIFT) >> 10;
			list_for_each(p, &(zone->free_area[order].free_list[i])) {
				if (size > *max_free) {
					*max_free = size;
				}
				*total_free += size;
			}
		}
		spin_unlock_irqrestore(&zone->lock, flags);
#else /* !CONFIG_PAGE_ALLOC2 */
		spin_lock_irqsave(&zone->lock, flags);
		list_for_each_entry(page, &(zone->free_area2.free_list), free_entry.entry) {
			size = page->free_entry.size;
			if (size > *max_free) {
				*max_free = size;
			}
			
			*total_free += size;
		}
		spin_unlock_irqrestore(&zone->lock, flags);
#endif /* !CONFIG_PAGE_ALLOC2 */
	}
}

struct profile_counter_pkt profile_builtin_stats[] = 
{
	{
	"Free memory(KB)", 0
	},
	{
	"Max free Block(KB)", 0
	}
};

/*
 * make a packet full of performance counters
 */
static char prof_pkt[PROFILE_MAX_PACKET_SIZE];
static int profile_make_stats_packet(char *buf, int count)
{
	char *ptr = prof_pkt;
	struct profile_header_counters hdr;
	int stat_count = 0;
	int i;
	unsigned int total_free, max_free;
	int builtin_count = sizeof(profile_builtin_stats) / sizeof(struct profile_counter_pkt);
	struct profile_counter counter;
	
	if (count > PROFILE_MAX_PACKET_SIZE) {
		count = PROFILE_MAX_PACKET_SIZE;
	}
	stat_count = (count - sizeof(struct profile_header_counters)) / sizeof (struct profile_counter_pkt);
	stat_count -= builtin_count;

	if (stat_count <= 0) {
		return 0;
	}

	if (stat_count > node->num_counters + profile_num_counters) {
		stat_count = node->num_counters + profile_num_counters;
	}

	hdr.magic = PROF_MAGIC_COUNTERS;
	hdr.ultra_sample_time = node->clocks;
	hdr.ultra_count = stat_count;
	hdr.linux_sample_time = UBICOM32_IO_TIMER->sysval;
	hdr.linux_count = builtin_count;
	memcpy(ptr, (void *)&hdr, sizeof(struct profile_header_counters));
	ptr += sizeof(struct profile_header_counters);

	for (i = 0; i < node->num_counters && i < stat_count; ++i) {
		memcpy(ptr, (void *)(&(node->counters[i])), sizeof(struct profile_counter));
		ptr += sizeof(struct profile_counter);
	}

	for (i = 0; i < profile_num_counters && i + node->num_counters < stat_count; ++i) {
		counter.value = *(profile_counter[i]);
		strcpy(counter.name, profile_name[i]);
		memcpy(ptr, (void *)(&counter), sizeof(struct profile_counter));
		ptr += sizeof(struct profile_counter);
	}

	/*
	 * built in statistics
	 */
	profile_get_memory_stats(&total_free, &max_free);
	profile_builtin_stats[0].value = total_free;
	profile_builtin_stats[1].value = max_free;
	memcpy(ptr, (void *)profile_builtin_stats, sizeof(profile_builtin_stats));
	ptr += sizeof(profile_builtin_stats);

	if (copy_to_user(buf, prof_pkt, ptr - prof_pkt) != 0) {
		return -EFAULT;
	}
	return ptr - prof_pkt;
}

/*
 * return a udp packet ready to send to the profiler tool
 * when there are no packets left to make, return 0
 */
static int profile_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	int result = 0;
	if (!node) {
		return -ENOENT;
	}
	node->busy = 1;
	if (!node->enabled) {
		node->busy = 0;
		return -EPERM;
	}
	if (!node->samples) {
		node->busy = 0;
		return -ENOMEM;
	}

	if (profile_first_packet) {
		result = profile_make_stats_packet(buf, count);
		profile_first_packet = 0;
	} 
	if (result == 0) {
		result = profile_make_data_packet(buf, count);
		if (result == 0) {
			profile_first_packet = 1;
		}
	}
	node->busy = 0;
	return result;

}

static int profile_release(struct inode *inode, struct file *filp)
{
	if (!node) {
		return -ENOENT;
	}
	node->busy = 1;
	if (node->enabled) {
		node->enabled = 0;
		node->count = 0;
		node->tail = node->head;
		node->busy = 0;
		return 0;
	}
	node->busy = 0;
	profile_first_packet = 1;
	return -EBADF;
}

static const struct file_operations profile_fops = {
	.open		= profile_open,
	.read		= profile_read,
	.release	= profile_release,
};

static int page_aligned(void *x)
{
	return !((unsigned int)x & ((1 << PAGE_SHIFT) - 1));
}

static int add_info_to_page_list(struct profile_map *data, int idx, int max_data, 
				 unsigned int page_num, unsigned int size, unsigned int type)
{
	while (size > 0)
	{
		if (idx >= max_data)
			return idx;

		data[idx].start = page_num;
		
		if (size >= (1 << 12)) {
			data[idx].type_size = (type << PROFILE_MAP_TYPE_SHIFT);
			page_num += (1 << 12);
			size -= (1 << 12);
		} else {
			data[idx].type_size = size | (type << PROFILE_MAP_TYPE_SHIFT);
			size = 0;
		}

		idx++;
	}

	return idx;
}

#if 0
// for debugging, if needed
static void dump_page_list(struct profile_map *data, int count)
{
	int idx;
	
	for (idx = 0; idx < count; idx++) {
		printk("PAGE type %2d: start %08x, length %08x\n", 
		       (data[idx].type_size >> PROFILE_MAP_TYPE_SHIFT),
		       data[idx].start << PAGE_SHIFT,
		       (data[idx].type_size & PROFILE_MAP_SIZE_MASK) << PAGE_SHIFT);
	}
}
#endif

static int get_kmem_cache_block_info(char *name, struct profile_map *pm, int idx, unsigned int type)
{
	int num;
	int i;
	
	num = kmem_cache_block_info(name, profile_pm_tmp, PROFILE_TMP_NUM_MAPS);
	for (i = 0; i < num; i++) {
		idx = add_info_to_page_list(profile_pm, idx, PROFILE_NUM_MAPS, profile_pm_tmp[i].page, 1 << profile_pm_tmp[i].order, type);
	}

	return idx;
}

static int profile_maps_open(struct inode *inode, struct file *filp)
{
	int slab_start __attribute__ ((unused));
	int flags __attribute__ ((unused));
	int i __attribute__ ((unused));

	unsigned long page_offset;
	unsigned long order;

	int num = 0;
	struct list_head *p;
	struct zone *zone;

#ifdef CONFIG_PAGE_ALLOC2
	int num_orig = 0;
	unsigned long remaining_size;
	unsigned long order_size;
	unsigned long order_mask;
#endif

	/*
	 * get the slab data (first so dups will show up as vmas)
	 */
#if defined(CONFIG_SLAB)
#if defined(CONFIG_SLABINFO)
	slab_start = num;
	num = get_kmem_cache_block_info("size-512", profile_pm, num, PROFILE_MAP_TYPE_SMALL);
	num = get_kmem_cache_block_info("size-1024", profile_pm, num, PROFILE_MAP_TYPE_SMALL);
	num = get_kmem_cache_block_info("size-2048", profile_pm, num, PROFILE_MAP_TYPE_SMALL);
	num = get_kmem_cache_block_info("size-4096", profile_pm, num, PROFILE_MAP_TYPE_SMALL);
	num = get_kmem_cache_block_info("size-8192", profile_pm, num, PROFILE_MAP_TYPE_SMALL);

	slab_start = num;
	num = get_kmem_cache_block_info("dentry", profile_pm, num, PROFILE_MAP_TYPE_FS);
	num = get_kmem_cache_block_info("inode_cache", profile_pm, num, PROFILE_MAP_TYPE_FS);
	num = get_kmem_cache_block_info("sysfs_dir_cache", profile_pm, num, PROFILE_MAP_TYPE_FS);
	num = get_kmem_cache_block_info("unionfs_inode_cache", profile_pm, num, PROFILE_MAP_TYPE_FS);
	num = get_kmem_cache_block_info("unionfs_dentry", profile_pm, num, PROFILE_MAP_TYPE_FS);
	num = get_kmem_cache_block_info("proc_inode_cache", profile_pm, num, PROFILE_MAP_TYPE_FS);
#endif
#endif

	/*
	 * get all the vma regions (allocated by mmap, most likely
	 */
	num = profile_get_vma(profile_pm, num, PROFILE_NUM_MAPS);
	if (num == -1) {
		return -ENOMEM;
	}

	/*
	 * get all the free regions.  In each zone, the array of free_area lists contains the first page of each frame of size 1 << order
	 */
	for_each_zone(zone) {
		unsigned long flags, i;
		struct page *page;

		if (!populated_zone(zone))
			continue;
		
		if (!is_normal(zone))
			continue;

#ifndef CONFIG_PAGE_ALLOC2
		spin_lock_irqsave(&zone->lock, flags);
		for_each_migratetype_order(order, i) {
			list_for_each(p, &(zone->free_area[order].free_list[i])) {
				page = list_entry(p, struct page, lru);
				page_offset = ((page_to_phys(page) - SDRAMSTART) >> PAGE_SHIFT);
				num = add_info_to_page_list(profile_pm, num, PROFILE_NUM_MAPS,
							    page_offset, (1 << order),
							    PROFILE_MAP_TYPE_FREE);
			}
		}
		spin_unlock_irqrestore(&zone->lock, flags);
#else /* !CONFIG_PAGE_ALLOC2 */
		spin_lock_irqsave(&zone->lock, flags);
		list_for_each_entry(page, &(zone->free_area2.free_list), free_entry.entry) {
			page_offset = ((page_to_phys(page) - SDRAMSTART) >> PAGE_SHIFT);

			num = add_info_to_page_list(profile_pm, num, PROFILE_NUM_MAPS,
						    page_offset, page->free_entry.size,
						    PROFILE_MAP_TYPE_FREE);
		}
		spin_unlock_irqrestore(&zone->lock, flags);
#endif /* !CONFIG_PAGE_ALLOC2 */
	}

	/*
	 * get the filesystem inodes
	 */
	list_for_each(p, &(super_blocks)) {
		struct super_block *sb;
		struct list_head *q;
		if (num >= PROFILE_NUM_MAPS)
			break;
		sb = list_entry(p, struct super_block, s_list);
		if (page_aligned(sb)) {
			profile_pm[num].start = ((unsigned int)sb - SDRAMSTART) >> PAGE_SHIFT;
			profile_pm[num].type_size = 1 | (PROFILE_MAP_TYPE_FS << PROFILE_MAP_TYPE_SHIFT);
			num++;
		}
		list_for_each(q, &(sb->s_inodes)) {
			struct inode *in;
			if (num >= PROFILE_NUM_MAPS)
				break;
			in = list_entry(q, struct inode, i_sb_list);
			if (page_aligned(in)) {
				profile_pm[num].start = ((unsigned int)in - SDRAMSTART) >> PAGE_SHIFT;
				profile_pm[num].type_size = 1 | (PROFILE_MAP_TYPE_FS << PROFILE_MAP_TYPE_SHIFT);
				num++;
			}
		}
	}

	/*
	 * get the buffer cache pages
	 */
	for (i = 0; i < num_physpages && num < PROFILE_NUM_MAPS; ++i) {
		if ((mem_map + i)->flags & (1 << PG_lru)) {
			int start = i;
			while ((mem_map + i)->flags & (1 << PG_lru) && i < num_physpages)
				i++;
			profile_pm[num].start = start - (OCMSIZE >> PAGE_SHIFT);
			profile_pm[num].type_size = (i - start) | (PROFILE_MAP_TYPE_CACHE << PROFILE_MAP_TYPE_SHIFT);
			num++;
		}
	}

/*	dump_page_list(profile_pm, num); */

	filp->private_data = (void *)num;
	return 0;
}

/*
 * return one packet of map data, or 0 if all maps have been returned already
 */
static int profile_maps_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	struct profile_header_maps header;
	char *p = buf + sizeof(header);
	int total = (int)filp->private_data;

	header.count = (count - sizeof(header)) / sizeof(struct profile_map);
	if (header.count > PROFILE_MAX_MAPS) {
		header.count = PROFILE_MAX_MAPS;
	}
	if (header.count > total - *f_pos) {
		header.count = total - *f_pos;
	}

	if (header.count == 0) {
		return 0;
	}

	header.magic = PROF_MAGIC_MAPS;
	header.page_shift = PAGE_SHIFT;
	if (*f_pos + header.count == total) {
		header.magic++;		// set last_packet bit
	}

	if (copy_to_user(buf, &header, sizeof(header)) != 0) {
		return -EFAULT;
	}
	if (copy_to_user(p, (void *)&profile_pm[*f_pos], sizeof(struct profile_map) * header.count) != 0) {
		return -EFAULT;
	}
	*f_pos += header.count;

	return sizeof(header) + sizeof(struct profile_map) * header.count;
}

static int profile_maps_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static const struct file_operations profile_maps_fops = {
	.open		= profile_maps_open,
	.read		= profile_maps_read,
	.release	= profile_maps_release,
};

static int profile_rate_show(struct seq_file *m, void *v)
{
	if (node) {
		seq_printf(m, "%d samples per second.  %d ultra, %d linux virtual counters.  %d dropped samples.  %d queued of %d max sampels.  %d sent packets.\n", 
			node->rate, node->num_counters, profile_num_counters, node->dropped_samples, node->count, node->max_samples, profile_sequence_num);
	} else {
		seq_printf(m, "Profiler is not initialized.\n");
	}
	return 0;
}

static int profile_rate_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, profile_rate_show, NULL);
}

static int profile_rate_write(struct file *filp, const char *buf, size_t len, loff_t *off)
{
	*off = 0;
	return 0;
}

static const struct file_operations profile_rate_fops = {
	.open		= profile_rate_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
	.write		= profile_rate_write,
};

int ubi32_profile_init_module(void)
{
	struct proc_dir_entry *pdir;

	/*
	 * find the device
	 */
	node = (struct profilenode *)devtree_find_node("profiler");
	if (!node) {
		printk(KERN_INFO "ipProfile (profiling) is not enabled in Ultra.  Profiler /proc interface is disabled.\n");
		return -ENODEV;
	}

	/*
	 * allocate the sample buffer
	 */
	node->max_samples = PROFILE_MAX_SAMPLES;
	node->samples = kmalloc(node->max_samples * sizeof(struct profile_sample), GFP_KERNEL);
	if (!node->samples) {
		printk(KERN_INFO "Profiler sample buffer kmalloc failed.\n");
		return -ENOMEM;
	}
	node->sw_ksp_ptr = sw_ksp;
	node->task_offset = offsetof(struct thread_info, task);
	node->pid_offset = offsetof(struct task_struct, pid);

	/*
	 * sw_ksp is an array of pointers to struct thread_info, the current task executing for each linux virtual processor
	 */
	node->sw_ksp_ptr = sw_ksp;
	node->task_offset = offsetof(struct thread_info, task);
	node->pid_offset = offsetof(struct task_struct, tgid);

	/*
	 * connect to the file system
	 */
	pdir = proc_mkdir("profile", NULL);
	if (!pdir) {
		return -ENOMEM;
	}
	if (!proc_create("data", 0, pdir, &profile_fops)) {
		return -ENOMEM;
	}
	if (!proc_create("rate", 0, pdir, &profile_rate_fops)) {
		return -ENOMEM;
	}
	if (!proc_create("maps", 0, pdir, &profile_maps_fops)) {
		return -ENOMEM;
	}

	profile_register_performance_counter(&profile_sequence_num, "Profile driver data packets");
	return 0;
}


module_init(ubi32_profile_init_module);

MODULE_AUTHOR("David Fotland");
MODULE_LICENSE("GPL");
