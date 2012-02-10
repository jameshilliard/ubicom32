/*
 * (C) Copyright 2009-2010
 * Ubicom, Inc. www.ubicom.com
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307	 USA
 *
 */

#include <common.h>
#include <command.h>
#include <image.h>
#include <u-boot/zlib.h>
#include <asm/byteorder.h>
#include <asm/devtree.h>

DECLARE_GLOBAL_DATA_PTR;

extern struct devtree_node *devtree;
extern char init_mtdparts[CONFIG_ARGS_SZ];

/*
 * do_bootm_linux
 */
int do_bootm_linux(int flag, int argc, char *argv[], bootm_headers_t *images)
{
	void	(*theKernel)(void *, void *);

	theKernel = (void (*)(void *, void *))images->ep;

	show_boot_progress (15);

	debug ("## Transferring control to Linux (at address %08lx) ...\n",
	       (ulong) theKernel);

	/* we assume that the kernel is in place */
	printf ("\nStarting kernel ...\n\n");

#ifdef CONFIG_USB_DEVICE
	{
		extern void udc_disconnect (void);
		udc_disconnect ();
	}
#endif
#ifdef CONFIG_CMD_NET
	{
		extern void ubi32_eth_cleanup(void);
		extern int eth_init_done;
		if (eth_init_done)
			ubi32_eth_cleanup();
	}
#endif

	/*
	 * The kernel will pick up boot_command_line from end of memory - 512.
	 */
	char *bootargs = getenv("bootargs");
	char *mtdparts = getenv("mtdparts");
	const char *memory_end = (char *)(CONFIG_RAMBASE + CONFIG_RAMSIZE);
	char *boot_command_line = (char *)(memory_end - CONFIG_ARGS_SZ);
	*boot_command_line = '\0';

	/*
 	 * If mtdparts was changed and we have bootargs set then
	 * set the mtdparts= part of bootargs to the new mtdparts.
	 */
	if (bootargs) {
		if (mtdparts && strcmp(mtdparts, init_mtdparts)) {
			/*
			 * Find the mtdparts= clause(s) in bootargs
			 * and remove them all.
			 */
			char *str = boot_command_line;
			strncpy(str, bootargs,  CONFIG_ARGS_SZ - 1);
			str[CONFIG_ARGS_SZ - 1] = '\0';
			while ((str = strstr(str, "mtdparts="))) {
				char *end = str;
				while ((*end != ' ') && (*end != '\0'))
					end++;
				while (*end == ' ')
					end++;
				strcpy(str, end);
			}
			strncat(boot_command_line, " ", 
				CONFIG_ARGS_SZ - 1 - strlen(boot_command_line));
			strncat(boot_command_line, mtdparts, 
				CONFIG_ARGS_SZ - 1 - strlen(boot_command_line));
		} else {
			strncpy(boot_command_line, bootargs, CONFIG_ARGS_SZ - 1);
			boot_command_line[CONFIG_ARGS_SZ - 1] = '\0';
		}
	} else {
		if (mtdparts) {
			strncpy(boot_command_line, mtdparts, CONFIG_ARGS_SZ - 1);
			boot_command_line[CONFIG_ARGS_SZ - 1] = '\0';
		}
	}

	theKernel (devtree, images->legacy_hdr_os);
	/* does not return */

	return 1;
}

