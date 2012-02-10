/*
 * drivers/misc/ubicom32videoin.c
 *	Ubicom32 VideoIn driver
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
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/devtree.h>
#include <asm/ubicom32videoin.h>
#include <asm/videointio.h>

#define DRIVER_NAME	"ubicom32videoin"

struct ubicom32videoin_device {
	struct module *owner;
	struct videointio_node *vn;
	struct cdev char_dev;
};

static int ubicom32videoin_open(struct inode *inode, struct file *file)
{
	struct ubicom32videoin_device *ud = container_of(inode->i_cdev, struct ubicom32videoin_device, char_dev);

	file->private_data = ud;
	return 0;
}

static long ubicom32videoin_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct ubicom32videoin_device *ud = file->private_data;
	struct videointio_regs *regs = ud->vn->regs;

	switch (cmd) {
	case UBICOM32VIDEOIN_IOCTL_SET_HPIXELS:
		if (!(regs->status & VIDEOINTIO_STAT_STOPPED)) {
			return -EBUSY;
		}
		regs->hpixels = arg;
		break;

	case UBICOM32VIDEOIN_IOCTL_SET_VLINES:
		if (!(regs->status & VIDEOINTIO_STAT_STOPPED)) {
			return -EBUSY;
		}
		regs->vlines = arg;
		break;

	case UBICOM32VIDEOIN_IOCTL_SET_RING_BUFFER_BEGIN:
		if (!(regs->status & VIDEOINTIO_STAT_STOPPED)) {
			return -EBUSY;
		}
		regs->ring_begin = (void __user *)arg;
		break;

	case UBICOM32VIDEOIN_IOCTL_SET_RING_BUFFER_END:
		if (!(regs->status & VIDEOINTIO_STAT_STOPPED)) {
			return -EBUSY;
		}
		regs->ring_end = (void __user *)arg;
		break;

	case UBICOM32VIDEOIN_IOCTL_STOP:
		if (regs->status & VIDEOINTIO_STAT_STOPPED) {
			break;
		}
		regs->command |= VIDEOINTIO_CMD_VALID | VIDEOINTIO_CMD_STOP;
		break;

	case UBICOM32VIDEOIN_IOCTL_START:
		if (regs->status & VIDEOINTIO_STAT_STOPPED) {
			ubicom32_set_interrupt(ud->vn->dn.sendirq);
		}
		break;

	case UBICOM32VIDEOIN_IOCTL_GET_STATUS:
		return copy_to_user((u32 __user *)arg, &regs->status, sizeof(regs->status));

	default:
		return -EINVAL;
	}

	return 0;
}

static struct file_operations ubicom32videoin_fops = {
	.open = ubicom32videoin_open,
	.unlocked_ioctl = ubicom32videoin_ioctl,
	.owner = THIS_MODULE,
};

static dev_t ubicom32videoin_devt;

static int __devinit ubicom32videoin_probe(struct platform_device *pdev)
{
	struct ubicom32videoin_device *ud;
	struct videointio_node *vn;
	int ret = -ENOMEM;

	/*
	 * Check the device tree to see if the device exists
	 */
	vn = (struct videointio_node *)devtree_find_node("videointio");
	if (!vn) {
		return -ENODEV;
	}

	/*
	 * Allocate our driver information
	 */
	ud = kzalloc(sizeof(*ud), GFP_KERNEL);
	if (ud == NULL) {
		dev_err(&pdev->dev, "unable to allocate device\n");
		return -ENOMEM;
	}

	ud->owner = THIS_MODULE;
	ud->vn = vn;

	/*
	 * Initialize the character device
	 */
	cdev_init(&ud->char_dev, &ubicom32videoin_fops);
	ud->char_dev.owner = ud->owner;
	ret = cdev_add(&ud->char_dev, ubicom32videoin_devt, 1);
	if (ret) {
		printk(KERN_WARNING "ubicom32videoin: failed to add minor\n");
		kfree(ud);
		return -ENOMEM;
	}

	/*
	 * We're good to go
	 */
	printk(KERN_INFO "ubicom32videoin: reg:%p sendirq:%d ver:%d/%d dev:%d:%d\n", 
			ud->vn, vn->dn.sendirq, vn->version, vn->regs->version,
			MAJOR(ubicom32videoin_devt), MINOR(ubicom32videoin_devt));

	platform_set_drvdata(pdev, ud);
	return ret;
}

static int __devexit ubicom32videoin_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct ubicom32videoin_device *ud = dev_get_drvdata(dev);

	cdev_del(&ud->char_dev);

	kfree(ud);

	return 0;
}

static struct platform_device *ubicom32videoin_platform_device = NULL;
static struct platform_driver ubicom32videoin_platform_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},
	.probe = ubicom32videoin_probe,
	.remove = __devexit_p(ubicom32videoin_remove),
};

static int __init ubicom32videoin_init(void)
{
	int ret;

	ret = platform_driver_register(&ubicom32videoin_platform_driver);
	if (ret) {
		return ret;
	}

	/*
	 * Get a chardev region, we only have 1 device
	 */
	ret = alloc_chrdev_region(&ubicom32videoin_devt, 0, 1, DRIVER_NAME);
	if (ret < 0) {
		printk("Unable to get chardev node VideoIn, err=%d\n", ret);
		return ret;
	}

	ubicom32videoin_platform_device = platform_device_alloc(DRIVER_NAME, 0);
	if (ubicom32videoin_platform_device) {
		ubicom32videoin_platform_device->dev.devt = ubicom32videoin_devt;
		ret = platform_device_add(ubicom32videoin_platform_device);
	} else {
		ret = -ENOMEM;
	}

	if (ret) {
		unregister_chrdev_region(ubicom32videoin_devt, 1);
		platform_device_put(ubicom32videoin_platform_device);
		platform_driver_unregister(&ubicom32videoin_platform_driver);
	}

	return ret;
}
module_init(ubicom32videoin_init);

static void __exit ubicom32videoin_exit(void)
{
	unregister_chrdev_region(ubicom32videoin_devt, 1);

	platform_device_unregister(ubicom32videoin_platform_device);
	ubicom32videoin_platform_device = NULL;

	platform_driver_unregister(&ubicom32videoin_platform_driver);
}
module_exit(ubicom32videoin_exit);

MODULE_AUTHOR("Patrick Tjin <pat.tjin@ubicom.com>");
MODULE_DESCRIPTION("Ubicom32 Video In driver");
MODULE_LICENSE("GPL");
