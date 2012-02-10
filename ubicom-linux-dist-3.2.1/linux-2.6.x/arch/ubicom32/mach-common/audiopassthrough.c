/*
 * arch/ubicom32/mach-common/audiopassthrough.c
 *   audio passthrough configuration proc node
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
#include <asm/ip5000.h>
#include <asm/devtree.h>
#include <asm/audiopassthrough.h>

#if defined(CONFIG_PROC_FS)

#include <linux/proc_fs.h>

static struct audio_passthrough_node	*ap_node = NULL;

/*
 * ubicom32_audiopassthrough_sample_rate_proc_write
 */
static ssize_t ubicom32_audiopassthrough_sample_rate_proc_write( struct file *file, 
		const char __user *buf, size_t count, loff_t *ppos)
{
	struct proc_dir_entry *pde = PDE(file->f_dentry->d_inode);
	struct audio_passthrough_node *apn = (struct audio_passthrough_node *)pde->data;
	struct audio_passthrough_regs *apr = apn->regs;
	char s[13];
	int val;

	if (count > 12) {
		return -EINVAL;
	}

	if (copy_from_user(s, buf, count)) {
		return -EINVAL;
	}

	val = simple_strtoul(s, NULL, 10);

	apr->sample_rate = val;

	return count;
}

/*
 * ubicom32_audiopassthrough_proc_write
 */
static ssize_t ubicom32_audiopassthrough_proc_write(struct file *file, const char __user *buf,
				       size_t count, loff_t *ppos)
{
	struct proc_dir_entry *pde = PDE(file->f_dentry->d_inode);
	struct audio_passthrough_node *apn = (struct audio_passthrough_node *)pde->data;
	struct audio_passthrough_regs *apr = apn->regs;
	char s[13];
	int val;

	if (count > 12) {
		return -EINVAL;
	}

	if (copy_from_user(s, buf, count)) {
		return -EINVAL;
	}

	val = simple_strtoul(s, NULL, 10);

	apr->command = val;
	ubicom32_set_interrupt(apn->dn.sendirq);

	return count;
}

/*
 * ubicom32_audiopassthrough_proc_read
 */
static ssize_t ubicom32_audiopassthrough_proc_read(struct file *file, char __user *buf,
				      size_t count, loff_t *ppos)
{
	struct proc_dir_entry *pde = PDE(file->f_dentry->d_inode);
	struct audio_passthrough_node *apn = (struct audio_passthrough_node *)pde->data;
	struct audio_passthrough_regs *apr = apn->regs;
	char s[13];
	int len;

	sprintf(s, "%d", apr->enabled);
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

static const struct file_operations ubicom32_audiopassthrough_sample_rate_proc_fops = {
	.write = ubicom32_audiopassthrough_sample_rate_proc_write,
};

static const struct file_operations ubicom32_audiopassthrough_command_proc_fops = {
	.write = ubicom32_audiopassthrough_proc_write,
};

static const struct file_operations ubicom32_audiopassthrough_enable_proc_fops = {
	.read = ubicom32_audiopassthrough_proc_read,
};

/*
 * ubicom32_audiopassthrough_probe
 */
static __init int ubicom32_audiopassthrough_probe(void)
{
	struct proc_dir_entry *proc_root;
	struct proc_dir_entry *proc_entry;

	printk(KERN_INFO "AudioPassthrough: Init proc node\n");

	ap_node = (struct audio_passthrough_node *)devtree_find_node("audiopassthrough");
	if (!ap_node) {
		printk(KERN_WARNING "AudioPassthrough: not found\n");
		return -ENODEV;
	}

	if (ap_node->version != AUDIO_PASSTHROUGH_NODE_VERSION) {
		printk(KERN_WARNING "AudioPassthrough: node not compatible\n");
		return -ENODEV;
	}

	if (ap_node->regs->version != AUDIO_PASSTHROUGH_REGS_VERSION) {
		printk(KERN_WARNING "AudioPassthrough: ap not compatible\n");
		return -ENODEV;
	}

	proc_root = proc_mkdir("audiopassthrough", NULL);
	if (!proc_root) {
		printk(KERN_WARNING "AudioPassthrough: Could not create proc node\n");
		return -ENOMEM;
	}

	proc_entry = create_proc_entry("enabled", S_IRUGO, proc_root);
	if (proc_entry) {
		proc_entry->data = (void *)ap_node;
		proc_entry->proc_fops = &ubicom32_audiopassthrough_enable_proc_fops;
	}

	proc_entry = create_proc_entry("sample_rate", S_IWUGO, proc_root);
	if (proc_entry) {
		proc_entry->data = (void *)ap_node;
		proc_entry->proc_fops = &ubicom32_audiopassthrough_sample_rate_proc_fops;
	}

	proc_entry = create_proc_entry("command", S_IWUGO, proc_root);
	if (proc_entry) {
		proc_entry->data = (void *)ap_node;
		proc_entry->proc_fops = &ubicom32_audiopassthrough_command_proc_fops;
	}

	return 0;
}
device_initcall(ubicom32_audiopassthrough_probe);
#endif
