/*
 * arch/ubicom32/include/asm/dma-mapping.h
 *   Generic dma-mapping.h for Ubicom32 architecture.
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
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 *   arch/mips
 */
#ifndef _ASM_UBICOM32_DMA_MAPPING_H
#define _ASM_UBICOM32_DMA_MAPPING_H

#include <asm/cachectl.h>
#include <asm/cacheflush.h>

#include <linux/scatterlist.h>

/*
 * inlined map/unmap APIs
 */
/*
 * Memory in ubicom32 arch version 4 and earlier is always coherent and no need
 * to flush and invalidate
 */
static inline void dma_cache_wback(unsigned long addr, size_t size)
{
#if (UBICOM32_ARCH_VERSION > 4)
	/*
	 * It is actually a flush and invalidate, given that we have only one instruction for it.
	 */
	flush_dcache_range(addr, addr + size);
#endif
}

static inline void dma_cache_inv(unsigned long addr, size_t size)
{
#if (UBICOM32_ARCH_VERSION > 4)
	mem_d_cache_control(addr, addr + size, CCR_CTRL_INV_ADDR);
#endif
}

static inline void dma_cache_wback_inv(unsigned long addr, size_t size)
{
#if (UBICOM32_ARCH_VERSION > 4)
	flush_dcache_range(addr, addr + size);
#endif
}

static inline void dma_sync_for_device(unsigned long addr, size_t size,
	enum dma_data_direction direction)
{
	switch (direction) {
	case DMA_TO_DEVICE:
		dma_cache_wback(addr, size);
		break;

	case DMA_FROM_DEVICE:
		/*
		 * Per DMA-API.txt, this sync option should be done before the driver accesses data that
		 * may be changed by the device. BUT if any dirty data is already in cache, they may be
		 * evicted out later to cause conflict. Invalidate (or flush) here to be safe.
		 */
		//dma_cache_inv(addr, size);
		dma_cache_wback_inv(addr, size);
		break;

	case DMA_BIDIRECTIONAL:
		dma_cache_wback_inv(addr, size);
		break;

	default:
		BUG();
	}
}

static inline void dma_sync_for_cpu(unsigned long addr, size_t size,
	enum dma_data_direction direction)
{
	switch (direction) {
	case DMA_TO_DEVICE:
		/*
		 * Per DMA-API.txt, the memory should be treated as read-only by the device.
		 * So there is no need to invalidate it, even if we hadn't invalidated it.
		 */
		break;

	case DMA_FROM_DEVICE:
		#if 0
		dma_cache_inv(addr, size);
		#endif
		break;

	case DMA_BIDIRECTIONAL:
		#if 0
		dma_cache_inv(addr, size);
		#endif
		break;

	default:
		BUG();
	}
}

static inline dma_addr_t dma_map_mem(struct device *dev, void *addr, size_t size)
{
	return (dma_addr_t)virt_to_phys(addr);
}

static inline void dma_unmap_mem(struct device *dev, dma_addr_t dma_handle, size_t size)
{
}

static inline dma_addr_t dma_map_mem_page(struct device *dev,
	struct page *page)
{
	return page_to_phys(page);
}

static inline unsigned long dma_addr_to_virt(struct device *dev, dma_addr_t dma_addr)
{
	return (unsigned long)phys_to_virt(dma_addr);
}

/*
 * A place holder for coherent DMA mapping/unmapping which needs some page table/TLB patching later
 */
static inline dma_addr_t dma_map_coherent_mem(struct device *dev, void *addr, size_t size)
{
	return dma_map_mem(dev, addr, size);
}

static inline void dma_unmap_coherent_mem(struct device *dev, dma_addr_t dma_handle, size_t size)
{
}

static inline dma_addr_t dma_map_single(struct device *dev, void *ptr, size_t size,
	enum dma_data_direction direction)
{
	unsigned long addr = (unsigned long) ptr;

	dma_sync_for_device(addr, size, direction);
	return dma_map_mem(dev, ptr, size);
}

static inline void dma_unmap_single(struct device *dev, dma_addr_t dma_addr, size_t size,
	enum dma_data_direction direction)
{
	dma_sync_for_cpu(dma_addr_to_virt(dev, dma_addr), size,
	           direction);
	dma_unmap_mem(dev, dma_addr, size);
}

static inline void dma_sync_single_for_cpu(struct device *dev, dma_addr_t dma_handle,
	size_t size, enum dma_data_direction direction)
{
	unsigned long addr;
	addr = dma_addr_to_virt(dev, dma_handle);
	dma_sync_for_cpu(addr, size, direction);
}

static inline void dma_sync_single_for_device(struct device *dev, dma_addr_t dma_handle,
	size_t size, enum dma_data_direction direction)
{
	unsigned long addr;
	addr = dma_addr_to_virt(dev, dma_handle);
	dma_sync_for_device(addr, size, direction);
}

static inline void dma_sync_single_range_for_cpu(struct device *dev, dma_addr_t dma_handle,
	unsigned long offset, size_t size, enum dma_data_direction direction)
{
	unsigned long addr;
	addr = dma_addr_to_virt(dev, dma_handle);
	dma_sync_for_cpu(addr + offset, size, direction);
}

static inline void dma_sync_single_range_for_device(struct device *dev, dma_addr_t dma_handle,
	unsigned long offset, size_t size, enum dma_data_direction direction)
{
	unsigned long addr;
	addr = dma_addr_to_virt(dev, dma_handle);
	dma_sync_for_device(addr + offset, size, direction);
}

/*
 * alloc & free
 */
extern void *dma_alloc_noncoherent(struct device *dev, size_t size,
			   dma_addr_t *dma_handle, gfp_t flag);

extern void dma_free_noncoherent(struct device *dev, size_t size,
			 void *vaddr, dma_addr_t dma_handle);

extern void *dma_alloc_coherent(struct device *dev, size_t size,
			   dma_addr_t *dma_handle, gfp_t flag);

extern void dma_free_coherent(struct device *dev, size_t size,
			 void *vaddr, dma_addr_t dma_handle);

/*
 * page
 */
extern dma_addr_t dma_map_page(struct device *dev, struct page *page,
	unsigned long offset, size_t size, enum dma_data_direction direction);

extern void dma_unmap_page(struct device *dev, dma_addr_t dma_address,
	size_t size, enum dma_data_direction direction);

/*
 * sg
 */
extern int dma_map_sg(struct device *dev, struct scatterlist *sg, int nents,
	enum dma_data_direction direction);

extern void dma_unmap_sg(struct device *dev, struct scatterlist *sg,
	int nhwentries, enum dma_data_direction direction);

extern void dma_sync_sg_for_cpu(struct device *dev, struct scatterlist *sg,
	int nelems, enum dma_data_direction direction);

extern void dma_sync_sg_for_device(struct device *dev, struct scatterlist *sg,
	int nelems, enum dma_data_direction direction);

/*
 * mics.
 */
extern int dma_mapping_error(struct device *dev, dma_addr_t dma_addr);

static inline int dma_supported(struct device *dev, u64 mask)
{
	/*
	 * GFP_DMA is overloaded by OCM now by ubicom32 platform, so disallow it.
	 */
	return (mask > DMA_BIT_MASK(24)) ? 1 : 0;
}

static inline int dma_set_mask(struct device *dev, u64 mask)
{
	if(!dev->dma_mask || !dma_supported(dev, mask)) {
		return -EIO;
	}

	*dev->dma_mask = mask;

	return 0;
}

#endif /* _ASM_UBICOM32_DMA_MAPPING_H */
