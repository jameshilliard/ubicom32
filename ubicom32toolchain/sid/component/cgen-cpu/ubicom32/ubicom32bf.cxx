// ubixom32bf.cxx - Implementations of hand-written functions for the Ubicom32
// simulator. -*- C++ -*-

// Copyright (C) 1999, 2000, 2001, 2002 Red Hat Inc.
// This file is part of SID and is licensed under the GPL.
// See the file COPYING.SID for conditions for redistribution.

#include "ubicom32bf.h"
#include <fenv.h>
#include <math.h>

#define FE_FLTPREC       0x000
#define FE_INVALIDPREC   0x100
#define FE_DBLPREC       0x200
#define FE_LDBLPREC      0x300

/*
  Set the precision to prec if it is a valid
  floating point precision macro.
  Returns 1 if precision set, 0 otherwise.
  */
int fesetprecision(int prec)
{
  unsigned short cw;
  asm volatile ("fnstcw %0" : "=m" (cw));
  if ( !(prec & ~FE_LDBLPREC) && (prec != FE_INVALIDPREC) )
    {
      cw = (cw & ~FE_LDBLPREC) | (prec & FE_LDBLPREC);
      //cw &= ~0x3f;
      //cw |= 0x2;
      asm volatile ("fldcw %0" : : "m" (cw));
      return 1;
    }
  else
    return 0;
}

using namespace std;
using namespace sid;
using namespace sidutil;
using namespace ubicom32bf;
using namespace ubicom32;
using namespace cgen;

struct ubicom32_regs
{
  char *name;			// Register name
  int  index;			// GDB index
  unsigned int addr;		// Direct space address for register;
};

struct ubicom32_regs ubicom32v2_regs[] = {
  // Mars registers
  /* General data.  */
  { "d0", 0, 0x0 },
  { "d1", 1, 0x4 },
  { "d2", 2, 0x8 },
  { "d3", 3, 0xc },
  { "d4", 4, 0x10 },
  { "d5", 5, 0x14 },
  { "d6", 6, 0x18 },
  { "d7", 7, 0x1c },
  { "d8", 8, 0x20 },
  { "d9", 9, 0x24 },
  { "d10", 10, 0x28 },
  { "d11", 11, 0x2c },
  { "d12", 12, 0x30 },
  { "d13", 13, 0x34 },
  { "d14", 14, 0x38 },
  { "d15", 15, 0x3c },

 /* General address.  */
  { "a0", 16, 0x80 },
  { "a1", 17, 0x84 },
  { "a2", 18, 0x88 },
  { "a3", 19, 0x8c },
  { "a4", 20, 0x90 },
  { "a5", 21, 0x94 },
  { "a6", 22, 0x98 },
  /* Stack pointer.  */
  { "sp", 23, 0x9c },
  /* High 16 bits of multiply accumulator.  */
  { "mac_hi", 24, 0xa0 },
  /* Low 32 bits of multiply accumulator.  */
  { "mac_lo", 25, 0xa4 },
  /* Rounded and clipped S16.15 image of multiply accumulator.  */
  { "mac_rc16", 26, 0xa8 },
  /* 3rd source operand.  */
  { "source_3", 27,0xac },
  /* Current thread's execution count. */
  { "context_cnt", 28, 0xb0 },
  /* Control/status.  */
  { "csr", 29, 0xb4 },
  /* Read-only status.  */
  { "rosr", 30, 0xb8 },
  /* Iread output.  */
  { "iread_data", 31, 0xbc },
  /* Low 32 bits of interrupt mask.  */
  { "int_mask0", 32, 0xc0 },
  /* High 32 bits of interrupt mask.  */
  { "int_mask1", 33, 0xc4 },
  /* Program counter.  */
  { "pc", 34, 0xd0 },
  /* Chip identification.  */
  { "chip_id", 35, 0x100 },
  /* Low 32 bits of interrupt status.  */
  { "int_stat0", 36, 0x104 },
  /* High 32 bits of interrupt status.  */
  { "int_stat1", 37, 0x108 },
  /* Set bits in int_stat0.  */
  { "int_set0", 38, 0x114 },
  /* Set bits in int_stat1.  */
  { "int_set1", 39, 0x118 },
  /* Clear bits in int_stat0.  */
  { "int_clr0", 40, 0x124 },
  /* Clear bits in int_stat1.  */
  { "int_clr1", 41, 0x128 },
  /* Global control.  */
  { "global_ctrl", 42, 0x134 },
  /* Set bits in mt_active.  */
  { "mt_active_set", 43, 0x13c },
  /* Clear bits in mt_active.  */
  { "mt_active_clr", 44, 0x140 },
  /* Threads active status.  */
  { "mt_active", 45, 0x138 },
  /* Set bits in mt_dbg_active.  */
  { "mt_dbg_active_set", 46, 0x148 },
  /* Debugging threads active status.  */
  { "mt_dbg_active", 47, 0x144 },
  /* Threads enabled.  */
  { "mt_en", 48, 0x14C },
  /* Thread priorities.  */
  { "mt_pri", 49, 0x150 },
  /* Thread scheduling policies.  */
  { "mt_sched", 50, 0x154 },
  /* Clear bits in mt_break.  */
  { "mt_break_clr", 51, 0x15C },
  /* Threads stopped on a break condition.  */
  { "mt_break", 52, 0x158 },
  /* Single-step threads.  */
  { "mt_single_step", 53, 0x160 },
  /* Threads with minimum delay scheduling constraing.  */
  { "mt_min_del_en", 54, 0x164 },

  /* Debugging threads active status clear register.  */
  { "mt_dbg_active_clr", 55, 0x17c },
  /* Parity Error Address.  */
  { "perr_addr", 56, 0x16c },
  /* Thread that wrote to dcapt.  */
  { "dcapt_thread", 57, 0x178 },
  /* PC at which thread wrote to dcapt.  */
  { "dcapt_pc", 58, 0x174 },
  /* Data capture address.  */
  { "dcapt", 59, 0x170 },
  /* scratchpad registers */
  { "scratchpad0", 60, 0x180 },
  { "scratchpad1", 61, 0x184 },
  { "scratchpad2", 62, 0x188 },
  { "scratchpad3", 63, 0x18c },

  /* High 16 bits of multiply accumulator1.  */
  { "acc1_hi", 64, 0xe0 },
  /* Low 32 bits of multiply accumulator1.  */
  { "acc1_lo", 65, 0xe4 },
  /* Hidden register to change threads in SID.  */
};

struct ubicom32_regs ubicom32v3_regs[] = {
  // Mars registers
  /* General data.  */
  { "d0", 0, 0x0 },
  { "d1", 1, 0x4 },
  { "d2", 2, 0x8 },
  { "d3", 3, 0xc },
  { "d4", 4, 0x10 },
  { "d5", 5, 0x14 },
  { "d6", 6, 0x18 },
  { "d7", 7, 0x1c },
  { "d8", 8, 0x20 },
  { "d9", 9, 0x24 },
  { "d10", 10, 0x28 },
  { "d11", 11, 0x2c },
  { "d12", 12, 0x30 },
  { "d13", 13, 0x34 },
  { "d14", 14, 0x38 },
  { "d15", 15, 0x3c },
 /* General address.  */
  { "a0", 16, 0x80 },
  { "a1", 17, 0x84 },
  { "a2", 18, 0x88 },
  { "a3", 19, 0x8c },
  { "a4", 20, 0x90 },
  { "a5", 21, 0x94 },
  { "a6", 22, 0x98 },
  /* Stack pointer.  */
  { "sp", 23, 0x9c },
  /* High 16 bits of multiply accumulator.  */
  { "mac_hi", 24, 0xa0 },
  /* Low 32 bits of multiply accumulator.  */
  { "mac_lo", 25, 0xa4 },
  /* Rounded and clipped S16.15 image of multiply accumulator.  */
  { "mac_rc16", 26, 0xa8 },
  /* 3rd source operand.  */
  { "source_3", 27,0xac },
  /* Current thread's execution count. */
  { "context_cnt", 28, 0xb0 },
  /* Control/status.  */
  { "csr", 29, 0xb4 },
  /* Read-only status.  */
  { "rosr", 30, 0xb8 },
  /* Iread output.  */
  { "iread_data", 31, 0xbc },
  /* Low 32 bits of interrupt mask.  */
  { "int_mask0", 32, 0xc0 },
  /* High 32 bits of interrupt mask.  */
  { "int_mask1", 33, 0xc4 },
  /* Program counter.  */
  { "pc", 34, 0xd0 },
  /* Chip identification.  */

  { "trap_cause", 35, 0xd4 },
  { "acc1_hi", 36, 0xd8 },
  { "acc1_lo", 37, 0xdc },
  { "previous_pc", 38, 0xe0 },

  /* 0xd0 - 0xff  Reserved */
  /* Chip identification.  */
  { "chip_id", 39, 0x100 },
  /* Low 32 bits of interrupt status.  */
  { "int_stat0", 40, 0x104 },
  /* High 32 bits of interrupt status.  */
  { "int_stat1", 41, 0x108 },

  /* 0x10c - 0x113 Reserved */
  /* Set bits in int_stat0.  */
  { "int_set0", 42, 0x114 },
  /* Set bits in int_stat1.  */
  { "int_set1", 43, 0x118 },

  /* 0x11c - 0x123 Reserved */
  /* Clear bits in int_stat0.  */
  { "int_clr0", 44, 0x124 },
  /* Clear bits in int_stat1.  */
  { "int_clr1", 45, 0x128 },
  
  /* 0x12c - 0x133 Reserved */
  /* Global control.  */
  { "global_ctrl", 46, 0x134 },
  /* Threads active status.  */
  { "mt_active", 47, 0x138 },
  /* Set bits in mt_active.  */
  { "mt_active_set", 48, 0x13c },
  /* Clear bits in mt_active.  */
  { "mt_active_clr", 49, 0x140 },
  /* Debugging threads active status.  */
  { "mt_dbg_active", 50, 0x144 },
  /* Set bits in mt_dbg_active.  */
  { "mt_dbg_active_set", 51, 0x148 },
  /* Threads enabled.  */
  { "mt_en", 52, 0x14C },
  /* Thread priorities.  */
  { "mt_pri", 53, 0x150 },
  /* Thread scheduling policies.  */
  { "mt_sched", 54, 0x154 },
  /* Threads stopped on a break condition.  */
  { "mt_break", 55, 0x158 },
  /* Clear bits in mt_break.  */
  { "mt_break_clr", 56, 0x15C },
  /* Single-step threads.  */
  { "mt_single_step", 57, 0x160 },
  /* Threads with minimum delay scheduling constraing.  */
  { "mt_min_del_en", 58, 0x164 },
  { "mt_break_set", 59, 0x168 },

  /* 0x16c - 0x16f reserved */
  /* Data capture address.  */
  { "dcapt", 60, 0x170 },

  /* 0x174 - 0x17b reserved */
  /* Debugging threads active status clear register.  */
  { "mt_dbg_active_clr", 61, 0x17c },
  /* scratchpad registers */
  { "scratchpad0", 62, 0x180 },
  { "scratchpad1", 63, 0x184 },
  { "scratchpad2", 64, 0x188 },
  { "scratchpad3", 65, 0x18c },

  /* 0x190 - 0x19f Reserved */
  { "chip_cfg", 66, 0x1a0 },
  { "mt_i_blocked", 67, 0x1a4 },
  { "mt_d_blocked", 68, 0x1a8 },
  { "mt_i_blocked_set", 69, 0x1ac },
  { "mt_d_blocked_set", 70, 0x1b0 },
  { "mt_blocked_clr", 71, 0x1b4 },
  { "mt_trap_en", 72, 0x1b8 },
  { "mt_trap", 73, 0x1bc },
  { "mt_trap_set", 74, 0x1c0 },
  { "mt_trap_clr", 75, 0x1c4 },

  /* 0x1c8-0x1FF Reserved */
  { "i_range0_hi", 76, 0x200 },
  { "i_range1_hi", 77, 0x204 },
  { "i_range2_hi", 78, 0x208 },

  /* 0x20c-0x21f Reserved */
  { "i_range0_lo", 79, 0x220 },
  { "i_range1_lo", 80, 0x224 },
  { "i_range2_lo", 81, 0x228 },

  /* 0x22c-0x23f Reserved */
  { "i_range0_en", 82, 0x240 },
  { "i_range1_en", 83, 0x244 },
  { "i_range2_en", 84, 0x248 },
  
  /* 0x24c-0x25f Reserved */
  { "d_range0_hi", 85, 0x260 },
  { "d_range1_hi", 86, 0x264 },
  { "d_range2_hi", 87, 0x268 },
  { "d_range3_hi", 88, 0x26c },

  /* 0x270-0x27f Reserved */
  { "d_range0_lo", 89, 0x280 },
  { "d_range1_lo", 90, 0x284 },
  { "d_range2_lo", 91, 0x288 },
  { "d_range3_lo", 92, 0x28c },
  
  /* 0x290-0x29f Reserved */
  { "d_range0_en", 93, 0x2a0 },
  { "d_range1_en", 94, 0x2a4 },
  { "d_range2_en", 95, 0x2a8 },
  { "d_range3_en", 96, 0x2ac },
};

struct ubicom32_regs ubicom32v4_regs[] = {
  // Mars registers
  /* General data.  */
  { "d0", 0, 0x0 },
  { "d1", 1, 0x4 },
  { "d2", 2, 0x8 },
  { "d3", 3, 0xc },
  { "d4", 4, 0x10 },
  { "d5", 5, 0x14 },
  { "d6", 6, 0x18 },
  { "d7", 7, 0x1c },
  { "d8", 8, 0x20 },
  { "d9", 9, 0x24 },
  { "d10", 10, 0x28 },
  { "d11", 11, 0x2c },
  { "d12", 12, 0x30 },
  { "d13", 13, 0x34 },
  { "d14", 14, 0x38 },
  { "d15", 15, 0x3c },
 /* General address.  */
  { "a0", 16, 0x80 },
  { "a1", 17, 0x84 },
  { "a2", 18, 0x88 },
  { "a3", 19, 0x8c },
  { "a4", 20, 0x90 },
  { "a5", 21, 0x94 },
  { "a6", 22, 0x98 },
  /* Stack pointer.  */
  { "sp", 23, 0x9c },
  /* High 16 bits of multiply accumulator.  */
  { "mac_hi", 24, 0xa0 },
  /* Low 32 bits of multiply accumulator.  */
  { "mac_lo", 25, 0xa4 },
  /* Rounded and clipped S16.15 image of multiply accumulator.  */
  { "mac_rc16", 26, 0xa8 },
  /* 3rd source operand.  */
  { "source_3", 27,0xac },
  /* Current thread's execution count. */
  { "context_cnt", 28, 0xb0 },
  /* Control/status.  */
  { "csr", 29, 0xb4 },
  /* Read-only status.  */
  { "rosr", 30, 0xb8 },
  /* Iread output.  */
  { "iread_data", 31, 0xbc },
  /* Low 32 bits of interrupt mask.  */
  { "int_mask0", 32, 0xc0 },
  /* High 32 bits of interrupt mask.  */
  { "int_mask1", 33, 0xc4 },
  /* Program counter.  */
  { "pc", 34, 0xd0 },
  /* Chip identification.  */

  { "trap_cause", 35, 0xd4 },
  { "acc1_hi", 36, 0xd8 },
  { "acc1_lo", 37, 0xdc },
  { "previous_pc", 38, 0xe0 },

  /* 0xd0 - 0xff  Reserved */
  /* Chip identification.  */
  { "chip_id", 39, 0x100 },
  /* Low 32 bits of interrupt status.  */
  { "int_stat0", 40, 0x104 },
  /* High 32 bits of interrupt status.  */
  { "int_stat1", 41, 0x108 },

  /* 0x10c - 0x113 Reserved */
  /* Set bits in int_stat0.  */
  { "int_set0", 42, 0x114 },
  /* Set bits in int_stat1.  */
  { "int_set1", 43, 0x118 },

  /* 0x11c - 0x123 Reserved */
  /* Clear bits in int_stat0.  */
  { "int_clr0", 44, 0x124 },
  /* Clear bits in int_stat1.  */
  { "int_clr1", 45, 0x128 },
  
  /* 0x12c - 0x133 Reserved */
  /* Global control.  */
  { "global_ctrl", 46, 0x134 },
  /* Threads active status.  */
  { "mt_active", 47, 0x138 },
  /* Set bits in mt_active.  */
  { "mt_active_set", 48, 0x13c },
  /* Clear bits in mt_active.  */
  { "mt_active_clr", 49, 0x140 },
  /* Debugging threads active status.  */
  { "mt_dbg_active", 50, 0x144 },
  /* Set bits in mt_dbg_active.  */
  { "mt_dbg_active_set", 51, 0x148 },
  /* Threads enabled.  */
  { "mt_en", 52, 0x14C },
  /* Thread priorities.  */
  { "mt_pri", 53, 0x150 },
  /* Thread scheduling policies.  */
  { "mt_sched", 54, 0x154 },
  /* Threads stopped on a break condition.  */
  { "mt_break", 55, 0x158 },
  /* Clear bits in mt_break.  */
  { "mt_break_clr", 56, 0x15C },
  /* Single-step threads.  */
  { "mt_single_step", 57, 0x160 },
  /* Threads with minimum delay scheduling constraing.  */
  { "mt_min_del_en", 58, 0x164 },
  { "mt_break_set", 59, 0x168 },

  /* 0x16c - 0x16f reserved */
  /* Data capture address.  */
  { "dcapt", 60, 0x170 },

  /* 0x174 - 0x17b reserved */
  /* Debugging threads active status clear register.  */
  { "mt_dbg_active_clr", 61, 0x17c },
  /* scratchpad registers */
  { "scratchpad0", 62, 0x180 },
  { "scratchpad1", 63, 0x184 },
  { "scratchpad2", 64, 0x188 },
  { "scratchpad3", 65, 0x18c },

  /* 0x190 - 0x19f Reserved */
  { "chip_cfg", 66, 0x1a0 },
  { "mt_i_blocked", 67, 0x1a4 },
  { "mt_d_blocked", 68, 0x1a8 },
  { "mt_i_blocked_set", 69, 0x1ac },
  { "mt_d_blocked_set", 70, 0x1b0 },
  { "mt_blocked_clr", 71, 0x1b4 },
  { "mt_trap_en", 72, 0x1b8 },
  { "mt_trap", 73, 0x1bc },
  { "mt_trap_set", 74, 0x1c0 },
  { "mt_trap_clr", 75, 0x1c4 },

  /* 0x1c8-0x1FF Reserved */
  { "i_range0_hi", 76, 0x200 },
  { "i_range1_hi", 77, 0x204 },
  { "i_range2_hi", 78, 0x208 },
  { "i_range3_hi", 79, 0x20c },

  /* 0x210-0x21f Reserved */
  { "i_range0_lo", 80, 0x220 },
  { "i_range1_lo", 81, 0x224 },
  { "i_range2_lo", 82, 0x228 },
  { "i_range3_lo", 83, 0x22c },

  /* 0x230-0x23f Reserved */
  { "i_range0_en", 84, 0x240 },
  { "i_range1_en", 85, 0x244 },
  { "i_range2_en", 86, 0x248 },
  { "i_range3_en", 87, 0x24c },
  
  /* 0x250-0x25f Reserved */
  { "d_range0_hi", 88, 0x260 },
  { "d_range1_hi", 89, 0x264 },
  { "d_range2_hi", 90, 0x268 },
  { "d_range3_hi", 91, 0x26c },
  { "d_range4_hi", 92, 0x270 },

  /* 0x274-0x27f Reserved */
  { "d_range0_lo", 93, 0x280 },
  { "d_range1_lo", 94, 0x284 },
  { "d_range2_lo", 95, 0x288 },
  { "d_range3_lo", 96, 0x28c },
  { "d_range4_lo", 97, 0x290 },
  
  /* 0x294-0x29f Reserved */
  { "d_range0_en", 98, 0x2a0 },
  { "d_range1_en", 99, 0x2a4 },
  { "d_range2_en", 100, 0x2a8 },
  { "d_range3_en", 101, 0x2ac },
  { "d_range4_en", 102, 0x2b0 },
};

struct ubicom32_regs ubicom32v5_regs[] = {
  /* General data.  */
  { "d0", 0, 0x0 },
  { "d1", 1, 0x4 },
  { "d2", 2, 0x8 },
  { "d3", 3, 0xc },
  { "d4", 4, 0x10 },
  { "d5", 5, 0x14 },
  { "d6", 6, 0x18 },
  { "d7", 7, 0x1c },
  { "d8", 8, 0x20 },
  { "d9", 9, 0x24 },
  { "d10", 10, 0x28 },
  { "d11", 11, 0x2c },
  { "d12", 12, 0x30 },
  { "d13", 13, 0x34 },
  { "d14", 14, 0x38 },
  { "d15", 15, 0x3c },
 /* General address.  */
  { "a0", 16, 0x80 },
  { "a1", 17, 0x84 },
  { "a2", 18, 0x88 },
  { "a3", 19, 0x8c },
  { "a4", 20, 0x90 },
  { "a5", 21, 0x94 },
  { "a6", 22, 0x98 },
  /* Stack pointer.  */
  { "sp", 23, 0x9c },
  /* High 16 bits of multiply accumulator.  */
  { "mac_hi", 24, 0xa0 },
  /* Low 32 bits of multiply accumulator.  */
  { "mac_lo", 25, 0xa4 },
  /* Rounded and clipped S16.15 image of multiply accumulator.  */
  { "mac_rc16", 26, 0xa8 },
  /* 3rd source operand.  */
  { "source_3", 27,0xac },
  /* Current thread's execution count. */
  { "context_cnt", 28, 0xb0 },
  /* Control/status.  */
  { "csr", 29, 0xb4 },
  /* Read-only status.  */
  { "rosr", 30, 0xb8 },
  /* Iread output.  */
  { "iread_data", 31, 0xbc },
  /* Low 32 bits of interrupt mask.  */
  { "int_mask0", 32, 0xc0 },
  /* High 32 bits of interrupt mask.  */
  { "int_mask1", 33, 0xc4 },
  { "int_mask2", 34, 0xc8 },

  /* 0xc8 - 0xcf Reserved */
  /* Program counter.  */
  { "pc", 35, 0xd0 },
  { "trap_cause", 36, 0xd4 },
  { "acc1_hi", 37, 0xd8 },
  { "acc1_lo", 38, 0xdc },
  { "previous_pc", 39, 0xe0 },
  { "ucsr", 40, 0xe4 },

  /* 0xd0 - 0xff  Reserved */
  /* Chip identification.  */
  { "chip_id", 41, 0x100 },
  /* Low 32 bits of interrupt status.  */
  { "int_stat0", 42, 0x104 },
  /* High 32 bits of interrupt status.  */
  { "int_stat1", 43, 0x108 },
  { "int_stat2", 44, 0x10c },

  /* 0x110 - 0x113 Reserved */
  /* Set bits in int_stat0.  */
  { "int_set0", 45, 0x114 },
  /* Set bits in int_stat1.  */
  { "int_set1", 46, 0x118 },
  { "int_set2", 47, 0x11c },

  /* 0x120 - 0x123 Reserved */
  /* Clear bits in int_stat0.  */
  { "int_clr0", 48, 0x124 },
  /* Clear bits in int_stat1.  */
  { "int_clr1", 49, 0x128 },
  { "int_clr2", 50, 0x12c },

  /* 0x130 - 0x133 Reserved */
  /* Global control.  */
  { "global_ctrl", 51, 0x134 },
  /* Threads active status.  */
  { "mt_active", 52, 0x138 },
  /* Set bits in mt_active.  */
  { "mt_active_set", 53, 0x13c },
  /* Clear bits in mt_active.  */
  { "mt_active_clr", 54, 0x140 },
  /* Debugging threads active status.  */
  { "mt_dbg_active", 55, 0x144 },
  /* Set bits in mt_dbg_active.  */
  { "mt_dbg_active_set", 56, 0x148 },
  /* Threads enabled.  */
  { "mt_en", 57, 0x14C },
  /* Thread priorities.  */
  { "mt_pri", 58, 0x150 },
  /* Thread scheduling policies.  */
  { "mt_sched", 59, 0x154 },
  /* Threads stopped on a break condition.  */
  { "mt_break", 60, 0x158 },
  /* Clear bits in mt_break.  */
  { "mt_break_clr", 61, 0x15C },
  /* Single-step threads.  */
  { "mt_single_step", 62, 0x160 },
  /* Threads with minimum delay scheduling constraing.  */
  { "mt_min_del_en", 63, 0x164 },
  { "mt_break_set", 64, 0x168 },

  /* 0x16c - 0x16f reserved */
  /* Data capture address.  */
  { "dcapt", 65, 0x170 },

  /* 0x174 - 0x17b reserved */
  /* Debugging threads active status clear register.  */
  { "mt_dbg_active_clr", 66, 0x17c },
  /* scratchpad registers */
  { "scratchpad0", 67, 0x180 },
  { "scratchpad1", 68, 0x184 },
  { "scratchpad2", 69, 0x188 },
  { "scratchpad3", 70, 0x18c },
  { "scratchpad4", 71, 0x190 },
  { "scratchpad5", 72, 0x194 },
  { "scratchpad6", 73, 0x198 },

  /* 0x19c - 0x19f Reserved */
  { "chip_cfg", 74, 0x1a0 },
  { "mt_i_blocked", 75, 0x1a4 },
  { "mt_d_blocked", 76, 0x1a8 },
  { "mt_i_blocked_set", 77, 0x1ac },
  { "mt_d_blocked_set", 78, 0x1b0 },
  { "mt_blocked_clr", 79, 0x1b4 },
  { "mt_trap_en", 80, 0x1b8 },
  { "mt_trap", 81, 0x1bc },
  { "mt_trap_set", 82, 0x1c0 },
  { "mt_trap_clr", 83, 0x1c4 },
  { "sep", 84, 0x1c8 },

  /* 0x1cc-0x1FF Reserved */
  { "i_range0_hi", 85, 0x200 },
  { "i_range1_hi", 86, 0x204 },
  { "i_range2_hi", 87, 0x208 },
  { "i_range3_hi", 88, 0x20c },

  /* 0x210-0x21f Reserved */
  { "i_range0_lo", 89, 0x220 },
  { "i_range1_lo", 90, 0x224 },
  { "i_range2_lo", 91, 0x228 },
  { "i_range3_lo", 92, 0x22c },

  /* 0x230-0x23f Reserved */
  { "i_range0_en", 93, 0x240 },
  { "i_range1_en", 94, 0x244 },
  { "i_range2_en", 95, 0x248 },
  { "i_range3_en", 96, 0x24c },

  /* 0x250-0x25f Reserved */
  { "d_range0_hi", 97, 0x260 },
  { "d_range1_hi", 98, 0x264 },
  { "d_range2_hi", 99, 0x268 },
  { "d_range3_hi", 100, 0x26c },
  { "d_range4_hi", 101, 0x270 },
  { "d_range5_hi", 102, 0x274 },

  /* 0x278-0x27f Reserved */
  { "d_range0_lo", 103, 0x280 },
  { "d_range1_lo", 104, 0x284 },
  { "d_range2_lo", 105, 0x288 },
  { "d_range3_lo", 106, 0x28c },
  { "d_range4_lo", 107, 0x290 },
  { "d_range5_lo", 108, 0x294 },

  /* 0x298-0x29f Reserved */
  { "d_range0_en", 109, 0x2a0 },
  { "d_range1_en", 110, 0x2a4 },
  { "d_range2_en", 111, 0x2a8 },
  { "d_range3_en", 112, 0x2ac },
  { "d_range4_en", 113, 0x2b0 },
  { "d_range5_en", 114, 0x2b4 },

  /* 0x2b8 - 0x2bf Reserved */
  { "i_range0_user_en", 115, 0x2c0 },
  { "i_range1_user_en", 116, 0x2c4 },
  { "i_range2_user_en", 117, 0x2c8 },
  { "i_range3_user_en", 118, 0x2cc },

  /* 0x2d0-0x2df Reserved */
  { "d_range0_user_en", 119, 0x2e0 },
  { "d_range1_user_en", 120, 0x2e4 },
  { "d_range2_user_en", 121, 0x2e8 },
  { "d_range3_user_en", 122, 0x2ec },
  { "d_range4_user_en", 123, 0x2f0 },
  { "d_range5_user_en", 124, 0x2f4 },
};

struct ubicom32_regs *u32_regs[] = {
  ubicom32v2_regs,
  ubicom32v3_regs,
  ubicom32v4_regs,
  ubicom32v5_regs,
};
bus::status 
ubicom32bf_mapped_bus::word_write(host_int_4 word_addr, big_int_4 be_mask, big_int_4 data)
{
  USI mask = be_mask;
  USI neg_mask = ~mask;
  USI masked_data = (USI)data & mask;
  USI (ubicom32bf_cpu_cgen::*getter) () const;
  void (ubicom32bf_cpu_cgen::*setter) (USI value);
  USI tmp;
  big_int_4 target_tmp;

  switch (word_addr)
    {
    case ubicom32bf_cpu_cgen::DR_INDEX:
    case ubicom32bf_cpu_cgen::DR_INDEX + 1:
    case ubicom32bf_cpu_cgen::DR_INDEX + 2:
    case ubicom32bf_cpu_cgen::DR_INDEX + 3:
    case ubicom32bf_cpu_cgen::DR_INDEX + 4:
    case ubicom32bf_cpu_cgen::DR_INDEX + 5:
    case ubicom32bf_cpu_cgen::DR_INDEX + 6:
    case ubicom32bf_cpu_cgen::DR_INDEX + 7:
    case ubicom32bf_cpu_cgen::DR_INDEX + 8:
    case ubicom32bf_cpu_cgen::DR_INDEX + 9:
    case ubicom32bf_cpu_cgen::DR_INDEX + 10:
    case ubicom32bf_cpu_cgen::DR_INDEX + 11:
    case ubicom32bf_cpu_cgen::DR_INDEX + 12:
    case ubicom32bf_cpu_cgen::DR_INDEX + 13:
    case ubicom32bf_cpu_cgen::DR_INDEX + 14:
    case ubicom32bf_cpu_cgen::DR_INDEX + 15:

#if 0
    case ubicom32bf_cpu_cgen::DR_INDEX + 16:
    case ubicom32bf_cpu_cgen::DR_INDEX + 17:
    case ubicom32bf_cpu_cgen::DR_INDEX + 18:
    case ubicom32bf_cpu_cgen::DR_INDEX + 19:
    case ubicom32bf_cpu_cgen::DR_INDEX + 20:
    case ubicom32bf_cpu_cgen::DR_INDEX + 21:
    case ubicom32bf_cpu_cgen::DR_INDEX + 22:
    case ubicom32bf_cpu_cgen::DR_INDEX + 23:
    case ubicom32bf_cpu_cgen::DR_INDEX + 24:
    case ubicom32bf_cpu_cgen::DR_INDEX + 25:
    case ubicom32bf_cpu_cgen::DR_INDEX + 26:
    case ubicom32bf_cpu_cgen::DR_INDEX + 27:
    case ubicom32bf_cpu_cgen::DR_INDEX + 28:
    case ubicom32bf_cpu_cgen::DR_INDEX + 29:
    case ubicom32bf_cpu_cgen::DR_INDEX + 30:
    case ubicom32bf_cpu_cgen::DR_INDEX + 31:
#endif
      {
	UINT dr_index = word_addr - ubicom32bf_cpu_cgen::DR_INDEX;
	SI tmp = cpu->get_h_dr (dr_index);
	cpu->set_h_dr_internal (dr_index, (SI)((tmp & neg_mask) | masked_data));
	return bus::ok;
      }
      break;
    case ubicom32bf_cpu_cgen::AR_INDEX:
    case ubicom32bf_cpu_cgen::AR_INDEX + 1:
    case ubicom32bf_cpu_cgen::AR_INDEX + 2:
    case ubicom32bf_cpu_cgen::AR_INDEX + 3:
    case ubicom32bf_cpu_cgen::AR_INDEX + 4:
    case ubicom32bf_cpu_cgen::AR_INDEX + 5:
    case ubicom32bf_cpu_cgen::AR_INDEX + 6:
    case ubicom32bf_cpu_cgen::AR_INDEX + 7:
      {
	UINT ar_index = word_addr - ubicom32bf_cpu_cgen::AR_INDEX;
	USI tmp = cpu->get_h_ar (ar_index);
	cpu->set_h_ar (ar_index, (tmp & neg_mask) | masked_data);
	return bus::ok;
      }
      break;

    case ubicom32bf_cpu_cgen::SCRATCHPAD0_INDEX:
    case ubicom32bf_cpu_cgen::SCRATCHPAD0_INDEX + 1:
    case ubicom32bf_cpu_cgen::SCRATCHPAD0_INDEX + 2:
    case ubicom32bf_cpu_cgen::SCRATCHPAD0_INDEX + 3:
    case ubicom32bf_cpu_cgen::SCRATCHPAD0_INDEX + 4:
    case ubicom32bf_cpu_cgen::SCRATCHPAD0_INDEX + 5:
    case ubicom32bf_cpu_cgen::SCRATCHPAD0_INDEX + 6:
      {
	UINT sp_index = word_addr - ubicom32bf_cpu_cgen::SCRATCHPAD0_INDEX;
	USI tmp = cpu->get_h_scratchpad (sp_index);
	cpu->set_h_scratchpad (sp_index, (tmp & neg_mask) | masked_data);
	return bus::ok;
      }
      break;
#if 0
    case ubicom32bf_cpu_cgen::TIMER_ALARM_INDEX:
    case ubicom32bf_cpu_cgen::TIMER_ALARM_INDEX + 1:
    case ubicom32bf_cpu_cgen::TIMER_ALARM_INDEX + 2:
    case ubicom32bf_cpu_cgen::TIMER_ALARM_INDEX + 3:
    case ubicom32bf_cpu_cgen::TIMER_ALARM_INDEX + 4:
    case ubicom32bf_cpu_cgen::TIMER_ALARM_INDEX + 5:
    case ubicom32bf_cpu_cgen::TIMER_ALARM_INDEX + 6:
    case ubicom32bf_cpu_cgen::TIMER_ALARM_INDEX + 7:
      {
	UINT index = word_addr - ubicom32bf_cpu_cgen::TIMER_ALARM_INDEX;
	USI tmp = cpu->get_h_timer_alarm (index);
	cpu->set_h_timer_alarm (index, (tmp & neg_mask) | masked_data);
	return bus::ok;
      }
      break;

    case ubicom32bf_cpu_cgen::TCTRL_INDEX:
      {
	USI tmp = cpu->get_h_tctrl_w ();
	cpu->set_h_tctrl_w ((tmp & neg_mask) | masked_data);
	return bus::ok;
      }
      break;
#endif

    // read-only registers
    case ubicom32bf_cpu_cgen::ID_REVISION_INDEX:
    case ubicom32bf_cpu_cgen::MT_ACTIVE_INDEX:
    case ubicom32bf_cpu_cgen::MT_DBG_ACTIVE_INDEX:
#if 0
    case ubicom32bf_cpu_cgen::PIPE_BUSY_INDEX:
#endif
    case ubicom32bf_cpu_cgen::ROSR_INDEX:
    case ubicom32bf_cpu_cgen::DC_CAPTURE_PC_INDEX:
    case ubicom32bf_cpu_cgen::DC_CAPTURE_THREAD_INDEX:
    case ubicom32bf_cpu_cgen::CONTEXT_COUNT_INDEX:
    case ubicom32bf_cpu_cgen::IRQ_STATUS_0_INDEX:
    case ubicom32bf_cpu_cgen::IRQ_STATUS_1_INDEX:
    case ubicom32bf_cpu_cgen::MT_BREAK_INDEX:
      return bus::ok;
      break;
      
    case ubicom32bf_cpu_cgen::INTSET_1_INDEX:
      {
	cpu->set_h_intset_1 ((tmp & neg_mask) | masked_data);
	return bus::ok;
      }
      break;
    case ubicom32bf_cpu_cgen::INTCLR_1_INDEX:
      {
	cpu->set_h_intclr_1 ((tmp & neg_mask) | masked_data);
	return bus::ok;
      }
      break;
    case ubicom32bf_cpu_cgen::INTSET_0_INDEX:
      {
	cpu->set_h_intset_0 ((tmp & neg_mask) | masked_data);
	return bus::ok;
      }
      break;
    case ubicom32bf_cpu_cgen::INTCLR_0_INDEX:
      {
	cpu->set_h_intclr_0 ((tmp & neg_mask) | masked_data);
	return bus::ok;
      }
      break;
    case ubicom32bf_cpu_cgen::MT_ACTIVE_SET_INDEX:
      {
	cpu->set_h_mt_active_set ((tmp & neg_mask) | masked_data);
	return bus::ok;
      }
      break;
    case ubicom32bf_cpu_cgen::MT_ACTIVE_CLR_INDEX:
      {
	cpu->set_h_mt_active_clr ((tmp & neg_mask) | masked_data);
	return bus::ok;
      }
      break;
    case ubicom32bf_cpu_cgen::MT_BREAK_CLR_INDEX:
      {
	cpu->set_h_mt_break_clr ((tmp & neg_mask) | masked_data);
	return bus::ok;
      }
      break;
    case ubicom32bf_cpu_cgen::MT_DBG_ACTIVE_SET_INDEX:
      {
	cpu->set_h_mt_dbg_active_set ((tmp & neg_mask) | masked_data);
	return bus::ok;
      }
      break;
    case ubicom32bf_cpu_cgen::MT_DBG_ACTIVE_CLR_INDEX:
      {
	cpu->set_h_mt_dbg_active_clr ((tmp & neg_mask) | masked_data);
	return bus::ok;
      }
      break;
    case ubicom32bf_cpu_cgen::MAC_HI_INDEX:
      {
	SI tmp = cpu->get_h_mac_hi ();
	cpu->set_h_mac_hi ((SI)((tmp & neg_mask) | masked_data));
	return bus::ok;
      }
      break;
    case ubicom32bf_cpu_cgen::MAC_LO_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_mac_lo;
      setter = &ubicom32bf_cpu_cgen::set_h_mac_lo;
      break;
    case ubicom32bf_cpu_cgen::ACC1_HI_INDEX:
      {
	SI tmp = cpu->get_h_acc1_hi ();
	cpu->set_h_acc1_hi ((SI)((tmp & neg_mask) | masked_data));
	return bus::ok;
      }
      break;
    case ubicom32bf_cpu_cgen::ACC1_LO_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_acc1_lo;
      setter = &ubicom32bf_cpu_cgen::set_h_acc1_lo;
      break;
    case ubicom32bf_cpu_cgen::MAC_RC16_INDEX:
      {
	SI tmp = cpu->get_h_mac_rc16 ();
	cpu->set_h_mac_rc16 ((SI)((tmp & neg_mask) | masked_data));
	return bus::ok;
      }
      break;
    case ubicom32bf_cpu_cgen::SRC_3_INDEX:
      {
	SI tmp = cpu->get_h_src_3 ();
	cpu->set_h_src_3 ((SI)((tmp & neg_mask) | masked_data));
	return bus::ok;
      }
      break;
    case ubicom32bf_cpu_cgen::PC_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_pc;
      setter = &ubicom32bf_cpu_cgen::set_h_pc_user;
      break;
    case ubicom32bf_cpu_cgen::MT_MIN_DELAY_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_mt_min_delay_enable;
      setter = &ubicom32bf_cpu_cgen::set_h_mt_min_delay_enable;
      break;
    case ubicom32bf_cpu_cgen::CSR_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_csr;
      setter = &ubicom32bf_cpu_cgen::set_h_csr;
      break;
    case ubicom32bf_cpu_cgen::IREAD_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_iread;
      setter = &ubicom32bf_cpu_cgen::set_h_iread;
      break;
    case ubicom32bf_cpu_cgen::IRQ_MASK_1_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_irq_mask_1;
      setter = &ubicom32bf_cpu_cgen::set_h_irq_mask_1;
      break;
    case ubicom32bf_cpu_cgen::IRQ_MASK_0_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_irq_mask_0;
      setter = &ubicom32bf_cpu_cgen::set_h_irq_mask_0;
      break;
    case ubicom32bf_cpu_cgen::GLBL_CTL_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_glbl_ctl;
      setter = &ubicom32bf_cpu_cgen::set_h_glbl_ctl;
      break;
    case ubicom32bf_cpu_cgen::MT_ENABLE_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_mt_enable;
      setter = &ubicom32bf_cpu_cgen::set_h_mt_enable;
      break;
    case ubicom32bf_cpu_cgen::MT_PRIORITY_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_mt_priority;
      setter = &ubicom32bf_cpu_cgen::set_h_mt_priority;
      break;
    case ubicom32bf_cpu_cgen::MT_SCHEDULE_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_mt_schedule;
      setter = &ubicom32bf_cpu_cgen::set_h_mt_schedule;
      break;
    case ubicom32bf_cpu_cgen::MT_SINGLESTEP_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_mt_single_step;
      setter = &ubicom32bf_cpu_cgen::set_h_mt_single_step;
      break;
    case ubicom32bf_cpu_cgen::DC_CAPTURE_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_dcapt;
      setter = &ubicom32bf_cpu_cgen::set_h_dcapt;
      break;
#if 0
    case ubicom32bf_cpu_cgen::WATCHDOG_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_watchdog;
      setter = &ubicom32bf_cpu_cgen::set_h_watchdog;
      break;
    case ubicom32bf_cpu_cgen::CLOCK_CTL_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_clock_ctl;
      setter = &ubicom32bf_cpu_cgen::set_h_clock_ctl;
      break;
#endif

    default:
      if(word_addr < ubicom32bf_cpu_cgen::ID_REVISION_INDEX)
	cpu->register_file[cpu->context][word_addr] =
	  (cpu->common_register_file[(word_addr - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)] & neg_mask) | masked_data;
      else
	cpu->common_register_file[(word_addr - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)] = 
	  (cpu->common_register_file[(word_addr - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)] & neg_mask) | masked_data;
      return bus::ok;
    }

  tmp = (cpu->*getter) ();
  (cpu->*setter) ((tmp & neg_mask) | masked_data);
  return bus::ok;
}
      
bus::status 
ubicom32bf_mapped_bus::word_read(host_int_4 word_addr, big_int_4 be_mask, big_int_4& data)
{
  USI (ubicom32bf_cpu_cgen::*getter) () const;

  switch (word_addr)
    {
    case ubicom32bf_cpu_cgen::DR_INDEX:
    case ubicom32bf_cpu_cgen::DR_INDEX + 1:
    case ubicom32bf_cpu_cgen::DR_INDEX + 2:
    case ubicom32bf_cpu_cgen::DR_INDEX + 3:
    case ubicom32bf_cpu_cgen::DR_INDEX + 4:
    case ubicom32bf_cpu_cgen::DR_INDEX + 5:
    case ubicom32bf_cpu_cgen::DR_INDEX + 6:
    case ubicom32bf_cpu_cgen::DR_INDEX + 7:
    case ubicom32bf_cpu_cgen::DR_INDEX + 8:
    case ubicom32bf_cpu_cgen::DR_INDEX + 9:
    case ubicom32bf_cpu_cgen::DR_INDEX + 10:
    case ubicom32bf_cpu_cgen::DR_INDEX + 11:
    case ubicom32bf_cpu_cgen::DR_INDEX + 12:
    case ubicom32bf_cpu_cgen::DR_INDEX + 13:
    case ubicom32bf_cpu_cgen::DR_INDEX + 14:
    case ubicom32bf_cpu_cgen::DR_INDEX + 15:
#if 0
    case ubicom32bf_cpu_cgen::DR_INDEX + 16:
    case ubicom32bf_cpu_cgen::DR_INDEX + 17:
    case ubicom32bf_cpu_cgen::DR_INDEX + 18:
    case ubicom32bf_cpu_cgen::DR_INDEX + 19:
    case ubicom32bf_cpu_cgen::DR_INDEX + 20:
    case ubicom32bf_cpu_cgen::DR_INDEX + 21:
    case ubicom32bf_cpu_cgen::DR_INDEX + 22:
    case ubicom32bf_cpu_cgen::DR_INDEX + 23:
    case ubicom32bf_cpu_cgen::DR_INDEX + 24:
    case ubicom32bf_cpu_cgen::DR_INDEX + 25:
    case ubicom32bf_cpu_cgen::DR_INDEX + 26:
    case ubicom32bf_cpu_cgen::DR_INDEX + 27:
    case ubicom32bf_cpu_cgen::DR_INDEX + 28:
    case ubicom32bf_cpu_cgen::DR_INDEX + 29:
    case ubicom32bf_cpu_cgen::DR_INDEX + 30:
    case ubicom32bf_cpu_cgen::DR_INDEX + 31:
#endif
      {
	UINT dr_index = word_addr - ubicom32bf_cpu_cgen::DR_INDEX;
	data = cpu->get_h_dr (dr_index);
	return bus::ok;
      }
      break;

    case ubicom32bf_cpu_cgen::AR_INDEX:
    case ubicom32bf_cpu_cgen::AR_INDEX + 1:
    case ubicom32bf_cpu_cgen::AR_INDEX + 2:
    case ubicom32bf_cpu_cgen::AR_INDEX + 3:
    case ubicom32bf_cpu_cgen::AR_INDEX + 4:
    case ubicom32bf_cpu_cgen::AR_INDEX + 5:
    case ubicom32bf_cpu_cgen::AR_INDEX + 6:
    case ubicom32bf_cpu_cgen::AR_INDEX + 7:
      {
	UINT ar_index = word_addr - ubicom32bf_cpu_cgen::AR_INDEX;
	data = cpu->get_h_ar (ar_index);
	return bus::ok;
      }
      break;

    case ubicom32bf_cpu_cgen::SCRATCHPAD0_INDEX:
    case ubicom32bf_cpu_cgen::SCRATCHPAD0_INDEX + 1:
    case ubicom32bf_cpu_cgen::SCRATCHPAD0_INDEX + 2:
    case ubicom32bf_cpu_cgen::SCRATCHPAD0_INDEX + 3:
    case ubicom32bf_cpu_cgen::SCRATCHPAD0_INDEX + 4:
    case ubicom32bf_cpu_cgen::SCRATCHPAD0_INDEX + 5:
    case ubicom32bf_cpu_cgen::SCRATCHPAD0_INDEX + 6:
      {
	UINT sp_index = word_addr - ubicom32bf_cpu_cgen::SCRATCHPAD0_INDEX;
	data = cpu->get_h_scratchpad (sp_index);
	return bus::ok;
      }
      break;

#if 0
    case ubicom32bf_cpu_cgen::TIMER_ALARM_INDEX:
      {
	UINT index = word_addr - ubicom32bf_cpu_cgen::TIMER_ALARM_INDEX;
	data = cpu->get_h_timer_alarm (index);
	return bus::ok;
      }
      break;

    case ubicom32bf_cpu_cgen::TCTRL_INDEX:
      {
	USI tmp = cpu->get_h_tctrl_w ();
	data = tmp;
	return bus::ok;
      }
      break;
#endif

    case ubicom32bf_cpu_cgen::CONTEXT_COUNT_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_count;
      break;
    case ubicom32bf_cpu_cgen::IRQ_STATUS_1_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_irq_status_1;
      break;
    case ubicom32bf_cpu_cgen::IRQ_STATUS_0_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_irq_status_0;
      break;
    case ubicom32bf_cpu_cgen::ID_REVISION_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_id_revision;
      break;
#if 0
    case ubicom32bf_cpu_cgen::GLBL_TIMER_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_glbl_timer;
      break;
#endif
    case ubicom32bf_cpu_cgen::MT_ACTIVE_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_mt_active;
      break;
    case ubicom32bf_cpu_cgen::MT_DBG_ACTIVE_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_mt_dbg_active;
      break;
    case ubicom32bf_cpu_cgen::MT_SINGLESTEP_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_mt_single_step;
      break;
    case ubicom32bf_cpu_cgen::MT_MIN_DELAY_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_mt_min_delay_enable;
      break;

#if 0
    case ubicom32bf_cpu_cgen::PIPE_BUSY_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_pipe_busy;
      break;
#endif

    case ubicom32bf_cpu_cgen::MAC_HI_INDEX:
      {
	data = cpu->get_h_mac_hi ();
	return bus::ok;
      }
      break;
    case ubicom32bf_cpu_cgen::MAC_LO_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_mac_lo;
      break;
    case ubicom32bf_cpu_cgen::ACC1_HI_INDEX:
      {
	data = cpu->get_h_acc1_hi ();
	return bus::ok;
      }
      break;
    case ubicom32bf_cpu_cgen::ACC1_LO_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_acc1_lo;
      break;
    case ubicom32bf_cpu_cgen::PC_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_pc;
      break;
    case ubicom32bf_cpu_cgen::MAC_RC16_INDEX:
      {
	data = cpu->get_h_mac_rc16 ();
	return bus::ok;
      }
      break;
    case ubicom32bf_cpu_cgen::SRC_3_INDEX:
      {
	data = cpu->get_h_src_3 ();
	return bus::ok;
      }
      break;
    case ubicom32bf_cpu_cgen::CSR_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_csr;
      break;
    case ubicom32bf_cpu_cgen::ROSR_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_rosr;
      break;
    case ubicom32bf_cpu_cgen::IREAD_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_iread;
      break;
    case ubicom32bf_cpu_cgen::IRQ_MASK_1_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_irq_mask_1;
      break;
    case ubicom32bf_cpu_cgen::IRQ_MASK_0_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_irq_mask_0;
      break;
    case ubicom32bf_cpu_cgen::GLBL_CTL_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_glbl_ctl;
      break;
    case ubicom32bf_cpu_cgen::MT_ENABLE_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_mt_enable;
      break;
    case ubicom32bf_cpu_cgen::MT_PRIORITY_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_mt_priority;
      break;
    case ubicom32bf_cpu_cgen::MT_SCHEDULE_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_mt_schedule;
      break;
    case ubicom32bf_cpu_cgen::MT_BREAK_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_mt_break;
      break;
    case ubicom32bf_cpu_cgen::DC_CAPTURE_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_dcapt;
      break;
    case ubicom32bf_cpu_cgen::DC_CAPTURE_PC_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_dcapt_pc;
      break;
    case ubicom32bf_cpu_cgen::DC_CAPTURE_THREAD_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_dcapt_thread;
      break;
#if 0
    case ubicom32bf_cpu_cgen::WATCHDOG_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_watchdog;
      break;
    case ubicom32bf_cpu_cgen::RANDOM_SEED_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_random_seed;
      break;
    case ubicom32bf_cpu_cgen::CLOCK_CTL_INDEX:
      getter = &ubicom32bf_cpu_cgen::get_h_clock_ctl;
      break;
#endif
    default:
      if(word_addr < ubicom32bf_cpu_cgen::ID_REVISION_INDEX)
	data = (big_int_4)(cpu->register_file[cpu->context][word_addr]);
      else
	data = (big_int_4)(cpu->common_register_file[(word_addr - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
      return bus::ok;
    }

  data = (cpu->*getter) ();
  return bus::ok;
}

int 
ubicom32bf_mapped_bus::do_direct_passthrough (host_int_4 &addr) 
{
  host_int_4 word_addr;
  int direct_access;

  word_addr = addr & WORD_ALIGN_MASK;
  direct_access = cpu->direct_access;
  cpu->direct_access = 0; /* prevent recursive memory access from seeing flag on */

  /* don't use internal bus unless we are performing direct memory access.  Let
     the multifunction timers pass through so they are handled by the timer itself. */
  if (!direct_access)
    return 1;

#if 0
  if (!direct_access || (word_addr >= 0x290 && word_addr < 0x2d0))
    return 1;
#endif

  return 0;
}

ubicom32bf_cpu::ubicom32bf_cpu (const USI cpu) :
  engine (32768), // XXX: tune size
  real_data_bus (0),
  mapped_data_bus (& this->real_data_bus, this),
#if 0
  harvard_debugger_bus (& this->data_bus, & this->insn_bus, FIRST_DATA_SECTION, FIRST_DATA_SECTION,
			FIRST_INSN_SECTION, FIRST_INSN_SECTION),
#endif
  watchdog_trigger_pin (this, & ubicom32bf_cpu::do_watchdog_trigger_pin),
  irq_0_pin (this, & ubicom32bf_cpu::do_irq_0_pin),
  irq_1_pin (this, & ubicom32bf_cpu::do_irq_1_pin),
  irq_2_pin (this, & ubicom32bf_cpu::do_irq_2_pin),
  irq_3_pin (this, & ubicom32bf_cpu::do_irq_3_pin),
  irq_4_pin (this, & ubicom32bf_cpu::do_irq_4_pin),
  irq_5_pin (this, & ubicom32bf_cpu::do_irq_5_pin),
  irq_6_pin (this, & ubicom32bf_cpu::do_irq_6_pin),
  irq_7_pin (this, & ubicom32bf_cpu::do_irq_7_pin),
  irq_8_pin (this, & ubicom32bf_cpu::do_irq_8_pin),
  irq_9_pin (this, & ubicom32bf_cpu::do_irq_9_pin),
  irq_10_pin (this, & ubicom32bf_cpu::do_irq_10_pin),
  irq_11_pin (this, & ubicom32bf_cpu::do_irq_11_pin),
  irq_12_pin (this, & ubicom32bf_cpu::do_irq_12_pin),
  irq_13_pin (this, & ubicom32bf_cpu::do_irq_13_pin),
  irq_14_pin (this, & ubicom32bf_cpu::do_irq_14_pin),
  irq_15_pin (this, & ubicom32bf_cpu::do_irq_15_pin),
  irq_16_pin (this, & ubicom32bf_cpu::do_irq_16_pin),
  irq_17_pin (this, & ubicom32bf_cpu::do_irq_17_pin),
  irq_18_pin (this, & ubicom32bf_cpu::do_irq_18_pin),
  irq_19_pin (this, & ubicom32bf_cpu::do_irq_19_pin),
  irq_20_pin (this, & ubicom32bf_cpu::do_irq_20_pin),
  irq_21_pin (this, & ubicom32bf_cpu::do_irq_21_pin),
  irq_22_pin (this, & ubicom32bf_cpu::do_irq_22_pin),
  irq_23_pin (this, & ubicom32bf_cpu::do_irq_23_pin),
  irq_24_pin (this, & ubicom32bf_cpu::do_irq_24_pin),
  irq_25_pin (this, & ubicom32bf_cpu::do_irq_25_pin),
  irq_26_pin (this, & ubicom32bf_cpu::do_irq_26_pin),
  irq_27_pin (this, & ubicom32bf_cpu::do_irq_27_pin),
  irq_28_pin (this, & ubicom32bf_cpu::do_irq_28_pin),
  irq_29_pin (this, & ubicom32bf_cpu::do_irq_29_pin),
  irq_30_pin (this, & ubicom32bf_cpu::do_irq_30_pin),
  irq_31_pin (this, & ubicom32bf_cpu::do_irq_31_pin),
  mbox_intr_pin (this, & ubicom32bf_cpu::do_mbox_intr_pin),
  rttmr_intr_pin (this, & ubicom32bf_cpu::do_rttmr_intr_pin),
  scheduler_pin (this, & ubicom32bf_cpu::do_scheduler_pin),
  execute_pin (this, & ubicom32bf_cpu::do_execute_pin),
  irq_math_coprocessor_completion_pin (this, & ubicom32bf_cpu::do_irq_math_coprocessor_completion_pin),
  irq_math_coprocessor_exception_pin (this, & ubicom32bf_cpu::do_irq_math_coprocessor_exception_pin)
{
  // externally visible pins
  add_watchable_pin ("irq_0", & irq_0_pin);
  add_watchable_pin ("irq_1", & irq_1_pin);
  add_watchable_pin ("irq_2", & irq_2_pin);
  add_watchable_pin ("irq_3", & irq_3_pin);
  add_watchable_pin ("irq_4", & irq_4_pin);
  add_watchable_pin ("irq_5", & irq_5_pin);
  add_watchable_pin ("irq_6", & irq_6_pin);
  add_watchable_pin ("irq_7", & irq_7_pin);
  add_watchable_pin ("irq_8", & irq_8_pin);
  add_watchable_pin ("irq_9", & irq_9_pin);
  add_watchable_pin ("irq_10", & irq_10_pin);
  add_watchable_pin ("irq_11", & irq_11_pin);
  add_watchable_pin ("irq_12", & irq_12_pin);
  add_watchable_pin ("irq_13", & irq_13_pin);
  add_watchable_pin ("irq_14", & irq_14_pin);
  add_watchable_pin ("irq_15", & irq_15_pin);
  add_watchable_pin ("irq_16", & irq_16_pin);
  add_watchable_pin ("irq_17", & irq_17_pin);
  add_watchable_pin ("irq_18", & irq_18_pin);
  add_watchable_pin ("irq_19", & irq_19_pin);
  add_watchable_pin ("irq_20", & irq_20_pin);
  add_watchable_pin ("irq_21", & irq_21_pin);
  add_watchable_pin ("irq_22", & irq_22_pin);
  add_watchable_pin ("irq_23", & irq_23_pin);
  add_watchable_pin ("irq_24", & irq_24_pin);
  add_watchable_pin ("irq_25", & irq_25_pin);
  add_watchable_pin ("irq_26", & irq_26_pin);
  add_watchable_pin ("irq_27", & irq_27_pin);
  add_watchable_pin ("irq_28", & irq_28_pin);
  add_watchable_pin ("irq_29", & irq_29_pin);
  add_watchable_pin ("irq_30", & irq_30_pin);
  add_watchable_pin ("irq_31", & irq_31_pin);
  add_watchable_pin ("irq_math_coprocessor_completion", & irq_math_coprocessor_completion_pin);
  add_watchable_pin ("irq_math_coprocessor_exception", & irq_math_coprocessor_exception_pin);
  add_watchable_pin ("mbox-interrupt", & mbox_intr_pin);
  add_watchable_pin ("rttmr-interrupt", & rttmr_intr_pin);
  add_watchable_pin ("mercury-scheduler", & scheduler_pin);
  add_watchable_pin ("mercury-execute", & execute_pin);
  add_watchable_pin ("watchdog_enable", & watchdog_enable_pin);
  add_watchable_pin ("watchdog_reset", & watchdog_reset_pin);
  add_watchable_pin ("watchdog_trigger", & watchdog_trigger_pin);

#if 0
  add_watchable_pin ("timer_enable", & timer_enable_pin);
  add_watchable_pin ("tctrl_t1rst", & tctrl_t1rst_pin);
  add_watchable_pin ("tctrl_t2rst", & tctrl_t2rst_pin);
  add_watchable_pin ("tctrl_t3rst", & tctrl_t3rst_pin);
  add_watchable_pin ("tctrl_t4rst", & tctrl_t4rst_pin);
  add_watchable_pin ("tctrl_t1ie", & tctrl_t1ie_pin);
  add_watchable_pin ("tctrl_t2ie", & tctrl_t2ie_pin);
  add_watchable_pin ("tctrl_t3ie", & tctrl_t3ie_pin);
  add_watchable_pin ("tctrl_t4ie", & tctrl_t4ie_pin);
#endif
  // The coprocessor needs to know when the core is running so it can properly
  // detect data memory access conflicts.
  this->add_watchable_pin ("running", & this->running_pin);

  // We need to replace the data memory accessor to not point to data_bus.  We do this so we can force
  // data_bus to point to a memory-mapped data bus that maps addresses to the register file of the cpu.
  // This allows default memory access functions used by generated instructions to remain untouched.  These
  // default memory access functions will thus access the mapped data bus instead of directly to low-level
  // memory.  We want the data memory accessor to point to a second bus pointer so configuration 
  // will still connect the CPU to memory and not overwrite the mapped data bus.  The mapped data bus 
  // is a passthrough data bus such that it initializes using the second data bus pointer such that
  // low-level requests will be honored by the configuration-supplied memory.

  add_accessor ("data-memory", & this->real_data_bus);
  this->data_bus = & this->mapped_data_bus;

  // Add a bus accessor to the mapped data bus for use by gloss.  This was originally
  // added to allow gloss to be COA neutral which is no longer necessary.

  add_bus ("data-bus", & this->mapped_data_bus);

  // We also need to replace the debugger bus so that it performs the decoding of a single
  // virtual address space used by the debugger into the true harvard architecture whereby
  // data and insn memory are separate and have overlapping addresses.

#if 0
  bus *bus = find_bus ("debugger-bus");
  disconnect_bus ("debugger-bus", bus);
  add_bus ("debugger-bus", & this->harvard_debugger_bus);
#endif

  // named attributes
  this->add_watchable_register(string("mpcontext"), &sidlibContext);
  this->add_watchable_register (string("ubicom_thread_select"), &sidlibContext, this, 
			 & ubicom32bf_cpu:: get_current_thread_attr,
			 & ubicom32bf_cpu:: set_current_thread_attr);
  for (unsigned i = 0; i < NUM_CONTEXTS; ++i)
    {
      string context = string ("c") + make_numeric_attribute (i);
      SI *dptr = register_file[i];
      
      // set up d0 - d31
      for (unsigned j = 0; j < 32; ++j)
	{
	  string name = context + string (":d") + make_numeric_attribute (j);
	  SI* value = dptr++;
	  this->add_watchable_register (name, value);
	}
      // set up a0 - a7
      for (unsigned j = 0; j < 8; ++j)
	{
	  string name = context + string (":a") + make_numeric_attribute (j);
	  SI* value = dptr++;
	  this->add_watchable_register (name, value);
	}
      // other context registers
      this->add_watchable_register (context + string(":mac_hi"), dptr++);
      this->add_watchable_register (context + string(":mac_lo"), dptr++);
      this->add_watchable_register (context + string(":mac_rc16"), dptr++);
      this->add_watchable_register (context + string(":source3"), dptr++);

      this->add_watchable_ro_register (context + string(":inst_cnt"), dptr++);
      this->add_watchable_register (context + string(":csr"), i, this, 
			     & ubicom32bf_cpu:: get_csr_attr,
			     & ubicom32bf_cpu:: set_csr_attr);
      ++dptr;
      this->add_watchable_register (context + string(":rosr"), i, this, 
				    & ubicom32bf_cpu:: get_rosr_attr,
				    & ubicom32bf_cpu:: set_rosr_attr);
      ++dptr;
      this->add_watchable_register (context + string(":iread_data"), (USI *)dptr++);
      this->add_watchable_register (context + string(":int_mask0"), (USI *)dptr++);
      this->add_watchable_register (context + string(":int_mask1"), (USI *)dptr++);

      SI* value = &register_file[i][ubicom32bf_cpu_cgen::PC_INDEX];;
      //USI* value = &pc[i];
      this->add_watchable_register (context + string(":pc"), value);
      if(cpu) 
	{
	  dptr = &register_file[i][TRAP_CAUSE_INDEX];
	  this->add_watchable_register (context + string(":trap_cause"), dptr++);
	  this->add_watchable_register (context + string(":acc1_hi"), dptr++);
	  this->add_watchable_register (context + string(":acc1_lo"), dptr++);
	  this->add_watchable_register (context + string(":previous_pc"), dptr++);
	}
    }

  this->add_watchable_register (string("global_ctrl"), GLBL_CTL_ADDR, this, 
			 & ubicom32bf_cpu:: get_generic_register_attr,
			 & ubicom32bf_cpu:: set_generic_register_attr);
  this->add_watchable_register (string("int_stat0"), IRQ_STATUS_0_ADDR, this, 
			 & ubicom32bf_cpu:: get_generic_register_attr,
			 & ubicom32bf_cpu:: set_stat_register_attr);
  this->add_watchable_register (string("int_stat1"), IRQ_STATUS_1_ADDR, this, 
			 & ubicom32bf_cpu:: get_generic_register_attr,
			 & ubicom32bf_cpu:: set_stat_register_attr);
  this->add_watchable_register (string("mt_en"), &(this->common_register_file[(ubicom32bf_cpu_cgen::MT_ENABLE_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]));
  this->add_watchable_register (string("mt_hpri"), &(this->common_register_file[(ubicom32bf_cpu_cgen::MT_PRIORITY_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]));
  this->add_watchable_register (string("mt_hrt"), MT_SCHEDULE_ADDR, this, 
			 & ubicom32bf_cpu:: get_generic_register_attr,
			 & ubicom32bf_cpu:: set_generic_register_attr);


  if(cpu)
    {
      SI *dptr = &common_register_file[MT_TRAP_EN_INDEX];

      this->add_watchable_register (string("mt_trap_en"), (USI *)dptr++);
      this->add_watchable_register (string("mt_trap"), (USI *)dptr++);
      this->add_watchable_register (string("mt_trap_set"), (USI *)dptr++);
      this->add_watchable_register (string("mt_trap_clr"), (USI *)dptr++);
    }

#if 0
  this->add_watchable_register (string("tctrl"), TCTRL_ADDR, this, 
			 & ubicom32bf_cpu:: get_generic_register_attr,
			 & ubicom32bf_cpu:: set_generic_register_attr);

  this->add_watchable_ro_register (string("global_timer"), &this->h_glbl_timer);
#endif
  
  
  // add syscall registers
  this->add_watchable_register (string("syscall-arg0"), &syscall_arg0);
  this->add_watchable_register (string("syscall-arg1"), &syscall_arg1);
  this->add_watchable_register (string("syscall-arg2"), &syscall_arg2);
  this->add_watchable_register (string("syscall-arg3"), &syscall_arg3);
  this->add_watchable_register (string("syscall-result"), &syscall_result);
  this->add_watchable_register (string("syscall-error"), &syscall_error);
  this->add_attribute (string("syscall-trap"), &syscall_trap_num, "setting");
  
  // we have a single pc register which equals the current context pc
  USI *value = &this->hardware.h_pc;

  this->add_watchable_register (string("pc"), value);

  this->add_attribute_virtual (string("harvard-pc"), this,
		       & ubicom32bf_cpu::get_harvard_pc_attr,
		       & ubicom32bf_cpu::set_harvard_pc_attr);

  if (cpu == 0) {
    printf("Registering 66 regs\n");
    this->create_gdb_register_attrs (66, "0;21;22;23;30;34;45;47;48", & this->hardware.h_pc);
  } else if (cpu == 1) {
    printf("Registering 97 regs\n");
    this->create_gdb_register_attrs (97, "0;21;22;23;30;34;47;50;52", & this->hardware.h_pc);
  } else if (cpu == 2) {
    printf("Registering 103 regs\n");
    this->create_gdb_register_attrs (103, "0;21;22;23;30;34;47;50;52", & this->hardware.h_pc);
  } else if (cpu == 3) {
    int num_regs = sizeof (ubicom32v5_regs)/sizeof(struct ubicom32_regs );
    printf("Registering %d regs\n", num_regs);
    this->create_gdb_register_attrs (num_regs, "0;21;22;23;30;34;47;50;52", & this->hardware.h_pc);
  }

  leapdec = 0;
  no_active=false;
  cpu_type = cpu;
  if (cpu >= 3)
    {
      /* Version 5+ */
      ubicom32bf_cpu_cgen::CC_CSR_INDEX = (ubicom32bf_cpu_cgen::UCSR_ADDR) / 4;
    }
  else
    {
      /* Older versions */
      ubicom32bf_cpu_cgen::CC_CSR_INDEX = (ubicom32bf_cpu_cgen::CSR_ADDR) / 4;
    }
}

void
ubicom32bf_cpu::reset()
{
  unsigned int i, j;

  context = 0;            // current thread is 0
  no_active = false;
  sidlibContext = context;
#if 0
  hrt_count = 0;          // no hrt threads at reset
  hrt_index = 0;
#endif

  if (cpu_type >= 3)
    currentHrtBase = 0xb8000800;
  else
    currentHrtBase = HRT_TABLE_ADDR_1;
  currentHrtSlot = 0;

  direct_access = 0;
  gdbAccess = 0;

  // context specific registers
  for (i = 0; i < NUM_CONTEXTS; ++i)
    {
      memset (register_file[i], 0, 256 * sizeof(SI));
      if(cpu_type)
	{
	  if (cpu_type >= 3)
	    register_file[i][ubicom32bf_cpu_cgen::PC_INDEX] = 0xb0000000;
	  else
	    register_file[i][ubicom32bf_cpu_cgen::PC_INDEX] = 0x60000000;
	  //pc[i] = 0x60000000;
	  common_register_file[0] = 1;
	  //ubicom32bf_cpu_cgen::REVISION_ID_VALUE =1;
	}
      else
	{
	  register_file[i][ubicom32bf_cpu_cgen::PC_INDEX] = 0x20000000;
	  //pc[i] = 0x20000000;
	  common_register_file[0] = 0;
	  //ubicom32bf_cpu_cgen::REVISION_ID_VALUE =0;
	}
    }

  // Initialize the i_rangeX_en register to 0x3ff for mars, 0xfff for ares 0xffff for jupiter. 
  SI enable;
  if (cpu_type)
    {
      switch(cpu_type)
	{
	case 1: 
	  enable = 0x3ff;
	  break;
	case 2:
	  enable = 0xfff;
	  break;
	case 3:
	  enable = 0xffff;
	  break;
	}

      // Initialize the i_rangeX_hi registers to 0xfffffffc
      i = (0x200 - 0x100)/4;
      for (j=0; j< 4; j++, i++)
	common_register_file[i] = 0xfffffffc;

      i = (0x240 - 0x100)/4;
      for (j=0; j< 4; j++, i++)
	common_register_file[i] = enable;

      // Initialize d_rangeX_hi to 0xfffffffc
      i = (0x260 - 0x100)/4;
      for (j=0; j< 5; j++, i++)
	common_register_file[i] = 0xfffffffc;

      // Initialize d_rangeX_en
      i = (0x2a0 - 0x100)/4;
      for (j=0; j< 5; j++, i++)
	common_register_file[i] = enable;
    }

  // global registers
  set_h_glbl_ctl (0);     // by default control bits disabled
  set_h_mt_priority (1);  // thread 0 is high priority
  set_h_mt_enable (1);    // only thread 0 is enabled by default
  set_h_mt_active (1);    // only thread 0 is active by default
  set_h_mt_schedule (0);  // all threads are SRT by default
  set_h_mt_break (0);     // no breakpoints triggered
  set_h_mt_min_delay_enable (0); // no scheduling delays

  // reset all thread pcs to 0, all s1/dest contexts to themselves, and all timer alarms to 0
  for (i = 0; i < NUM_CONTEXTS; ++i)
    {
      if(cpu_type)
	{
	  if (cpu_type >= 3)
	    set_h_pc_c (0xb0000000, i);
	  else
	    set_h_pc_c (0x60000000, i);
	  common_register_file[0] = 1;
	}
      else
	{
	  set_h_pc_c (0x20000000, i);
	  common_register_file[0] = 0;
	}
      set_h_count (i, 0);
      s1_context[i] = i;
      d_context[i] = i;
#if 0
      timer_alarm[i] = 0;
#endif
      h_ar_inc_set (i, 0);  /* reset pre/post increment values */
      delay_table[i] = MAX_DELAY;

      // set all the rosr registers
      set_h_rosr_reset(i);
    }

  srt_index = NUM_CONTEXTS;

  set_h_irq_status_0 (0);
  set_h_irq_status_1 (0);

#if 0
  reset_h_glbl_timer ();

  set_h_random_seed ((USI)time(NULL));
  set_h_tctrl (0);
#endif

  set_h_mt_dbg_active (0x1);
  set_h_mt_single_step (0);

  set_h_dcapt (0);
  set_h_dcapt_pc (0);
  set_h_dcapt_thread (0);
  set_set_dcapt_interrupt(0);
  set_h_perr_addr(0);

#if 0
  set_h_watchdog (0);
  set_h_clock_ctl (0);
#endif

  for(i=0; i< 4; i++)
    set_h_scratchpad(i, 0);

  // clear hardware registers
  memset (& this->hardware, 0, sizeof (this->hardware));
  if(cpu_type)
    {
      if (cpu_type >= 3)
	this->hardware.h_pc = 0xb0000000;
      else
	this->hardware.h_pc = 0x60000000;
      common_register_file[0] = 1;
    }
  else
    {
      this->hardware.h_pc = 0x20000000;
      common_register_file[0] = 0;
    }

  /* try initializing the hrt table at 0x800  if they exist */
  try
    {
      PCADDR pc;
      ADDR hrt = 0x800;
      if(cpu_type)
	{
	  if (cpu_type >= 3)
	    {
	      pc = 0xb0000000;
	      hrt = 0xb8000800;
	    }
	  else
	    pc = 0x60000000;
	  common_register_file[0] = 1;
	}
      else
	{
	  pc = 0x20000000;
	  common_register_file[0] = 0;
	}

      for(int i=0; i< 256; i++, hrt++)
	SETMEMQI (pc, hrt, 0xc0);
    }
  catch (cpu_memory_fault& t)
    {
      // do nothing 
    }

  /* try initializing the hrt table at 0x900  if they exist */
  try
    {
      PCADDR pc;
      ADDR hrt = 0x900;
      if(cpu_type)
	{
	  if (cpu_type >= 3)
	    {
	      pc = 0xb0000000;
	      hrt = 0xb8000900;
	    }
	  else
	    pc = 0x60000000;
	  common_register_file[0] = 1;
	}
      else
	{
	  pc = 0x20000000;
	  common_register_file[0] = 0;
	}
      for(int i=0; i< 256; i++, hrt++)
	SETMEMQI (pc, hrt, 0xc0);
    }
  catch (cpu_memory_fault& t)
    {
      // do nothing 
    }

  // need to do this so encapsulation reset will reread insns from insn memory
  flush_icache ();
}

// low-level data access routines that bypass data mapping
USI 
ubicom32bf_cpu::read_real_data_word (host_int_4 pc, host_int_4 address) const
{
  big_int_4 value;
  sid::bus::status s;

  if (LIKELY(this->real_data_bus)) {
    s = this->real_data_bus->read (address, value);
  } else {
    s = sid::bus::unmapped;
  }
  if (LIKELY(s == sid::bus::ok))
    return (USI)value;
  
  throw cpu_memory_fault (pc, address, s, "data read");
}

void
ubicom32bf_cpu::write_real_data_word (host_int_4 pc, host_int_4 address, big_int_4 value) const
{
  sid::bus::status s;

  if (LIKELY(this->real_data_bus)) {
    s = this->real_data_bus->write (address, value);
  } else {
    s = sid::bus::unmapped;
  }
  if (LIKELY(s == sid::bus::ok))
    return;
  
  throw cpu_memory_fault (pc, address, s, "data write");
}

// get s1 direct memory - account for source thread context in CSR
SI
ubicom32bf_cpu::get_s1_direct_mem (USI addr)
{
  // must switch global context, then switch back
  SI value = 0;
  int old_context = context;
  PCADDR pc = get_h_pc();
  context = s1_context[context];
  direct_access = 1;
  try
    {
      value = GETMEMSI (pc, addr);
    }
  catch (cpu_memory_fault& t)
    {
      // do nothing 
    }
  
  if(addr == ROSR_ADDR && (context != old_context))
    {
      /* we have to doctor the rosr value */
      value &= ~0x7c;
      value |= (old_context << 2);
    }

  context = old_context;
  direct_access = 0;
  return value;
}

// set destination direct memory - account for destination thread context in CSR
void
ubicom32bf_cpu::set_d_direct_mem (USI addr, SI value)
{
  // must switch global context, then switch back
  int old_context = context;
  PCADDR pc = get_h_pc();

  context = d_context[context];
  direct_access = 1;
  gdbAccess = 1;
  try
    {
      SETMEMSI_DIRECT (pc, addr, value);
    }
  catch (cpu_memory_fault& t)
    {
      // do nothing
    }
  context = old_context;
  direct_access = 0;
  gdbAccess = 0;

  // Do the DCAPT address check now
  if (this->get_h_dcapt () & 0x1)
    {
      USI doTheCheck = 1;
      if(this->leapdec)
	{
	  // a lea/pdec instruction processing
	  if(addr >= 0x80 && addr < 0xa0)
	    {
	      // a0-a7 is the target
	      if(!(this->get_h_csr() & CSR_DEST_THREAD_SELECT_BIT_MASK))
		{
		  // DCAPT check is not needed for this case
		  doTheCheck = 0;
		}
	    }
	}

      if(doTheCheck)
	{
	  USI cmpAddr = addr | ((addr > 0xff) ? 0: d_context[context]<<10);

	  // check for debug capture address being written to
	  if ((cmpAddr & ~0x3) == (this->get_h_dcapt () & ~0x3))
	    {
	      USI tvalue = context | DCAPT_ADDRESS_MATCH; 
	      set_h_dcapt_pc (pc);                         
	      set_set_dcapt_interrupt (tvalue);                 
	    }
	}
    }

}

// register access routines
USI
ubicom32bf_cpu_cgen::get_h_pc () const
{
  return register_file[context][ubicom32bf_cpu_cgen::PC_INDEX];;
}

void
ubicom32bf_cpu_cgen::set_h_pc (USI value)
{
  set_h_pc_c (value, context);
}

void
ubicom32bf_cpu_cgen::set_h_pc_c (USI value, int context_num)
{
  register_file[context_num][ubicom32bf_cpu_cgen::PC_INDEX] = value;
}

void
ubicom32bf_cpu_cgen::set_h_pc_user (USI value)
{
  // user only allowed to change pc if destination thread is inactive
  if (!is_active (context))
    set_h_pc_c (value, context);
}

SI
ubicom32bf_cpu_cgen::get_h_dr (UINT index) const
{
  const SI *ptr = register_file[context];
  return ptr[index];
}


void
ubicom32bf_cpu_cgen::set_h_dr (UINT index, SI value)
{
  PCADDR pc = get_h_pc();

  if(gdbAccess == 0)
    {
      // Do the DCAPT address check now
      if (this->get_h_dcapt () & 0x1)
	{
	  USI cmpAddr = (index << 2) | (((index << 2) > 0xff) ? 0: d_context[context]<<10);
	  
	  // check for debug capture address being written to
	  if ((cmpAddr & ~0x3) == (this->get_h_dcapt () & ~0x3))
	    {
	      USI tvalue = context | DCAPT_ADDRESS_MATCH; 
	      set_h_dcapt_pc (pc);                         
	      set_set_dcapt_interrupt (tvalue);                 
	    }
	}
    }

  SI *ptr = register_file[d_context[context]];
  ptr[index] = value;
}

void
ubicom32bf_cpu_cgen::set_h_dr_internal (UINT index, SI value)
{
  PCADDR pc = get_h_pc();

  if(gdbAccess == 0)
    {
      // Do the DCAPT address check now
      if (this->get_h_dcapt () & 0x1)
	{
	  USI cmpAddr = (index << 2) | (((index << 2) > 0xff) ? 0: context<<10);
	  
	  // check for debug capture address being written to
	  if ((cmpAddr & ~0x3) == (this->get_h_dcapt () & ~0x3))
	    {
	      USI tvalue = context | DCAPT_ADDRESS_MATCH; 
	      set_h_dcapt_pc (pc);                         
	      set_set_dcapt_interrupt (tvalue);                 
	    }
	}
    }

  SI *ptr = register_file[context];
  ptr[index] = value;
}

SI
ubicom32bf_cpu_cgen::get_h_s1_dr (UINT index) const
{
  const SI *ptr = register_file[context];
  return ptr[index];
}

USI
ubicom32bf_cpu_cgen::get_h_ar (UINT index) const
{
  const USI *ptr = (USI *)&register_file[context][AR_INDEX];
  return ptr[index];
}

void
ubicom32bf_cpu_cgen::set_h_ar (UINT index, USI value)
{
  USI *ptr = (USI *)&register_file[context][AR_INDEX];
  ptr[index] = value;
}

USI
ubicom32bf_cpu_cgen::get_h_scratchpad (UINT index) const
{
  const SI *spad_ptr = &(this->common_register_file[(ubicom32bf_cpu_cgen::SCRATCHPAD0_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  return spad_ptr[index];
}

SI
ubicom32bf_cpu_cgen::get_h_sep (void)
{
  SI *sep_ptr = &(this->common_register_file[(ubicom32bf_cpu_cgen::SEP_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  return *sep_ptr;
}

void
ubicom32bf_cpu_cgen::set_h_scratchpad (UINT index, USI value)
{
  SI *spad_ptr = &(this->common_register_file[(ubicom32bf_cpu_cgen::SCRATCHPAD0_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  spad_ptr[index] = value;
}

SI
ubicom32bf_cpu_cgen::get_h_mac_hi () const
{
  const SI *ptr = register_file[context];
  return ptr[MAC_HI_INDEX];
}

void
ubicom32bf_cpu_cgen::set_h_mac_hi (SI value)
{
  SI *ptr = register_file[context];
  ptr[MAC_HI_INDEX] = value;
}

USI
ubicom32bf_cpu_cgen::get_h_mac_lo () const
{
  USI *ptr = (USI *)register_file[context];
  return ptr[MAC_LO_INDEX];
}

void
ubicom32bf_cpu_cgen::set_h_mac_lo (USI value)
{
  USI *ptr = (USI *)register_file[context];
  ptr[MAC_LO_INDEX] = value;
}

SI
ubicom32bf_cpu_cgen::get_h_acc1_hi () const
{
  const SI *ptr = register_file[context];
  return ptr[ACC1_HI_INDEX];
}

void
ubicom32bf_cpu_cgen::set_h_acc1_hi (SI value)
{
  SI *ptr = register_file[context];
  ptr[ACC1_HI_INDEX] = value;
}

USI
ubicom32bf_cpu_cgen::get_h_acc1_lo () const
{
  USI *ptr = (USI *)register_file[context];
  return ptr[ACC1_LO_INDEX];
}

void
ubicom32bf_cpu_cgen::set_h_acc1_lo (USI value)
{
  USI *ptr = (USI *)register_file[context];
  ptr[ACC1_LO_INDEX] = value;
}

USI
ubicom32bf_cpu_cgen::get_acc_lo (UINT index) const
{
  if(index == 0)
     return get_h_mac_lo();
  else
     return get_h_acc1_lo();
}

DI
ubicom32bf_cpu_cgen::get_acc (UINT index) const
{
  DI tmp1, tmp2;
  if(index == 0) {
    tmp1 = get_h_mac_hi();
    tmp2 = get_h_mac_lo();
  } else {
    tmp1 = get_h_acc1_hi();
    tmp2 = get_h_acc1_lo();
  }

  tmp1 &= 0xffff;
  if(tmp1 & 0x8000) {
    /* we have to sign extend this */
    tmp1 |= 0xffff0000;
  }

  DI result = (((DI) tmp1)<<32 | ((DI)tmp2));
  return result;
}

USI
ubicom32bf_cpu_cgen::get_h_previous_pc () const
{
  const SI *ptr = register_file[context];
  return ptr[PREVIOUS_PC_INDEX];
}

void
ubicom32bf_cpu_cgen::set_h_previous_pc (USI value)
{
  USI *ptr = (USI *)register_file[context];
  ptr[PREVIOUS_PC_INDEX] = value;
}

SI
ubicom32bf_cpu_cgen::get_h_mac_rc16 () const
{
  USI *ptr = (USI *)register_file[context];
  return ptr[MAC_RC16_INDEX];
}

void
ubicom32bf_cpu_cgen::set_h_mac_rc16 (SI value)
{
  SI *ptr = (SI *)register_file[context];
  ptr[MAC_RC16_INDEX] = value;
}

SI
ubicom32bf_cpu_cgen::get_h_src_3 () const
{
  const SI *ptr = register_file[context];
  return ptr[SRC_3_INDEX];
}

void
ubicom32bf_cpu_cgen::set_h_src_3 (SI value)
{
  SI *ptr = register_file[context];
  ptr[SRC_3_INDEX] = value;
}

#if 0
USI
ubicom32bf_cpu_cgen::get_h_timer_alarm (int index) const
{
  return timer_alarm[index];
}

void
ubicom32bf_cpu_cgen::set_h_timer_alarm (int index, USI value)
{
  timer_alarm[index] = value;
}
#endif

USI
ubicom32bf_cpu_cgen::get_h_csr () const
{
  return get_h_csr_c (context);
}

USI
ubicom32bf_cpu_cgen::get_h_csr_c (int context_num) const
{
  USI *ptr = (USI *)register_file[context_num];
  return ptr[CSR_INDEX];
}

void
ubicom32bf_cpu_cgen::set_h_csr (USI value)
{
  set_h_csr_c (value, context);
}

void
ubicom32bf_cpu_cgen::set_h_csr_c (USI value, int context_num)
{
  USI *ptr = (USI *)&register_file[context_num][CSR_INDEX];

  // if source thread context is enabled, then the context specified is used
  // to get the source CR
  if (value & CSR_SOURCE_THREAD_SELECT_BIT_MASK)
    {
      int s1_context = (value & CSR_SOURCE_THREAD_MASK) >> CSR_SOURCE_THREAD_SHIFT;
      this->s1_context[context_num] = s1_context & 0x1f;
    }
  else
    s1_context[context_num] = context_num;

  // if destination thread context is enabled, then the context specified is used
  // to get the destination thread context
  if (value & CSR_DEST_THREAD_SELECT_BIT_MASK)
    {
      int d_context = (value & CSR_DEST_THREAD_MASK) >> CSR_DEST_THREAD_SHIFT;
      this->d_context[context_num] = d_context & 0x1f;
    }
  else
    d_context[context_num] = context_num;

  // we can only alter certain r/w fields
  USI masked_value = value & CSR_READ_ONLY_MASK;
  *ptr &= ~CSR_READ_ONLY_MASK;  
  *ptr |= masked_value;
}

USI
ubicom32bf_cpu_cgen::get_h_cc_csr () const
{
  return get_h_cc_csr_c (context);
}

USI
ubicom32bf_cpu_cgen::get_h_cc_csr_c (int context_num) const
{
  USI *ptr = (USI *)register_file[context_num];
  return ptr[UCSR_ADDR/4];
}

void
ubicom32bf_cpu_cgen::set_h_cc_csr (USI value)
{
  set_h_cc_csr_c (value, context);
}

void
ubicom32bf_cpu_cgen::set_h_cc_csr_c (USI value, int context_num)
{
  USI *ptr = (USI *)&register_file[context_num][UCSR_ADDR/4];
  *ptr = value;
}

void
ubicom32bf_cpu_cgen::set_h_nbit_16 (BI newval)
{
  USI *ptr = (USI *)&register_file[context][UCSR_ADDR/4];
  if (newval)
    *ptr |= CSR_NBIT_16_MASK;
  else
    *ptr &= ~CSR_NBIT_16_MASK;
}

void
ubicom32bf_cpu_cgen::set_h_zbit_16 (BI newval)
{
  USI *ptr = (USI *)&register_file[context][UCSR_ADDR/4];
  if (newval)
    *ptr |= CSR_ZBIT_16_MASK;
  else
    *ptr &= ~CSR_ZBIT_16_MASK;
}

void
ubicom32bf_cpu_cgen::set_h_vbit_16 (BI newval)
{
  USI *ptr = (USI *)&register_file[context][UCSR_ADDR/4];
  if (newval)
    *ptr |= CSR_VBIT_16_MASK;
  else
    *ptr &= ~CSR_VBIT_16_MASK;
}

void
ubicom32bf_cpu_cgen::set_h_cbit_16 (BI newval)
{
  USI *ptr = (USI *)&register_file[context][UCSR_ADDR/4];
  if (newval)
    *ptr |= CSR_CBIT_16_MASK;
  else
    *ptr &= ~CSR_CBIT_16_MASK;
}

void
ubicom32bf_cpu_cgen::set_h_nbit_32 (BI newval)
{
  USI *ptr = (USI *)&register_file[context][UCSR_ADDR/4];
  if (newval)
    *ptr |= CSR_NBIT_32_MASK;
  else
    *ptr &= ~CSR_NBIT_32_MASK;
}

void
ubicom32bf_cpu_cgen::set_h_zbit_32 (BI newval)
{
  USI *ptr = (USI *)&register_file[context][UCSR_ADDR/4];
  if (newval)
    *ptr |= CSR_ZBIT_32_MASK;
  else
    *ptr &= ~CSR_ZBIT_32_MASK;
}

void
ubicom32bf_cpu_cgen::set_h_vbit_32 (BI newval)
{
  USI *ptr = (USI *)&register_file[context][UCSR_ADDR/4];
  if (newval)
    *ptr |= CSR_VBIT_32_MASK;
  else
    *ptr &= ~CSR_VBIT_32_MASK;
}

void
ubicom32bf_cpu_cgen::set_h_cbit_32 (BI newval)
{
  USI *ptr = (USI *)&register_file[context][UCSR_ADDR/4];
  if (newval)
    *ptr |= CSR_CBIT_32_MASK;
  else
    *ptr &= ~CSR_CBIT_32_MASK;
}

void
ubicom32bf_cpu_cgen::set_h_dsp_obit (BI newval)
{
  USI *ptr = (USI *)&register_file[context][UCSR_ADDR/4];
  if (newval)
    *ptr |= CSR_DSP_OBIT_MASK;
  else
    *ptr &= ~CSR_DSP_OBIT_MASK;
}

void
ubicom32bf_cpu_cgen::set_h_privbit (BI newval)
{
  USI *ptr = (USI *)&register_file[context][CSR_INDEX];
  if (newval)
    *ptr |= CSR_PRIVILEGE_MASK;
  else
    *ptr &= ~CSR_PRIVILEGE_MASK;
}

void
ubicom32bf_cpu_cgen::set_h_prev_privbit (BI newval)
{
  USI *ptr = (USI *)&register_file[context][CSR_INDEX];
  if (newval)
    *ptr |= CSR_PREV_PRIVILEGE_MASK;
  else
    *ptr &= ~CSR_PREV_PRIVILEGE_MASK;
}

USI
ubicom32bf_cpu_cgen::get_h_rosr_c (int context_num) const
{
  USI *ptr = (USI *)&register_file[context_num][ROSR_INDEX];
  USI val;
  val = *ptr |
    (((get_h_irq_status_0() & get_h_irq_mask_0_c(context_num)) ||
      (get_h_irq_status_1() & get_h_irq_mask_1_c(context_num))) &
     (get_h_glbl_ctl() & GLBL_CTL_INTERRUPT_ENABLE_BIT_MASK));

  return val;
}

USI
ubicom32bf_cpu_cgen::get_h_rosr () const
{
  return get_h_rosr_c (context);
}

void
ubicom32bf_cpu_cgen::set_h_rosr_reset(int context)
{
  USI *ptr = (USI *)&register_file[context][ROSR_INDEX];
  *ptr = (context << ROSR_CONTEXT_SHIFT);
}

void
ubicom32bf_cpu_cgen::set_h_rosr_c (USI value, int context_num)
{
  USI *ptr = (USI *)&register_file[context_num][ROSR_INDEX];
  USI val;
  val = *ptr;

  val &= (~ROSR_MEM_BUSY_BIT_MASK);

  val |= (value & ROSR_MEM_BUSY_BIT_MASK);
  *ptr = val;
}

void
ubicom32bf_cpu_cgen::set_h_rosr (USI value)
{
  set_h_rosr_c (value, context);
}

#if 0
USI
ubicom32bf_cpu_cgen::get_h_pipe_busy () const
{
  return 0;
}
#endif

USI
ubicom32bf_cpu_cgen::get_h_iread () const
{
  USI *ptr = (USI *)register_file[context];
  return ptr[IREAD_INDEX];
}

void
ubicom32bf_cpu_cgen::set_h_iread (USI value)
{
  USI *ptr = (USI *)register_file[context];
  ptr[IREAD_INDEX] = value;
}

USI
ubicom32bf_cpu_cgen::get_h_irq_mask_0 () const
{
  return get_h_irq_mask_0_c (context);
}

USI
ubicom32bf_cpu_cgen::get_h_irq_mask_0_c (int context_num) const
{
  USI *ptr = (USI *)register_file[context_num];
  return ptr[IRQ_MASK_0_INDEX];
}

void
ubicom32bf_cpu_cgen::set_h_irq_mask_0 (USI value)
{
  USI *ptr = (USI *)register_file[context];
  ptr[IRQ_MASK_0_INDEX] = value;
}

USI
ubicom32bf_cpu_cgen::get_h_irq_mask_1 () const
{
  return get_h_irq_mask_1_c (context);
}

USI
ubicom32bf_cpu_cgen::get_h_irq_mask_1_c (int context_num) const
{
  USI *ptr = (USI *)register_file[context_num];
  return ptr[IRQ_MASK_1_INDEX];
}

void
ubicom32bf_cpu_cgen::set_h_irq_mask_1 (USI value)
{
  USI *ptr = (USI *)register_file[context];
  ptr[IRQ_MASK_1_INDEX] = value;
}


// Global register access routines

USI
ubicom32bf_cpu_cgen::get_h_id_revision () const
{
  //return REVISION_ID_VALUE;
  return (USI) common_register_file[0];
}

#if 0
USI
ubicom32bf_cpu_cgen::get_h_glbl_timer () const
{
  return this->h_glbl_timer;
}

void
ubicom32bf_cpu_cgen::set_h_glbl_timer (USI value)
{
  /* not allowed */
}

void
ubicom32bf_cpu_cgen::reset_h_glbl_timer ()
{
  this->h_glbl_timer = 0;
}

const USI ubicom32bf_cpu_cgen::IRQ_1_TIMER_BIT[]  = 
      { 0x00010000, 0x00020000, 0x00040000, 0x00080000,
	0x00100000, 0x00200000, 0x00400000, 0x00800000 };

void
ubicom32bf_cpu_cgen::inc_h_glbl_timer ()
{
  ++this->h_glbl_timer;
  // check if any timer alarm gets triggered by the new timer value
  for (int i = 0; i < NUM_CONTEXTS; ++i)
    {
      if (this->h_glbl_timer == get_h_timer_alarm (i))
	set_h_intset_1 (IRQ_1_TIMER_BIT[i]);
    }
}
#endif

USI
ubicom32bf_cpu_cgen::get_h_irq_status_0 () const
{
  SI data = (this->common_register_file[(ubicom32bf_cpu_cgen::IRQ_STATUS_0_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);

  return (USI) data;
}

void
ubicom32bf_cpu_cgen::set_h_irq_status_0 (USI value)
{
  (this->common_register_file[(ubicom32bf_cpu_cgen::IRQ_STATUS_0_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]) = value;
}

void
ubicom32bf_cpu_cgen::set_h_intset_0 (USI bit_mask)
{
  // if changing the irq_status_0 register, check if any inactive contexts
  // are made active due to an unmasked interrupt condition
  USI *irq_status_0 = (USI *)&(this->common_register_file[(ubicom32bf_cpu_cgen::IRQ_STATUS_0_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  if (*irq_status_0 ^ bit_mask)
    {
      *irq_status_0 |= bit_mask;
      for (int i = 0; i < NUM_CONTEXTS; ++i)
	{
	  if (!is_active (i))
	    {
	      /* check if interrupts are enabled and we have triggered one */
	      if (get_h_glbl_ctl_interrupt_enable_bit () &&
		  (*irq_status_0 & get_h_irq_mask_0_c (i)))
		try_to_activate (i);
	    }
	}
    }
}

void
ubicom32bf_cpu_cgen::set_h_intclr_0 (USI bit_mask)
{
  /* clear bits specified as 1 in bit_mask */
  USI *irq_status_0 = (USI *)&(this->common_register_file[(ubicom32bf_cpu_cgen::IRQ_STATUS_0_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  *irq_status_0 &= ~bit_mask;
}

USI
ubicom32bf_cpu_cgen::get_h_irq_status_1 () const
{
  SI data = (this->common_register_file[(ubicom32bf_cpu_cgen::IRQ_STATUS_1_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);

  return (USI) data;
}

void
ubicom32bf_cpu_cgen::set_h_irq_status_1 (USI value)
{
  (this->common_register_file[(ubicom32bf_cpu_cgen::IRQ_STATUS_1_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]) = value;
}

void
ubicom32bf_cpu_cgen::set_h_intset_1 (USI bit_mask)
{
  // check for a syscall on the current thread. Hadware simulations will set syscall_trap_num to zero.
  if (syscall_trap_num)
    {
      if (bit_mask & (1 << (syscall_trap_num - 32)))
	{
	  do_syscall ();
	  return;
	}
    }
  // else if changing the irq_status_1 register, check if any inactive contexts
  // are made active due to an unmasked interrupt condition
  USI *irq_status_1 = (USI *)&(this->common_register_file[(ubicom32bf_cpu_cgen::IRQ_STATUS_1_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  if (*irq_status_1 ^ bit_mask)
    {
      *irq_status_1 |= bit_mask;
      for (int i = 0; i < NUM_CONTEXTS; ++i)
	{
	  if (!is_active (i))
	    {
	      /* check if interrupts are enabled and we have triggered one */
	      if (get_h_glbl_ctl_interrupt_enable_bit () &&
		  (*irq_status_1 & get_h_irq_mask_1_c (i)))
		try_to_activate (i);
	    }
	}
    }
}

void
ubicom32bf_cpu_cgen::set_h_intclr_1 (USI bit_mask)
{
  /* clear bits specified as 1 in bit_mask */
  USI *irq_status_1 = (USI *)&(this->common_register_file[(ubicom32bf_cpu_cgen::IRQ_STATUS_1_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  *irq_status_1 &= ~bit_mask;
}

USI
ubicom32bf_cpu_cgen::get_h_glbl_ctl () const
{
  return (USI )(this->common_register_file[(ubicom32bf_cpu_cgen::GLBL_CTL_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
}

void
ubicom32bf_cpu_cgen::set_h_glbl_ctl (USI value)
{
  USI old_value = get_h_glbl_ctl ();

  if (value != old_value)
    {
      USI masked_value;

#if 0
      masked_value = value & GLBL_CTL_TIMER_ENABLE_BIT_MASK;
      if (masked_value != (old_value & GLBL_CTL_TIMER_ENABLE_BIT_MASK))
	{
	  // indicate a timer enable change
	  signal_timer_enable_change (masked_value != 0);
	}
      masked_value = value & GLBL_CTL_WATCHDOG_ENABLE_BIT_MASK;
      if (masked_value != (old_value & GLBL_CTL_WATCHDOG_ENABLE_BIT_MASK))
	{
	  // indicate a watchdog enable change
          signal_timer_enable_change (masked_value != 0);
        }
#endif

      // if changing hrt table, must rebuild the internal hrt table
      masked_value = value & GLBL_CTL_HRT_TABLE_SELECT_BIT_MASK;

#if 0
      if (masked_value != (old_value & GLBL_CTL_HRT_TABLE_SELECT_BIT_MASK) &&
          hrt_count > 0)
        build_hrt_table();
#endif

      // if enabling interrupte, check if any threads are activated */
      masked_value = value & GLBL_CTL_INTERRUPT_ENABLE_BIT_MASK;
      if (masked_value && !(old_value & GLBL_CTL_HRT_TABLE_SELECT_BIT_MASK))
	{
	  for (int i = 0; i < NUM_CONTEXTS; ++i)
	    {
	      if (!is_active (i))
		{
		  /* check if interrupt will activate thread */
		  if ((get_h_irq_status_0() & get_h_irq_mask_0_c(i)) ||
		      (get_h_irq_status_1() & get_h_irq_mask_1_c(i)))
		    try_to_activate (i);
		}
	    }
	}

      (this->common_register_file[(ubicom32bf_cpu_cgen::GLBL_CTL_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]) = value;
      this->current_delay = (value & GLBL_CTL_MIN_DELAY_MASK) >> GLBL_CTL_MIN_DELAY_SHIFT;
    }
}

USI
ubicom32bf_cpu_cgen::get_h_glbl_ctl_interrupt_enable_bit () const
{
  return (get_h_glbl_ctl () & GLBL_CTL_INTERRUPT_ENABLE_BIT_MASK) != 0;
}

USI
ubicom32bf_cpu_cgen::get_h_glbl_ctl_ierase_enable_bit () const
{
  return (get_h_glbl_ctl () & GLBL_CTL_IERASE_ENABLE_BIT_MASK) != 0;
}

USI
ubicom32bf_cpu_cgen::get_h_glbl_ctl_hrt_table_select_bit () const
{
  return (get_h_glbl_ctl () & GLBL_CTL_HRT_TABLE_SELECT_BIT_MASK) != 0;
}

#if 0
UHI
ubicom32bf_cpu_cgen::get_h_tctrl () const
{
  return (this->get_h_tctrl_w () >> 16);
}

USI
ubicom32bf_cpu_cgen::get_h_tctrl_w () const
{
  return this->h_tctrl_w;
}

void
ubicom32bf_cpu::set_h_tctrl (UHI value)
{
  USI old_value = get_h_tctrl_w ();
  USI new_value = (old_value & 0xffff) | (value << 16);
  set_h_tctrl_w (new_value);
}

void
ubicom32bf_cpu::set_h_tctrl_w (USI value)
{
  UHI old_value = get_h_tctrl ();
  UHI new_value = value >> 16;
  // mask so certain bits are left as 0
  USI masked_value = value & TCTRL_W_WRITE_MASK;

  // only bother to check bits if value has changed
  if (old_value != new_value)
    {
      // FIXME: how is timer enabled...check if timers are enabled at all before performing a reset
      if (1)
	{
	  // check if any of the timers are to be reset
	  if (value & TCTRL_T1RST_MASK)
	    this->tctrl_t1rst_pin.drive (1);
	  if (value & TCTRL_T2RST_MASK)
	    this->tctrl_t2rst_pin.drive (1);
	  if (value & TCTRL_T3RST_MASK)
	    this->tctrl_t3rst_pin.drive (1);
	  if (value & TCTRL_T4RST_MASK)
	    this->tctrl_t4rst_pin.drive (1);
	  
	}
      // check if any of the interrupt enable bits have changed
      if ((old_value ^ new_value) & TCTRL_T1IE_MASK)
	this->tctrl_t1ie_pin.drive ((new_value & TCTRL_T1IE_MASK) != 0);
      if ((old_value ^ new_value) & TCTRL_T2IE_MASK)
	this->tctrl_t2ie_pin.drive ((new_value & TCTRL_T2IE_MASK) != 0);
      if ((old_value ^ new_value) & TCTRL_T3IE_MASK)
	this->tctrl_t3ie_pin.drive ((new_value & TCTRL_T3IE_MASK) != 0);
      if ((old_value ^ new_value) & TCTRL_T4IE_MASK)
	this->tctrl_t4ie_pin.drive ((new_value & TCTRL_T4IE_MASK) != 0);
    }
  this->h_tctrl_w = masked_value;
}
#endif


USI
ubicom32bf_cpu_cgen::get_h_mt_enable () const
{
  return (this->common_register_file[(ubicom32bf_cpu_cgen::MT_ENABLE_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
}

void
ubicom32bf_cpu_cgen::set_h_mt_enable (USI value)
{
  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_ENABLE_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]) = value&((1<<NUM_CONTEXTS)-1);
}

USI
ubicom32bf_cpu_cgen::get_h_mt_active () const
{
  return (this->common_register_file[(ubicom32bf_cpu_cgen::MT_ACTIVE_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
}

void
ubicom32bf_cpu_cgen::set_h_mt_active (USI value)
{
  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_ACTIVE_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]) = value&((1<<NUM_CONTEXTS)-1);
}

void
ubicom32bf_cpu_cgen::set_h_mt_active_set (USI bit_mask)
{
  set_h_mt_active (get_h_mt_active() | bit_mask);
}

void
ubicom32bf_cpu_cgen::set_h_mt_active_clr (USI bit_mask)
{
  /* clear bits specified as 1 in bit_mask */
  set_h_mt_active (get_h_mt_active() & ~bit_mask);
}

USI
ubicom32bf_cpu_cgen::get_h_mt_dbg_active () const
{
  return (this->common_register_file[(ubicom32bf_cpu_cgen::MT_DBG_ACTIVE_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
}

void
ubicom32bf_cpu_cgen::set_h_mt_dbg_active (USI value)
{
  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_DBG_ACTIVE_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]) = value&((1<<NUM_CONTEXTS)-1);

}

void
ubicom32bf_cpu_cgen::set_h_mt_dbg_active_set (USI bit_mask)
{
  set_h_mt_dbg_active (get_h_mt_dbg_active() | bit_mask);
}

void
ubicom32bf_cpu_cgen::set_h_mt_dbg_active_clr (USI bit_mask)
{
  set_h_mt_dbg_active (get_h_mt_dbg_active() & ~bit_mask);
}

void
ubicom32bf_cpu_cgen::suspend_context (int context_num)
{
  /* suspend only if there is no pending interrupt */
  int pending_interrupt;
  if (get_h_glbl_ctl() & GLBL_CTL_INTERRUPT_ENABLE_BIT_MASK)
    {
      pending_interrupt = 
	(get_h_irq_status_0() & get_h_irq_mask_0_c(context_num)) ||
	(get_h_irq_status_1() & get_h_irq_mask_1_c(context_num));
    }
  else
    {
      pending_interrupt = 0;
    }

  fflush(stdout);
  if (!pending_interrupt)
    {
      long mask = 1 << context_num;
      set_h_mt_active (get_h_mt_active() & ~mask);
    }
}

USI
ubicom32bf_cpu_cgen::get_h_mt_priority () const
{
  return (this->common_register_file[(ubicom32bf_cpu_cgen::MT_PRIORITY_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
}

void
ubicom32bf_cpu_cgen::set_h_mt_priority (USI value)
{
  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_PRIORITY_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]) = value&((1<<NUM_CONTEXTS)-1);
}

USI
ubicom32bf_cpu_cgen::get_h_mt_schedule () const
{
  return (this->common_register_file[(ubicom32bf_cpu_cgen::MT_SCHEDULE_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);;
}


void
ubicom32bf_cpu_cgen::set_h_mt_schedule (USI value)
{
  value&=((1<<NUM_CONTEXTS)-1);
  if (value != (this->common_register_file[(ubicom32bf_cpu_cgen::MT_SCHEDULE_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]))
    {
#if 0
      if (value == 0)
	hrt_count = 0;  // no HRT threads
      else
	build_hrt_table ();
#endif
      (this->common_register_file[(ubicom32bf_cpu_cgen::MT_SCHEDULE_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]) = value;
    }
}

USI
ubicom32bf_cpu_cgen::get_h_mt_break () const
{
  return (this->common_register_file[(ubicom32bf_cpu_cgen::MT_BREAK_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
}

void
ubicom32bf_cpu_cgen::set_h_mt_break (USI value)
{
  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_BREAK_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]) = value&((1<<NUM_CONTEXTS)-1);

  if (value != 0)
    set_h_intset_1 (IRQ_1_BKPT_MASK);
}

void
ubicom32bf_cpu_cgen::set_h_mt_break_clr (USI mask)
{
  set_h_mt_break (get_h_mt_break() & ~mask);
}

USI
ubicom32bf_cpu_cgen::get_h_mt_single_step () const
{
  return (this->common_register_file[(ubicom32bf_cpu_cgen::MT_SINGLESTEP_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
}

void
ubicom32bf_cpu_cgen::set_h_mt_single_step (USI value)
{
  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_SINGLESTEP_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]) = value&((1<<NUM_CONTEXTS)-1);
}

USI
ubicom32bf_cpu_cgen::get_h_dcapt () const
{
  return (this->common_register_file[(ubicom32bf_cpu_cgen::DC_CAPTURE_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
}

void
ubicom32bf_cpu_cgen::set_h_dcapt (USI value)
{
  (this->common_register_file[(ubicom32bf_cpu_cgen::DC_CAPTURE_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]) = value;
}

USI
ubicom32bf_cpu_cgen::get_h_dcapt_pc () const
{
  return (this->common_register_file[(ubicom32bf_cpu_cgen::DC_CAPTURE_PC_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
}

void
ubicom32bf_cpu_cgen::set_h_dcapt_pc (USI value)
{
  (this->common_register_file[(ubicom32bf_cpu_cgen::DC_CAPTURE_PC_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]) = value;
}

USI
ubicom32bf_cpu_cgen::get_h_dcapt_thread () const
{
  return (this->common_register_file[(ubicom32bf_cpu_cgen::DC_CAPTURE_THREAD_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
}

void
ubicom32bf_cpu_cgen::set_h_dcapt_thread (USI value)
{
  (this->common_register_file[(ubicom32bf_cpu_cgen::DC_CAPTURE_THREAD_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]) = value;
}

#if 0
USI
ubicom32bf_cpu_cgen::get_h_watchdog () const
{
  return this->h_watchdog;
}

void
ubicom32bf_cpu_cgen::set_h_watchdog (USI value)
{
  this->h_watchdog = value;
}

USI
ubicom32bf_cpu_cgen::get_h_clock_ctl () const
{
  return this->h_clock_ctl;
}

void
ubicom32bf_cpu_cgen::set_h_clock_ctl (USI value)
{
  this->h_clock_ctl = value;
}

USI
ubicom32bf_cpu_cgen::get_h_random_seed () const
{
  /* we fake a random generator by using the
     code from newlib;s rand_r function.
    
     Pseudo-random generator based on Minimal Standard by
     Lewis, Goodman, and Miller in 1969.
 
     I[j+1] = a*I[j] (mod m)
     
     where a = 16807
     m = 2147483647
     
     Using Schrage's algorithm, a*I[j] (mod m) can be rewritten as:
     
     a*(I[j] mod q) - r*{I[j]/q}      if >= 0
     a*(I[j] mod q) - r*{I[j]/q} + m  otherwise
     
     where: {} denotes integer division 
     q = {m/a} = 127773 
     r = m (mod a) = 2836
     
     note that the seed value of 0 cannot be used in the calculation as
     it results in 0 itself
  */
  long k;
  long s = (long)h_random_seed;
  if (s == 0)
    s = 0x12345987;
  k = s / 127773;
  s = 16807 * (s - k * 127773) - 2836 * k;
  if (s < 0)
    s += 2147483647;
  this->h_random_seed = (USI)s;
  return (USI)(s & 0x7fffffff);
}

void
ubicom32bf_cpu_cgen::set_h_random_seed (USI value)
{
  this->h_random_seed = value;
}
#endif

void
ubicom32bf_cpu_cgen::set_h_perr_addr (USI value)
{
  (this->common_register_file[(ubicom32bf_cpu_cgen::PERR_ADDR_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]) = value;
}

USI
ubicom32bf_cpu_cgen::get_h_perr_addr () const
{
  return (this->common_register_file[(ubicom32bf_cpu_cgen::PERR_ADDR_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
}

USI
ubicom32bf_cpu_cgen::get_h_count () const
{
  return this->register_file[context][ubicom32bf_cpu_cgen::CONTEXT_COUNT_INDEX];
}

void
ubicom32bf_cpu_cgen::set_h_count (int context_num, USI value)
{
  this->register_file[context_num][ubicom32bf_cpu_cgen::CONTEXT_COUNT_INDEX] = value;
}

void
ubicom32bf_cpu_cgen::inc_h_count ()
{
  ++this->register_file[context][ubicom32bf_cpu_cgen::CONTEXT_COUNT_INDEX];
}

USI
ubicom32bf_cpu_cgen::get_h_mt_min_delay_enable () const
{
  return (this->common_register_file[(ubicom32bf_cpu_cgen::MT_MIN_DELAY_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
}

void
ubicom32bf_cpu_cgen::set_h_mt_min_delay_enable (USI value)
{
  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_MIN_DELAY_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]) = value&((1<<NUM_CONTEXTS)-1);
}

// thread routines
int
ubicom32bf_cpu_cgen::is_enabled_m (int mask)
{
  return (get_h_mt_enable () & mask) != 0;
}

int
ubicom32bf_cpu_cgen::is_active_m (int mask)
{
  return is_enabled_m (mask) && ((get_h_mt_active () & mask) != 0) && 
    ((get_h_mt_dbg_active () & mask) != 0);
}

int
ubicom32bf_cpu_cgen::is_active (int context_value)
{
  int mask = (1 << context_value);
  return is_active_m (mask);
}

void
ubicom32bf_cpu_cgen::try_to_activate (int context_num)
{
  int mask = (1 << context_num);
  set_h_mt_active (get_h_mt_active () | mask);
}

int
ubicom32bf_cpu_cgen::is_active_srt (int context_value)
{
  int mask = (1 << context_value);
  return is_active_m (mask) && ((get_h_mt_schedule () & mask) == 0)
    && ((get_h_mt_priority () & mask) == 0);
}

int
ubicom32bf_cpu_cgen::is_active_hp_srt (int context_value)
{
  int mask = (1 << context_value);
  return is_active_m (mask) && ((get_h_mt_schedule () & mask) == 0)  &&
    ((get_h_mt_priority () & mask) != 0);
}

int
ubicom32bf_cpu_cgen::have_srt_hp_threads ()
{
  return (get_h_mt_priority() != 0);
}

int
ubicom32bf_cpu_cgen::is_past_delay (int context_value)
{
  int delay_enable = this->get_h_mt_min_delay_enable ();
  if (delay_enable != 0)
    {
      int mask = (1 << context_value);
      if (delay_enable & mask)
	return (this->delay_table[context_value] > this->current_delay);
    }
  return true;
}
	  
void
ubicom32bf_cpu_cgen::inc_delay_table ()
{
  for (int i = 0; i < 8; ++i)
    {
      if (this->delay_table[i] < MAX_DELAY)
	++this->delay_table[i];
    }
}

#if 0
void
ubicom32bf_cpu::build_hrt_table ()
{
  char *ptr = this->hrt_table;
  USI mem_ptr;

  if (cpu_type >= 3)
    {
      mem_ptr = (get_h_glbl_ctl_hrt_table_select_bit() ? 
		 (USI)(0xb8000800): 
		 (USI)(0xb8000900));
    }
  else
    {
      mem_ptr= (get_h_glbl_ctl_hrt_table_select_bit() ? 
		(USI)(ubicom32bf_cpu_cgen::HRT_TABLE_ADDR_2): 
		(USI)(ubicom32bf_cpu_cgen::HRT_TABLE_ADDR_1));
    }

  hrt_index = 0; 

  for (int i = 0; i < HRT_MAX_ENTRIES; ++i)
    {
      try 
	{
	  *ptr = GETMEMQI (get_h_pc(), mem_ptr);
	  ++hrt_count;
	  ++mem_ptr;
	  if (is_last (*ptr++))
	    return;
	}
      catch (cpu_memory_fault& t)
	{
	  this->memory_trap (t);
	  this->yield ();
	}
    }

  // if we get here then we did not find the last entry
  cerr << "Invalid HRT table without last entry" << endl;
}
#endif

void
ubicom32bf_cpu::do_syscall ()
{
  // must get current context argument registers and place them in syscall argument registers
  syscall_arg0 = get_h_dr(1);
  syscall_arg1 = get_h_dr(2);
  syscall_arg2 = get_h_dr(3);
  syscall_arg3 = get_h_dr(4);

  this->set_h_pc (this->h_pc_get());

  cpu_trap_disposition whatnext = this->signal_trap (cpu_trap_software, syscall_trap_num);
  switch (whatnext)
    {
    case cpu_trap_unhandled:
      cerr << "hw-cpu-ubicom32: invalid insn handler unimplemented!" << endl;
      return;
      
    case cpu_trap_reissue:
      break;

    case cpu_trap_skip:
      this->h_pc_set (this->h_pc_get() + 4);
      /* fall-through */

    case cpu_trap_handled:
      {
	// must get syscall results and place them in current context registers
	string errAttr = this->attribute_value ("syscall-error");
	string resultAttr = this->attribute_value ("syscall-result");
	
	host_int_4 errValue;
	host_int_4 resultValue;
	parse_attribute(errAttr, errValue);
	parse_attribute(resultAttr, resultValue);
	
	set_h_dr_internal (5, errValue);
	set_h_dr_internal (0, resultValue);
      }
      return;
      
    default:
      abort ();
    }
  
  this->yield();
  throw cpu_exception ();
}

// pin support routines
void
ubicom32bf_cpu::do_scheduler_pin(host_int_4 value)
{
  (void)scheduler();
}

void
ubicom32bf_cpu::do_execute_pin(host_int_4 value)
{
  execute();
}

void
ubicom32bf_cpu::do_watchdog_trigger_pin (host_int_4 value)
{
  if (!value)
    return;

  // pin has been driven high

  // indicate a watchdog reset
  this->watchdog_reset_pin.drive (1);
}

// hardware interrupt pins

void
ubicom32bf_cpu::do_mbox_intr_pin (host_int_4 value)
{
  if (!value)
    return;

  // pin has been driven high

  // set the mbox interrupt bit in intset_1
  set_h_intset_1 (IRQ_1_MBOX_MASK);
}

void
ubicom32bf_cpu::do_rttmr_intr_pin (host_int_4 value)
{
  if (!value)
    return;

  // pin has been driven high

  // set the mbox interrupt bit in intset_1
  set_h_intset_1 (IRQ_1_RT_TIMER_MASK);
}

void
ubicom32bf_cpu::do_irq_pin (host_int_4 value, host_int_4 pin_number)
{
  if (!value)
    return;

  // pin has been driven high

  // set the irq status bit
  set_h_intset_0 (1 << pin_number);
}

void
ubicom32bf_cpu::do_irq_0_pin (host_int_4 value)
{
  do_irq_pin (value, 0);
}

void
ubicom32bf_cpu::do_irq_1_pin (host_int_4 value)
{
  do_irq_pin (value, 1);
}

void
ubicom32bf_cpu::do_irq_2_pin (host_int_4 value)
{
  do_irq_pin (value, 2);
}

void
ubicom32bf_cpu::do_irq_3_pin (host_int_4 value)
{
  do_irq_pin (value, 3);
}

void
ubicom32bf_cpu::do_irq_4_pin (host_int_4 value)
{
  do_irq_pin (value, 4);
}

void
ubicom32bf_cpu::do_irq_5_pin (host_int_4 value)
{
  do_irq_pin (value, 5);
}

void
ubicom32bf_cpu::do_irq_6_pin (host_int_4 value)
{
  do_irq_pin (value, 6);
}

void
ubicom32bf_cpu::do_irq_7_pin (host_int_4 value)
{
  do_irq_pin (value, 7);
}

void
ubicom32bf_cpu::do_irq_8_pin (host_int_4 value)
{
  do_irq_pin (value, 8);
}

void
ubicom32bf_cpu::do_irq_9_pin (host_int_4 value)
{
  do_irq_pin (value, 9);
}

void
ubicom32bf_cpu::do_irq_10_pin (host_int_4 value)
{
  do_irq_pin (value, 10);
}

void
ubicom32bf_cpu::do_irq_11_pin (host_int_4 value)
{
  do_irq_pin (value, 11);
}

void
ubicom32bf_cpu::do_irq_12_pin (host_int_4 value)
{
  do_irq_pin (value, 12);
}

void
ubicom32bf_cpu::do_irq_13_pin (host_int_4 value)
{
  do_irq_pin (value, 13);
}

void
ubicom32bf_cpu::do_irq_14_pin (host_int_4 value)
{
  do_irq_pin (value, 14);
}

void
ubicom32bf_cpu::do_irq_15_pin (host_int_4 value)
{
  do_irq_pin (value, 15);
}

void
ubicom32bf_cpu::do_irq_16_pin (host_int_4 value)
{
  do_irq_pin (value, 16);
}

void
ubicom32bf_cpu::do_irq_17_pin (host_int_4 value)
{
  do_irq_pin (value, 17);
}

void
ubicom32bf_cpu::do_irq_18_pin (host_int_4 value)
{
  do_irq_pin (value, 18);
}

void
ubicom32bf_cpu::do_irq_19_pin (host_int_4 value)
{
  do_irq_pin (value, 19);
}

void
ubicom32bf_cpu::do_irq_20_pin (host_int_4 value)
{
  do_irq_pin (value, 20);
}

void
ubicom32bf_cpu::do_irq_21_pin (host_int_4 value)
{
  do_irq_pin (value, 21);
}

void
ubicom32bf_cpu::do_irq_22_pin (host_int_4 value)
{
  do_irq_pin (value, 22);
}

void
ubicom32bf_cpu::do_irq_23_pin (host_int_4 value)
{
  do_irq_pin (value, 23);
}

void
ubicom32bf_cpu::do_irq_24_pin (host_int_4 value)
{
  do_irq_pin (value, 24);
}

void
ubicom32bf_cpu::do_irq_25_pin (host_int_4 value)
{
  do_irq_pin (value, 25);
}

void
ubicom32bf_cpu::do_irq_26_pin (host_int_4 value)
{
  do_irq_pin (value, 26);
}

void
ubicom32bf_cpu::do_irq_27_pin (host_int_4 value)
{
  do_irq_pin (value, 27);
}

void
ubicom32bf_cpu::do_irq_28_pin (host_int_4 value)
{
  do_irq_pin (value, 28);
}

void
ubicom32bf_cpu::do_irq_29_pin (host_int_4 value)
{
  do_irq_pin (value, 29);
}

void
ubicom32bf_cpu::do_irq_30_pin (host_int_4 value)
{
  do_irq_pin (value, 30);
}

void
ubicom32bf_cpu::do_irq_31_pin (host_int_4 value)
{
  do_irq_pin (value, 31);
}

// software interrupt pins
void
ubicom32bf_cpu::do_irq_math_coprocessor_completion_pin (host_int_4 value)
{
  if (!value)
    return;

  // pin has been driven high

  // set the irq status bit
  set_h_intset_1 (IRQ_1_MATH_COPROCESSOR_COMPLETION_MASK);
}

void
ubicom32bf_cpu::do_irq_math_coprocessor_exception_pin (host_int_4 value)
{
  if (!value)
    return;

  // pin has been driven high

  // set the irq status bit
  set_h_intset_1 (IRQ_1_MATH_COPROCESSOR_EXCEPTION_MASK);
}

// Instruction support routines
int
ubicom32bf_cpu::check_mask (UINT use_32_bit_flags, UINT cc)
{
  int cbit, vbit, nbit, zbit;
  USI csr = get_h_cc_csr ();

  if (use_32_bit_flags)
    {
      cbit = (csr & CSR_CBIT_32_MASK) != 0;
      vbit = (csr & CSR_VBIT_32_MASK) != 0;
      nbit = (csr & CSR_NBIT_32_MASK) != 0;
      zbit = (csr & CSR_ZBIT_32_MASK) != 0;
    }
  else
    {
      cbit = (csr & CSR_CBIT_16_MASK) != 0;
      vbit = (csr & CSR_VBIT_16_MASK) != 0;
      nbit = (csr & CSR_NBIT_16_MASK) != 0;
      zbit = (csr & CSR_ZBIT_16_MASK) != 0;
    }
  
  switch (cc)
    {
    case CC_F:
      return 0;
    case CC_T:
      return 1;
    case CC_C:
      return (cbit);
    case CC_NC:
      return (cbit == 0);
    case CC_NZ:
      return (zbit == 0);
    case CC_Z:
      return (zbit);
    case CC_GE:
      return ((nbit ^ vbit) == 0);
    case CC_LT:
      return (nbit ^ vbit);
    case CC_GT:
      return ((zbit | 
	       (nbit ^ vbit)) == 0);
    case CC_LE:
      return (zbit | (nbit ^ vbit));
    case CC_LS:
      return (!cbit | zbit);
    case CC_HI:
      return ((!cbit | zbit) == 0);
    case CC_MI:
      return (nbit);
    case CC_PL:
      return (nbit == 0);
    case CC_V:
      return (vbit);
    case CC_NV:
      return (vbit == 0);
    }
  
  /* should never get here */
  abort ();
  return 0;
}

SF
ubicom32bf_cpu_cgen::get_acc_fpu_32(UINT reg_no) const
{
  USI regval;
  SF *regptr = (SF *) &regval;
  switch(reg_no)
    {
    case 0:
      regval = get_h_mac_lo();
      break;
    case 1:
      regval = get_h_mac_hi();
      break;
    case 2:
      regval = get_h_acc1_lo();
      break;
    case 3:
      regval = get_h_acc1_hi();
      break;
    }

  return *regptr;
}

void
ubicom32bf_cpu_cgen::set_acc_fpu_32(UINT reg_no, SF newval)
{
  USI *regval = (USI *) &newval;
  switch(reg_no)
    {
    case 0:
      set_h_mac_lo(*regval);
      break;
    case 1:
      set_h_mac_hi(*regval);
      break;
    case 2:
      set_h_acc1_lo(*regval);
      break;
    case 3:
      set_h_acc1_hi(*regval);
      break;
    }
}

DF
ubicom32bf_cpu_cgen::get_acc_fpu_64(UINT index) const
{
  DI tmp1, tmp2;
  if(index == 0) {
    tmp1 = get_h_mac_hi();
    tmp2 = get_h_mac_lo();
  } else {
    tmp1 = get_h_acc1_hi();
    tmp2 = get_h_acc1_lo();
  }

  DI result = (((DI) tmp1)<<32 | ((DI)tmp2));

  //printf("accfpu64 hi=0x%x lo=0x%x result = %lx\n", tmp1, tmp2, result);
  DF *regptr = (DF *) &result;

  return *regptr;
}

void
ubicom32bf_cpu_cgen::set_acc_fpu_64(UINT reg_no, DF newval)
{
  UDI *regval = (UDI *) &newval;
  USI tmp2 = (*regval >> 32) & 0xffffffff;
  USI tmp1 = *regval & 0xffffffff;

  switch(reg_no)
    {
    case 0:
      set_h_mac_lo(tmp1);
      set_h_mac_hi(tmp2);
      break;
    case 1:
      set_h_acc1_lo(tmp1);
      set_h_acc1_hi(tmp2);
      break;
    }
}

SF
ubicom32bf_cpu_cgen::get_fpu_32_from_data_reg(UINT reg_no) const
{
  USI regval;
  SF *regptr = (SF *) &regval;

  regval = get_h_dr(reg_no);
  return *regptr;
}

DF
ubicom32bf_cpu_cgen::get_fpu_64_from_data_reg(UINT reg_no) const
{
  USI tmp1, tmp2;
  host_int_4 index=reg_no*2;
  tmp1 = (USI) get_h_dr(index);
  index++;
  tmp2 = (USI) get_h_dr(index);
  UDI result = (((UDI) tmp1)<<32 | ((UDI)tmp2));
  DF *regptr = (DF *) &result;

  return *regptr;
}

void
ubicom32bf_cpu::setup_rounding(void)
{
  USI round_mode = (get_h_cc_csr () >> UCSR_FPRM_SHIFT) & 0x3;

  USI i386_rm;

  switch(round_mode) {
  case UCSR_FPRM_ROUND_NEAREST:
    i386_rm = FE_TONEAREST;
    break;
  case UCSR_FPRM_ROUND_ZERO:
    i386_rm = FE_TOWARDZERO;
    break;
  case UCSR_FPRM_ROUND_POSITIVE:
    i386_rm = FE_UPWARD;
    break;
  case UCSR_FPRM_ROUND_NEGATIVE:
    i386_rm = FE_DOWNWARD;
    break;
  }

  //printf("\nRounding = 0x%2x 386rm = 0x%2x\n", round_mode, i386_rm);

  feclearexcept(FE_ALL_EXCEPT);
  fesetround(i386_rm);
}

void
ubicom32bf_cpu::set_fpu_exception_flags(void)
{
  fexcept_t ex_flags;
  int fp_ret = fegetexceptflag(&ex_flags, (FE_ALL_EXCEPT));
  USI fpu_trap_enable = (get_h_cc_csr () >> UCSR_FPTE_SHIFT) & 0x1f;

  USI exceptions = get_h_cc_csr();

  //printf("EX_flags = 0x%2x\n", ex_flags);
  if (ex_flags &FE_INVALID)
    exceptions |= UCSR_FPESB_INVALID;
  if (ex_flags &FE_DIVBYZERO)
    exceptions |= UCSR_FPESB_DIV0;
  if (ex_flags &FE_OVERFLOW)
    exceptions |= UCSR_FPESB_OVERFLOW;
  if (ex_flags &FE_UNDERFLOW)
    exceptions |= UCSR_FPESB_UNDERFLOW;
  if (ex_flags &FE_INEXACT)
    exceptions |= UCSR_FPESB_INEXACT;

  set_h_cc_csr(exceptions);

  /*
   * Set the proper bit in the trap cause register and create a trap.
   */
}

SF
ubicom32bf_cpu::fadds(SI source1, SF source2)
{
  float *s1, *s2;
  (void) fesetprecision(FE_FLTPREC);
  setup_rounding();

  s1 = (float *)&source1;
  s2 = (float *)&source2;
  *s2 += *s1;
  set_fpu_exception_flags();

  return (source2);
}

DF
ubicom32bf_cpu::faddd(DF source1, DF source2)
{
  double *s1, *s2;
  (void) fesetprecision(FE_DBLPREC);
  setup_rounding();

  s1 = (double *)&source1;
  s2 = (double *)&source2;
  *s1 += *s2;

  set_fpu_exception_flags();
  return (source1);
}

SF
ubicom32bf_cpu::fsubs(SI source1, SF source2)
{
  float *s1, *s2;
  (void) fesetprecision(FE_FLTPREC);
  setup_rounding();

  s1 = (float *)&source1;
  s2 = (float *)&source2;
  *s2 = *s1 - *s2;
  set_fpu_exception_flags();

  return (source2);
}

DF
ubicom32bf_cpu::fsubd(DF source1, DF source2)
{
  double *s1, *s2;
  (void) fesetprecision(FE_DBLPREC);
  setup_rounding();

  s1 = (double *)&source1;
  s2 = (double *)&source2;
  *s1 -= *s2;

  set_fpu_exception_flags();
  return (source1);
}

SF
ubicom32bf_cpu::fmuls(SI source1, SF source2)
{
  float *s1, *s2;
  (void) fesetprecision(FE_FLTPREC);
  setup_rounding();

  s1 = (float *)&source1;
  s2 = (float *)&source2;
  *s2 = *s1 * *s2;
  set_fpu_exception_flags();

  return (source2);
}

DF
ubicom32bf_cpu::fmuld(DF source1, DF source2)
{
  double *s1, *s2;
  (void) fesetprecision(FE_DBLPREC);
  setup_rounding();

  s1 = (double *)&source1;
  s2 = (double *)&source2;
  *s1 *= *s2;

  set_fpu_exception_flags();
  return (source1);
}

SF
ubicom32bf_cpu::fdivs(SI source1, SF source2)
{
  float *s1, *s2;
  (void) fesetprecision(FE_FLTPREC);
  setup_rounding();

  s1 = (float *)&source1;
  s2 = (float *)&source2;
  *s2 = *s1 / *s2;
  set_fpu_exception_flags();

  return (source2);
}

DF
ubicom32bf_cpu::fdivd(DF source1, DF source2)
{
  double *s1, *s2;
  (void) fesetprecision(FE_DBLPREC);
  setup_rounding();

  s1 = (double *)&source1;
  s2 = (double *)&source2;
  *s1 /= *s2;

  set_fpu_exception_flags();
  return (source1);
}

SF
ubicom32bf_cpu::fsqrts(SI source1)
{
  float *s1, result;
  SF *rptr = (SF *)&result;
  
  (void) fesetprecision(FE_FLTPREC);
  setup_rounding();

  s1 = (float *)&source1;
  result = sqrtf(*s1);
  set_fpu_exception_flags();

  return (*rptr);
}

DF
ubicom32bf_cpu::fsqrtd(DF source1)
{
  double *s1;
  (void) fesetprecision(FE_DBLPREC);
  setup_rounding();

  s1 = (double *)&source1;
  *s1 = sqrt(*s1);

  set_fpu_exception_flags();
  return (source1);
}

SF
ubicom32bf_cpu::fnegs(SI source1)
{
  float *s1, result;
  SF *rptr = (SF *)&result;
  
  (void) fesetprecision(FE_FLTPREC);
  setup_rounding();

  s1 = (float *)&source1;
  result = -*s1;
  set_fpu_exception_flags();

  return (*rptr);
}

DF
ubicom32bf_cpu::fnegd(DF source1)
{
  double *s1;
  (void) fesetprecision(FE_DBLPREC);
  setup_rounding();

  s1 = (double *)&source1;
  *s1 = -*s1;

  set_fpu_exception_flags();
  return (source1);
}

SF
ubicom32bf_cpu::fabss(SI source1)
{
  float *s1, result;
  SF *rptr = (SF *)&result;
  
  (void) fesetprecision(FE_FLTPREC);
  setup_rounding();

  s1 = (float *)&source1;
  result = fabsf(*s1);
  set_fpu_exception_flags();

  return (*rptr);
}

DF
ubicom32bf_cpu::fabsd(DF source1)
{
  double *s1 = (double *)&source1;

  (void) fesetprecision(FE_DBLPREC);
  setup_rounding();

  *s1 = fabs(*s1);

  set_fpu_exception_flags();
  return (source1);
}

SF
ubicom32bf_cpu::fi2s(SI source1)
{
  float result;
  SF *rptr = (SF *)&result;
  
  (void) fesetprecision(FE_FLTPREC);
  setup_rounding();

  result = (float)((long)source1);
  set_fpu_exception_flags();

  return (*rptr);
}

DF
ubicom32bf_cpu::fi2d(SI source1)
{
  double result;
  DF *rptr = (DF *)&result;
  (void) fesetprecision(FE_DBLPREC);
  setup_rounding();

  result = (double)((long)source1);

  set_fpu_exception_flags();
  return (*rptr);
}

SF
ubicom32bf_cpu::fl2s(DF source1)
{
  float result;
  SF *rptr = (SF *)&result;
  long long data = source1;
  (void) fesetprecision(FE_FLTPREC);
  setup_rounding();

  result = (float)(data);
  set_fpu_exception_flags();

  return (*rptr);
}

DF
ubicom32bf_cpu::fl2d(DF source1)
{
  double result;
  DF *rptr = (DF *)&result;
  (void) fesetprecision(FE_DBLPREC);
  setup_rounding();

  result = (double)((long long)source1);

  set_fpu_exception_flags();
  return (*rptr);
}

DF
ubicom32bf_cpu::fs2d(SI source1)
{
  float *s1 = (float *)&source1;
  double result;
  DF *rptr = (DF *)&result;
  (void) fesetprecision(FE_DBLPREC);
  setup_rounding();

  result = (double)(*s1);

  set_fpu_exception_flags();
  return (*rptr);
}

SF
ubicom32bf_cpu::fd2s(DF source1)
{
  double *s1 = (double *)&source1;
  float result;
  SF *rptr = (SF *)&result;
  (void) fesetprecision(FE_DBLPREC);
  setup_rounding();

  result = (float)(*s1);

  set_fpu_exception_flags();
  return (*rptr);
}

SF
ubicom32bf_cpu::fs2i(SI source1)
{
  float *s1 = (float *)&source1;
  SF result;
  
  (void) fesetprecision(FE_FLTPREC);
  setup_rounding();

  result = (int)(*s1);
  set_fpu_exception_flags();

  return (result);
}

SF
ubicom32bf_cpu::fd2i(DF source1)
{
  double *s1 = (double *)&source1;
  SF result;
  
  (void) fesetprecision(FE_DBLPREC);
  setup_rounding();

  result = (int)(*s1);
  set_fpu_exception_flags();

  return (result);
}

DF
ubicom32bf_cpu::fs2l(SI source1)
{
  float *s1 = (float *)&source1;
  long long int result;
  DF *rptr = (DF *)&result;
  
  (void) fesetprecision(FE_FLTPREC);
  setup_rounding();

  result = (long long int)(*s1);
  set_fpu_exception_flags();

  return (*rptr);
}

DF
ubicom32bf_cpu::fd2l(DF source1)
{
  double *s1 = (double *)&source1;
  long long int result;
  DF *rptr = (DF *)&result;
  
  (void) fesetprecision(FE_DBLPREC);
  setup_rounding();

  result = (long long int)(*s1);
  set_fpu_exception_flags();

  return (*rptr);
}

SF
ubicom32bf_cpu::fcmps(SI source1, SF source2)
{
  float *s1 = (float *)&source1;
  float *s2 = (float *)&source2;

  if (*s1 == *s2) {
    /* Set Z clean N and V */
    set_h_zbit_16(1);
    set_h_zbit_32(1);
    set_h_nbit_16(0);
    set_h_nbit_32(0);
    set_h_vbit_16(0);
    set_h_vbit_32(0);
  } else if(*s1 < *s2) {
    /* set N clear V and Z */
    set_h_zbit_16(0);
    set_h_zbit_32(0);
    set_h_nbit_16(1);
    set_h_nbit_32(1);
    set_h_vbit_16(0);
    set_h_vbit_32(0);
  } else if (*s1 > *s2) {
    /* Clear N Z and V */
    set_h_zbit_16(0);
    set_h_zbit_32(0);
    set_h_nbit_16(0);
    set_h_nbit_32(0);
    set_h_vbit_16(0);
    set_h_vbit_32(0);
  } else {
    /* Unordered, Set V clear N and Z */
    set_h_zbit_16(0);
    set_h_zbit_32(0);
    set_h_nbit_16(0);
    set_h_nbit_32(0);
    set_h_vbit_16(1);
    set_h_vbit_32(1);
  }
  return 0;
}

DF
ubicom32bf_cpu::fcmpd(DF source1, DF source2)
{
  double *s1 = (double *)&source1;
  double *s2 = (double *)&source2;

  if (*s1 == *s2) {
    /* Set Z clean N and V */
    set_h_zbit_16(1);
    set_h_zbit_32(1);
    set_h_nbit_16(0);
    set_h_nbit_32(0);
    set_h_vbit_16(0);
    set_h_vbit_32(0);
  } else if(*s1 < *s2) {
    /* set N clear V and Z */
    set_h_zbit_16(0);
    set_h_zbit_32(0);
    set_h_nbit_16(1);
    set_h_nbit_32(1);
    set_h_vbit_16(0);
    set_h_vbit_32(0);
  } else if (*s1 > *s2) {
    /* Clear N Z and V */
    set_h_zbit_16(0);
    set_h_zbit_32(0);
    set_h_nbit_16(0);
    set_h_nbit_32(0);
    set_h_vbit_16(0);
    set_h_vbit_32(0);
  } else {
    /* Unordered, Set V clear N and Z */
    set_h_zbit_16(0);
    set_h_zbit_32(0);
    set_h_nbit_16(0);
    set_h_nbit_32(0);
    set_h_vbit_16(1);
    set_h_vbit_32(1);
  }
  return 0;
}

SI
ubicom32bf_cpu::finish_lshift_2(SI tmp_result, USI tmp_uresult)
{
  if (cpu_type >= 2)
    return (SI) tmp_uresult;
  else
    return tmp_result;
}

UHI
ubicom32bf_cpu::pixel_convert(SI truncate, SI data)
{
  /* Convert ARGB 8888 to RGB 565 */
  UHI red = (data >> 18) & 0x3f;

  if (truncate == 0)
    {
      red ++;
    }
  red >>= 1;
  if (red > 31)
    {
      red = 31;
    }

  UHI green = (data >> 9) & 0x7f;

  if (truncate == 0)
    {
      green ++;
    }
  green >>= 1;
  if (green > 63)
    {
      green = 63;
    }

  UHI blue = (data >> 2) & 0x3f;

  if (truncate == 0)
    {
      blue ++;
    }
  blue >>= 1;
  if (blue > 31)
    {
      blue = 31;
    }

  /* construct the reply */
  UHI result;
  result = (red << 11) | (green << 5) | blue;

  return result;
}

USI
ubicom32bf_cpu::pixel_blend(SI truncate, SI data, SI s2, SI src3)
{
  /* extract Pixel 0 and 1  R G B from data */
  USI p0_r, p0_g, p0_b, p1_r, p1_g, p1_b;
  
  p0_r = (((USI) data) >> 27) & 0x1f;
  p0_g = (((USI) data) >> 21) & 0x3f;
  p0_b = (((USI) data) >> 16) & 0x1f;

  p1_r = (((USI) data) >> 11) & 0x1f;
  p1_g = (((USI) data) >> 5) & 0x3f;
  p1_b = (((USI) data) >> 0) & 0x1f;

  /* extract Pixel 2 and 3  R G B from s2 */
  USI p2_r, p2_g, p2_b, p3_r, p3_g, p3_b;
  
  p2_r = (((USI) s2) >> 27) & 0x1f;
  p2_g = (((USI) s2) >> 21) & 0x3f;
  p2_b = (((USI) s2) >> 16) & 0x1f;

  p3_r = (((USI) s2) >> 11) & 0x1f;
  p3_g = (((USI) s2) >> 5) & 0x3f;
  p3_b = (((USI) s2) >> 0) & 0x1f;

  /* extract Alpha A */
  USI Alpha_Arb, Alpha_Ag;

  Alpha_Arb = (src3 >> 10) & 0x3f;
  Alpha_Ag = (src3 >> 9) & 0x7f;

  /* extract Alpha B */
  USI Alpha_Brb, Alpha_Bg;

  Alpha_Brb = (src3 >> 2) & 0x3f;
  Alpha_Bg = (src3 >> 1) & 0x7f;

  /* Calculate the blended A pixel */
  USI blendA_r, blendA_g, blendA_b;

  blendA_r = p2_r * Alpha_Arb + (64 - Alpha_Arb) * p0_r;
  if (truncate == 0)
    {
      /* do rounding */
      blendA_r += 0x20;
    }
  blendA_r >>= 6;
  
  if (blendA_r > 31)
    {
      blendA_r = 31;
    }

  blendA_g = p2_g * Alpha_Ag + (128 - Alpha_Ag) * p0_g;
  if (truncate == 0)
    {
      /* do rounding */
      blendA_g += 0x40;
    }
  blendA_g >>= 7;

  if (blendA_g > 63)
    {
      blendA_g = 63;
    }

  blendA_b = p2_b * Alpha_Arb + (64 - Alpha_Arb) * p0_b;
  if (truncate == 0)
    {
      /* do rounding */
      blendA_b += 0x20;
    }
  blendA_b >>= 6;
  
  if (blendA_b > 31)
    {
      blendA_b = 31;
    }

  /* Calculate the blended B pixel */
  USI blendB_r, blendB_g, blendB_b;

  blendB_r = p3_r * Alpha_Brb + (64 - Alpha_Brb) * p1_r;
  if (truncate == 0)
    {
      /* do rounding */
      blendB_r += 0x20;
    }
  blendB_r >>= 6;

  if (blendB_r > 31)
    {
      blendB_r = 31;
    }

  //blendB_g = p3_g * Alpha_Bg + (128 - Alpha_Bg) * p1_g;
  blendB_g = 128 * p1_g - (Alpha_Bg * p1_g) + (p3_g * Alpha_Bg);

  if (truncate == 0)
    {
      /* do rounding */
      blendB_g += 0x40;
    }
  blendB_g >>= 7;

  if (blendB_g > 63)
    {
      blendB_g = 63;
    }

  blendB_b = p3_b * Alpha_Brb + (64 - Alpha_Brb) * p1_b;
  if (truncate == 0)
    {
      /* do rounding */
      blendB_b += 0x20;
    }
  blendB_b >>= 6;

  if (blendB_b > 31)
    {
      blendB_b = 31;
    }

  /* Construct the result */
  USI result;
  result = (blendA_r << 27) | (blendA_g << 21) | (blendA_b << 16) | (blendB_r << 11) | (blendB_g << 5) | (blendB_b << 0);
  return result;
}

USI
ubicom32bf_cpu::vertical_interpolation(SI order, SI data, SI s2, SI src3)
{
  /* Extract interpolation specifier.*/
  USI pos = (src3 >> 16) & 0x7;
 
  /* Extract rounding control. */
  USI rnd_ctl=(src3 & (1<<28))>>26;

  USI hi, lo;

  if (order)
    {
      /* swap s1 and s2 order */
      hi = s2;
      lo = data;
    }
  else
    {
      hi = data;
      lo = s2;
    }

  USI p_0, p_1;
  USI result = 0;
  int i;
  for (i=0; i<4; i++) 
    {
      p_0 = (hi >> i*8) & 0xff;
      p_1 = (lo >> i*8) & 0xff;
      
      USI tmp = p_0 * (8 - pos) + p_1 * pos + rnd_ctl;
      tmp >>= 3;
      if (tmp > 255)
	{
	  tmp = 255;
	}
      
      result |= (tmp << i*8);
    }

  return result;
}


USI
ubicom32bf_cpu::horizontal_interpolation(SI order, SI data, SI s2, SI src3)
{
  /* extract offset for the starting byte. */
  USI byte_offset = (src3>>8) & 0x3;

  /* extract rounding control. */
  USI rnd_ctl=(src3 & (1<<24))>>22;

  /* Extract interpolation position. */
  USI pos = src3 & 0x7;

  unsigned char p_data[8], p[5];
  int i;

  /* if swap s1 and s2 order. */
  if (order)
    {
      /* s2 is before s1 */
      for (i=0; i<4; i++) 
	{
	  p_data[i] = (s2 >> (3-i)*8) & 0xff;
	} 
      
      for (i=4; i<8; i++) 
	{
	  p_data[i] = (data >> (7-i)*8) & 0xff;
	}
    }
  else
    {
      /* s1 is before s2 */
      for (i=0; i<4; i++)
	{
	  p_data[i] = (data >> (3-i)*8) & 0xff;
	}

      for (i=4; i<8; i++)
	{
	  p_data[i] = (s2 >> (7-i)*8) & 0xff;
	}
    }

  /* get 5 pixel bytes for interpolation. */
  for (i=0; i<5; i++)
    {
      p[i] = p_data[i+byte_offset];
    }

  /*  horizontal interpolation. */
  USI result = 0;
  for (i=0; i<4; i++) {
    USI tmp = p[i] * (8 - pos) + p[i+1] * pos + rnd_ctl;
    tmp >>= 3;
    
    if (tmp > 255)
      {
	tmp = 255;
      }

    result |= (tmp << (3-i)*8);
  }

  return result;
}

UHI
ubicom32bf_cpu::add_and_saturate(SI upper, SI s1, SI s2, SI src3)
{
  int s1_a, s1_b;

  /* Extract the signed addends from s1. */
  s1_a = (s1) & 0x1ff;
  if (s1_a & 0x100) 
    {
      /* sign bit is set so sign extend it. */
      s1_a |= 0xffffff00;
    }

  s1_b = (s1 >> 16) & 0x1ff;
  if (s1_b & 0x100) 
    {
      /* sign bit is set so sign extend it. */
      s1_b |= 0xffffff00;
    }

  /* Extract the unsigned data form s2 */
  int s2_a, s2_b;
  if (upper)
    {
      s2_a = (s2 >> 16) & 0xff;
      s2_b = (s2 >> 24) & 0xff;
    }
  else
    {
      s2_a = (s2) & 0xff;
      s2_b = (s2 >> 8) & 0xff;
    }

  /* Extract the limits from src3 */
  int hi_limit, lo_limit;
  hi_limit = (src3 >> 8) & 0xff;
  lo_limit = (src3) & 0xff;

  /* Compute A and B values. */
  s2_a += s1_a;
  s2_b += s1_b;

  if (s2_a > hi_limit)
    {
      s2_a = hi_limit;
    }
  else if (s2_a < lo_limit) 
    {
      s2_a = lo_limit;
    }

  if (s2_b > hi_limit)
    {
      s2_b = hi_limit;
    }
  else if (s2_b < lo_limit) 
    {
      s2_b = lo_limit;
    }

  UHI result = (s2_b << 8) | s2_a;

  return result;
}

void
ubicom32bf_cpu::side_effect (USI d_field)
{
  USI address_mode = (d_field >> 8) & 0x7;
  if (address_mode == 2) {
    /*
     * post/pre increment mode. Need to record which address register and how much is the increment.
     */
    USI areg_index = (d_field >> 5) & 0x7;
    SI increment = (d_field & 0xf);
    if (increment >> 3) {
      /*
       * Negative increment.
       */
      increment |= 0xfffffff0;
    }

    /*
     * Scale the increment.
     */
    increment *= 4;
    this->hardware.h_ar_inc[areg_index] = increment;
    this->hardware.h_ar_inc_flag = 1;
  }
}
void
ubicom32bf_cpu::finish_multiply ()
{
  HI rounded_hi = get_h_mac_hi ();
  USI mac_lo = get_h_mac_lo ();

  SI positive=0;

  if((rounded_hi &0x8000) == 0)
    positive = 1;

  /* perform rounding first */
  USI rounded_lo = mac_lo + 0x8000;
  if (rounded_lo < mac_lo)
    rounded_hi += 1;

  /* check for saturation */
  if(positive)
    {
      if(rounded_hi & 0x8000)
	{
	  /* The number rolled from +ve to -ve. Clamp it to 0.999999 */
	  set_h_mac_rc16 (0x00007fff); /* value is >= 1.0 */
	  return;
	}
    }


  if (rounded_hi < 0)
    {
      /* check for negative saturation */
      if (!(rounded_hi == -1 && (rounded_lo & 0x80000000)))
	{
	  set_h_mac_rc16 (0xffff8000); /* value is < -1.0 */
	  return;
	}
    }
  else
    {
      /* check for positive saturation */
      if (rounded_hi != 0 || (rounded_lo & 0x80000000))
	{
	  set_h_mac_rc16 (0x00007fff); /* value is >= 1.0 */
	  return;
	}
    }

  /* want the top 16-bits of mac_lo, sign-extended */
  rounded_lo >>= 16;
  HI tmp = (HI)rounded_lo;
  set_h_mac_rc16 ((SI)tmp);
}

// bfrvrs - reverse bit order of s1 and then right shift result by contents of s2
SI
ubicom32bf_cpu::bfrvrs (SI s2, SI s1)
{
  USI shift = (USI)(s2 & 0x1f);
  USI result = 0;
  for (int i = 0; i < 32; ++i)
    {
      result <<= 1;
      result += s1 & 1;
      s1 >>= 1;
    }

  result >>= shift;
  return (SI)result;
}

// suspend - suspend a thread
void
ubicom32bf_cpu::suspend ()
{
  suspend_context (context);
}

// bkpt - perform breakpoint
void
ubicom32bf_cpu::bkpt (UINT mask)
{
  /* set breakpoint irq bit for current context and halt all threads in mask 
     plus current thread */
  set_h_mt_break (get_h_mt_break() | 1 << context);
  set_h_mt_dbg_active_clr (mask | (1 << context));
}

// unused insns - nop now that illegal insn bit is not used anymore
void
ubicom32bf_cpu::unused_insn ()
{
  /* do nothing for now */
}

// program memory access instructions

// iwrite
void
ubicom32bf_cpu::iwrite (USI pea, SI s1)
{
  try
  {
    // truncate the address before attempting the write
    pea &= (~3);
    this->SETIMEMSI (get_h_pc(), pea, s1);
    flush_icache (); /* in case we have modified insn in cache */
  }
  catch (cpu_memory_fault& t)
  {
    if (this->warnings_enabled)
      cerr << "Ignored iwrite to " << pea << "by thread " << context << endl;
  }
}

// iread
USI
ubicom32bf_cpu::iread (USI pea)
{
  USI value = 0;

  // truncate the address before attempting the read
  pea &= (~3);

  try
    {
      value = this->GETIMEMSI (get_h_pc(), pea); 
    }
  catch (cpu_memory_fault& t)
    {
      if (this->warnings_enabled)
	cerr << "Ignored iread from " << pea << "by thread " << context << endl;
      return this->get_h_iread ();
    }
  return value;
}

// ierase
void
ubicom32bf_cpu::ierase (USI pea)
{
  if (on_chip_flash ())
    {
      // if ierase is not enabled, exit immediately
      if (!get_h_glbl_ctl_ierase_enable_bit ())
	return;
      
      // write special data to the page to erase it
      // we must not use the page number to get the start of the page because
      // the page number skips empty space
      PCADDR addr = (pea >> NUM_PAGE_BITS) << NUM_PAGE_BITS;
      for (int i = 0; i < PAGE_SIZE / SI_SIZE; ++i)
	{
	  try 
	    {
	      this->SETIMEMSI (get_h_pc(), addr, IERASE_DATA);
	      addr += SI_SIZE;
	    }
	  catch (cpu_memory_fault &t)
	    {
	      if (this->warnings_enabled)
		cerr << "Ignored ierase to " << pea << "by thread " << context << endl;
	      break;
	    }
	}
    }
}

// setcsr
void
ubicom32bf_cpu::setcsr (SI newval)
{
  // want to change current context csr
  set_h_csr ((USI)newval);
}

// crcgen - based on algorithm in spec document
USI
ubicom32bf_cpu::crcgen (USI B, USI P)
{
  USI C = get_h_mac_lo ();
  USI S, F, X;
  USI machi = get_h_mac_hi();

  X = (C ^ B) & 0xff;
  F = X;
  for (int i = 0; i < 8; ++i)
    F = (F >> 1)^(F & 1 ? P : 0);

  S = C & 0xff;
  set_h_mac_lo (F ^ (C >> 8));

#if 0
  machi <<= 8;

  machi &= ~0xff;
  machi |= S;
#endif

  machi = (machi >> 8) | (S << 24);
  return machi;
}

// attribute handlers
string
ubicom32bf_cpu::get_csr_attr (unsigned context_num)
{
  return make_attribute(this->get_h_csr_c (context_num));
}

component::status
ubicom32bf_cpu::set_csr_attr (unsigned context_num, const string& state)
{
  USI value;
  component::status rc = parse_attribute(state, value);

  if (rc == component::ok)
    {
      set_h_csr_c (value, context_num);
    }
  return rc;
}

string
ubicom32bf_cpu::get_rosr_attr (unsigned context_num)
{
  return make_attribute(this->get_h_rosr_c (context_num));
}

component::status
ubicom32bf_cpu::set_rosr_attr (unsigned context_num, const string& state)
{
  /* ignore write requests */
  //return component::ok;

  USI value;
  component::status rc = parse_attribute(state, value);

  if (rc == component::ok)
    {
      set_h_rosr_c (value, context_num);
    }
  return rc;

}

string
ubicom32bf_cpu::get_generic_register_attr (SI addr)
{
  try 
    {
      return make_attribute((USI)this->GETMEMSI (get_h_pc(), addr));
    }
  catch (cpu_memory_fault& t)
    {
      this->memory_trap (t);
      this->yield ();
    }
  return string("ATTR-ERR");
}


component::status
ubicom32bf_cpu::set_stat_register_attr (SI addr, const string& state)
{
  USI value;
  component::status rc = parse_attribute(state, value);

  if (rc == component::ok)
    {
      if(addr == IRQ_STATUS_0_ADDR)
	{
	  // Call int clr to Clear the bits that are being set to 0
	  set_h_intclr_0(~value);

	  // Call int set to set the bits that are being set to 1
	  set_h_intset_0 (value);
	}
      else if(addr == IRQ_STATUS_1_ADDR)
	{
	  // Call int clr to Clear the bits that are being set to 0
	  set_h_intclr_1(~value);

	  // Call int set to set the bits that are being set to 1
	  set_h_intset_1 (value);
	}
      else
	return component::bad_value;
    }

  return rc;
}

string
ubicom32bf_cpu::get_current_thread_attr (int *addr)
{
  string attr;
  host_int_4 val4 = context;

  // Change to "target endian".
  big_int_4 v = val4;
  for (unsigned i=0; i<4; i++)
    attr += v.read_byte (i);

  return attr;
}

component::status
ubicom32bf_cpu::set_current_thread_attr (int *ignore, const string& attr)
{
  host_int_4 val4;

  gdbAccess = 1;

  // change from "target endian"
  big_int_4 v;
  for (unsigned i=0; i<4; i++)
    v.write_byte (i, attr[i]);
  val4 = v;


  /* Save current context. */
  set_h_pc (this->hardware.h_pc);
  /* Change context. */
  sidlibContext = context = val4 & 7;
  this->hardware.h_pc = get_h_pc ();

  gdbAccess = 0;
  return component::ok;
}

component::status
ubicom32bf_cpu::set_generic_register_attr (SI addr, const string& state)
{
  USI value;
  component::status rc = parse_attribute(state, value);

  if (rc == component::ok)
    {
      try 
	{
	  this->SETMEMSI (get_h_pc(), addr, value);
	}
      catch (cpu_memory_fault& t)
	{
	  this->memory_trap (t);
	  this->yield ();
	}
    }
  return rc;
}

string
ubicom32bf_cpu::get_harvard_pc_attr ()
{
  // return harvard pc for profiling usage
  return make_attribute(this->hardware.h_pc);
}

component::status
ubicom32bf_cpu::set_harvard_pc_attr (const string& state)
{
  return component::ok;
}

void
ubicom32bf_cpu::flush_icache ()
{
  this->engine.flush ();
}

void
ubicom32bf_cpu::invalid_insn (PCADDR pc)
{
  this->h_pc_set (pc);

  cerr << hex << "XXX: invalid insn @ " << pc << dec << endl;

  cpu_trap_disposition whatnext = this->signal_trap (cpu_trap_invalid_insn, pc);
  switch (whatnext)
    {
    case cpu_trap_unhandled:
      cerr << "hw-cpu-ubicom32: invalid insn handler unimplemented!" << endl;
      break;

    case cpu_trap_skip:
      this->h_pc_set (pc + 4);
      break;

    case cpu_trap_handled:
    case cpu_trap_reissue:
      break;

    default:
      abort ();
    }

  this->yield();
  throw cpu_exception ();
}

// Memory trap(/fault) handling.
void
ubicom32bf_cpu::memory_trap (const cpu_memory_fault& t)
{
  this->h_pc_set (t.pc);
  
  cpu_trap_disposition whatnext = this->signal_trap (cpu_trap_memory_fault, t.address);

  switch (whatnext)
    {
    case cpu_trap_unhandled:
      cerr << "hw-cpu-ubicom32: memory fault handler unimplemented!" << endl;
      break;

    case cpu_trap_skip:
      this->h_pc_set (t.pc + 4);
      break;

    case cpu_trap_handled:
    case cpu_trap_reissue:
      break;

    default:
      abort ();
    }

  this->yield();
}

// Stepper
void
ubicom32bf_cpu::step_insns ()
{
  while (LIKELY(true))
    {
      if (scheduler ())
	{
	  execute();
	  // test for exit condition
	  if (UNLIKELY(stop_after_insns_p (1)))
	    break;
	}
      else
	break;  // no active threads
    }
}

// helper routines

// execution
void
ubicom32bf_cpu::execute()
{
  if(no_active)
    return;

  running_pin.drive (1);

  PCADDR pc = get_h_pc ();

  // Set the previous pc
  set_h_previous_pc(pc);

  bool found;
  ubicom32_scache* sem = engine.find (pc, found);
  if (UNLIKELY(!found))
    {
      try
	{
	  USI insn = this->GETIMEMSI (pc, pc);
	  sem->decode (this, pc, insn, insn);
	}
      catch (cpu_memory_fault& t)
	{
	  this->memory_trap (t);
	  running_pin.drive (0);
	  return;
	}
    }
	  
  // Execute the instruction  -----------------------------------
  sem_status status = SEM_STATUS_NORMAL; // dummy value
  if (UNLIKELY(trace_disass_p))
    this->disassemble (pc, print_insn_ubicom32,
		       bfd_target_elf_flavour,
		       bfd_arch_ubicom32,
		       BFD_ENDIAN_BIG, "ubicom32");
  else if (UNLIKELY(trace_semantics_p))
    this->begin_trace (pc, sem->idesc->insn_name);
 
  try
    {
      status = sem->idesc->execute (this, sem);
    }
  catch (cpu_memory_fault& t)
    {
      // ignore unless warnings enabled in which case issue an exception and stop
      if (UNLIKELY(this->warnings_enabled))
	this->memory_trap (t);
      else if (UNLIKELY(trace_semantics_p))
	this->trace_stream << ":memory fault accessing " << "0x" << hex << t.address;
      this->yield ();

      // Create a DCAPT entry because of this fault
      set_h_dcapt_pc(t.pc);
      USI tvalue=context;
      if(!strncmp("data write", t.operation, sizeof("data write")))
	{
	  // Matches a data write So dest is out of bounds
	  tvalue |= DCAPT_ILLEGAL_DEST;
	}
      else
	{
	  // assuming it to be a read operation
	  tvalue |= DCAPT_ILLEGAL_SRC;
	}

      set_set_dcapt_interrupt(tvalue);
      this->hardware.h_pc = pc + 4;  // move to next insn regardless
    }
  catch (cpu_exception& t)
    {
      this->yield ();
    }

  // check and see if we have to set the DCAPT interrupt 
  if(get_set_dcapt_interrupt())
    {
      
      set_h_dcapt_thread(get_set_dcapt_interrupt());
      set_h_intset_1 (IRQ_1_DCAPT_MASK);
      
      // clear set_dcapt_interrupt request
      set_set_dcapt_interrupt(0);
    }

  // check for pre/post increment which is deferred until now
  if (h_ar_inc_flag_get())
    {
      h_ar_inc_flag_set(0);
      for (int i = 0; i < 8; ++i)
	{
	  USI inc = h_ar_inc_get(i);
	  if (inc)
	    {
	      set_h_ar (i, get_h_ar(i) + inc);
	      h_ar_inc_set (i, 0); 
	    }
	}
    }
 
  // increment context counter
  inc_h_count ();
  
  // need to do this in case any routine is modifying instruction memory.
  flush_icache ();

  if (UNLIKELY(trace_result_p))
    this->end_trace ();

  // if single stepping set up for this context, halt this context
  int mask = (1 << context);
  if (get_h_mt_single_step () & mask)
    set_h_mt_dbg_active_clr (mask);
  
  // Do post-instruction processing  ----------------------------
  if (UNLIKELY(this->enable_step_trap_p))
    this->signal_trap (sidutil::cpu_trap_stepped);
  this->triggerpoint_manager.check_and_dispatch ();
  running_pin.drive (0);
}

// Scheduler

int
ubicom32bf_cpu::scheduler ()
{
  no_active = true;

  // start by preserving current context location
  set_h_pc (this->hardware.h_pc);

#if 0
  // increment internal timers
  inc_h_glbl_timer ();
#endif

  // increment delay statistics
  inc_delay_table ();

  // multithreading enabled, look for next active thread

  // if there any HRT threads look at HRT table
  //if(get_h_mt_schedule())
  if(1)
    {
      char hrt_entry = GETMEMQI (get_h_pc(), (currentHrtBase + currentHrtSlot));

      if(is_last(hrt_entry))
	{
	  // last entry. Reset curretnHrtSlot to 0
	  currentHrtSlot = 0;

	  // Switch the base address of the table if needed
	  if (cpu_type >= 3)
	    {
	      currentHrtBase = (get_h_glbl_ctl_hrt_table_select_bit() ? 
				(USI)(0xb8000800): 
				(USI)(0xb8000900));
	    }
	  else
	    {
	      currentHrtBase= (get_h_glbl_ctl_hrt_table_select_bit() ? 
			       (USI)(ubicom32bf_cpu_cgen::HRT_TABLE_ADDR_2): 
			       (USI)(ubicom32bf_cpu_cgen::HRT_TABLE_ADDR_1));
	    }
	}
      else if(++currentHrtSlot == 64)
	{
	  // end of the table and no last entry. Just set the currentHrtSlot to 0
	  currentHrtSlot = 0;
	}
      if (!is_empty (hrt_entry))
	{
	  int new_context = get_entry_context (hrt_entry);
	  if (is_active (new_context) && is_past_delay (new_context))
	    {
	      sidlibContext = context = new_context;
	      this->hardware.h_pc = get_h_pc ();
	      //return true;
	      no_active = false;
	    }
	}
    }
  
  if(no_active)
    {
      // if we get here there isn't an active HRT thread
      // look for next high-priority SRT thread that is active
      if (have_srt_hp_threads())
	{
	  int index = srt_index;
	  do 
	    {
	      int new_context = index--;
	      if (index < 0)
		index = NUM_CONTEXTS-1;
	      if (is_active_hp_srt (new_context) && is_past_delay (new_context))
		{
		  sidlibContext = context = new_context;
		  srt_index = index;
		  this->hardware.h_pc = get_h_pc ();
		  //return true;
		  no_active = false;
		}
	    } while (index != srt_index);
	}
    }
  
  if(no_active)
    {
      // at this point there are no HRT or SRT high-priority threads that are active
      // look for next low-priority SRT thread
      int index = srt_index;
      do
	{
 	  int new_context = index--;
 	  if (index < 0)
	    index = NUM_CONTEXTS-1;
	  if (is_active_srt (new_context) && is_past_delay (new_context))
	    {
	      sidlibContext = context = new_context;
	      srt_index = index;
	      this->hardware.h_pc = get_h_pc ();
	      //return true;
	      no_active = false;
	    }
	} while (index != srt_index);
    }

  // at this point there are no threads that are active so we issue a message the first
  // time we know this and return false
  if (no_active)
    {
      if (this->warnings_enabled)
	cerr << "XXX: no active threads available" << endl;
      sidlibContext = -1;
      return false;
    }
  else
    {
      // reset delay table entry for chosen thread
      this->delay_table[context] = 0;
      return true;
    }
}

// Overrides the default version of this virtual function in order to reschedule
// for execution every second time unit. This allows for controlled alternation
// between the core and coprocessor when they are simulated together.
void
ubicom32bf_cpu::stepped (sid::host_int_4 n)
{
#if 0
  if (n < 2)
    n = 2; // reschedule for 2 time units from now
#endif
  // Call parent's version of this function
  cgen_bi_endian_cpu::stepped (n);
}


// Debugger interface functions: return bitwise register images in
// "target byte order".

string
ubicom32bf_cpu::dbg_get_reg (host_int_4 reg)
{
  string attr;
  host_int_4 val4;
  USI index = (u32_regs[cpu_type][reg].addr / 4);

  if(index < ubicom32bf_cpu_cgen::ID_REVISION_INDEX)
    val4 = register_file[context][index];
  else
    val4 = common_register_file[(index - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)];

  if (u32_regs[cpu_type][reg].addr == 0xd0)
    val4 = this->hardware.h_pc;

  // Change to "target endian".
    big_int_4 v = val4;
  for (unsigned i=0; i<4; i++)
    attr += v.read_byte (i);

  return attr;
}


component::status 
ubicom32bf_cpu::dbg_set_reg (host_int_4 reg, const string& attr)
{
  host_int_4 val4;

  //printf("Cpu type = %d\n", cpu_type);

  gdbAccess = 1;

  // change from "target endian"
  big_int_4 v;
  for (unsigned i=0; i<4; i++)
    v.write_byte (i, attr[i]);
  val4 = v;

  USI index = (u32_regs[cpu_type][reg].addr / 4);

  if(index < ubicom32bf_cpu_cgen::ID_REVISION_INDEX)
    register_file[context][index] = val4;
  else
    common_register_file[(index - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)] = val4;

  switch(u32_regs[cpu_type][reg].addr)
    {
    case 0xb4:	set_h_csr (val4);			break;
    case 0xd0:	this->hardware.h_pc = val4;		break;
    case 0x114:	set_h_intset_0 (val4);			break;
    case 0x118:	set_h_intset_1 (val4);			break;
    case 0x124:	set_h_intclr_0 (val4);			break;
    case 0x128:	set_h_intclr_1 (val4);			break;
    case 0x134:	set_h_glbl_ctl (val4);			break;
    case 0x13c:	set_h_mt_active_set (val4);		break;
    case 0x140:	set_h_mt_active_clr (val4);		break;
    case 0x148:	set_h_mt_dbg_active_set (val4);		break;
    case 0x15c:	set_h_mt_break_clr (val4);		break;
    case 0x17c:	set_h_mt_dbg_active_clr(val4);		break;
    }
  gdbAccess = 0;

  return component::ok;
}

void
ubicom32bf_cpu::stream_state (ostream& o) const
{
  // call base class first
  cgen_bi_endian_cpu::stream_state (o);

  o << " ubicom32-cpu";
  o << " " <<  context;
  o << " " <<  direct_access;
  for (int j = 0; j < NUM_CONTEXTS; ++j)
    for (int k = 0; k < 64; ++k)
        o << " " <<  register_file[j][k];
  for (int j = 0; j < 192; ++j)
    o << " " <<  common_register_file[j];
  for (int j = 0; j < NUM_CONTEXTS; ++j)
    o << " " <<  register_file[j][ubicom32bf_cpu_cgen::PC_INDEX];
  //o << " " <<  pc[j];
#if 0
  for (int j = 0; j < NUM_CONTEXTS; ++j)
    o << " " <<  timer_alarm[j];
#endif
  for (int j = 0; j < NUM_CONTEXTS; ++j)
    o << " " <<  s1_context[j]; 
  for (int j = 0; j < NUM_CONTEXTS; ++j)
    o << " " <<  d_context[j];
  o << " " <<  (common_register_file[(ubicom32bf_cpu_cgen::IRQ_STATUS_0_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  o << " " <<  (common_register_file[(ubicom32bf_cpu_cgen::IRQ_STATUS_1_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);;

#if 0
  o << " " <<  h_glbl_timer;
  o << " " <<  h_tctrl_w;
#endif

  o << " " <<  (common_register_file[(ubicom32bf_cpu_cgen::GLBL_CTL_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  o << " " <<  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_ACTIVE_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  o << " " <<  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_DBG_ACTIVE_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  o << " " <<  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_ENABLE_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  o << " " <<  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_PRIORITY_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  o << " " <<  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_SCHEDULE_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  o << " " <<  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_BREAK_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  o << " " <<  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_SINGLESTEP_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  o << " " <<  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_MIN_DELAY_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  o << " " <<  (this->common_register_file[(ubicom32bf_cpu_cgen::DC_CAPTURE_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  o << " " <<  (this->common_register_file[(ubicom32bf_cpu_cgen::DC_CAPTURE_PC_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  o << " " <<  (this->common_register_file[(ubicom32bf_cpu_cgen::DC_CAPTURE_THREAD_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);

  const SI *spad_ptr = &(this->common_register_file[(ubicom32bf_cpu_cgen::SCRATCHPAD0_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  for(int j = 0; j < 4 ; ++j)
    o << " " << spad_ptr[j];;

#if 0
  o << " " <<  h_pipe_busy;
  o << " " <<  h_watchdog;
  o << " " <<  h_clock_ctl;
  o << " " <<  h_random_seed;
#endif

  o << " " <<  (this->common_register_file[(ubicom32bf_cpu_cgen::PERR_ADDR_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  for (int j = 0; j < NUM_CONTEXTS; ++j)
    o << " " <<  this->register_file[j][ubicom32bf_cpu_cgen::CONTEXT_COUNT_INDEX];
#if 0
  o << " " <<  hrt_index;
  o << " " <<  hrt_count;
  for (int j = 0; j < HRT_MAX_ENTRIES; ++j)
    o << " " <<  hrt_table[j];
#endif
  o << " " <<  srt_index;
  for (int j = 0; j < NUM_CONTEXTS; ++j)
    o << " " <<  context_table[j];
  o << " " <<  srt_have_hp_threads;
  o << " " <<  no_active;
  o << " " <<  syscall_trap_num;

  // XXX: Cheat by dumping the entire hardware struct in binary form.  That
  // means that this state image will not be restorable on a different
  // endianness host platform.
  string hw = string(reinterpret_cast<const char*>(& this->hardware), sizeof(this->hardware)); 
  o << " " << string2stream(hw);
}

void
ubicom32bf_cpu::destream_state (istream& i)
{
  // call base class first 
  cgen_bi_endian_cpu::destream_state (i);

  string key;

  i >> key;

  if (key != "ubicom32-cpu")
    {
      i.setstate (ios::badbit);
      return;
    }

  i >>  context;
  i >>  direct_access;
  for (int j = 0; j < NUM_CONTEXTS; ++j)
    for (int k = 0; k < 64; ++k)
        i >>  register_file[j][k];
  for (int j = 0; j < 192; ++j)
    i >>  common_register_file[j];
  for (int j = 0; j < NUM_CONTEXTS; ++j)
    i >>  register_file[j][ubicom32bf_cpu_cgen::PC_INDEX];
  //i >>  pc[j];
#if 0
  for (int j = 0; j < NUM_CONTEXTS; ++j)
    i >>  timer_alarm[j];
#endif
  for (int j = 0; j < NUM_CONTEXTS; ++j)
    i >>  s1_context[j]; 
  for (int j = 0; j < NUM_CONTEXTS; ++j)
    i >>  d_context[j];
  i >>  (common_register_file[(ubicom32bf_cpu_cgen::IRQ_STATUS_0_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  i >>  (common_register_file[(ubicom32bf_cpu_cgen::IRQ_STATUS_1_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);

#if 0
  i >>  h_glbl_timer;
  i >>  h_tctrl_w;
#endif

  i >>  (this->common_register_file[(ubicom32bf_cpu_cgen::GLBL_CTL_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  i >>  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_ACTIVE_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  i >>  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_DBG_ACTIVE_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  i >>  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_ENABLE_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  i >>  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_PRIORITY_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  i >>  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_SCHEDULE_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  i >>  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_BREAK_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  i >>  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_SINGLESTEP_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  i >>  (this->common_register_file[(ubicom32bf_cpu_cgen::MT_MIN_DELAY_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  i >>  (this->common_register_file[(ubicom32bf_cpu_cgen::DC_CAPTURE_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  i >>  (this->common_register_file[(ubicom32bf_cpu_cgen::DC_CAPTURE_PC_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  i >>  (this->common_register_file[(ubicom32bf_cpu_cgen::DC_CAPTURE_THREAD_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);

  SI *spad_ptr = &(this->common_register_file[(ubicom32bf_cpu_cgen::SCRATCHPAD0_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);
  for(int j =0 ; j < 4; j++)
    i >> spad_ptr[j];

  i >>  (this->common_register_file[(ubicom32bf_cpu_cgen::PERR_ADDR_INDEX - ubicom32bf_cpu_cgen::ID_REVISION_INDEX)]);

  for (int j = 0; j < NUM_CONTEXTS; ++j)
    i >>  this->register_file[j][ubicom32bf_cpu_cgen::CONTEXT_COUNT_INDEX];
#if 0
  i >>  hrt_index;
  i >>  hrt_count;
  for (int j = 0; j < HRT_MAX_ENTRIES; ++j)
    i >>  hrt_table[j];
#endif
  i >>  srt_index;
  for (int j = 0; j < NUM_CONTEXTS; ++j)
    i >>  context_table[j];
  i >>  srt_have_hp_threads;
  i >>  no_active;
  i >>  syscall_trap_num;

  // restore the hardware struct all at once
  string hw;
  i >> stream2string(hw);
  if (hw.size() != sizeof(this->hardware))
    i.setstate (ios::badbit);
  else
    {
      const char* hwbin = hw.data();
      ::memcpy (reinterpret_cast<char*>(& this->hardware), hwbin, sizeof(this->hardware));
    }

  // reinitialize derived state
  flush_icache ();
}

DI
ubicom32bf_cpu::dsp_clip32(DI input)
{
  HI acc_hi = (input >>32)&0xffff;
  USI acc_lo = (input & 0xffffffff);
  DI clip_result = input;

  SI positive = 0;

  DI mask = 0x80000000;

  mask = mask << 32;

  if(( input & mask) == 0)
    {
      /* we are dealing with a +ve number. Check if it is saturated */
      DI max_posve_32bit_number = 0x7fffffff;
      if(input > max_posve_32bit_number)
	{
	  /* We are saturated. Set clip_result to 0x7fffffffff and set the dsp_obit. */
	  clip_result = max_posve_32bit_number;

	  /* set the dsp_obit */
	  set_h_dsp_obit(1);

	} 

      return clip_result;
    }

  /* we are dealing with a negative number. See if we are saturated */
  DI min_negv_32bit_number = 0xffffffff;

  min_negv_32bit_number = (min_negv_32bit_number << 32) | (0x80000000);

  if(input < min_negv_32bit_number)
    {
      /* We are negatively saturated. Set clip_result to 0x80000000 and set the dsp_obit */
      clip_result = min_negv_32bit_number;

      /* set the dsp_obit */
      set_h_dsp_obit(1);
    } 

  return clip_result;
}

DI
ubicom32bf_cpu::dsp_clip16(DI input)
{
   DI clip_result = input;

  SI positive = 0;

  DI mask = 0x80000000;
  mask = mask << 32;

  if((input & mask) == 0)
    {
      /* we are dealing with a +ve number. Check if it is saturated */
      DI max_posve_16bit_number = 0x7fff;
      if(input > max_posve_16bit_number)
	{
	  /* We are saturated. Set clip_result to 0x7fffff and set the dsp_obit. */
	  clip_result = max_posve_16bit_number;

	  /* set the dsp_obit */
	  set_h_dsp_obit(1);

	} 

      return clip_result;
    }

  /* we are dealing with a negative number. See if we are saturated */
  DI min_negv_16bit_number =0xffffffff;
  min_negv_16bit_number = (min_negv_16bit_number << 32) | (0xffff8000);

  if(input < min_negv_16bit_number)
    {
      /* We are negatively saturated. Set clip_result to 0xffffffffffff8000 and set the dsp_obit */
      clip_result = min_negv_16bit_number;

      /* set the dsp_obit */
      set_h_dsp_obit(1);
    } 

  return clip_result;
}

void
ubicom32bf_cpu::dsp_addsub(DI s1, DI s2, USI dest_acc, DI addsub, USI mode, USI clip)
{
  DI result, clip_result;

  result = (addsub == 1) ? s1+s2: s1-s2;

  /* We need to ignore mode */
  if(mode == 32) 
    clip_result = dsp_clip32(result);
  else
    clip_result = dsp_clip16(result);

  if(clip)
    result = clip_result;

  SI acc_hi = (result >>32) & 0xffff;
  SI acc_lo = (result & 0xffffffff);

  if(dest_acc)
    {
      /* data goes to acc1 */
      set_h_acc1_hi(acc_hi);
      set_h_acc1_lo(acc_lo);
    }
  else
    {
      set_h_mac_hi(acc_hi);
      set_h_mac_lo(acc_lo);
      finish_multiply();
    }
}

DI
ubicom32bf_cpu::my_get_acc (UINT index)
{
  DI tmp1, tmp2;
  if(index == 0) {
    tmp1 = get_h_mac_hi();
    tmp2 = get_h_mac_lo();
  } else {
    tmp1 = get_h_acc1_hi();
    tmp2 = get_h_acc1_lo();
  }

  tmp1 &= 0xffff;
  if(tmp1 & 0x8000) {
    /* we have to sign extend this */
    tmp1 |= 0xffff0000;
  }

  DI result = (((DI) tmp1)<<32 | ((DI)tmp2));
  return result;
}

void
ubicom32bf_cpu::my_set_h_dsp_obit (BI newval)
{
  USI *ptr = (USI *)&register_file[context][UCSR_ADDR/4];
  if (newval)
    *ptr |= CSR_DSP_OBIT_MASK;
  else
    *ptr &= ~CSR_DSP_OBIT_MASK;
}

