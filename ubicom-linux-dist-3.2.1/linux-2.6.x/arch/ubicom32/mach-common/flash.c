/*
 * arch/ubicom32/mach-common/flash.c
 *   Generic initialization for ubicom32fc, ubicom32_nand_spi_er
 *
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
 */

#include <linux/platform_device.h>

#ifdef CONFIG_UBICOM32_MULTI_FLASH
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <asm/flash.h>


static struct mutex flash_lock;
static struct mutex flash_select_lock;

static struct ubicom32_flash_info *flashes = NULL;
static struct ubicom32_flash_info *cur_sel = NULL;

struct ubicom32_flash_info {
	struct ubicom32_flash_info	*next;
	ubicom32_flash_select_fn	select;
	ubicom32_flash_select_fn	unselect;
	void				*appdata;
};

/*
 * ubicom32_flash_unselect
 */
void ubicom32_flash_unselect(void *appdata)
{
	BUG_ON(cur_sel != appdata);

	if (cur_sel->unselect) {
		cur_sel->unselect(cur_sel->appdata);
	}
	cur_sel = NULL;

	/*
	 * Unblock select calls
	 */
	mutex_unlock(&flash_select_lock);
}

/*
 * ubicom32_flash_select
 */
void ubicom32_flash_select(void *appdata)
{
	struct ubicom32_flash_info *info;
	BUG_ON(appdata == NULL);

	/*
	 * Block while another flash chip is selected.
	 */
	mutex_lock(&flash_select_lock);

	/*
	 * Grab the flash_lock while iterating over flashes.
	 */
	mutex_lock(&flash_lock);
	info = flashes;
	while (info && info != appdata) {
		info = info->next;
	}
	mutex_unlock(&flash_lock);

	BUG_ON(info == NULL);
	cur_sel = info;

	if (info->select) {
		info->select(info->appdata);
	}
}

/*
 * ubicom32_flash_alloc
 *	Used when multiple flash instances exist on the main flash controller
 */
void *ubicom32_flash_alloc(ubicom32_flash_select_fn select,
			   ubicom32_flash_select_fn unselect,
			   void *appdata)
{
	struct ubicom32_flash_info *info;

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (!info) {
		return NULL;
	}

	info->select = select;
	info->unselect = unselect;
	info->appdata = appdata;

	mutex_lock(&flash_lock);

	if (!flashes) {
		flashes = info;
	} else {
		info->next = flashes;
		flashes = info;
	}

	mutex_unlock(&flash_lock);

	return info;
}

/*
 * ubicom32_flash_add
 *	Add a flash device
 */
int __init ubicom32_flash_add(const char *device_name, int id, const void *pdata, size_t sz)
{
	int res;
	/*
	 * The platform_device structure which is passed to the driver
	 */
	struct platform_device *pdev;

	pdev = platform_device_alloc(device_name, id);
	if (!pdev) {
		printk(KERN_WARNING "Failed to alloc flash device '%s'\n", device_name);
		return -ENOMEM;
	}
	
	if (pdata) {
		res = platform_device_add_data(pdev, pdata, sz);
		if (res < 0) {
			printk(KERN_WARNING "Failed to add data '%s'\n", device_name);
			return res;
		}
	}

	/*
	 * Try to get the device registered
	 */
	res = platform_device_add(pdev);
	if (res < 0) {
		printk(KERN_WARNING "Failed to register flash '%s'\n", device_name);
	} else {
		printk(KERN_INFO "Added flash '%s'\n", device_name);
	}

	return res;
}

/*
 * ubicom32_flash_init
 */
void __init ubicom32_flash_init(void)
{
	printk(KERN_INFO "ubicom32 flash helper init\n");

	mutex_init(&flash_lock);
	mutex_init(&flash_select_lock);
}

#else // CONFIG_UBICOM32_MULTI_FLASH

/*
 * ubicom32_flash_single_init
 *	Call this function when the board only has one NOR flash
 */
int __init ubicom32_flash_single_init(void)
{
	struct platform_device *pdev = platform_device_alloc("ubicom32fc", 0);
	if (!pdev) {
		printk(KERN_WARNING "Failed to alloc flash device\n");
		return -ENOMEM;
	}
	return platform_device_add(pdev);
}

#endif // CONFIG_UBICOM32_MULTI_FLASH

