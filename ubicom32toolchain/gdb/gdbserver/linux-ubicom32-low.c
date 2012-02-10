/* GNU/Linux/ubicom32 specific low level interface, for the remote server for GDB.
   Copyright (C) 1995, 1996, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005,
   2007 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "server.h"
#include "linux-low.h"

#ifdef HAVE_SYS_REG_H
#include <sys/reg.h>
#endif

#include <asm/ptrace.h>

/* This table maps gdb register file with the one provided through
 * ptrace.  It must line up with ubicom32_posix_regs[] in
 * gdb/ubicom32-tdep.c
 */
static int ubicom32_regmap[] =
  {
    PT_D0,  PT_D1,  PT_D2,  PT_D3,  	/* 0-3 */
    PT_D4,  PT_D5,  PT_D6,  PT_D7,     	/* 4-7 */
    PT_D8,  PT_D9,  PT_D10, PT_D11,	/* 8-11 */
    PT_D12, PT_D13, PT_D14, PT_D15, 	/* 12-15 */
    PT_A0,  PT_A1,  PT_A2,  PT_A3, 	/* 16-19 */
    PT_A4,  PT_A5,  PT_A6,  PT_SP, 	/* 20-23 */
    PT_ACC0HI, PT_ACC0LO,  		/* 24, 25 */
    PT_MAC_RC16,  			/* 26 */
    PT_ACC1HI, PT_ACC1LO,  		/* 27, 28 */
    PT_SOURCE3, 	 		/* 29 */
    PT_INST_CNT, 	 		/* 30 */
    PT_CSR, 	 			/* 31 */
    PT_DUMMY_UNUSED,  			/* 32 */
    PT_INT_MASK0,  			/* 33 */
    PT_INT_MASK1,  			/* 34 */
    PT_TRAP_CAUSE,  			/* 35 */
    PT_PC,	  			/* 36 */
    PT_TEXT_ADDR, PT_TEXT_END_ADDR, PT_DATA_ADDR, /* 37, 38, 39 */
    PT_EXEC_FDPIC_LOADMAP, PT_INTERP_FDPIC_LOADMAP,/* 40, 41 */
    PT_PREVIOUS_PC,				   /* 42 */
  };

#define UBICOM32_NUM_REGS (sizeof( ubicom32_regmap) / sizeof( ubicom32_regmap[0]))

static int
ubicom32_cannot_store_register (int regno)
{
  if (regno == 42) {
    return 0;
  }

  if (regno >= 29) {
    if (regno > 36) {
      return 1;
    }
    if (regno == 29 || /* Source 3 */
	regno == 32 || /* Dummy Unsed */
	regno == 30 /* Inst Cnt */
	) {
      return 1;
    }
  }
  return 0;
}

static int
ubicom32_cannot_fetch_register (int regno)
{
  return (regno >= UBICOM32_NUM_REGS);
}

struct regset_info target_regsets[] = {
  { 0, 0, -1, -1, NULL, NULL }
};

static const unsigned char ubicom32_breakpoint[] = { 0xFA, 0xBB, 0xCC, 0xDD };
#define UBICOM32_BREAKPOINT_LEN 4

static CORE_ADDR
ubicom32_get_pc ()
{
  unsigned long pc;

  collect_register_by_name ("pc", &pc);
  return pc;
}

static void
ubicom32_set_pc (CORE_ADDR value)
{
  unsigned long newpc = value;

  supply_register_by_name ("pc", &newpc);
}

static int
ubicom32_breakpoint_at (CORE_ADDR pc)
{
  unsigned char c[UBICOM32_BREAKPOINT_LEN];
  read_inferior_memory (pc, c, UBICOM32_BREAKPOINT_LEN);
  

  if (memcmp(c, ubicom32_breakpoint, UBICOM32_BREAKPOINT_LEN) == 0) 
    return 1;

  return 0;
}

/* We only place breakpoints in empty marker functions, and thread locking
   is outside of the function.  So rather than importing software single-step,
   we can just run until exit. */

static CORE_ADDR
ubicom32_reinsert_addr ()
{
  static unsigned long last = 1; /* Could never be valid as it is odd */
  unsigned long rp;
  unsigned long pc;
  collect_register_by_name ("a5", &rp);
  collect_register_by_name ("pc", &pc);

  /*
   * Most other ISA's use the reverse pointer or link register here and
   * because that doesn't change across a return this is ok.  But the
   * ubicom32 compilers normally use calli over an actuall ret and this
   * will normally change a5 so we need to temporarily save it away in 'last' on
   * the first call.
   */
  if (last == pc) {
    last = 1;
    return pc;
  }

  last = rp;
  return last;
}

struct linux_target_ops the_low_target = {
  UBICOM32_NUM_REGS,
  ubicom32_regmap,
  ubicom32_cannot_fetch_register,
  ubicom32_cannot_store_register,
  ubicom32_get_pc,
  ubicom32_set_pc,
  ubicom32_breakpoint,
  UBICOM32_BREAKPOINT_LEN,
  ubicom32_reinsert_addr, /* breakpoint_reinsert_addr*/
  0, /* decr_pc_after_break*/
  ubicom32_breakpoint_at,
  NULL, /* insert_watchpoint */
  NULL, /* remove_watchpoint */
  NULL, /* stopped_by_watchpoint */
  NULL, /* stopped_data_address */
  0, /* left_pad_xfer */
  "ubicom32uclinux"
};
