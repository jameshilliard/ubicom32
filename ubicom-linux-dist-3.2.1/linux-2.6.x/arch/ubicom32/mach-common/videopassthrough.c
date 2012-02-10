/*
 * arch/ubicom32/mach-common/videopassthrough.c
 *   VIdeo passthrough configuration proc node
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
#include <asm/videopassthrough.h>

#if defined(CONFIG_PROC_FS)

#include <linux/proc_fs.h>

static struct video_passthrough_node	*vp_node = NULL;

/*
 * ubicom32_videopassthrough_proc_write
 */
static ssize_t ubicom32_videopassthrough_proc_write(struct file *file, const char __user *buf,
				       size_t count, loff_t *ppos)
{
	struct proc_dir_entry *pde = PDE(file->f_dentry->d_inode);
	struct video_passthrough_node *vpn = (struct video_passthrough_node *)pde->data;
	struct video_passthrough_regs *vpr = vpn->regs;
	char s[13];
	int val;

	if (count > 12) {
		return -EINVAL;
	}

	if (copy_from_user(s, buf, count)) {
		return -EINVAL;
	}

	val = simple_strtoul(s, NULL, 10);

	vpr->command = val;
	ubicom32_set_interrupt(vpn->dn.sendirq);

	return count;
}

/*
 * ubicom32_videopassthrough_proc_read
 */
static ssize_t ubicom32_videopassthrough_proc_read(struct file *file, char __user *buf,
				      size_t count, loff_t *ppos)
{
	struct proc_dir_entry *pde = PDE(file->f_dentry->d_inode);
	struct video_passthrough_node *vpn = (struct video_passthrough_node *)pde->data;
	struct video_passthrough_regs *vpr = vpn->regs;
	char s[13];
	int len;

	sprintf(s, "%d", vpr->enabled);
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

static const struct file_operations ubicom32_videopassthrough_command_proc_fops = {
	.write = ubicom32_videopassthrough_proc_write,
};

static const struct file_operations ubicom32_videopassthrough_enable_proc_fops = {
	.read = ubicom32_videopassthrough_proc_read,
};

/*
 * ubicom32_videopassthrough_probe
 */
static __init int ubicom32_videopassthrough_probe(void)
{
	struct proc_dir_entry *proc_root;
	struct proc_dir_entry *proc_entry;
	void *buffer;
	u32_t align;

	printk(KERN_INFO "VideoPassthrough: Init\n");

	vp_node = (struct video_passthrough_node *)devtree_find_node("videopassthrough");
	if (!vp_node) {
		printk(KERN_WARNING "VideoPassthrough: VP not found\n");
		return -ENODEV;
	}

	if (vp_node->version != VIDEO_PASSTHROUGH_NODE_VERSION) {
		printk(KERN_WARNING "VideoPassthrough: node not compatible\n");
		return -ENODEV;
	}

	if (vp_node->regs->version != VIDEO_PASSTHROUGH_REGS_VERSION) {
		printk(KERN_WARNING "VideoPassthrough: VP not compatible\n");
		return -ENODEV;
	}

	buffer = kmalloc(vp_node->regs->minimum_buffer_size + vp_node->regs->buffer_alignment, GFP_KERNEL);
	if (!buffer) {
		printk(KERN_WARNING "VideoPassthrough: Could not allocate buffer\n");
		return -ENOMEM;
	}
	align = vp_node->regs->buffer_alignment - 1;
	vp_node->regs->buffer = (void *)(((u32_t)buffer + align) & ~align);

	proc_root = proc_mkdir("videopassthrough", NULL);
	if (!proc_root) {
		printk(KERN_WARNING "VideoPassthrough: Could not create proc node\n");
		return -ENOMEM;
	}

	proc_entry = create_proc_entry("enabled", S_IRUGO, proc_root);
	if (proc_entry) {
		proc_entry->data = (void *)vp_node;
		proc_entry->proc_fops = &ubicom32_videopassthrough_enable_proc_fops;
	}

	proc_entry = create_proc_entry("command", S_IWUGO, proc_root);
	if (proc_entry) {
		proc_entry->data = (void *)vp_node;
		proc_entry->proc_fops = &ubicom32_videopassthrough_command_proc_fops;
	}

	printk(KERN_INFO "VideoPassthrough: Allocated, buffer: %p\n", vp_node->regs->buffer);

	return 0;
}
device_initcall(ubicom32_videopassthrough_probe);
#endif
