/*
 * Flash support files
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

#include <common.h>

#define BANKSZ		CONFIG_SYS_FLASH_SIZE
#define SECTSZ		EXTFLASH_PAGE_SIZE
#define USERFLASH	(2 * 1024 * 1024)	/* bottom 2 MB for user	*/

flash_info_t flash_info[CONFIG_SYS_MAX_FLASH_BANKS];

/*-----------------------------------------------------------------------
 */
extern u8_t mem_flash_type;
extern mem_addr_t mem_flash_actual_total_size;
extern mem_addr_t mem_flash_actual_page_size;


extern void mem_flash_init(void);

ulong flash_init (void)
{
	printf ("flash_init\n");

	mem_flash_type = mem_flash_query();
	mem_flash_actual_total_size = EXTFLASH_TOTAL_SIZE;
	mem_flash_actual_page_size = EXTFLASH_PAGE_SIZE;
	
	int i;
	unsigned long addr;
	flash_info_t *fli = &flash_info[0];

	fli->size = CONFIG_SYS_FLASH_SIZE;
	fli->sector_count = CONFIG_SYS_MAX_FLASH_SECT;
	fli->flash_id = FLASH_MAN_AMD + FLASH_AMDLV065D;

	addr = CONFIG_SYS_FLASH_BASE;
	for (i = 0; i < fli->sector_count; ++i) {
		fli->start[i] = addr;
		addr += SECTSZ;
		if (i < CONFIG_UBOOT_IMAGE_LAST_SECTOR) {
			fli->protect[i] = 1;
		} else {
			fli->protect[i] = 0;
		}
	}

	return (fli->size);
}

/*-----------------------------------------------------------------------
 */
void flash_print_info (flash_info_t * info)
{
	printf("flash_print_info\n");
	printf("flash bank size is %lu\n", info->size);
	printf("sector count is %hu\n", info->sector_count);
	printf("flash id is %lu\n", info->flash_id);

	printf("  Sector Start Addresses:");
	int i;
	for (i = 0; i < info->sector_count; ++i) {
		if ((i % 5) == 0)
			printf ("\n   ");
		printf(" %08lX%s",
			info->start[i], info->protect[i] ? " (RO)" : "     ");
	}
	printf("\n");
}

/*-----------------------------------------------------------------------
 */
int flash_erase (flash_info_t * info, int s_first, int s_last)
{
	int sect = 0;
	printf ("\n");
	for (sect=s_first; sect<=s_last; ++sect) {
		if (info->protect[sect]) {
			printf ("Warning: Sector %d is protected, it will not be erased!\n", sect);
			continue;
		}
		printf("Erasing sector %d\n", sect);
		mem_flash_erase((mem_addr_t)info->start[sect], SECTSZ);
	}

	return 0;
}

/*-----------------------------------------------------------------------
 * Copy memory to flash.
 */
int write_buff (flash_info_t * info, uchar * src, ulong addr, ulong cnt)
{
#if 1
	mem_flash_write((mem_addr_t) addr, (void*) src, (size_t)cnt);
#else
	printf("Writing to addr: 0x%x\n", addr);
	for (int i = 0; i < cnt; i++){
		printf("0x%x ", *(src+i));

		if ((i != 0) && ((i%16) == 0)){
			printf("\n");
		}
		printf("\n");
	}
#endif
	return 0;
}

