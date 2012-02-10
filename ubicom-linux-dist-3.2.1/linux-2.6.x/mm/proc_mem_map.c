/*
 * Creates a text file /proc/mem_map showing how allocated memory is
 * laid out, similar to the file of the same name in uClinux 2.4.x.
 *
 * This is useful for visualizing fragmentation in small
 * memory systems, particularly those without an MMU.
 *
 * Note that this is only suitable for small memory systems,
 * less than about 64MB.  With more memory, this file is too
 * large to be useful and you should use visualization tools.
 */

#include <linux/proc_fs.h>
#include <linux/mutex.h>
#include <linux/seq_file.h>
#include <linux/mm.h>

static DEFINE_MUTEX(mem_map_mutex);
static struct {
	unsigned long pfn, zone_start, zone_span;
	struct zone *zone;
	int addr_digits;
} mem_map_state;

static struct zone *find_lock_next_zone(unsigned long pfn_before)
{
	struct zone *zone, *best_match = NULL;

	for_each_zone(zone) {
		spin_lock(&zone->lock);
		if (!populated_zone(zone)
		    || !zone->spanned_pages
		    || pfn_before > zone->zone_start_pfn + zone->spanned_pages - 1
		    || (best_match != NULL
			&& best_match->zone_start_pfn < zone->zone_start_pfn)) {
			spin_unlock(&zone->lock);
			continue;
		}
		if (best_match != NULL)
			spin_unlock(&best_match->lock);
		best_match = zone;
		if (pfn_before >= zone->zone_start_pfn)
			break;
	}

	return best_match;
}

static struct zone *mem_map_track_pfn(void)
{
	struct zone *zone = mem_map_state.zone;

	if (zone != NULL) {
		if (mem_map_state.zone_start == zone->zone_start_pfn
		    && mem_map_state.zone_span == zone->spanned_pages
		    && mem_map_state.pfn >= mem_map_state.zone_start
		    && (mem_map_state.pfn - mem_map_state.zone_start
			< mem_map_state.zone_span)) {
			return zone;
		}
		spin_unlock(&zone->lock);
		mem_map_state.zone = NULL;
	}

	zone = find_lock_next_zone(mem_map_state.pfn);
	if (!zone)
		return NULL;

	mem_map_state.zone = zone;
	mem_map_state.zone_start = zone->zone_start_pfn;
	mem_map_state.zone_span = zone->spanned_pages;

	if (mem_map_state.pfn < zone->zone_start_pfn)
		mem_map_state.pfn = zone->zone_start_pfn;

	mem_map_state.addr_digits = 8;
	while ((mem_map_state.zone_start + (mem_map_state.zone_span - 1))
	       >> (4 * mem_map_state.addr_digits - PAGE_SHIFT))
		mem_map_state.addr_digits++;
	
	return zone;
}

static void mem_map_show_zone_header(struct seq_file *m, struct zone *zone)
{
	const char *zone_name = "type UNKNOWN";

	if (is_highmem(zone))
		zone_name = "ZONE_HIGHMEM";
	else if (is_dma32(zone))
		zone_name = "ZONE_DMA32";
	else if (is_dma(zone))
		zone_name = "ZONE_DMA";
	else if (is_normal(zone))
		zone_name = "ZONE_NORMAL";

	seq_printf(m, "Next zone at node %d, zone %ld (%s), starting at pfn 0x%lx\n\n",
		   zone->zone_pgdat->node_id, zone_idx(zone),
		   zone_name, zone->zone_start_pfn);
	seq_printf(m, "    ZONE_ALL_UNRECLAIMABLE = %d\n",
		   zone->all_unreclaimable);
	seq_printf(m, "    ZONE_RECLAIM_LOCKED = %d\n",
		   zone_is_reclaim_locked(zone));
	seq_printf(m, "    ZONE_OOM_LOCKED = %d\n",
		   zone_is_oom_locked(zone));
	seq_printf(m, "\n");
}

static char mem_map_char(unsigned long pfn, struct zone *zone)
{
	struct page *page;
	struct address_space *mapping;

	/* Janitorial TODO: This and pfn_to_page don't compile without <linux/mm.h>. */
	if (!pfn_valid(pfn))
		return ' ';

	page = pfn_to_page(pfn);

	/* #ifdef needed because this is not in 2.6.28. */
#ifdef CONFIG_ARCH_HAS_HOLES_MEMORYMODEL
	/* TODO: This is correct, but it still looks dodgy to me.
	   What if the memmap contents just happen to contain a pointer
	   which looks like the expected zone? */
	if (!memmap_valid_within(pfn, page, zone))
		return ' ';
#endif

	if (PageReserved(page))
		return 'X';

	/* Free pages. */
	if (!page_count(page))
		return '_';

	if (compound_head(page) != page)
		return '<';

	/* Kernel allocator pages. */
	if (PageSlab(page))
		return 'K';

	/* Anonymous pages. */
	/* Check: Should we be using page_is_file_cache? */
	if (PageAnon(page)) {
		if (PageError(page))
			return 'E';
		else if (PageReferenced(page) || PageDirty(page))
			return PageSwapCache(page) ? 'S' : 'A';
		else
			return PageSwapCache(page) ? 's' : 'a';
	}

	/* Inode-backed pages. */
	mapping = page_mapping(page);
#if 0
	if (page->mapping && page->mapping->a_ops) {
		/* 2.4.26 examines a_ops to decide what kind of device
		   is backing the page, and produce a corresponding letter. */
		return 'A';
	}
#endif

#if 0
	if (PageSwapCache(page))
		return 'x';

	if (PageCompound(page))
		return PageTail(page) ? 'c' : 'C';
#endif

	if (PageWriteback(page))
		return 'W';
	else if (PageError(page))
		return 'E';
	else if (PageDirty(page))
		return 'w';
	else if (!PageUptodate(page))
		return 'R';
	else if (PageLRU(page) && (PageActive(page) || PageUnevictable(page)))
		return 'r';
	
	return '?';
}

static unsigned long mem_map_pfn_line_end(void)
{
	unsigned long pfn = mem_map_state.pfn;
	unsigned long step = (mem_map_state.zone_span
			      - (pfn - mem_map_state.zone_start));

	if (likely(step >= 64))
		step = 64;

	pfn += step;
	if ((pfn & 63) < step)
		pfn -= (pfn & 63);

	return pfn;
}

static int mem_map_seq_show(struct seq_file *m, void *p)
{
	struct zone *zone = mem_map_state.zone;
	unsigned long pfn = mem_map_state.pfn;
	unsigned long pfn_line_end = mem_map_pfn_line_end();
	char buffer[16 + 3 + 64 + 1 + 1], *line = buffer;
	unsigned i;

	if (pfn == zone->zone_start_pfn)
		mem_map_show_zone_header(m, zone);

	line += sprintf(line, "%0*lx: ", mem_map_state.addr_digits,
					virt_to_phys(page_address(pfn_to_page(pfn))));
	for (i = 0; i < (pfn & 63); i++)
		*line++ = ' ';
	*line++ = '[';

	do
		*line++ = mem_map_char(pfn, zone);
	while (++pfn != pfn_line_end);

	*line++ = ']';
	*line++ = 0;

	seq_printf(m, "%s\n", buffer);
	return 0;
}

static void *mem_map_seq_next(struct seq_file *m, void *p, loff_t *pos)
{
	struct zone *zone = mem_map_state.zone;

	if (zone != NULL) {
		unsigned long pfn = mem_map_pfn_line_end();
		mem_map_state.pfn = pfn;
		if (unlikely(pfn == 0)) { /* Wrap check. */
			spin_unlock(&mem_map_state.zone->lock);
			zone = mem_map_state.zone = NULL;
			goto done;
		}
	}

	zone = mem_map_track_pfn();
done:
	*pos = (loff_t)mem_map_state.pfn;
	return zone ? &mem_map_state : NULL;
}

static void *mem_map_seq_start(struct seq_file *m, loff_t *pos)
{
	struct zone *zone;
	mutex_lock(&mem_map_mutex);

	mem_map_state.pfn = (unsigned long)*pos;
	mem_map_state.zone = NULL;

	zone = mem_map_track_pfn();
	*pos = (loff_t)mem_map_state.pfn;

	/*
	 * seq_file likes to increment the *pos by 1 instead of
	 * calling ->seq_next() when it isn't going to show the next
	 * item and just wants the index ready for another
	 * ->seq_start().  Because we're not incrementing the index in
	 * units of 1, detect when that's happened.
	 */
	if (zone != NULL
	    && (mem_map_state.pfn & 63) != 0
	    && mem_map_state.pfn != mem_map_state.zone_start) {
		return mem_map_seq_next(m, &mem_map_state, pos);
	}

	return zone ? &mem_map_state : NULL;
}

static void mem_map_seq_stop(struct seq_file *m, void *p)
{
	if (mem_map_state.zone != NULL) {
		spin_unlock(&mem_map_state.zone->lock);
		mem_map_state.zone = NULL;
	}
	mutex_unlock(&mem_map_mutex);
}

static const struct seq_operations mem_map_seq_ops = {
	.start		= mem_map_seq_start,
	.next		= mem_map_seq_next,
	.stop		= mem_map_seq_stop,
	.show		= mem_map_seq_show,
};

static int mem_map_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &mem_map_seq_ops);
}

static const struct file_operations mem_map_proc_ops = {
	.open		= mem_map_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= seq_release_private,
};

static struct proc_dir_entry *mem_map_proc_entry;

static int __init proc_mem_map_init(void)
{
	mem_map_proc_entry = proc_create("mem_map", S_IRUGO,
					 NULL, &mem_map_proc_ops);
	return mem_map_proc_entry ? 0 : -EINVAL;
}

static void __exit proc_mem_map_exit(void)
{
	if (mem_map_proc_entry)
		remove_proc_entry("mem_map", NULL);
}

module_init(proc_mem_map_init);
module_exit(proc_mem_map_exit);
