/*
 * include/configs/IP7500_IAP_BOARD.h
 *   Support for IP7500 Internet Audio Player
 *
 * This file supports the IP7500 Internet Audio Player:
 *      8007-1110  Rev 1.0
 *      8007-1111  Rev 1.1
 *      8007-1112  Rev 1.2
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
//#define CONFIG_CMD_BSP
#define CONFIG_CMD_DHCP
//#define CONFIG_CMD_DIAG
//#define CONFIG_CMD_FAT
//#define CONFIG_CMD_JFFS2
//#define CONFIG_CMD_NFS
//#define CONFIG_CMD_PING
//#define CONFIG_CMD_REGINFO
//#define CONFIG_CMD_SNTP
//#define CONFIG_CMD_USB
#define CONFIG_CMD_FLASH
#define CONFIG_CMD_ENV
#define CONFIG_CMD_PING
#define CONFIG_CMD_NET
#define CONFIG_CMD_GPIO
//#define CONFIG_CMD_SPI
#define CONFIG_CMD_MTDPARTS
#define MTDIDS_DEFAULT "nor0=ubicom32fc.0,spier0=ubi32-nand-spi-er.0"
#define MTDPARTS_DEFAULT "mtdparts=ubicom32fc.0:256k(bootloader)ro;ubi32-nand-spi-er.0:128M(user)"

/*
 * UBI
 */
#define	CONFIG_RBTREE
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_MTDPARTS
#define CONFIG_SYS_USE_UBI
#define	CONFIG_CMD_UBI
#define	CONFIG_CMD_NAND_SPI_ER
#define	CONFIG_CMD_NAND_SPI_ER_UBI32

/*
 * UBIFS
 *	Needs unaligned access + test_and_set_bit
 */
#define	CONFIG_CMD_UBIFS
#define	CONFIG_LZO

/*
 * Serial console configuration
 */
#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }
#define CONFIG_UBICOM_UART_MAILBOX
//#define CONFIG_UBICOM_SERDES_UART
//#define CONFIG_SYS_CONSOLE_IS_IN_ENV

/*
 * Ethernet configuration
 */
#define CONFIG_PHY_ADDR		0x00
#define CONFIG_ENV_OVERWRITE	1	/* allow overwriting of ethaddr */

/*
 * POST support
 */

/*
 * Autobooting
 */
#define CONFIG_BOOTDELAY	2	/* autoboot after 5 seconds */
#define CONFIG_ETHADDR		02:03:64:de:fa:dd
#define CONFIG_IPADDR		192.168.0.100
#define CONFIG_SERVERIP		192.168.0.1
#define CONFIG_BOOTFILE		vmlinux.ub

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
#define SYSTEM_FREQ 		452000000
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
#define DEBUG				1
#define CONFIG_SYS_LONGHELP				/* undef to save memory */

#define CONFIG_SYS_DIRECT_FLASH_TFTP

/*==========================*/
/* USB MSC support (if any) */
/*==========================*/
#define CONFIG_MUSB_HCD
#define CONFIG_USB_IP7K
#ifdef CONFIG_USB_IP7K
#define CONFIG_CMD_USB
#ifdef CONFIG_MUSB_HCD
#define CONFIG_USB_STORAGE
#define CONFIG_CMD_STORAGE
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION
#endif
#ifdef CONFIG_USB_KEYBOARD
#define CONFIG_SYS_USB_EVENT_POLL
#define CONFIG_PREBOOT "usb start"
#endif
#endif

#endif	/* __CONFIG_H */
