/*
 * ocm_data_alloc.c
 *	OCM (dynamic run-time memory) allocation routines.
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
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/jiffies.h>
#include <linux/string.h>
#include <asm/ocmdata_alloc.h>

#define TRUE	1
#define FALSE	0

/*
 * Configuration options:
 *
 * DEBUG_OCM_DATA_ALLOC		turns on debugging code
 * OCMDATA_GUARD_BAND_SIZE	> 0, turns on guard band to be this many bytes
 */
// #define DEBUG_OCM_DATA_ALLOC	3

/*
 * Runtime debug configuration
 */
#if defined(DEBUG_OCM_DATA_ALLOC)
#define OCMDATA_BLOCK_VERIFY_MAGIC(ob) ocmdata_block_verify_magic(ob)
#define DEBUG_ERROR(args...)	printk(args)
#define DEBUG_ASSERT(test, args...)	if (!(test)) { printk(args); BUG(); }

#if (DEBUG_OCM_DATA_ALLOC > 1)
#define DEBUG_WARN(args...)	printk(args)
#else
#define DEBUG_WARN(...)
#endif

#if (DEBUG_OCM_DATA_ALLOC > 2)
#define DEBUG_INFO(args...)	printk(args)
#else
#define DEBUG_INFO(...)
#endif

#if (DEBUG_OCM_DATA_ALLOC > 3)
#define DEBUG_TRACE(args...)	printk(args)
#else
#define DEBUG_TRACE(...)
#endif

#else
#define OCMDATA_BLOCK_VERIFY_MAGIC(ob)
#define DEBUG_TRACE(...)
#define DEBUG_ASSERT(...)
#define DEBUG_WARN(...)
#define DEBUG_ERROR(...)
#define DEBUG_INFO(...)
#endif

/*
 * Heap Bucket defines
 *
 * Do NOT change the bucket count unless you want to correct
 * ocmdata_pool_info to be correctly aligned
 */
#define OCMDATA_BUCKET_COUNT    4
#define OCMDATA_BUCKET_SHIFT    6
#define OCMDATA_BUCKET_MAX_SIZE (OCMDATA_BUCKET_COUNT << OCMDATA_BUCKET_SHIFT)

/*
 * Colour of the "paint" we use for the guard band and pre-initialization.
 */
#define OCMDATA_PRE_INIT_PAINT 0xcd
#define OCMDATA_GUARD_PAINT 0x77

#if defined(OCMDATA_GUARD_BAND_SIZE)
#if ((OCMDATA_GUARD_BAND_SIZE % IPHAL_DATA_ALIGN) != 0)
#error OCMDATA_GUARD_BAND_SIZE must be aligned to processor word size.
#endif
#else
#define OCMDATA_GUARD_BAND_SIZE 0
#endif

/*
 * Type definitions to make code more readable.
 */
typedef u32_t addr_t;

/*
 * Structure used to create a list of memory blocks.
 */
struct ocmdata_block {
	size_t size;			/* Size of the hole including this structure (always multiple of ALIGNMENT bytes) */
	size_t size_prev;		/* size of the hole before this one (by memory address) zero for first hole */
	u8_t flags;			/* Free or allocated, first block, or last block */
	struct ocmdata_block *next;	/* Pointer to the next similarly free or allocated block in memory */
	struct ocmdata_block *prev;	/* Pointer the previous similarly free or allocated block in memory */
	struct ocmdata_block *bucket_next;	/* Pointer to the next block in a bucket */

#if defined(DEBUG_OCM_DATA_ALLOC)
	size_t requested_size;		/* User requested size */
	u16_t magic;			/* Magic number to check if the ocmdata block is valid */
#endif
};

/*
 * Flags values.
 */
#define OCMDATA_FREE 1			/* Block is free */
#define OCMDATA_FIRST 2			/* First block in the list */
#define OCMDATA_LAST 4			/* Last block in the list */
#define OCMDATA_BUCKET 8		/* Block is in a fast-allocation bucket */

/*
 * Ocmdata pool information.
 */
struct ocmdata_pool_info {
	spinlock_t lock;		/* Lock - normally 32 bits! */
	size_t free_ram;		/* Amount of free memory in the pool */
	size_t total_ram;		/* Total amount of memory in the pool */
	size_t align;			/* Alignment requirement for the pool */
	size_t alloc_align;		/* Alignment requirement for the allocated chunk sizes */
	size_t bucket_shift;		/* Bit-shift required for bucket selection */
	struct ocmdata_block *bucket[OCMDATA_BUCKET_COUNT]; /* 16 buckets: 64, 128, 192, 256 .. 1024 */
	struct ocmdata_block *first_hole;	/* Pointer to the first unallocated block */
	struct ocmdata_block *first_block;	/* Pointer to the first allocated block */
	size_t low_water;		/* Lowest amount of free memory we've seen in the pool */
	u32_t low_water_time;		/* Time at which the low water mark was hit */
	u32_t low_water_reset;	/* Time at which the low water stats were last reset */
};

struct ocmdata_pool_info ocmdata_pool;

#define opi ((struct ocmdata_pool_info *)(&ocmdata_pool))

/*
 * ocmdata_verify_magic()
 */
#if defined(DEBUG_OCM_DATA_ALLOC)
#define OCMDATA_BLOCK_MAGIC 0x059e
static void ocmdata_block_verify_magic(struct ocmdata_block *ob)
{
	DEBUG_ASSERT(ob->magic == OCMDATA_BLOCK_MAGIC, "bad magic: %p\n", ob);
}
#endif

/*
 * ocmdata_block_get_size()
 *	Get the size of a ocmdata block.
 *
 * Lock should be held by caller
 */
size_t ocmdata_block_get_size(struct ocmdata_block *ob)
{
	OCMDATA_BLOCK_VERIFY_MAGIC(ob);

	return ob->size;
}

/*
 * ocmdata_block_get_pointer()
 *	Get a pointer to the allocated object associated with a ocmdata block.
 *
 * Lock should be held by caller
 */
void *ocmdata_block_get_pointer(struct ocmdata_block *ob)
{
	size_t align_mask = opi->align - 1;
	size_t ocmdata_block_offset = (sizeof(struct ocmdata_block) + align_mask) & ~align_mask;
	addr_t blk = ((addr_t)ob) + ocmdata_block_offset;

	OCMDATA_BLOCK_VERIFY_MAGIC(ob);

	return (void *)blk;
}

/*
 * ocmdata_block_get_next()
 *	Get pointer to next ocmdata block.
 *
 * Lock should be held by caller
 */
struct ocmdata_block *ocmdata_block_get_next(struct ocmdata_block *ob)
{
	struct ocmdata_block *ret = NULL;

	OCMDATA_BLOCK_VERIFY_MAGIC(ob);

	/*
	 * Is this the wrap-around?  If it is then we're done, but if not 
	 * then get the next pointer.
	 */
	if (ob->next != opi->first_block) {
		ret = ob->next;
	}

	return ret;
}

/*
 * debug_memory_info()
 *	Show memory information for the ocmdata.
 *
 * We use this function to dump memory statistics when we crash down due to
 * memory faults.  The aim is to trace how and where things have gone wrong.
 */
void debug_memory_info(void)
{
#if defined(DEBUG_OCM_DATA_ALLOC)
	struct ocmdata_block *mb;
	u32_t ct = 0;

	DEBUG_INFO("free memory: %u, total ocmdata: %u\n", opi->free_ram, opi->total_ram);

	mb = opi->first_block;
	while (mb) {
		DEBUG_INFO("alloc block: %p, sz: %u, sz_prv: %u, nxt: %p, prv: %p\n",
			   mb, mb->size, mb->size_prev, mb->next, mb->prev);
		mb = mb->next;
		if (mb == opi->first_block) {
			break;
		}

		ct++;
		if (ct > 500) {
			DEBUG_INFO("More left, but not displaying them!\n");
			break;
		}
	}

	mb = opi->first_hole;
	while (mb) {
		DEBUG_INFO("free block: %p, sz: %u, sz_prv: %u, nxt: %p, prv: %p\n",
			   mb, mb->size, mb->size_prev, mb->next, mb->prev);

		mb = mb->next;
		if (mb == opi->first_hole) {
			break;
		}

		ct++;
		if (ct > 500) {
			DEBUG_INFO("More left, but not displaying them!\n");
			break;
		}
	}
#endif
}

/*
 * ocmdata_get_info()
 *	Get ocmdata info.
 */
void ocmdata_get_info(struct ocmdata_info *info)
{
	unsigned long flags;
	size_t align_mask;

	info->ocmdata_start = 0;	//@@@ This is an assumption that is not necessarily true

	spin_lock_irqsave(&opi->lock, flags);
	info->ocmdata_size = opi->total_ram;
	align_mask = opi->align - 1;
	spin_unlock_irqrestore(&opi->lock, flags);

	info->bytes_before = (sizeof(struct ocmdata_block) + align_mask) & ~align_mask;
	info->bytes_after = OCMDATA_GUARD_BAND_SIZE;
}

/*
 * ocmdata_get_first_ocmdata_block()
 *	Get the first allocated ocmdata block.
 *
 * Lock should be held by caller
 */
struct ocmdata_block *ocmdata_get_first_ocmdata_block(void)
{
	return opi->first_block;
}

/*
 * ocmdata_alloc()
 *	Allocate a block of memory.
 *
 * ocmdata_alloc() allocates a block of size contiguous bytes from a ocmdata and
 * returns a pointer to the start of the block.  The memory is not cleared
 * before being returned.
 *
 * When the new block is allocated the system will have to allocate a small
 * amount of additional space contiguous with the requested block.  This space
 * will be used to store management information regarding the allocation so
 * that it can be freed back at some future time.  The additional space is not,
 * however, visible to the requester.
 *
 * This algorithm allocs from the oldest blocks first, putting freed blocks at the
 * end of the free list.  This is optimal for minimizing fragmentation, but has
 * a downside in that for cached memory it causes more cache misses when looking
 * for free blocks.  The algorithm could be changed to put the free block on the
 * front of the free list but that would harm OCM usage (and that's usually more
 * important).
 */
void *ocmdata_alloc(size_t size)
{
	unsigned long flags;
	size_t required;
	size_t alloc_align_mask;
	size_t data_required;
	size_t align_mask;
	size_t ocmdata_block_offset;
	struct ocmdata_block *mb;
	size_t mhsize;
	struct ocmdata_block *mh;
	size_t min_leftover;
	u8_t *blk;
#if OCMDATA_GUARD_BAND_SIZE > 0
	u8_t *user_end;
	u8_t *mb_end;
#endif

	/*
	 * If we've not been asked for any bytes then ensure we meet a
	 * minimum requirement.
	 */
	if (!size) {
		size = 1;
	}

	/*
	 * All allocations need to be "ocmdata_block" bytes larger than the
	 * amount requested by our caller.  They also need to be large enough
	 * that they can contain a "memory_hole" and any magic values used in
	 * debugging (for when the block gets freed and becomes an isolated
	 * hole).
	 */
	required = size;

#if defined(DEBUG_OCM_DATA_ALLOC)
	/*
	 * If we're debugging our dynamic allocations then we add a magic
	 * guard marker at the end of the block.  We can check this marker
	 * later to see if we've had any instances of writing past the end
	 * of the block.
	 */
	required += OCMDATA_GUARD_BAND_SIZE;
#endif

	/*
	 * Factor in any alignment requirements.
	 */
	spin_lock_irqsave(&opi->lock, flags);

	alloc_align_mask = opi->alloc_align - 1;
	data_required = (required + alloc_align_mask) & ~alloc_align_mask;

	align_mask = opi->align - 1;
	ocmdata_block_offset = (sizeof(struct ocmdata_block) + align_mask) & ~align_mask;
	required = ocmdata_block_offset + data_required;

	/*
	 * We have buckets for allocations up to 64 bytes, 128 bytes, ..., 256 bytes.
	 * If we have anything larger we allocate things from the ocmdata but otherwise
	 * we try a bucket first.
	 */
	if (data_required <= OCMDATA_BUCKET_MAX_SIZE) {
		struct ocmdata_block **bbp;
		size_t index = (data_required - opi->alloc_align) >> opi->bucket_shift;
		DEBUG_TRACE("Checking bucket %u for %u byte block (size %u)\n", 
			    index, data_required, size);

		bbp = &opi->bucket[index];
		mb = *bbp;
		if (mb) {
#if defined(OCMDATA_CLEAR_BLOCK_ON_FREE)
			u32_t *block;
			u32_t *end;
#endif

			/*
			 * We found an entry in the bucket so we can use it!
			 */
			*bbp = mb->bucket_next;
#if defined(DEBUG_OCM_DATA_ALLOC)
			/*
			 * Verify that this block is actually in a bucket
			 */
			if (!(mb->flags & OCMDATA_BUCKET)) {
				spin_unlock_irqrestore(&opi->lock, flags);
				DEBUG_ERROR("%p: ocmdata block from %u not in a bucket: %p\n", 
					    opi, index, mb);
				debug_memory_info();
				DEBUG_ASSERT(FALSE, "Stopped\n");
			}
#endif
			mb->flags &= ~OCMDATA_BUCKET;

#if defined(OCMDATA_CLEAR_BLOCK_ON_FREE)
			/*
			 * Bucket blocks are particularly susceptible to
			 * use-after-free bugs. Because of this when clear on free
			 * is enabled check all ocmdata allocs that are sourced from
			 * bucket for the correct free pattern (0).
			 */
			block = (u32_t *)((void *)mb + ocmdata_block_offset);
			end = (u32_t *)((void *)mb + mb->size);
			while (block < end) {
				DEBUG_ASSERT(*block == 0, "%p: use after free on block %p\n", 
					     mb, block);
				block++;
			}
#endif /* OCMDATA_CLEAR_BLOCK_ON_FREE */

			/*
			 * Got a block.. skip to the final allocation code
			 */
			goto allocated;
		}
	}

	/*
	 * We need to scan the list of all available memory holes and find the first
	 * one that meets our requirement.
	 */
	mhsize = 0;
	mh = opi->first_hole;

	/*
	 * Do we have any memory at all?
	 */
	if (mh) {
		/*
		 * Our algorithm is to allocate larger chunks from the head of the free list
		 * and smaller ones from the tail.  This means that we should end up with
		 * less fragmentation of the larger blocks and thus have more chance of
		 * making progress in systems that must run for months or years without reboots.
		 */
		if (data_required > OCMDATA_BUCKET_MAX_SIZE) {
			struct ocmdata_block *end_mh = mh;
			do {
				OCMDATA_BLOCK_VERIFY_MAGIC(mh);

#if defined(DEBUG_OCM_DATA_ALLOC)
				/*
				 * Verify that this block is actually free!
				 */
				if (!(mh->flags & OCMDATA_FREE)) {
					spin_unlock_irqrestore(&opi->lock, flags);
					DEBUG_ERROR("ocmdata unfree block: %p\n", mh);
					debug_memory_info();
					DEBUG_ASSERT(FALSE, "Stopped\n");
				}
#endif

				/*
				 * Does this free block meet our size needs?  If yes then we're
				 * done here.
				 */
				mhsize = mh->size;
				if (mhsize >= required) {
					break;
				}

				/*
				 * Move to the next block in the free list.  If this takes us
				 * back to the first one we looked at then we can do no more.
				 */
				mh = mh->next;
			} while (mh != end_mh);
		} else {
			struct ocmdata_block *end_mh;

			/*
			 * For a smaller block we need to start with the tail of the
			 * ocmdata free list.
			 */
			mh = mh->prev;

			end_mh = mh;
			do {
				OCMDATA_BLOCK_VERIFY_MAGIC(mh);

#if defined(DEBUG_OCM_DATA_ALLOC)
				/*
				 * Verify that this block is actually free!
				 */
				if (!(mh->flags & OCMDATA_FREE)) {
					spin_unlock_irqrestore(&opi->lock, flags);
					DEBUG_ERROR("ocmdata unfree block: %p\n", mh);
					debug_memory_info();
					DEBUG_ASSERT(FALSE, "Stopped\n");
				}
#endif

				/*
				 * Does this free block meet our size needs?  If yes then we're
				 * done here.
				 */
				mhsize = mh->size;
				if (mhsize >= required) {
					break;
				}

				/*
				 * Move to the previous block in the free list.  If this takes us
				 * back to the first one we looked at then we can do no more.
				 */
				mh = mh->prev;
			} while (mh != end_mh);
		}
	}

	/*
	 * Did we find any space available?  If no, then return NULL and quit.
	 */
	if (mhsize < required) {
		spin_unlock_irqrestore(&opi->lock, flags);
		DEBUG_WARN("Unable to alloc %u bytes\n", size);
		return NULL;
	}

	/*
	 * We found some space so now we remove a chunk of it.  If the space is large
	 * enough, we remove space from the end, and leave this chunk on the free list
	 */
	min_leftover = ocmdata_block_offset + ((OCMDATA_GUARD_BAND_SIZE + opi->alloc_align) & (~alloc_align_mask));

	mb = mh;
	if ((mh->size - required) >= min_leftover) {
		u8_t flags;

		DEBUG_TRACE("Splitting ocmdata block at %p\n", mh);

		flags = mh->flags;
		mh->flags &= ~OCMDATA_LAST;
		mh->size -= required;

		mb = (struct ocmdata_block *)(((addr_t)mh) + mh->size);
		mb->flags = flags & ~OCMDATA_FIRST;
		mb->size_prev = mh->size;
		mb->size = required;
		mb->next = mh->next;
		if (!(mb->flags & OCMDATA_LAST)) {
			struct ocmdata_block *next = (struct ocmdata_block *)(((addr_t)mb) + mb->size);
			next->size_prev = mb->size;
		}
	} else {
		DEBUG_TRACE("Using ocmdata block at %p\n", mh);

		required = mh->size;
		mh->prev->next = mh->next;
		mh->next->prev = mh->prev;
		if (opi->first_hole == mh) {
			if (mh->next == mh) {
				opi->first_hole = NULL;
			} else {
				opi->first_hole = mh->next;
			}
		}
	}

	if (!opi->first_block) {
		opi->first_block = mb;
		mb->prev = mb;
		mb->next = mb;
	} else {
		mb->next = opi->first_block;
		mb->prev = opi->first_block->prev;;
		opi->first_block->prev = mb;
		mb->prev->next = mb;
	}

	mb->flags &= ~OCMDATA_FREE;

#if defined(DEBUG_OCM_DATA_ALLOC)
	mb->magic = OCMDATA_BLOCK_MAGIC;
#endif

allocated:
	opi->free_ram -= required;
	if (opi->free_ram < opi->low_water) {
		opi->low_water = opi->free_ram;
		opi->low_water_time = jiffies;
	}

	blk = ((u8_t *)mb) + ocmdata_block_offset;

#if defined(DEBUG_OCM_DATA_ALLOC)
#if defined(OCMDATA_PAINT_BLOCK_ON_ALLOC)
	memset(blk, OCMDATA_PRE_INIT_PAINT, size);
#endif /* OCMDATA_PAINT_BLOCK_ON_ALLOC */

#if OCMDATA_GUARD_BAND_SIZE > 0
	mb->requested_size = size;
	user_end = blk + mb->requested_size;
	mb_end = (u8_t *)mb + mb->size;
	memset(user_end, OCMDATA_GUARD_PAINT, mb_end - user_end);
#endif /* OCMDATA_GUARD_BAND_SIZE */
#endif /* defined(DEBUG_OCM_DATA_ALLOC) */

	spin_unlock_irqrestore(&opi->lock, flags);

	DEBUG_TRACE("%p: Allocated block: %p\n", mb, blk);

	DEBUG_ASSERT(((addr_t)mb % 4) == 0, "alignment error\n");

	return blk;
}

/*
 * ocmdata_free()
 *	Release a block of memory to the pool it came from.
 *
 * Released block goes to the end of the free list after being coalesced
 */
void ocmdata_free(void *block)
{
	unsigned long flags;
#if OCMDATA_GUARD_BAND_SIZE > 0
	u8_t *user_end;
	u8_t *mb_end;
#endif
	size_t blk_data_size;
	
	size_t align_mask = opi->align - 1;
	size_t ocmdata_block_offset = (sizeof(struct ocmdata_block) + align_mask) & ~align_mask;

	addr_t mba = ((addr_t)block) - ocmdata_block_offset;
	struct ocmdata_block *mb = (struct ocmdata_block *)mba;

	DEBUG_TRACE("%p: Free block: %p\n", mb, block);

	spin_lock_irqsave(&opi->lock, flags);

	OCMDATA_BLOCK_VERIFY_MAGIC(mb);

	/*
	 * Check if this block is already free or in a bucket. If it's not we cannot
	 * reinsert it in to the free list as it will cause problems.  Our only alternative
	 * is to either crash, or return (and wait for the crash).
	 */
	if (mb->flags & (OCMDATA_FREE | OCMDATA_BUCKET)) {
		spin_unlock_irqrestore(&opi->lock, flags);
		DEBUG_ERROR("%p: ocmdata bad free: flags %hhu, size %u, block: %p\n", 
			    mb, mb->flags, mb->size, block);
		debug_memory_info();
		DEBUG_ASSERT(FALSE, "Stopped\n");

		/*
		 * We have just seen a double-free so the system has gone slightly insane.
		 * We have no idea what we can do so instead we go into an infinite loop
		 * and let the watchdog reboot us when it eventually gets bored waiting.
		 * What we really need here is a solution that kills the system
		 * immediately, but currently that is not available.
		 */
		while (1);
	}

	/*
	 * Look at the ocmdata guard band (if we have one) and see if anything has
	 * written past the end of the block.  This shows up as "footprints" in
	 * our "paint".
	 */
#if defined(DEBUG_OCM_DATA_ALLOC)
#if OCMDATA_GUARD_BAND_SIZE > 0
	/*
	 * user_end is the end of the user space part of the ocmdata block. mb_end is the
	 * actual end of the ocmdata block.
	 */
	user_end = (u8_t *)block + mb->requested_size;
	mb_end = (u8_t *)mb + mb->size;
	while (user_end < mb_end) {
		if (*(user_end++) != OCMDATA_GUARD_PAINT) {
			spin_unlock_irqrestore(&opi->lock, flags);
			DEBUG_ERROR("%p: buffer overrun @ %p: %p start gb=%p end=%p, size=%u\n", mb,
				    user_end, block, (u8_t *)block + mb->requested_size,
				    (u8_t *)mb + mb->size, mb->requested_size);
			debug_memory_info();
			DEBUG_ASSERT(FALSE, "Stopped\n");
		}
	}
#endif /* OCMDATA_GUARD_BAND_SIZE */

#if defined(OCMDATA_CLEAR_BLOCK_ON_FREE)
	/*
	 * If we're set to clear block contents when we free things now is a good time.
	 */
	memset(block, 0, mb->size - ocmdata_block_offset);

#endif /* OCMDATA_CLEAR_BLOCK_ON_FREE */
#endif /* defined(DEBUG_OCM_DATA_ALLOC) */

	opi->free_ram += mb->size;

	/*
	 * Is this block small enough that we want to release it to a bucket instead of the
	 * main ocmdata?
	 */
	blk_data_size = mb->size - ocmdata_block_offset;
	if (blk_data_size <= OCMDATA_BUCKET_MAX_SIZE) {
		size_t index = (blk_data_size - opi->alloc_align) >> opi->bucket_shift;
		struct ocmdata_block **bbp;
		DEBUG_TRACE("%p: Release %u byte block to bucket %u (size %u)\n", 
			    mb, blk_data_size, index, mb->size);

		bbp = &opi->bucket[index];
		mb->bucket_next = *bbp;
		mb->flags |= OCMDATA_BUCKET;

		/*
		 * Tag the bucketized block as belonging to the ocmdata.
		 */
		*bbp = mb;

		spin_unlock_irqrestore(&opi->lock, flags);
		return;
	}

	/*
	 * Remove this one from the list of allocated blocks.
	 */
	if (mb->next == mb) {
		opi->first_block = NULL;
	} else {
		if (opi->first_block == mb) {
			opi->first_block = mb->next;
		}
		mb->next->prev = mb->prev;
		mb->prev->next = mb->next;
	}

	/*
	 * Coallesce our block if possible.  If there's a hole after this block then unlink
	 * it and add it to this block.
	 */
	if (!(mb->flags & OCMDATA_LAST)) {
		struct ocmdata_block *hole = (struct ocmdata_block *)(((addr_t)mb) + mb->size);
		OCMDATA_BLOCK_VERIFY_MAGIC(hole);

		if (hole->flags & OCMDATA_FREE) {
			/*
			 * Unlink hole from the free list.
			 */
			hole->next->prev = hole->prev;
			hole->prev->next = hole->next;

#if defined(DEBUG_OCM_DATA_ALLOC)
			hole->magic = 0;
#endif

			/*
			 * We are removing the node currently pointed to by the free list.
			 */
			if (opi->first_hole == hole) {
				if (hole->next == hole) {
					/*
					 * If there is only one item on the list, the list is now empty.
					 */
					opi->first_hole = NULL;
				} else {
					opi->first_hole = hole->next;
				}
			}

			/*
			 * Combine the hole into me.
			 */
			mb->size += hole->size;
			if (hole->flags & OCMDATA_LAST) {
				mb->flags |= OCMDATA_LAST;
			} else {
				/*
				 * Make the next block down's previous block size include me.
				 */
				struct ocmdata_block *mbnext = (struct ocmdata_block *)(((addr_t)mb) + mb->size);
				OCMDATA_BLOCK_VERIFY_MAGIC(mbnext);

				mbnext->size_prev = mb->size;
			}
		}
	}

	/*
	 * If the hole before is contiguous just merge ourself into it.
	 */
	if (!(mb->flags & OCMDATA_FIRST)) {
		struct ocmdata_block *hole = (struct ocmdata_block *)(((addr_t)mb) - mb->size_prev);
		OCMDATA_BLOCK_VERIFY_MAGIC(hole);

		if (hole->flags & OCMDATA_FREE) {
#if defined(DEBUG_OCM_DATA_ALLOC)
			mb->magic = 0;
#endif

			hole->size += mb->size;
			if (mb->flags & OCMDATA_LAST) {
				hole->flags |= OCMDATA_LAST;
			} else {
				struct ocmdata_block *next = (struct ocmdata_block *)(((addr_t)hole) + hole->size);
				OCMDATA_BLOCK_VERIFY_MAGIC(next);

				next->size_prev = hole->size;
			}

			spin_unlock_irqrestore(&opi->lock, flags);
			return;
		}
	}

	/*
	 * link in the freed block to the end of the free list
	 */
	mb->flags |= OCMDATA_FREE;
	if (!opi->first_hole) {
		mb->next = mb;
		mb->prev = mb;
		opi->first_hole = mb;
	} else {
		mb->next = opi->first_hole;
		mb->prev = opi->first_hole->prev;
		opi->first_hole->prev = mb;
		mb->prev->next = mb;
	}

	spin_unlock_irqrestore(&opi->lock, flags);
}

/*
 * ocmdata_realloc()
 *	Reallocate space that was previously allocated.
 *
 * The new allocation can be in a different pool than the old one
 */
void *ocmdata_realloc(void *ptr, size_t size)
{
	size_t oldsz;

	/*
	 * We have to handle a NULL pointer first!
	 */
	void *p = ocmdata_alloc(size);
	if (!p || !ptr) {
		/*
		 * On failure we leave the original data alone and don't free it!
		 * If input buffer is empty, no need to copy or free it.
		 */
		return p;
	}

	/*
	 * Ensure that we copy the smaller of the old or new size.
	 */
	oldsz = ocmdata_get_size(ptr);
	if (size > oldsz) {
		size = oldsz;
	}

	memcpy(p, ptr, size);

	ocmdata_free(ptr);
	return p;
}

/*
 * ocmdata_alloc_and_zero()
 *	Allocate space and zero it.
 */
void *ocmdata_alloc_and_zero(size_t size)
{
	void *p = ocmdata_alloc(size);
	if (!p) {
		return NULL;
	}

	memset(p, 0, size);
	return p;
}

/*
 * ocmdata_strdup()
 *	Duplicate a string, allocating space for the duplicate.
 *
 * The new allocation can be in a different region than the old one
 */
char *ocmdata_strdup(const char *s)
{
	char *ns = (char *)ocmdata_alloc(strlen(s) + 1);
	if (!ns) {
		return NULL;
	}

	strcpy(ns, s);
	return ns;
}

/*
 * ocmdata_get_size()
 *	Get the size of a memory block allocated from the ocmdata.
 */
size_t ocmdata_get_size(void *block)
{
	unsigned long flags;
	size_t align_mask;
	size_t ocmdata_block_offset;
	addr_t mba;
	struct ocmdata_block *mb;
	size_t ret;
	
	spin_lock_irqsave(&opi->lock, flags);

	align_mask = opi->align - 1;
	ocmdata_block_offset = (sizeof(struct ocmdata_block) + align_mask) & ~align_mask;
	mba = ((addr_t)block) - ocmdata_block_offset;
	mb = (struct ocmdata_block *)mba;

	OCMDATA_BLOCK_VERIFY_MAGIC(mb);

	ret = mb->size - ocmdata_block_offset;
	spin_unlock_irqrestore(&opi->lock, flags);

#if defined(DEBUG_OCM_DATA_ALLOC)
	ret -= OCMDATA_GUARD_BAND_SIZE;
#endif

	return ret;
}

/*
 * ocmdata_get_total()
 *	Return the total size of a ocmdata.
 */
size_t ocmdata_get_total(void)
{
	unsigned long flags;
	size_t ret;

	spin_lock_irqsave(&opi->lock, flags);
	ret = opi->total_ram;
	spin_unlock_irqrestore(&opi->lock, flags);

	return ret;
}

/*
 * ocmdata_get_free()
 *	Return the amount of ocmdata space that's still available.
 */
size_t ocmdata_get_free(void)
{
	unsigned long flags;
	size_t ret;
	
	spin_lock_irqsave(&opi->lock, flags);
	ret = opi->free_ram;
	spin_unlock_irqrestore(&opi->lock, flags);
	return ret;
}

/*
 * ocmdata_get_alignment()
 *	Return the alignment of ocmdata space in a given pool.
 */
size_t ocmdata_get_alignment(void)
{
	/*
	 * If our alignment could ever change then we'd really want to acquire the
	 * pool lock here but alignment never changes for the lifetime of the pool
	 * so we're safe.
	 */
	return opi->align;
}

/*
 * ocmdata_get_low_water_in_pool()
 *	Return the lowest level that the free ocmdata has reached.
 */
size_t ocmdata_get_low_water()
{
	unsigned long flags;
	size_t ret;
	
	spin_lock_irqsave(&opi->lock, flags);
	ret = opi->low_water;
	spin_unlock_irqrestore(&opi->lock, flags);
	return ret;
}

/*
 * ocmdata_reset_low_water()
 *	Reset the low water mark.
 */
void ocmdata_reset_low_water(void)
{
	unsigned long flags;
	u32_t t;
	
	spin_lock_irqsave(&opi->lock, flags);

	opi->low_water = opi->total_ram;

	t = jiffies;
	opi->low_water_time = t;
	opi->low_water_reset = t;

	spin_unlock_irqrestore(&opi->lock, flags);
}

/*
 * ocmdata_get_and_reset_low_water()
 *	Return the lowest level that the free ocmdata has reached and then reset that to the current ocmdata level.
 */
size_t ocmdata_get_and_reset_low_water(void)
{
	unsigned long flags;
	size_t ret;
	u32_t t;
	
	spin_lock_irqsave(&opi->lock, flags);

	ret = opi->low_water;
	opi->low_water = opi->free_ram;

	t = jiffies;
	opi->low_water_time = t;
	opi->low_water_reset = t;

	spin_unlock_irqrestore(&opi->lock, flags);

	return ret;
}

/*
 * ocmdata_get_low_water_statistics()
 */
int ocmdata_get_low_water_statistics(struct ocmdata_low_water *hlw)
{
	unsigned long flags;
	
	spin_lock_irqsave(&opi->lock, flags);
	hlw->low_water_mark = opi->low_water;
	hlw->low_water_time = opi->low_water_time;
	hlw->last_reset_time = opi->low_water_reset;
	spin_unlock_irqrestore(&opi->lock, flags);

	return TRUE;
}

/*
 * ocmdata_get_and_reset_low_water_statistics()
 */
int ocmdata_get_and_reset_low_water_statistics(struct ocmdata_low_water *hlw)
{
	unsigned long flags;
	u32_t t = jiffies;

	spin_lock_irqsave(&opi->lock, flags);
	hlw->low_water_mark = opi->low_water;
	hlw->low_water_time = opi->low_water_time;
	hlw->last_reset_time = opi->low_water_reset;
	opi->low_water = opi->free_ram;
	opi->low_water_time = t;
	opi->low_water_reset = t;
	spin_unlock_irqrestore(&opi->lock, flags);

	return TRUE;
}

/*
 * ocmdata_get_statistics()
 */
int ocmdata_get_statistics(struct ocmdata_statistics *stats)
{
	unsigned long flags;
	struct ocmdata_low_water *hlw = &stats->low_water;

	spin_lock_irqsave(&opi->lock, flags);
	stats->bytes_free = opi->free_ram;
	hlw->low_water_mark = opi->low_water;
	hlw->low_water_time = opi->low_water_time;
	hlw->last_reset_time = opi->low_water_reset;
	spin_unlock_irqrestore(&opi->lock, flags);

	stats->version = OCMDATA_STATISTICS_VERSION_NUMBER;
	stats->total_size = ocmdata_get_total();
	stats->timestamp = jiffies;

	return TRUE;
}

/*
 * ocmdata_add()
 *
 * This function allows a block of memory to be added to a ocmdata.  Typically
 * this would be done when the system starts to allocate any RAM that has not been
 * used by compile-time static allocations or is not required for stacks, etc.
 */
static void ocmdata_add(unsigned long addr, size_t sz)
{
	unsigned long flags;
	size_t align_mask = opi->align - 1;
	size_t ocmdata_block_offset = (sizeof(struct ocmdata_block) + align_mask) & ~align_mask;
	struct ocmdata_block *mb;
	struct ocmdata_block *first_block;

	DEBUG_ASSERT((addr & align_mask) == 0, "unaligned pointer: %lu and mask %x", addr, align_mask);
	sz &= ~align_mask;

	/*
	 * don't add a block if it is too small
	 */
	if (sz <= ocmdata_block_offset + align_mask) {
#if defined(DEBUG_OCM_DATA_ALLOC)
		DEBUG_WARN("ocmdata_add size too small, %u at %lx", sz, addr);
#endif
		return;
	}

	DEBUG_INFO("Ocmdata add start: %lu, size: %u", addr, sz);

	/*
	 * Adding new space to the ocmdata is just a case of fooling the ocmdata_free()
	 * function into believing that the new space was previously allocated.
	 * All we have to do is forge an "alloc"'d block!
	 */
	mb = (struct ocmdata_block *)addr;
	mb->size = sz;
	mb->size_prev = 0;

	/*
	 * Set the flags, but do not set the free flag as that will be done by the call to ocmdata_free()
	 */
	mb->flags = OCMDATA_FIRST | OCMDATA_LAST;

	DEBUG_TRACE("ocmdata_add to %u at %lx\n", sz, addr);
	spin_lock_irqsave(&opi->lock, flags);

	first_block = opi->first_block;
	if (!first_block) {
		mb->next = mb;
		mb->prev = mb;
	} else {
		mb->next = first_block;
		mb->prev = first_block->prev;
		first_block->prev = mb;
		mb->prev->next = mb;
	}

	opi->first_block = mb;

#if defined(DEBUG_OCM_DATA_ALLOC)
	mb->magic = OCMDATA_BLOCK_MAGIC;

#if OCMDATA_GUARD_BAND_SIZE > 0
	/*
	 * Initialize the guard band
	 */
	mb->requested_size = sz - (OCMDATA_GUARD_BAND_SIZE + ocmdata_block_offset);
	memset((u8_t *)mb + sz - OCMDATA_GUARD_BAND_SIZE, OCMDATA_GUARD_PAINT, OCMDATA_GUARD_BAND_SIZE);
#endif /* OCMDATA_GUARD_BAND_SIZE */
#endif /* defined(DEBUG_OCM_DATA_ALLOC) */

	opi->total_ram += sz;
	opi->low_water = opi->total_ram;
	spin_unlock_irqrestore(&opi->lock, flags);

	ocmdata_free((u8_t *)mb + ocmdata_block_offset);
}

/*
 * ocmdata_pkg_init()
 */
void ocmdata_init(unsigned long addr, size_t sz)
{
	unsigned long aligned_addr;
	size_t align_mask;

	spin_lock_init(&opi->lock);

	/*
	 * OCM alignment isn't the same as the default pool since OCM isn't cached and we
	 * want optimal packing rather than optimal cache alignment.
	 */
	opi->align = 4;
	opi->alloc_align = 4;
	opi->bucket_shift = 0;

	/*
	 * Starting address of the OCM block and the size of the OCM block need to
	 * be aligned.  Move the starting address to the next larger aligned address if
	 * it isn't already aligned.
	 */
	align_mask = opi->align - 1;
	aligned_addr = (addr + align_mask) & ~align_mask;

	/*
	 * If the starting address is moved, then we need to adjust the size by the
	 * same amount.  The size should then be reduced to the nearest multiple of the
	 * alignment size if it isn't already at a multiple.
	 */
	sz -= (aligned_addr - addr);
	sz &= ~align_mask;
	
	ocmdata_add(aligned_addr, sz);
}
