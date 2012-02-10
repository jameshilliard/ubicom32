/*
 * drivers/mtd/maps/ubicom32flash.c
 *
 * Ubicom32 flash driver on GPIO pins.
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
 *
 * This driver assumes the following:
 *	RC[n:0]		Address lines
 *	RB[15:0]	Data lines
 *
 * Defaults (overridable via platform data):
 *	n		26
 *	RC30		nOE
 *	RC29		nWE
 *	RC28		nCE
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/platform_device.h>
#include <linux/types.h>

#include <asm/ubicom32.h>
#include <linux/gpio.h>
#include <linux/io.h>
#include <asm/unaligned.h>
#include <linux/delay.h>
#include <asm/ubicom32flash.h>

#undef ndelay
#define ndelay(x) udelay(2)

#define pr_devinit(fmt, args...) ({ static const __initdata char __fmt[] = fmt; printk(__fmt, ## args); })

#define DRIVER_NAME "ubicom32flash"

struct ubicom32flash_info {
	struct mtd_info	*mtd;
	struct map_info	map;

	int		oe;
	int		ce;
	int		we;

	int		n_addr_bits;
	unsigned int	addr_mask;

	unsigned int	tdh;
	unsigned int	tds;
	unsigned int	twp;
	unsigned int	twc;
	unsigned int	tacc;
};

/*
 * ubicom32_flash_read_word
 */
static uint16_t ubicom32_flash_read_word(struct ubicom32flash_info *info, uint32_t addr)
{
	uint32_t data;
	volatile struct ubicom32_io_port *rb = (struct ubicom32_io_port *)IO_PORT_RB;
	volatile struct ubicom32_io_port *rc = (struct ubicom32_io_port *)IO_PORT_RC;

	/*
	 * Set the address
	 */
	rc->gpio_out &= ~info->addr_mask;
	rc->gpio_out |= (addr & info->addr_mask);

	/*
	 * Assert nCE + nOE
	 */
	gpio_set_value(info->ce, 0);
	gpio_set_value(info->oe, 0);
	ndelay(info->tacc);
	data = rb->gpio_in & 0xFFFF;

	/*
	 * Deassert nCE + nOE
	 */
	gpio_set_value(info->oe, 1);
	gpio_set_value(info->ce, 1);

	return data;
}

/*
 * ubicom32_flash_read
 *	Assumes bus is in read state
 */
static map_word ubicom32_flash_read(struct map_info *map, unsigned long ofs)
{
	struct ubicom32flash_info *info = (struct ubicom32flash_info *)map->map_priv_1;
	map_word read;

	read.x[0] = ubicom32_flash_read_word(info, ofs >> 1);
	return read;
}

/*
 * ubicom32_flash_copy_from
 */
static void ubicom32_flash_copy_from(struct map_info *map, void *to, unsigned long from, ssize_t len)
{
	struct ubicom32flash_info *info = (struct ubicom32flash_info *)map->map_priv_1;
	uint32_t addr = from;
	uint32_t dest = (uint32_t)to;
	uint8_t *b = to;
	uint16_t *p = to;
	uint16_t prev_word;

	printk(KERN_INFO "Flash copy from to=%p from=%lx len=%d\n", to, from, len);

	addr >>= 1;
	addr &= info->addr_mask;

	/*
	 * Aligned source and Aligned dest
	 *	Typically found when accessing via block interface
	 * Unaligned source and Unaligned dest
	 */
	if ((dest & 1) == (from & 1)) {
		/*
		 * Align ourselves.  If dest is unaligned, src will be as well.
		 * If we read one byte, both will become aligned.
		 */
		if (dest & 1) {
			*b++ = ubicom32_flash_read_word(info, addr) >> 8;
			addr++;
			len--;
			p = (uint16_t *)((uint8_t *)to + 1);
		}

		while (len >= 2) {
			*p++ = ubicom32_flash_read_word(info, addr);
			addr += 2;;
			len -= 2;
		}

		if (len) {
			b = (uint8_t *)p;
			*b = ubicom32_flash_read_word(info, addr) >> 8;
		}
		return;
	}

	/*
	 * Different alignments, we have to do it byte by byte
	 */
	if (from & 1) {
		/*
		 * Source is unaligned, Destination is aligned
		 */
		prev_word = ubicom32_flash_read_word(info, addr);
		addr++;
		while (len >= 2) {
			uint16_t cur_word = ubicom32_flash_read_word(info, addr);
			*p++ = (prev_word << 8) | (cur_word >> 8);
			addr += 2;
			len -= 2;
			prev_word = cur_word;
		}
		if (len) {
			b = (uint8_t *)p;
			*b = prev_word & 0xFF;
		}
		return;
	}

	/*
	 * Source is aligned, Destination is unaligned
	 */
	while (len >= 2) {
		uint16_t data = ubicom32_flash_read_word(info, addr);
		*b++ = data >> 8;
		*b++ = data & 0xFF;
		addr += 2;
		len -= 2;
	}

	if (len) {
		b = (uint8_t *)p;
		*b = ubicom32_flash_read_word(info, addr) >> 8;
	}
}

/*
 * ubicom32_flash_write
 *	leaves bus in read state
 */
static void ubicom32_flash_write(struct map_info *map, map_word d1, unsigned long ofs)
{
	struct ubicom32flash_info *info = (struct ubicom32flash_info *)map->map_priv_1;

	volatile struct ubicom32_io_port *rb = (struct ubicom32_io_port *)IO_PORT_RB;
	volatile struct ubicom32_io_port *rc = (struct ubicom32_io_port *)IO_PORT_RC;

	/*
	 * Set the address and data
	 */
	rc->gpio_out &= ~info->addr_mask;
	rc->gpio_out |= ((ofs >> 1) & info->addr_mask);

	rb->gpio_out &= ~0xFFFF;
	rb->gpio_out = d1.x[0];
	rb->gpio_ctl |= 0xFFFF;

	ndelay(info->tds);

	/*
	 * Assert nCE
	 */
	gpio_set_value(info->ce, 0);

	/*
	 * Toggle nWE
	 */
	gpio_set_value(info->we, 0);
	ndelay(info->twp);
	gpio_set_value(info->we, 1);
	ndelay(info->tdh);

	/*
	 * Disable output and Deassert nCE
	 */
	rb->gpio_ctl &= ~0xFFFF;
	gpio_set_value(info->ce, 1);

	/*
	 * Take care of any remaining write cycle delay
	 */
	ndelay(info->twc);
}

#ifdef CONFIG_MTD_PARTITIONS
static const char *part_probe_types[] = { "cmdlinepart", NULL };
#endif

/*
 * ubicom32_flash_probe
 */
static int __devinit ubicom32_flash_probe(struct platform_device *pdev)
{
	int i;
	int ret = 0;
	struct ubicom32flash_platform_data *pdata = pdev->dev.platform_data;
	struct ubicom32flash_info *info;
#ifdef CONFIG_MTD_PARTITIONS
	struct mtd_partition *parts = NULL;
#endif

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (!info) {
		return -ENOMEM;
	}

	info->map.name       = DRIVER_NAME;
	info->map.read       = ubicom32_flash_read;
	info->map.copy_from  = ubicom32_flash_copy_from;
	info->map.write      = ubicom32_flash_write;
	info->map.bankwidth  = 2;
	info->map.size       = 0x10000000;
	info->map.virt       = (void __iomem *)0xF0000000;
	info->map.phys       = 0xF0000000;
	info->map.map_priv_1 = (unsigned long)info;

	/*
	 * By default, we'll use RC[25:0] for Address and RB[15:0]
	 * RC30 for nOE, RC29 for nWE, and RC28 for nCE.
	 */
	if (pdata && pdata->has_control) {
		info->oe = pdata->oe_pin;
		info->we = pdata->we_pin;
		info->ce = pdata->ce_pin;
		if (pdata->n_addr_bits) {
			info->n_addr_bits = pdata->n_addr_bits;
		}
	} else {
		info->oe = GPIO_RC_30;
		info->we = GPIO_RC_29;
		info->ce = GPIO_RC_28;
		info->n_addr_bits = 26;
	}

	info->addr_mask = (1 << info->n_addr_bits) - 1;

	for (i = 0; i < info->n_addr_bits; i++) {
		if (gpio_request(GPIO_RC_0 + i, DRIVER_NAME ":Addr")) {
			pr_devinit(KERN_ERR DRIVER_NAME ":Failed to request gpio %d\n", GPIO_RC_0 + i);
			ret = -EBUSY;
			goto fail;
		}
		gpio_direction_output(GPIO_RC_0 + i, 0);
	}

	for (i = GPIO_RB_0; i <= GPIO_RB_15; i++) {
		if (gpio_request(i, DRIVER_NAME ":Data")) {
			pr_devinit(KERN_ERR DRIVER_NAME ":Failed to request gpio %d\n", i);
			ret = -EBUSY;
			goto fail;
		}
		gpio_direction_input(i);
	}

	if (gpio_request(info->oe, DRIVER_NAME ":OE")) {
		pr_devinit(KERN_ERR DRIVER_NAME ":Failed to request gpio %d\n", i);
		ret = -EBUSY;
		goto fail;
	}
	gpio_direction_output(info->oe, 1);

	if (gpio_request(info->we, DRIVER_NAME ":WE")) {
		pr_devinit(KERN_ERR DRIVER_NAME ":Failed to request gpio %d\n", i);
		ret = -EBUSY;
		goto fail;
	}
	gpio_direction_output(info->we, 1);

	if (gpio_request(info->ce, DRIVER_NAME ":CE")) {
		pr_devinit(KERN_ERR DRIVER_NAME ":Failed to request gpio %d\n", i);
		ret = -EBUSY;
		goto fail;
	}
	gpio_direction_output(info->ce, 1);

	/*
	 * User can specify tDH/tDS or tWP, we'll take the larger of the
	 * two and apply the difference to tDS.
	 */
	if (!pdata) {
		info->tds = 0;
		info->tdh = 0;
		info->twc = 55;
		info->tacc = 90;
		info->twp = 35;
	} else {
		if (pdata->tdh) {
			info->tdh = pdata->tdh;
		}

		if ((pdata->tds + pdata->tdh) < pdata->twp) {
			info->tds = pdata->twp - (pdata->tds + pdata->tdh);
		}

		/*
		 * info->tWC is the difference between tDS+tDH and tWC, or 0
		 */
		if (pdata->twc) {
			info->twc = pdata->twc;
		}

		if (info->twc > (info->tds + info->tdh)) {
			info->twc -= (info->tds + info->tdh);
		} else {
			info->twc = 0;
		}

		if (pdata->tacc) {
			info->tacc = pdata->tacc;
		}
	}

	pr_devinit(KERN_NOTICE DRIVER_NAME ": probing %d-bit flash bus\n", info->map.bankwidth * 8);
	info->mtd = do_map_probe((pdata && pdata->probe_name) ? pdata->probe_name : "cfi_probe", &info->map);
	if (!info->mtd) {
		ret = -ENXIO;
		goto fail;
	}

#ifdef CONFIG_MTD_PARTITIONS
	ret = parse_mtd_partitions(info->mtd, part_probe_types, &parts, 0);
	if (ret > 0) {
		pr_devinit(KERN_NOTICE DRIVER_NAME ": Using commandline partition definition\n");
		add_mtd_partitions(info->mtd, parts, ret);
	} else
#endif
	{
		pr_devinit(KERN_NOTICE DRIVER_NAME ": no partition info available, registering whole flash at once\n");
		add_mtd_device(info->mtd);
	}

	platform_set_drvdata(pdev, info);

	return 0;

fail:
	kfree(info);
	return ret;
}

/*
 * ubicom32_flash_remove
 */
static int __devexit ubicom32_flash_remove(struct platform_device *pdev)
{
	struct ubicom32flash_info *info = platform_get_drvdata(pdev);
	int i;

	for (i = GPIO_RC_0; i <= GPIO_RC_25; i++) {
		gpio_free(i);
	}

	for (i = GPIO_RB_0; i <= GPIO_RB_15; i++) {
		gpio_free(i);
	}

	gpio_free(info->oe);
	gpio_free(info->we);
	gpio_free(info->ce);

#ifdef CONFIG_MTD_PARTITIONS
	del_mtd_partitions(info->mtd);
#endif
	map_destroy(info->mtd);
	kfree(info);
	return 0;
}

static struct platform_driver ubicom32_flash_driver = {
	.probe		= ubicom32_flash_probe,
	.remove		= __devexit_p(ubicom32_flash_remove),
	.driver		= {
		.name	= DRIVER_NAME,
	},
};

/*
 * ubicom32_flash_init
 */
static int __init ubicom32_flash_init(void)
{
	return platform_driver_register(&ubicom32_flash_driver);
}
module_init(ubicom32_flash_init);

/*
 * ubicom32_flash_exit
 */
static void __exit ubicom32_flash_exit(void)
{
	platform_driver_unregister(&ubicom32_flash_driver);
}
module_exit(ubicom32_flash_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MTD map driver for flash chips on Ubicom32 GPIO");
