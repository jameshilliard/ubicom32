/*
 * Definitions valid for all cpu's of the Ubicom32 architecture.
 *
 * (C) Copyright 2009
 * Ubicom, Inc. www.ubicom.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef _ASM_UBICOM32_H
#define _ASM_UBICOM32_H

const char *board_get_revision(void);

extern void timer_init(void);
extern void mem_read(void *dst, mem_addr_t addr, size_t length);
extern u8_t mem_read_u8(mem_addr_t addr);
extern u16_t mem_read_u16(mem_addr_t addr);
extern u32_t mem_read_u32(mem_addr_t addr);
extern void mem_flash_erase(mem_addr_t addr, mem_addr_t length);
extern void mem_flash_write(mem_addr_t addr, const void *src, size_t length);
extern u32_t mem_flash_query(void);

#endif
