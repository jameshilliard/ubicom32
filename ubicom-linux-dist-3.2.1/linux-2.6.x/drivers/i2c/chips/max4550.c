/*
 * max4550.c
 *	Support for Maxim MAX4550:
 *		Dual 4x2, Clickless Audio/Video Crosspoint Switch
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

#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/i2c/max4550.h>
#ifdef CONFIG_PROC_FS
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#endif

#include <asm/gpio.h>

#define MAX4550_BIASMODE	(1 << 5)
#define MAX4550_AUX		(1 << 4)
#define MAX4550_COM2B		(1 << 3)
#define MAX4550_COM1B		(1 << 2)
#define MAX4550_COM2A		(1 << 1)
#define MAX4550_COM1A		(1 << 0)

static const struct i2c_device_id max4550_id[] = {
	{ "max4550", },
	{ }
};
MODULE_DEVICE_TABLE(i2c, max4550_id);

struct max4550_chip {
	unsigned		gpio_base;
	uint8_t			aux_reg;

	bool			has_gpio;

	struct i2c_client	*client;
	struct gpio_chip	gpio_chip;

#ifdef CONFIG_PROC_FS
	struct proc_dir_entry	*pde_root;
#endif
};

/*
 * max4550_write
 */
static int max4550_write(struct max4550_chip *chip, uint8_t reg, uint8_t val)
{
	uint8_t data[2];

	data[0] = reg;
	data[1] = val;

	return i2c_master_send(chip->client, data, sizeof(data));
}

/*
 * max4550_gpio_get_value
 *	Return the value of the output bit
 *
 * Note that the MAX4550 is a receive only device, so we have to report
 * the state of the GPIO using our cached register
 */
static int max4550_gpio_get_value(struct gpio_chip *gc, unsigned bit)
{
	struct max4550_chip *chip;

	if (bit > 3) {
		return -EINVAL;
	}

	chip = container_of(gc, struct max4550_chip, gpio_chip);

	return (chip->aux_reg & (1 << bit)) ? 1 : 0;
}

/*
 * max4550_gpio_set_value
 *	Set the value of the output bit
 */
static void max4550_gpio_set_value(struct gpio_chip *gc, unsigned bit, int val)
{
	struct max4550_chip *chip;
	uint8_t newval;
	int ret;

	if (bit > 3) {
		return;
	}

	chip = container_of(gc, struct max4550_chip, gpio_chip);

	newval = chip->aux_reg;
	newval &= ~(1 << bit);
	if (val) {
		newval |= (1 << bit);
	}

	ret = max4550_write(chip, MAX4550_AUX, newval);
	if (ret) {
		return;
	}

	/*
	 * Only update our cached value once we know that the write
	 * was successful
	 */
	chip->aux_reg = newval;
}

/*
 * max4550_gpio_direction_output
 */
static int max4550_gpio_direction_output(struct gpio_chip *gc,
		unsigned bit, int val)
{
	if (bit > 3) {
		return -EINVAL;
	}

	/*
	 * MAX4550 is always output
	 */
	max4550_gpio_set_value(gc, bit, val);
	return 0;
}

#ifdef CONFIG_PROC_FS
/*
 * max4550_procfs_write
 */
static ssize_t max4550_procfs_write(struct file *file, const char __user *buffer,
					 size_t size, loff_t *pos)
{
	struct proc_dir_entry *pde = PDE(file->f_dentry->d_inode);
	struct max4550_chip *chip = (struct max4550_chip *)pde->parent->data;
	unsigned long val;
	char buf[13];
	int ret;

	/*
	 * Copy and truncate
	 */
	if (size > (sizeof(buf) - 1)) {
		return -EINVAL;
	}
	if (copy_from_user(buf, buffer, size)) {
		return -EFAULT;
	}
	buf[size] = 0;

	ret = strict_strtoul(buf, 0, &val);
	if (ret) {
		return ret;
	}
	ret = max4550_write(chip, (uint32_t)pde->data, val);
	if (ret < 0) {
		return ret;
	}

	return size;
}

static const struct file_operations max4550_procfs_fops = {
	.write		= max4550_procfs_write,
};

/*
 * max4550_procfs_free
 */
static void max4550_procfs_free(struct max4550_chip *chip)
{
	if (!chip->pde_root) {
		return;
	}

	remove_proc_entry("com1a", chip->pde_root);
	remove_proc_entry("com2a", chip->pde_root);
	remove_proc_entry("com1b", chip->pde_root);
	remove_proc_entry("com2b", chip->pde_root);
	remove_proc_entry("com1", chip->pde_root);
	remove_proc_entry("com2", chip->pde_root);
	remove_proc_entry("biasmode", chip->pde_root);
	remove_proc_entry("max4550", NULL);
}

/*
 * max4550_procfs_create
 */
static int __devinit max4550_procfs_create(struct max4550_chip *chip)
{
	struct proc_dir_entry	*pde;

	chip->pde_root = proc_mkdir("max4550", NULL);
	if (!chip->pde_root) {
		return -ENOMEM;
	}
	chip->pde_root->data = chip;

	pde = proc_create("com1", 0200, chip->pde_root, &max4550_procfs_fops);
	if (!pde) {
		return -ENOMEM;
	}
	pde->data = (void *)(MAX4550_COM1A | MAX4550_COM1B);

	pde = proc_create("com1a", 0200, chip->pde_root, &max4550_procfs_fops);
	if (!pde) {
		return -ENOMEM;
	}
	pde->data = (void *)MAX4550_COM1A;

	pde = proc_create("com1b", 0200, chip->pde_root, &max4550_procfs_fops);
	if (!pde) {
		return -ENOMEM;
	}
	pde->data = (void *)MAX4550_COM1B;

	pde = proc_create("com2", 0200, chip->pde_root, &max4550_procfs_fops);
	if (!pde) {
		return -ENOMEM;
	}
	pde->data = (void *)(MAX4550_COM2A | MAX4550_COM2B);

	pde = proc_create("com2a", 0200, chip->pde_root, &max4550_procfs_fops);
	if (!pde) {
		return -ENOMEM;
	}
	pde->data = (void *)MAX4550_COM2A;

	pde = proc_create("com2b", 0200, chip->pde_root, &max4550_procfs_fops);
	if (!pde) {
		return -ENOMEM;
	}
	pde->data = (void *)MAX4550_COM2B;

	pde = proc_create("biasmode", 0200, chip->pde_root, &max4550_procfs_fops);
	if (!pde) {
		return -ENOMEM;
	}
	pde->data = (void *)(MAX4550_BIASMODE);

	return 0;
}
#endif

/*
 * max4550_probe
 */
static int __devinit max4550_probe(struct i2c_client *client,
				   const struct i2c_device_id *id)
{
	struct max4550_platform_data *pdata;
	struct max4550_chip *chip;
	int ret = -ENOMEM;;

	pdata = client->dev.platform_data;

	chip = kzalloc(sizeof(struct max4550_chip), GFP_KERNEL);
	if (chip == NULL) {
		return -ENOMEM;
	}

	chip->client = client;

#ifdef CONFIG_PROC_FS
	if (max4550_procfs_create(chip)) {
		goto failed;
	}
#endif

	/*
	 * If we were given platform data then setup the GPIO, 
	 * if necessary.
	 */
	if (!pdata || !pdata->gpio_base) {
		goto done;
	}

	chip->gpio_base = pdata->gpio_base;

	/*
	 * The MAX4550 is write only, so we do not know the initial state of
	 * the gpio.  We can write the default given in the pdata.
	 */
	ret = max4550_write(chip, MAX4550_AUX, pdata->gpio_default & 0x0F);
	if (ret) {
		printk(KERN_WARNING "MAX4550 failed write addr=0x%02x\n", client->addr);
		goto failed;
	}
	chip->aux_reg = pdata->gpio_default & 0x0F;

	chip->gpio_chip.direction_output = max4550_gpio_direction_output;
	chip->gpio_chip.get = max4550_gpio_get_value;
	chip->gpio_chip.set = max4550_gpio_set_value;
	chip->gpio_chip.base = chip->gpio_base;
	chip->gpio_chip.label = chip->client->name;
	chip->gpio_chip.can_sleep = 1;
	chip->gpio_chip.ngpio = 4;
	chip->gpio_chip.dev = &chip->client->dev;
	chip->gpio_chip.owner = THIS_MODULE;

	ret = gpiochip_add(&chip->gpio_chip);
	if (ret) {
		printk(KERN_WARNING "MAX4550 failed to add gpio chip addr=0x%02x\n", client->addr);
		goto failed;
	}

	printk(KERN_INFO "MAX4550 GPIO base: %d\n", pdata->gpio_base);

done:
	printk(KERN_INFO "MAX4550 driver at address 0x%02x\n", client->addr);
	i2c_set_clientdata(client, chip);
	return 0;

failed:
#ifdef CONFIG_PROC_FS
	max4550_procfs_free(chip);
#endif
	kfree(chip);
	return ret;
}

static int max4550_remove(struct i2c_client *client)
{
	struct max4550_chip *chip = i2c_get_clientdata(client);
	int ret = 0;

#ifdef CONFIG_PROC_FS
	max4550_procfs_free(chip);
#endif

	if (chip->has_gpio) {
		ret = gpiochip_remove(&chip->gpio_chip);
		if (ret) {
			dev_err(&client->dev, "%s failed, %d\n",
					"gpiochip_remove()", ret);
			return ret;
		}
	}

	kfree(chip);
	return 0;
}

static struct i2c_driver max4550_driver = {
	.driver = {
		.name	= "max4550",
	},
	.probe		= max4550_probe,
	.remove		= max4550_remove,
	.id_table	= max4550_id,
};

static int __init max4550_init(void)
{
	return i2c_add_driver(&max4550_driver);
}
/* register after i2c postcore initcall and before
 * subsys initcalls that may rely on these GPIOs
 */
subsys_initcall(max4550_init);

static void __exit max4550_exit(void)
{
	i2c_del_driver(&max4550_driver);
}
module_exit(max4550_exit);

MODULE_AUTHOR("Patrick Tjin <pat.tjin@ubicom.com>");
MODULE_DESCRIPTION("Driver for MAX4550");
MODULE_LICENSE("GPL");
