/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2005 Rowel Atienza <rowel@diwalabs.com>
 * Armadillo board HT1070
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>

extern void devtree_print(void);

int do_devtree_print(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	devtree_print();
	return 1;
}

U_BOOT_CMD (devtree, 1, 0, do_devtree_print,
	    "print ubicom device tree contents", NULL);


void show_boot_progress(int progress)
{
    printf("Boot reached stage %d\n", progress);
}
