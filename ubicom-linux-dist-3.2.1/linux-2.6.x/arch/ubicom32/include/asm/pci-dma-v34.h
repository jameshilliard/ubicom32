/*
 * arch/ubicom32/include/asm/pci.h
 *   Definitions of PCI operations for Ubicom32 architecture.
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
 */
#ifndef _ASM_UBICOM32_PCI_DMA_V34
#define _ASM_UBICOM32_PCI_DMA_V34

#include <linux/slab.h>

static inline void * pci_alloc_consistent(struct pci_dev *hwdev, size_t size,
                     dma_addr_t *dma_handle)
{
    void *vaddr = kmalloc(size, GFP_KERNEL);
    if(vaddr != NULL) {
        *dma_handle = virt_to_phys(vaddr);
    }
    return vaddr;
}

static  inline int pci_dma_supported(struct pci_dev *hwdev, dma_addr_t mask)
{
 	return 1;
}

static  inline void pci_free_consistent(struct pci_dev *hwdev, size_t size,
	void *cpu_addr, dma_addr_t dma_handle)
{ 
	kfree(cpu_addr);
	return;
}

static inline dma_addr_t pci_map_single(struct pci_dev *hwdev, void *ptr,
	size_t size, int direction)
{
	 return virt_to_phys(ptr);
}

static inline void pci_unmap_single(struct pci_dev *hwdev, dma_addr_t dma_addr,
	size_t size, int direction)
{
	 return;
}

static inline dma_addr_t
pci_map_page(struct pci_dev *hwdev, struct page *page,
             unsigned long offset, size_t size, int direction)
{
	 return pci_map_single(hwdev, page_address(page) + offset, size, (int)direction);
}


static inline void
pci_unmap_page(struct pci_dev *hwdev, dma_addr_t dma_address,
               size_t size, int direction)
{
	pci_unmap_single(hwdev, dma_address, size, direction);
}

static inline int
pci_map_sg(struct pci_dev *hwdev, struct scatterlist *sg,
           int nents, int direction)
{
        return nents; 
}

static inline void
pci_unmap_sg(struct pci_dev *hwdev, struct scatterlist *sg,
             int nents, int direction)
{
}

static inline void
pci_dma_sync_sg_for_cpu(struct pci_dev *hwdev, struct scatterlist *sg,
                int nelems, int direction)
{
}

static inline void
pci_dma_sync_sg_for_device(struct pci_dev *hwdev, struct scatterlist *sg,
                int nelems, int direction)
{
}

static inline void
pci_dma_sync_single_for_cpu(struct pci_dev *hwdev, dma_addr_t dma_handle,
                    size_t size, int direction)
{
}

static inline void
pci_dma_sync_single_for_device(struct pci_dev *hwdev, dma_addr_t dma_handle,
                    size_t size, int direction)
{
}

static inline int
pci_dma_mapping_error(struct pci_dev *hwdev, dma_addr_t dma_addr)
{
        return dma_addr == 0;
}

#endif //_ASM_UBICOM32_PCI_DMA_V34
