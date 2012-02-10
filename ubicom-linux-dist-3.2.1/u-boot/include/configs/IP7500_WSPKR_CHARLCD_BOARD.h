/*
 * include/configs/IP7500_WSPKR_CHARLCD_BOARD.h
 *   Support for IP7500 Wireless Speaker board with character LCD
 *
 * This file supports the IP7500 Internet Audio Player:
 *      8007-1210  Rev 1.0
 *      8007-1210  Rev 1.1
 *
 * With display adapter board:
 *	8007-1910  Rev 1.0
 *
 * (C) Copyright 2009-2010
 * Ubicom, Inc. www.ubicom.com
 *
 * (C) Copyright 2003-2007
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define	CONFIG_ENV_IS_IN_FLASH	1
#include <configs/ultra_uboot_config.h>

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_IP7K		1	/* This is an ip7k CPU	*/

/*
 * Supported commands
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_FLASH
#define CONFIG_CMD_ENV
#define CONFIG_CMD_GPIO
#define CONFIG_CMD_MTDPARTS
#define MTDIDS_DEFAULT "nor0=ubicom32fc.0"
#define MTDPARTS_DEFAULT "mtdparts=ubicom32fc.0:512k(launcher)ro,14080k(upgrade),1536k(jffs2),256k(fw_env)ro"

/*
 * Serial console configuration
 */
#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }
#define CONFIG_UBICOM_UART_MAILBOX

/*
 * Autobooting
 */
#define CONFIG_BOOTDELAY	2	/* autoboot after 5 seconds */

#define CONFIG_BOOTCOMMAND 	"bootm"
#undef CONFIG_BOOTARGS

#define CONFIG_AUTOBOOT_KEYED		1
#define CONFIG_AUTOBOOT_PROMPT		"Press SPACE to abort autoboot in %d seconds\n", bootdelay
#define CONFIG_AUTOBOOT_DELAY_STR 	"d"
#define CONFIG_AUTOBOOT_STOP_STR 	" "

/*
 * Low level configuration
 */

/*
 * Clock configuration
 */
#define SYSTEM_FREQ 		501000000
#define CONFIG_SYS_HZ 		1000
#define CONFIG_SYS_CLK_FREQ	SYSTEM_FREQ

/*
 * On Chip Memory map
 */

/*
 * External DDR SDRAM Memory Map
 */
#define CONFIG_RAMBASE 		0x40000000
#define CONFIG_RAMSIZE 		0x8000000
#define CONFIG_ARGS_SZ		512				/* bootargs */
#define CONFIG_SYS_BOOTM_LEN	0x1C00000
#define CONFIG_SYS_MAXARGS	16				/* max number of command args */
#define	CONFIG_SYS_LOAD_ADDR	CONFIG_UCLINUX_IMAGE_START_ADDR	/* default load address	for ubiboard1: kernel img is here*/

#define CONFIG_SYS_MEMTEST_START	CONFIG_UCLINUX_MEM_START_ADDR
#define CONFIG_SYS_MEMTEST_END		(CONFIG_RAMBASE + CONFIG_RAMSIZE - CONFIG_UBOOT_MEM_SIZE - 4)

/*
 * Flash Memory Map
 */
#define FLASHSTART			0x60000000

#define EXTFLASH_MIN_TOTAL_SIZE_KB 	16384
#define EXTFLASH_MAX_PAGE_SIZE_KB 	256
#define EXTFLASH_MAX_ACCESS_TIME 	20
#define EXTFLASH_PAGE_SIZE 		((EXTFLASH_MAX_PAGE_SIZE_KB) * 1024)
#define EXTFLASH_TOTAL_SIZE 		((EXTFLASH_MIN_TOTAL_SIZE_KB) * 1024)

/* 
 * FLASH and environment organization
 */
#define CONFIG_SYS_MAX_FLASH_BANKS	1		/* max number of memory banks		*/
#define CONFIG_SYS_MAX_FLASH_SECT	(EXTFLASH_MIN_TOTAL_SIZE_KB/EXTFLASH_MAX_PAGE_SIZE_KB)	/* max number of sectors on one chip	*/
#define CONFIG_SYS_FLASH_SIZE		EXTFLASH_TOTAL_SIZE					/* max number of sectors on one chip	*/
#define CONFIG_SYS_FLASH_BASE		FLASHSTART

/*
 * Console settings
 */
#define	CONFIG_SYS_PROMPT		"Ubicom =>"	/* Monitor Command Prompt	*/
#define	CONFIG_SYS_CBSIZE		256		/* Console I/O Buffer Size	*/
#define	CONFIG_SYS_PBSIZE 		(CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16) 	/* Print Buffer Size */
#define CONFIG_CMDLINE_EDITING		1		/* add command line history	*/
#define CONFIG_AUTO_COMPLETE		1		/* add autocompletion support	*/

/*
 * Other settings
 */
//#define DEBUG				1
#define CONFIG_SYS_LONGHELP				/* undef to save memory */

/*
 * USB MSC support
 */
#define CONFIG_MUSB_HCD
#define CONFIG_USB_IP7K
#define CONFIG_CMD_USB
#define CONFIG_USB_STORAGE
#define CONFIG_CMD_STORAGE
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION
#ifdef CONFIG_USB_KEYBOARD
#define CONFIG_SYS_USB_EVENT_POLL
#define CONFIG_PREBOOT "usb start"
#endif

/*
 * USB Emergency Upgrade
 */
#define CONFIG_CMD_USB_UPGRADE
#define CONFIG_CMD_USB_UPGRADE_DEVICE		"nor0"
#define CONFIG_CMD_USB_UPGRADE_PARTITION	"upgrade"
#define CONFIG_CMD_USB_UPGRADE_LOAD_ADDRESS	(CONFIG_RAMBASE + 0x01000000)
#define CONFIG_EUPGRADE
#define CONFIG_EUPGRADE_CHARLCD
#define CONFIG_EUPGRADE_CHARLCD_DATA_PORT	RI
#define CONFIG_EUPGRADE_CHARLCD_RS_PORT		RI
#define CONFIG_EUPGRADE_CHARLCD_RS_BIT		8
#define CONFIG_EUPGRADE_CHARLCD_RW_PORT		RI
#define CONFIG_EUPGRADE_CHARLCD_RW_BIT		9
#define CONFIG_EUPGRADE_CHARLCD_E_PORT		RI
#define CONFIG_EUPGRADE_CHARLCD_E_BIT		10

#endif	/* __CONFIG_H */
