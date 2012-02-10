/*
 * (C) Copyright 2010, Ubicom, Inc.
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

#ifndef _DWC_UBICOM_H_
#define _DWC_UBICOM_H_

#ifdef CONFIG_UBICOM32

#define dwc_dma_flush(buf, size)        ubi32_flush(buf, size)
#define dwc_dma_invalidate(buf, size)   ubi32_invalidate(buf, size)
#define dwc_dma_sync()                  ubi32_sync()
#define dwc_dma_check_ready(regs)       ubi32_dwc_dma_ready(FUNCTION_BASE(regs))
#else
#define dwc_dma_flush(buf, size)
#define dwc_dma_invalidate(buf, size)
#define dwc_dma_sync()
#define dwc_dma_check_ready(regs)

#endif
extern void ubi32_flush(void *buf, unsigned int size);
extern void ubi32_invalidate(void *buf, unsigned int size);
extern void ubi32_sync(void);

extern void ubi32_br_writel(volatile u32 * volatile addr, u32 val);
extern u32 ubi32_br_readl(volatile u32 * volatile addr);

#ifdef CONFIG_UBI32_DWC_OTG
/*
 * Wait for the data written by DWC OTG core to be ready for read
 */
extern void ubi32_dwc_dma_ready(void __iomem *fbase);
#endif


#endif // _DWC_UBICOM_H_
