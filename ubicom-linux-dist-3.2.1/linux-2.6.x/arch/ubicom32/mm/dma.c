/*
 * arch/ubicom32/kernel/dma.c
 *   Ubicom32 architecture dynamic DMA mapping support.
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

#include <linux/types.h>
#include <linux/dma-mapping.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/device.h>
#include <linux/scatterlist.h>
#include <linux/io.h>

void *dma_alloc_noncoherent(struct device *dev, size_t size,
	dma_addr_t *dma_handle, gfp_t gfp)
{
	void *addr;
	/* ignore region specifiers */
	gfp &= ~(__GFP_DMA | __GFP_HIGHMEM);

	addr = (void *)__get_free_pages(gfp, get_order(size));
	if (addr == NULL) {
		return NULL;
	}

	memset(addr, 0, size);
	dma_cache_wback_inv((unsigned long)addr, size);
	*dma_handle = dma_map_mem(dev, addr, size);
	return addr;
}

EXPORT_SYMBOL(dma_alloc_noncoherent);

void dma_free_noncoherent(struct device *dev, size_t size, void *vaddr,
	dma_addr_t dma_handle)
{
	dma_unmap_mem(dev, dma_handle, size);
	free_pages((unsigned long) vaddr, get_order(size));
}

EXPORT_SYMBOL(dma_free_noncoherent);

void *dma_alloc_coherent(struct device *dev, size_t size,
			   dma_addr_t *dma_handle, gfp_t gfp)
{
	void *addr;
	/* ignore region specifiers */
	gfp &= ~(__GFP_DMA | __GFP_HIGHMEM);

	addr = (void *)__get_free_pages(gfp, get_order(size));
	if (addr == NULL) {
		return NULL;
	}

	memset(addr, 0, size);
	dma_cache_wback_inv((unsigned long)addr, size);
	*dma_handle = dma_map_coherent_mem(dev, addr, size);
	return addr;
}

EXPORT_SYMBOL(dma_alloc_coherent);

void dma_free_coherent(struct device *dev, size_t size,
			 void *vaddr, dma_addr_t dma_handle)
{
	dma_unmap_coherent_mem(dev, dma_handle, size);
	free_pages((unsigned long)vaddr, get_order(size));
}

EXPORT_SYMBOL(dma_free_coherent);

dma_addr_t dma_map_page(struct device *dev, struct page *page,
	unsigned long offset, size_t size, enum dma_data_direction direction)
{
	unsigned long addr;
	addr = (unsigned long) page_address(page) + offset;
	dma_sync_for_device(addr, size, direction);
	return dma_map_mem_page(dev, page) + offset;
}

EXPORT_SYMBOL(dma_map_page);

void dma_unmap_page(struct device *dev, dma_addr_t dma_address, size_t size,
	enum dma_data_direction direction)
{
	unsigned long addr;
	addr = dma_addr_to_virt(dev, dma_address);
	dma_sync_for_cpu(addr, size, direction);
	dma_unmap_mem(dev, dma_address, size);
}

EXPORT_SYMBOL(dma_unmap_page);

int dma_map_sg(struct device *dev, struct scatterlist *sg, int nents,
	enum dma_data_direction direction)
{
	int i;

	for (i = 0; i < nents; i++, sg++) {
		unsigned long addr;
		addr = (unsigned long) sg_virt(sg);
		dma_sync_for_device(addr, sg->length, direction);
		sg->dma_address = dma_map_mem(dev, (void *)addr, sg->length);
	}

	return nents;
}

EXPORT_SYMBOL(dma_map_sg);

void dma_unmap_sg(struct device *dev, struct scatterlist *sg, int nhwentries,
	enum dma_data_direction direction)
{
	int i;

	for (i = 0; i < nhwentries; i++, sg++) {
		unsigned long addr;
		addr = (unsigned long) sg_virt(sg);
		dma_sync_for_cpu(addr, sg->length, direction);
		dma_unmap_mem(dev, sg->dma_address, sg->length);
	}
}

EXPORT_SYMBOL(dma_unmap_sg);

void dma_sync_sg_for_cpu(struct device *dev, struct scatterlist *sg, int nelems,
	enum dma_data_direction direction)
{
	int i;
	for (i = 0; i < nelems; i++, sg++) {
		dma_sync_for_cpu((unsigned long)page_address(sg_page(sg)),
		           sg->length, direction);
	}
}

EXPORT_SYMBOL(dma_sync_sg_for_cpu);

void dma_sync_sg_for_device(struct device *dev, struct scatterlist *sg, int nelems,
	enum dma_data_direction direction)
{
	int i;
	for (i = 0; i < nelems; i++, sg++) {
		dma_sync_for_device((unsigned long)page_address(sg_page(sg)),
		           sg->length, direction);
	}
}

EXPORT_SYMBOL(dma_sync_sg_for_device);

int dma_mapping_error(struct device *dev, dma_addr_t dma_addr)
{
	return 0;
}

EXPORT_SYMBOL(dma_mapping_error);


