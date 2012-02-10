/*
 *
 * (C) Copyright 2009
 * Ubicom, Inc. www.ubicom.com
 *
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/ip5000.h>

/*
 * mem_cache_control()
 *	Special cache control operation
 */
extern void mem_cache_control(mem_addr_t cc, mem_addr_t begin_addr, mem_addr_t end_addr, u8_t op);

void flush_cache (ulong start_addr, ulong size)
{
	unsigned long end = start_addr + size;
	mem_cache_control(DCCR_BASE, start_addr, end, CCR_CTRL_FLUSH_ADDR);
	mem_cache_control(ICCR_BASE, start_addr, end, CCR_CTRL_INV_ADDR);
}
