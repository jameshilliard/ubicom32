/*
 * (C) Copyright 2000
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

#include <config.h>

#define UBOOT_IMAGE_SIZE_AT_RAM	(CONFIG_UBOOT_MEM_SIZE)
#define UBOOT_IMAGE_START_ADDR_AT_RAM	(CONFIG_RAMBASE + CONFIG_RAMSIZE - UBOOT_IMAGE_SIZE_AT_RAM)

OUTPUT_ARCH(ubicom32)
ENTRY(_start)

MEMORY {
 ram : ORIGIN = UBOOT_IMAGE_START_ADDR_AT_RAM, LENGTH = UBOOT_IMAGE_SIZE_AT_RAM
}

SECTIONS
{
	__ocm_begin = 0xbffc0000;
	__ocm_limit = __ocm_begin + 0x00040000;
	. = UBOOT_IMAGE_START_ADDR_AT_RAM;
	. = ALIGN(4);
	_stext = .;
	.text      :
	{
	  cpu/ip8k/start.o	(.text)
	  *(.text)
		*(.text .text.*)
	} > ram

	. = ALIGN(4);
	.rodata : { *(.rodata) } > ram

	. = ALIGN(4);
	.data : { *(.data) } > ram

	. = ALIGN(4);
	.got : { *(.got) } > ram

	__u_boot_cmd_start = .;
	.u_boot_cmd : { *(.u_boot_cmd) } > ram
	__u_boot_cmd_end = .;

	. = ALIGN(4);
	__bss_start = .;
	.bss (NOLOAD) : { *(.bss) } > ram
	_end = .;
}
