/*
 * arch/ubicom32/include/asm/flash.h
 *   Ubicom32 architecture generic FC helper
 *
 * Compatible with the following drivers:
 *	ubi32-fc
 *	ubi32-nand-spi-er
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
#ifndef __UBICOM32_FLASH_H__
#define __UBICOM32_FLASH_H__

#ifdef CONFIG_UBICOM32_MULTI_FLASH

/*
 * Use the following API if the board has more than one flash
 */
typedef void (*ubicom32_flash_select_fn)(void *appdata);

extern void ubicom32_flash_select(void *appdata);
extern void ubicom32_flash_unselect(void *appdata);

extern int __init ubicom32_flash_add(const char *device_name, int id, const void *pdata, size_t sz);
extern void __init ubicom32_flash_init(void);
extern void *ubicom32_flash_alloc(ubicom32_flash_select_fn select, ubicom32_flash_select_fn unselect, void *appdata);

#else // CONFIG_UBICOM32_MULTI_FLASH

/*
 * Use only the following API if the board has one NOR flash
 */
extern int __init ubicom32_flash_single_init(void);

#endif // CONFIG_UBICOM32_MULTI_FLASH

#endif

#if 0
/*
 * Example usage:
 */
ostatic void myboard_select(void *appdata)
{
	switch((int)appdata) {
	case 0:
		gpio_set_value(GPIO_RA_6, 0);
		gpio_set_value(GPIO_RA_7, 0);
		break;
	case 1:
		gpio_set_value(GPIO_RA_6, 1);
		gpio_set_value(GPIO_RA_7, 0);
		break;
	case 2:
		gpio_set_value(GPIO_RA_6, 0);
		gpio_set_value(GPIO_RA_7, 1);
		break;
	case 3:
		gpio_set_value(GPIO_RA_6, 1);
		gpio_set_value(GPIO_RA_7, 1);
		break;
	}
}

static int __init board_init(void)
{
	:
	:

	ubicom32_flash_init();

	/*
	 * 3x NOR Flash
	 */
	tmp = ubicom32_flash_alloc(myboard_select, NULL, (void *)0);
	if (!tmp) {
		printk(KERN_WARNING "Could not allocate flash 0\n");
		return -ENOMEM;
	}
	memset(&fcpd, 0, sizeof(fcpd));
	fcpd.select = ubicom32_flash_select;
	fcpd.unselect = ubicom32_flash_unselect;
	fcpd.appdata = tmp;
	ubicom32_flash_add("ubicom32fc", 0, &fcpd, sizeof(fcpd));

	tmp = ubicom32_flash_alloc(myboard_select, NULL, (void *)1);
	if (!tmp) {
		printk(KERN_WARNING "Could not allocate flash 1\n");
		return -ENOMEM;
	}
	fcpd.appdata = tmp;
	ubicom32_flash_add("ubicom32fc", 1, &fcpd, sizeof(fcpd));

	tmp = ubicom32_flash_alloc(myboard_select, NULL, (void *)2);
	if (!tmp) {
		printk(KERN_WARNING "Could not allocate flash 2\n");
		return -ENOMEM;
	}
	fcpd.appdata = tmp;
	ubicom32_flash_add("ubicom32fc", 2, &fcpd, sizeof(fcpd));

	/*
	 * NAND Flash
	 */
	tmp = ubicom32_flash_alloc(myboard_select, NULL, (void *)3);
	if (!tmp) {
		printk(KERN_WARNING "Could not allocate flash 3\n");
		return -ENOMEM;
	}
	memset(&ncpd, 0, sizeof(ncpd));
	nspd.select = ubicom32_flash_select;
	nspd.unselect = ubicom32_flash_unselect;
	nspd.appdata = tmp;
	ubicom32_flash_add("ubi32-nand-spi-er", 3, &fcpd, sizeof(fcpd));

	:
	:
}
#endif
