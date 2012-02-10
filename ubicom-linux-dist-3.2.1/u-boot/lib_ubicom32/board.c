/*
 *
 * (C) Copyright 2009
 * Ubicom, Inc. www.ubicom.com
 *
 * (C) Copyright 2003, Psyent Corporation <www.psyent.com>
 * Scott McNutt <smcnutt@psyent.com>
 *
 * (C) Copyright 2000-2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
#include <devices.h>
#include <watchdog.h>
#include <net.h>
#include <spi.h>
#include <malloc.h>
#include <asm/ip5000.h>
#include <asm/devtree.h>
#include <asm/ip5000.h>
#if defined(CONFIG_CMD_NAND_SPI_ER)
#include <nand-spi-er-uboot.h>
#endif
#if defined(CONFIG_CMD_NAND_SPI_ER_UBI32)
#include <asm/storage.h>
#include <linux/mtd/nand-spi-er.h>
#endif
#ifdef CONFIG_EUPGRADE
#include "eupgrade.h"
#endif
#ifdef CONFIG_EUPGRADE_VIDEO
#include "eupgrade_video.h"
#endif
#ifdef CONFIG_EUPGRADE_CHARLCD
#include "eupgrade_charlcd.h"
#endif
#ifdef CONFIG_EUPGRADE_LED
#include "eupgrade_led.h"
#endif

#ifdef CONFIG_STATUS_LED
#include <status_led.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

/*
 * All attempts to come up with a "common" initialization sequence
 * that works for all boards and architectures failed: some of the
 * requirements are just _too_ different. To get rid of the resulting
 * mess of board dependend #ifdef'ed code we now make the whole
 * initialization sequence configurable to the user.
 *
 * The requirements for any new initalization function is simple: it
 * receives a pointer to the "global data" structure as it's only
 * argument, and returns an integer return code, where 0 means
 * "continue" and != 0 means "fatal error, hang the system".
 */

const char version_string[] =
	" (" __DATE__ " - " __TIME__ ")";

extern void malloc_bin_reloc (void);

struct boardnode *bn;
struct bootargsnode *ban;
char init_mtdparts[CONFIG_ARGS_SZ];

/*
 * Begin and End of memory area for malloc(), and current "brk"
 */
static	ulong	mem_malloc_start = 0;
static	ulong	mem_malloc_end	 = 0;
static	ulong	mem_malloc_brk	 = 0;

/*
 * The Malloc area is immediately below the monitor copy in RAM
 */
extern char _end[];
static void mem_malloc_init (void)
{
	mem_malloc_start = (ulong)_end;

	/*
	 * We reserve 4K for stack.
	 */
	mem_malloc_end = CONFIG_RAMBASE + CONFIG_RAMSIZE - 0x1000;
	mem_malloc_brk = mem_malloc_start;
	memset ((void *) mem_malloc_start,
		0,
		mem_malloc_end - mem_malloc_start);
}

void *sbrk (ptrdiff_t increment)
{
	ulong old = mem_malloc_brk;
	ulong new = old + increment;

	if ((new < mem_malloc_start) || (new > mem_malloc_end)) {
		return (NULL);
	}
	mem_malloc_brk = new;
	return ((void *) old);
}

/*
 * bootargs_get_cmdline()
 */
const char *bootargs_get_cmdline(void)
{
	if (!ban) {
		return "";
	}

	return ban->cmdline;
}

/*
 * board_get_revision()
 *	Returns revision string of the board.
 */
const char *board_get_revision(void)
{
	if (!bn) {
		return "NULL";
	}

	return bn->revision;
}

static int display_banner(void)
{
	printf ("\n\n%s\n\n", version_string);
	return (0);
}

typedef int (init_fnc_t)(void);

init_fnc_t *init_sequence[] = {
	console_init_f,
	display_banner,
	devices_init,
	console_init_r,
#ifdef CONFIG_EUPGRADE_LED
	eupgrade_led_init,
#endif
#ifdef CONFIG_EUPGRADE_CHARLCD
	eupgrade_charlcd_init,
#endif
#ifdef CONFIG_EUPGRADE_VIDEO
	eupgrade_video_init,
#endif
#ifdef CONFIG_EUPGRADE
	/*
	 * This should be the last init since it will fall through
	 * if the user does not want to do upgrade.
	 */
	eupgrade_init,
#endif
	NULL,
};

/*
 * board_init
 */
void board_init (void)
{
	init_fnc_t **init_fnc_ptr;
	bd_t *bd;

	bn = (struct boardnode *)devtree_find_node("board");
	if (!bn) {
		puts("board node not found\n");
	}
	if (bn->version != BOARDNODE_VERSION) {
		puts("board node is wrong revision\n");
		bn = NULL;
	}

	ban = (struct bootargsnode *)devtree_find_node("bootargs");
	if (!ban) {
		puts("bootargs node not found\n");
	}

	mem_malloc_init();
	gd = malloc(sizeof(gd_t)+sizeof(bd_t));
	gd->bd = (bd_t *)(gd+1);	/* At end of global data */
	bd = gd->bd;
	
	serial_init();
	puts("u-boot started\n");	

	gd->flags = gd->flags | GD_FLG_RELOC;


	gd->baudrate = CONFIG_BAUDRATE;
	gd->cpu_clk = CONFIG_SYS_CLK_FREQ;

	bd = gd->bd;
	bd->bi_memstart	= CONFIG_RAMBASE;
	bd->bi_memsize = CONFIG_RAMSIZE;
	bd->bi_flashstart = CONFIG_SYS_FLASH_BASE;
	bd->bi_flashsize = CONFIG_SYS_FLASH_SIZE;	

#if	defined(CONFIG_SYS_SRAM_BASE) && defined(CONFIG_SYS_SRAM_SIZE)
	bd->bi_sramstart= CONFIG_SYS_SRAM_BASE;
	bd->bi_sramsize	= CONFIG_SYS_SRAM_SIZE;
#endif
	bd->bi_baudrate	= CONFIG_BAUDRATE;

	WATCHDOG_RESET ();

	timer_init();
	flash_init();
#if defined(CONFIG_CMD_NAND_SPI_ER_UBI32)
	struct storage_node *sn = (struct storage_node *)devtree_find_node("storage");
	if (sn) {
		if (sn->version != STORAGE_NODE_VERSION) {
			printf("Storage node version mismatch!\n");
		} else {
			/*
			 * the switch port can't be PA0 so we can assume 0/0 is not set
			 */
			nand_spi_er_switch_port = sn->spi_er_nand_switch_port;
			nand_spi_er_switch_pin = sn->spi_er_nand_switch_pin;
		}
	}
#endif
#if defined(CONFIG_CMD_NAND_SPI_ER) || defined(CONFIG_CMD_NAND_SPI_ER_UBI32)
	nand_spi_er_init();
#endif
	env_init();
	env_relocate();

	char *s, *e;
	int i;

	const char *cmdline = bootargs_get_cmdline();
	char *bootargs = getenv ("bootargs");
	if (bootargs == NULL) {
		if (cmdline[0] != '\0') {
			setenv ("bootargs", (char *)cmdline);
			bootargs = getenv ("bootargs");
		}
	}

	char *mtdparts = getenv("mtdparts");
	if (mtdparts == NULL) {
		if (cmdline[0] != '\0') {
			char tmp[CONFIG_ARGS_SZ];
			char *str = strstr(cmdline, "mtdparts=");
			char *end = str;
			while ((*end != ' ') && (*end != '\0'))
				end++;
			strncpy(tmp, str, end - str);
			tmp[end - str] = '\0';
			setenv ("mtdparts", tmp);
			mtdparts = getenv("mtdparts");
		}
	}
	if (mtdparts)
		strcpy(init_mtdparts, mtdparts);

#ifdef CONFIG_CMD_NET
	bd->bi_ip_addr = getenv_IPaddr ("ipaddr");
	printf("ipaddr = %lu.%lu.%lu.%lu\n", bd->bi_ip_addr >> 24, 
		(bd->bi_ip_addr >> 16) & 0xFF,
		(bd->bi_ip_addr >> 8) & 0xFF,
		(bd->bi_ip_addr & 0xFF));

	s = getenv ("ethaddr");
	for (i = 0; i < 6; ++i) {
		bd->bi_enetaddr[i] = s ? simple_strtoul (s, &e, 16) : 0;
		if (s) s = (*e) ? e + 1 : e;
	}

	if ((s = getenv("bootfile")) != NULL)
		copy_filename(BootFile, s, sizeof(BootFile));
#endif
	if ((s = getenv("loadaddr")) != NULL)
		load_addr = simple_strtoul(s, NULL, 16);

#ifdef CONFIG_SOFT_SPI
	board_spi_init();
#endif
								
#ifdef CONFIG_UBICOM_SWITCH_AR8316
int mdio_reset(void);
int mdio_init(void);
void athrs16_reg_init(void);
        mdio_reset();
        mdio_init();
        athrs16_reg_init();
#endif
	WATCHDOG_RESET ();
	jumptable_init();
	interrupt_init ();

	for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
		WATCHDOG_RESET ();
		if ((*init_fnc_ptr) () != 0) {
			hang ();
		}
	}
	
	/* main_loop */
	for (;;) {
		WATCHDOG_RESET ();
		main_loop ();
	}
}


/***********************************************************************/

void hang (void)
{
	puts("### ERROR ### Please reset board ###\n");
	for (;;);
}

unsigned long do_go_exec (ulong (*entry)(int, char *[]), int argc, char *argv[])
{
	/*
	 * x86 does not use a dedicated register to pass the pointer
	 * to the global_data
	 */
	argv[-1] = (char *)gd;
	return entry (argc, argv);
}

