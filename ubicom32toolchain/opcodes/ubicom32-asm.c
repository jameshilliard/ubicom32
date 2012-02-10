/* Assembler interface for targets using CGEN. -*- C -*-
   CGEN: Cpu tools GENerator

   THIS FILE IS MACHINE GENERATED WITH CGEN.
   - the resultant file is machine generated, cgen-asm.in isn't

   Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2005, 2007
   Free Software Foundation, Inc.

   This file is part of libopcodes.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */


/* ??? Eventually more and more of this stuff can go to cpu-independent files.
   Keep that in mind.  */

#include "sysdep.h"
#include <stdio.h>
#include "ansidecl.h"
#include "bfd.h"
#include "symcat.h"
#include "ubicom32-desc.h"
#include "ubicom32-opc.h"
#include "opintl.h"
#include "xregex.h"
#include "libiberty.h"
#include "safe-ctype.h"

#undef  min
#define min(a,b) ((a) < (b) ? (a) : (b))
#undef  max
#define max(a,b) ((a) > (b) ? (a) : (b))

static const char * parse_insn_normal
  (CGEN_CPU_DESC, const CGEN_INSN *, const char **, CGEN_FIELDS *);

/* -- assembler routines inserted here.  */

/* -- asm.c */

/* Directly addressable registers on the UBICOM32.
 */

#define RW  0  /* read/write */
#define RO  1  /* read-only  */
#define WO  2  /* write-only */

struct ubicom32_cgen_data_space_map ubicom32_cgen_data_space_map_mercury[] = {
	{ 0x0,		"d0", RW, },	/* data registers */
	/* d1, d2 and d3 are later */
	{ 0x10,		"d4", RW, },
	{ 0x14,		"d5", RW, },
	{ 0x18,		"d6", RW, },
	{ 0x1c,		"d7", RW, },
	{ 0x20,		"d8", RW, },
	{ 0x24,		"d9", RW, },
	{ 0x28,		"d10", RW, },
	{ 0x2c,		"d11", RW, },
	{ 0x30,		"d12", RW, },
	{ 0x34,		"d13", RW, },
	{ 0x38,		"d14", RW, },
	{ 0x3c,		"d15", RW, },
	{ 0x4,		"d1", RW, },	/* put them here where they work */
	{ 0x8,		"d2", RW, },
	{ 0xc,		"d3", RW, },
	{ A0_ADDRESS,	"a0", RW, },	/* address registers */
	{ A1_ADDRESS,	"a1", RW, },
	{ A2_ADDRESS,   "a2", RW, },
	{ A3_ADDRESS,	"a3", RW, },
	{ A4_ADDRESS,	"a4", RW, },
	{ A5_ADDRESS,	"a5", RW, },
	{ A6_ADDRESS,	"a6", RW, },
	{ A7_ADDRESS,	"sp", RW, },	/* sp is a7; first so we use it */
	{ A7_ADDRESS,	"a7", RW, },
	{ 0xa0,		"mac_hi", RW, },
	{ 0xa4,		"mac_lo", RW, },
	{ 0xa8,         "mac_rc16", RW, },
	{ 0xac,         "source3", RW, },
	{ 0xac,         "source_3", RW, },
	{ 0xb0,         "context_cnt", RO,},
	{ 0xb0,         "inst_cnt", RO,},
	{ 0xb4,		"csr", RW, },
	{ 0xb8,         "rosr", RO, },
	{ 0xbc,		"iread_data", RW, },
	{ 0xc0,		"int_mask0", RW, },
	{ 0xc4,		"int_mask1", RW, },
	/* 0xc8 - 0xcf reserved for future interrupt masks */
	{ 0xd0,         "pc", RW, },
	/* 0xd4 - ff reserved */
	{ 0x100,        "chip_id", RO, },
	{ 0x104,	"int_stat0", RO, },
	{ 0x108,	"int_stat1", RO, },
	/* 0x10c - 0x113 reserved for future interrupt masks */
	{ 0x114,        "int_set0", WO, },
	{ 0x118,        "int_set1", WO, },
        /* 0x11c - 0x123 reserved for future interrupt set */
	{ 0x124,        "int_clr0", WO, },
        { 0x128,        "int_clr1", WO, },
        /* 0x13c - 0x133 reserved for future interrupt clear */
	{ 0x134,	"global_ctrl", RW, },
	{ 0x13c,        "mt_active_set", WO, },
	{ 0x140,        "mt_active_clr", WO, },
	{ 0x138,        "mt_active", RO, },
	{ 0x148,        "mt_dbg_active_set", WO, },
	{ 0x144,        "mt_dbg_active", RO, },
	{ 0x14C,        "mt_en", RW, },
	{ 0x150,        "mt_hpri", RW, }, 
	{ 0x150,        "mt_pri", RW, }, 
	{ 0x154,        "mt_hrt", RW, },
	{ 0x154,        "mt_sched", RW, },
	{ 0x15C,        "mt_break_clr", WO, },
	{ 0x158,        "mt_break", RO, },
	{ 0x160,        "mt_single_step", RW, },
	{ 0x164,        "mt_min_delay_en", RW, },
	{ 0x164,        "mt_min_del_en", RW, },

	{ 0x16c,        "perr_addr", RO, },
	{ 0x178,        "dcapt_tnum", RO, },
	{ 0x174,        "dcapt_pc", RO, },
	{ 0x170,        "dcapt", RW, },
	/* 0x17c - 0x1ff reserved */
	{ 0x17c,        "mt_dbg_active_clr", WO, },
	{ 0x180,        "scratchpad0", RW, },
	{ 0x184,        "scratchpad1", RW, },
	{ 0x188,        "scratchpad2", RW, },
	{ 0x18c,        "scratchpad3", RW, },

	{ 0x0,		0, RW, },
};

struct ubicom32_cgen_data_space_map ubicom32_cgen_data_space_map_mars[] = {
	{ 0x0,		"d0", RW, },	/* data registers */
	/* d1, d2 and d3 are later */
	{ 0x10,		"d4", RW, },
	{ 0x14,		"d5", RW, },
	{ 0x18,		"d6", RW, },
	{ 0x1c,		"d7", RW, },
	{ 0x20,		"d8", RW, },
	{ 0x24,		"d9", RW, },
	{ 0x28,		"d10", RW, },
	{ 0x2c,		"d11", RW, },
	{ 0x30,		"d12", RW, },
	{ 0x34,		"d13", RW, },
	{ 0x38,		"d14", RW, },
	{ 0x3c,		"d15", RW, },
	{ 0x4,		"d1", RW, },	/* put them here where they work */
	{ 0x8,		"d2", RW, },
	{ 0xc,		"d3", RW, },
	{ A0_ADDRESS,	"a0", RW, },	/* address registers */
	{ A1_ADDRESS,	"a1", RW, },
	{ A2_ADDRESS,   "a2", RW, },
	{ A3_ADDRESS,	"a3", RW, },
	{ A4_ADDRESS,	"a4", RW, },
	{ A5_ADDRESS,	"a5", RW, },
	{ A6_ADDRESS,	"a6", RW, },
	{ A7_ADDRESS,	"sp", RW, },	/* sp is a7; first so we use it */
	{ A7_ADDRESS,	"a7", RW, },
	{ 0xa0,		"mac_hi", RW, },
	{ 0xa0,		"acc0_hi", RW, }, /* mac_hi and mac_lo are also known as acc0_hi and acc0_lo */
	{ 0xa4,		"mac_lo", RW, },
	{ 0xa4,		"acc0_lo", RW, },
	{ 0xa8,         "mac_rc16", RW, },
	{ 0xac,         "source3", RW, },
	{ 0xac,         "source_3", RW, },
	{ 0xb0,         "context_cnt", RO,},
	{ 0xb0,         "inst_cnt", RO,},
	{ 0xb4,		"csr", RW, },
	{ 0xb8,         "rosr", RO, },
	{ 0xbc,		"iread_data", RW, },
	{ 0xc0,		"int_mask0", RW, },
	{ 0xc4,		"int_mask1", RW, },
	/* 0xc8 - 0xcf reserved for future interrupt masks */
	{ 0xd0,         "pc", RW, },
	{ 0xd4,         "trap_cause", RW, },
	{ 0xd8,		"acc1_hi", RW, }, /* Defines for acc1 */
	{ 0xdc,		"acc1_lo", RW, },
	{ 0xe0,		"previous_pc", RO, },
	{ 0xe4,		"ucsr", RW, },

	/* 0xe8 - ff reserved */
	{ 0x100,        "chip_id", RO, },
	{ 0x104,	"int_stat0", RO, },
	{ 0x108,	"int_stat1", RO, },
	/* 0x10c - 0x113 reserved for future interrupt masks */
	{ 0x114,        "int_set0", WO, },
	{ 0x118,        "int_set1", WO, },
        /* 0x11c - 0x123 reserved for future interrupt set */
	{ 0x124,        "int_clr0", WO, },
        { 0x128,        "int_clr1", WO, },
        /* 0x130 - 0x133 reserved for future interrupt clear */
	{ 0x134,	"global_ctrl", RW, },
	{ 0x13c,        "mt_active_set", WO, },
	{ 0x140,        "mt_active_clr", WO, },
	{ 0x138,        "mt_active", RO, },
	{ 0x148,        "mt_dbg_active_set", WO, },
	{ 0x144,        "mt_dbg_active", RO, },
	{ 0x14C,        "mt_en", RW, },
	{ 0x150,        "mt_hpri", RW, }, 
	{ 0x150,        "mt_pri", RW, }, 
	{ 0x154,        "mt_hrt", RW, },
	{ 0x154,        "mt_sched", RW, },
	{ 0x15C,        "mt_break_clr", WO, },
	{ 0x158,        "mt_break", RO, },
	{ 0x160,        "mt_single_step", RW, },
	{ 0x164,        "mt_min_delay_en", RW, },
	{ 0x164,        "mt_min_del_en", RW, },
	{ 0x168,        "mt_break_set", WO, },
	/* 0x16c - 0x16f reserved */
	{ 0x170,        "dcapt", RW, },
	/* 0x174 - 0x17b reserved */
	{ 0x17c,        "mt_dbg_active_clr", WO, },
	{ 0x180,        "scratchpad0", RW, },
	{ 0x184,        "scratchpad1", RW, },
	{ 0x188,        "scratchpad2", RW, },
	{ 0x18c,        "scratchpad3", RW, },
	{ 0x190,        "scratchpad4", RW, },
	{ 0x194,        "scratchpad5", RW, },
	{ 0x198,        "scratchpad6", RW, },

	/* 0x19c - 0x19f Reserved */
	{ 0x1a0,        "chip_cfg", RW, },
	{ 0x1a4,        "mt_i_blocked", RO, },
	{ 0x1a8,	"mt_d_blocked", RO, },
	{ 0x1ac,	"mt_i_blocked_set", WO},
	{ 0x1b0, 	"mt_d_blocked_set", WO},
	{ 0x1b4,	"mt_blocked_clr", WO},
	{ 0x1b8,        "mt_trap_en", RW, },
	{ 0x1bc,        "mt_trap", RO, },
	{ 0x1c0,        "mt_trap_set", WO, },
	{ 0x1c4,        "mt_trap_clr", WO, },
	{ 0x1c8,        "sep", RW, },
	/* 0x1cc-0x1FF Reserved */
 	{ 0x200,	"i_range0_hi", RW},
	{ 0x204,	"i_range1_hi", RW},
	{ 0x208,	"i_range2_hi", RW},
	{ 0x20c,	"i_range3_hi", RW},

	/* 0x210-0x21f Reserved */
 	{ 0x220,	"i_range0_lo", RW},
	{ 0x224,	"i_range1_lo", RW},
	{ 0x228,	"i_range2_lo", RW},
	{ 0x22c,	"i_range3_lo", RW},

	/* 0x230-0x23f Reserved */
 	{ 0x240,	"i_range0_en", RW},
	{ 0x244,	"i_range1_en", RW},
	{ 0x248,	"i_range2_en", RW},
	{ 0x24c,	"i_range3_en", RW},

	/* 0x250-0x25f Reserved */
 	{ 0x260,	"d_range0_hi", RW},
	{ 0x264,	"d_range1_hi", RW},
	{ 0x268,	"d_range2_hi", RW},
	{ 0x26c,	"d_range3_hi", RW},
	{ 0x270,	"d_range4_hi", RW},

	/* 0x274-0x27f Reserved */
 	{ 0x280,	"d_range0_lo", RW},
	{ 0x284,	"d_range1_lo", RW},
	{ 0x288,	"d_range2_lo", RW},
	{ 0x28c,	"d_range3_lo", RW},
	{ 0x290,	"d_range4_lo", RW},

	/* 0x294-0x29f Reserved */
 	{ 0x2a0,	"d_range0_en", RW},
	{ 0x2a4,	"d_range1_en", RW},
	{ 0x2a8,	"d_range2_en", RW},
	{ 0x2ac,	"d_range3_en", RW},
	{ 0x2b0,	"d_range4_en", RW},

	/* 0x2b4-0x3ff Reserved */

	{ 0x0,		0, RW, },
};

struct ubicom32_cgen_data_space_map ubicom32_cgen_data_space_map_jupiter[] = {
	{ 0x0,		"d0", RW, },	/* data registers */
	/* d1, d2 and d3 are later */
	{ 0x10,		"d4", RW, },
	{ 0x14,		"d5", RW, },
	{ 0x18,		"d6", RW, },
	{ 0x1c,		"d7", RW, },
	{ 0x20,		"d8", RW, },
	{ 0x24,		"d9", RW, },
	{ 0x28,		"d10", RW, },
	{ 0x2c,		"d11", RW, },
	{ 0x30,		"d12", RW, },
	{ 0x34,		"d13", RW, },
	{ 0x38,		"d14", RW, },
	{ 0x3c,		"d15", RW, },
	{ 0x4,		"d1", RW, },	/* put them here where they work */
	{ 0x8,		"d2", RW, },
	{ 0xc,		"d3", RW, },
	{ A0_ADDRESS,	"a0", RW, },	/* address registers */
	{ A1_ADDRESS,	"a1", RW, },
	{ A2_ADDRESS,   "a2", RW, },
	{ A3_ADDRESS,	"a3", RW, },
	{ A4_ADDRESS,	"a4", RW, },
	{ A5_ADDRESS,	"a5", RW, },
	{ A6_ADDRESS,	"a6", RW, },
	{ A7_ADDRESS,	"sp", RW, },	/* sp is a7; first so we use it */
	{ A7_ADDRESS,	"a7", RW, },
	{ 0xa0,		"mac_hi", RW, },
	{ 0xa0,		"acc0_hi", RW, }, /* mac_hi and mac_lo are also known as acc0_hi and acc0_lo */
	{ 0xa4,		"mac_lo", RW, },
	{ 0xa4,		"acc0_lo", RW, },
	{ 0xa8,         "mac_rc16", RW, },
	{ 0xac,         "source3", RW, },
	{ 0xac,         "source_3", RW, },
	{ 0xb0,         "context_cnt", RO,},
	{ 0xb0,         "inst_cnt", RO,},
	{ 0xb4,		"csr", RW, },
	{ 0xb8,         "rosr", RO, },
	{ 0xbc,		"iread_data", RW, },
	{ 0xc0,		"int_mask0", RW, },
	{ 0xc4,		"int_mask1", RW, },
	{ 0xc8,		"int_mask2", RW, },
	/* 0xcc - 0xcf reserved for future interrupt masks */
	{ 0xd0,         "pc", RW, },
	{ 0xd4,         "trap_cause", RW, },
	{ 0xd8,		"acc1_hi", RW, }, /* Defines for acc1 */
	{ 0xdc,		"acc1_lo", RW, },
	{ 0xe0,		"previous_pc", RO, },
	{ 0xe4,		"ucsr", RW, },

	/* 0xe8 - ff reserved */
	{ 0x100,        "chip_id", RO, },
	{ 0x104,	"int_stat0", RO, },
	{ 0x108,	"int_stat1", RO, },
	{ 0x10c,	"int_stat2", RO, },
	/* 0x110 - 0x113 reserved for future interrupt masks */
	{ 0x114,        "int_set0", WO, },
	{ 0x118,        "int_set1", WO, },
	{ 0x11c,        "int_set2", WO, },
        /* 0x120 - 0x123 reserved for future interrupt set */
	{ 0x124,        "int_clr0", WO, },
        { 0x128,        "int_clr1", WO, },
        { 0x12c,        "int_clr2", WO, },
        /* 0x130 - 0x133 reserved for future interrupt clear */
	{ 0x134,	"global_ctrl", RW, },
	{ 0x13c,        "mt_active_set", WO, },
	{ 0x140,        "mt_active_clr", WO, },
	{ 0x138,        "mt_active", RO, },
	{ 0x148,        "mt_dbg_active_set", WO, },
	{ 0x144,        "mt_dbg_active", RO, },
	{ 0x14C,        "mt_en", RW, },
	{ 0x150,        "mt_hpri", RW, }, 
	{ 0x150,        "mt_pri", RW, }, 
	{ 0x154,        "mt_hrt", RW, },
	{ 0x154,        "mt_sched", RW, },
	{ 0x15C,        "mt_break_clr", WO, },
	{ 0x158,        "mt_break", RO, },
	{ 0x160,        "mt_single_step", RW, },
	{ 0x164,        "mt_min_delay_en", RW, },
	{ 0x164,        "mt_min_del_en", RW, },
	{ 0x168,        "mt_break_set", WO, },
	{ 0x16c,        "mt_fp_flush", WO, },
	{ 0x170,        "dcapt", RW, },
	{ 0x174,	"mt_fp_blocked", RO, },
	{ 0x178,	"mt_fp_blocked_set", WO},
	{ 0x17c,        "mt_dbg_active_clr", WO, },
	{ 0x180,        "scratchpad0", RW, },
	{ 0x184,        "scratchpad1", RW, },
	{ 0x188,        "scratchpad2", RW, },
	{ 0x18c,        "scratchpad3", RW, },
	{ 0x190,        "scratchpad4", RW, },
	{ 0x194,        "scratchpad5", RW, },
	{ 0x198,        "scratchpad6", RW, },

	/* 0x19c - 0x19f Reserved */
	{ 0x1a0,        "chip_cfg", RW, },
	{ 0x1a4,        "mt_i_blocked", RO, },
	{ 0x1a8,	"mt_d_blocked", RO, },
	{ 0x1ac,	"mt_i_blocked_set", WO},
	{ 0x1b0, 	"mt_d_blocked_set", WO},
	{ 0x1b4,	"mt_blocked_clr", WO},
	{ 0x1b8,        "mt_trap_en", RW, },
	{ 0x1bc,        "mt_trap", RO, },
	{ 0x1c0,        "mt_trap_set", WO, },
	{ 0x1c4,        "mt_trap_clr", WO, },
	{ 0x1c8,        "sep", RW, },
	{ 0x1cc,        "mt_btb_en", RW, },
	{ 0x1d0,        "btb_ctrl", RW, },
	{ 0x1d4,        "tnum", RO, },

	/* 0x1d8-0x1FF Reserved */
 	{ 0x200,	"i_range0_hi", RW},
	{ 0x204,	"i_range1_hi", RW},
	{ 0x208,	"i_range2_hi", RW},
	{ 0x20c,	"i_range3_hi", RW},

	/* 0x210-0x21f Reserved */
 	{ 0x220,	"i_range0_lo", RW},
	{ 0x224,	"i_range1_lo", RW},
	{ 0x228,	"i_range2_lo", RW},
	{ 0x22c,	"i_range3_lo", RW},

	/* 0x230-0x23f Reserved */
 	{ 0x240,	"i_range0_en", RW},
	{ 0x244,	"i_range1_en", RW},
	{ 0x248,	"i_range2_en", RW},
	{ 0x24c,	"i_range3_en", RW},

	/* 0x250-0x25f Reserved */
 	{ 0x260,	"d_range0_hi", RW},
	{ 0x264,	"d_range1_hi", RW},
	{ 0x268,	"d_range2_hi", RW},
	{ 0x26c,	"d_range3_hi", RW},
	{ 0x270,	"d_range4_hi", RW},
	{ 0x274,	"d_range5_hi", RW},

	/* 0x278-0x27f Reserved */
 	{ 0x280,	"d_range0_lo", RW},
	{ 0x284,	"d_range1_lo", RW},
	{ 0x288,	"d_range2_lo", RW},
	{ 0x28c,	"d_range3_lo", RW},
	{ 0x290,	"d_range4_lo", RW},
	{ 0x294,	"d_range5_lo", RW},

	/* 0x298-0x29f Reserved */
 	{ 0x2a0,	"d_range0_en", RW},
	{ 0x2a4,	"d_range1_en", RW},
	{ 0x2a8,	"d_range2_en", RW},
	{ 0x2ac,	"d_range3_en", RW},
	{ 0x2b0,	"d_range4_en", RW},
	{ 0x2b4,	"d_range5_en", RW},

	/* 0x2b8-0x2bf Reserved */
 	{ 0x2c0,	"i_range0_user_en", RW},
	{ 0x2c4,	"i_range1_user_en", RW},
	{ 0x2c8,	"i_range2_user_en", RW},
	{ 0x2cc,	"i_range3_user_en", RW},

	/* 0x2d0 - 0x2df Reserved */
 	{ 0x2e0,	"d_range0_user_en", RW},
	{ 0x2e4,	"d_range1_user_en", RW},
	{ 0x2e8,	"d_range2_user_en", RW},
	{ 0x2ec,	"d_range3_user_en", RW},
	{ 0x2f0,	"d_range4_user_en", RW},
	{ 0x2f4,	"d_range5_user_en", RW},

	/* 0x2f8 - 0x3ff Reserved */
	{ 0x0,		0, RW, },
};

/* t_is_set will be 1 if .t is set for the madd.2 and msub.2 instructions */
static unsigned char t_is_set =0;

static const char *
parse_t_is_set_for_addsub (
			   CGEN_CPU_DESC cd ATTRIBUTE_UNUSED,
			   const char **strp,
			   CGEN_KEYWORD *keyword_table,
			   long *valuep)
{
  const char *errmsg;

  t_is_set = 0;

  errmsg = cgen_parse_keyword (cd, strp, keyword_table, valuep);
  if (errmsg)
    {
      t_is_set = 0;

      return errmsg;
    }

  if((int)*valuep)
     t_is_set = 1;

  return NULL;
}

char myerrmsg[128];

/* 
 * If accumulator is selected for madd.2 and msub.2 instructions then
 * the T bit should not be selected. Flag an assembler error in those
 * cases.
 */
static const char *
parse_acc_for_addsub (CGEN_CPU_DESC cd ATTRIBUTE_UNUSED,
		      const char **strp,
		      CGEN_KEYWORD *keyword_table,
		      long *valuep)
{
  const char *errmsg;

  errmsg = cgen_parse_keyword (cd, strp, keyword_table, valuep);
  if (errmsg)
    {
      t_is_set = 0;

      return errmsg;
    }

  
  if(t_is_set)
    {
      /* This is erroneous. */
      sprintf(myerrmsg, "Extenstion \".t\" is illegal when using acc%d as Source 2 register.", (int)*valuep);
      t_is_set=0;
      return (myerrmsg);
    }

  t_is_set=0;
  return NULL;
}

/*
 * For dsp madd/msub cases if S2 is a data register then t_is_set flag should be set to zero.
 */
static const char *
parse_dr_for_addsub (CGEN_CPU_DESC cd ATTRIBUTE_UNUSED,
		     const char **strp,
		     CGEN_KEYWORD *keyword_table,
		     long *valuep)
{
  const char *errmsg;

  errmsg = cgen_parse_keyword (cd, strp, keyword_table, valuep);
  if (errmsg)
    {
      t_is_set = 0;

      return errmsg;
    }
  t_is_set=0;
  return NULL;
}

static const char *
parse_bit5 (CGEN_CPU_DESC cd,
	    const char **strp,
	    int opindex,
	    long *valuep)
{
  const char *errmsg;
  char mode = 0;
  long count = 0;
  unsigned long value;

  if (strncmp (*strp, "%bit", 4) == 0)
    {
      *strp += 4;
      mode = 1;
    }
  else if (strncmp (*strp, "%msbbit", 7) == 0)
    {
      *strp += 7;
      mode = 2;
    }
  else if (strncmp (*strp, "%lsbbit", 7) == 0)
    {
      *strp += 7;
      mode = 3;
    }

  errmsg = cgen_parse_unsigned_integer (cd, strp, opindex, valuep);
  if (errmsg) {
    return errmsg;
  }

  if (mode) {
    value = (unsigned long) *valuep;
    if (value == 0) {
      errmsg = _("Attempt to find bit index of 0");
      return errmsg;
    }
    
    if (mode == 1) {
      count = 31;
      while ((value & 0x80000000) == 0) {
        count--;
        value <<= 1;
      }
      if ((value & 0x7FFFFFFF) != 0) {
        errmsg = _("More than one bit set in bitmask");
        return errmsg;
      }
    } else if (mode == 2) {
      count = 31;
      while ((value & 0x80000000) == 0) {
        count--;
        value <<= 1;
      }
    } else if (mode == 3) {
      count = 0;
      while ((value & 0x00000001) == 0) {
        count++;
        value >>= 1;
      }
    }
    
    *valuep = count;
  }

  return errmsg;
}

/*
 * For dsp madd/msub cases if S2 is a #bit5 then t_is_set flag should be set to zero.
 */
static const char *
parse_bit5_for_addsub (CGEN_CPU_DESC cd ATTRIBUTE_UNUSED,
		       const char **strp,
		       int opindex,
		       long *valuep)
{
  const char *errmsg;

  errmsg = parse_bit5(cd, strp, opindex, valuep);
  if (errmsg)
    {
      t_is_set = 0;

      return errmsg;
    }
  t_is_set=0;
  return NULL;
}

/* Parse signed 4 bit immediate value, being careful (hacky) to avoid
   eating a `++' that might be present */
static const char *
parse_imm4 (CGEN_CPU_DESC cd,
	    const char **strp,
	    int opindex,
	    long *valuep,
	    int size)
{
  const char *errmsg;
  char *plusplus;
  long value;

  plusplus = strstr(*strp, "++");
  if (plusplus)
    *plusplus = 0;
  errmsg = cgen_parse_signed_integer (cd, strp, opindex, &value);
  if (plusplus)
    *plusplus = '+';

  if (errmsg == NULL)
    {
      if ((size == 2 && (value % 2)) ||
          (size == 4 && (value % 4)))
	errmsg = _("unaligned increment");
      else if ((size == 1 && (value < -8 || value > 7)) ||
          (size == 2 && (value < -16 || value > 15)) ||
          (size == 4 && (value < -32 || value > 31)))
	errmsg = _("out of bounds increment");
      else
        *valuep = value;
    }
  return errmsg;
}

/* as above, for single byte addresses */
static const char *
parse_imm4_1 (CGEN_CPU_DESC cd,
	      const char **strp,
	      int opindex,
	      long *valuep)
{
  return parse_imm4 (cd, strp, opindex, valuep, 1);
}

/* as above, for half-word addresses */
static const char *
parse_imm4_2 (CGEN_CPU_DESC cd,
	      const char **strp,
	      int opindex,
	      long *valuep)
{
  return parse_imm4 (cd, strp, opindex, valuep, 2);
}

/* as above, for word addresses */
static const char *
parse_imm4_4 (CGEN_CPU_DESC cd,
	      const char **strp,
	      int opindex,
	      long *valuep)
{
  return parse_imm4 (cd, strp, opindex, valuep, 4);
}

/* Parse a direct address.  This can be either `$xx' or a Register
   Mnemonic.
 */
static const char *
parse_direct_addr (CGEN_CPU_DESC cd,
		   const char **strp,
		   int opindex,
		   long *valuep,
		   int isdest)
{
  const char *errmsg = NULL;
  bfd_vma value;
  struct ubicom32_cgen_data_space_map *cur;
  size_t len;

  if(cd->machs & (1<<MACH_IP3035))
    {
      /* cpu is mercury */
      cur = ubicom32_cgen_data_space_map_mercury;
    }

  if((cd->machs & (1<<MACH_UBICOM32DSP)) || (cd->machs & (1<<MACH_UBICOM32_VER4)))
    {
      /* cpu is mars/ares */
      cur = ubicom32_cgen_data_space_map_mars;
    }

  if(cd->machs & (1<<MACH_UBICOM32_VER5))
    {
      /* cpu is jupiter */
      cur = ubicom32_cgen_data_space_map_jupiter;
    }

  /* First, try to look for the literal register name. */
  for (; cur->name; cur++)
    if (strncasecmp(cur->name, *strp, (len = strlen(cur->name))) == 0 &&
        !ISALNUM((*strp)[len]) && (*strp)[len] != '_' )
      {
	*strp += len;
        /* fail if specifying a read-only register as a destination */
	if (isdest && cur->type == RO)
	  return _("attempt to write to read-only register");
	
	/* fail if specifying a write-only register as a source */
	if ((isdest==0) && cur->type == WO)
	  return _("attempt to read a write-only register");
	value = cur->address;
	errmsg = NULL;
	break;
      }
  
  /* Not found: try parsing it as a literal */
  if (cur->name == NULL)
    {
      char *plusplus;
      if (**strp == '(')
	{
	  return _("parentheses are reserved for indirect addressing");
	}

      if (strncasecmp(*strp, "%f", 2) == 0)
	{
	  *valuep = 0;
	  return NULL;
	}

      /* we want to avoid parsing a negative post-increment expression as a numeric
	 expression because the parser assumes zeroes exist between the pluses and
	 issues an extraneous warning message. */
      plusplus = strstr(*strp, "++");
      if (plusplus)
	*plusplus = 0;
      errmsg = cgen_parse_signed_integer (cd, strp, opindex, &value);
      if (plusplus)
	*plusplus = '+';

      if (errmsg)
	return errmsg;
    }

  value &= 0x3ff;
  *valuep = value;
  return errmsg;
}

static const char *
parse_d_direct_addr (CGEN_CPU_DESC cd,
		     const char **strp,
		     int opindex,
		     long *valuep)
{
  return parse_direct_addr (cd, strp, opindex, valuep, 1);
}

static const char *
parse_s1_direct_addr (CGEN_CPU_DESC cd,
		      const char **strp,
		      int opindex,
		      long *valuep)
{
  return parse_direct_addr (cd, strp, opindex, valuep, 0);
}

/* support for source-1 and destination operand 7-bit immediates for indirect addressing */
static const char *imm7_1_rangemsg = "7-bit byte immediate value out of range";
static const char *imm7_2_rangemsg = "7-bit halfword immediate value out of range";
static const char *imm7_4_rangemsg = "7-bit word immediate value out of range";
static const char *imm7_pdec_rangemsg = "Pdec offset out of range. Allowed range is >=4 and <=512.";
static const char *imm7_2_maskmsg = "7-bit halfword immediate not a multiple of 2";
static const char *imm7_4_maskmsg = "7-bit word immediate not a multiple of 4";

/* Parse 7-bit immediates, allow %lo() operator */
static const char *
parse_imm7_basic (CGEN_CPU_DESC cd,
		  const char **strp,
		  int opindex,
		  unsigned long *valuep,
		  const char *rangemsg,
		  const char *maskmsg,
		  bfd_vma max,
		  int mask,
		  int reloc)
{
  const char *errmsg;
  enum cgen_parse_operand_result result_type = CGEN_PARSE_OPERAND_RESULT_NUMBER;
  bfd_vma value;
  int newreloc;

  /* in this case we want low 7-bits to accompany the 24-bit immediate of a moveai instruction */
  if (strncasecmp (*strp, "%lo(", 4) == 0)
    {
      *strp += 4;
      errmsg = cgen_parse_address (cd, strp, opindex, reloc,
				   &result_type, &value);
      if (**strp != ')')
	return _("missing `)'");
      ++*strp;
      if (errmsg == NULL
  	  && result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
	value &= 0x7f;  /* always want 7 bits, regardless of imm7 type */
      *valuep = value;
      return errmsg;
    }
  else if (strncasecmp (*strp, "%got_lo(", strlen("%got_lo(")) == 0)
    {
      *strp += strlen("%got_lo(");

      /* Switch the relocation to the GOT relocation. */
      switch(reloc)
	{
	case BFD_RELOC_UBICOM32_LO7_S:
	  reloc = BFD_RELOC_UBICOM32_GOT_LO7_S;
	  break;
	case BFD_RELOC_UBICOM32_LO7_2_S:
	  reloc = BFD_RELOC_UBICOM32_GOT_LO7_2_S;
	  break;
	case BFD_RELOC_UBICOM32_LO7_4_S:
	  reloc = BFD_RELOC_UBICOM32_GOT_LO7_4_S;
	  break;
	case BFD_RELOC_UBICOM32_LO7_D:
	  reloc = BFD_RELOC_UBICOM32_GOT_LO7_D;
	  break;
	case BFD_RELOC_UBICOM32_LO7_2_D:
	  reloc = BFD_RELOC_UBICOM32_GOT_LO7_2_D;
	  break;
	case BFD_RELOC_UBICOM32_LO7_4_D:
	  reloc = BFD_RELOC_UBICOM32_GOT_LO7_4_D;
	  break;
	}
      errmsg = cgen_parse_address (cd, strp, opindex, reloc,
				   &result_type, &value);
      if (**strp != ')')
	return _("missing `)'");
      ++*strp;
      if (errmsg == NULL
  	  && result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
	value &= 0x7f;  /* always want 7 bits, regardless of imm7 type */
      *valuep = value;
      return errmsg;
    }
  else if (strncasecmp (*strp, "%funcdesc_got_lo(", strlen("%funcdesc_got_lo(")) == 0)
    {
      *strp += strlen("%funcdesc_got_lo(");

      /* Switch the relocation to the GOT relocation. */
      switch(reloc)
	{
	case BFD_RELOC_UBICOM32_LO7_S:
	  reloc = BFD_RELOC_UBICOM32_FUNCDESC_GOT_LO7_S;
	  break;
	case BFD_RELOC_UBICOM32_LO7_2_S:
	  reloc = BFD_RELOC_UBICOM32_FUNCDESC_GOT_LO7_2_S;
	  break;
	case BFD_RELOC_UBICOM32_LO7_4_S:
	  reloc = BFD_RELOC_UBICOM32_FUNCDESC_GOT_LO7_4_S;
	  break;
	case BFD_RELOC_UBICOM32_LO7_D:
	  reloc = BFD_RELOC_UBICOM32_FUNCDESC_GOT_LO7_D;
	  break;
	case BFD_RELOC_UBICOM32_LO7_2_D:
	  reloc = BFD_RELOC_UBICOM32_FUNCDESC_GOT_LO7_2_D;
	  break;
	case BFD_RELOC_UBICOM32_LO7_4_D:
	  reloc = BFD_RELOC_UBICOM32_FUNCDESC_GOT_LO7_4_D;
	  break;
	}
      errmsg = cgen_parse_address (cd, strp, opindex, reloc,
				   &result_type, &value);
      if (**strp != ')')
	return _("missing `)'");
      ++*strp;
      if (errmsg == NULL
  	  && result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
	value &= 0x7f;  /* always want 7 bits, regardless of imm7 type */
      *valuep = value;
      return errmsg;
    }
  else
    {
      if (**strp == '(')
	{
	  return _("parentheses are reserved for indirect addressing");
	}

      errmsg = cgen_parse_unsigned_integer (cd, strp, opindex, &value);
    }

  if (errmsg == NULL)
    {
      if (value > max)
	return rangemsg;
      if (value & mask)
	return maskmsg;
    }

  *valuep = value & max;
  return errmsg;
}

/* Parse 7-bit immediates, allow %lo() operator */
static const char *
parse_imm7_pdec (CGEN_CPU_DESC cd,
		 const char **strp,
		 int opindex,
		 unsigned long *valuep,
		 const char *rangemsg,
		 const char *maskmsg,
		 int reloc)
{
  const char *errmsg;
  enum cgen_parse_operand_result result_type = CGEN_PARSE_OPERAND_RESULT_NUMBER;
  bfd_vma value;

  /* in this case we want low 7-bits to accompany the 24-bit immediate of a moveai instruction */
  if (strncasecmp (*strp, "%lo(", 4) == 0)
    {
      *strp += 4;
      errmsg = cgen_parse_address (cd, strp, opindex, reloc,
				   &result_type, &value);
      if (**strp != ')')
	return _("missing `)'");
      ++*strp;
      if (errmsg == NULL
  	  && result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
	value &= 0x7f;  /* always want 7 bits, regardless of imm7 type */
      *valuep = value;
      return errmsg;
    }
  else
    {
      if (**strp == '(')
	{
	  return _("parentheses are reserved for indirect addressing");
	}

      errmsg = cgen_parse_unsigned_integer (cd, strp, opindex, &value);
    }

  if (errmsg == NULL)
    {
      if (((long)value > 512) || ((long)value < 4))
	return rangemsg;
      if (value & 0x3)
	return maskmsg;
    }

  *valuep = value;
  return errmsg;
}

/* single byte imm7 */
static const char *
parse_imm7_1_s (CGEN_CPU_DESC cd,
		const char **strp,
		int opindex,
		unsigned long *valuep)
{
  return parse_imm7_basic (cd, strp, opindex, valuep, _(imm7_1_rangemsg),
			   NULL, 0x7f, 0, BFD_RELOC_UBICOM32_LO7_S);
}

/* halfword imm7 */
static const char *
parse_imm7_2_s (CGEN_CPU_DESC cd,
		const char **strp,
		int opindex,
		unsigned long *valuep)
{
  return parse_imm7_basic (cd, strp, opindex, valuep, 
			   _(imm7_2_rangemsg),
			   _(imm7_2_maskmsg),
			   0xfe, 0x1, BFD_RELOC_UBICOM32_LO7_2_S);
}

/* word imm7 */
static const char *
parse_imm7_4_s (CGEN_CPU_DESC cd,
		const char **strp,
		int opindex,
		unsigned long *valuep)
{
  return parse_imm7_basic (cd, strp, opindex, valuep, 
			   _(imm7_4_rangemsg),
			   _(imm7_4_maskmsg),
			   0x1fc, 0x3, BFD_RELOC_UBICOM32_LO7_4_S);
}

/* word imm7 */
static const char *
parse_pdec_imm7_4_s (CGEN_CPU_DESC cd,
		     const char **strp,
		     int opindex,
		     unsigned long *valuep)
{
  unsigned long value;
  const char *errmsg = parse_imm7_pdec (cd, strp, opindex, &value, 
					_(imm7_pdec_rangemsg),
					_(imm7_4_maskmsg),
					BFD_RELOC_UBICOM32_LO7_4_S);

  if(errmsg == NULL)
    {
      /* at this point we have a valid value. Take the 2's comp and truncate to 7 bits */
      if(value == 0)
	return _("Offset for PDEC source cannot be 0");

      value = ~value;
      value ++;
      value &= 0x1fc;
      *valuep = value;
    }

  return errmsg;
}

/* single byte dest imm7 */
static const char *
parse_imm7_1_d (CGEN_CPU_DESC cd,
		const char **strp,
		int opindex,
		unsigned long *valuep)
{
  return parse_imm7_basic (cd, strp, opindex, valuep, _(imm7_1_rangemsg),
			   NULL, 0x7f, 0, BFD_RELOC_UBICOM32_LO7_D);
}

/* halfword dest imm7 */
static const char *
parse_imm7_2_d (CGEN_CPU_DESC cd,
		const char **strp,
		int opindex,
		unsigned long *valuep)
{
  return parse_imm7_basic (cd, strp, opindex, valuep, 
			   _(imm7_2_rangemsg),
			   _(imm7_2_maskmsg),
			   0xfe, 0x1, BFD_RELOC_UBICOM32_LO7_2_D);
}

/* word dest imm7 */
static const char *
parse_imm7_4_d (CGEN_CPU_DESC cd,
		const char **strp,
		int opindex,
		unsigned long *valuep)
{
  return parse_imm7_basic (cd, strp, opindex, valuep, 
			   _(imm7_4_rangemsg),
			   _(imm7_4_maskmsg),
			   0x1fc, 0x3, BFD_RELOC_UBICOM32_LO7_4_D);
}

/* Parse 16-bit immediate, allow %hi() or %lo() operators */
static const char *
parse_imm16 (CGEN_CPU_DESC cd,
	     const char **strp,
	     int opindex,
	     unsigned long *valuep)
{
  const char *errmsg;
  enum cgen_parse_operand_result result_type = CGEN_PARSE_OPERAND_RESULT_NUMBER;
  bfd_vma value;

  if (strncasecmp (*strp, "%hi(", 4) == 0)
    {
      *strp += 4;
      errmsg = cgen_parse_address (cd, strp, opindex, BFD_RELOC_HI16,
				   &result_type, &value);
      if (**strp != ')')
	return _("missing `)'");
      ++*strp;
      if (errmsg == NULL
  	  && result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
	value >>= 16;
      *valuep = value;
      return errmsg;
    }
  else if (strncasecmp (*strp, "%got_hi(", strlen("%got_hi(")) == 0)
    {
      *strp += strlen("%got_hi(");
      errmsg = cgen_parse_address (cd, strp, opindex, BFD_RELOC_UBICOM32_GOT_HI,
				   &result_type, &value);
      if (**strp != ')')
	return _("missing `)'");
      ++*strp;
      if (errmsg == NULL
  	  && result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
	value >>= 16;
      *valuep = value;
      return errmsg;
    }
  else if ((strncasecmp (*strp, "%funcdesc_got_hi(", strlen("%funcdesc_got_hi(")) == 0)
	   || (strncasecmp (*strp, "%got_funcdesc_hi(", strlen("%got_funcdesc_hi(")) == 0))
    {
      *strp += strlen("%funcdesc_got_hi(");
      errmsg = cgen_parse_address (cd, strp, opindex, BFD_RELOC_UBICOM32_FUNCDESC_GOT_HI,
				   &result_type, &value);
      if (**strp != ')')
	return _("missing `)'");
      ++*strp;
      if (errmsg == NULL
  	  && result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
	value >>= 16;
      *valuep = value;
      return errmsg;
    }
  else if (strncasecmp (*strp, "%lo(", 4) == 0)
    {
      *strp += 4;
      errmsg = cgen_parse_address (cd, strp, opindex, BFD_RELOC_LO16,
				   &result_type, &value);
      if (**strp != ')')
	return _("missing `)'");
      ++*strp;
      if (errmsg == NULL
  	  && result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
	value &= 0xffff;
      *valuep = value;
      return errmsg;
    }
  else if (strncasecmp (*strp, "%got_lo(", strlen("%got_lo(")) == 0)
    {
      *strp += strlen("%got_lo(");
      errmsg = cgen_parse_address (cd, strp, opindex, BFD_RELOC_UBICOM32_GOT_LO,
				   &result_type, &value);
      if (**strp != ')')
	return _("missing `)'");
      ++*strp;
      if (errmsg == NULL
  	  && result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
	value &= 0xffff;
      *valuep = value;
      return errmsg;
    }
  else if ((strncasecmp (*strp, "%funcdesc_got_lo(", strlen("%funcdesc_got_lo(")) == 0)
	   || (strncasecmp (*strp, "%got_funcdesc_lo(", strlen("%got_funcdesc_lo(")) == 0))
    {
      *strp += strlen("%funcdesc_got_lo(");
      errmsg = cgen_parse_address (cd, strp, opindex, BFD_RELOC_UBICOM32_FUNCDESC_GOT_LO,
				   &result_type, &value);
      if (**strp != ')')
	return _("missing `)'");
      ++*strp;
      if (errmsg == NULL
  	  && result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
	value &= 0xffff;
      *valuep = value;
      return errmsg;
    }
  else
    {
      errmsg = cgen_parse_unsigned_integer (cd, strp, opindex, &value);
    }

  if (errmsg == NULL
      && ((long)value > 65535 || (long)value < -32768))
    return _("16-bit immediate value out of range");

  *valuep = value & 0xffff;
  return errmsg;
}

/* Parse 24-bit immediate for moveai instruction and allow %hi() operator */
static const char *
parse_imm24 (CGEN_CPU_DESC cd,
	     const char **strp,
	     int opindex,
	     unsigned long *valuep)
{
  const char *errmsg;
  enum cgen_parse_operand_result result_type = CGEN_PARSE_OPERAND_RESULT_NUMBER;
  bfd_vma value;

  if (strncasecmp (*strp, "%hi(", 4) == 0)
    {
      *strp += 4;
      errmsg = cgen_parse_address (cd, strp, opindex, BFD_RELOC_UBICOM32_HI24,
				   &result_type, &value);
      if (**strp != ')')
	return _("missing `)'");
      ++*strp;
      if (errmsg == NULL
  	  && result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
	value >>= 7;
      *valuep = value;
      return errmsg;
    }
  else if (strncasecmp (*strp, "%got_hi(", strlen("%got_hi(")) == 0)
    {
      *strp += strlen("%got_hi(");
      errmsg = cgen_parse_address (cd, strp, opindex, BFD_RELOC_UBICOM32_GOT_HI24,
				   &result_type, &value);
      if (**strp != ')')
	return _("missing `)'");
      ++*strp;
      if (errmsg == NULL
  	  && result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
	value >>= 7;
      *valuep = value;
      return errmsg;
    }
  else if (strncasecmp (*strp, "%funcdesc_got_hi(", strlen("%funcdesc_got_hi(")) == 0)
    {
      *strp += strlen("%funcdesc_got_hi(");
      errmsg = cgen_parse_address (cd, strp, opindex, BFD_RELOC_UBICOM32_FUNCDESC_GOT_HI24,
				   &result_type, &value);
      if (**strp != ')')
	return _("missing `)'");
      ++*strp;
      if (errmsg == NULL
  	  && result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
	value >>= 7;
      *valuep = value;
      return errmsg;
    }
  else
    {
      errmsg = cgen_parse_unsigned_integer (cd, strp, opindex, &value);
    }

  if (errmsg == NULL
      && ((long)value > 16777215 || (long)value < 0))
    return _("24-bit immediate value out of range");

  *valuep = value;
  return errmsg;
}

/* Parse 24-bit immediate for moveai instruction and allow %hi() operator */
static const char *
parse_imm25 (CGEN_CPU_DESC cd,
	     const char **strp,
	     int opindex,
	     unsigned long *valuep)
{
  const char *errmsg;
  enum cgen_parse_operand_result result_type = CGEN_PARSE_OPERAND_RESULT_NUMBER;
  bfd_vma value;

  if (strncasecmp (*strp, "%hi(", 4) == 0)
    {
      *strp += 4;
      errmsg = cgen_parse_address (cd, strp, opindex, BFD_RELOC_UBICOM32_HI24,
				   &result_type, &value);
      if (**strp != ')')
	return _("missing `)'");
      ++*strp;
      if (errmsg == NULL
  	  && result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
	value >>= 7;
      *valuep = value;
      return errmsg;
    }
  else if (strncasecmp (*strp, "%got_hi(", strlen("%got_hi(")) == 0)
    {
      *strp += strlen("%got_hi(");
      errmsg = cgen_parse_address (cd, strp, opindex, BFD_RELOC_UBICOM32_GOT_HI24,
				   &result_type, &value);
      if (**strp != ')')
	return _("missing `)'");
      ++*strp;
      if (errmsg == NULL
  	  && result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
	value >>= 7;
      *valuep = value;
      return errmsg;
    }
  else if (strncasecmp (*strp, "%funcdesc_got_hi(", strlen("%funcdesc_got_hi(")) == 0)
    {
      *strp += strlen("%funcdesc_got_hi(");
      errmsg = cgen_parse_address (cd, strp, opindex, BFD_RELOC_UBICOM32_FUNCDESC_GOT_HI24,
				   &result_type, &value);
      if (**strp != ')')
	return _("missing `)'");
      ++*strp;
      if (errmsg == NULL
  	  && result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
	value >>= 7;
      *valuep = value;
      return errmsg;
    }
  else
    {
      errmsg = cgen_parse_unsigned_integer (cd, strp, opindex, &value);
    }

  if (errmsg == NULL
      && ((long)value > 33554431 || (long)value < 0))
    return _("24-bit immediate value out of range");

  *valuep = value;
  return errmsg;
}

static const char *
parse_offset21 (CGEN_CPU_DESC cd,
		const char **strp,
		int opindex,
		int reloc ATTRIBUTE_UNUSED,
		enum cgen_parse_operand_result *type_addr ATTRIBUTE_UNUSED, 
		unsigned long *valuep)
{
  const char *errmsg;
  enum cgen_parse_operand_result result_type = CGEN_PARSE_OPERAND_RESULT_NUMBER;
  bfd_vma value;

  if (**strp == '#')
    {
      ++*strp;
      errmsg = cgen_parse_unsigned_integer (cd, strp, opindex, &value);
    }
  else
    errmsg = cgen_parse_address (cd, strp, opindex, BFD_RELOC_UBICOM32_21_PCREL,
			       &result_type, &value);

  if (errmsg == NULL && result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
    {
      /* we treat jmp #constant as being jump to pc + constant * 4 */
      if ((long)value > 1048575 || (long)value < -1048576)
        return _("21-bit relative offset out of range");
    }

  *valuep = value & 0x7fffff; /* address is actually 23 bits before shift */
  return errmsg;
}

static const char *
parse_offset16 (CGEN_CPU_DESC cd,
		const char **strp,
		int opindex,
		unsigned long *valuep)
{
  const char *errmsg;
  enum cgen_parse_operand_result result_type = CGEN_PARSE_OPERAND_RESULT_NUMBER;
  bfd_vma value;

  /* in this case we want low 7-bits to accompany the 24-bit immediate of a moveai instruction */
  if (strncasecmp (*strp, "%lo(", 4) == 0)
    {
      *strp += 4;
      errmsg = cgen_parse_address (cd, strp, opindex, BFD_RELOC_UBICOM32_LO7_CALLI,
				   &result_type, &value);
      if (errmsg != NULL)
        return errmsg;

      if (**strp != ')')
	return _("missing `)'");
      ++*strp;

      if (result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
        *valuep = value & 0x7c;

      return NULL;
    }

  if (strncasecmp (*strp, "%got_lo(", strlen("%got_lo(")) == 0)
    {
      *strp += strlen("%got_lo(");
      errmsg = cgen_parse_address (cd, strp, opindex, BFD_RELOC_UBICOM32_GOT_LO7_CALLI,
				   &result_type, &value);
      if (errmsg != NULL)
        return errmsg;

      if (**strp != ')')
	return _("missing `)'");
      ++*strp;

      if (result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
        *valuep = value & 0x7c;

      return NULL;
    }

  if (strncasecmp (*strp, "%funcdesc_got_lo(", strlen("%funcdesc_got_lo(")) == 0)
    {
      *strp += strlen("%funcdesc_got_lo(");
      errmsg = cgen_parse_address (cd, strp, opindex, BFD_RELOC_UBICOM32_FUNCDESC_GOT_LO7_CALLI,
				   &result_type, &value);
      if (errmsg != NULL)
        return errmsg;

      if (**strp != ')')
	return _("missing `)'");
      ++*strp;

      if (result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
        *valuep = value & 0x7c;

      return NULL;
    }

  if (strncasecmp (*strp, "%lo18(", 6) == 0)
    {
      *strp += 6;
      errmsg = cgen_parse_address (cd, strp, opindex, BFD_RELOC_UBICOM32_LO16_CALLI,
				   &result_type, &value);
      if (errmsg != NULL)
        return errmsg;

      if (**strp != ')')
	return _("missing `)'");
      ++*strp;

      if (result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
        *valuep = value & 0x0003fffc;

      return NULL;
    }

  errmsg = cgen_parse_signed_integer (cd, strp, opindex, &value);
  if (errmsg != NULL)
    return errmsg;

  /* ensure calli constant within limits and is multiple of 4 */
  if (value & 0x3)
    return _("calli offset must be multiple of 4");

  if ((long)value > 131071 || (long)value < -131072)
    return _("16-bit calli offset out of range");

  *valuep = value & 0x0003fffc; /* offset is actually 18 bits before shift */
  return NULL;
}

static const char *
parse_leai_offset16 (CGEN_CPU_DESC cd,
		const char **strp,
		int opindex,
		unsigned long *valuep)
{
  const char *errmsg;
  enum cgen_parse_operand_result result_type = CGEN_PARSE_OPERAND_RESULT_NUMBER;
  bfd_vma value;

  /* in this case we want low 7-bits to accompany the 24-bit immediate of a moveai instruction */
  if (strncasecmp (*strp, "%lo(", 4) == 0)
    {
      *strp += 4;
      errmsg = cgen_parse_address (cd, strp, opindex, BFD_RELOC_UBICOM32_LO7_LEAI,
				   &result_type, &value);
      if (errmsg != NULL)
        return errmsg;

      if (**strp != ')')
	return _("missing `)'");
      ++*strp;

      if (result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
        *valuep = value & 0x7c;

      return NULL;
    }

  if (strncasecmp (*strp, "%got_lo(", strlen("%got_lo(")) == 0)
    {
      *strp += strlen("%got_lo(");
      errmsg = cgen_parse_address (cd, strp, opindex, BFD_RELOC_UBICOM32_GOT_LO_LEAI,
				   &result_type, &value);
      if (errmsg != NULL)
        return errmsg;

      if (**strp != ')')
	return _("missing `)'");
      ++*strp;

      if (result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
        *valuep = value & 0xffff;

      return NULL;
    }

  if (strncasecmp (*strp, "%funcdesc_got_lo(", strlen("%funcdesc_got_lo(")) == 0)
    {
      *strp += strlen("%funcdesc_got_lo(");
      errmsg = cgen_parse_address (cd, strp, opindex, BFD_RELOC_UBICOM32_FUNCDESC_GOT_LO_LEAI,
				   &result_type, &value);
      if (errmsg != NULL)
        return errmsg;

      if (**strp != ')')
	return _("missing `)'");
      ++*strp;

      if (result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
        *valuep = value & 0xffff;

      return NULL;
    }

  if (strncasecmp (*strp, "%lo18(", 6) == 0)
    {
      *strp += 6;
      errmsg = cgen_parse_address (cd, strp, opindex, BFD_RELOC_UBICOM32_LO16_LEAI,
				   &result_type, &value);
      if (errmsg != NULL)
        return errmsg;

      if (**strp != ')')
	return _("missing `)'");
      ++*strp;

      if (result_type == CGEN_PARSE_OPERAND_RESULT_NUMBER)
        *valuep = value & 0x0003fffc;

      return NULL;
    }

  errmsg = cgen_parse_signed_integer (cd, strp, opindex, &value);
  if (errmsg != NULL)
    return errmsg;

  /* ensure leai constant within limits and is multiple of 4 */
  if (value & 0x3)
    return _("leai offset must be multiple of 4");

  if ((long)value > 131071 || (long)value < -131072)
    return _("16-bit leai offset out of range");

  *valuep = value & 0x0003fffc; /* address is actually 18 bits before shift */
  return NULL;
}

static const char *
parse_imm8 (CGEN_CPU_DESC cd,
	    const char **strp,
	    int opindex,
	    unsigned long *valuep)
{
  const char *errmsg;
  bfd_vma value;
  int no_sign = 0;

  if (**strp == '0' && TOUPPER(*(*strp+1)) == 'X')
    no_sign = 1;

  errmsg = cgen_parse_signed_integer (cd, strp, opindex, &value);

  if (errmsg == NULL)
    {
      if ((no_sign && ((long)value > 255)) || 
	  (!no_sign && (((long)value > 127) || ((long)value < -128))))
        return _("8-bit immediate value out of range");
    }

  *valuep = value & 0xff;
  return errmsg;
}

/* -- dis.c */

const char * ubicom32_cgen_parse_operand
  (CGEN_CPU_DESC, int, const char **, CGEN_FIELDS *);

/* Main entry point for operand parsing.

   This function is basically just a big switch statement.  Earlier versions
   used tables to look up the function to use, but
   - if the table contains both assembler and disassembler functions then
     the disassembler contains much of the assembler and vice-versa,
   - there's a lot of inlining possibilities as things grow,
   - using a switch statement avoids the function call overhead.

   This function could be moved into `parse_insn_normal', but keeping it
   separate makes clear the interface between `parse_insn_normal' and each of
   the handlers.  */

const char *
ubicom32_cgen_parse_operand (CGEN_CPU_DESC cd,
			   int opindex,
			   const char ** strp,
			   CGEN_FIELDS * fields)
{
  const char * errmsg = NULL;
  /* Used by scalar operands that still need to be parsed.  */
  long junk ATTRIBUTE_UNUSED;

  switch (opindex)
    {
    case UBICOM32_OPERAND_AM :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_addr_names, & fields->f_Am);
      break;
    case UBICOM32_OPERAND_AN :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_addr_names, & fields->f_An);
      break;
    case UBICOM32_OPERAND_C :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_h_C, & fields->f_C);
      break;
    case UBICOM32_OPERAND_DN :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_data_names, & fields->f_Dn);
      break;
    case UBICOM32_OPERAND_P :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_h_P, & fields->f_P);
      break;
    case UBICOM32_OPERAND_ACC1HI :
      errmsg = cgen_parse_unsigned_integer (cd, strp, UBICOM32_OPERAND_ACC1HI, (unsigned long *) (& junk));
      break;
    case UBICOM32_OPERAND_ACC1LO :
      errmsg = cgen_parse_unsigned_integer (cd, strp, UBICOM32_OPERAND_ACC1LO, (unsigned long *) (& junk));
      break;
    case UBICOM32_OPERAND_BIT5 :
      errmsg = parse_bit5 (cd, strp, UBICOM32_OPERAND_BIT5, (unsigned long *) (& fields->f_bit5));
      break;
    case UBICOM32_OPERAND_BIT5_ADDSUB :
      errmsg = parse_bit5_for_addsub (cd, strp, UBICOM32_OPERAND_BIT5_ADDSUB, (unsigned long *) (& fields->f_bit5));
      break;
    case UBICOM32_OPERAND_CC :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_h_cc, & fields->f_cond);
      break;
    case UBICOM32_OPERAND_D_AN :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_addr_names, & fields->f_d_An);
      break;
    case UBICOM32_OPERAND_D_DIRECT_ADDR :
      errmsg = parse_d_direct_addr (cd, strp, UBICOM32_OPERAND_D_DIRECT_ADDR, (unsigned long *) (& fields->f_d_direct));
      break;
    case UBICOM32_OPERAND_D_I4_1 :
      errmsg = parse_imm4_1 (cd, strp, UBICOM32_OPERAND_D_I4_1, (long *) (& fields->f_d_i4_1));
      break;
    case UBICOM32_OPERAND_D_I4_2 :
      errmsg = parse_imm4_2 (cd, strp, UBICOM32_OPERAND_D_I4_2, (long *) (& fields->f_d_i4_2));
      break;
    case UBICOM32_OPERAND_D_I4_4 :
      errmsg = parse_imm4_4 (cd, strp, UBICOM32_OPERAND_D_I4_4, (long *) (& fields->f_d_i4_4));
      break;
    case UBICOM32_OPERAND_D_IMM7_1 :
      errmsg = parse_imm7_1_d (cd, strp, UBICOM32_OPERAND_D_IMM7_1, (unsigned long *) (& fields->f_d_imm7_1));
      break;
    case UBICOM32_OPERAND_D_IMM7_2 :
      errmsg = parse_imm7_2_d (cd, strp, UBICOM32_OPERAND_D_IMM7_2, (unsigned long *) (& fields->f_d_imm7_2));
      break;
    case UBICOM32_OPERAND_D_IMM7_4 :
      errmsg = parse_imm7_4_d (cd, strp, UBICOM32_OPERAND_D_IMM7_4, (unsigned long *) (& fields->f_d_imm7_4));
      break;
    case UBICOM32_OPERAND_D_IMM8 :
      errmsg = parse_imm8 (cd, strp, UBICOM32_OPERAND_D_IMM8, (long *) (& fields->f_d_imm8));
      break;
    case UBICOM32_OPERAND_D_R :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_data_names, & fields->f_d_r);
      break;
    case UBICOM32_OPERAND_DSP_S2_ACC_REG_ADDSUB :
      errmsg = parse_acc_for_addsub (cd, strp, & ubicom32_cgen_opval_acc_names, & fields->f_dsp_S2);
      break;
    case UBICOM32_OPERAND_DSP_S2_ACC_REG_MUL :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_acc_names, & fields->f_dsp_S2);
      break;
    case UBICOM32_OPERAND_DSP_S2_DATA_REG :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_data_names, & fields->f_dsp_S2);
      break;
    case UBICOM32_OPERAND_DSP_S2_DATA_REG_ADDSUB :
      errmsg = parse_dr_for_addsub (cd, strp, & ubicom32_cgen_opval_data_names, & fields->f_dsp_S2);
      break;
    case UBICOM32_OPERAND_DSP_S2_SEL :
      errmsg = cgen_parse_unsigned_integer (cd, strp, UBICOM32_OPERAND_DSP_S2_SEL, (unsigned long *) (& fields->f_dsp_S2_sel));
      break;
    case UBICOM32_OPERAND_DSP_C :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_h_DSP_C, & fields->f_dsp_C);
      break;
    case UBICOM32_OPERAND_DSP_DESTA :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_h_DSP_Dest_A, & fields->f_dsp_destA);
      break;
    case UBICOM32_OPERAND_DSP_T :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_h_DSP_T, & fields->f_dsp_T);
      break;
    case UBICOM32_OPERAND_DSP_T_ADDSUB :
      errmsg = parse_t_is_set_for_addsub (cd, strp, & ubicom32_cgen_opval_h_DSP_T_addsub, & fields->f_dsp_T);
      break;
    case UBICOM32_OPERAND_FPU_32_S2_ACC_REG :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_acc_lo_names, & fields->f_FPS2_reg32);
      break;
    case UBICOM32_OPERAND_FPU_32_S2_DATA_REG :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_data_names, & fields->f_FPS2_reg32);
      break;
    case UBICOM32_OPERAND_FPU_32_DEST_REG :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_acc_lo_names, & fields->f_FPD32);
      break;
    case UBICOM32_OPERAND_FPU_64_S1_ACC_REG :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_acc_names, & fields->f_FPS1_reg64);
      break;
    case UBICOM32_OPERAND_FPU_64_S1_DATA_REG :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_data_names_even, & fields->f_FPS1_reg64);
      break;
    case UBICOM32_OPERAND_FPU_64_S2_ACC_REG :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_acc_names, & fields->f_FPS2_reg64);
      break;
    case UBICOM32_OPERAND_FPU_64_S2_DATA_REG :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_data_names_even, & fields->f_FPS2_reg64);
      break;
    case UBICOM32_OPERAND_FPU_64_DEST_REG :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_acc_names, & fields->f_FPD64);
      break;
    case UBICOM32_OPERAND_IMM16_1 :
      errmsg = cgen_parse_signed_integer (cd, strp, UBICOM32_OPERAND_IMM16_1, (long *) (& fields->f_imm16_1));
      break;
    case UBICOM32_OPERAND_IMM16_2 :
      errmsg = parse_imm16 (cd, strp, UBICOM32_OPERAND_IMM16_2, (long *) (& fields->f_imm16_2));
      break;
    case UBICOM32_OPERAND_IMM24 :
      errmsg = parse_imm24 (cd, strp, UBICOM32_OPERAND_IMM24, (unsigned long *) (& fields->f_imm24));
      break;
    case UBICOM32_OPERAND_IMM25 :
      errmsg = parse_imm25 (cd, strp, UBICOM32_OPERAND_IMM25, (unsigned long *) (& fields->f_imm25));
      break;
    case UBICOM32_OPERAND_INTERRUPT :
      errmsg = cgen_parse_unsigned_integer (cd, strp, UBICOM32_OPERAND_INTERRUPT, (unsigned long *) (& fields->f_int));
      break;
    case UBICOM32_OPERAND_IREAD :
      errmsg = cgen_parse_unsigned_integer (cd, strp, UBICOM32_OPERAND_IREAD, (unsigned long *) (& junk));
      break;
    case UBICOM32_OPERAND_IRQ_0 :
      errmsg = cgen_parse_unsigned_integer (cd, strp, UBICOM32_OPERAND_IRQ_0, (unsigned long *) (& junk));
      break;
    case UBICOM32_OPERAND_IRQ_1 :
      errmsg = cgen_parse_unsigned_integer (cd, strp, UBICOM32_OPERAND_IRQ_1, (unsigned long *) (& junk));
      break;
    case UBICOM32_OPERAND_LEAI_OFFSET16 :
      errmsg = parse_leai_offset16 (cd, strp, UBICOM32_OPERAND_LEAI_OFFSET16, (long *) (& fields->f_leai_o16));
      break;
    case UBICOM32_OPERAND_MACHI :
      errmsg = cgen_parse_unsigned_integer (cd, strp, UBICOM32_OPERAND_MACHI, (unsigned long *) (& junk));
      break;
    case UBICOM32_OPERAND_MACLO :
      errmsg = cgen_parse_unsigned_integer (cd, strp, UBICOM32_OPERAND_MACLO, (unsigned long *) (& junk));
      break;
    case UBICOM32_OPERAND_OFFSET16 :
      errmsg = parse_offset16 (cd, strp, UBICOM32_OPERAND_OFFSET16, (long *) (& fields->f_o16));
      break;
    case UBICOM32_OPERAND_OFFSET21 :
      {
        bfd_vma value = 0;
        errmsg = parse_offset21 (cd, strp, UBICOM32_OPERAND_OFFSET21, 0, NULL,  & value);
        fields->f_o21 = value;
      }
      break;
    case UBICOM32_OPERAND_OFFSET24 :
      {
        bfd_vma value = 0;
        errmsg = cgen_parse_address (cd, strp, UBICOM32_OPERAND_OFFSET24, 0, NULL,  & value);
        fields->f_o24 = value;
      }
      break;
    case UBICOM32_OPERAND_OPC1 :
      errmsg = cgen_parse_unsigned_integer (cd, strp, UBICOM32_OPERAND_OPC1, (unsigned long *) (& fields->f_op1));
      break;
    case UBICOM32_OPERAND_OPC2 :
      errmsg = cgen_parse_unsigned_integer (cd, strp, UBICOM32_OPERAND_OPC2, (unsigned long *) (& fields->f_op2));
      break;
    case UBICOM32_OPERAND_PDEC_S1_IMM7_4 :
      errmsg = parse_pdec_imm7_4_s (cd, strp, UBICOM32_OPERAND_PDEC_S1_IMM7_4, (unsigned long *) (& fields->f_s1_imm7_4));
      break;
    case UBICOM32_OPERAND_S1_AN :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_addr_names, & fields->f_s1_An);
      break;
    case UBICOM32_OPERAND_S1_DIRECT_ADDR :
      errmsg = parse_s1_direct_addr (cd, strp, UBICOM32_OPERAND_S1_DIRECT_ADDR, (unsigned long *) (& fields->f_s1_direct));
      break;
    case UBICOM32_OPERAND_S1_I4_1 :
      errmsg = parse_imm4_1 (cd, strp, UBICOM32_OPERAND_S1_I4_1, (long *) (& fields->f_s1_i4_1));
      break;
    case UBICOM32_OPERAND_S1_I4_2 :
      errmsg = parse_imm4_2 (cd, strp, UBICOM32_OPERAND_S1_I4_2, (long *) (& fields->f_s1_i4_2));
      break;
    case UBICOM32_OPERAND_S1_I4_4 :
      errmsg = parse_imm4_4 (cd, strp, UBICOM32_OPERAND_S1_I4_4, (long *) (& fields->f_s1_i4_4));
      break;
    case UBICOM32_OPERAND_S1_IMM7_1 :
      errmsg = parse_imm7_1_s (cd, strp, UBICOM32_OPERAND_S1_IMM7_1, (unsigned long *) (& fields->f_s1_imm7_1));
      break;
    case UBICOM32_OPERAND_S1_IMM7_2 :
      errmsg = parse_imm7_2_s (cd, strp, UBICOM32_OPERAND_S1_IMM7_2, (unsigned long *) (& fields->f_s1_imm7_2));
      break;
    case UBICOM32_OPERAND_S1_IMM7_4 :
      errmsg = parse_imm7_4_s (cd, strp, UBICOM32_OPERAND_S1_IMM7_4, (unsigned long *) (& fields->f_s1_imm7_4));
      break;
    case UBICOM32_OPERAND_S1_IMM8 :
      errmsg = parse_imm8 (cd, strp, UBICOM32_OPERAND_S1_IMM8, (long *) (& fields->f_s1_imm8));
      break;
    case UBICOM32_OPERAND_S1_R :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_data_names, & fields->f_s1_r);
      break;
    case UBICOM32_OPERAND_S2 :
      errmsg = cgen_parse_keyword (cd, strp, & ubicom32_cgen_opval_data_names, & fields->f_s2);
      break;
    case UBICOM32_OPERAND_SRC3 :
      errmsg = cgen_parse_unsigned_integer (cd, strp, UBICOM32_OPERAND_SRC3, (unsigned long *) (& junk));
      break;
    case UBICOM32_OPERAND_X_BIT26 :
      errmsg = cgen_parse_unsigned_integer (cd, strp, UBICOM32_OPERAND_X_BIT26, (unsigned long *) (& fields->f_bit26));
      break;
    case UBICOM32_OPERAND_X_D :
      errmsg = cgen_parse_unsigned_integer (cd, strp, UBICOM32_OPERAND_X_D, (unsigned long *) (& fields->f_d));
      break;
    case UBICOM32_OPERAND_X_DN :
      errmsg = cgen_parse_unsigned_integer (cd, strp, UBICOM32_OPERAND_X_DN, (unsigned long *) (& fields->f_Dn));
      break;
    case UBICOM32_OPERAND_X_OP2 :
      errmsg = cgen_parse_unsigned_integer (cd, strp, UBICOM32_OPERAND_X_OP2, (unsigned long *) (& fields->f_op2));
      break;
    case UBICOM32_OPERAND_X_S1 :
      errmsg = cgen_parse_unsigned_integer (cd, strp, UBICOM32_OPERAND_X_S1, (unsigned long *) (& fields->f_s1));
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while parsing.\n"), opindex);
      abort ();
  }

  return errmsg;
}

cgen_parse_fn * const ubicom32_cgen_parse_handlers[] = 
{
  parse_insn_normal,
};

void
ubicom32_cgen_init_asm (CGEN_CPU_DESC cd)
{
  ubicom32_cgen_init_opcode_table (cd);
  ubicom32_cgen_init_ibld_table (cd);
  cd->parse_handlers = & ubicom32_cgen_parse_handlers[0];
  cd->parse_operand = ubicom32_cgen_parse_operand;
#ifdef CGEN_ASM_INIT_HOOK
CGEN_ASM_INIT_HOOK
#endif
}



/* Regex construction routine.

   This translates an opcode syntax string into a regex string,
   by replacing any non-character syntax element (such as an
   opcode) with the pattern '.*'

   It then compiles the regex and stores it in the opcode, for
   later use by ubicom32_cgen_assemble_insn

   Returns NULL for success, an error message for failure.  */

char * 
ubicom32_cgen_build_insn_regex (CGEN_INSN *insn)
{  
  CGEN_OPCODE *opc = (CGEN_OPCODE *) CGEN_INSN_OPCODE (insn);
  const char *mnem = CGEN_INSN_MNEMONIC (insn);
  char rxbuf[CGEN_MAX_RX_ELEMENTS];
  char *rx = rxbuf;
  const CGEN_SYNTAX_CHAR_TYPE *syn;
  int reg_err;

  syn = CGEN_SYNTAX_STRING (CGEN_OPCODE_SYNTAX (opc));

  /* Mnemonics come first in the syntax string.  */
  if (! CGEN_SYNTAX_MNEMONIC_P (* syn))
    return _("missing mnemonic in syntax string");
  ++syn;

  /* Generate a case sensitive regular expression that emulates case
     insensitive matching in the "C" locale.  We cannot generate a case
     insensitive regular expression because in Turkish locales, 'i' and 'I'
     are not equal modulo case conversion.  */

  /* Copy the literal mnemonic out of the insn.  */
  for (; *mnem; mnem++)
    {
      char c = *mnem;

      if (ISALPHA (c))
	{
	  *rx++ = '[';
	  *rx++ = TOLOWER (c);
	  *rx++ = TOUPPER (c);
	  *rx++ = ']';
	}
      else
	*rx++ = c;
    }

  /* Copy any remaining literals from the syntax string into the rx.  */
  for(; * syn != 0 && rx <= rxbuf + (CGEN_MAX_RX_ELEMENTS - 7 - 4); ++syn)
    {
      if (CGEN_SYNTAX_CHAR_P (* syn)) 
	{
	  char c = CGEN_SYNTAX_CHAR (* syn);

	  switch (c) 
	    {
	      /* Escape any regex metacharacters in the syntax.  */
	    case '.': case '[': case '\\': 
	    case '*': case '^': case '$': 

#ifdef CGEN_ESCAPE_EXTENDED_REGEX
	    case '?': case '{': case '}': 
	    case '(': case ')': case '*':
	    case '|': case '+': case ']':
#endif
	      *rx++ = '\\';
	      *rx++ = c;
	      break;

	    default:
	      if (ISALPHA (c))
		{
		  *rx++ = '[';
		  *rx++ = TOLOWER (c);
		  *rx++ = TOUPPER (c);
		  *rx++ = ']';
		}
	      else
		*rx++ = c;
	      break;
	    }
	}
      else
	{
	  /* Replace non-syntax fields with globs.  */
	  *rx++ = '.';
	  *rx++ = '*';
	}
    }

  /* Trailing whitespace ok.  */
  * rx++ = '['; 
  * rx++ = ' '; 
  * rx++ = '\t'; 
  * rx++ = ']'; 
  * rx++ = '*'; 

  /* But anchor it after that.  */
  * rx++ = '$'; 
  * rx = '\0';

  CGEN_INSN_RX (insn) = xmalloc (sizeof (regex_t));
  reg_err = regcomp ((regex_t *) CGEN_INSN_RX (insn), rxbuf, REG_NOSUB);

  if (reg_err == 0) 
    return NULL;
  else
    {
      static char msg[80];

      regerror (reg_err, (regex_t *) CGEN_INSN_RX (insn), msg, 80);
      regfree ((regex_t *) CGEN_INSN_RX (insn));
      free (CGEN_INSN_RX (insn));
      (CGEN_INSN_RX (insn)) = NULL;
      return msg;
    }
}


/* Default insn parser.

   The syntax string is scanned and operands are parsed and stored in FIELDS.
   Relocs are queued as we go via other callbacks.

   ??? Note that this is currently an all-or-nothing parser.  If we fail to
   parse the instruction, we return 0 and the caller will start over from
   the beginning.  Backtracking will be necessary in parsing subexpressions,
   but that can be handled there.  Not handling backtracking here may get
   expensive in the case of the m68k.  Deal with later.

   Returns NULL for success, an error message for failure.  */

static const char *
parse_insn_normal (CGEN_CPU_DESC cd,
		   const CGEN_INSN *insn,
		   const char **strp,
		   CGEN_FIELDS *fields)
{
  /* ??? Runtime added insns not handled yet.  */
  const CGEN_SYNTAX *syntax = CGEN_INSN_SYNTAX (insn);
  const char *str = *strp;
  const char *errmsg;
  const char *p;
  const CGEN_SYNTAX_CHAR_TYPE * syn;
#ifdef CGEN_MNEMONIC_OPERANDS
  /* FIXME: wip */
  int past_opcode_p;
#endif

  /* For now we assume the mnemonic is first (there are no leading operands).
     We can parse it without needing to set up operand parsing.
     GAS's input scrubber will ensure mnemonics are lowercase, but we may
     not be called from GAS.  */
  p = CGEN_INSN_MNEMONIC (insn);
  while (*p && TOLOWER (*p) == TOLOWER (*str))
    ++p, ++str;

  if (* p)
    return _("unrecognized instruction");

#ifndef CGEN_MNEMONIC_OPERANDS
  if (* str && ! ISSPACE (* str))
    return _("unrecognized instruction");
#endif

  CGEN_INIT_PARSE (cd);
  cgen_init_parse_operand (cd);
#ifdef CGEN_MNEMONIC_OPERANDS
  past_opcode_p = 0;
#endif

  /* We don't check for (*str != '\0') here because we want to parse
     any trailing fake arguments in the syntax string.  */
  syn = CGEN_SYNTAX_STRING (syntax);

  /* Mnemonics come first for now, ensure valid string.  */
  if (! CGEN_SYNTAX_MNEMONIC_P (* syn))
    abort ();

  ++syn;

  while (* syn != 0)
    {
      /* Non operand chars must match exactly.  */
      if (CGEN_SYNTAX_CHAR_P (* syn))
	{
	  /* FIXME: While we allow for non-GAS callers above, we assume the
	     first char after the mnemonic part is a space.  */
	  /* FIXME: We also take inappropriate advantage of the fact that
	     GAS's input scrubber will remove extraneous blanks.  */
	  if (TOLOWER (*str) == TOLOWER (CGEN_SYNTAX_CHAR (* syn)))
	    {
#ifdef CGEN_MNEMONIC_OPERANDS
	      if (CGEN_SYNTAX_CHAR(* syn) == ' ')
		past_opcode_p = 1;
#endif
	      ++ syn;
	      ++ str;
	    }
	  else if (*str)
	    {
	      /* Syntax char didn't match.  Can't be this insn.  */
	      static char msg [80];

	      /* xgettext:c-format */
	      sprintf (msg, _("syntax error (expected char `%c', found `%c')"),
		       CGEN_SYNTAX_CHAR(*syn), *str);
	      return msg;
	    }
	  else
	    {
	      /* Ran out of input.  */
	      static char msg [80];

	      /* xgettext:c-format */
	      sprintf (msg, _("syntax error (expected char `%c', found end of instruction)"),
		       CGEN_SYNTAX_CHAR(*syn));
	      return msg;
	    }
	  continue;
	}

      /* We have an operand of some sort.  */
      errmsg = cd->parse_operand (cd, CGEN_SYNTAX_FIELD (*syn),
					  &str, fields);
      if (errmsg)
	return errmsg;

      /* Done with this operand, continue with next one.  */
      ++ syn;
    }

  /* If we're at the end of the syntax string, we're done.  */
  if (* syn == 0)
    {
      /* FIXME: For the moment we assume a valid `str' can only contain
	 blanks now.  IE: We needn't try again with a longer version of
	 the insn and it is assumed that longer versions of insns appear
	 before shorter ones (eg: lsr r2,r3,1 vs lsr r2,r3).  */
      while (ISSPACE (* str))
	++ str;

      if (* str != '\0')
	return _("junk at end of line"); /* FIXME: would like to include `str' */

      return NULL;
    }

  /* We couldn't parse it.  */
  return _("unrecognized instruction");
}

/* Main entry point.
   This routine is called for each instruction to be assembled.
   STR points to the insn to be assembled.
   We assume all necessary tables have been initialized.
   The assembled instruction, less any fixups, is stored in BUF.
   Remember that if CGEN_INT_INSN_P then BUF is an int and thus the value
   still needs to be converted to target byte order, otherwise BUF is an array
   of bytes in target byte order.
   The result is a pointer to the insn's entry in the opcode table,
   or NULL if an error occured (an error message will have already been
   printed).

   Note that when processing (non-alias) macro-insns,
   this function recurses.

   ??? It's possible to make this cpu-independent.
   One would have to deal with a few minor things.
   At this point in time doing so would be more of a curiosity than useful
   [for example this file isn't _that_ big], but keeping the possibility in
   mind helps keep the design clean.  */

const CGEN_INSN *
ubicom32_cgen_assemble_insn (CGEN_CPU_DESC cd,
			   const char *str,
			   CGEN_FIELDS *fields,
			   CGEN_INSN_BYTES_PTR buf,
			   char **errmsg)
{
  const char *start;
  CGEN_INSN_LIST *ilist;
  const char *parse_errmsg = NULL;
  const char *insert_errmsg = NULL;
  int recognized_mnemonic = 0;

  /* Skip leading white space.  */
  while (ISSPACE (* str))
    ++ str;

  /* The instructions are stored in hashed lists.
     Get the first in the list.  */
  ilist = CGEN_ASM_LOOKUP_INSN (cd, str);

  /* Keep looking until we find a match.  */
  start = str;
  for ( ; ilist != NULL ; ilist = CGEN_ASM_NEXT_INSN (ilist))
    {
      const CGEN_INSN *insn = ilist->insn;
      recognized_mnemonic = 1;

#ifdef CGEN_VALIDATE_INSN_SUPPORTED 
      /* Not usually needed as unsupported opcodes
	 shouldn't be in the hash lists.  */
      /* Is this insn supported by the selected cpu?  */
      if (! ubicom32_cgen_insn_supported (cd, insn))
	continue;
#endif
      /* If the RELAXED attribute is set, this is an insn that shouldn't be
	 chosen immediately.  Instead, it is used during assembler/linker
	 relaxation if possible.  */
      if (CGEN_INSN_ATTR_VALUE (insn, CGEN_INSN_RELAXED) != 0)
	continue;

      str = start;

      /* Skip this insn if str doesn't look right lexically.  */
      if (CGEN_INSN_RX (insn) != NULL &&
	  regexec ((regex_t *) CGEN_INSN_RX (insn), str, 0, NULL, 0) == REG_NOMATCH)
	continue;

      /* Allow parse/insert handlers to obtain length of insn.  */
      CGEN_FIELDS_BITSIZE (fields) = CGEN_INSN_BITSIZE (insn);

      parse_errmsg = CGEN_PARSE_FN (cd, insn) (cd, insn, & str, fields);
      if (parse_errmsg != NULL)
	continue;

      /* ??? 0 is passed for `pc'.  */
      insert_errmsg = CGEN_INSERT_FN (cd, insn) (cd, insn, fields, buf,
						 (bfd_vma) 0);
      if (insert_errmsg != NULL)
        continue;

      /* It is up to the caller to actually output the insn and any
         queued relocs.  */
      return insn;
    }

  {
    static char errbuf[150];
#ifdef CGEN_VERBOSE_ASSEMBLER_ERRORS
    const char *tmp_errmsg;

    /* If requesting verbose error messages, use insert_errmsg.
       Failing that, use parse_errmsg.  */
    tmp_errmsg = (insert_errmsg ? insert_errmsg :
		  parse_errmsg ? parse_errmsg :
		  recognized_mnemonic ?
		  _("unrecognized form of instruction") :
		  _("unrecognized instruction"));

    if (strlen (start) > 50)
      /* xgettext:c-format */
      sprintf (errbuf, "%s `%.50s...'", tmp_errmsg, start);
    else 
      /* xgettext:c-format */
      sprintf (errbuf, "%s `%.50s'", tmp_errmsg, start);
#else
    if (strlen (start) > 50)
      /* xgettext:c-format */
      sprintf (errbuf, _("bad instruction `%.50s...'"), start);
    else 
      /* xgettext:c-format */
      sprintf (errbuf, _("bad instruction `%.50s'"), start);
#endif
      
    *errmsg = errbuf;
    return NULL;
  }
}
