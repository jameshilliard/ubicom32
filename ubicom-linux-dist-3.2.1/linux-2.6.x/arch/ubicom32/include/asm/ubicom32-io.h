/*
 * arch/ubicom32/include/asm/ubicom32-io.h
 *   Special I/O memory accessor functions for Ubicom32 architecture.
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
 */
#ifndef _ASM_UBICOM32_UBICOM32IO_H
#define _ASM_UBICOM32_UBICOM32IO_H

#define REG_SIZE IO_PORT_BR_OFFSET
#define FUNCTION_BASE(addr) (void *)((unsigned int)(addr) & ~0x3FFF)

#define FUNCTION_CTL0(base) (void *)((unsigned int)(base) + IO_CTL0)
#define FUNCTION_STA0(base) (void *)((unsigned int)(base) + IO_STATUS0)

#define FUNCTION_BR_BASE(base) (void *)((unsigned int)(base) + REG_SIZE)
#define FUNCTION_BR_OFFSET(addr, base) ((unsigned int)(addr) - (unsigned int)(base))

#ifdef CONFIG_UBI32_DWC_OTG
/*
 * Wait for the data written by DWC OTG core to be ready for read
 */
extern void ubi32_dwc_dma_ready(void __iomem *fbase);
#endif

#endif
