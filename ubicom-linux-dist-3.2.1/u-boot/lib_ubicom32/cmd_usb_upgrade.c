/*
 * cmd_usb_upgrade.c
 *	Command for doing upgrades from usb
 *
 * Copyright (C) 2010 Ubicom, Inc.
 * http://www.ubicom.com
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
 */
#include <common.h>
#include <asm/devtree.h>
#include <linux/list.h>
#include <jffs2/load_kernel.h>
#include <asm/errno.h>
#include "cmd_usb_upgrade.h"

/*
 * Defines:
 *	CONFIG_CMD_USB_UPGRADE_LOAD_ADDRESS
 *		Address where we will load the upgrade image
 *	CONFIG_CMD_USB_UPGRADE_DEVICE
 *		Device name to search write image to, i.e. 'nor0'
 *	CONFIG_CMD_USB_UPGRADE_PARTITION
 *		Partition name to search write image to, i.e. 'upgrade'
 */

/*
 * Incremental write size
 */
#define CMD_USB_UPGRADE_WRITE_SIZE	(32 * 1024)

#ifndef CONFIG_CMD_USB
#error CONFIG_CMD_USB must be defined for this module to work
#endif
#ifndef CONFIG_CMD_FAT
#error CONFIG_CMD_FAT must be defined for this module to work
#endif

extern struct list_head devices;
extern struct boardnode *bn;

usb_upgrade_status_callback_t usb_upgrade_status_callback = NULL;
#define USB_UPGRADE_STATUS_CALLBACK(state, step, steps, status)			\
	if (usb_upgrade_status_callback) {					\
		usb_upgrade_status_callback(state, step, steps, status);	\
	}

/*
 * do_usb_upgrade
 */
static int do_usb_upgrade(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned int load_addr = CONFIG_CMD_USB_UPGRADE_LOAD_ADDRESS;

	/*
	 * need at least two arguments 
	 */
	if (argc < 2)
		goto usage;

	char *device = argv[1];
	char *filename = argv[2];

	printf("Upgrade from usb device %s:%s\n", device, filename);

	/*
	 * Make sure that mtdparts has been done
	 */
	if (!getenv("mtdids")) {
		int res = run_command("mtdparts", 0);
		if (res < 0) {
			puts("Failed to perform mtdparts\n");
			return 1;
		}
	}

	/*
	 * Execute fatload
	 */
	USB_UPGRADE_STATUS_CALLBACK(USB_UPGRADE_STATE_READ, 0, 1, 0);
	char s[80];
	sprintf(s, "fatload usb %s 0x%08x %s", device, load_addr, filename);
	int res = run_command(s, 0);
	if (res < 0) {
		puts("Failed to load file\n");
		USB_UPGRADE_STATUS_CALLBACK(USB_UPGRADE_STATE_READ, 1, 1, -EINVAL);
		return 2;
	}
	USB_UPGRADE_STATUS_CALLBACK(USB_UPGRADE_STATE_READ, 1, 1, 0);

	/*
	 * File size (in hex) should have been put in the env by fatload
	 */
	char *size = getenv("filesize");
	if (!size) {
		puts("Failed to get filesize from env\n");
		return 3;
	}
	size_t fsize = simple_strtoul(size, NULL, 16);
	printf("Loaded %d bytes at 0x%08x\n", fsize, load_addr);

	/*
	 * Check the image
	 */
	USB_UPGRADE_STATUS_CALLBACK(USB_UPGRADE_STATE_VERIFY, USB_UPGRADE_VERIFY_STEP_HEADER, 4, 0);
	image_header_t *hdr = (image_header_t *)load_addr;
	image_print_contents(hdr);
	if (!bn) {
		puts("Failed to find board node\n");
		return 4;
	}

	USB_UPGRADE_STATUS_CALLBACK(USB_UPGRADE_STATE_VERIFY, USB_UPGRADE_VERIFY_STEP_IDENTITY, 4, 0);
	if (strcmp(bn->identity_string, image_get_name(hdr)) != 0) {
		puts("Identity string mismatch\n");
		printf("Identity       : '%s'\n", bn->identity_string);
		printf("Image Identity : '%s'\n", image_get_name(hdr));
		USB_UPGRADE_STATUS_CALLBACK(USB_UPGRADE_STATE_VERIFY, USB_UPGRADE_VERIFY_STEP_IDENTITY, 4, -EINVAL);
		return 5;
	}

	USB_UPGRADE_STATUS_CALLBACK(USB_UPGRADE_STATE_VERIFY, USB_UPGRADE_VERIFY_STEP_HEADER_CRC, 4, 0);
	if (!image_check_hcrc(hdr)) {
		puts("Bad header crc\n");
		USB_UPGRADE_STATUS_CALLBACK(USB_UPGRADE_STATE_VERIFY, USB_UPGRADE_VERIFY_STEP_HEADER_CRC, 4, -EINVAL);
		return 6;
	}

	USB_UPGRADE_STATUS_CALLBACK(USB_UPGRADE_STATE_VERIFY, USB_UPGRADE_VERIFY_STEP_DATA_CRC, 4, 0);
	if (!image_check_dcrc(hdr)) {
		puts("Bad data crc\n");
		USB_UPGRADE_STATUS_CALLBACK(USB_UPGRADE_STATE_VERIFY, USB_UPGRADE_VERIFY_STEP_DATA_CRC, 4, -EINVAL);
		return 7;
	}
	USB_UPGRADE_STATUS_CALLBACK(USB_UPGRADE_STATE_VERIFY, 4, 4, 0);

	/*
	 * Find the appropriate partition
	 */
	struct list_head *dentry;
	struct list_head *pentry;
	struct part_info *part;
	struct mtd_device *dev;
	unsigned int addr = 0;
	list_for_each(dentry, &devices) {
		dev = list_entry(dentry, struct mtd_device, link);
		sprintf(s, "%s%d", MTD_DEV_TYPE(dev->id->type), dev->id->num);
		if (strcmp(s, CONFIG_CMD_USB_UPGRADE_DEVICE) == 0) {
			list_for_each(pentry, &dev->parts) {
				part = list_entry(pentry, struct part_info, link);
				if (strcmp(part->name, CONFIG_CMD_USB_UPGRADE_PARTITION) == 0) {
					addr = FLASHSTART + part->offset;
					if (part->size < fsize) {
						printf("Partition size too small for image (%d < %d)\n", part->size, fsize);
						return 8;
					}
					break;
				}
			}
		}
	}
	if (!addr) {
		printf("Could not find destination %s:%s\n", CONFIG_CMD_USB_UPGRADE_DEVICE, CONFIG_CMD_USB_UPGRADE_PARTITION);
		return 9;
	}
	printf("Found load address 0x%08x\n", addr);

	/*
	 * Looks good, erase the flash
	 */
	unsigned int erase_addr = addr;
	int erase_size = fsize;
	while (erase_size > 0) {
		USB_UPGRADE_STATUS_CALLBACK(USB_UPGRADE_STATE_ERASE, fsize - erase_size, fsize, 0);
		sprintf(s, "erase 0x%08x +0x%08x", erase_addr, EXTFLASH_PAGE_SIZE);
		res = run_command(s, 0);
		if (res < 0) {
			puts("Failed to erase flash\n");
			USB_UPGRADE_STATUS_CALLBACK(USB_UPGRADE_STATE_ERASE, fsize - erase_size, fsize, -EIO);
			return 10;
		}
		erase_addr += EXTFLASH_PAGE_SIZE;
		erase_size -= EXTFLASH_PAGE_SIZE;
	}
	USB_UPGRADE_STATUS_CALLBACK(USB_UPGRADE_STATE_ERASE, fsize, fsize, 0);


	/*
	 * Copy the image to flash
	 */
	int to_write = fsize;
	while (to_write > 0) {
		USB_UPGRADE_STATUS_CALLBACK(USB_UPGRADE_STATE_WRITE, fsize - to_write, fsize, 0);
		sprintf(s, "cp.b 0x%08x 0x%08x 0x%08x", load_addr, addr, CMD_USB_UPGRADE_WRITE_SIZE);
		res = run_command(s, 0);
		if (res < 0) {
			puts("Failed to copy image\n");
			USB_UPGRADE_STATUS_CALLBACK(USB_UPGRADE_STATE_WRITE, fsize - to_write, fsize, -EIO);
			return 11;
		}
		addr += CMD_USB_UPGRADE_WRITE_SIZE;
		load_addr += CMD_USB_UPGRADE_WRITE_SIZE;
		to_write -= CMD_USB_UPGRADE_WRITE_SIZE;
	}
	USB_UPGRADE_STATUS_CALLBACK(USB_UPGRADE_STATE_WRITE, fsize, fsize, 0);

	return 0;

usage:
	printf("Usage:\n%s\n", cmdtp->help);
	return -1;
}

U_BOOT_CMD(
	usb_upgrade,	3,	0,	do_usb_upgrade,
	"Perform firmware upgrade from usb",
	"usb_upgrade <device[:part]> <filename>\n"
	"   USB device must be FAT formatted\n");

