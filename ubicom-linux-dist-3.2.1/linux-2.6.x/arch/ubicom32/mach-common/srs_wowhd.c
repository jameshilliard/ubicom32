/*
 * arch/ubicom32/mach-common/srs_wowhd.c
 *   SRS WOWHD configuration proc node
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
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
//#include <linux/io.h>
//#include <linux/gpio.h>
//#include <linux/irq.h>
//#include <linux/version.h>
//#include <linux/io.h>
#include <asm/ip5000.h>
//#include <linux/gpio.h>
#include <asm/srs_wowhd.h>

#if defined(CONFIG_PROC_FS)

#include <linux/proc_fs.h>

static struct srs_wowhd_node	*srs_node = NULL;

/*
 * ubicom32_srs_proc_write
 */
static ssize_t ubicom32_srs_proc_write(struct file *file, const char __user *buf,
				       size_t count, loff_t *ppos)
{
	struct proc_dir_entry *pde = PDE(file->f_dentry->d_inode);
	char s[13];
	int val;

	if (count > 12) {
		return -EINVAL;
	}

	if (copy_from_user(s, buf, count)) {
		return -EINVAL;
	}

	val = simple_strtoul(s, NULL, 10);

	switch((int)pde->data) {
		case 1:
			srs_node->regs->wowhd_3d_mode = val;
			break;
		case 2:
			srs_node->regs->wowhd_3d_space = val;
			break;
		case 3:
			srs_node->regs->wowhd_3d_center = val;
			break;
		case 4:
			srs_node->regs->vc_input_gain = val;
			break;
		case 5:
			srs_node->regs->vc_output_gain = val;
			break;
		case 6:
			srs_node->regs->vc_max_gain = val;
			break;
		case 7:
			if (val) {
				srs_node->regs->flags |= SRS_WOWHD_FLAG_DEFINITION_ENABLE;
			} else {
				srs_node->regs->flags &= ~SRS_WOWHD_FLAG_DEFINITION_ENABLE;
			}
			break;
		case 8:
			if (val) {
				srs_node->regs->flags |= SRS_WOWHD_FLAG_TRUBASS_ENABLE;
			} else {
				srs_node->regs->flags &= ~SRS_WOWHD_FLAG_TRUBASS_ENABLE;
			}
			break;
		case 9:
			if (val) {
				srs_node->regs->flags |= SRS_WOWHD_FLAG_CLARITY_ENABLE;
			} else {
				srs_node->regs->flags &= ~SRS_WOWHD_FLAG_CLARITY_ENABLE;
			}
			break;
		case 10:
			if (val) {
				srs_node->regs->flags |= SRS_WOWHD_FLAG_WOWHD_ENABLE;
			} else {
				srs_node->regs->flags &= ~SRS_WOWHD_FLAG_WOWHD_ENABLE;
			}
			break;
		case 11:
			if (val) {
				srs_node->regs->flags |= SRS_WOWHD_FLAG_VOLUMECONTROL_ENABLE;
			} else {
				srs_node->regs->flags &= ~SRS_WOWHD_FLAG_VOLUMECONTROL_ENABLE;
			}
			break;
		case 12:
			if (val) {
				srs_node->regs->flags |= SRS_WOWHD_FLAG_VC_NORMALIZER_ENABLE;
			} else {
				srs_node->regs->flags &= ~SRS_WOWHD_FLAG_VC_NORMALIZER_ENABLE;
			}
			break;
		case 13:
			ubicom32_set_interrupt(srs_node->sendirq);
			break;

		default:
			return -EINVAL;
	}

	return count;
}

/*
 * ubicom32_srs_proc_read
 */
static ssize_t ubicom32_srs_proc_read(struct file *file, char __user *buf,
				      size_t count, loff_t *ppos)
{
	struct proc_dir_entry *pde = PDE(file->f_dentry->d_inode);
	char s[13];
	int len;

	switch((int)pde->data) {
		case 1:
			sprintf(s, "%hu", srs_node->regs->wowhd_3d_mode);
			break;
		case 2:
			sprintf(s, "%hu", srs_node->regs->wowhd_3d_space);
			break;
		case 3:
			sprintf(s, "%hu", srs_node->regs->wowhd_3d_center);
			break;
		case 4:
			sprintf(s, "%hu", srs_node->regs->vc_input_gain);
			break;
		case 5:
			sprintf(s, "%hu", srs_node->regs->vc_output_gain);
			break;
		case 6:
			sprintf(s, "%hu", srs_node->regs->vc_max_gain);
			break;
		case 7:
			sprintf(s, "%hu", (srs_node->regs->flags & SRS_WOWHD_FLAG_DEFINITION_ENABLE) ? 1 : 0);
			break;
		case 8:
			sprintf(s, "%hu", (srs_node->regs->flags & SRS_WOWHD_FLAG_TRUBASS_ENABLE) ? 1 : 0);
			break;
		case 9:
			sprintf(s, "%hu", (srs_node->regs->flags & SRS_WOWHD_FLAG_CLARITY_ENABLE) ? 1 : 0);
			break;
		case 10:
			sprintf(s, "%hu", (srs_node->regs->flags & SRS_WOWHD_FLAG_WOWHD_ENABLE) ? 1 : 0);
			break;
		case 11:
			sprintf(s, "%hu", (srs_node->regs->flags & SRS_WOWHD_FLAG_VOLUMECONTROL_ENABLE) ? 1 : 0);
			break;
		case 12:
			sprintf(s, "%hu", (srs_node->regs->flags & SRS_WOWHD_FLAG_VC_NORMALIZER_ENABLE) ? 1 : 0);
			break;
		default:
			return -EINVAL;
	}

	len = strlen(s);
	if (*ppos < len) {
		len = min_t(size_t, len - *ppos, count);
		if (copy_to_user(buf, s + *ppos, len)) {
			return -EFAULT;
		}
		*ppos += len;
	} else {
		len = 0;
	}

	return len;
}

static const struct file_operations ubicom32_srs_proc_fops = {
	.read = ubicom32_srs_proc_read,
	.write = ubicom32_srs_proc_write,
};

/*
 * ubicom32_srs_probe
 */
static __init int ubicom32_srs_probe(void)
{
	struct proc_dir_entry *proc_root;
	struct proc_dir_entry *proc_entry;

	printk(KERN_INFO "SRS WOWHD: Init proc node\n");

	srs_node = (struct srs_wowhd_node *)devtree_find_node("srswowhd");
	if (!srs_node) {
		printk(KERN_WARNING "SRS WOWHD: VP not found\n");
		return -ENODEV;
	}

	if (srs_node->version != SRS_WOWHD_NODE_VERSION) {
		printk(KERN_WARNING "SRS WOWHD: node not compatible\n");
		return -ENODEV;
	}

	if (srs_node->regs->version != SRS_WOWHD_VP_VERSION) {
		printk(KERN_WARNING "SRS WOWHD: VP not compatible\n");
		return -ENODEV;
	}

	proc_root = proc_mkdir("srswowhd", NULL);
	if (!proc_root) {
		printk(KERN_WARNING "SRS WOWHD: Could not create proc node\n");
		return -ENOMEM;
	}

	proc_entry = create_proc_entry("3d_mode", S_IRUGO | S_IWUGO, proc_root);
	if (proc_entry) {
		proc_entry->data = (void *)1;
		proc_entry->proc_fops = &ubicom32_srs_proc_fops;
	}

	proc_entry = create_proc_entry("3d_space", S_IRUGO | S_IWUGO, proc_root);
	if (proc_entry) {
		proc_entry->data = (void *)2;
		proc_entry->proc_fops = &ubicom32_srs_proc_fops;
	}

	proc_entry = create_proc_entry("3d_center", S_IRUGO | S_IWUGO, proc_root);
	if (proc_entry) {
		proc_entry->data = (void *)3;
		proc_entry->proc_fops = &ubicom32_srs_proc_fops;
	}

	proc_entry = create_proc_entry("vc_input", S_IRUGO | S_IWUGO, proc_root);
	if (proc_entry) {
		proc_entry->data = (void *)4;
		proc_entry->proc_fops = &ubicom32_srs_proc_fops;
	}

	proc_entry = create_proc_entry("vc_output", S_IRUGO | S_IWUGO, proc_root);
	if (proc_entry) {
		proc_entry->data = (void *)5;
		proc_entry->proc_fops = &ubicom32_srs_proc_fops;
	}

	proc_entry = create_proc_entry("vc_max_gain", S_IRUGO | S_IWUGO, proc_root);
	if (proc_entry) {
		proc_entry->data = (void *)6;
		proc_entry->proc_fops = &ubicom32_srs_proc_fops;
	}

	proc_entry = create_proc_entry("definition_enable", S_IRUGO | S_IWUGO, proc_root);
	if (proc_entry) {
		proc_entry->data = (void *)7;
		proc_entry->proc_fops = &ubicom32_srs_proc_fops;
	}

	proc_entry = create_proc_entry("trubass_enable", S_IRUGO | S_IWUGO, proc_root);
	if (proc_entry) {
		proc_entry->data = (void *)8;
		proc_entry->proc_fops = &ubicom32_srs_proc_fops;
	}

	proc_entry = create_proc_entry("clarity_enable", S_IRUGO | S_IWUGO, proc_root);
	if (proc_entry) {
		proc_entry->data = (void *)9;
		proc_entry->proc_fops = &ubicom32_srs_proc_fops;
	}

	proc_entry = create_proc_entry("wowhd_enable", S_IRUGO | S_IWUGO, proc_root);
	if (proc_entry) {
		proc_entry->data = (void *)10;
		proc_entry->proc_fops = &ubicom32_srs_proc_fops;
	}

	proc_entry = create_proc_entry("volumecontrol_enable", S_IRUGO | S_IWUGO, proc_root);
	if (proc_entry) {
		proc_entry->data = (void *)11;
		proc_entry->proc_fops = &ubicom32_srs_proc_fops;
	}

	proc_entry = create_proc_entry("vc_normalizer_enable", S_IRUGO | S_IWUGO, proc_root);
	if (proc_entry) {
		proc_entry->data = (void *)12;
		proc_entry->proc_fops = &ubicom32_srs_proc_fops;
	}

	proc_entry = create_proc_entry("commit", S_IWUGO, proc_root);
	if (proc_entry) {
		proc_entry->data = (void *)13;
		proc_entry->proc_fops = &ubicom32_srs_proc_fops;
	}

	return 0;
}
device_initcall(ubicom32_srs_probe);
#endif
