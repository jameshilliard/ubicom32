/*
 * (C) Copyright 2009
 * Ubicom, Inc. www.ubicom.com
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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
 ********************************************************************
 * NOTE: This header file defines an interface to U-Boot. Including
 * this (unmodified) header file in another file is considered normal
 * use of U-Boot, and does *not* fall under the heading of "derived
 * work".
 ********************************************************************
 */

#ifndef _U_BOOT_H_
#define _U_BOOT_H_	1

typedef struct bd_info {
    int			bi_baudrate;	/* serial console baudrate */
    unsigned long	bi_ip_addr;	/* IP Address */
    unsigned char	bi_enetaddr[6]; /* Ethernet adress */
    struct environment_s	       *bi_env;
    ulong	        bi_arch_number;	/* unique id for this board */
    ulong	        bi_boot_params;	/* where this board expects params */

#ifdef NOT_UBICOM32
    struct				/* RAM configuration */
    {
	ulong start;
	ulong size;
    }			bi_dram[CONFIG_NR_DRAM_BANKS];
#endif //NOT_UBICOM32

#ifdef CONFIG_HAS_ETH1
    /* second onboard ethernet port */
    unsigned char   bi_enet1addr[6];
#endif

	phys_size_t bi_memsize;		/* size  of DRAM memory in bytes */
	unsigned long bi_memstart;	/* start of DRAM memory */

	unsigned long bi_flashstart;	/* start of FLASH memory */
	unsigned long bi_flashsize;	/* size  of FLASH memory */
	unsigned long bi_flashoffset;	/* reserved area for startup monitor */

	
} bd_t;

#define bi_env_data bi_env->data
#define bi_env_crc  bi_env->crc

#endif	/* _U_BOOT_H_ */
