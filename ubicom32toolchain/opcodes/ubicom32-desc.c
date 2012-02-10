/* CPU data for ubicom32.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright 1996-2007 Free Software Foundation, Inc.

This file is part of the GNU Binutils and/or GDB, the GNU debugger.

   This file is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.

*/

#include "sysdep.h"
#include <stdio.h>
#include <stdarg.h>
#include "ansidecl.h"
#include "bfd.h"
#include "symcat.h"
#include "ubicom32-desc.h"
#include "ubicom32-opc.h"
#include "opintl.h"
#include "libiberty.h"
#include "xregex.h"

/* Attributes.  */

static const CGEN_ATTR_ENTRY bool_attr[] =
{
  { "#f", 0 },
  { "#t", 1 },
  { 0, 0 }
};

static const CGEN_ATTR_ENTRY MACH_attr[] ATTRIBUTE_UNUSED =
{
  { "base", MACH_BASE },
  { "ip3035", MACH_IP3035 },
  { "ubicom32dsp", MACH_UBICOM32DSP },
  { "ip3023compatibility", MACH_IP3023COMPATIBILITY },
  { "ubicom32_ver4", MACH_UBICOM32_VER4 },
  { "ubicom32_ver5", MACH_UBICOM32_VER5 },
  { "ubicom32_oldmoveai", MACH_UBICOM32_OLDMOVEAI },
  { "ubicom32_newmoveai", MACH_UBICOM32_NEWMOVEAI },
  { "ubicom32_newmoveaisimulate", MACH_UBICOM32_NEWMOVEAISIMULATE },
  { "max", MACH_MAX },
  { 0, 0 }
};

static const CGEN_ATTR_ENTRY ISA_attr[] ATTRIBUTE_UNUSED =
{
  { "ubicom32", ISA_UBICOM32 },
  { "max", ISA_MAX },
  { 0, 0 }
};

const CGEN_ATTR_TABLE ubicom32_cgen_ifield_attr_table[] =
{
  { "MACH", & MACH_attr[0], & MACH_attr[0] },
  { "VIRTUAL", &bool_attr[0], &bool_attr[0] },
  { "PCREL-ADDR", &bool_attr[0], &bool_attr[0] },
  { "ABS-ADDR", &bool_attr[0], &bool_attr[0] },
  { "RESERVED", &bool_attr[0], &bool_attr[0] },
  { "SIGN-OPT", &bool_attr[0], &bool_attr[0] },
  { "SIGNED", &bool_attr[0], &bool_attr[0] },
  { 0, 0, 0 }
};

const CGEN_ATTR_TABLE ubicom32_cgen_hardware_attr_table[] =
{
  { "MACH", & MACH_attr[0], & MACH_attr[0] },
  { "VIRTUAL", &bool_attr[0], &bool_attr[0] },
  { "CACHE-ADDR", &bool_attr[0], &bool_attr[0] },
  { "PC", &bool_attr[0], &bool_attr[0] },
  { "PROFILE", &bool_attr[0], &bool_attr[0] },
  { 0, 0, 0 }
};

const CGEN_ATTR_TABLE ubicom32_cgen_operand_attr_table[] =
{
  { "MACH", & MACH_attr[0], & MACH_attr[0] },
  { "VIRTUAL", &bool_attr[0], &bool_attr[0] },
  { "PCREL-ADDR", &bool_attr[0], &bool_attr[0] },
  { "ABS-ADDR", &bool_attr[0], &bool_attr[0] },
  { "SIGN-OPT", &bool_attr[0], &bool_attr[0] },
  { "SIGNED", &bool_attr[0], &bool_attr[0] },
  { "NEGATIVE", &bool_attr[0], &bool_attr[0] },
  { "RELAX", &bool_attr[0], &bool_attr[0] },
  { "SEM-ONLY", &bool_attr[0], &bool_attr[0] },
  { 0, 0, 0 }
};

const CGEN_ATTR_TABLE ubicom32_cgen_insn_attr_table[] =
{
  { "MACH", & MACH_attr[0], & MACH_attr[0] },
  { "ALIAS", &bool_attr[0], &bool_attr[0] },
  { "VIRTUAL", &bool_attr[0], &bool_attr[0] },
  { "UNCOND-CTI", &bool_attr[0], &bool_attr[0] },
  { "COND-CTI", &bool_attr[0], &bool_attr[0] },
  { "SKIP-CTI", &bool_attr[0], &bool_attr[0] },
  { "DELAY-SLOT", &bool_attr[0], &bool_attr[0] },
  { "RELAXABLE", &bool_attr[0], &bool_attr[0] },
  { "RELAXED", &bool_attr[0], &bool_attr[0] },
  { "NO-DIS", &bool_attr[0], &bool_attr[0] },
  { "PBB", &bool_attr[0], &bool_attr[0] },
  { 0, 0, 0 }
};

/* Instruction set variants.  */

static const CGEN_ISA ubicom32_cgen_isa_table[] = {
  { "ubicom32", 32, 32, 32, 32 },
  { 0, 0, 0, 0, 0 }
};

/* Machine variants.  */

static const CGEN_MACH ubicom32_cgen_mach_table[] = {
  { "ip3035", "ubicom32", MACH_IP3035, 0 },
  { "ubicom32dsp", "ubicom32dsp", MACH_UBICOM32DSP, 0 },
  { "ip3023compatibility", "ubicom32dsp", MACH_IP3023COMPATIBILITY, 0 },
  { "ubicom32_ver4", "ubicom32ver4", MACH_UBICOM32_VER4, 0 },
  { "ubicom32_ver5", "ubicom32ver5", MACH_UBICOM32_VER5, 0 },
  { "ubicom32_oldmoveai", "ubicom32ver4", MACH_UBICOM32_OLDMOVEAI, 0 },
  { "ubicom32_newmoveai", "ubicom32ver5", MACH_UBICOM32_NEWMOVEAI, 0 },
  { 0, 0, 0, 0 }
};

static CGEN_KEYWORD_ENTRY ubicom32_cgen_opval_data_names_entries[] =
{
  { "d0", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "d1", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "d2", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "d3", 3, {0, {{{0, 0}}}}, 0, 0 },
  { "d4", 4, {0, {{{0, 0}}}}, 0, 0 },
  { "d5", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "d6", 6, {0, {{{0, 0}}}}, 0, 0 },
  { "d7", 7, {0, {{{0, 0}}}}, 0, 0 },
  { "d8", 8, {0, {{{0, 0}}}}, 0, 0 },
  { "d9", 9, {0, {{{0, 0}}}}, 0, 0 },
  { "d10", 10, {0, {{{0, 0}}}}, 0, 0 },
  { "d11", 11, {0, {{{0, 0}}}}, 0, 0 },
  { "d12", 12, {0, {{{0, 0}}}}, 0, 0 },
  { "d13", 13, {0, {{{0, 0}}}}, 0, 0 },
  { "d14", 14, {0, {{{0, 0}}}}, 0, 0 },
  { "d15", 15, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD ubicom32_cgen_opval_data_names =
{
  & ubicom32_cgen_opval_data_names_entries[0],
  16,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY ubicom32_cgen_opval_addr_names_entries[] =
{
  { "sp", 7, {0, {{{0, 0}}}}, 0, 0 },
  { "a0", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "a1", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "a2", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "a3", 3, {0, {{{0, 0}}}}, 0, 0 },
  { "a4", 4, {0, {{{0, 0}}}}, 0, 0 },
  { "a5", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "a6", 6, {0, {{{0, 0}}}}, 0, 0 },
  { "a7", 7, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD ubicom32_cgen_opval_addr_names =
{
  & ubicom32_cgen_opval_addr_names_entries[0],
  9,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY ubicom32_cgen_opval_acc_names_entries[] =
{
  { "acc0", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "acc1", 1, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD ubicom32_cgen_opval_acc_names =
{
  & ubicom32_cgen_opval_acc_names_entries[0],
  2,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY ubicom32_cgen_opval_acc_lo_names_entries[] =
{
  { "acc0_lo", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "acc0_hi", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "acc1_lo", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "acc1_hi", 3, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD ubicom32_cgen_opval_acc_lo_names =
{
  & ubicom32_cgen_opval_acc_lo_names_entries[0],
  4,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY ubicom32_cgen_opval_data_names_even_entries[] =
{
  { "d0", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "d2", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "d4", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "d6", 3, {0, {{{0, 0}}}}, 0, 0 },
  { "d8", 4, {0, {{{0, 0}}}}, 0, 0 },
  { "d10", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "d12", 6, {0, {{{0, 0}}}}, 0, 0 },
  { "d14", 7, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD ubicom32_cgen_opval_data_names_even =
{
  & ubicom32_cgen_opval_data_names_even_entries[0],
  8,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY ubicom32_cgen_opval_spad_names_entries[] =
{
  { "scratchpad0", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "scratchpad1", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "scratchpad2", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "scratchpad3", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "scratchpad4", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "scratchpad5", 0, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD ubicom32_cgen_opval_spad_names =
{
  & ubicom32_cgen_opval_spad_names_entries[0],
  6,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY ubicom32_cgen_opval_h_cc_entries[] =
{
  { "f", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "lo", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "cc", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "hs", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "cs", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "eq", 3, {0, {{{0, 0}}}}, 0, 0 },
  { "ge", 4, {0, {{{0, 0}}}}, 0, 0 },
  { "gt", 5, {0, {{{0, 0}}}}, 0, 0 },
  { "hi", 6, {0, {{{0, 0}}}}, 0, 0 },
  { "le", 7, {0, {{{0, 0}}}}, 0, 0 },
  { "ls", 8, {0, {{{0, 0}}}}, 0, 0 },
  { "lt", 9, {0, {{{0, 0}}}}, 0, 0 },
  { "mi", 10, {0, {{{0, 0}}}}, 0, 0 },
  { "ne", 11, {0, {{{0, 0}}}}, 0, 0 },
  { "pl", 12, {0, {{{0, 0}}}}, 0, 0 },
  { "t", 13, {0, {{{0, 0}}}}, 0, 0 },
  { "vc", 14, {0, {{{0, 0}}}}, 0, 0 },
  { "vs", 15, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD ubicom32_cgen_opval_h_cc =
{
  & ubicom32_cgen_opval_h_cc_entries[0],
  18,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY ubicom32_cgen_opval_h_C_entries[] =
{
  { "", 1, {0, {{{0, 0}}}}, 0, 0 },
  { ".s", 0, {0, {{{0, 0}}}}, 0, 0 },
  { ".w", 1, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD ubicom32_cgen_opval_h_C =
{
  & ubicom32_cgen_opval_h_C_entries[0],
  3,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY ubicom32_cgen_opval_h_P_entries[] =
{
  { ".t", 1, {0, {{{0, 0}}}}, 0, 0 },
  { ".f", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "", 1, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD ubicom32_cgen_opval_h_P =
{
  & ubicom32_cgen_opval_h_P_entries[0],
  3,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY ubicom32_cgen_opval_h_DSP_C_entries[] =
{
  { ".c", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "", 0, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD ubicom32_cgen_opval_h_DSP_C =
{
  & ubicom32_cgen_opval_h_DSP_C_entries[0],
  2,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY ubicom32_cgen_opval_h_FPU_acc_lo_32_entries[] =
{
  { "acc0_lo", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "acc0_hi", 1, {0, {{{0, 0}}}}, 0, 0 },
  { "acc1_lo", 2, {0, {{{0, 0}}}}, 0, 0 },
  { "acc1_hi", 3, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD ubicom32_cgen_opval_h_FPU_acc_lo_32 =
{
  & ubicom32_cgen_opval_h_FPU_acc_lo_32_entries[0],
  4,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY ubicom32_cgen_opval_h_DSP_T_entries[] =
{
  { "", 0, {0, {{{0, 0}}}}, 0, 0 },
  { ".t", 1, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD ubicom32_cgen_opval_h_DSP_T =
{
  & ubicom32_cgen_opval_h_DSP_T_entries[0],
  2,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY ubicom32_cgen_opval_h_DSP_T_addsub_entries[] =
{
  { "", 0, {0, {{{0, 0}}}}, 0, 0 },
  { ".t", 1, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD ubicom32_cgen_opval_h_DSP_T_addsub =
{
  & ubicom32_cgen_opval_h_DSP_T_addsub_entries[0],
  2,
  0, 0, 0, 0, ""
};

static CGEN_KEYWORD_ENTRY ubicom32_cgen_opval_h_DSP_Dest_A_entries[] =
{
  { "acc0", 0, {0, {{{0, 0}}}}, 0, 0 },
  { "acc1", 1, {0, {{{0, 0}}}}, 0, 0 }
};

CGEN_KEYWORD ubicom32_cgen_opval_h_DSP_Dest_A =
{
  & ubicom32_cgen_opval_h_DSP_Dest_A_entries[0],
  2,
  0, 0, 0, 0, ""
};


/* The hardware table.  */

#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define A(a) (1 << CGEN_HW_##a)
#else
#define A(a) (1 << CGEN_HW_/**/a)
#endif

const CGEN_HW_ENTRY ubicom32_cgen_hw_table[] =
{
  { "h-memory", HW_H_MEMORY, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-sint", HW_H_SINT, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-uint", HW_H_UINT, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-addr", HW_H_ADDR, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-iaddr", HW_H_IADDR, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-global-control", HW_H_GLOBAL_CONTROL, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-mt-break", HW_H_MT_BREAK, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-mt-active", HW_H_MT_ACTIVE, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-mt-enable", HW_H_MT_ENABLE, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-mt-priority", HW_H_MT_PRIORITY, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-mt-schedule", HW_H_MT_SCHEDULE, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-irq-status-0", HW_H_IRQ_STATUS_0, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-irq-status-1", HW_H_IRQ_STATUS_1, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-dr", HW_H_DR, CGEN_ASM_KEYWORD, (PTR) & ubicom32_cgen_opval_data_names, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-s1-dr", HW_H_S1_DR, CGEN_ASM_KEYWORD, (PTR) & ubicom32_cgen_opval_data_names, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-ar", HW_H_AR, CGEN_ASM_KEYWORD, (PTR) & ubicom32_cgen_opval_addr_names, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-ar-inc", HW_H_AR_INC, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-ar-inc-flag", HW_H_AR_INC_FLAG, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-mac-hi", HW_H_MAC_HI, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-mac-lo", HW_H_MAC_LO, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-src-3", HW_H_SRC_3, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-csr", HW_H_CSR, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-iread", HW_H_IREAD, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-acc1-hi", HW_H_ACC1_HI, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } } },
  { "h-acc1-lo", HW_H_ACC1_LO, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } } },
  { "h-pc", HW_H_PC, CGEN_ASM_NONE, 0, { 0|A(PROFILE)|A(PC), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-nbit-16", HW_H_NBIT_16, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-zbit-16", HW_H_ZBIT_16, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-vbit-16", HW_H_VBIT_16, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-cbit-16", HW_H_CBIT_16, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-nbit-32", HW_H_NBIT_32, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-zbit-32", HW_H_ZBIT_32, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-vbit-32", HW_H_VBIT_32, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-cbit-32", HW_H_CBIT_32, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-prev-privbit", HW_H_PREV_PRIVBIT, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-privbit", HW_H_PRIVBIT, CGEN_ASM_NONE, 0, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { "h-cc", HW_H_CC, CGEN_ASM_KEYWORD, (PTR) & ubicom32_cgen_opval_h_cc, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-C", HW_H_C, CGEN_ASM_KEYWORD, (PTR) & ubicom32_cgen_opval_h_C, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-P", HW_H_P, CGEN_ASM_KEYWORD, (PTR) & ubicom32_cgen_opval_h_P, { 0, { { { (1<<MACH_BASE), 0 } } } } },
  { "h-DSP-C", HW_H_DSP_C, CGEN_ASM_KEYWORD, (PTR) & ubicom32_cgen_opval_h_DSP_C, { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } } },
  { "h-FPU-acc-lo-32", HW_H_FPU_ACC_LO_32, CGEN_ASM_KEYWORD, (PTR) & ubicom32_cgen_opval_h_FPU_acc_lo_32, { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } } },
  { "h-DSP-T", HW_H_DSP_T, CGEN_ASM_KEYWORD, (PTR) & ubicom32_cgen_opval_h_DSP_T, { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } } },
  { "h-DSP-T-addsub", HW_H_DSP_T_ADDSUB, CGEN_ASM_KEYWORD, (PTR) & ubicom32_cgen_opval_h_DSP_T_addsub, { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } } },
  { "h-DSP-Dest-A", HW_H_DSP_DEST_A, CGEN_ASM_KEYWORD, (PTR) & ubicom32_cgen_opval_h_DSP_Dest_A, { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } } },
  { "h-DSP-S2-Acc-reg-mul", HW_H_DSP_S2_ACC_REG_MUL, CGEN_ASM_KEYWORD, (PTR) & ubicom32_cgen_opval_acc_names, { 0|A(VIRTUAL), { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } } },
  { "h-DSP-S2-Acc-reg-addsub", HW_H_DSP_S2_ACC_REG_ADDSUB, CGEN_ASM_KEYWORD, (PTR) & ubicom32_cgen_opval_acc_names, { 0|A(VIRTUAL), { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } } },
  { "h-FPU-acc-64", HW_H_FPU_ACC_64, CGEN_ASM_KEYWORD, (PTR) & ubicom32_cgen_opval_acc_names, { 0|A(VIRTUAL), { { { (1<<MACH_UBICOM32_VER5), 0 } } } } },
  { "h-FPU-acc-32", HW_H_FPU_ACC_32, CGEN_ASM_KEYWORD, (PTR) & ubicom32_cgen_opval_acc_lo_names, { 0|A(VIRTUAL), { { { (1<<MACH_UBICOM32_VER5), 0 } } } } },
  { "h-FPU-data-reg-32", HW_H_FPU_DATA_REG_32, CGEN_ASM_KEYWORD, (PTR) & ubicom32_cgen_opval_data_names, { 0|A(VIRTUAL), { { { (1<<MACH_UBICOM32_VER5), 0 } } } } },
  { "h-FPU-data-reg-64", HW_H_FPU_DATA_REG_64, CGEN_ASM_KEYWORD, (PTR) & ubicom32_cgen_opval_data_names_even, { 0|A(VIRTUAL), { { { (1<<MACH_UBICOM32_VER5), 0 } } } } },
  { "h-sp", HW_H_SP, CGEN_ASM_KEYWORD, (PTR) & ubicom32_cgen_opval_spad_names, { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } } },
  { 0, 0, CGEN_ASM_NONE, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } }
};

#undef A


/* The instruction field table.  */

#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define A(a) (1 << CGEN_IFLD_##a)
#else
#define A(a) (1 << CGEN_IFLD_/**/a)
#endif

const CGEN_IFLD ubicom32_cgen_ifld_table[] =
{
  { UBICOM32_F_NIL, "f-nil", 0, 0, 0, 0, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_ANYOF, "f-anyof", 0, 0, 0, 0, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_D, "f-d", 0, 32, 26, 11, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_D_BIT10, "f-d-bit10", 0, 32, 26, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_D_TYPE, "f-d-type", 0, 32, 25, 2, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_D_R, "f-d-r", 0, 32, 20, 5, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_D_M, "f-d-M", 0, 32, 20, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_D_I4_1, "f-d-i4-1", 0, 32, 19, 4, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_D_I4_2, "f-d-i4-2", 0, 32, 19, 4, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_D_I4_4, "f-d-i4-4", 0, 32, 19, 4, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_D_AN, "f-d-An", 0, 32, 23, 3, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_D_DIRECT, "f-d-direct", 0, 32, 23, 8, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_D_IMM8, "f-d-imm8", 0, 32, 23, 8, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_D_IMM7_T, "f-d-imm7-t", 0, 32, 25, 2, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_D_IMM7_B, "f-d-imm7-b", 0, 32, 20, 5, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_D_IMM7_1, "f-d-imm7-1", 0, 0, 0, 0,{ 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_D_IMM7_2, "f-d-imm7-2", 0, 0, 0, 0,{ 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_D_IMM7_4, "f-d-imm7-4", 0, 0, 0, 0,{ 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_S1, "f-s1", 0, 32, 10, 11, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_S1_BIT10, "f-s1-bit10", 0, 32, 10, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_S1_TYPE, "f-s1-type", 0, 32, 9, 2, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_S1_R, "f-s1-r", 0, 32, 4, 5, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_S1_M, "f-s1-M", 0, 32, 4, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_S1_I4_1, "f-s1-i4-1", 0, 32, 3, 4, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_S1_I4_2, "f-s1-i4-2", 0, 32, 3, 4, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_S1_I4_4, "f-s1-i4-4", 0, 32, 3, 4, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_S1_AN, "f-s1-An", 0, 32, 7, 3, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_S1_DIRECT, "f-s1-direct", 0, 32, 7, 8, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_S1_IMM8, "f-s1-imm8", 0, 32, 7, 8, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_S1_IMM7_T, "f-s1-imm7-t", 0, 32, 9, 2, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_S1_IMM7_B, "f-s1-imm7-b", 0, 32, 4, 5, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_S1_IMM7_1, "f-s1-imm7-1", 0, 0, 0, 0,{ 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_S1_IMM7_2, "f-s1-imm7-2", 0, 0, 0, 0,{ 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_S1_IMM7_4, "f-s1-imm7-4", 0, 0, 0, 0,{ 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_OP1, "f-op1", 0, 32, 31, 5, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_OPMOVEAI, "f-opmoveai", 0, 32, 31, 4, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_OP2, "f-op2", 0, 32, 15, 5, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_BIT26, "f-bit26", 0, 32, 26, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_OPEXT, "f-opext", 0, 32, 25, 5, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_COND, "f-cond", 0, 32, 26, 4, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_IMM16_1, "f-imm16-1", 0, 32, 26, 16, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_IMM16_2, "f-imm16-2", 0, 32, 15, 16, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_O21, "f-o21", 0, 32, 20, 21, { 0|A(PCREL_ADDR), { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_O23_21, "f-o23-21", 0, 32, 26, 3, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_O20_0, "f-o20-0", 0, 32, 20, 21, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_O24, "f-o24", 0, 0, 0, 0,{ 0|A(PCREL_ADDR)|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_IMM23_21, "f-imm23-21", 0, 32, 26, 3, { 0, { { { (1<<MACH_UBICOM32_OLDMOVEAI), 0 } } } }  },
  { UBICOM32_F_IMM24, "f-imm24", 0, 0, 0, 0,{ 0|A(VIRTUAL), { { { (1<<MACH_UBICOM32_OLDMOVEAI), 0 } } } }  },
  { UBICOM32_F_IMM24_21, "f-imm24-21", 0, 32, 27, 4, { 0, { { { (1<<MACH_UBICOM32_NEWMOVEAI), 0 } } } }  },
  { UBICOM32_F_IMM25, "f-imm25", 0, 0, 0, 0,{ 0|A(VIRTUAL), { { { (1<<MACH_UBICOM32_NEWMOVEAI), 0 } } } }  },
  { UBICOM32_F_O15_13, "f-o15-13", 0, 32, 26, 3, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_O12_8, "f-o12-8", 0, 32, 20, 5, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_O7_5, "f-o7-5", 0, 32, 10, 3, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_O4_0, "f-o4-0", 0, 32, 4, 5, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_O16, "f-o16", 0, 0, 0, 0,{ 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_LEAI_O16, "f-leai-o16", 0, 0, 0, 0,{ 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_AN, "f-An", 0, 32, 23, 3, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_AM, "f-Am", 0, 32, 7, 3, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_DN, "f-Dn", 0, 32, 20, 5, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_BIT5, "f-bit5", 0, 32, 15, 5, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_P, "f-P", 0, 32, 22, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_C, "f-C", 0, 32, 21, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_INT, "f-int", 0, 32, 5, 6, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_DSP_C, "f-dsp-C", 0, 32, 20, 1, { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }  },
  { UBICOM32_F_DSP_T, "f-dsp-T", 0, 32, 19, 1, { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }  },
  { UBICOM32_F_DSP_S2_SEL, "f-dsp-S2-sel", 0, 32, 18, 1, { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }  },
  { UBICOM32_F_DSP_R, "f-dsp-R", 0, 32, 17, 1, { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }  },
  { UBICOM32_F_DSP_DESTA, "f-dsp-destA", 0, 32, 16, 1, { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }  },
  { UBICOM32_F_DSP_B15, "f-dsp-b15", 0, 32, 15, 1, { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }  },
  { UBICOM32_F_DSP_S2, "f-dsp-S2", 0, 32, 14, 4, { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }  },
  { UBICOM32_F_DSP_J, "f-dsp-J", 0, 32, 26, 1, { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }  },
  { UBICOM32_F_S2, "f-s2", 0, 32, 14, 4, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_B15, "f-b15", 0, 32, 15, 1, { 0, { { { (1<<MACH_BASE), 0 } } } }  },
  { UBICOM32_F_BIT0, "f-bit0", 0, 32, 0, 1, { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
  { UBICOM32_F_BIT16, "f-bit16", 0, 32, 16, 1, { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
  { UBICOM32_F_FPUNUSED20, "f-FPunused20", 0, 32, 20, 3, { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
  { UBICOM32_F_FPUNUSED15, "f-FPunused15", 0, 32, 15, 5, { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
  { UBICOM32_F_FPUNUSED11, "f-FPunused11", 0, 32, 11, 7, { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
  { UBICOM32_F_FPD32, "f-FPD32", 0, 32, 17, 2, { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
  { UBICOM32_F_FPD64, "f-FPD64", 0, 32, 17, 1, { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
  { UBICOM32_F_FPS2_SEL, "f-FPS2-sel", 0, 32, 15, 1, { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
  { UBICOM32_F_FPS2_REG32, "f-FPS2-reg32", 0, 32, 14, 4, { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
  { UBICOM32_F_FPS232, "f-FPS232", 0, 32, 15, 5, { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
  { UBICOM32_F_FPS2_REG64, "f-FPS2-reg64", 0, 32, 14, 3, { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
  { UBICOM32_F_FPS264, "f-FPS264", 0, 32, 15, 4, { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
  { UBICOM32_F_FPS1_SEL, "f-FPS1-sel", 0, 32, 4, 1, { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
  { UBICOM32_F_FPS1_REG64, "f-FPS1-reg64", 0, 32, 3, 3, { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
  { UBICOM32_F_FPS164, "f-FPS164", 0, 32, 4, 4, { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
  { 0, 0, 0, 0, 0, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } }
};

#undef A



/* multi ifield declarations */

const CGEN_MAYBE_MULTI_IFLD UBICOM32_F_D_IMM7_1_MULTI_IFIELD [];
const CGEN_MAYBE_MULTI_IFLD UBICOM32_F_D_IMM7_2_MULTI_IFIELD [];
const CGEN_MAYBE_MULTI_IFLD UBICOM32_F_D_IMM7_4_MULTI_IFIELD [];
const CGEN_MAYBE_MULTI_IFLD UBICOM32_F_S1_IMM7_1_MULTI_IFIELD [];
const CGEN_MAYBE_MULTI_IFLD UBICOM32_F_S1_IMM7_2_MULTI_IFIELD [];
const CGEN_MAYBE_MULTI_IFLD UBICOM32_F_S1_IMM7_4_MULTI_IFIELD [];
const CGEN_MAYBE_MULTI_IFLD UBICOM32_F_O24_MULTI_IFIELD [];
const CGEN_MAYBE_MULTI_IFLD UBICOM32_F_IMM24_MULTI_IFIELD [];
const CGEN_MAYBE_MULTI_IFLD UBICOM32_F_IMM25_MULTI_IFIELD [];
const CGEN_MAYBE_MULTI_IFLD UBICOM32_F_O16_MULTI_IFIELD [];
const CGEN_MAYBE_MULTI_IFLD UBICOM32_F_LEAI_O16_MULTI_IFIELD [];


/* multi ifield definitions */

const CGEN_MAYBE_MULTI_IFLD UBICOM32_F_D_IMM7_1_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_D_IMM7_T] } },
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_D_IMM7_B] } },
    { 0, { (const PTR) 0 } }
};
const CGEN_MAYBE_MULTI_IFLD UBICOM32_F_D_IMM7_2_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_D_IMM7_T] } },
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_D_IMM7_B] } },
    { 0, { (const PTR) 0 } }
};
const CGEN_MAYBE_MULTI_IFLD UBICOM32_F_D_IMM7_4_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_D_IMM7_T] } },
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_D_IMM7_B] } },
    { 0, { (const PTR) 0 } }
};
const CGEN_MAYBE_MULTI_IFLD UBICOM32_F_S1_IMM7_1_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_S1_IMM7_T] } },
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_S1_IMM7_B] } },
    { 0, { (const PTR) 0 } }
};
const CGEN_MAYBE_MULTI_IFLD UBICOM32_F_S1_IMM7_2_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_S1_IMM7_T] } },
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_S1_IMM7_B] } },
    { 0, { (const PTR) 0 } }
};
const CGEN_MAYBE_MULTI_IFLD UBICOM32_F_S1_IMM7_4_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_S1_IMM7_T] } },
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_S1_IMM7_B] } },
    { 0, { (const PTR) 0 } }
};
const CGEN_MAYBE_MULTI_IFLD UBICOM32_F_O24_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_O23_21] } },
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_O20_0] } },
    { 0, { (const PTR) 0 } }
};
const CGEN_MAYBE_MULTI_IFLD UBICOM32_F_IMM24_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_IMM23_21] } },
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_O20_0] } },
    { 0, { (const PTR) 0 } }
};
const CGEN_MAYBE_MULTI_IFLD UBICOM32_F_IMM25_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_IMM24_21] } },
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_O20_0] } },
    { 0, { (const PTR) 0 } }
};
const CGEN_MAYBE_MULTI_IFLD UBICOM32_F_O16_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_O15_13] } },
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_O12_8] } },
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_O7_5] } },
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_O4_0] } },
    { 0, { (const PTR) 0 } }
};
const CGEN_MAYBE_MULTI_IFLD UBICOM32_F_LEAI_O16_MULTI_IFIELD [] =
{
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_O15_13] } },
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_O12_8] } },
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_O7_5] } },
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_O4_0] } },
    { 0, { (const PTR) 0 } }
};

/* The operand table.  */

#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define A(a) (1 << CGEN_OPERAND_##a)
#else
#define A(a) (1 << CGEN_OPERAND_/**/a)
#endif
#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define OPERAND(op) UBICOM32_OPERAND_##op
#else
#define OPERAND(op) UBICOM32_OPERAND_/**/op
#endif

const CGEN_OPERAND ubicom32_cgen_operand_table[] =
{
/* pc: program counter */
  { "pc", UBICOM32_OPERAND_PC, HW_H_PC, 0, 0,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_NIL] } }, 
    { 0|A(SEM_ONLY), { { { (1<<MACH_BASE), 0 } } } }  },
/* s2: s2 register for op3 */
  { "s2", UBICOM32_OPERAND_S2, HW_H_DR, 14, 4,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_S2] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* src3: src-3 register */
  { "src3", UBICOM32_OPERAND_SRC3, HW_H_SRC_3, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* offset24: 24-bit relative word offset */
  { "offset24", UBICOM32_OPERAND_OFFSET24, HW_H_IADDR, 20, 24,
    { 2, { (const PTR) &UBICOM32_F_O24_MULTI_IFIELD[0] } }, 
    { 0|A(PCREL_ADDR)|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
/* An: An register for call */
  { "An", UBICOM32_OPERAND_AN, HW_H_AR, 23, 3,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_AN] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* cc: condition code */
  { "cc", UBICOM32_OPERAND_CC, HW_H_CC, 26, 4,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_COND] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* C: condition code select bits */
  { "C", UBICOM32_OPERAND_C, HW_H_C, 21, 1,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_C] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* P: prediction bit */
  { "P", UBICOM32_OPERAND_P, HW_H_P, 22, 1,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_P] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* Am: Am register for calli */
  { "Am", UBICOM32_OPERAND_AM, HW_H_AR, 7, 3,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_AM] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* Dn: Dn reg for mac/mulu/mulf */
  { "Dn", UBICOM32_OPERAND_DN, HW_H_DR, 20, 5,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_DN] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* interrupt: interrupt code */
  { "interrupt", UBICOM32_OPERAND_INTERRUPT, HW_H_UINT, 5, 6,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_INT] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* imm16-1: 16 bit immediate for cmpi */
  { "imm16-1", UBICOM32_OPERAND_IMM16_1, HW_H_SINT, 26, 16,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_IMM16_1] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* x-op2: ignored secondary opcode */
  { "x-op2", UBICOM32_OPERAND_X_OP2, HW_H_UINT, 15, 5,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_OP2] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* x-bit26: ignored bit 26 */
  { "x-bit26", UBICOM32_OPERAND_X_BIT26, HW_H_UINT, 26, 1,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_BIT26] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* x-s1: ignored s1 operand */
  { "x-s1", UBICOM32_OPERAND_X_S1, HW_H_UINT, 10, 11,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_S1] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* x-d: ignored d operand */
  { "x-d", UBICOM32_OPERAND_X_D, HW_H_UINT, 26, 11,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_D] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* x-dn: ignored dn operand */
  { "x-dn", UBICOM32_OPERAND_X_DN, HW_H_UINT, 20, 5,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_DN] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* machi: mac hi register */
  { "machi", UBICOM32_OPERAND_MACHI, HW_H_MAC_HI, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* maclo: mac lo register */
  { "maclo", UBICOM32_OPERAND_MACLO, HW_H_MAC_LO, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* acc1hi: acc1 hi register */
  { "acc1hi", UBICOM32_OPERAND_ACC1HI, HW_H_ACC1_HI, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }  },
/* acc1lo: acc1 lo register */
  { "acc1lo", UBICOM32_OPERAND_ACC1LO, HW_H_ACC1_LO, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }  },
/* irq-0: irq status register 0 */
  { "irq-0", UBICOM32_OPERAND_IRQ_0, HW_H_IRQ_STATUS_0, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* irq-1: irq status register 1 */
  { "irq-1", UBICOM32_OPERAND_IRQ_1, HW_H_IRQ_STATUS_1, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* iread: iread register */
  { "iread", UBICOM32_OPERAND_IREAD, HW_H_IREAD, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* opc1: primary opcode */
  { "opc1", UBICOM32_OPERAND_OPC1, HW_H_UINT, 31, 5,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_OP1] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* opc2: secondary opcode */
  { "opc2", UBICOM32_OPERAND_OPC2, HW_H_UINT, 15, 5,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_OP2] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* An-inc: An pre/post inc flag */
  { "An-inc", UBICOM32_OPERAND_AN_INC, HW_H_AR_INC_FLAG, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0|A(SEM_ONLY), { { { (1<<MACH_BASE), 0 } } } }  },
/* dsp-c: DSP Clip bit */
  { "dsp-c", UBICOM32_OPERAND_DSP_C, HW_H_DSP_C, 20, 1,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_DSP_C] } }, 
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }  },
/* dsp-t: DSP Top Half bit */
  { "dsp-t", UBICOM32_OPERAND_DSP_T, HW_H_DSP_T, 19, 1,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_DSP_T] } }, 
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }  },
/* dsp-destA: DSP Destination Acc Sel */
  { "dsp-destA", UBICOM32_OPERAND_DSP_DESTA, HW_H_DSP_DEST_A, 16, 1,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_DSP_DESTA] } }, 
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }  },
/* dsp-S2-sel: DSP S2 reg Select */
  { "dsp-S2-sel", UBICOM32_OPERAND_DSP_S2_SEL, HW_H_UINT, 18, 1,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_DSP_S2_SEL] } }, 
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }  },
/* dsp-S2-data-reg: DSP S2 is a data reg  */
  { "dsp-S2-data-reg", UBICOM32_OPERAND_DSP_S2_DATA_REG, HW_H_DR, 14, 4,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_DSP_S2] } }, 
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }  },
/* dsp-S2-acc-reg-mul: DSP S2 reg is a  Acc Lo reg */
  { "dsp-S2-acc-reg-mul", UBICOM32_OPERAND_DSP_S2_ACC_REG_MUL, HW_H_DSP_S2_ACC_REG_MUL, 14, 4,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_DSP_S2] } }, 
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }  },
/* dsp-S2-acc-reg-addsub: DSP S2 reg is a  Acc reg for madd and msuub */
  { "dsp-S2-acc-reg-addsub", UBICOM32_OPERAND_DSP_S2_ACC_REG_ADDSUB, HW_H_DSP_S2_ACC_REG_ADDSUB, 14, 4,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_DSP_S2] } }, 
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }  },
/* dsp-S2-data-reg-addsub: DSP S2 reg is a data reg for madd and msuub */
  { "dsp-S2-data-reg-addsub", UBICOM32_OPERAND_DSP_S2_DATA_REG_ADDSUB, HW_H_DR, 14, 4,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_DSP_S2] } }, 
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }  },
/* dsp-t-addsub: DSP Top Half spec for madd.2 and msub.2 */
  { "dsp-t-addsub", UBICOM32_OPERAND_DSP_T_ADDSUB, HW_H_DSP_T_ADDSUB, 19, 1,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_DSP_T] } }, 
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }  },
/* bit5: immediate bit index */
  { "bit5", UBICOM32_OPERAND_BIT5, HW_H_UINT, 15, 5,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_BIT5] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* bit5-addsub: immediate bit index */
  { "bit5-addsub", UBICOM32_OPERAND_BIT5_ADDSUB, HW_H_UINT, 15, 5,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_BIT5] } }, 
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }  },
/* fpu-32-S2-data-reg: FPU S2 is a 32 bit data register */
  { "fpu-32-S2-data-reg", UBICOM32_OPERAND_FPU_32_S2_DATA_REG, HW_H_FPU_DATA_REG_32, 14, 4,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_FPS2_REG32] } }, 
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
/* fpu-32-S2-acc-reg: FPU S2 is a 32 bit accumulator register */
  { "fpu-32-S2-acc-reg", UBICOM32_OPERAND_FPU_32_S2_ACC_REG, HW_H_FPU_ACC_32, 14, 4,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_FPS2_REG32] } }, 
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
/* fpu-64-S2-data-reg: FPU S2 is a 64 bit data register */
  { "fpu-64-S2-data-reg", UBICOM32_OPERAND_FPU_64_S2_DATA_REG, HW_H_FPU_DATA_REG_64, 14, 3,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_FPS2_REG64] } }, 
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
/* fpu-64-S2-acc-reg: FPU S2 is a 64 bit accumulator register */
  { "fpu-64-S2-acc-reg", UBICOM32_OPERAND_FPU_64_S2_ACC_REG, HW_H_FPU_ACC_64, 14, 3,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_FPS2_REG64] } }, 
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
/* fpu-64-S1-data-reg: FPU S1 is a 64 bit data register */
  { "fpu-64-S1-data-reg", UBICOM32_OPERAND_FPU_64_S1_DATA_REG, HW_H_FPU_DATA_REG_64, 3, 3,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_FPS1_REG64] } }, 
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
/* fpu-64-S1-acc-reg: FPU S1 is a 64 bit accumulator register */
  { "fpu-64-S1-acc-reg", UBICOM32_OPERAND_FPU_64_S1_ACC_REG, HW_H_FPU_ACC_64, 3, 3,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_FPS1_REG64] } }, 
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
/* fpu-32-dest-reg: FPU Destination is a 32 bit accumulator register */
  { "fpu-32-dest-reg", UBICOM32_OPERAND_FPU_32_DEST_REG, HW_H_FPU_ACC_32, 17, 2,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_FPD32] } }, 
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
/* fpu-64-dest-reg: FPU Destination is a 64 bit accumulator register */
  { "fpu-64-dest-reg", UBICOM32_OPERAND_FPU_64_DEST_REG, HW_H_FPU_ACC_64, 17, 1,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_FPD64] } }, 
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }  },
/* fpu-32-src2-data-reg:  */
/* fpu-32-src2-acc-reg:  */
/* dsp-src2-reg-acc-reg-mul:  */
/* dsp-src2-reg-acc-reg-addsub:  */
/* dsp-src2-data-reg:  */
/* dsp-src2-data-reg-addsub:  */
/* dsp-src2-data-reg-addsub2:  */
/* dsp-imm-bit5:  */
/* dsp-imm-bit5-addsub:  */
/* dsp-imm-bit5-addsub2:  */
/* imm-bit5:  */
/* dyn-reg:  */
/* op3: 5-bit immediate value or dynamic register specification */
/* dsp-src2-mul: Data register or accumulator lo register specification */
/* dsp-compatibility-src2-mul: Data register or accumulator lo register specification */
/* dsp-src2-addsub: Data register or accumulator register specification for madd msub instructions */
/* dsp-src2-addsub2: Data register or accumulator register specification for madd msub instructions */
/* offset21: 21-bit relative offset */
  { "offset21", UBICOM32_OPERAND_OFFSET21, HW_H_IADDR, 20, 21,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_O21] } }, 
    { 0|A(PCREL_ADDR), { { { (1<<MACH_BASE), 0 } } } }  },
/* offset16: 16-bit calli offset */
  { "offset16", UBICOM32_OPERAND_OFFSET16, HW_H_SINT, 4, 16,
    { 4, { (const PTR) &UBICOM32_F_O16_MULTI_IFIELD[0] } }, 
    { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
/* leai-offset16: 16-bit leai offset */
  { "leai-offset16", UBICOM32_OPERAND_LEAI_OFFSET16, HW_H_SINT, 4, 16,
    { 4, { (const PTR) &UBICOM32_F_LEAI_O16_MULTI_IFIELD[0] } }, 
    { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
/* imm24: 24-bit immediate */
  { "imm24", UBICOM32_OPERAND_IMM24, HW_H_UINT, 20, 24,
    { 2, { (const PTR) &UBICOM32_F_IMM24_MULTI_IFIELD[0] } }, 
    { 0|A(VIRTUAL), { { { (1<<MACH_UBICOM32_OLDMOVEAI), 0 } } } }  },
/* imm25: 25-bit immediate */
  { "imm25", UBICOM32_OPERAND_IMM25, HW_H_UINT, 20, 25,
    { 2, { (const PTR) &UBICOM32_F_IMM25_MULTI_IFIELD[0] } }, 
    { 0|A(VIRTUAL), { { { (1<<MACH_UBICOM32_NEWMOVEAI), 0 } } } }  },
/* nbit-16: 16-bit negative    bit */
  { "nbit-16", UBICOM32_OPERAND_NBIT_16, HW_H_NBIT_16, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0|A(SEM_ONLY), { { { (1<<MACH_BASE), 0 } } } }  },
/* vbit-16: 16-bit overflow    bit */
  { "vbit-16", UBICOM32_OPERAND_VBIT_16, HW_H_VBIT_16, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0|A(SEM_ONLY), { { { (1<<MACH_BASE), 0 } } } }  },
/* zbit-16: 16-bit zero        bit */
  { "zbit-16", UBICOM32_OPERAND_ZBIT_16, HW_H_ZBIT_16, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0|A(SEM_ONLY), { { { (1<<MACH_BASE), 0 } } } }  },
/* cbit-16: 16-bit carry       bit */
  { "cbit-16", UBICOM32_OPERAND_CBIT_16, HW_H_CBIT_16, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0|A(SEM_ONLY), { { { (1<<MACH_BASE), 0 } } } }  },
/* nbit-32: 32-bit negative    bit */
  { "nbit-32", UBICOM32_OPERAND_NBIT_32, HW_H_NBIT_32, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0|A(SEM_ONLY), { { { (1<<MACH_BASE), 0 } } } }  },
/* vbit-32: 32-bit overflow    bit */
  { "vbit-32", UBICOM32_OPERAND_VBIT_32, HW_H_VBIT_32, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0|A(SEM_ONLY), { { { (1<<MACH_BASE), 0 } } } }  },
/* zbit-32: 32-bit zero        bit */
  { "zbit-32", UBICOM32_OPERAND_ZBIT_32, HW_H_ZBIT_32, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0|A(SEM_ONLY), { { { (1<<MACH_BASE), 0 } } } }  },
/* cbit-32: 32-bit carry       bit */
  { "cbit-32", UBICOM32_OPERAND_CBIT_32, HW_H_CBIT_32, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0|A(SEM_ONLY), { { { (1<<MACH_BASE), 0 } } } }  },
/* privilege-bit: Privilege bit from CSR */
  { "privilege-bit", UBICOM32_OPERAND_PRIVILEGE_BIT, HW_H_PRIVBIT, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0|A(SEM_ONLY), { { { (1<<MACH_BASE), 0 } } } }  },
/* prev-privilege-bit: Previous Privilege bit from CSR */
  { "prev-privilege-bit", UBICOM32_OPERAND_PREV_PRIVILEGE_BIT, HW_H_PREV_PRIVBIT, 0, 0,
    { 0, { (const PTR) 0 } }, 
    { 0|A(SEM_ONLY), { { { (1<<MACH_BASE), 0 } } } }  },
/* s1-imm7-1: 7-bit immediate byte */
  { "s1-imm7-1", UBICOM32_OPERAND_S1_IMM7_1, HW_H_UINT, 4, 7,
    { 2, { (const PTR) &UBICOM32_F_S1_IMM7_1_MULTI_IFIELD[0] } }, 
    { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
/* s1-imm7-2: 7-bit immediate halfword */
  { "s1-imm7-2", UBICOM32_OPERAND_S1_IMM7_2, HW_H_UINT, 4, 7,
    { 2, { (const PTR) &UBICOM32_F_S1_IMM7_2_MULTI_IFIELD[0] } }, 
    { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
/* s1-imm7-4: 7-bit immediate word */
  { "s1-imm7-4", UBICOM32_OPERAND_S1_IMM7_4, HW_H_UINT, 4, 7,
    { 2, { (const PTR) &UBICOM32_F_S1_IMM7_4_MULTI_IFIELD[0] } }, 
    { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
/* pdec-s1-imm7-4: 7-bit immediate word for pdec */
  { "pdec-s1-imm7-4", UBICOM32_OPERAND_PDEC_S1_IMM7_4, HW_H_UINT, 4, 7,
    { 2, { (const PTR) &UBICOM32_F_S1_IMM7_4_MULTI_IFIELD[0] } }, 
    { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
/* s1-imm8: 8-bit signed immediate */
  { "s1-imm8", UBICOM32_OPERAND_S1_IMM8, HW_H_SINT, 7, 8,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_S1_IMM8] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* s1-An: s1 address register */
  { "s1-An", UBICOM32_OPERAND_S1_AN, HW_H_AR, 7, 3,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_S1_AN] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* s1-r: s1 index register */
  { "s1-r", UBICOM32_OPERAND_S1_R, HW_H_S1_DR, 4, 5,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_S1_R] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* s1-An-inc: s1 An register pre/post inc */
  { "s1-An-inc", UBICOM32_OPERAND_S1_AN_INC, HW_H_AR_INC, 7, 3,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_S1_AN] } }, 
    { 0|A(SEM_ONLY), { { { (1<<MACH_BASE), 0 } } } }  },
/* s1-i4-1: 4 bit signed-immediate value */
  { "s1-i4-1", UBICOM32_OPERAND_S1_I4_1, HW_H_SINT, 3, 4,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_S1_I4_1] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* s1-i4-2: 4 bit signed-immediate value */
  { "s1-i4-2", UBICOM32_OPERAND_S1_I4_2, HW_H_SINT, 3, 4,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_S1_I4_2] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* s1-i4-4: 4 bit signed-immediate value */
  { "s1-i4-4", UBICOM32_OPERAND_S1_I4_4, HW_H_SINT, 3, 4,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_S1_I4_4] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* s1-indirect-1:  */
/* s1-indirect-2:  */
/* s1-indirect-4:  */
/* s1-indirect-with-offset-1:  */
/* s1-indirect-with-offset-2:  */
/* s1-indirect-with-offset-4:  */
/* s1-indirect-with-index-1:  */
/* s1-indirect-with-index-2:  */
/* s1-indirect-with-index-4:  */
/* s1-indirect-with-post-increment-1:  */
/* s1-indirect-with-post-increment-2:  */
/* s1-indirect-with-post-increment-4:  */
/* s1-indirect-with-pre-increment-1:  */
/* s1-indirect-with-pre-increment-2:  */
/* s1-indirect-with-pre-increment-4:  */
/* s1-direct-addr: s1 direct address */
  { "s1-direct-addr", UBICOM32_OPERAND_S1_DIRECT_ADDR, HW_H_UINT, 7, 8,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_S1_DIRECT] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* s1-direct:  */
/* s1-immediate:  */
/* s1-1: source 1 operand 1 */
/* s1-2: source 1 operand 2 */
/* s1-4: source 1 operand 4 */
/* s1-ea-indirect:  */
/* s1-ea-indirect-with-offset-1:  */
/* s1-ea-indirect-with-offset-2:  */
/* s1-ea-indirect-with-offset-4:  */
/* s1-ea-indirect-with-index-1:  */
/* s1-ea-indirect-with-index-2:  */
/* s1-ea-indirect-with-index-4:  */
/* s1-ea-indirect-with-post-increment-1:  */
/* s1-ea-indirect-with-post-increment-2:  */
/* s1-ea-indirect-with-post-increment-4:  */
/* s1-ea-indirect-with-pre-increment-1:  */
/* s1-ea-indirect-with-pre-increment-2:  */
/* s1-ea-indirect-with-pre-increment-4:  */
/* s1-ea-immediate:  */
/* s1-ea-direct:  */
/* s1-ea-1: source 1 ea operand */
/* s1-ea-2: source 1 ea operand */
/* s1-ea-4: source 1 ea operand */
/* s1-pea: source 1 pea operand */
/* pdec-s1-ea-indirect-with-offset-4:  */
/* pdec-pea-s1: source 1 pea operand for pdec instruction */
/* d-imm7-1: 7-bit immediate byte */
  { "d-imm7-1", UBICOM32_OPERAND_D_IMM7_1, HW_H_UINT, 20, 7,
    { 2, { (const PTR) &UBICOM32_F_D_IMM7_1_MULTI_IFIELD[0] } }, 
    { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
/* d-imm7-2: 7-bit immediate halfword */
  { "d-imm7-2", UBICOM32_OPERAND_D_IMM7_2, HW_H_UINT, 20, 7,
    { 2, { (const PTR) &UBICOM32_F_D_IMM7_2_MULTI_IFIELD[0] } }, 
    { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
/* d-imm7-4: 7-bit immediate word */
  { "d-imm7-4", UBICOM32_OPERAND_D_IMM7_4, HW_H_UINT, 20, 7,
    { 2, { (const PTR) &UBICOM32_F_D_IMM7_4_MULTI_IFIELD[0] } }, 
    { 0|A(VIRTUAL), { { { (1<<MACH_BASE), 0 } } } }  },
/* d-imm8: 8-bit signed immediate */
  { "d-imm8", UBICOM32_OPERAND_D_IMM8, HW_H_SINT, 23, 8,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_D_IMM8] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* d-An: d address register */
  { "d-An", UBICOM32_OPERAND_D_AN, HW_H_AR, 23, 3,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_D_AN] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* d-r: d index register */
  { "d-r", UBICOM32_OPERAND_D_R, HW_H_DR, 20, 5,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_D_R] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* d-An-inc: d An register pre/post inc */
  { "d-An-inc", UBICOM32_OPERAND_D_AN_INC, HW_H_AR_INC, 23, 3,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_D_AN] } }, 
    { 0|A(SEM_ONLY), { { { (1<<MACH_BASE), 0 } } } }  },
/* d-i4-1: 4 bit signed-immediate value */
  { "d-i4-1", UBICOM32_OPERAND_D_I4_1, HW_H_SINT, 19, 4,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_D_I4_1] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* d-i4-2: 4 bit signed-immediate value */
  { "d-i4-2", UBICOM32_OPERAND_D_I4_2, HW_H_SINT, 19, 4,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_D_I4_2] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* d-i4-4: 4 bit signed-immediate value */
  { "d-i4-4", UBICOM32_OPERAND_D_I4_4, HW_H_SINT, 19, 4,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_D_I4_4] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* d-indirect-1:  */
/* d-indirect-2:  */
/* d-indirect-4:  */
/* d-indirect-with-offset-1:  */
/* d-indirect-with-offset-2:  */
/* d-indirect-with-offset-4:  */
/* d-indirect-with-index-1:  */
/* d-indirect-with-index-2:  */
/* d-indirect-with-index-4:  */
/* d-indirect-with-post-increment-1:  */
/* d-indirect-with-post-increment-2:  */
/* d-indirect-with-post-increment-4:  */
/* d-indirect-with-pre-increment-1:  */
/* d-indirect-with-pre-increment-2:  */
/* d-indirect-with-pre-increment-4:  */
/* d-direct-addr: dest direct address */
  { "d-direct-addr", UBICOM32_OPERAND_D_DIRECT_ADDR, HW_H_UINT, 23, 8,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_D_DIRECT] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* d-direct:  */
/* d-immediate-1:  */
/* d-immediate-2:  */
/* d-immediate-4:  */
/* d-1: destination operand 1 */
/* d-2: destination operand 2 */
/* d-4: destination operand 4 */
/* d-pea-indirect:  */
/* d-pea-indirect-with-offset:  */
/* d-pea-indirect-with-post-increment:  */
/* d-pea-indirect-with-pre-increment:  */
/* d-pea-indirect-with-index:  */
/* d-pea: destination 1 pea operand */
/* imm16-2: 16 bit immediate, for movei */
  { "imm16-2", UBICOM32_OPERAND_IMM16_2, HW_H_SINT, 15, 16,
    { 0, { (const PTR) &ubicom32_cgen_ifld_table[UBICOM32_F_IMM16_2] } }, 
    { 0, { { { (1<<MACH_BASE), 0 } } } }  },
/* sentinel */
  { 0, 0, 0, 0, 0,
    { 0, { (const PTR) 0 } },
    { 0, { { { (1<<MACH_BASE), 0 } } } } }
};

#undef A


/* The instruction table.  */

#define OP(field) CGEN_SYNTAX_MAKE_FIELD (OPERAND (field))
#if defined (__STDC__) || defined (ALMOST_STDC) || defined (HAVE_STRINGIZE)
#define A(a) (1 << CGEN_INSN_##a)
#else
#define A(a) (1 << CGEN_INSN_/**/a)
#endif

static const CGEN_IBASE ubicom32_cgen_insn_table[MAX_INSNS] =
{
  /* Special null first entry.
     A `num' value of zero is thus invalid.
     Also, the special `invalid' insn resides here.  */
  { 0, 0, 0, 0, { 0, { { { (1<<MACH_BASE), 0 } } } } },
/* fcmps ${s1-direct-addr},${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FCMPS_ACC_S1_DIRECT, "fcmps_acc-s1-direct", "fcmps", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fcmps #${s1-imm8},${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FCMPS_ACC_S1_IMMEDIATE, "fcmps_acc-s1-immediate", "fcmps", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fcmps (${s1-An},${s1-r}),${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FCMPS_ACC_S1_INDIRECT_WITH_INDEX_4, "fcmps_acc-s1-indirect-with-index-4", "fcmps", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fcmps ${s1-imm7-4}(${s1-An}),${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FCMPS_ACC_S1_INDIRECT_WITH_OFFSET_4, "fcmps_acc-s1-indirect-with-offset-4", "fcmps", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fcmps (${s1-An}),${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FCMPS_ACC_S1_INDIRECT_4, "fcmps_acc-s1-indirect-4", "fcmps", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fcmps (${s1-An})${s1-i4-4}++,${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FCMPS_ACC_S1_INDIRECT_WITH_POST_INCREMENT_4, "fcmps_acc-s1-indirect-with-post-increment-4", "fcmps", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fcmps ${s1-i4-4}(${s1-An})++,${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FCMPS_ACC_S1_INDIRECT_WITH_PRE_INCREMENT_4, "fcmps_acc-s1-indirect-with-pre-increment-4", "fcmps", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fcmps ${s1-direct-addr},${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FCMPS_DATA_S1_DIRECT, "fcmps_data-s1-direct", "fcmps", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fcmps #${s1-imm8},${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FCMPS_DATA_S1_IMMEDIATE, "fcmps_data-s1-immediate", "fcmps", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fcmps (${s1-An},${s1-r}),${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FCMPS_DATA_S1_INDIRECT_WITH_INDEX_4, "fcmps_data-s1-indirect-with-index-4", "fcmps", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fcmps ${s1-imm7-4}(${s1-An}),${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FCMPS_DATA_S1_INDIRECT_WITH_OFFSET_4, "fcmps_data-s1-indirect-with-offset-4", "fcmps", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fcmps (${s1-An}),${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FCMPS_DATA_S1_INDIRECT_4, "fcmps_data-s1-indirect-4", "fcmps", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fcmps (${s1-An})${s1-i4-4}++,${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FCMPS_DATA_S1_INDIRECT_WITH_POST_INCREMENT_4, "fcmps_data-s1-indirect-with-post-increment-4", "fcmps", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fcmps ${s1-i4-4}(${s1-An})++,${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FCMPS_DATA_S1_INDIRECT_WITH_PRE_INCREMENT_4, "fcmps_data-s1-indirect-with-pre-increment-4", "fcmps", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fs2l ${fpu-64-dest-reg},${s1-direct-addr} */
  {
    UBICOM32_INSN_FS2L_S1_DIRECT, "fs2l-s1-direct", "fs2l", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fs2l ${fpu-64-dest-reg},#${s1-imm8} */
  {
    UBICOM32_INSN_FS2L_S1_IMMEDIATE, "fs2l-s1-immediate", "fs2l", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fs2l ${fpu-64-dest-reg},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_FS2L_S1_INDIRECT_WITH_INDEX_4, "fs2l-s1-indirect-with-index-4", "fs2l", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fs2l ${fpu-64-dest-reg},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_FS2L_S1_INDIRECT_WITH_OFFSET_4, "fs2l-s1-indirect-with-offset-4", "fs2l", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fs2l ${fpu-64-dest-reg},(${s1-An}) */
  {
    UBICOM32_INSN_FS2L_S1_INDIRECT_4, "fs2l-s1-indirect-4", "fs2l", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fs2l ${fpu-64-dest-reg},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_FS2L_S1_INDIRECT_WITH_POST_INCREMENT_4, "fs2l-s1-indirect-with-post-increment-4", "fs2l", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fs2l ${fpu-64-dest-reg},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_FS2L_S1_INDIRECT_WITH_PRE_INCREMENT_4, "fs2l-s1-indirect-with-pre-increment-4", "fs2l", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fs2i ${fpu-32-dest-reg},${s1-direct-addr} */
  {
    UBICOM32_INSN_FS2I_S1_DIRECT, "fs2i-s1-direct", "fs2i", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fs2i ${fpu-32-dest-reg},#${s1-imm8} */
  {
    UBICOM32_INSN_FS2I_S1_IMMEDIATE, "fs2i-s1-immediate", "fs2i", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fs2i ${fpu-32-dest-reg},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_FS2I_S1_INDIRECT_WITH_INDEX_4, "fs2i-s1-indirect-with-index-4", "fs2i", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fs2i ${fpu-32-dest-reg},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_FS2I_S1_INDIRECT_WITH_OFFSET_4, "fs2i-s1-indirect-with-offset-4", "fs2i", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fs2i ${fpu-32-dest-reg},(${s1-An}) */
  {
    UBICOM32_INSN_FS2I_S1_INDIRECT_4, "fs2i-s1-indirect-4", "fs2i", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fs2i ${fpu-32-dest-reg},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_FS2I_S1_INDIRECT_WITH_POST_INCREMENT_4, "fs2i-s1-indirect-with-post-increment-4", "fs2i", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fs2i ${fpu-32-dest-reg},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_FS2I_S1_INDIRECT_WITH_PRE_INCREMENT_4, "fs2i-s1-indirect-with-pre-increment-4", "fs2i", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fs2d ${fpu-64-dest-reg},${s1-direct-addr} */
  {
    UBICOM32_INSN_FS2D_S1_DIRECT, "fs2d-s1-direct", "fs2d", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fs2d ${fpu-64-dest-reg},#${s1-imm8} */
  {
    UBICOM32_INSN_FS2D_S1_IMMEDIATE, "fs2d-s1-immediate", "fs2d", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fs2d ${fpu-64-dest-reg},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_FS2D_S1_INDIRECT_WITH_INDEX_4, "fs2d-s1-indirect-with-index-4", "fs2d", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fs2d ${fpu-64-dest-reg},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_FS2D_S1_INDIRECT_WITH_OFFSET_4, "fs2d-s1-indirect-with-offset-4", "fs2d", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fs2d ${fpu-64-dest-reg},(${s1-An}) */
  {
    UBICOM32_INSN_FS2D_S1_INDIRECT_4, "fs2d-s1-indirect-4", "fs2d", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fs2d ${fpu-64-dest-reg},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_FS2D_S1_INDIRECT_WITH_POST_INCREMENT_4, "fs2d-s1-indirect-with-post-increment-4", "fs2d", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fs2d ${fpu-64-dest-reg},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_FS2D_S1_INDIRECT_WITH_PRE_INCREMENT_4, "fs2d-s1-indirect-with-pre-increment-4", "fs2d", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fi2d ${fpu-64-dest-reg},${s1-direct-addr} */
  {
    UBICOM32_INSN_FI2D_S1_DIRECT, "fi2d-s1-direct", "fi2d", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fi2d ${fpu-64-dest-reg},#${s1-imm8} */
  {
    UBICOM32_INSN_FI2D_S1_IMMEDIATE, "fi2d-s1-immediate", "fi2d", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fi2d ${fpu-64-dest-reg},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_FI2D_S1_INDIRECT_WITH_INDEX_4, "fi2d-s1-indirect-with-index-4", "fi2d", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fi2d ${fpu-64-dest-reg},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_FI2D_S1_INDIRECT_WITH_OFFSET_4, "fi2d-s1-indirect-with-offset-4", "fi2d", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fi2d ${fpu-64-dest-reg},(${s1-An}) */
  {
    UBICOM32_INSN_FI2D_S1_INDIRECT_4, "fi2d-s1-indirect-4", "fi2d", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fi2d ${fpu-64-dest-reg},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_FI2D_S1_INDIRECT_WITH_POST_INCREMENT_4, "fi2d-s1-indirect-with-post-increment-4", "fi2d", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fi2d ${fpu-64-dest-reg},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_FI2D_S1_INDIRECT_WITH_PRE_INCREMENT_4, "fi2d-s1-indirect-with-pre-increment-4", "fi2d", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fi2s ${fpu-32-dest-reg},${s1-direct-addr} */
  {
    UBICOM32_INSN_FI2S_S1_DIRECT, "fi2s-s1-direct", "fi2s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fi2s ${fpu-32-dest-reg},#${s1-imm8} */
  {
    UBICOM32_INSN_FI2S_S1_IMMEDIATE, "fi2s-s1-immediate", "fi2s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fi2s ${fpu-32-dest-reg},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_FI2S_S1_INDIRECT_WITH_INDEX_4, "fi2s-s1-indirect-with-index-4", "fi2s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fi2s ${fpu-32-dest-reg},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_FI2S_S1_INDIRECT_WITH_OFFSET_4, "fi2s-s1-indirect-with-offset-4", "fi2s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fi2s ${fpu-32-dest-reg},(${s1-An}) */
  {
    UBICOM32_INSN_FI2S_S1_INDIRECT_4, "fi2s-s1-indirect-4", "fi2s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fi2s ${fpu-32-dest-reg},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_FI2S_S1_INDIRECT_WITH_POST_INCREMENT_4, "fi2s-s1-indirect-with-post-increment-4", "fi2s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fi2s ${fpu-32-dest-reg},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_FI2S_S1_INDIRECT_WITH_PRE_INCREMENT_4, "fi2s-s1-indirect-with-pre-increment-4", "fi2s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fabss ${fpu-32-dest-reg},${s1-direct-addr} */
  {
    UBICOM32_INSN_FABSS_S1_DIRECT, "fabss-s1-direct", "fabss", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fabss ${fpu-32-dest-reg},#${s1-imm8} */
  {
    UBICOM32_INSN_FABSS_S1_IMMEDIATE, "fabss-s1-immediate", "fabss", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fabss ${fpu-32-dest-reg},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_FABSS_S1_INDIRECT_WITH_INDEX_4, "fabss-s1-indirect-with-index-4", "fabss", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fabss ${fpu-32-dest-reg},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_FABSS_S1_INDIRECT_WITH_OFFSET_4, "fabss-s1-indirect-with-offset-4", "fabss", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fabss ${fpu-32-dest-reg},(${s1-An}) */
  {
    UBICOM32_INSN_FABSS_S1_INDIRECT_4, "fabss-s1-indirect-4", "fabss", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fabss ${fpu-32-dest-reg},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_FABSS_S1_INDIRECT_WITH_POST_INCREMENT_4, "fabss-s1-indirect-with-post-increment-4", "fabss", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fabss ${fpu-32-dest-reg},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_FABSS_S1_INDIRECT_WITH_PRE_INCREMENT_4, "fabss-s1-indirect-with-pre-increment-4", "fabss", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fnegs ${fpu-32-dest-reg},${s1-direct-addr} */
  {
    UBICOM32_INSN_FNEGS_S1_DIRECT, "fnegs-s1-direct", "fnegs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fnegs ${fpu-32-dest-reg},#${s1-imm8} */
  {
    UBICOM32_INSN_FNEGS_S1_IMMEDIATE, "fnegs-s1-immediate", "fnegs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fnegs ${fpu-32-dest-reg},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_FNEGS_S1_INDIRECT_WITH_INDEX_4, "fnegs-s1-indirect-with-index-4", "fnegs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fnegs ${fpu-32-dest-reg},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_FNEGS_S1_INDIRECT_WITH_OFFSET_4, "fnegs-s1-indirect-with-offset-4", "fnegs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fnegs ${fpu-32-dest-reg},(${s1-An}) */
  {
    UBICOM32_INSN_FNEGS_S1_INDIRECT_4, "fnegs-s1-indirect-4", "fnegs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fnegs ${fpu-32-dest-reg},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_FNEGS_S1_INDIRECT_WITH_POST_INCREMENT_4, "fnegs-s1-indirect-with-post-increment-4", "fnegs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fnegs ${fpu-32-dest-reg},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_FNEGS_S1_INDIRECT_WITH_PRE_INCREMENT_4, "fnegs-s1-indirect-with-pre-increment-4", "fnegs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsqrts ${fpu-32-dest-reg},${s1-direct-addr} */
  {
    UBICOM32_INSN_FSQRTS_S1_DIRECT, "fsqrts-s1-direct", "fsqrts", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsqrts ${fpu-32-dest-reg},#${s1-imm8} */
  {
    UBICOM32_INSN_FSQRTS_S1_IMMEDIATE, "fsqrts-s1-immediate", "fsqrts", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsqrts ${fpu-32-dest-reg},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_FSQRTS_S1_INDIRECT_WITH_INDEX_4, "fsqrts-s1-indirect-with-index-4", "fsqrts", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsqrts ${fpu-32-dest-reg},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_FSQRTS_S1_INDIRECT_WITH_OFFSET_4, "fsqrts-s1-indirect-with-offset-4", "fsqrts", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsqrts ${fpu-32-dest-reg},(${s1-An}) */
  {
    UBICOM32_INSN_FSQRTS_S1_INDIRECT_4, "fsqrts-s1-indirect-4", "fsqrts", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsqrts ${fpu-32-dest-reg},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_FSQRTS_S1_INDIRECT_WITH_POST_INCREMENT_4, "fsqrts-s1-indirect-with-post-increment-4", "fsqrts", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsqrts ${fpu-32-dest-reg},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_FSQRTS_S1_INDIRECT_WITH_PRE_INCREMENT_4, "fsqrts-s1-indirect-with-pre-increment-4", "fsqrts", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fdivs ${fpu-32-dest-reg},${s1-direct-addr},${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FDIVS_ACC_S1_DIRECT, "fdivs_acc-s1-direct", "fdivs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fdivs ${fpu-32-dest-reg},#${s1-imm8},${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FDIVS_ACC_S1_IMMEDIATE, "fdivs_acc-s1-immediate", "fdivs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fdivs ${fpu-32-dest-reg},(${s1-An},${s1-r}),${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FDIVS_ACC_S1_INDIRECT_WITH_INDEX_4, "fdivs_acc-s1-indirect-with-index-4", "fdivs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fdivs ${fpu-32-dest-reg},${s1-imm7-4}(${s1-An}),${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FDIVS_ACC_S1_INDIRECT_WITH_OFFSET_4, "fdivs_acc-s1-indirect-with-offset-4", "fdivs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fdivs ${fpu-32-dest-reg},(${s1-An}),${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FDIVS_ACC_S1_INDIRECT_4, "fdivs_acc-s1-indirect-4", "fdivs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fdivs ${fpu-32-dest-reg},(${s1-An})${s1-i4-4}++,${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FDIVS_ACC_S1_INDIRECT_WITH_POST_INCREMENT_4, "fdivs_acc-s1-indirect-with-post-increment-4", "fdivs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fdivs ${fpu-32-dest-reg},${s1-i4-4}(${s1-An})++,${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FDIVS_ACC_S1_INDIRECT_WITH_PRE_INCREMENT_4, "fdivs_acc-s1-indirect-with-pre-increment-4", "fdivs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fdivs ${fpu-32-dest-reg},${s1-direct-addr},${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FDIVS_DATA_S1_DIRECT, "fdivs_data-s1-direct", "fdivs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fdivs ${fpu-32-dest-reg},#${s1-imm8},${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FDIVS_DATA_S1_IMMEDIATE, "fdivs_data-s1-immediate", "fdivs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fdivs ${fpu-32-dest-reg},(${s1-An},${s1-r}),${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FDIVS_DATA_S1_INDIRECT_WITH_INDEX_4, "fdivs_data-s1-indirect-with-index-4", "fdivs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fdivs ${fpu-32-dest-reg},${s1-imm7-4}(${s1-An}),${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FDIVS_DATA_S1_INDIRECT_WITH_OFFSET_4, "fdivs_data-s1-indirect-with-offset-4", "fdivs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fdivs ${fpu-32-dest-reg},(${s1-An}),${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FDIVS_DATA_S1_INDIRECT_4, "fdivs_data-s1-indirect-4", "fdivs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fdivs ${fpu-32-dest-reg},(${s1-An})${s1-i4-4}++,${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FDIVS_DATA_S1_INDIRECT_WITH_POST_INCREMENT_4, "fdivs_data-s1-indirect-with-post-increment-4", "fdivs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fdivs ${fpu-32-dest-reg},${s1-i4-4}(${s1-An})++,${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FDIVS_DATA_S1_INDIRECT_WITH_PRE_INCREMENT_4, "fdivs_data-s1-indirect-with-pre-increment-4", "fdivs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fmuls ${fpu-32-dest-reg},${s1-direct-addr},${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FMULS_ACC_S1_DIRECT, "fmuls_acc-s1-direct", "fmuls", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fmuls ${fpu-32-dest-reg},#${s1-imm8},${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FMULS_ACC_S1_IMMEDIATE, "fmuls_acc-s1-immediate", "fmuls", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fmuls ${fpu-32-dest-reg},(${s1-An},${s1-r}),${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FMULS_ACC_S1_INDIRECT_WITH_INDEX_4, "fmuls_acc-s1-indirect-with-index-4", "fmuls", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fmuls ${fpu-32-dest-reg},${s1-imm7-4}(${s1-An}),${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FMULS_ACC_S1_INDIRECT_WITH_OFFSET_4, "fmuls_acc-s1-indirect-with-offset-4", "fmuls", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fmuls ${fpu-32-dest-reg},(${s1-An}),${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FMULS_ACC_S1_INDIRECT_4, "fmuls_acc-s1-indirect-4", "fmuls", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fmuls ${fpu-32-dest-reg},(${s1-An})${s1-i4-4}++,${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FMULS_ACC_S1_INDIRECT_WITH_POST_INCREMENT_4, "fmuls_acc-s1-indirect-with-post-increment-4", "fmuls", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fmuls ${fpu-32-dest-reg},${s1-i4-4}(${s1-An})++,${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FMULS_ACC_S1_INDIRECT_WITH_PRE_INCREMENT_4, "fmuls_acc-s1-indirect-with-pre-increment-4", "fmuls", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fmuls ${fpu-32-dest-reg},${s1-direct-addr},${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FMULS_DATA_S1_DIRECT, "fmuls_data-s1-direct", "fmuls", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fmuls ${fpu-32-dest-reg},#${s1-imm8},${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FMULS_DATA_S1_IMMEDIATE, "fmuls_data-s1-immediate", "fmuls", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fmuls ${fpu-32-dest-reg},(${s1-An},${s1-r}),${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FMULS_DATA_S1_INDIRECT_WITH_INDEX_4, "fmuls_data-s1-indirect-with-index-4", "fmuls", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fmuls ${fpu-32-dest-reg},${s1-imm7-4}(${s1-An}),${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FMULS_DATA_S1_INDIRECT_WITH_OFFSET_4, "fmuls_data-s1-indirect-with-offset-4", "fmuls", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fmuls ${fpu-32-dest-reg},(${s1-An}),${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FMULS_DATA_S1_INDIRECT_4, "fmuls_data-s1-indirect-4", "fmuls", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fmuls ${fpu-32-dest-reg},(${s1-An})${s1-i4-4}++,${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FMULS_DATA_S1_INDIRECT_WITH_POST_INCREMENT_4, "fmuls_data-s1-indirect-with-post-increment-4", "fmuls", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fmuls ${fpu-32-dest-reg},${s1-i4-4}(${s1-An})++,${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FMULS_DATA_S1_INDIRECT_WITH_PRE_INCREMENT_4, "fmuls_data-s1-indirect-with-pre-increment-4", "fmuls", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsubs ${fpu-32-dest-reg},${s1-direct-addr},${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FSUBS_ACC_S1_DIRECT, "fsubs_acc-s1-direct", "fsubs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsubs ${fpu-32-dest-reg},#${s1-imm8},${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FSUBS_ACC_S1_IMMEDIATE, "fsubs_acc-s1-immediate", "fsubs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsubs ${fpu-32-dest-reg},(${s1-An},${s1-r}),${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FSUBS_ACC_S1_INDIRECT_WITH_INDEX_4, "fsubs_acc-s1-indirect-with-index-4", "fsubs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsubs ${fpu-32-dest-reg},${s1-imm7-4}(${s1-An}),${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FSUBS_ACC_S1_INDIRECT_WITH_OFFSET_4, "fsubs_acc-s1-indirect-with-offset-4", "fsubs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsubs ${fpu-32-dest-reg},(${s1-An}),${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FSUBS_ACC_S1_INDIRECT_4, "fsubs_acc-s1-indirect-4", "fsubs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsubs ${fpu-32-dest-reg},(${s1-An})${s1-i4-4}++,${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FSUBS_ACC_S1_INDIRECT_WITH_POST_INCREMENT_4, "fsubs_acc-s1-indirect-with-post-increment-4", "fsubs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsubs ${fpu-32-dest-reg},${s1-i4-4}(${s1-An})++,${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FSUBS_ACC_S1_INDIRECT_WITH_PRE_INCREMENT_4, "fsubs_acc-s1-indirect-with-pre-increment-4", "fsubs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsubs ${fpu-32-dest-reg},${s1-direct-addr},${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FSUBS_DATA_S1_DIRECT, "fsubs_data-s1-direct", "fsubs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsubs ${fpu-32-dest-reg},#${s1-imm8},${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FSUBS_DATA_S1_IMMEDIATE, "fsubs_data-s1-immediate", "fsubs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsubs ${fpu-32-dest-reg},(${s1-An},${s1-r}),${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FSUBS_DATA_S1_INDIRECT_WITH_INDEX_4, "fsubs_data-s1-indirect-with-index-4", "fsubs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsubs ${fpu-32-dest-reg},${s1-imm7-4}(${s1-An}),${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FSUBS_DATA_S1_INDIRECT_WITH_OFFSET_4, "fsubs_data-s1-indirect-with-offset-4", "fsubs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsubs ${fpu-32-dest-reg},(${s1-An}),${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FSUBS_DATA_S1_INDIRECT_4, "fsubs_data-s1-indirect-4", "fsubs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsubs ${fpu-32-dest-reg},(${s1-An})${s1-i4-4}++,${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FSUBS_DATA_S1_INDIRECT_WITH_POST_INCREMENT_4, "fsubs_data-s1-indirect-with-post-increment-4", "fsubs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsubs ${fpu-32-dest-reg},${s1-i4-4}(${s1-An})++,${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FSUBS_DATA_S1_INDIRECT_WITH_PRE_INCREMENT_4, "fsubs_data-s1-indirect-with-pre-increment-4", "fsubs", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fadds ${fpu-32-dest-reg},${s1-direct-addr},${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FADDS_ACC_S1_DIRECT, "fadds_acc-s1-direct", "fadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fadds ${fpu-32-dest-reg},#${s1-imm8},${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FADDS_ACC_S1_IMMEDIATE, "fadds_acc-s1-immediate", "fadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fadds ${fpu-32-dest-reg},(${s1-An},${s1-r}),${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FADDS_ACC_S1_INDIRECT_WITH_INDEX_4, "fadds_acc-s1-indirect-with-index-4", "fadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fadds ${fpu-32-dest-reg},${s1-imm7-4}(${s1-An}),${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FADDS_ACC_S1_INDIRECT_WITH_OFFSET_4, "fadds_acc-s1-indirect-with-offset-4", "fadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fadds ${fpu-32-dest-reg},(${s1-An}),${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FADDS_ACC_S1_INDIRECT_4, "fadds_acc-s1-indirect-4", "fadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fadds ${fpu-32-dest-reg},(${s1-An})${s1-i4-4}++,${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FADDS_ACC_S1_INDIRECT_WITH_POST_INCREMENT_4, "fadds_acc-s1-indirect-with-post-increment-4", "fadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fadds ${fpu-32-dest-reg},${s1-i4-4}(${s1-An})++,${fpu-32-S2-acc-reg} */
  {
    UBICOM32_INSN_FADDS_ACC_S1_INDIRECT_WITH_PRE_INCREMENT_4, "fadds_acc-s1-indirect-with-pre-increment-4", "fadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fadds ${fpu-32-dest-reg},${s1-direct-addr},${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FADDS_DATA_S1_DIRECT, "fadds_data-s1-direct", "fadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fadds ${fpu-32-dest-reg},#${s1-imm8},${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FADDS_DATA_S1_IMMEDIATE, "fadds_data-s1-immediate", "fadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fadds ${fpu-32-dest-reg},(${s1-An},${s1-r}),${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FADDS_DATA_S1_INDIRECT_WITH_INDEX_4, "fadds_data-s1-indirect-with-index-4", "fadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fadds ${fpu-32-dest-reg},${s1-imm7-4}(${s1-An}),${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FADDS_DATA_S1_INDIRECT_WITH_OFFSET_4, "fadds_data-s1-indirect-with-offset-4", "fadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fadds ${fpu-32-dest-reg},(${s1-An}),${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FADDS_DATA_S1_INDIRECT_4, "fadds_data-s1-indirect-4", "fadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fadds ${fpu-32-dest-reg},(${s1-An})${s1-i4-4}++,${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FADDS_DATA_S1_INDIRECT_WITH_POST_INCREMENT_4, "fadds_data-s1-indirect-with-post-increment-4", "fadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fadds ${fpu-32-dest-reg},${s1-i4-4}(${s1-An})++,${fpu-32-S2-data-reg} */
  {
    UBICOM32_INSN_FADDS_DATA_S1_INDIRECT_WITH_PRE_INCREMENT_4, "fadds_data-s1-indirect-with-pre-increment-4", "fadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* msub.2${dsp-c}${dsp-t-addsub} ${dsp-destA},${s1-direct-addr},${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_2_S1_DIRECT_DSP_SRC2_DATA_REG_ADDSUB2, "dsp-msub.2-s1-direct-dsp-src2-data-reg-addsub2", "msub.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.2${dsp-c}${dsp-t-addsub} ${dsp-destA},#${s1-imm8},${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_2_S1_IMMEDIATE_DSP_SRC2_DATA_REG_ADDSUB2, "dsp-msub.2-s1-immediate-dsp-src2-data-reg-addsub2", "msub.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.2${dsp-c}${dsp-t-addsub} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_2_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_DATA_REG_ADDSUB2, "dsp-msub.2-s1-indirect-with-index-2-dsp-src2-data-reg-addsub2", "msub.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.2${dsp-c}${dsp-t-addsub} ${dsp-destA},${s1-imm7-2}(${s1-An}),${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_2_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_DATA_REG_ADDSUB2, "dsp-msub.2-s1-indirect-with-offset-2-dsp-src2-data-reg-addsub2", "msub.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.2${dsp-c}${dsp-t-addsub} ${dsp-destA},(${s1-An}),${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_2_S1_INDIRECT_2_DSP_SRC2_DATA_REG_ADDSUB2, "dsp-msub.2-s1-indirect-2-dsp-src2-data-reg-addsub2", "msub.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.2${dsp-c}${dsp-t-addsub} ${dsp-destA},(${s1-An})${s1-i4-2}++,${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_2_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_DATA_REG_ADDSUB2, "dsp-msub.2-s1-indirect-with-post-increment-2-dsp-src2-data-reg-addsub2", "msub.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.2${dsp-c}${dsp-t-addsub} ${dsp-destA},${s1-i4-2}(${s1-An})++,${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_2_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_DATA_REG_ADDSUB2, "dsp-msub.2-s1-indirect-with-pre-increment-2-dsp-src2-data-reg-addsub2", "msub.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.2${dsp-c}${dsp-t-addsub} ${dsp-destA},${s1-direct-addr},${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_2_S1_DIRECT_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-msub.2-s1-direct-dsp-src2-reg-acc-reg-addsub", "msub.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.2${dsp-c}${dsp-t-addsub} ${dsp-destA},#${s1-imm8},${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_2_S1_IMMEDIATE_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-msub.2-s1-immediate-dsp-src2-reg-acc-reg-addsub", "msub.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.2${dsp-c}${dsp-t-addsub} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_2_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-msub.2-s1-indirect-with-index-2-dsp-src2-reg-acc-reg-addsub", "msub.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.2${dsp-c}${dsp-t-addsub} ${dsp-destA},${s1-imm7-2}(${s1-An}),${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_2_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-msub.2-s1-indirect-with-offset-2-dsp-src2-reg-acc-reg-addsub", "msub.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.2${dsp-c}${dsp-t-addsub} ${dsp-destA},(${s1-An}),${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_2_S1_INDIRECT_2_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-msub.2-s1-indirect-2-dsp-src2-reg-acc-reg-addsub", "msub.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.2${dsp-c}${dsp-t-addsub} ${dsp-destA},(${s1-An})${s1-i4-2}++,${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_2_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-msub.2-s1-indirect-with-post-increment-2-dsp-src2-reg-acc-reg-addsub", "msub.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.2${dsp-c}${dsp-t-addsub} ${dsp-destA},${s1-i4-2}(${s1-An})++,${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_2_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-msub.2-s1-indirect-with-pre-increment-2-dsp-src2-reg-acc-reg-addsub", "msub.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.2${dsp-c}${dsp-t-addsub} ${dsp-destA},${s1-direct-addr},#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_2_S1_DIRECT_DSP_IMM_BIT5_ADDSUB2, "dsp-msub.2-s1-direct-dsp-imm-bit5-addsub2", "msub.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.2${dsp-c}${dsp-t-addsub} ${dsp-destA},#${s1-imm8},#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_2_S1_IMMEDIATE_DSP_IMM_BIT5_ADDSUB2, "dsp-msub.2-s1-immediate-dsp-imm-bit5-addsub2", "msub.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.2${dsp-c}${dsp-t-addsub} ${dsp-destA},(${s1-An},${s1-r}),#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_2_S1_INDIRECT_WITH_INDEX_2_DSP_IMM_BIT5_ADDSUB2, "dsp-msub.2-s1-indirect-with-index-2-dsp-imm-bit5-addsub2", "msub.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.2${dsp-c}${dsp-t-addsub} ${dsp-destA},${s1-imm7-2}(${s1-An}),#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_2_S1_INDIRECT_WITH_OFFSET_2_DSP_IMM_BIT5_ADDSUB2, "dsp-msub.2-s1-indirect-with-offset-2-dsp-imm-bit5-addsub2", "msub.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.2${dsp-c}${dsp-t-addsub} ${dsp-destA},(${s1-An}),#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_2_S1_INDIRECT_2_DSP_IMM_BIT5_ADDSUB2, "dsp-msub.2-s1-indirect-2-dsp-imm-bit5-addsub2", "msub.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.2${dsp-c}${dsp-t-addsub} ${dsp-destA},(${s1-An})${s1-i4-2}++,#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_2_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_IMM_BIT5_ADDSUB2, "dsp-msub.2-s1-indirect-with-post-increment-2-dsp-imm-bit5-addsub2", "msub.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.2${dsp-c}${dsp-t-addsub} ${dsp-destA},${s1-i4-2}(${s1-An})++,#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_2_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_IMM_BIT5_ADDSUB2, "dsp-msub.2-s1-indirect-with-pre-increment-2-dsp-imm-bit5-addsub2", "msub.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.4${dsp-c} ${dsp-destA},${s1-direct-addr},${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_4_S1_DIRECT_DSP_SRC2_DATA_REG_ADDSUB, "dsp-msub.4-s1-direct-dsp-src2-data-reg-addsub", "msub.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.4${dsp-c} ${dsp-destA},#${s1-imm8},${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_4_S1_IMMEDIATE_DSP_SRC2_DATA_REG_ADDSUB, "dsp-msub.4-s1-immediate-dsp-src2-data-reg-addsub", "msub.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.4${dsp-c} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_4_S1_INDIRECT_WITH_INDEX_4_DSP_SRC2_DATA_REG_ADDSUB, "dsp-msub.4-s1-indirect-with-index-4-dsp-src2-data-reg-addsub", "msub.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.4${dsp-c} ${dsp-destA},${s1-imm7-4}(${s1-An}),${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_4_S1_INDIRECT_WITH_OFFSET_4_DSP_SRC2_DATA_REG_ADDSUB, "dsp-msub.4-s1-indirect-with-offset-4-dsp-src2-data-reg-addsub", "msub.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.4${dsp-c} ${dsp-destA},(${s1-An}),${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_4_S1_INDIRECT_4_DSP_SRC2_DATA_REG_ADDSUB, "dsp-msub.4-s1-indirect-4-dsp-src2-data-reg-addsub", "msub.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.4${dsp-c} ${dsp-destA},(${s1-An})${s1-i4-4}++,${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_4_S1_INDIRECT_WITH_POST_INCREMENT_4_DSP_SRC2_DATA_REG_ADDSUB, "dsp-msub.4-s1-indirect-with-post-increment-4-dsp-src2-data-reg-addsub", "msub.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.4${dsp-c} ${dsp-destA},${s1-i4-4}(${s1-An})++,${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_4_S1_INDIRECT_WITH_PRE_INCREMENT_4_DSP_SRC2_DATA_REG_ADDSUB, "dsp-msub.4-s1-indirect-with-pre-increment-4-dsp-src2-data-reg-addsub", "msub.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.4${dsp-c} ${dsp-destA},${s1-direct-addr},${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_4_S1_DIRECT_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-msub.4-s1-direct-dsp-src2-reg-acc-reg-addsub", "msub.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.4${dsp-c} ${dsp-destA},#${s1-imm8},${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_4_S1_IMMEDIATE_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-msub.4-s1-immediate-dsp-src2-reg-acc-reg-addsub", "msub.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.4${dsp-c} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_4_S1_INDIRECT_WITH_INDEX_4_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-msub.4-s1-indirect-with-index-4-dsp-src2-reg-acc-reg-addsub", "msub.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.4${dsp-c} ${dsp-destA},${s1-imm7-4}(${s1-An}),${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_4_S1_INDIRECT_WITH_OFFSET_4_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-msub.4-s1-indirect-with-offset-4-dsp-src2-reg-acc-reg-addsub", "msub.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.4${dsp-c} ${dsp-destA},(${s1-An}),${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_4_S1_INDIRECT_4_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-msub.4-s1-indirect-4-dsp-src2-reg-acc-reg-addsub", "msub.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.4${dsp-c} ${dsp-destA},(${s1-An})${s1-i4-4}++,${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_4_S1_INDIRECT_WITH_POST_INCREMENT_4_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-msub.4-s1-indirect-with-post-increment-4-dsp-src2-reg-acc-reg-addsub", "msub.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.4${dsp-c} ${dsp-destA},${s1-i4-4}(${s1-An})++,${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_4_S1_INDIRECT_WITH_PRE_INCREMENT_4_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-msub.4-s1-indirect-with-pre-increment-4-dsp-src2-reg-acc-reg-addsub", "msub.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.4${dsp-c} ${dsp-destA},${s1-direct-addr},#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_4_S1_DIRECT_DSP_IMM_BIT5_ADDSUB, "dsp-msub.4-s1-direct-dsp-imm-bit5-addsub", "msub.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.4${dsp-c} ${dsp-destA},#${s1-imm8},#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_4_S1_IMMEDIATE_DSP_IMM_BIT5_ADDSUB, "dsp-msub.4-s1-immediate-dsp-imm-bit5-addsub", "msub.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.4${dsp-c} ${dsp-destA},(${s1-An},${s1-r}),#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_4_S1_INDIRECT_WITH_INDEX_4_DSP_IMM_BIT5_ADDSUB, "dsp-msub.4-s1-indirect-with-index-4-dsp-imm-bit5-addsub", "msub.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.4${dsp-c} ${dsp-destA},${s1-imm7-4}(${s1-An}),#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_4_S1_INDIRECT_WITH_OFFSET_4_DSP_IMM_BIT5_ADDSUB, "dsp-msub.4-s1-indirect-with-offset-4-dsp-imm-bit5-addsub", "msub.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.4${dsp-c} ${dsp-destA},(${s1-An}),#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_4_S1_INDIRECT_4_DSP_IMM_BIT5_ADDSUB, "dsp-msub.4-s1-indirect-4-dsp-imm-bit5-addsub", "msub.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.4${dsp-c} ${dsp-destA},(${s1-An})${s1-i4-4}++,#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_4_S1_INDIRECT_WITH_POST_INCREMENT_4_DSP_IMM_BIT5_ADDSUB, "dsp-msub.4-s1-indirect-with-post-increment-4-dsp-imm-bit5-addsub", "msub.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msub.4${dsp-c} ${dsp-destA},${s1-i4-4}(${s1-An})++,#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MSUB_4_S1_INDIRECT_WITH_PRE_INCREMENT_4_DSP_IMM_BIT5_ADDSUB, "dsp-msub.4-s1-indirect-with-pre-increment-4-dsp-imm-bit5-addsub", "msub.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.2${dsp-c}${dsp-t-addsub} ${dsp-destA},${s1-direct-addr},${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_2_S1_DIRECT_DSP_SRC2_DATA_REG_ADDSUB2, "dsp-madd.2-s1-direct-dsp-src2-data-reg-addsub2", "madd.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.2${dsp-c}${dsp-t-addsub} ${dsp-destA},#${s1-imm8},${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_2_S1_IMMEDIATE_DSP_SRC2_DATA_REG_ADDSUB2, "dsp-madd.2-s1-immediate-dsp-src2-data-reg-addsub2", "madd.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.2${dsp-c}${dsp-t-addsub} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_2_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_DATA_REG_ADDSUB2, "dsp-madd.2-s1-indirect-with-index-2-dsp-src2-data-reg-addsub2", "madd.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.2${dsp-c}${dsp-t-addsub} ${dsp-destA},${s1-imm7-2}(${s1-An}),${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_2_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_DATA_REG_ADDSUB2, "dsp-madd.2-s1-indirect-with-offset-2-dsp-src2-data-reg-addsub2", "madd.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.2${dsp-c}${dsp-t-addsub} ${dsp-destA},(${s1-An}),${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_2_S1_INDIRECT_2_DSP_SRC2_DATA_REG_ADDSUB2, "dsp-madd.2-s1-indirect-2-dsp-src2-data-reg-addsub2", "madd.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.2${dsp-c}${dsp-t-addsub} ${dsp-destA},(${s1-An})${s1-i4-2}++,${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_2_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_DATA_REG_ADDSUB2, "dsp-madd.2-s1-indirect-with-post-increment-2-dsp-src2-data-reg-addsub2", "madd.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.2${dsp-c}${dsp-t-addsub} ${dsp-destA},${s1-i4-2}(${s1-An})++,${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_2_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_DATA_REG_ADDSUB2, "dsp-madd.2-s1-indirect-with-pre-increment-2-dsp-src2-data-reg-addsub2", "madd.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.2${dsp-c}${dsp-t-addsub} ${dsp-destA},${s1-direct-addr},${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_2_S1_DIRECT_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-madd.2-s1-direct-dsp-src2-reg-acc-reg-addsub", "madd.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.2${dsp-c}${dsp-t-addsub} ${dsp-destA},#${s1-imm8},${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_2_S1_IMMEDIATE_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-madd.2-s1-immediate-dsp-src2-reg-acc-reg-addsub", "madd.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.2${dsp-c}${dsp-t-addsub} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_2_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-madd.2-s1-indirect-with-index-2-dsp-src2-reg-acc-reg-addsub", "madd.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.2${dsp-c}${dsp-t-addsub} ${dsp-destA},${s1-imm7-2}(${s1-An}),${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_2_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-madd.2-s1-indirect-with-offset-2-dsp-src2-reg-acc-reg-addsub", "madd.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.2${dsp-c}${dsp-t-addsub} ${dsp-destA},(${s1-An}),${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_2_S1_INDIRECT_2_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-madd.2-s1-indirect-2-dsp-src2-reg-acc-reg-addsub", "madd.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.2${dsp-c}${dsp-t-addsub} ${dsp-destA},(${s1-An})${s1-i4-2}++,${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_2_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-madd.2-s1-indirect-with-post-increment-2-dsp-src2-reg-acc-reg-addsub", "madd.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.2${dsp-c}${dsp-t-addsub} ${dsp-destA},${s1-i4-2}(${s1-An})++,${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_2_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-madd.2-s1-indirect-with-pre-increment-2-dsp-src2-reg-acc-reg-addsub", "madd.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.2${dsp-c}${dsp-t-addsub} ${dsp-destA},${s1-direct-addr},#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_2_S1_DIRECT_DSP_IMM_BIT5_ADDSUB2, "dsp-madd.2-s1-direct-dsp-imm-bit5-addsub2", "madd.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.2${dsp-c}${dsp-t-addsub} ${dsp-destA},#${s1-imm8},#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_2_S1_IMMEDIATE_DSP_IMM_BIT5_ADDSUB2, "dsp-madd.2-s1-immediate-dsp-imm-bit5-addsub2", "madd.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.2${dsp-c}${dsp-t-addsub} ${dsp-destA},(${s1-An},${s1-r}),#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_2_S1_INDIRECT_WITH_INDEX_2_DSP_IMM_BIT5_ADDSUB2, "dsp-madd.2-s1-indirect-with-index-2-dsp-imm-bit5-addsub2", "madd.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.2${dsp-c}${dsp-t-addsub} ${dsp-destA},${s1-imm7-2}(${s1-An}),#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_2_S1_INDIRECT_WITH_OFFSET_2_DSP_IMM_BIT5_ADDSUB2, "dsp-madd.2-s1-indirect-with-offset-2-dsp-imm-bit5-addsub2", "madd.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.2${dsp-c}${dsp-t-addsub} ${dsp-destA},(${s1-An}),#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_2_S1_INDIRECT_2_DSP_IMM_BIT5_ADDSUB2, "dsp-madd.2-s1-indirect-2-dsp-imm-bit5-addsub2", "madd.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.2${dsp-c}${dsp-t-addsub} ${dsp-destA},(${s1-An})${s1-i4-2}++,#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_2_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_IMM_BIT5_ADDSUB2, "dsp-madd.2-s1-indirect-with-post-increment-2-dsp-imm-bit5-addsub2", "madd.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.2${dsp-c}${dsp-t-addsub} ${dsp-destA},${s1-i4-2}(${s1-An})++,#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_2_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_IMM_BIT5_ADDSUB2, "dsp-madd.2-s1-indirect-with-pre-increment-2-dsp-imm-bit5-addsub2", "madd.2", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.4${dsp-c} ${dsp-destA},${s1-direct-addr},${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_4_S1_DIRECT_DSP_SRC2_DATA_REG_ADDSUB, "dsp-madd.4-s1-direct-dsp-src2-data-reg-addsub", "madd.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.4${dsp-c} ${dsp-destA},#${s1-imm8},${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_4_S1_IMMEDIATE_DSP_SRC2_DATA_REG_ADDSUB, "dsp-madd.4-s1-immediate-dsp-src2-data-reg-addsub", "madd.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.4${dsp-c} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_4_S1_INDIRECT_WITH_INDEX_4_DSP_SRC2_DATA_REG_ADDSUB, "dsp-madd.4-s1-indirect-with-index-4-dsp-src2-data-reg-addsub", "madd.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.4${dsp-c} ${dsp-destA},${s1-imm7-4}(${s1-An}),${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_4_S1_INDIRECT_WITH_OFFSET_4_DSP_SRC2_DATA_REG_ADDSUB, "dsp-madd.4-s1-indirect-with-offset-4-dsp-src2-data-reg-addsub", "madd.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.4${dsp-c} ${dsp-destA},(${s1-An}),${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_4_S1_INDIRECT_4_DSP_SRC2_DATA_REG_ADDSUB, "dsp-madd.4-s1-indirect-4-dsp-src2-data-reg-addsub", "madd.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.4${dsp-c} ${dsp-destA},(${s1-An})${s1-i4-4}++,${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_4_S1_INDIRECT_WITH_POST_INCREMENT_4_DSP_SRC2_DATA_REG_ADDSUB, "dsp-madd.4-s1-indirect-with-post-increment-4-dsp-src2-data-reg-addsub", "madd.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.4${dsp-c} ${dsp-destA},${s1-i4-4}(${s1-An})++,${dsp-S2-data-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_4_S1_INDIRECT_WITH_PRE_INCREMENT_4_DSP_SRC2_DATA_REG_ADDSUB, "dsp-madd.4-s1-indirect-with-pre-increment-4-dsp-src2-data-reg-addsub", "madd.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.4${dsp-c} ${dsp-destA},${s1-direct-addr},${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_4_S1_DIRECT_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-madd.4-s1-direct-dsp-src2-reg-acc-reg-addsub", "madd.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.4${dsp-c} ${dsp-destA},#${s1-imm8},${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_4_S1_IMMEDIATE_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-madd.4-s1-immediate-dsp-src2-reg-acc-reg-addsub", "madd.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.4${dsp-c} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_4_S1_INDIRECT_WITH_INDEX_4_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-madd.4-s1-indirect-with-index-4-dsp-src2-reg-acc-reg-addsub", "madd.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.4${dsp-c} ${dsp-destA},${s1-imm7-4}(${s1-An}),${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_4_S1_INDIRECT_WITH_OFFSET_4_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-madd.4-s1-indirect-with-offset-4-dsp-src2-reg-acc-reg-addsub", "madd.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.4${dsp-c} ${dsp-destA},(${s1-An}),${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_4_S1_INDIRECT_4_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-madd.4-s1-indirect-4-dsp-src2-reg-acc-reg-addsub", "madd.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.4${dsp-c} ${dsp-destA},(${s1-An})${s1-i4-4}++,${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_4_S1_INDIRECT_WITH_POST_INCREMENT_4_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-madd.4-s1-indirect-with-post-increment-4-dsp-src2-reg-acc-reg-addsub", "madd.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.4${dsp-c} ${dsp-destA},${s1-i4-4}(${s1-An})++,${dsp-S2-acc-reg-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_4_S1_INDIRECT_WITH_PRE_INCREMENT_4_DSP_SRC2_REG_ACC_REG_ADDSUB, "dsp-madd.4-s1-indirect-with-pre-increment-4-dsp-src2-reg-acc-reg-addsub", "madd.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.4${dsp-c} ${dsp-destA},${s1-direct-addr},#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_4_S1_DIRECT_DSP_IMM_BIT5_ADDSUB, "dsp-madd.4-s1-direct-dsp-imm-bit5-addsub", "madd.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.4${dsp-c} ${dsp-destA},#${s1-imm8},#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_4_S1_IMMEDIATE_DSP_IMM_BIT5_ADDSUB, "dsp-madd.4-s1-immediate-dsp-imm-bit5-addsub", "madd.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.4${dsp-c} ${dsp-destA},(${s1-An},${s1-r}),#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_4_S1_INDIRECT_WITH_INDEX_4_DSP_IMM_BIT5_ADDSUB, "dsp-madd.4-s1-indirect-with-index-4-dsp-imm-bit5-addsub", "madd.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.4${dsp-c} ${dsp-destA},${s1-imm7-4}(${s1-An}),#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_4_S1_INDIRECT_WITH_OFFSET_4_DSP_IMM_BIT5_ADDSUB, "dsp-madd.4-s1-indirect-with-offset-4-dsp-imm-bit5-addsub", "madd.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.4${dsp-c} ${dsp-destA},(${s1-An}),#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_4_S1_INDIRECT_4_DSP_IMM_BIT5_ADDSUB, "dsp-madd.4-s1-indirect-4-dsp-imm-bit5-addsub", "madd.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.4${dsp-c} ${dsp-destA},(${s1-An})${s1-i4-4}++,#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_4_S1_INDIRECT_WITH_POST_INCREMENT_4_DSP_IMM_BIT5_ADDSUB, "dsp-madd.4-s1-indirect-with-post-increment-4-dsp-imm-bit5-addsub", "madd.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* madd.4${dsp-c} ${dsp-destA},${s1-i4-4}(${s1-An})++,#${bit5-addsub} */
  {
    UBICOM32_INSN_DSP_MADD_4_S1_INDIRECT_WITH_PRE_INCREMENT_4_DSP_IMM_BIT5_ADDSUB, "dsp-madd.4-s1-indirect-with-pre-increment-4-dsp-imm-bit5-addsub", "madd.4", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msuf${dsp-c}${dsp-t} ${dsp-destA},${s1-direct-addr},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MSUF_S1_DIRECT_DSP_SRC2_DATA_REG, "dsp-msuf-s1-direct-dsp-src2-data-reg", "msuf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msuf${dsp-c}${dsp-t} ${dsp-destA},#${s1-imm8},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MSUF_S1_IMMEDIATE_DSP_SRC2_DATA_REG, "dsp-msuf-s1-immediate-dsp-src2-data-reg", "msuf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msuf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MSUF_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_DATA_REG, "dsp-msuf-s1-indirect-with-index-2-dsp-src2-data-reg", "msuf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msuf${dsp-c}${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MSUF_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_DATA_REG, "dsp-msuf-s1-indirect-with-offset-2-dsp-src2-data-reg", "msuf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msuf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MSUF_S1_INDIRECT_2_DSP_SRC2_DATA_REG, "dsp-msuf-s1-indirect-2-dsp-src2-data-reg", "msuf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msuf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MSUF_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_DATA_REG, "dsp-msuf-s1-indirect-with-post-increment-2-dsp-src2-data-reg", "msuf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msuf${dsp-c}${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MSUF_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_DATA_REG, "dsp-msuf-s1-indirect-with-pre-increment-2-dsp-src2-data-reg", "msuf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msuf${dsp-c}${dsp-t} ${dsp-destA},${s1-direct-addr},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MSUF_S1_DIRECT_DSP_SRC2_REG_ACC_REG_MUL, "dsp-msuf-s1-direct-dsp-src2-reg-acc-reg-mul", "msuf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msuf${dsp-c}${dsp-t} ${dsp-destA},#${s1-imm8},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MSUF_S1_IMMEDIATE_DSP_SRC2_REG_ACC_REG_MUL, "dsp-msuf-s1-immediate-dsp-src2-reg-acc-reg-mul", "msuf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msuf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MSUF_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-msuf-s1-indirect-with-index-2-dsp-src2-reg-acc-reg-mul", "msuf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msuf${dsp-c}${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MSUF_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-msuf-s1-indirect-with-offset-2-dsp-src2-reg-acc-reg-mul", "msuf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msuf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MSUF_S1_INDIRECT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-msuf-s1-indirect-2-dsp-src2-reg-acc-reg-mul", "msuf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msuf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MSUF_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-msuf-s1-indirect-with-post-increment-2-dsp-src2-reg-acc-reg-mul", "msuf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msuf${dsp-c}${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MSUF_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-msuf-s1-indirect-with-pre-increment-2-dsp-src2-reg-acc-reg-mul", "msuf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msuf${dsp-c}${dsp-t} ${dsp-destA},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_DSP_MSUF_S1_DIRECT_DSP_IMM_BIT5, "dsp-msuf-s1-direct-dsp-imm-bit5", "msuf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msuf${dsp-c}${dsp-t} ${dsp-destA},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_DSP_MSUF_S1_IMMEDIATE_DSP_IMM_BIT5, "dsp-msuf-s1-immediate-dsp-imm-bit5", "msuf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msuf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MSUF_S1_INDIRECT_WITH_INDEX_2_DSP_IMM_BIT5, "dsp-msuf-s1-indirect-with-index-2-dsp-imm-bit5", "msuf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msuf${dsp-c}${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MSUF_S1_INDIRECT_WITH_OFFSET_2_DSP_IMM_BIT5, "dsp-msuf-s1-indirect-with-offset-2-dsp-imm-bit5", "msuf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msuf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MSUF_S1_INDIRECT_2_DSP_IMM_BIT5, "dsp-msuf-s1-indirect-2-dsp-imm-bit5", "msuf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msuf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MSUF_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_IMM_BIT5, "dsp-msuf-s1-indirect-with-post-increment-2-dsp-imm-bit5", "msuf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* msuf${dsp-c}${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MSUF_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_IMM_BIT5, "dsp-msuf-s1-indirect-with-pre-increment-2-dsp-imm-bit5", "msuf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macus${dsp-c}${dsp-t} ${dsp-destA},${s1-direct-addr},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACUS_S1_DIRECT_DSP_SRC2_DATA_REG, "dsp-macus-s1-direct-dsp-src2-data-reg", "macus", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macus${dsp-c}${dsp-t} ${dsp-destA},#${s1-imm8},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACUS_S1_IMMEDIATE_DSP_SRC2_DATA_REG, "dsp-macus-s1-immediate-dsp-src2-data-reg", "macus", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macus${dsp-c}${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACUS_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_DATA_REG, "dsp-macus-s1-indirect-with-index-2-dsp-src2-data-reg", "macus", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macus${dsp-c}${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACUS_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_DATA_REG, "dsp-macus-s1-indirect-with-offset-2-dsp-src2-data-reg", "macus", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macus${dsp-c}${dsp-t} ${dsp-destA},(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACUS_S1_INDIRECT_2_DSP_SRC2_DATA_REG, "dsp-macus-s1-indirect-2-dsp-src2-data-reg", "macus", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macus${dsp-c}${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACUS_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_DATA_REG, "dsp-macus-s1-indirect-with-post-increment-2-dsp-src2-data-reg", "macus", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macus${dsp-c}${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACUS_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_DATA_REG, "dsp-macus-s1-indirect-with-pre-increment-2-dsp-src2-data-reg", "macus", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macus${dsp-c}${dsp-t} ${dsp-destA},${s1-direct-addr},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACUS_S1_DIRECT_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macus-s1-direct-dsp-src2-reg-acc-reg-mul", "macus", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macus${dsp-c}${dsp-t} ${dsp-destA},#${s1-imm8},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACUS_S1_IMMEDIATE_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macus-s1-immediate-dsp-src2-reg-acc-reg-mul", "macus", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macus${dsp-c}${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACUS_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macus-s1-indirect-with-index-2-dsp-src2-reg-acc-reg-mul", "macus", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macus${dsp-c}${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACUS_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macus-s1-indirect-with-offset-2-dsp-src2-reg-acc-reg-mul", "macus", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macus${dsp-c}${dsp-t} ${dsp-destA},(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACUS_S1_INDIRECT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macus-s1-indirect-2-dsp-src2-reg-acc-reg-mul", "macus", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macus${dsp-c}${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACUS_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macus-s1-indirect-with-post-increment-2-dsp-src2-reg-acc-reg-mul", "macus", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macus${dsp-c}${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACUS_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macus-s1-indirect-with-pre-increment-2-dsp-src2-reg-acc-reg-mul", "macus", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macus${dsp-c}${dsp-t} ${dsp-destA},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_DSP_MACUS_S1_DIRECT_DSP_IMM_BIT5, "dsp-macus-s1-direct-dsp-imm-bit5", "macus", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macus${dsp-c}${dsp-t} ${dsp-destA},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_DSP_MACUS_S1_IMMEDIATE_DSP_IMM_BIT5, "dsp-macus-s1-immediate-dsp-imm-bit5", "macus", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macus${dsp-c}${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MACUS_S1_INDIRECT_WITH_INDEX_2_DSP_IMM_BIT5, "dsp-macus-s1-indirect-with-index-2-dsp-imm-bit5", "macus", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macus${dsp-c}${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MACUS_S1_INDIRECT_WITH_OFFSET_2_DSP_IMM_BIT5, "dsp-macus-s1-indirect-with-offset-2-dsp-imm-bit5", "macus", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macus${dsp-c}${dsp-t} ${dsp-destA},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MACUS_S1_INDIRECT_2_DSP_IMM_BIT5, "dsp-macus-s1-indirect-2-dsp-imm-bit5", "macus", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macus${dsp-c}${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MACUS_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_IMM_BIT5, "dsp-macus-s1-indirect-with-post-increment-2-dsp-imm-bit5", "macus", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macus${dsp-c}${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MACUS_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_IMM_BIT5, "dsp-macus-s1-indirect-with-pre-increment-2-dsp-imm-bit5", "macus", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macf${dsp-c}${dsp-t} ${dsp-destA},${s1-direct-addr},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACF_S1_DIRECT_DSP_SRC2_DATA_REG, "dsp-macf-s1-direct-dsp-src2-data-reg", "macf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macf${dsp-c}${dsp-t} ${dsp-destA},#${s1-imm8},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACF_S1_IMMEDIATE_DSP_SRC2_DATA_REG, "dsp-macf-s1-immediate-dsp-src2-data-reg", "macf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACF_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_DATA_REG, "dsp-macf-s1-indirect-with-index-2-dsp-src2-data-reg", "macf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macf${dsp-c}${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACF_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_DATA_REG, "dsp-macf-s1-indirect-with-offset-2-dsp-src2-data-reg", "macf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACF_S1_INDIRECT_2_DSP_SRC2_DATA_REG, "dsp-macf-s1-indirect-2-dsp-src2-data-reg", "macf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACF_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_DATA_REG, "dsp-macf-s1-indirect-with-post-increment-2-dsp-src2-data-reg", "macf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macf${dsp-c}${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACF_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_DATA_REG, "dsp-macf-s1-indirect-with-pre-increment-2-dsp-src2-data-reg", "macf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macf${dsp-c}${dsp-t} ${dsp-destA},${s1-direct-addr},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACF_S1_DIRECT_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macf-s1-direct-dsp-src2-reg-acc-reg-mul", "macf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macf${dsp-c}${dsp-t} ${dsp-destA},#${s1-imm8},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACF_S1_IMMEDIATE_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macf-s1-immediate-dsp-src2-reg-acc-reg-mul", "macf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACF_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macf-s1-indirect-with-index-2-dsp-src2-reg-acc-reg-mul", "macf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macf${dsp-c}${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACF_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macf-s1-indirect-with-offset-2-dsp-src2-reg-acc-reg-mul", "macf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACF_S1_INDIRECT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macf-s1-indirect-2-dsp-src2-reg-acc-reg-mul", "macf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACF_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macf-s1-indirect-with-post-increment-2-dsp-src2-reg-acc-reg-mul", "macf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macf${dsp-c}${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACF_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macf-s1-indirect-with-pre-increment-2-dsp-src2-reg-acc-reg-mul", "macf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macf${dsp-c}${dsp-t} ${dsp-destA},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_DSP_MACF_S1_DIRECT_DSP_IMM_BIT5, "dsp-macf-s1-direct-dsp-imm-bit5", "macf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macf${dsp-c}${dsp-t} ${dsp-destA},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_DSP_MACF_S1_IMMEDIATE_DSP_IMM_BIT5, "dsp-macf-s1-immediate-dsp-imm-bit5", "macf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MACF_S1_INDIRECT_WITH_INDEX_2_DSP_IMM_BIT5, "dsp-macf-s1-indirect-with-index-2-dsp-imm-bit5", "macf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macf${dsp-c}${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MACF_S1_INDIRECT_WITH_OFFSET_2_DSP_IMM_BIT5, "dsp-macf-s1-indirect-with-offset-2-dsp-imm-bit5", "macf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MACF_S1_INDIRECT_2_DSP_IMM_BIT5, "dsp-macf-s1-indirect-2-dsp-imm-bit5", "macf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MACF_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_IMM_BIT5, "dsp-macf-s1-indirect-with-post-increment-2-dsp-imm-bit5", "macf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macf${dsp-c}${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MACF_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_IMM_BIT5, "dsp-macf-s1-indirect-with-pre-increment-2-dsp-imm-bit5", "macf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulf${dsp-c}${dsp-t} ${dsp-destA},${s1-direct-addr},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULF_S1_DIRECT_DSP_SRC2_DATA_REG, "dsp-mulf-s1-direct-dsp-src2-data-reg", "mulf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulf${dsp-c}${dsp-t} ${dsp-destA},#${s1-imm8},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULF_S1_IMMEDIATE_DSP_SRC2_DATA_REG, "dsp-mulf-s1-immediate-dsp-src2-data-reg", "mulf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULF_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_DATA_REG, "dsp-mulf-s1-indirect-with-index-2-dsp-src2-data-reg", "mulf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulf${dsp-c}${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULF_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_DATA_REG, "dsp-mulf-s1-indirect-with-offset-2-dsp-src2-data-reg", "mulf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULF_S1_INDIRECT_2_DSP_SRC2_DATA_REG, "dsp-mulf-s1-indirect-2-dsp-src2-data-reg", "mulf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULF_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_DATA_REG, "dsp-mulf-s1-indirect-with-post-increment-2-dsp-src2-data-reg", "mulf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulf${dsp-c}${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULF_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_DATA_REG, "dsp-mulf-s1-indirect-with-pre-increment-2-dsp-src2-data-reg", "mulf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulf${dsp-c}${dsp-t} ${dsp-destA},${s1-direct-addr},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULF_S1_DIRECT_DSP_SRC2_REG_ACC_REG_MUL, "dsp-mulf-s1-direct-dsp-src2-reg-acc-reg-mul", "mulf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulf${dsp-c}${dsp-t} ${dsp-destA},#${s1-imm8},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULF_S1_IMMEDIATE_DSP_SRC2_REG_ACC_REG_MUL, "dsp-mulf-s1-immediate-dsp-src2-reg-acc-reg-mul", "mulf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULF_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-mulf-s1-indirect-with-index-2-dsp-src2-reg-acc-reg-mul", "mulf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulf${dsp-c}${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULF_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-mulf-s1-indirect-with-offset-2-dsp-src2-reg-acc-reg-mul", "mulf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULF_S1_INDIRECT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-mulf-s1-indirect-2-dsp-src2-reg-acc-reg-mul", "mulf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULF_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-mulf-s1-indirect-with-post-increment-2-dsp-src2-reg-acc-reg-mul", "mulf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulf${dsp-c}${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULF_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-mulf-s1-indirect-with-pre-increment-2-dsp-src2-reg-acc-reg-mul", "mulf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulf${dsp-c}${dsp-t} ${dsp-destA},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_DSP_MULF_S1_DIRECT_DSP_IMM_BIT5, "dsp-mulf-s1-direct-dsp-imm-bit5", "mulf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulf${dsp-c}${dsp-t} ${dsp-destA},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_DSP_MULF_S1_IMMEDIATE_DSP_IMM_BIT5, "dsp-mulf-s1-immediate-dsp-imm-bit5", "mulf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MULF_S1_INDIRECT_WITH_INDEX_2_DSP_IMM_BIT5, "dsp-mulf-s1-indirect-with-index-2-dsp-imm-bit5", "mulf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulf${dsp-c}${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MULF_S1_INDIRECT_WITH_OFFSET_2_DSP_IMM_BIT5, "dsp-mulf-s1-indirect-with-offset-2-dsp-imm-bit5", "mulf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MULF_S1_INDIRECT_2_DSP_IMM_BIT5, "dsp-mulf-s1-indirect-2-dsp-imm-bit5", "mulf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulf${dsp-c}${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MULF_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_IMM_BIT5, "dsp-mulf-s1-indirect-with-post-increment-2-dsp-imm-bit5", "mulf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulf${dsp-c}${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MULF_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_IMM_BIT5, "dsp-mulf-s1-indirect-with-pre-increment-2-dsp-imm-bit5", "mulf", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macu${dsp-c}${dsp-t} ${dsp-destA},${s1-direct-addr},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACU_S1_DIRECT_DSP_SRC2_DATA_REG, "dsp-macu-s1-direct-dsp-src2-data-reg", "macu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macu${dsp-c}${dsp-t} ${dsp-destA},#${s1-imm8},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACU_S1_IMMEDIATE_DSP_SRC2_DATA_REG, "dsp-macu-s1-immediate-dsp-src2-data-reg", "macu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macu${dsp-c}${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACU_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_DATA_REG, "dsp-macu-s1-indirect-with-index-2-dsp-src2-data-reg", "macu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macu${dsp-c}${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACU_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_DATA_REG, "dsp-macu-s1-indirect-with-offset-2-dsp-src2-data-reg", "macu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macu${dsp-c}${dsp-t} ${dsp-destA},(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACU_S1_INDIRECT_2_DSP_SRC2_DATA_REG, "dsp-macu-s1-indirect-2-dsp-src2-data-reg", "macu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macu${dsp-c}${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACU_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_DATA_REG, "dsp-macu-s1-indirect-with-post-increment-2-dsp-src2-data-reg", "macu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macu${dsp-c}${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACU_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_DATA_REG, "dsp-macu-s1-indirect-with-pre-increment-2-dsp-src2-data-reg", "macu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macu${dsp-c}${dsp-t} ${dsp-destA},${s1-direct-addr},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACU_S1_DIRECT_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macu-s1-direct-dsp-src2-reg-acc-reg-mul", "macu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macu${dsp-c}${dsp-t} ${dsp-destA},#${s1-imm8},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACU_S1_IMMEDIATE_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macu-s1-immediate-dsp-src2-reg-acc-reg-mul", "macu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macu${dsp-c}${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACU_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macu-s1-indirect-with-index-2-dsp-src2-reg-acc-reg-mul", "macu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macu${dsp-c}${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACU_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macu-s1-indirect-with-offset-2-dsp-src2-reg-acc-reg-mul", "macu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macu${dsp-c}${dsp-t} ${dsp-destA},(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACU_S1_INDIRECT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macu-s1-indirect-2-dsp-src2-reg-acc-reg-mul", "macu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macu${dsp-c}${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACU_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macu-s1-indirect-with-post-increment-2-dsp-src2-reg-acc-reg-mul", "macu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macu${dsp-c}${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACU_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macu-s1-indirect-with-pre-increment-2-dsp-src2-reg-acc-reg-mul", "macu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macu${dsp-c}${dsp-t} ${dsp-destA},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_DSP_MACU_S1_DIRECT_DSP_IMM_BIT5, "dsp-macu-s1-direct-dsp-imm-bit5", "macu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macu${dsp-c}${dsp-t} ${dsp-destA},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_DSP_MACU_S1_IMMEDIATE_DSP_IMM_BIT5, "dsp-macu-s1-immediate-dsp-imm-bit5", "macu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macu${dsp-c}${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MACU_S1_INDIRECT_WITH_INDEX_2_DSP_IMM_BIT5, "dsp-macu-s1-indirect-with-index-2-dsp-imm-bit5", "macu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macu${dsp-c}${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MACU_S1_INDIRECT_WITH_OFFSET_2_DSP_IMM_BIT5, "dsp-macu-s1-indirect-with-offset-2-dsp-imm-bit5", "macu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macu${dsp-c}${dsp-t} ${dsp-destA},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MACU_S1_INDIRECT_2_DSP_IMM_BIT5, "dsp-macu-s1-indirect-2-dsp-imm-bit5", "macu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macu${dsp-c}${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MACU_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_IMM_BIT5, "dsp-macu-s1-indirect-with-post-increment-2-dsp-imm-bit5", "macu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macu${dsp-c}${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MACU_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_IMM_BIT5, "dsp-macu-s1-indirect-with-pre-increment-2-dsp-imm-bit5", "macu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macu.4 ${dsp-destA},${s1-direct-addr},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACU_4_S1_DIRECT_DSP_SRC2_DATA_REG, "dsp-macu.4-s1-direct-dsp-src2-data-reg", "macu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macu.4 ${dsp-destA},#${s1-imm8},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACU_4_S1_IMMEDIATE_DSP_SRC2_DATA_REG, "dsp-macu.4-s1-immediate-dsp-src2-data-reg", "macu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macu.4 ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACU_4_S1_INDIRECT_WITH_INDEX_4_DSP_SRC2_DATA_REG, "dsp-macu.4-s1-indirect-with-index-4-dsp-src2-data-reg", "macu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macu.4 ${dsp-destA},${s1-imm7-4}(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACU_4_S1_INDIRECT_WITH_OFFSET_4_DSP_SRC2_DATA_REG, "dsp-macu.4-s1-indirect-with-offset-4-dsp-src2-data-reg", "macu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macu.4 ${dsp-destA},(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACU_4_S1_INDIRECT_4_DSP_SRC2_DATA_REG, "dsp-macu.4-s1-indirect-4-dsp-src2-data-reg", "macu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macu.4 ${dsp-destA},(${s1-An})${s1-i4-4}++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACU_4_S1_INDIRECT_WITH_POST_INCREMENT_4_DSP_SRC2_DATA_REG, "dsp-macu.4-s1-indirect-with-post-increment-4-dsp-src2-data-reg", "macu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macu.4 ${dsp-destA},${s1-i4-4}(${s1-An})++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACU_4_S1_INDIRECT_WITH_PRE_INCREMENT_4_DSP_SRC2_DATA_REG, "dsp-macu.4-s1-indirect-with-pre-increment-4-dsp-src2-data-reg", "macu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macu.4 ${dsp-destA},${s1-direct-addr},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACU_4_S1_DIRECT_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macu.4-s1-direct-dsp-src2-reg-acc-reg-mul", "macu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macu.4 ${dsp-destA},#${s1-imm8},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACU_4_S1_IMMEDIATE_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macu.4-s1-immediate-dsp-src2-reg-acc-reg-mul", "macu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macu.4 ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACU_4_S1_INDIRECT_WITH_INDEX_4_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macu.4-s1-indirect-with-index-4-dsp-src2-reg-acc-reg-mul", "macu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macu.4 ${dsp-destA},${s1-imm7-4}(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACU_4_S1_INDIRECT_WITH_OFFSET_4_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macu.4-s1-indirect-with-offset-4-dsp-src2-reg-acc-reg-mul", "macu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macu.4 ${dsp-destA},(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACU_4_S1_INDIRECT_4_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macu.4-s1-indirect-4-dsp-src2-reg-acc-reg-mul", "macu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macu.4 ${dsp-destA},(${s1-An})${s1-i4-4}++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACU_4_S1_INDIRECT_WITH_POST_INCREMENT_4_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macu.4-s1-indirect-with-post-increment-4-dsp-src2-reg-acc-reg-mul", "macu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macu.4 ${dsp-destA},${s1-i4-4}(${s1-An})++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACU_4_S1_INDIRECT_WITH_PRE_INCREMENT_4_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macu.4-s1-indirect-with-pre-increment-4-dsp-src2-reg-acc-reg-mul", "macu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macu.4 ${dsp-destA},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_DSP_MACU_4_S1_DIRECT_DSP_IMM_BIT5, "dsp-macu.4-s1-direct-dsp-imm-bit5", "macu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macu.4 ${dsp-destA},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_DSP_MACU_4_S1_IMMEDIATE_DSP_IMM_BIT5, "dsp-macu.4-s1-immediate-dsp-imm-bit5", "macu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macu.4 ${dsp-destA},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MACU_4_S1_INDIRECT_WITH_INDEX_4_DSP_IMM_BIT5, "dsp-macu.4-s1-indirect-with-index-4-dsp-imm-bit5", "macu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macu.4 ${dsp-destA},${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MACU_4_S1_INDIRECT_WITH_OFFSET_4_DSP_IMM_BIT5, "dsp-macu.4-s1-indirect-with-offset-4-dsp-imm-bit5", "macu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macu.4 ${dsp-destA},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MACU_4_S1_INDIRECT_4_DSP_IMM_BIT5, "dsp-macu.4-s1-indirect-4-dsp-imm-bit5", "macu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macu.4 ${dsp-destA},(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MACU_4_S1_INDIRECT_WITH_POST_INCREMENT_4_DSP_IMM_BIT5, "dsp-macu.4-s1-indirect-with-post-increment-4-dsp-imm-bit5", "macu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macu.4 ${dsp-destA},${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MACU_4_S1_INDIRECT_WITH_PRE_INCREMENT_4_DSP_IMM_BIT5, "dsp-macu.4-s1-indirect-with-pre-increment-4-dsp-imm-bit5", "macu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* mulu.4 ${dsp-destA},${s1-direct-addr},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULU_4_S1_DIRECT_DSP_SRC2_DATA_REG, "dsp-mulu.4-s1-direct-dsp-src2-data-reg", "mulu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu.4 ${dsp-destA},#${s1-imm8},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULU_4_S1_IMMEDIATE_DSP_SRC2_DATA_REG, "dsp-mulu.4-s1-immediate-dsp-src2-data-reg", "mulu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu.4 ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULU_4_S1_INDIRECT_WITH_INDEX_4_DSP_SRC2_DATA_REG, "dsp-mulu.4-s1-indirect-with-index-4-dsp-src2-data-reg", "mulu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu.4 ${dsp-destA},${s1-imm7-4}(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULU_4_S1_INDIRECT_WITH_OFFSET_4_DSP_SRC2_DATA_REG, "dsp-mulu.4-s1-indirect-with-offset-4-dsp-src2-data-reg", "mulu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu.4 ${dsp-destA},(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULU_4_S1_INDIRECT_4_DSP_SRC2_DATA_REG, "dsp-mulu.4-s1-indirect-4-dsp-src2-data-reg", "mulu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu.4 ${dsp-destA},(${s1-An})${s1-i4-4}++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULU_4_S1_INDIRECT_WITH_POST_INCREMENT_4_DSP_SRC2_DATA_REG, "dsp-mulu.4-s1-indirect-with-post-increment-4-dsp-src2-data-reg", "mulu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu.4 ${dsp-destA},${s1-i4-4}(${s1-An})++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULU_4_S1_INDIRECT_WITH_PRE_INCREMENT_4_DSP_SRC2_DATA_REG, "dsp-mulu.4-s1-indirect-with-pre-increment-4-dsp-src2-data-reg", "mulu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu.4 ${dsp-destA},${s1-direct-addr},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULU_4_S1_DIRECT_DSP_SRC2_REG_ACC_REG_MUL, "dsp-mulu.4-s1-direct-dsp-src2-reg-acc-reg-mul", "mulu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu.4 ${dsp-destA},#${s1-imm8},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULU_4_S1_IMMEDIATE_DSP_SRC2_REG_ACC_REG_MUL, "dsp-mulu.4-s1-immediate-dsp-src2-reg-acc-reg-mul", "mulu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu.4 ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULU_4_S1_INDIRECT_WITH_INDEX_4_DSP_SRC2_REG_ACC_REG_MUL, "dsp-mulu.4-s1-indirect-with-index-4-dsp-src2-reg-acc-reg-mul", "mulu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu.4 ${dsp-destA},${s1-imm7-4}(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULU_4_S1_INDIRECT_WITH_OFFSET_4_DSP_SRC2_REG_ACC_REG_MUL, "dsp-mulu.4-s1-indirect-with-offset-4-dsp-src2-reg-acc-reg-mul", "mulu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu.4 ${dsp-destA},(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULU_4_S1_INDIRECT_4_DSP_SRC2_REG_ACC_REG_MUL, "dsp-mulu.4-s1-indirect-4-dsp-src2-reg-acc-reg-mul", "mulu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu.4 ${dsp-destA},(${s1-An})${s1-i4-4}++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULU_4_S1_INDIRECT_WITH_POST_INCREMENT_4_DSP_SRC2_REG_ACC_REG_MUL, "dsp-mulu.4-s1-indirect-with-post-increment-4-dsp-src2-reg-acc-reg-mul", "mulu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu.4 ${dsp-destA},${s1-i4-4}(${s1-An})++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULU_4_S1_INDIRECT_WITH_PRE_INCREMENT_4_DSP_SRC2_REG_ACC_REG_MUL, "dsp-mulu.4-s1-indirect-with-pre-increment-4-dsp-src2-reg-acc-reg-mul", "mulu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu.4 ${dsp-destA},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_DSP_MULU_4_S1_DIRECT_DSP_IMM_BIT5, "dsp-mulu.4-s1-direct-dsp-imm-bit5", "mulu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu.4 ${dsp-destA},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_DSP_MULU_4_S1_IMMEDIATE_DSP_IMM_BIT5, "dsp-mulu.4-s1-immediate-dsp-imm-bit5", "mulu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu.4 ${dsp-destA},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MULU_4_S1_INDIRECT_WITH_INDEX_4_DSP_IMM_BIT5, "dsp-mulu.4-s1-indirect-with-index-4-dsp-imm-bit5", "mulu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu.4 ${dsp-destA},${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MULU_4_S1_INDIRECT_WITH_OFFSET_4_DSP_IMM_BIT5, "dsp-mulu.4-s1-indirect-with-offset-4-dsp-imm-bit5", "mulu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu.4 ${dsp-destA},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MULU_4_S1_INDIRECT_4_DSP_IMM_BIT5, "dsp-mulu.4-s1-indirect-4-dsp-imm-bit5", "mulu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu.4 ${dsp-destA},(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MULU_4_S1_INDIRECT_WITH_POST_INCREMENT_4_DSP_IMM_BIT5, "dsp-mulu.4-s1-indirect-with-post-increment-4-dsp-imm-bit5", "mulu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu.4 ${dsp-destA},${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MULU_4_S1_INDIRECT_WITH_PRE_INCREMENT_4_DSP_IMM_BIT5, "dsp-mulu.4-s1-indirect-with-pre-increment-4-dsp-imm-bit5", "mulu.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu${dsp-t} ${dsp-destA},${s1-direct-addr},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULU_S1_DIRECT_DSP_SRC2_DATA_REG, "dsp-mulu-s1-direct-dsp-src2-data-reg", "mulu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu${dsp-t} ${dsp-destA},#${s1-imm8},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULU_S1_IMMEDIATE_DSP_SRC2_DATA_REG, "dsp-mulu-s1-immediate-dsp-src2-data-reg", "mulu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULU_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_DATA_REG, "dsp-mulu-s1-indirect-with-index-2-dsp-src2-data-reg", "mulu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULU_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_DATA_REG, "dsp-mulu-s1-indirect-with-offset-2-dsp-src2-data-reg", "mulu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu${dsp-t} ${dsp-destA},(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULU_S1_INDIRECT_2_DSP_SRC2_DATA_REG, "dsp-mulu-s1-indirect-2-dsp-src2-data-reg", "mulu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULU_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_DATA_REG, "dsp-mulu-s1-indirect-with-post-increment-2-dsp-src2-data-reg", "mulu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULU_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_DATA_REG, "dsp-mulu-s1-indirect-with-pre-increment-2-dsp-src2-data-reg", "mulu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu${dsp-t} ${dsp-destA},${s1-direct-addr},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULU_S1_DIRECT_DSP_SRC2_REG_ACC_REG_MUL, "dsp-mulu-s1-direct-dsp-src2-reg-acc-reg-mul", "mulu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu${dsp-t} ${dsp-destA},#${s1-imm8},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULU_S1_IMMEDIATE_DSP_SRC2_REG_ACC_REG_MUL, "dsp-mulu-s1-immediate-dsp-src2-reg-acc-reg-mul", "mulu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULU_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-mulu-s1-indirect-with-index-2-dsp-src2-reg-acc-reg-mul", "mulu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULU_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-mulu-s1-indirect-with-offset-2-dsp-src2-reg-acc-reg-mul", "mulu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu${dsp-t} ${dsp-destA},(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULU_S1_INDIRECT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-mulu-s1-indirect-2-dsp-src2-reg-acc-reg-mul", "mulu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULU_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-mulu-s1-indirect-with-post-increment-2-dsp-src2-reg-acc-reg-mul", "mulu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULU_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-mulu-s1-indirect-with-pre-increment-2-dsp-src2-reg-acc-reg-mul", "mulu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu${dsp-t} ${dsp-destA},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_DSP_MULU_S1_DIRECT_DSP_IMM_BIT5, "dsp-mulu-s1-direct-dsp-imm-bit5", "mulu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu${dsp-t} ${dsp-destA},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_DSP_MULU_S1_IMMEDIATE_DSP_IMM_BIT5, "dsp-mulu-s1-immediate-dsp-imm-bit5", "mulu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MULU_S1_INDIRECT_WITH_INDEX_2_DSP_IMM_BIT5, "dsp-mulu-s1-indirect-with-index-2-dsp-imm-bit5", "mulu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MULU_S1_INDIRECT_WITH_OFFSET_2_DSP_IMM_BIT5, "dsp-mulu-s1-indirect-with-offset-2-dsp-imm-bit5", "mulu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu${dsp-t} ${dsp-destA},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MULU_S1_INDIRECT_2_DSP_IMM_BIT5, "dsp-mulu-s1-indirect-2-dsp-imm-bit5", "mulu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MULU_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_IMM_BIT5, "dsp-mulu-s1-indirect-with-post-increment-2-dsp-imm-bit5", "mulu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* mulu${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MULU_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_IMM_BIT5, "dsp-mulu-s1-indirect-with-pre-increment-2-dsp-imm-bit5", "mulu", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macs${dsp-c}${dsp-t} ${dsp-destA},${s1-direct-addr},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACS_S1_DIRECT_DSP_SRC2_DATA_REG, "dsp-macs-s1-direct-dsp-src2-data-reg", "macs", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macs${dsp-c}${dsp-t} ${dsp-destA},#${s1-imm8},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACS_S1_IMMEDIATE_DSP_SRC2_DATA_REG, "dsp-macs-s1-immediate-dsp-src2-data-reg", "macs", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macs${dsp-c}${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACS_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_DATA_REG, "dsp-macs-s1-indirect-with-index-2-dsp-src2-data-reg", "macs", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macs${dsp-c}${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACS_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_DATA_REG, "dsp-macs-s1-indirect-with-offset-2-dsp-src2-data-reg", "macs", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macs${dsp-c}${dsp-t} ${dsp-destA},(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACS_S1_INDIRECT_2_DSP_SRC2_DATA_REG, "dsp-macs-s1-indirect-2-dsp-src2-data-reg", "macs", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macs${dsp-c}${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACS_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_DATA_REG, "dsp-macs-s1-indirect-with-post-increment-2-dsp-src2-data-reg", "macs", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macs${dsp-c}${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACS_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_DATA_REG, "dsp-macs-s1-indirect-with-pre-increment-2-dsp-src2-data-reg", "macs", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macs${dsp-c}${dsp-t} ${dsp-destA},${s1-direct-addr},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACS_S1_DIRECT_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macs-s1-direct-dsp-src2-reg-acc-reg-mul", "macs", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macs${dsp-c}${dsp-t} ${dsp-destA},#${s1-imm8},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACS_S1_IMMEDIATE_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macs-s1-immediate-dsp-src2-reg-acc-reg-mul", "macs", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macs${dsp-c}${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACS_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macs-s1-indirect-with-index-2-dsp-src2-reg-acc-reg-mul", "macs", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macs${dsp-c}${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACS_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macs-s1-indirect-with-offset-2-dsp-src2-reg-acc-reg-mul", "macs", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macs${dsp-c}${dsp-t} ${dsp-destA},(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACS_S1_INDIRECT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macs-s1-indirect-2-dsp-src2-reg-acc-reg-mul", "macs", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macs${dsp-c}${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACS_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macs-s1-indirect-with-post-increment-2-dsp-src2-reg-acc-reg-mul", "macs", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macs${dsp-c}${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACS_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macs-s1-indirect-with-pre-increment-2-dsp-src2-reg-acc-reg-mul", "macs", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macs${dsp-c}${dsp-t} ${dsp-destA},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_DSP_MACS_S1_DIRECT_DSP_IMM_BIT5, "dsp-macs-s1-direct-dsp-imm-bit5", "macs", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macs${dsp-c}${dsp-t} ${dsp-destA},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_DSP_MACS_S1_IMMEDIATE_DSP_IMM_BIT5, "dsp-macs-s1-immediate-dsp-imm-bit5", "macs", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macs${dsp-c}${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MACS_S1_INDIRECT_WITH_INDEX_2_DSP_IMM_BIT5, "dsp-macs-s1-indirect-with-index-2-dsp-imm-bit5", "macs", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macs${dsp-c}${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MACS_S1_INDIRECT_WITH_OFFSET_2_DSP_IMM_BIT5, "dsp-macs-s1-indirect-with-offset-2-dsp-imm-bit5", "macs", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macs${dsp-c}${dsp-t} ${dsp-destA},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MACS_S1_INDIRECT_2_DSP_IMM_BIT5, "dsp-macs-s1-indirect-2-dsp-imm-bit5", "macs", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macs${dsp-c}${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MACS_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_IMM_BIT5, "dsp-macs-s1-indirect-with-post-increment-2-dsp-imm-bit5", "macs", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macs${dsp-c}${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MACS_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_IMM_BIT5, "dsp-macs-s1-indirect-with-pre-increment-2-dsp-imm-bit5", "macs", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* macs.4 ${dsp-destA},${s1-direct-addr},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACS_4_S1_DIRECT_DSP_SRC2_DATA_REG, "dsp-macs.4-s1-direct-dsp-src2-data-reg", "macs.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macs.4 ${dsp-destA},#${s1-imm8},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACS_4_S1_IMMEDIATE_DSP_SRC2_DATA_REG, "dsp-macs.4-s1-immediate-dsp-src2-data-reg", "macs.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macs.4 ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACS_4_S1_INDIRECT_WITH_INDEX_4_DSP_SRC2_DATA_REG, "dsp-macs.4-s1-indirect-with-index-4-dsp-src2-data-reg", "macs.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macs.4 ${dsp-destA},${s1-imm7-4}(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACS_4_S1_INDIRECT_WITH_OFFSET_4_DSP_SRC2_DATA_REG, "dsp-macs.4-s1-indirect-with-offset-4-dsp-src2-data-reg", "macs.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macs.4 ${dsp-destA},(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACS_4_S1_INDIRECT_4_DSP_SRC2_DATA_REG, "dsp-macs.4-s1-indirect-4-dsp-src2-data-reg", "macs.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macs.4 ${dsp-destA},(${s1-An})${s1-i4-4}++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACS_4_S1_INDIRECT_WITH_POST_INCREMENT_4_DSP_SRC2_DATA_REG, "dsp-macs.4-s1-indirect-with-post-increment-4-dsp-src2-data-reg", "macs.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macs.4 ${dsp-destA},${s1-i4-4}(${s1-An})++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MACS_4_S1_INDIRECT_WITH_PRE_INCREMENT_4_DSP_SRC2_DATA_REG, "dsp-macs.4-s1-indirect-with-pre-increment-4-dsp-src2-data-reg", "macs.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macs.4 ${dsp-destA},${s1-direct-addr},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACS_4_S1_DIRECT_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macs.4-s1-direct-dsp-src2-reg-acc-reg-mul", "macs.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macs.4 ${dsp-destA},#${s1-imm8},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACS_4_S1_IMMEDIATE_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macs.4-s1-immediate-dsp-src2-reg-acc-reg-mul", "macs.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macs.4 ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACS_4_S1_INDIRECT_WITH_INDEX_4_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macs.4-s1-indirect-with-index-4-dsp-src2-reg-acc-reg-mul", "macs.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macs.4 ${dsp-destA},${s1-imm7-4}(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACS_4_S1_INDIRECT_WITH_OFFSET_4_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macs.4-s1-indirect-with-offset-4-dsp-src2-reg-acc-reg-mul", "macs.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macs.4 ${dsp-destA},(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACS_4_S1_INDIRECT_4_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macs.4-s1-indirect-4-dsp-src2-reg-acc-reg-mul", "macs.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macs.4 ${dsp-destA},(${s1-An})${s1-i4-4}++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACS_4_S1_INDIRECT_WITH_POST_INCREMENT_4_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macs.4-s1-indirect-with-post-increment-4-dsp-src2-reg-acc-reg-mul", "macs.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macs.4 ${dsp-destA},${s1-i4-4}(${s1-An})++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MACS_4_S1_INDIRECT_WITH_PRE_INCREMENT_4_DSP_SRC2_REG_ACC_REG_MUL, "dsp-macs.4-s1-indirect-with-pre-increment-4-dsp-src2-reg-acc-reg-mul", "macs.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macs.4 ${dsp-destA},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_DSP_MACS_4_S1_DIRECT_DSP_IMM_BIT5, "dsp-macs.4-s1-direct-dsp-imm-bit5", "macs.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macs.4 ${dsp-destA},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_DSP_MACS_4_S1_IMMEDIATE_DSP_IMM_BIT5, "dsp-macs.4-s1-immediate-dsp-imm-bit5", "macs.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macs.4 ${dsp-destA},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MACS_4_S1_INDIRECT_WITH_INDEX_4_DSP_IMM_BIT5, "dsp-macs.4-s1-indirect-with-index-4-dsp-imm-bit5", "macs.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macs.4 ${dsp-destA},${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MACS_4_S1_INDIRECT_WITH_OFFSET_4_DSP_IMM_BIT5, "dsp-macs.4-s1-indirect-with-offset-4-dsp-imm-bit5", "macs.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macs.4 ${dsp-destA},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MACS_4_S1_INDIRECT_4_DSP_IMM_BIT5, "dsp-macs.4-s1-indirect-4-dsp-imm-bit5", "macs.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macs.4 ${dsp-destA},(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MACS_4_S1_INDIRECT_WITH_POST_INCREMENT_4_DSP_IMM_BIT5, "dsp-macs.4-s1-indirect-with-post-increment-4-dsp-imm-bit5", "macs.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* macs.4 ${dsp-destA},${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MACS_4_S1_INDIRECT_WITH_PRE_INCREMENT_4_DSP_IMM_BIT5, "dsp-macs.4-s1-indirect-with-pre-increment-4-dsp-imm-bit5", "macs.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* muls.4 ${dsp-destA},${s1-direct-addr},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULS_4_S1_DIRECT_DSP_SRC2_DATA_REG, "dsp-muls.4-s1-direct-dsp-src2-data-reg", "muls.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls.4 ${dsp-destA},#${s1-imm8},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULS_4_S1_IMMEDIATE_DSP_SRC2_DATA_REG, "dsp-muls.4-s1-immediate-dsp-src2-data-reg", "muls.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls.4 ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULS_4_S1_INDIRECT_WITH_INDEX_4_DSP_SRC2_DATA_REG, "dsp-muls.4-s1-indirect-with-index-4-dsp-src2-data-reg", "muls.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls.4 ${dsp-destA},${s1-imm7-4}(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULS_4_S1_INDIRECT_WITH_OFFSET_4_DSP_SRC2_DATA_REG, "dsp-muls.4-s1-indirect-with-offset-4-dsp-src2-data-reg", "muls.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls.4 ${dsp-destA},(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULS_4_S1_INDIRECT_4_DSP_SRC2_DATA_REG, "dsp-muls.4-s1-indirect-4-dsp-src2-data-reg", "muls.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls.4 ${dsp-destA},(${s1-An})${s1-i4-4}++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULS_4_S1_INDIRECT_WITH_POST_INCREMENT_4_DSP_SRC2_DATA_REG, "dsp-muls.4-s1-indirect-with-post-increment-4-dsp-src2-data-reg", "muls.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls.4 ${dsp-destA},${s1-i4-4}(${s1-An})++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULS_4_S1_INDIRECT_WITH_PRE_INCREMENT_4_DSP_SRC2_DATA_REG, "dsp-muls.4-s1-indirect-with-pre-increment-4-dsp-src2-data-reg", "muls.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls.4 ${dsp-destA},${s1-direct-addr},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULS_4_S1_DIRECT_DSP_SRC2_REG_ACC_REG_MUL, "dsp-muls.4-s1-direct-dsp-src2-reg-acc-reg-mul", "muls.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls.4 ${dsp-destA},#${s1-imm8},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULS_4_S1_IMMEDIATE_DSP_SRC2_REG_ACC_REG_MUL, "dsp-muls.4-s1-immediate-dsp-src2-reg-acc-reg-mul", "muls.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls.4 ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULS_4_S1_INDIRECT_WITH_INDEX_4_DSP_SRC2_REG_ACC_REG_MUL, "dsp-muls.4-s1-indirect-with-index-4-dsp-src2-reg-acc-reg-mul", "muls.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls.4 ${dsp-destA},${s1-imm7-4}(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULS_4_S1_INDIRECT_WITH_OFFSET_4_DSP_SRC2_REG_ACC_REG_MUL, "dsp-muls.4-s1-indirect-with-offset-4-dsp-src2-reg-acc-reg-mul", "muls.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls.4 ${dsp-destA},(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULS_4_S1_INDIRECT_4_DSP_SRC2_REG_ACC_REG_MUL, "dsp-muls.4-s1-indirect-4-dsp-src2-reg-acc-reg-mul", "muls.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls.4 ${dsp-destA},(${s1-An})${s1-i4-4}++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULS_4_S1_INDIRECT_WITH_POST_INCREMENT_4_DSP_SRC2_REG_ACC_REG_MUL, "dsp-muls.4-s1-indirect-with-post-increment-4-dsp-src2-reg-acc-reg-mul", "muls.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls.4 ${dsp-destA},${s1-i4-4}(${s1-An})++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULS_4_S1_INDIRECT_WITH_PRE_INCREMENT_4_DSP_SRC2_REG_ACC_REG_MUL, "dsp-muls.4-s1-indirect-with-pre-increment-4-dsp-src2-reg-acc-reg-mul", "muls.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls.4 ${dsp-destA},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_DSP_MULS_4_S1_DIRECT_DSP_IMM_BIT5, "dsp-muls.4-s1-direct-dsp-imm-bit5", "muls.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls.4 ${dsp-destA},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_DSP_MULS_4_S1_IMMEDIATE_DSP_IMM_BIT5, "dsp-muls.4-s1-immediate-dsp-imm-bit5", "muls.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls.4 ${dsp-destA},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MULS_4_S1_INDIRECT_WITH_INDEX_4_DSP_IMM_BIT5, "dsp-muls.4-s1-indirect-with-index-4-dsp-imm-bit5", "muls.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls.4 ${dsp-destA},${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MULS_4_S1_INDIRECT_WITH_OFFSET_4_DSP_IMM_BIT5, "dsp-muls.4-s1-indirect-with-offset-4-dsp-imm-bit5", "muls.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls.4 ${dsp-destA},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MULS_4_S1_INDIRECT_4_DSP_IMM_BIT5, "dsp-muls.4-s1-indirect-4-dsp-imm-bit5", "muls.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls.4 ${dsp-destA},(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MULS_4_S1_INDIRECT_WITH_POST_INCREMENT_4_DSP_IMM_BIT5, "dsp-muls.4-s1-indirect-with-post-increment-4-dsp-imm-bit5", "muls.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls.4 ${dsp-destA},${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MULS_4_S1_INDIRECT_WITH_PRE_INCREMENT_4_DSP_IMM_BIT5, "dsp-muls.4-s1-indirect-with-pre-increment-4-dsp-imm-bit5", "muls.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls${dsp-t} ${dsp-destA},${s1-direct-addr},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULS_S1_DIRECT_DSP_SRC2_DATA_REG, "dsp-muls-s1-direct-dsp-src2-data-reg", "muls", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls${dsp-t} ${dsp-destA},#${s1-imm8},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULS_S1_IMMEDIATE_DSP_SRC2_DATA_REG, "dsp-muls-s1-immediate-dsp-src2-data-reg", "muls", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULS_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_DATA_REG, "dsp-muls-s1-indirect-with-index-2-dsp-src2-data-reg", "muls", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULS_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_DATA_REG, "dsp-muls-s1-indirect-with-offset-2-dsp-src2-data-reg", "muls", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls${dsp-t} ${dsp-destA},(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULS_S1_INDIRECT_2_DSP_SRC2_DATA_REG, "dsp-muls-s1-indirect-2-dsp-src2-data-reg", "muls", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULS_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_DATA_REG, "dsp-muls-s1-indirect-with-post-increment-2-dsp-src2-data-reg", "muls", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_DSP_MULS_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_DATA_REG, "dsp-muls-s1-indirect-with-pre-increment-2-dsp-src2-data-reg", "muls", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls${dsp-t} ${dsp-destA},${s1-direct-addr},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULS_S1_DIRECT_DSP_SRC2_REG_ACC_REG_MUL, "dsp-muls-s1-direct-dsp-src2-reg-acc-reg-mul", "muls", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls${dsp-t} ${dsp-destA},#${s1-imm8},${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULS_S1_IMMEDIATE_DSP_SRC2_REG_ACC_REG_MUL, "dsp-muls-s1-immediate-dsp-src2-reg-acc-reg-mul", "muls", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULS_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-muls-s1-indirect-with-index-2-dsp-src2-reg-acc-reg-mul", "muls", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULS_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-muls-s1-indirect-with-offset-2-dsp-src2-reg-acc-reg-mul", "muls", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls${dsp-t} ${dsp-destA},(${s1-An}),${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULS_S1_INDIRECT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-muls-s1-indirect-2-dsp-src2-reg-acc-reg-mul", "muls", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULS_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-muls-s1-indirect-with-post-increment-2-dsp-src2-reg-acc-reg-mul", "muls", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,${dsp-S2-acc-reg-mul} */
  {
    UBICOM32_INSN_DSP_MULS_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_REG_ACC_REG_MUL, "dsp-muls-s1-indirect-with-pre-increment-2-dsp-src2-reg-acc-reg-mul", "muls", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls${dsp-t} ${dsp-destA},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_DSP_MULS_S1_DIRECT_DSP_IMM_BIT5, "dsp-muls-s1-direct-dsp-imm-bit5", "muls", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls${dsp-t} ${dsp-destA},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_DSP_MULS_S1_IMMEDIATE_DSP_IMM_BIT5, "dsp-muls-s1-immediate-dsp-imm-bit5", "muls", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls${dsp-t} ${dsp-destA},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MULS_S1_INDIRECT_WITH_INDEX_2_DSP_IMM_BIT5, "dsp-muls-s1-indirect-with-index-2-dsp-imm-bit5", "muls", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls${dsp-t} ${dsp-destA},${s1-imm7-2}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MULS_S1_INDIRECT_WITH_OFFSET_2_DSP_IMM_BIT5, "dsp-muls-s1-indirect-with-offset-2-dsp-imm-bit5", "muls", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls${dsp-t} ${dsp-destA},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_DSP_MULS_S1_INDIRECT_2_DSP_IMM_BIT5, "dsp-muls-s1-indirect-2-dsp-imm-bit5", "muls", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls${dsp-t} ${dsp-destA},(${s1-An})${s1-i4-2}++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MULS_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_IMM_BIT5, "dsp-muls-s1-indirect-with-post-increment-2-dsp-imm-bit5", "muls", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* muls${dsp-t} ${dsp-destA},${s1-i4-2}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_DSP_MULS_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_IMM_BIT5, "dsp-muls-s1-indirect-with-pre-increment-2-dsp-imm-bit5", "muls", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sysret ${d-direct-addr},${s1-direct-addr} */
  {
    UBICOM32_INSN_SYSRET_D_DIRECT_S1_DIRECT, "sysret-d-direct-s1-direct", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret #${d-imm8},${s1-direct-addr} */
  {
    UBICOM32_INSN_SYSRET_D_IMMEDIATE_4_S1_DIRECT, "sysret-d-immediate-4-s1-direct", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret (${d-An},${d-r}),${s1-direct-addr} */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_INDEX_4_S1_DIRECT, "sysret-d-indirect-with-index-4-s1-direct", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret ${d-imm7-4}(${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT, "sysret-d-indirect-with-offset-4-s1-direct", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret (${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_4_S1_DIRECT, "sysret-d-indirect-4-s1-direct", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret (${d-An})${d-i4-4}++,${s1-direct-addr} */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_POST_INCREMENT_4_S1_DIRECT, "sysret-d-indirect-with-post-increment-4-s1-direct", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret ${d-i4-4}(${d-An})++,${s1-direct-addr} */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT, "sysret-d-indirect-with-pre-increment-4-s1-direct", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret ${d-direct-addr},#${s1-imm8} */
  {
    UBICOM32_INSN_SYSRET_D_DIRECT_S1_IMMEDIATE, "sysret-d-direct-s1-immediate", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret #${d-imm8},#${s1-imm8} */
  {
    UBICOM32_INSN_SYSRET_D_IMMEDIATE_4_S1_IMMEDIATE, "sysret-d-immediate-4-s1-immediate", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret (${d-An},${d-r}),#${s1-imm8} */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_INDEX_4_S1_IMMEDIATE, "sysret-d-indirect-with-index-4-s1-immediate", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret ${d-imm7-4}(${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_OFFSET_4_S1_IMMEDIATE, "sysret-d-indirect-with-offset-4-s1-immediate", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret (${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_4_S1_IMMEDIATE, "sysret-d-indirect-4-s1-immediate", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret (${d-An})${d-i4-4}++,#${s1-imm8} */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_POST_INCREMENT_4_S1_IMMEDIATE, "sysret-d-indirect-with-post-increment-4-s1-immediate", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret ${d-i4-4}(${d-An})++,#${s1-imm8} */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_IMMEDIATE, "sysret-d-indirect-with-pre-increment-4-s1-immediate", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret ${d-direct-addr},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_SYSRET_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "sysret-d-direct-s1-indirect-with-index-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret #${d-imm8},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_SYSRET_D_IMMEDIATE_4_S1_INDIRECT_WITH_INDEX_4, "sysret-d-immediate-4-s1-indirect-with-index-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret (${d-An},${d-r}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_INDEX_4, "sysret-d-indirect-with-index-4-s1-indirect-with-index-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_INDEX_4, "sysret-d-indirect-with-offset-4-s1-indirect-with-index-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret (${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_4_S1_INDIRECT_WITH_INDEX_4, "sysret-d-indirect-4-s1-indirect-with-index-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret (${d-An})${d-i4-4}++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "sysret-d-indirect-with-post-increment-4-s1-indirect-with-index-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "sysret-d-indirect-with-pre-increment-4-s1-indirect-with-index-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret ${d-direct-addr},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_SYSRET_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "sysret-d-direct-s1-indirect-with-offset-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret #${d-imm8},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_SYSRET_D_IMMEDIATE_4_S1_INDIRECT_WITH_OFFSET_4, "sysret-d-immediate-4-s1-indirect-with-offset-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret (${d-An},${d-r}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_OFFSET_4, "sysret-d-indirect-with-index-4-s1-indirect-with-offset-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_OFFSET_4, "sysret-d-indirect-with-offset-4-s1-indirect-with-offset-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret (${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_4_S1_INDIRECT_WITH_OFFSET_4, "sysret-d-indirect-4-s1-indirect-with-offset-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "sysret-d-indirect-with-post-increment-4-s1-indirect-with-offset-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "sysret-d-indirect-with-pre-increment-4-s1-indirect-with-offset-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret ${d-direct-addr},(${s1-An}) */
  {
    UBICOM32_INSN_SYSRET_D_DIRECT_S1_INDIRECT_4, "sysret-d-direct-s1-indirect-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret #${d-imm8},(${s1-An}) */
  {
    UBICOM32_INSN_SYSRET_D_IMMEDIATE_4_S1_INDIRECT_4, "sysret-d-immediate-4-s1-indirect-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret (${d-An},${d-r}),(${s1-An}) */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_4, "sysret-d-indirect-with-index-4-s1-indirect-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret ${d-imm7-4}(${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_4, "sysret-d-indirect-with-offset-4-s1-indirect-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret (${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_4_S1_INDIRECT_4, "sysret-d-indirect-4-s1-indirect-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret (${d-An})${d-i4-4}++,(${s1-An}) */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_4, "sysret-d-indirect-with-post-increment-4-s1-indirect-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret ${d-i4-4}(${d-An})++,(${s1-An}) */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_4, "sysret-d-indirect-with-pre-increment-4-s1-indirect-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret ${d-direct-addr},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_SYSRET_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "sysret-d-direct-s1-indirect-with-post-increment-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret #${d-imm8},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_SYSRET_D_IMMEDIATE_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "sysret-d-immediate-4-s1-indirect-with-post-increment-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret (${d-An},${d-r}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "sysret-d-indirect-with-index-4-s1-indirect-with-post-increment-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "sysret-d-indirect-with-offset-4-s1-indirect-with-post-increment-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret (${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "sysret-d-indirect-4-s1-indirect-with-post-increment-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "sysret-d-indirect-with-post-increment-4-s1-indirect-with-post-increment-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "sysret-d-indirect-with-pre-increment-4-s1-indirect-with-post-increment-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret ${d-direct-addr},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_SYSRET_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "sysret-d-direct-s1-indirect-with-pre-increment-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret #${d-imm8},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_SYSRET_D_IMMEDIATE_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "sysret-d-immediate-4-s1-indirect-with-pre-increment-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret (${d-An},${d-r}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "sysret-d-indirect-with-index-4-s1-indirect-with-pre-increment-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "sysret-d-indirect-with-offset-4-s1-indirect-with-pre-increment-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret (${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "sysret-d-indirect-4-s1-indirect-with-pre-increment-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "sysret-d-indirect-with-post-increment-4-s1-indirect-with-pre-increment-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sysret ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_SYSRET_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "sysret-d-indirect-with-pre-increment-4-s1-indirect-with-pre-increment-4", "sysret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* syscall ${d-direct-addr} */
  {
    UBICOM32_INSN_SYSCALL_D_DIRECT, "syscall-d-direct", "syscall", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* syscall #${d-imm8} */
  {
    UBICOM32_INSN_SYSCALL_D_IMMEDIATE_4, "syscall-d-immediate-4", "syscall", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* syscall (${d-An},${d-r}) */
  {
    UBICOM32_INSN_SYSCALL_D_INDIRECT_WITH_INDEX_4, "syscall-d-indirect-with-index-4", "syscall", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* syscall ${d-imm7-4}(${d-An}) */
  {
    UBICOM32_INSN_SYSCALL_D_INDIRECT_WITH_OFFSET_4, "syscall-d-indirect-with-offset-4", "syscall", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* syscall (${d-An}) */
  {
    UBICOM32_INSN_SYSCALL_D_INDIRECT_4, "syscall-d-indirect-4", "syscall", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* syscall (${d-An})${d-i4-4}++ */
  {
    UBICOM32_INSN_SYSCALL_D_INDIRECT_WITH_POST_INCREMENT_4, "syscall-d-indirect-with-post-increment-4", "syscall", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* syscall ${d-i4-4}(${d-An})++ */
  {
    UBICOM32_INSN_SYSCALL_D_INDIRECT_WITH_PRE_INCREMENT_4, "syscall-d-indirect-with-pre-increment-4", "syscall", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* prefetch ${d-direct-addr} */
  {
    UBICOM32_INSN_PREFETCH_D_DIRECT, "prefetch-d-direct", "prefetch", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* prefetch #${d-imm8} */
  {
    UBICOM32_INSN_PREFETCH_D_IMMEDIATE_4, "prefetch-d-immediate-4", "prefetch", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* prefetch (${d-An},${d-r}) */
  {
    UBICOM32_INSN_PREFETCH_D_INDIRECT_WITH_INDEX_4, "prefetch-d-indirect-with-index-4", "prefetch", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* prefetch ${d-imm7-4}(${d-An}) */
  {
    UBICOM32_INSN_PREFETCH_D_INDIRECT_WITH_OFFSET_4, "prefetch-d-indirect-with-offset-4", "prefetch", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* prefetch (${d-An}) */
  {
    UBICOM32_INSN_PREFETCH_D_INDIRECT_4, "prefetch-d-indirect-4", "prefetch", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* prefetch (${d-An})${d-i4-4}++ */
  {
    UBICOM32_INSN_PREFETCH_D_INDIRECT_WITH_POST_INCREMENT_4, "prefetch-d-indirect-with-post-increment-4", "prefetch", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* prefetch ${d-i4-4}(${d-An})++ */
  {
    UBICOM32_INSN_PREFETCH_D_INDIRECT_WITH_PRE_INCREMENT_4, "prefetch-d-indirect-with-pre-increment-4", "prefetch", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sync ${d-direct-addr} */
  {
    UBICOM32_INSN_SYNC_D_DIRECT, "sync-d-direct", "sync", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sync #${d-imm8} */
  {
    UBICOM32_INSN_SYNC_D_IMMEDIATE_4, "sync-d-immediate-4", "sync", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sync (${d-An},${d-r}) */
  {
    UBICOM32_INSN_SYNC_D_INDIRECT_WITH_INDEX_4, "sync-d-indirect-with-index-4", "sync", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sync ${d-imm7-4}(${d-An}) */
  {
    UBICOM32_INSN_SYNC_D_INDIRECT_WITH_OFFSET_4, "sync-d-indirect-with-offset-4", "sync", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sync (${d-An}) */
  {
    UBICOM32_INSN_SYNC_D_INDIRECT_4, "sync-d-indirect-4", "sync", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sync (${d-An})${d-i4-4}++ */
  {
    UBICOM32_INSN_SYNC_D_INDIRECT_WITH_POST_INCREMENT_4, "sync-d-indirect-with-post-increment-4", "sync", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* sync ${d-i4-4}(${d-An})++ */
  {
    UBICOM32_INSN_SYNC_D_INDIRECT_WITH_PRE_INCREMENT_4, "sync-d-indirect-with-pre-increment-4", "sync", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* inval ${d-direct-addr} */
  {
    UBICOM32_INSN_INVAL_D_DIRECT, "inval-d-direct", "inval", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* inval #${d-imm8} */
  {
    UBICOM32_INSN_INVAL_D_IMMEDIATE_4, "inval-d-immediate-4", "inval", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* inval (${d-An},${d-r}) */
  {
    UBICOM32_INSN_INVAL_D_INDIRECT_WITH_INDEX_4, "inval-d-indirect-with-index-4", "inval", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* inval ${d-imm7-4}(${d-An}) */
  {
    UBICOM32_INSN_INVAL_D_INDIRECT_WITH_OFFSET_4, "inval-d-indirect-with-offset-4", "inval", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* inval (${d-An}) */
  {
    UBICOM32_INSN_INVAL_D_INDIRECT_4, "inval-d-indirect-4", "inval", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* inval (${d-An})${d-i4-4}++ */
  {
    UBICOM32_INSN_INVAL_D_INDIRECT_WITH_POST_INCREMENT_4, "inval-d-indirect-with-post-increment-4", "inval", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* inval ${d-i4-4}(${d-An})++ */
  {
    UBICOM32_INSN_INVAL_D_INDIRECT_WITH_PRE_INCREMENT_4, "inval-d-indirect-with-pre-increment-4", "inval", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* flushni ${d-direct-addr} */
  {
    UBICOM32_INSN_FLUSHNI_D_DIRECT, "flushni-d-direct", "flushni", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* flushni #${d-imm8} */
  {
    UBICOM32_INSN_FLUSHNI_D_IMMEDIATE_4, "flushni-d-immediate-4", "flushni", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* flushni (${d-An},${d-r}) */
  {
    UBICOM32_INSN_FLUSHNI_D_INDIRECT_WITH_INDEX_4, "flushni-d-indirect-with-index-4", "flushni", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* flushni ${d-imm7-4}(${d-An}) */
  {
    UBICOM32_INSN_FLUSHNI_D_INDIRECT_WITH_OFFSET_4, "flushni-d-indirect-with-offset-4", "flushni", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* flushni (${d-An}) */
  {
    UBICOM32_INSN_FLUSHNI_D_INDIRECT_4, "flushni-d-indirect-4", "flushni", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* flushni (${d-An})${d-i4-4}++ */
  {
    UBICOM32_INSN_FLUSHNI_D_INDIRECT_WITH_POST_INCREMENT_4, "flushni-d-indirect-with-post-increment-4", "flushni", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* flushni ${d-i4-4}(${d-An})++ */
  {
    UBICOM32_INSN_FLUSHNI_D_INDIRECT_WITH_PRE_INCREMENT_4, "flushni-d-indirect-with-pre-increment-4", "flushni", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* flush ${d-direct-addr} */
  {
    UBICOM32_INSN_FLUSH_D_DIRECT, "flush-d-direct", "flush", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* flush #${d-imm8} */
  {
    UBICOM32_INSN_FLUSH_D_IMMEDIATE_4, "flush-d-immediate-4", "flush", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* flush (${d-An},${d-r}) */
  {
    UBICOM32_INSN_FLUSH_D_INDIRECT_WITH_INDEX_4, "flush-d-indirect-with-index-4", "flush", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* flush ${d-imm7-4}(${d-An}) */
  {
    UBICOM32_INSN_FLUSH_D_INDIRECT_WITH_OFFSET_4, "flush-d-indirect-with-offset-4", "flush", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* flush (${d-An}) */
  {
    UBICOM32_INSN_FLUSH_D_INDIRECT_4, "flush-d-indirect-4", "flush", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* flush (${d-An})${d-i4-4}++ */
  {
    UBICOM32_INSN_FLUSH_D_INDIRECT_WITH_POST_INCREMENT_4, "flush-d-indirect-with-post-increment-4", "flush", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* flush ${d-i4-4}(${d-An})++ */
  {
    UBICOM32_INSN_FLUSH_D_INDIRECT_WITH_PRE_INCREMENT_4, "flush-d-indirect-with-pre-increment-4", "flush", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* iread (${s1-An}) */
  {
    UBICOM32_INSN_IREAD_S1_EA_INDIRECT, "iread-s1-ea-indirect", "iread", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iread (${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_IREAD_S1_EA_INDIRECT_WITH_INDEX_4, "iread-s1-ea-indirect-with-index-4", "iread", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iread (${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_IREAD_S1_EA_INDIRECT_WITH_POST_INCREMENT_4, "iread-s1-ea-indirect-with-post-increment-4", "iread", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iread ${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_IREAD_S1_EA_INDIRECT_WITH_PRE_INCREMENT_4, "iread-s1-ea-indirect-with-pre-increment-4", "iread", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iread ${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_IREAD_S1_EA_INDIRECT_WITH_OFFSET_4, "iread-s1-ea-indirect-with-offset-4", "iread", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite (${d-An},${d-r}),${s1-direct-addr} */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_INDEX_S1_DIRECT, "iwrite-d-pea-indirect-with-index-s1-direct", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite ${d-imm7-4}(${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_OFFSET_S1_DIRECT, "iwrite-d-pea-indirect-with-offset-s1-direct", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite (${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_S1_DIRECT, "iwrite-d-pea-indirect-s1-direct", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite (${d-An})${d-i4-4}++,${s1-direct-addr} */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_POST_INCREMENT_S1_DIRECT, "iwrite-d-pea-indirect-with-post-increment-s1-direct", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite ${d-i4-4}(${d-An})++,${s1-direct-addr} */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_PRE_INCREMENT_S1_DIRECT, "iwrite-d-pea-indirect-with-pre-increment-s1-direct", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite (${d-An},${d-r}),#${s1-imm8} */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_INDEX_S1_IMMEDIATE, "iwrite-d-pea-indirect-with-index-s1-immediate", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite ${d-imm7-4}(${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_OFFSET_S1_IMMEDIATE, "iwrite-d-pea-indirect-with-offset-s1-immediate", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite (${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_S1_IMMEDIATE, "iwrite-d-pea-indirect-s1-immediate", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite (${d-An})${d-i4-4}++,#${s1-imm8} */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_POST_INCREMENT_S1_IMMEDIATE, "iwrite-d-pea-indirect-with-post-increment-s1-immediate", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite ${d-i4-4}(${d-An})++,#${s1-imm8} */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_PRE_INCREMENT_S1_IMMEDIATE, "iwrite-d-pea-indirect-with-pre-increment-s1-immediate", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite (${d-An},${d-r}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_INDEX_S1_INDIRECT_WITH_INDEX_4, "iwrite-d-pea-indirect-with-index-s1-indirect-with-index-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_OFFSET_S1_INDIRECT_WITH_INDEX_4, "iwrite-d-pea-indirect-with-offset-s1-indirect-with-index-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite (${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_S1_INDIRECT_WITH_INDEX_4, "iwrite-d-pea-indirect-s1-indirect-with-index-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite (${d-An})${d-i4-4}++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_POST_INCREMENT_S1_INDIRECT_WITH_INDEX_4, "iwrite-d-pea-indirect-with-post-increment-s1-indirect-with-index-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_PRE_INCREMENT_S1_INDIRECT_WITH_INDEX_4, "iwrite-d-pea-indirect-with-pre-increment-s1-indirect-with-index-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite (${d-An},${d-r}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_INDEX_S1_INDIRECT_WITH_OFFSET_4, "iwrite-d-pea-indirect-with-index-s1-indirect-with-offset-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_OFFSET_S1_INDIRECT_WITH_OFFSET_4, "iwrite-d-pea-indirect-with-offset-s1-indirect-with-offset-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite (${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_S1_INDIRECT_WITH_OFFSET_4, "iwrite-d-pea-indirect-s1-indirect-with-offset-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_POST_INCREMENT_S1_INDIRECT_WITH_OFFSET_4, "iwrite-d-pea-indirect-with-post-increment-s1-indirect-with-offset-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_PRE_INCREMENT_S1_INDIRECT_WITH_OFFSET_4, "iwrite-d-pea-indirect-with-pre-increment-s1-indirect-with-offset-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite (${d-An},${d-r}),(${s1-An}) */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_INDEX_S1_INDIRECT_4, "iwrite-d-pea-indirect-with-index-s1-indirect-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite ${d-imm7-4}(${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_OFFSET_S1_INDIRECT_4, "iwrite-d-pea-indirect-with-offset-s1-indirect-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite (${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_S1_INDIRECT_4, "iwrite-d-pea-indirect-s1-indirect-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite (${d-An})${d-i4-4}++,(${s1-An}) */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_POST_INCREMENT_S1_INDIRECT_4, "iwrite-d-pea-indirect-with-post-increment-s1-indirect-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite ${d-i4-4}(${d-An})++,(${s1-An}) */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_PRE_INCREMENT_S1_INDIRECT_4, "iwrite-d-pea-indirect-with-pre-increment-s1-indirect-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite (${d-An},${d-r}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_INDEX_S1_INDIRECT_WITH_POST_INCREMENT_4, "iwrite-d-pea-indirect-with-index-s1-indirect-with-post-increment-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_OFFSET_S1_INDIRECT_WITH_POST_INCREMENT_4, "iwrite-d-pea-indirect-with-offset-s1-indirect-with-post-increment-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite (${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "iwrite-d-pea-indirect-s1-indirect-with-post-increment-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_POST_INCREMENT_S1_INDIRECT_WITH_POST_INCREMENT_4, "iwrite-d-pea-indirect-with-post-increment-s1-indirect-with-post-increment-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_PRE_INCREMENT_S1_INDIRECT_WITH_POST_INCREMENT_4, "iwrite-d-pea-indirect-with-pre-increment-s1-indirect-with-post-increment-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite (${d-An},${d-r}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_INDEX_S1_INDIRECT_WITH_PRE_INCREMENT_4, "iwrite-d-pea-indirect-with-index-s1-indirect-with-pre-increment-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_OFFSET_S1_INDIRECT_WITH_PRE_INCREMENT_4, "iwrite-d-pea-indirect-with-offset-s1-indirect-with-pre-increment-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite (${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "iwrite-d-pea-indirect-s1-indirect-with-pre-increment-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_POST_INCREMENT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "iwrite-d-pea-indirect-with-post-increment-s1-indirect-with-pre-increment-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* iwrite ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_IWRITE_D_PEA_INDIRECT_WITH_PRE_INCREMENT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "iwrite-d-pea-indirect-with-pre-increment-s1-indirect-with-pre-increment-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* tbclr ${d-direct-addr},${s1-direct-addr} */
  {
    UBICOM32_INSN_TBCLR_D_DIRECT_S1_DIRECT, "tbclr-d-direct-s1-direct", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr #${d-imm8},${s1-direct-addr} */
  {
    UBICOM32_INSN_TBCLR_D_IMMEDIATE_4_S1_DIRECT, "tbclr-d-immediate-4-s1-direct", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr (${d-An},${d-r}),${s1-direct-addr} */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_INDEX_4_S1_DIRECT, "tbclr-d-indirect-with-index-4-s1-direct", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr ${d-imm7-4}(${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT, "tbclr-d-indirect-with-offset-4-s1-direct", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr (${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_4_S1_DIRECT, "tbclr-d-indirect-4-s1-direct", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr (${d-An})${d-i4-4}++,${s1-direct-addr} */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_POST_INCREMENT_4_S1_DIRECT, "tbclr-d-indirect-with-post-increment-4-s1-direct", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr ${d-i4-4}(${d-An})++,${s1-direct-addr} */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT, "tbclr-d-indirect-with-pre-increment-4-s1-direct", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr ${d-direct-addr},#${s1-imm8} */
  {
    UBICOM32_INSN_TBCLR_D_DIRECT_S1_IMMEDIATE, "tbclr-d-direct-s1-immediate", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr #${d-imm8},#${s1-imm8} */
  {
    UBICOM32_INSN_TBCLR_D_IMMEDIATE_4_S1_IMMEDIATE, "tbclr-d-immediate-4-s1-immediate", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr (${d-An},${d-r}),#${s1-imm8} */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_INDEX_4_S1_IMMEDIATE, "tbclr-d-indirect-with-index-4-s1-immediate", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr ${d-imm7-4}(${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_OFFSET_4_S1_IMMEDIATE, "tbclr-d-indirect-with-offset-4-s1-immediate", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr (${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_4_S1_IMMEDIATE, "tbclr-d-indirect-4-s1-immediate", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr (${d-An})${d-i4-4}++,#${s1-imm8} */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_POST_INCREMENT_4_S1_IMMEDIATE, "tbclr-d-indirect-with-post-increment-4-s1-immediate", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr ${d-i4-4}(${d-An})++,#${s1-imm8} */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_IMMEDIATE, "tbclr-d-indirect-with-pre-increment-4-s1-immediate", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr ${d-direct-addr},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_TBCLR_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "tbclr-d-direct-s1-indirect-with-index-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr #${d-imm8},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_TBCLR_D_IMMEDIATE_4_S1_INDIRECT_WITH_INDEX_4, "tbclr-d-immediate-4-s1-indirect-with-index-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr (${d-An},${d-r}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_INDEX_4, "tbclr-d-indirect-with-index-4-s1-indirect-with-index-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_INDEX_4, "tbclr-d-indirect-with-offset-4-s1-indirect-with-index-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr (${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_4_S1_INDIRECT_WITH_INDEX_4, "tbclr-d-indirect-4-s1-indirect-with-index-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr (${d-An})${d-i4-4}++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "tbclr-d-indirect-with-post-increment-4-s1-indirect-with-index-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "tbclr-d-indirect-with-pre-increment-4-s1-indirect-with-index-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr ${d-direct-addr},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_TBCLR_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "tbclr-d-direct-s1-indirect-with-offset-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr #${d-imm8},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_TBCLR_D_IMMEDIATE_4_S1_INDIRECT_WITH_OFFSET_4, "tbclr-d-immediate-4-s1-indirect-with-offset-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr (${d-An},${d-r}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_OFFSET_4, "tbclr-d-indirect-with-index-4-s1-indirect-with-offset-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_OFFSET_4, "tbclr-d-indirect-with-offset-4-s1-indirect-with-offset-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr (${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_4_S1_INDIRECT_WITH_OFFSET_4, "tbclr-d-indirect-4-s1-indirect-with-offset-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "tbclr-d-indirect-with-post-increment-4-s1-indirect-with-offset-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "tbclr-d-indirect-with-pre-increment-4-s1-indirect-with-offset-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr ${d-direct-addr},(${s1-An}) */
  {
    UBICOM32_INSN_TBCLR_D_DIRECT_S1_INDIRECT_4, "tbclr-d-direct-s1-indirect-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr #${d-imm8},(${s1-An}) */
  {
    UBICOM32_INSN_TBCLR_D_IMMEDIATE_4_S1_INDIRECT_4, "tbclr-d-immediate-4-s1-indirect-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr (${d-An},${d-r}),(${s1-An}) */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_4, "tbclr-d-indirect-with-index-4-s1-indirect-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr ${d-imm7-4}(${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_4, "tbclr-d-indirect-with-offset-4-s1-indirect-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr (${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_4_S1_INDIRECT_4, "tbclr-d-indirect-4-s1-indirect-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr (${d-An})${d-i4-4}++,(${s1-An}) */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_4, "tbclr-d-indirect-with-post-increment-4-s1-indirect-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr ${d-i4-4}(${d-An})++,(${s1-An}) */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_4, "tbclr-d-indirect-with-pre-increment-4-s1-indirect-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr ${d-direct-addr},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_TBCLR_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "tbclr-d-direct-s1-indirect-with-post-increment-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr #${d-imm8},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_TBCLR_D_IMMEDIATE_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "tbclr-d-immediate-4-s1-indirect-with-post-increment-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr (${d-An},${d-r}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "tbclr-d-indirect-with-index-4-s1-indirect-with-post-increment-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "tbclr-d-indirect-with-offset-4-s1-indirect-with-post-increment-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr (${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "tbclr-d-indirect-4-s1-indirect-with-post-increment-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "tbclr-d-indirect-with-post-increment-4-s1-indirect-with-post-increment-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "tbclr-d-indirect-with-pre-increment-4-s1-indirect-with-post-increment-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr ${d-direct-addr},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_TBCLR_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "tbclr-d-direct-s1-indirect-with-pre-increment-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr #${d-imm8},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_TBCLR_D_IMMEDIATE_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "tbclr-d-immediate-4-s1-indirect-with-pre-increment-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr (${d-An},${d-r}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "tbclr-d-indirect-with-index-4-s1-indirect-with-pre-increment-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "tbclr-d-indirect-with-offset-4-s1-indirect-with-pre-increment-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr (${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "tbclr-d-indirect-4-s1-indirect-with-pre-increment-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "tbclr-d-indirect-with-post-increment-4-s1-indirect-with-pre-increment-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbclr ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_TBCLR_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "tbclr-d-indirect-with-pre-increment-4-s1-indirect-with-pre-increment-4", "tbclr", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset ${d-direct-addr},${s1-direct-addr} */
  {
    UBICOM32_INSN_TBSET_D_DIRECT_S1_DIRECT, "tbset-d-direct-s1-direct", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset #${d-imm8},${s1-direct-addr} */
  {
    UBICOM32_INSN_TBSET_D_IMMEDIATE_4_S1_DIRECT, "tbset-d-immediate-4-s1-direct", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset (${d-An},${d-r}),${s1-direct-addr} */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_INDEX_4_S1_DIRECT, "tbset-d-indirect-with-index-4-s1-direct", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset ${d-imm7-4}(${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT, "tbset-d-indirect-with-offset-4-s1-direct", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset (${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_4_S1_DIRECT, "tbset-d-indirect-4-s1-direct", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset (${d-An})${d-i4-4}++,${s1-direct-addr} */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_POST_INCREMENT_4_S1_DIRECT, "tbset-d-indirect-with-post-increment-4-s1-direct", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset ${d-i4-4}(${d-An})++,${s1-direct-addr} */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT, "tbset-d-indirect-with-pre-increment-4-s1-direct", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset ${d-direct-addr},#${s1-imm8} */
  {
    UBICOM32_INSN_TBSET_D_DIRECT_S1_IMMEDIATE, "tbset-d-direct-s1-immediate", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset #${d-imm8},#${s1-imm8} */
  {
    UBICOM32_INSN_TBSET_D_IMMEDIATE_4_S1_IMMEDIATE, "tbset-d-immediate-4-s1-immediate", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset (${d-An},${d-r}),#${s1-imm8} */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_INDEX_4_S1_IMMEDIATE, "tbset-d-indirect-with-index-4-s1-immediate", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset ${d-imm7-4}(${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_OFFSET_4_S1_IMMEDIATE, "tbset-d-indirect-with-offset-4-s1-immediate", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset (${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_4_S1_IMMEDIATE, "tbset-d-indirect-4-s1-immediate", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset (${d-An})${d-i4-4}++,#${s1-imm8} */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_POST_INCREMENT_4_S1_IMMEDIATE, "tbset-d-indirect-with-post-increment-4-s1-immediate", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset ${d-i4-4}(${d-An})++,#${s1-imm8} */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_IMMEDIATE, "tbset-d-indirect-with-pre-increment-4-s1-immediate", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset ${d-direct-addr},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_TBSET_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "tbset-d-direct-s1-indirect-with-index-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset #${d-imm8},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_TBSET_D_IMMEDIATE_4_S1_INDIRECT_WITH_INDEX_4, "tbset-d-immediate-4-s1-indirect-with-index-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset (${d-An},${d-r}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_INDEX_4, "tbset-d-indirect-with-index-4-s1-indirect-with-index-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_INDEX_4, "tbset-d-indirect-with-offset-4-s1-indirect-with-index-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset (${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_4_S1_INDIRECT_WITH_INDEX_4, "tbset-d-indirect-4-s1-indirect-with-index-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset (${d-An})${d-i4-4}++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "tbset-d-indirect-with-post-increment-4-s1-indirect-with-index-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "tbset-d-indirect-with-pre-increment-4-s1-indirect-with-index-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset ${d-direct-addr},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_TBSET_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "tbset-d-direct-s1-indirect-with-offset-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset #${d-imm8},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_TBSET_D_IMMEDIATE_4_S1_INDIRECT_WITH_OFFSET_4, "tbset-d-immediate-4-s1-indirect-with-offset-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset (${d-An},${d-r}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_OFFSET_4, "tbset-d-indirect-with-index-4-s1-indirect-with-offset-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_OFFSET_4, "tbset-d-indirect-with-offset-4-s1-indirect-with-offset-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset (${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_4_S1_INDIRECT_WITH_OFFSET_4, "tbset-d-indirect-4-s1-indirect-with-offset-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "tbset-d-indirect-with-post-increment-4-s1-indirect-with-offset-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "tbset-d-indirect-with-pre-increment-4-s1-indirect-with-offset-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset ${d-direct-addr},(${s1-An}) */
  {
    UBICOM32_INSN_TBSET_D_DIRECT_S1_INDIRECT_4, "tbset-d-direct-s1-indirect-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset #${d-imm8},(${s1-An}) */
  {
    UBICOM32_INSN_TBSET_D_IMMEDIATE_4_S1_INDIRECT_4, "tbset-d-immediate-4-s1-indirect-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset (${d-An},${d-r}),(${s1-An}) */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_4, "tbset-d-indirect-with-index-4-s1-indirect-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset ${d-imm7-4}(${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_4, "tbset-d-indirect-with-offset-4-s1-indirect-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset (${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_4_S1_INDIRECT_4, "tbset-d-indirect-4-s1-indirect-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset (${d-An})${d-i4-4}++,(${s1-An}) */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_4, "tbset-d-indirect-with-post-increment-4-s1-indirect-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset ${d-i4-4}(${d-An})++,(${s1-An}) */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_4, "tbset-d-indirect-with-pre-increment-4-s1-indirect-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset ${d-direct-addr},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_TBSET_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "tbset-d-direct-s1-indirect-with-post-increment-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset #${d-imm8},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_TBSET_D_IMMEDIATE_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "tbset-d-immediate-4-s1-indirect-with-post-increment-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset (${d-An},${d-r}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "tbset-d-indirect-with-index-4-s1-indirect-with-post-increment-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "tbset-d-indirect-with-offset-4-s1-indirect-with-post-increment-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset (${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "tbset-d-indirect-4-s1-indirect-with-post-increment-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "tbset-d-indirect-with-post-increment-4-s1-indirect-with-post-increment-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "tbset-d-indirect-with-pre-increment-4-s1-indirect-with-post-increment-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset ${d-direct-addr},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_TBSET_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "tbset-d-direct-s1-indirect-with-pre-increment-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset #${d-imm8},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_TBSET_D_IMMEDIATE_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "tbset-d-immediate-4-s1-indirect-with-pre-increment-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset (${d-An},${d-r}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "tbset-d-indirect-with-index-4-s1-indirect-with-pre-increment-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "tbset-d-indirect-with-offset-4-s1-indirect-with-pre-increment-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset (${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "tbset-d-indirect-4-s1-indirect-with-pre-increment-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "tbset-d-indirect-with-post-increment-4-s1-indirect-with-pre-increment-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* tbset ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_TBSET_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "tbset-d-indirect-with-pre-increment-4-s1-indirect-with-pre-increment-4", "tbset", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* setcsr ${s1-direct-addr} */
  {
    UBICOM32_INSN_SETCSR_S1_DIRECT, "setcsr-s1-direct", "setcsr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* setcsr #${s1-imm8} */
  {
    UBICOM32_INSN_SETCSR_S1_IMMEDIATE, "setcsr-s1-immediate", "setcsr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* setcsr (${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_SETCSR_S1_INDIRECT_WITH_INDEX_4, "setcsr-s1-indirect-with-index-4", "setcsr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* setcsr ${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_SETCSR_S1_INDIRECT_WITH_OFFSET_4, "setcsr-s1-indirect-with-offset-4", "setcsr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* setcsr (${s1-An}) */
  {
    UBICOM32_INSN_SETCSR_S1_INDIRECT_4, "setcsr-s1-indirect-4", "setcsr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* setcsr (${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_SETCSR_S1_INDIRECT_WITH_POST_INCREMENT_4, "setcsr-s1-indirect-with-post-increment-4", "setcsr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* setcsr ${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_SETCSR_S1_INDIRECT_WITH_PRE_INCREMENT_4, "setcsr-s1-indirect-with-pre-increment-4", "setcsr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bkpt ${s1-direct-addr} */
  {
    UBICOM32_INSN_BKPT_S1_DIRECT, "bkpt-s1-direct", "bkpt", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_BASE), 0 } } } }
  },
/* bkpt #${s1-imm8} */
  {
    UBICOM32_INSN_BKPT_S1_IMMEDIATE, "bkpt-s1-immediate", "bkpt", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_BASE), 0 } } } }
  },
/* bkpt (${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_BKPT_S1_INDIRECT_WITH_INDEX_4, "bkpt-s1-indirect-with-index-4", "bkpt", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_BASE), 0 } } } }
  },
/* bkpt ${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_BKPT_S1_INDIRECT_WITH_OFFSET_4, "bkpt-s1-indirect-with-offset-4", "bkpt", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_BASE), 0 } } } }
  },
/* bkpt (${s1-An}) */
  {
    UBICOM32_INSN_BKPT_S1_INDIRECT_4, "bkpt-s1-indirect-4", "bkpt", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_BASE), 0 } } } }
  },
/* bkpt (${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_BKPT_S1_INDIRECT_WITH_POST_INCREMENT_4, "bkpt-s1-indirect-with-post-increment-4", "bkpt", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_BASE), 0 } } } }
  },
/* bkpt ${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_BKPT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "bkpt-s1-indirect-with-pre-increment-4", "bkpt", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_BASE), 0 } } } }
  },
/* ret ${s1-direct-addr} */
  {
    UBICOM32_INSN_RET_S1_DIRECT, "ret-s1-direct", "ret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_BASE), 0 } } } }
  },
/* ret #${s1-imm8} */
  {
    UBICOM32_INSN_RET_S1_IMMEDIATE, "ret-s1-immediate", "ret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_BASE), 0 } } } }
  },
/* ret (${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_RET_S1_INDIRECT_WITH_INDEX_4, "ret-s1-indirect-with-index-4", "ret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_BASE), 0 } } } }
  },
/* ret ${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_RET_S1_INDIRECT_WITH_OFFSET_4, "ret-s1-indirect-with-offset-4", "ret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_BASE), 0 } } } }
  },
/* ret (${s1-An}) */
  {
    UBICOM32_INSN_RET_S1_INDIRECT_4, "ret-s1-indirect-4", "ret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_BASE), 0 } } } }
  },
/* ret (${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_RET_S1_INDIRECT_WITH_POST_INCREMENT_4, "ret-s1-indirect-with-post-increment-4", "ret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_BASE), 0 } } } }
  },
/* ret ${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_RET_S1_INDIRECT_WITH_PRE_INCREMENT_4, "ret-s1-indirect-with-pre-increment-4", "ret", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_BASE), 0 } } } }
  },
/* movea ${d-direct-addr},${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVEA_D_DIRECT_S1_DIRECT, "movea-d-direct-s1-direct", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea #${d-imm8},${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVEA_D_IMMEDIATE_4_S1_DIRECT, "movea-d-immediate-4-s1-direct", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea (${d-An},${d-r}),${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_INDEX_4_S1_DIRECT, "movea-d-indirect-with-index-4-s1-direct", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea ${d-imm7-4}(${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT, "movea-d-indirect-with-offset-4-s1-direct", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea (${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_4_S1_DIRECT, "movea-d-indirect-4-s1-direct", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea (${d-An})${d-i4-4}++,${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_POST_INCREMENT_4_S1_DIRECT, "movea-d-indirect-with-post-increment-4-s1-direct", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea ${d-i4-4}(${d-An})++,${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT, "movea-d-indirect-with-pre-increment-4-s1-direct", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea ${d-direct-addr},#${s1-imm8} */
  {
    UBICOM32_INSN_MOVEA_D_DIRECT_S1_IMMEDIATE, "movea-d-direct-s1-immediate", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea #${d-imm8},#${s1-imm8} */
  {
    UBICOM32_INSN_MOVEA_D_IMMEDIATE_4_S1_IMMEDIATE, "movea-d-immediate-4-s1-immediate", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea (${d-An},${d-r}),#${s1-imm8} */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_INDEX_4_S1_IMMEDIATE, "movea-d-indirect-with-index-4-s1-immediate", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea ${d-imm7-4}(${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_OFFSET_4_S1_IMMEDIATE, "movea-d-indirect-with-offset-4-s1-immediate", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea (${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_4_S1_IMMEDIATE, "movea-d-indirect-4-s1-immediate", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea (${d-An})${d-i4-4}++,#${s1-imm8} */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_POST_INCREMENT_4_S1_IMMEDIATE, "movea-d-indirect-with-post-increment-4-s1-immediate", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea ${d-i4-4}(${d-An})++,#${s1-imm8} */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_IMMEDIATE, "movea-d-indirect-with-pre-increment-4-s1-immediate", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea ${d-direct-addr},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVEA_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "movea-d-direct-s1-indirect-with-index-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea #${d-imm8},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVEA_D_IMMEDIATE_4_S1_INDIRECT_WITH_INDEX_4, "movea-d-immediate-4-s1-indirect-with-index-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea (${d-An},${d-r}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_INDEX_4, "movea-d-indirect-with-index-4-s1-indirect-with-index-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_INDEX_4, "movea-d-indirect-with-offset-4-s1-indirect-with-index-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea (${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_4_S1_INDIRECT_WITH_INDEX_4, "movea-d-indirect-4-s1-indirect-with-index-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea (${d-An})${d-i4-4}++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "movea-d-indirect-with-post-increment-4-s1-indirect-with-index-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "movea-d-indirect-with-pre-increment-4-s1-indirect-with-index-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea ${d-direct-addr},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_MOVEA_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "movea-d-direct-s1-indirect-with-offset-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea #${d-imm8},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_MOVEA_D_IMMEDIATE_4_S1_INDIRECT_WITH_OFFSET_4, "movea-d-immediate-4-s1-indirect-with-offset-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea (${d-An},${d-r}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_OFFSET_4, "movea-d-indirect-with-index-4-s1-indirect-with-offset-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_OFFSET_4, "movea-d-indirect-with-offset-4-s1-indirect-with-offset-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea (${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_4_S1_INDIRECT_WITH_OFFSET_4, "movea-d-indirect-4-s1-indirect-with-offset-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "movea-d-indirect-with-post-increment-4-s1-indirect-with-offset-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "movea-d-indirect-with-pre-increment-4-s1-indirect-with-offset-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea ${d-direct-addr},(${s1-An}) */
  {
    UBICOM32_INSN_MOVEA_D_DIRECT_S1_INDIRECT_4, "movea-d-direct-s1-indirect-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea #${d-imm8},(${s1-An}) */
  {
    UBICOM32_INSN_MOVEA_D_IMMEDIATE_4_S1_INDIRECT_4, "movea-d-immediate-4-s1-indirect-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea (${d-An},${d-r}),(${s1-An}) */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_4, "movea-d-indirect-with-index-4-s1-indirect-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea ${d-imm7-4}(${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_4, "movea-d-indirect-with-offset-4-s1-indirect-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea (${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_4_S1_INDIRECT_4, "movea-d-indirect-4-s1-indirect-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea (${d-An})${d-i4-4}++,(${s1-An}) */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_4, "movea-d-indirect-with-post-increment-4-s1-indirect-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea ${d-i4-4}(${d-An})++,(${s1-An}) */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_4, "movea-d-indirect-with-pre-increment-4-s1-indirect-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea ${d-direct-addr},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_MOVEA_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "movea-d-direct-s1-indirect-with-post-increment-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea #${d-imm8},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_MOVEA_D_IMMEDIATE_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "movea-d-immediate-4-s1-indirect-with-post-increment-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea (${d-An},${d-r}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "movea-d-indirect-with-index-4-s1-indirect-with-post-increment-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "movea-d-indirect-with-offset-4-s1-indirect-with-post-increment-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea (${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "movea-d-indirect-4-s1-indirect-with-post-increment-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "movea-d-indirect-with-post-increment-4-s1-indirect-with-post-increment-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "movea-d-indirect-with-pre-increment-4-s1-indirect-with-post-increment-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea ${d-direct-addr},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVEA_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "movea-d-direct-s1-indirect-with-pre-increment-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea #${d-imm8},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVEA_D_IMMEDIATE_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "movea-d-immediate-4-s1-indirect-with-pre-increment-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea (${d-An},${d-r}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "movea-d-indirect-with-index-4-s1-indirect-with-pre-increment-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "movea-d-indirect-with-offset-4-s1-indirect-with-pre-increment-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea (${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "movea-d-indirect-4-s1-indirect-with-pre-increment-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "movea-d-indirect-with-post-increment-4-s1-indirect-with-pre-increment-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* movea ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVEA_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "movea-d-indirect-with-pre-increment-4-s1-indirect-with-pre-increment-4", "movea", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* move.4 ${d-direct-addr},${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVE_4_D_DIRECT_S1_DIRECT, "move.4-d-direct-s1-direct", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 #${d-imm8},${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVE_4_D_IMMEDIATE_4_S1_DIRECT, "move.4-d-immediate-4-s1-direct", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 (${d-An},${d-r}),${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_INDEX_4_S1_DIRECT, "move.4-d-indirect-with-index-4-s1-direct", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 ${d-imm7-4}(${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT, "move.4-d-indirect-with-offset-4-s1-direct", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 (${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_4_S1_DIRECT, "move.4-d-indirect-4-s1-direct", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 (${d-An})${d-i4-4}++,${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_DIRECT, "move.4-d-indirect-with-post-increment-4-s1-direct", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 ${d-i4-4}(${d-An})++,${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT, "move.4-d-indirect-with-pre-increment-4-s1-direct", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 ${d-direct-addr},#${s1-imm8} */
  {
    UBICOM32_INSN_MOVE_4_D_DIRECT_S1_IMMEDIATE, "move.4-d-direct-s1-immediate", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 #${d-imm8},#${s1-imm8} */
  {
    UBICOM32_INSN_MOVE_4_D_IMMEDIATE_4_S1_IMMEDIATE, "move.4-d-immediate-4-s1-immediate", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 (${d-An},${d-r}),#${s1-imm8} */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_INDEX_4_S1_IMMEDIATE, "move.4-d-indirect-with-index-4-s1-immediate", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 ${d-imm7-4}(${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_OFFSET_4_S1_IMMEDIATE, "move.4-d-indirect-with-offset-4-s1-immediate", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 (${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_4_S1_IMMEDIATE, "move.4-d-indirect-4-s1-immediate", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 (${d-An})${d-i4-4}++,#${s1-imm8} */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_IMMEDIATE, "move.4-d-indirect-with-post-increment-4-s1-immediate", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 ${d-i4-4}(${d-An})++,#${s1-imm8} */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_IMMEDIATE, "move.4-d-indirect-with-pre-increment-4-s1-immediate", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 ${d-direct-addr},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVE_4_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "move.4-d-direct-s1-indirect-with-index-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 #${d-imm8},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVE_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_INDEX_4, "move.4-d-immediate-4-s1-indirect-with-index-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 (${d-An},${d-r}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_INDEX_4, "move.4-d-indirect-with-index-4-s1-indirect-with-index-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_INDEX_4, "move.4-d-indirect-with-offset-4-s1-indirect-with-index-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 (${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_4_S1_INDIRECT_WITH_INDEX_4, "move.4-d-indirect-4-s1-indirect-with-index-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 (${d-An})${d-i4-4}++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "move.4-d-indirect-with-post-increment-4-s1-indirect-with-index-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "move.4-d-indirect-with-pre-increment-4-s1-indirect-with-index-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 ${d-direct-addr},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_4_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "move.4-d-direct-s1-indirect-with-offset-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 #${d-imm8},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_OFFSET_4, "move.4-d-immediate-4-s1-indirect-with-offset-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 (${d-An},${d-r}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_OFFSET_4, "move.4-d-indirect-with-index-4-s1-indirect-with-offset-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_OFFSET_4, "move.4-d-indirect-with-offset-4-s1-indirect-with-offset-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 (${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_4_S1_INDIRECT_WITH_OFFSET_4, "move.4-d-indirect-4-s1-indirect-with-offset-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "move.4-d-indirect-with-post-increment-4-s1-indirect-with-offset-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "move.4-d-indirect-with-pre-increment-4-s1-indirect-with-offset-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 ${d-direct-addr},(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_4_D_DIRECT_S1_INDIRECT_4, "move.4-d-direct-s1-indirect-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 #${d-imm8},(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_4_D_IMMEDIATE_4_S1_INDIRECT_4, "move.4-d-immediate-4-s1-indirect-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 (${d-An},${d-r}),(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_4, "move.4-d-indirect-with-index-4-s1-indirect-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 ${d-imm7-4}(${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_4, "move.4-d-indirect-with-offset-4-s1-indirect-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 (${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_4_S1_INDIRECT_4, "move.4-d-indirect-4-s1-indirect-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 (${d-An})${d-i4-4}++,(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_4, "move.4-d-indirect-with-post-increment-4-s1-indirect-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 ${d-i4-4}(${d-An})++,(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_4, "move.4-d-indirect-with-pre-increment-4-s1-indirect-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 ${d-direct-addr},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_MOVE_4_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "move.4-d-direct-s1-indirect-with-post-increment-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 #${d-imm8},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_MOVE_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "move.4-d-immediate-4-s1-indirect-with-post-increment-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 (${d-An},${d-r}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "move.4-d-indirect-with-index-4-s1-indirect-with-post-increment-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "move.4-d-indirect-with-offset-4-s1-indirect-with-post-increment-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 (${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "move.4-d-indirect-4-s1-indirect-with-post-increment-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "move.4-d-indirect-with-post-increment-4-s1-indirect-with-post-increment-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "move.4-d-indirect-with-pre-increment-4-s1-indirect-with-post-increment-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 ${d-direct-addr},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVE_4_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "move.4-d-direct-s1-indirect-with-pre-increment-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 #${d-imm8},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVE_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "move.4-d-immediate-4-s1-indirect-with-pre-increment-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 (${d-An},${d-r}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "move.4-d-indirect-with-index-4-s1-indirect-with-pre-increment-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "move.4-d-indirect-with-offset-4-s1-indirect-with-pre-increment-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 (${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "move.4-d-indirect-4-s1-indirect-with-pre-increment-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "move.4-d-indirect-with-post-increment-4-s1-indirect-with-pre-increment-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.4 ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "move.4-d-indirect-with-pre-increment-4-s1-indirect-with-pre-increment-4", "move.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* iread (${s1-An}) */
  {
    UBICOM32_INSN_COMPATIBILITY_IREAD_S1_EA_INDIRECT, "compatibility-iread-s1-ea-indirect", "iread", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iread (${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_COMPATIBILITY_IREAD_S1_EA_INDIRECT_WITH_INDEX_4, "compatibility-iread-s1-ea-indirect-with-index-4", "iread", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iread (${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_COMPATIBILITY_IREAD_S1_EA_INDIRECT_WITH_POST_INCREMENT_4, "compatibility-iread-s1-ea-indirect-with-post-increment-4", "iread", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iread ${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_COMPATIBILITY_IREAD_S1_EA_INDIRECT_WITH_PRE_INCREMENT_4, "compatibility-iread-s1-ea-indirect-with-pre-increment-4", "iread", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iread ${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_COMPATIBILITY_IREAD_S1_EA_INDIRECT_WITH_OFFSET_4, "compatibility-iread-s1-ea-indirect-with-offset-4", "iread", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite (${d-An},${d-r}),${s1-direct-addr} */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_INDEX_S1_DIRECT, "compatibility-iwrite-d-pea-indirect-with-index-s1-direct", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite ${d-imm7-4}(${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_OFFSET_S1_DIRECT, "compatibility-iwrite-d-pea-indirect-with-offset-s1-direct", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite (${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_S1_DIRECT, "compatibility-iwrite-d-pea-indirect-s1-direct", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite (${d-An})${d-i4-4}++,${s1-direct-addr} */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_POST_INCREMENT_S1_DIRECT, "compatibility-iwrite-d-pea-indirect-with-post-increment-s1-direct", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite ${d-i4-4}(${d-An})++,${s1-direct-addr} */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_PRE_INCREMENT_S1_DIRECT, "compatibility-iwrite-d-pea-indirect-with-pre-increment-s1-direct", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite (${d-An},${d-r}),#${s1-imm8} */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_INDEX_S1_IMMEDIATE, "compatibility-iwrite-d-pea-indirect-with-index-s1-immediate", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite ${d-imm7-4}(${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_OFFSET_S1_IMMEDIATE, "compatibility-iwrite-d-pea-indirect-with-offset-s1-immediate", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite (${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_S1_IMMEDIATE, "compatibility-iwrite-d-pea-indirect-s1-immediate", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite (${d-An})${d-i4-4}++,#${s1-imm8} */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_POST_INCREMENT_S1_IMMEDIATE, "compatibility-iwrite-d-pea-indirect-with-post-increment-s1-immediate", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite ${d-i4-4}(${d-An})++,#${s1-imm8} */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_PRE_INCREMENT_S1_IMMEDIATE, "compatibility-iwrite-d-pea-indirect-with-pre-increment-s1-immediate", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite (${d-An},${d-r}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_INDEX_S1_INDIRECT_WITH_INDEX_4, "compatibility-iwrite-d-pea-indirect-with-index-s1-indirect-with-index-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_OFFSET_S1_INDIRECT_WITH_INDEX_4, "compatibility-iwrite-d-pea-indirect-with-offset-s1-indirect-with-index-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite (${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_S1_INDIRECT_WITH_INDEX_4, "compatibility-iwrite-d-pea-indirect-s1-indirect-with-index-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite (${d-An})${d-i4-4}++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_POST_INCREMENT_S1_INDIRECT_WITH_INDEX_4, "compatibility-iwrite-d-pea-indirect-with-post-increment-s1-indirect-with-index-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_PRE_INCREMENT_S1_INDIRECT_WITH_INDEX_4, "compatibility-iwrite-d-pea-indirect-with-pre-increment-s1-indirect-with-index-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite (${d-An},${d-r}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_INDEX_S1_INDIRECT_WITH_OFFSET_4, "compatibility-iwrite-d-pea-indirect-with-index-s1-indirect-with-offset-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_OFFSET_S1_INDIRECT_WITH_OFFSET_4, "compatibility-iwrite-d-pea-indirect-with-offset-s1-indirect-with-offset-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite (${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_S1_INDIRECT_WITH_OFFSET_4, "compatibility-iwrite-d-pea-indirect-s1-indirect-with-offset-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_POST_INCREMENT_S1_INDIRECT_WITH_OFFSET_4, "compatibility-iwrite-d-pea-indirect-with-post-increment-s1-indirect-with-offset-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_PRE_INCREMENT_S1_INDIRECT_WITH_OFFSET_4, "compatibility-iwrite-d-pea-indirect-with-pre-increment-s1-indirect-with-offset-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite (${d-An},${d-r}),(${s1-An}) */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_INDEX_S1_INDIRECT_4, "compatibility-iwrite-d-pea-indirect-with-index-s1-indirect-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite ${d-imm7-4}(${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_OFFSET_S1_INDIRECT_4, "compatibility-iwrite-d-pea-indirect-with-offset-s1-indirect-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite (${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_S1_INDIRECT_4, "compatibility-iwrite-d-pea-indirect-s1-indirect-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite (${d-An})${d-i4-4}++,(${s1-An}) */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_POST_INCREMENT_S1_INDIRECT_4, "compatibility-iwrite-d-pea-indirect-with-post-increment-s1-indirect-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite ${d-i4-4}(${d-An})++,(${s1-An}) */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_PRE_INCREMENT_S1_INDIRECT_4, "compatibility-iwrite-d-pea-indirect-with-pre-increment-s1-indirect-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite (${d-An},${d-r}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_INDEX_S1_INDIRECT_WITH_POST_INCREMENT_4, "compatibility-iwrite-d-pea-indirect-with-index-s1-indirect-with-post-increment-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_OFFSET_S1_INDIRECT_WITH_POST_INCREMENT_4, "compatibility-iwrite-d-pea-indirect-with-offset-s1-indirect-with-post-increment-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite (${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "compatibility-iwrite-d-pea-indirect-s1-indirect-with-post-increment-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_POST_INCREMENT_S1_INDIRECT_WITH_POST_INCREMENT_4, "compatibility-iwrite-d-pea-indirect-with-post-increment-s1-indirect-with-post-increment-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_PRE_INCREMENT_S1_INDIRECT_WITH_POST_INCREMENT_4, "compatibility-iwrite-d-pea-indirect-with-pre-increment-s1-indirect-with-post-increment-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite (${d-An},${d-r}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_INDEX_S1_INDIRECT_WITH_PRE_INCREMENT_4, "compatibility-iwrite-d-pea-indirect-with-index-s1-indirect-with-pre-increment-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_OFFSET_S1_INDIRECT_WITH_PRE_INCREMENT_4, "compatibility-iwrite-d-pea-indirect-with-offset-s1-indirect-with-pre-increment-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite (${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "compatibility-iwrite-d-pea-indirect-s1-indirect-with-pre-increment-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_POST_INCREMENT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "compatibility-iwrite-d-pea-indirect-with-post-increment-s1-indirect-with-pre-increment-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* iwrite ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_COMPATIBILITY_IWRITE_D_PEA_INDIRECT_WITH_PRE_INCREMENT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "compatibility-iwrite-d-pea-indirect-with-pre-increment-s1-indirect-with-pre-increment-4", "iwrite", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* move.2 ${d-direct-addr},${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVE_2_D_DIRECT_S1_DIRECT, "move.2-d-direct-s1-direct", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 #${d-imm8},${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVE_2_D_IMMEDIATE_2_S1_DIRECT, "move.2-d-immediate-2-s1-direct", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 (${d-An},${d-r}),${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_INDEX_2_S1_DIRECT, "move.2-d-indirect-with-index-2-s1-direct", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 ${d-imm7-2}(${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_OFFSET_2_S1_DIRECT, "move.2-d-indirect-with-offset-2-s1-direct", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 (${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_2_S1_DIRECT, "move.2-d-indirect-2-s1-direct", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 (${d-An})${d-i4-2}++,${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_DIRECT, "move.2-d-indirect-with-post-increment-2-s1-direct", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 ${d-i4-2}(${d-An})++,${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_DIRECT, "move.2-d-indirect-with-pre-increment-2-s1-direct", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 ${d-direct-addr},#${s1-imm8} */
  {
    UBICOM32_INSN_MOVE_2_D_DIRECT_S1_IMMEDIATE, "move.2-d-direct-s1-immediate", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 #${d-imm8},#${s1-imm8} */
  {
    UBICOM32_INSN_MOVE_2_D_IMMEDIATE_2_S1_IMMEDIATE, "move.2-d-immediate-2-s1-immediate", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 (${d-An},${d-r}),#${s1-imm8} */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_INDEX_2_S1_IMMEDIATE, "move.2-d-indirect-with-index-2-s1-immediate", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 ${d-imm7-2}(${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_OFFSET_2_S1_IMMEDIATE, "move.2-d-indirect-with-offset-2-s1-immediate", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 (${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_2_S1_IMMEDIATE, "move.2-d-indirect-2-s1-immediate", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 (${d-An})${d-i4-2}++,#${s1-imm8} */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_IMMEDIATE, "move.2-d-indirect-with-post-increment-2-s1-immediate", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 ${d-i4-2}(${d-An})++,#${s1-imm8} */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_IMMEDIATE, "move.2-d-indirect-with-pre-increment-2-s1-immediate", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 ${d-direct-addr},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVE_2_D_DIRECT_S1_INDIRECT_WITH_INDEX_2, "move.2-d-direct-s1-indirect-with-index-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 #${d-imm8},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVE_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_INDEX_2, "move.2-d-immediate-2-s1-indirect-with-index-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 (${d-An},${d-r}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_INDEX_2, "move.2-d-indirect-with-index-2-s1-indirect-with-index-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 ${d-imm7-2}(${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_INDEX_2, "move.2-d-indirect-with-offset-2-s1-indirect-with-index-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 (${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_2_S1_INDIRECT_WITH_INDEX_2, "move.2-d-indirect-2-s1-indirect-with-index-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 (${d-An})${d-i4-2}++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_INDEX_2, "move.2-d-indirect-with-post-increment-2-s1-indirect-with-index-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 ${d-i4-2}(${d-An})++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_INDEX_2, "move.2-d-indirect-with-pre-increment-2-s1-indirect-with-index-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 ${d-direct-addr},${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_2_D_DIRECT_S1_INDIRECT_WITH_OFFSET_2, "move.2-d-direct-s1-indirect-with-offset-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 #${d-imm8},${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_OFFSET_2, "move.2-d-immediate-2-s1-indirect-with-offset-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 (${d-An},${d-r}),${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_OFFSET_2, "move.2-d-indirect-with-index-2-s1-indirect-with-offset-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 ${d-imm7-2}(${d-An}),${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_OFFSET_2, "move.2-d-indirect-with-offset-2-s1-indirect-with-offset-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 (${d-An}),${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_2_S1_INDIRECT_WITH_OFFSET_2, "move.2-d-indirect-2-s1-indirect-with-offset-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 (${d-An})${d-i4-2}++,${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_2, "move.2-d-indirect-with-post-increment-2-s1-indirect-with-offset-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 ${d-i4-2}(${d-An})++,${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_2, "move.2-d-indirect-with-pre-increment-2-s1-indirect-with-offset-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 ${d-direct-addr},(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_2_D_DIRECT_S1_INDIRECT_2, "move.2-d-direct-s1-indirect-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 #${d-imm8},(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_2_D_IMMEDIATE_2_S1_INDIRECT_2, "move.2-d-immediate-2-s1-indirect-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 (${d-An},${d-r}),(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_2, "move.2-d-indirect-with-index-2-s1-indirect-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 ${d-imm7-2}(${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_2, "move.2-d-indirect-with-offset-2-s1-indirect-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 (${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_2_S1_INDIRECT_2, "move.2-d-indirect-2-s1-indirect-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 (${d-An})${d-i4-2}++,(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_2, "move.2-d-indirect-with-post-increment-2-s1-indirect-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 ${d-i4-2}(${d-An})++,(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_2, "move.2-d-indirect-with-pre-increment-2-s1-indirect-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 ${d-direct-addr},(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_MOVE_2_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_2, "move.2-d-direct-s1-indirect-with-post-increment-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 #${d-imm8},(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_MOVE_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "move.2-d-immediate-2-s1-indirect-with-post-increment-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 (${d-An},${d-r}),(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "move.2-d-indirect-with-index-2-s1-indirect-with-post-increment-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 ${d-imm7-2}(${d-An}),(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "move.2-d-indirect-with-offset-2-s1-indirect-with-post-increment-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 (${d-An}),(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "move.2-d-indirect-2-s1-indirect-with-post-increment-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 (${d-An})${d-i4-2}++,(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "move.2-d-indirect-with-post-increment-2-s1-indirect-with-post-increment-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 ${d-i4-2}(${d-An})++,(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "move.2-d-indirect-with-pre-increment-2-s1-indirect-with-post-increment-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 ${d-direct-addr},${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVE_2_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_2, "move.2-d-direct-s1-indirect-with-pre-increment-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 #${d-imm8},${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVE_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "move.2-d-immediate-2-s1-indirect-with-pre-increment-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 (${d-An},${d-r}),${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "move.2-d-indirect-with-index-2-s1-indirect-with-pre-increment-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 ${d-imm7-2}(${d-An}),${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "move.2-d-indirect-with-offset-2-s1-indirect-with-pre-increment-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 (${d-An}),${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "move.2-d-indirect-2-s1-indirect-with-pre-increment-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 (${d-An})${d-i4-2}++,${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "move.2-d-indirect-with-post-increment-2-s1-indirect-with-pre-increment-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.2 ${d-i4-2}(${d-An})++,${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVE_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "move.2-d-indirect-with-pre-increment-2-s1-indirect-with-pre-increment-2", "move.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 ${d-direct-addr},${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVE_1_D_DIRECT_S1_DIRECT, "move.1-d-direct-s1-direct", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 #${d-imm8},${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVE_1_D_IMMEDIATE_1_S1_DIRECT, "move.1-d-immediate-1-s1-direct", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 (${d-An},${d-r}),${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_INDEX_1_S1_DIRECT, "move.1-d-indirect-with-index-1-s1-direct", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 ${d-imm7-1}(${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_OFFSET_1_S1_DIRECT, "move.1-d-indirect-with-offset-1-s1-direct", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 (${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_1_S1_DIRECT, "move.1-d-indirect-1-s1-direct", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 (${d-An})${d-i4-1}++,${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_DIRECT, "move.1-d-indirect-with-post-increment-1-s1-direct", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 ${d-i4-1}(${d-An})++,${s1-direct-addr} */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_DIRECT, "move.1-d-indirect-with-pre-increment-1-s1-direct", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 ${d-direct-addr},#${s1-imm8} */
  {
    UBICOM32_INSN_MOVE_1_D_DIRECT_S1_IMMEDIATE, "move.1-d-direct-s1-immediate", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 #${d-imm8},#${s1-imm8} */
  {
    UBICOM32_INSN_MOVE_1_D_IMMEDIATE_1_S1_IMMEDIATE, "move.1-d-immediate-1-s1-immediate", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 (${d-An},${d-r}),#${s1-imm8} */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_INDEX_1_S1_IMMEDIATE, "move.1-d-indirect-with-index-1-s1-immediate", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 ${d-imm7-1}(${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_OFFSET_1_S1_IMMEDIATE, "move.1-d-indirect-with-offset-1-s1-immediate", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 (${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_1_S1_IMMEDIATE, "move.1-d-indirect-1-s1-immediate", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 (${d-An})${d-i4-1}++,#${s1-imm8} */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_IMMEDIATE, "move.1-d-indirect-with-post-increment-1-s1-immediate", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 ${d-i4-1}(${d-An})++,#${s1-imm8} */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_IMMEDIATE, "move.1-d-indirect-with-pre-increment-1-s1-immediate", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 ${d-direct-addr},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVE_1_D_DIRECT_S1_INDIRECT_WITH_INDEX_1, "move.1-d-direct-s1-indirect-with-index-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 #${d-imm8},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVE_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_INDEX_1, "move.1-d-immediate-1-s1-indirect-with-index-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 (${d-An},${d-r}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_INDEX_1, "move.1-d-indirect-with-index-1-s1-indirect-with-index-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 ${d-imm7-1}(${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_INDEX_1, "move.1-d-indirect-with-offset-1-s1-indirect-with-index-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 (${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_1_S1_INDIRECT_WITH_INDEX_1, "move.1-d-indirect-1-s1-indirect-with-index-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 (${d-An})${d-i4-1}++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_INDEX_1, "move.1-d-indirect-with-post-increment-1-s1-indirect-with-index-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 ${d-i4-1}(${d-An})++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_INDEX_1, "move.1-d-indirect-with-pre-increment-1-s1-indirect-with-index-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 ${d-direct-addr},${s1-imm7-1}(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_1_D_DIRECT_S1_INDIRECT_WITH_OFFSET_1, "move.1-d-direct-s1-indirect-with-offset-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 #${d-imm8},${s1-imm7-1}(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_OFFSET_1, "move.1-d-immediate-1-s1-indirect-with-offset-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 (${d-An},${d-r}),${s1-imm7-1}(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_OFFSET_1, "move.1-d-indirect-with-index-1-s1-indirect-with-offset-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 ${d-imm7-1}(${d-An}),${s1-imm7-1}(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_OFFSET_1, "move.1-d-indirect-with-offset-1-s1-indirect-with-offset-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 (${d-An}),${s1-imm7-1}(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_1_S1_INDIRECT_WITH_OFFSET_1, "move.1-d-indirect-1-s1-indirect-with-offset-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 (${d-An})${d-i4-1}++,${s1-imm7-1}(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_OFFSET_1, "move.1-d-indirect-with-post-increment-1-s1-indirect-with-offset-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 ${d-i4-1}(${d-An})++,${s1-imm7-1}(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_OFFSET_1, "move.1-d-indirect-with-pre-increment-1-s1-indirect-with-offset-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 ${d-direct-addr},(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_1_D_DIRECT_S1_INDIRECT_1, "move.1-d-direct-s1-indirect-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 #${d-imm8},(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_1_D_IMMEDIATE_1_S1_INDIRECT_1, "move.1-d-immediate-1-s1-indirect-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 (${d-An},${d-r}),(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_1, "move.1-d-indirect-with-index-1-s1-indirect-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 ${d-imm7-1}(${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_1, "move.1-d-indirect-with-offset-1-s1-indirect-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 (${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_1_S1_INDIRECT_1, "move.1-d-indirect-1-s1-indirect-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 (${d-An})${d-i4-1}++,(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_1, "move.1-d-indirect-with-post-increment-1-s1-indirect-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 ${d-i4-1}(${d-An})++,(${s1-An}) */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_1, "move.1-d-indirect-with-pre-increment-1-s1-indirect-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 ${d-direct-addr},(${s1-An})${s1-i4-1}++ */
  {
    UBICOM32_INSN_MOVE_1_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_1, "move.1-d-direct-s1-indirect-with-post-increment-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 #${d-imm8},(${s1-An})${s1-i4-1}++ */
  {
    UBICOM32_INSN_MOVE_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "move.1-d-immediate-1-s1-indirect-with-post-increment-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 (${d-An},${d-r}),(${s1-An})${s1-i4-1}++ */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "move.1-d-indirect-with-index-1-s1-indirect-with-post-increment-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 ${d-imm7-1}(${d-An}),(${s1-An})${s1-i4-1}++ */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "move.1-d-indirect-with-offset-1-s1-indirect-with-post-increment-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 (${d-An}),(${s1-An})${s1-i4-1}++ */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "move.1-d-indirect-1-s1-indirect-with-post-increment-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 (${d-An})${d-i4-1}++,(${s1-An})${s1-i4-1}++ */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "move.1-d-indirect-with-post-increment-1-s1-indirect-with-post-increment-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 ${d-i4-1}(${d-An})++,(${s1-An})${s1-i4-1}++ */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "move.1-d-indirect-with-pre-increment-1-s1-indirect-with-post-increment-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 ${d-direct-addr},${s1-i4-1}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVE_1_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_1, "move.1-d-direct-s1-indirect-with-pre-increment-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 #${d-imm8},${s1-i4-1}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVE_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "move.1-d-immediate-1-s1-indirect-with-pre-increment-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 (${d-An},${d-r}),${s1-i4-1}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "move.1-d-indirect-with-index-1-s1-indirect-with-pre-increment-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 ${d-imm7-1}(${d-An}),${s1-i4-1}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "move.1-d-indirect-with-offset-1-s1-indirect-with-pre-increment-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 (${d-An}),${s1-i4-1}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "move.1-d-indirect-1-s1-indirect-with-pre-increment-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 (${d-An})${d-i4-1}++,${s1-i4-1}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "move.1-d-indirect-with-post-increment-1-s1-indirect-with-pre-increment-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* move.1 ${d-i4-1}(${d-An})++,${s1-i4-1}(${s1-An})++ */
  {
    UBICOM32_INSN_MOVE_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "move.1-d-indirect-with-pre-increment-1-s1-indirect-with-pre-increment-1", "move.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 ${d-direct-addr},${s1-direct-addr} */
  {
    UBICOM32_INSN_EXT_2_D_DIRECT_S1_DIRECT, "ext.2-d-direct-s1-direct", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 #${d-imm8},${s1-direct-addr} */
  {
    UBICOM32_INSN_EXT_2_D_IMMEDIATE_2_S1_DIRECT, "ext.2-d-immediate-2-s1-direct", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 (${d-An},${d-r}),${s1-direct-addr} */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_INDEX_2_S1_DIRECT, "ext.2-d-indirect-with-index-2-s1-direct", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 ${d-imm7-2}(${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_OFFSET_2_S1_DIRECT, "ext.2-d-indirect-with-offset-2-s1-direct", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 (${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_2_S1_DIRECT, "ext.2-d-indirect-2-s1-direct", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 (${d-An})${d-i4-2}++,${s1-direct-addr} */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_DIRECT, "ext.2-d-indirect-with-post-increment-2-s1-direct", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 ${d-i4-2}(${d-An})++,${s1-direct-addr} */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_DIRECT, "ext.2-d-indirect-with-pre-increment-2-s1-direct", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 ${d-direct-addr},#${s1-imm8} */
  {
    UBICOM32_INSN_EXT_2_D_DIRECT_S1_IMMEDIATE, "ext.2-d-direct-s1-immediate", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 #${d-imm8},#${s1-imm8} */
  {
    UBICOM32_INSN_EXT_2_D_IMMEDIATE_2_S1_IMMEDIATE, "ext.2-d-immediate-2-s1-immediate", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 (${d-An},${d-r}),#${s1-imm8} */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_INDEX_2_S1_IMMEDIATE, "ext.2-d-indirect-with-index-2-s1-immediate", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 ${d-imm7-2}(${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_OFFSET_2_S1_IMMEDIATE, "ext.2-d-indirect-with-offset-2-s1-immediate", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 (${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_2_S1_IMMEDIATE, "ext.2-d-indirect-2-s1-immediate", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 (${d-An})${d-i4-2}++,#${s1-imm8} */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_IMMEDIATE, "ext.2-d-indirect-with-post-increment-2-s1-immediate", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 ${d-i4-2}(${d-An})++,#${s1-imm8} */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_IMMEDIATE, "ext.2-d-indirect-with-pre-increment-2-s1-immediate", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 ${d-direct-addr},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_EXT_2_D_DIRECT_S1_INDIRECT_WITH_INDEX_2, "ext.2-d-direct-s1-indirect-with-index-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 #${d-imm8},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_EXT_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_INDEX_2, "ext.2-d-immediate-2-s1-indirect-with-index-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 (${d-An},${d-r}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_INDEX_2, "ext.2-d-indirect-with-index-2-s1-indirect-with-index-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 ${d-imm7-2}(${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_INDEX_2, "ext.2-d-indirect-with-offset-2-s1-indirect-with-index-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 (${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_2_S1_INDIRECT_WITH_INDEX_2, "ext.2-d-indirect-2-s1-indirect-with-index-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 (${d-An})${d-i4-2}++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_INDEX_2, "ext.2-d-indirect-with-post-increment-2-s1-indirect-with-index-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 ${d-i4-2}(${d-An})++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_INDEX_2, "ext.2-d-indirect-with-pre-increment-2-s1-indirect-with-index-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 ${d-direct-addr},${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_EXT_2_D_DIRECT_S1_INDIRECT_WITH_OFFSET_2, "ext.2-d-direct-s1-indirect-with-offset-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 #${d-imm8},${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_EXT_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_OFFSET_2, "ext.2-d-immediate-2-s1-indirect-with-offset-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 (${d-An},${d-r}),${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_OFFSET_2, "ext.2-d-indirect-with-index-2-s1-indirect-with-offset-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 ${d-imm7-2}(${d-An}),${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_OFFSET_2, "ext.2-d-indirect-with-offset-2-s1-indirect-with-offset-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 (${d-An}),${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_2_S1_INDIRECT_WITH_OFFSET_2, "ext.2-d-indirect-2-s1-indirect-with-offset-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 (${d-An})${d-i4-2}++,${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_2, "ext.2-d-indirect-with-post-increment-2-s1-indirect-with-offset-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 ${d-i4-2}(${d-An})++,${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_2, "ext.2-d-indirect-with-pre-increment-2-s1-indirect-with-offset-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 ${d-direct-addr},(${s1-An}) */
  {
    UBICOM32_INSN_EXT_2_D_DIRECT_S1_INDIRECT_2, "ext.2-d-direct-s1-indirect-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 #${d-imm8},(${s1-An}) */
  {
    UBICOM32_INSN_EXT_2_D_IMMEDIATE_2_S1_INDIRECT_2, "ext.2-d-immediate-2-s1-indirect-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 (${d-An},${d-r}),(${s1-An}) */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_2, "ext.2-d-indirect-with-index-2-s1-indirect-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 ${d-imm7-2}(${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_2, "ext.2-d-indirect-with-offset-2-s1-indirect-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 (${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_2_S1_INDIRECT_2, "ext.2-d-indirect-2-s1-indirect-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 (${d-An})${d-i4-2}++,(${s1-An}) */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_2, "ext.2-d-indirect-with-post-increment-2-s1-indirect-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 ${d-i4-2}(${d-An})++,(${s1-An}) */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_2, "ext.2-d-indirect-with-pre-increment-2-s1-indirect-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 ${d-direct-addr},(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_EXT_2_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_2, "ext.2-d-direct-s1-indirect-with-post-increment-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 #${d-imm8},(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_EXT_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "ext.2-d-immediate-2-s1-indirect-with-post-increment-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 (${d-An},${d-r}),(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "ext.2-d-indirect-with-index-2-s1-indirect-with-post-increment-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 ${d-imm7-2}(${d-An}),(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "ext.2-d-indirect-with-offset-2-s1-indirect-with-post-increment-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 (${d-An}),(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "ext.2-d-indirect-2-s1-indirect-with-post-increment-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 (${d-An})${d-i4-2}++,(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "ext.2-d-indirect-with-post-increment-2-s1-indirect-with-post-increment-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 ${d-i4-2}(${d-An})++,(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "ext.2-d-indirect-with-pre-increment-2-s1-indirect-with-post-increment-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 ${d-direct-addr},${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_EXT_2_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_2, "ext.2-d-direct-s1-indirect-with-pre-increment-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 #${d-imm8},${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_EXT_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "ext.2-d-immediate-2-s1-indirect-with-pre-increment-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 (${d-An},${d-r}),${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "ext.2-d-indirect-with-index-2-s1-indirect-with-pre-increment-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 ${d-imm7-2}(${d-An}),${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "ext.2-d-indirect-with-offset-2-s1-indirect-with-pre-increment-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 (${d-An}),${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "ext.2-d-indirect-2-s1-indirect-with-pre-increment-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 (${d-An})${d-i4-2}++,${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "ext.2-d-indirect-with-post-increment-2-s1-indirect-with-pre-increment-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.2 ${d-i4-2}(${d-An})++,${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_EXT_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "ext.2-d-indirect-with-pre-increment-2-s1-indirect-with-pre-increment-2", "ext.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 ${d-direct-addr},${s1-direct-addr} */
  {
    UBICOM32_INSN_EXT_1_D_DIRECT_S1_DIRECT, "ext.1-d-direct-s1-direct", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 #${d-imm8},${s1-direct-addr} */
  {
    UBICOM32_INSN_EXT_1_D_IMMEDIATE_1_S1_DIRECT, "ext.1-d-immediate-1-s1-direct", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 (${d-An},${d-r}),${s1-direct-addr} */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_INDEX_1_S1_DIRECT, "ext.1-d-indirect-with-index-1-s1-direct", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 ${d-imm7-1}(${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_OFFSET_1_S1_DIRECT, "ext.1-d-indirect-with-offset-1-s1-direct", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 (${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_1_S1_DIRECT, "ext.1-d-indirect-1-s1-direct", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 (${d-An})${d-i4-1}++,${s1-direct-addr} */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_DIRECT, "ext.1-d-indirect-with-post-increment-1-s1-direct", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 ${d-i4-1}(${d-An})++,${s1-direct-addr} */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_DIRECT, "ext.1-d-indirect-with-pre-increment-1-s1-direct", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 ${d-direct-addr},#${s1-imm8} */
  {
    UBICOM32_INSN_EXT_1_D_DIRECT_S1_IMMEDIATE, "ext.1-d-direct-s1-immediate", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 #${d-imm8},#${s1-imm8} */
  {
    UBICOM32_INSN_EXT_1_D_IMMEDIATE_1_S1_IMMEDIATE, "ext.1-d-immediate-1-s1-immediate", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 (${d-An},${d-r}),#${s1-imm8} */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_INDEX_1_S1_IMMEDIATE, "ext.1-d-indirect-with-index-1-s1-immediate", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 ${d-imm7-1}(${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_OFFSET_1_S1_IMMEDIATE, "ext.1-d-indirect-with-offset-1-s1-immediate", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 (${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_1_S1_IMMEDIATE, "ext.1-d-indirect-1-s1-immediate", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 (${d-An})${d-i4-1}++,#${s1-imm8} */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_IMMEDIATE, "ext.1-d-indirect-with-post-increment-1-s1-immediate", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 ${d-i4-1}(${d-An})++,#${s1-imm8} */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_IMMEDIATE, "ext.1-d-indirect-with-pre-increment-1-s1-immediate", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 ${d-direct-addr},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_EXT_1_D_DIRECT_S1_INDIRECT_WITH_INDEX_1, "ext.1-d-direct-s1-indirect-with-index-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 #${d-imm8},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_EXT_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_INDEX_1, "ext.1-d-immediate-1-s1-indirect-with-index-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 (${d-An},${d-r}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_INDEX_1, "ext.1-d-indirect-with-index-1-s1-indirect-with-index-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 ${d-imm7-1}(${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_INDEX_1, "ext.1-d-indirect-with-offset-1-s1-indirect-with-index-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 (${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_1_S1_INDIRECT_WITH_INDEX_1, "ext.1-d-indirect-1-s1-indirect-with-index-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 (${d-An})${d-i4-1}++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_INDEX_1, "ext.1-d-indirect-with-post-increment-1-s1-indirect-with-index-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 ${d-i4-1}(${d-An})++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_INDEX_1, "ext.1-d-indirect-with-pre-increment-1-s1-indirect-with-index-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 ${d-direct-addr},${s1-imm7-1}(${s1-An}) */
  {
    UBICOM32_INSN_EXT_1_D_DIRECT_S1_INDIRECT_WITH_OFFSET_1, "ext.1-d-direct-s1-indirect-with-offset-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 #${d-imm8},${s1-imm7-1}(${s1-An}) */
  {
    UBICOM32_INSN_EXT_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_OFFSET_1, "ext.1-d-immediate-1-s1-indirect-with-offset-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 (${d-An},${d-r}),${s1-imm7-1}(${s1-An}) */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_OFFSET_1, "ext.1-d-indirect-with-index-1-s1-indirect-with-offset-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 ${d-imm7-1}(${d-An}),${s1-imm7-1}(${s1-An}) */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_OFFSET_1, "ext.1-d-indirect-with-offset-1-s1-indirect-with-offset-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 (${d-An}),${s1-imm7-1}(${s1-An}) */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_1_S1_INDIRECT_WITH_OFFSET_1, "ext.1-d-indirect-1-s1-indirect-with-offset-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 (${d-An})${d-i4-1}++,${s1-imm7-1}(${s1-An}) */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_OFFSET_1, "ext.1-d-indirect-with-post-increment-1-s1-indirect-with-offset-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 ${d-i4-1}(${d-An})++,${s1-imm7-1}(${s1-An}) */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_OFFSET_1, "ext.1-d-indirect-with-pre-increment-1-s1-indirect-with-offset-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 ${d-direct-addr},(${s1-An}) */
  {
    UBICOM32_INSN_EXT_1_D_DIRECT_S1_INDIRECT_1, "ext.1-d-direct-s1-indirect-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 #${d-imm8},(${s1-An}) */
  {
    UBICOM32_INSN_EXT_1_D_IMMEDIATE_1_S1_INDIRECT_1, "ext.1-d-immediate-1-s1-indirect-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 (${d-An},${d-r}),(${s1-An}) */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_1, "ext.1-d-indirect-with-index-1-s1-indirect-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 ${d-imm7-1}(${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_1, "ext.1-d-indirect-with-offset-1-s1-indirect-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 (${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_1_S1_INDIRECT_1, "ext.1-d-indirect-1-s1-indirect-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 (${d-An})${d-i4-1}++,(${s1-An}) */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_1, "ext.1-d-indirect-with-post-increment-1-s1-indirect-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 ${d-i4-1}(${d-An})++,(${s1-An}) */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_1, "ext.1-d-indirect-with-pre-increment-1-s1-indirect-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 ${d-direct-addr},(${s1-An})${s1-i4-1}++ */
  {
    UBICOM32_INSN_EXT_1_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_1, "ext.1-d-direct-s1-indirect-with-post-increment-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 #${d-imm8},(${s1-An})${s1-i4-1}++ */
  {
    UBICOM32_INSN_EXT_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "ext.1-d-immediate-1-s1-indirect-with-post-increment-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 (${d-An},${d-r}),(${s1-An})${s1-i4-1}++ */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "ext.1-d-indirect-with-index-1-s1-indirect-with-post-increment-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 ${d-imm7-1}(${d-An}),(${s1-An})${s1-i4-1}++ */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "ext.1-d-indirect-with-offset-1-s1-indirect-with-post-increment-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 (${d-An}),(${s1-An})${s1-i4-1}++ */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "ext.1-d-indirect-1-s1-indirect-with-post-increment-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 (${d-An})${d-i4-1}++,(${s1-An})${s1-i4-1}++ */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "ext.1-d-indirect-with-post-increment-1-s1-indirect-with-post-increment-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 ${d-i4-1}(${d-An})++,(${s1-An})${s1-i4-1}++ */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "ext.1-d-indirect-with-pre-increment-1-s1-indirect-with-post-increment-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 ${d-direct-addr},${s1-i4-1}(${s1-An})++ */
  {
    UBICOM32_INSN_EXT_1_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_1, "ext.1-d-direct-s1-indirect-with-pre-increment-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 #${d-imm8},${s1-i4-1}(${s1-An})++ */
  {
    UBICOM32_INSN_EXT_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "ext.1-d-immediate-1-s1-indirect-with-pre-increment-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 (${d-An},${d-r}),${s1-i4-1}(${s1-An})++ */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "ext.1-d-indirect-with-index-1-s1-indirect-with-pre-increment-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 ${d-imm7-1}(${d-An}),${s1-i4-1}(${s1-An})++ */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "ext.1-d-indirect-with-offset-1-s1-indirect-with-pre-increment-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 (${d-An}),${s1-i4-1}(${s1-An})++ */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "ext.1-d-indirect-1-s1-indirect-with-pre-increment-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 (${d-An})${d-i4-1}++,${s1-i4-1}(${s1-An})++ */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "ext.1-d-indirect-with-post-increment-1-s1-indirect-with-pre-increment-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* ext.1 ${d-i4-1}(${d-An})++,${s1-i4-1}(${s1-An})++ */
  {
    UBICOM32_INSN_EXT_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "ext.1-d-indirect-with-pre-increment-1-s1-indirect-with-pre-increment-1", "ext.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* movei ${d-direct-addr},#${imm16-2} */
  {
    UBICOM32_INSN_MOVEI_D_DIRECT, "movei-d-direct", "movei", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* movei #${d-imm8},#${imm16-2} */
  {
    UBICOM32_INSN_MOVEI_D_IMMEDIATE_2, "movei-d-immediate-2", "movei", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* movei (${d-An},${d-r}),#${imm16-2} */
  {
    UBICOM32_INSN_MOVEI_D_INDIRECT_WITH_INDEX_2, "movei-d-indirect-with-index-2", "movei", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* movei ${d-imm7-2}(${d-An}),#${imm16-2} */
  {
    UBICOM32_INSN_MOVEI_D_INDIRECT_WITH_OFFSET_2, "movei-d-indirect-with-offset-2", "movei", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* movei (${d-An}),#${imm16-2} */
  {
    UBICOM32_INSN_MOVEI_D_INDIRECT_2, "movei-d-indirect-2", "movei", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* movei (${d-An})${d-i4-2}++,#${imm16-2} */
  {
    UBICOM32_INSN_MOVEI_D_INDIRECT_WITH_POST_INCREMENT_2, "movei-d-indirect-with-post-increment-2", "movei", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* movei ${d-i4-2}(${d-An})++,#${imm16-2} */
  {
    UBICOM32_INSN_MOVEI_D_INDIRECT_WITH_PRE_INCREMENT_2, "movei-d-indirect-with-pre-increment-2", "movei", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr ${d-direct-addr},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_DIRECT_S1_DIRECT, "bclr-d-direct-s1-direct", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr #${d-imm8},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_IMMEDIATE_4_S1_DIRECT, "bclr-d-immediate-4-s1-direct", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr (${d-An},${d-r}),${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_INDEX_4_S1_DIRECT, "bclr-d-indirect-with-index-4-s1-direct", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr ${d-imm7-4}(${d-An}),${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT, "bclr-d-indirect-with-offset-4-s1-direct", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr (${d-An}),${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_4_S1_DIRECT, "bclr-d-indirect-4-s1-direct", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr (${d-An})${d-i4-4}++,${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_POST_INCREMENT_4_S1_DIRECT, "bclr-d-indirect-with-post-increment-4-s1-direct", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr ${d-i4-4}(${d-An})++,${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT, "bclr-d-indirect-with-pre-increment-4-s1-direct", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr ${d-direct-addr},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_DIRECT_S1_IMMEDIATE, "bclr-d-direct-s1-immediate", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr #${d-imm8},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_IMMEDIATE_4_S1_IMMEDIATE, "bclr-d-immediate-4-s1-immediate", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr (${d-An},${d-r}),#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_INDEX_4_S1_IMMEDIATE, "bclr-d-indirect-with-index-4-s1-immediate", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr ${d-imm7-4}(${d-An}),#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_OFFSET_4_S1_IMMEDIATE, "bclr-d-indirect-with-offset-4-s1-immediate", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr (${d-An}),#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_4_S1_IMMEDIATE, "bclr-d-indirect-4-s1-immediate", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr (${d-An})${d-i4-4}++,#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_POST_INCREMENT_4_S1_IMMEDIATE, "bclr-d-indirect-with-post-increment-4-s1-immediate", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr ${d-i4-4}(${d-An})++,#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_IMMEDIATE, "bclr-d-indirect-with-pre-increment-4-s1-immediate", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr ${d-direct-addr},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "bclr-d-direct-s1-indirect-with-index-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr #${d-imm8},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_IMMEDIATE_4_S1_INDIRECT_WITH_INDEX_4, "bclr-d-immediate-4-s1-indirect-with-index-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr (${d-An},${d-r}),(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_INDEX_4, "bclr-d-indirect-with-index-4-s1-indirect-with-index-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_INDEX_4, "bclr-d-indirect-with-offset-4-s1-indirect-with-index-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr (${d-An}),(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_4_S1_INDIRECT_WITH_INDEX_4, "bclr-d-indirect-4-s1-indirect-with-index-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr (${d-An})${d-i4-4}++,(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "bclr-d-indirect-with-post-increment-4-s1-indirect-with-index-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "bclr-d-indirect-with-pre-increment-4-s1-indirect-with-index-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr ${d-direct-addr},${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "bclr-d-direct-s1-indirect-with-offset-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr #${d-imm8},${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_IMMEDIATE_4_S1_INDIRECT_WITH_OFFSET_4, "bclr-d-immediate-4-s1-indirect-with-offset-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr (${d-An},${d-r}),${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_OFFSET_4, "bclr-d-indirect-with-index-4-s1-indirect-with-offset-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_OFFSET_4, "bclr-d-indirect-with-offset-4-s1-indirect-with-offset-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr (${d-An}),${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_4_S1_INDIRECT_WITH_OFFSET_4, "bclr-d-indirect-4-s1-indirect-with-offset-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "bclr-d-indirect-with-post-increment-4-s1-indirect-with-offset-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "bclr-d-indirect-with-pre-increment-4-s1-indirect-with-offset-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr ${d-direct-addr},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_DIRECT_S1_INDIRECT_4, "bclr-d-direct-s1-indirect-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr #${d-imm8},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_IMMEDIATE_4_S1_INDIRECT_4, "bclr-d-immediate-4-s1-indirect-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr (${d-An},${d-r}),(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_4, "bclr-d-indirect-with-index-4-s1-indirect-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr ${d-imm7-4}(${d-An}),(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_4, "bclr-d-indirect-with-offset-4-s1-indirect-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr (${d-An}),(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_4_S1_INDIRECT_4, "bclr-d-indirect-4-s1-indirect-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr (${d-An})${d-i4-4}++,(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_4, "bclr-d-indirect-with-post-increment-4-s1-indirect-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr ${d-i4-4}(${d-An})++,(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_4, "bclr-d-indirect-with-pre-increment-4-s1-indirect-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr ${d-direct-addr},(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "bclr-d-direct-s1-indirect-with-post-increment-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr #${d-imm8},(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_IMMEDIATE_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "bclr-d-immediate-4-s1-indirect-with-post-increment-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr (${d-An},${d-r}),(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "bclr-d-indirect-with-index-4-s1-indirect-with-post-increment-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "bclr-d-indirect-with-offset-4-s1-indirect-with-post-increment-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr (${d-An}),(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "bclr-d-indirect-4-s1-indirect-with-post-increment-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "bclr-d-indirect-with-post-increment-4-s1-indirect-with-post-increment-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "bclr-d-indirect-with-pre-increment-4-s1-indirect-with-post-increment-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr ${d-direct-addr},${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "bclr-d-direct-s1-indirect-with-pre-increment-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr #${d-imm8},${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_IMMEDIATE_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "bclr-d-immediate-4-s1-indirect-with-pre-increment-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr (${d-An},${d-r}),${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "bclr-d-indirect-with-index-4-s1-indirect-with-pre-increment-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "bclr-d-indirect-with-offset-4-s1-indirect-with-pre-increment-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr (${d-An}),${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "bclr-d-indirect-4-s1-indirect-with-pre-increment-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "bclr-d-indirect-with-post-increment-4-s1-indirect-with-pre-increment-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bclr ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_BCLR_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "bclr-d-indirect-with-pre-increment-4-s1-indirect-with-pre-increment-4", "bclr", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset ${d-direct-addr},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_BSET_D_DIRECT_S1_DIRECT, "bset-d-direct-s1-direct", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset #${d-imm8},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_BSET_D_IMMEDIATE_4_S1_DIRECT, "bset-d-immediate-4-s1-direct", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset (${d-An},${d-r}),${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_INDEX_4_S1_DIRECT, "bset-d-indirect-with-index-4-s1-direct", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset ${d-imm7-4}(${d-An}),${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT, "bset-d-indirect-with-offset-4-s1-direct", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset (${d-An}),${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_4_S1_DIRECT, "bset-d-indirect-4-s1-direct", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset (${d-An})${d-i4-4}++,${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_POST_INCREMENT_4_S1_DIRECT, "bset-d-indirect-with-post-increment-4-s1-direct", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset ${d-i4-4}(${d-An})++,${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT, "bset-d-indirect-with-pre-increment-4-s1-direct", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset ${d-direct-addr},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_BSET_D_DIRECT_S1_IMMEDIATE, "bset-d-direct-s1-immediate", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset #${d-imm8},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_BSET_D_IMMEDIATE_4_S1_IMMEDIATE, "bset-d-immediate-4-s1-immediate", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset (${d-An},${d-r}),#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_INDEX_4_S1_IMMEDIATE, "bset-d-indirect-with-index-4-s1-immediate", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset ${d-imm7-4}(${d-An}),#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_OFFSET_4_S1_IMMEDIATE, "bset-d-indirect-with-offset-4-s1-immediate", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset (${d-An}),#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_4_S1_IMMEDIATE, "bset-d-indirect-4-s1-immediate", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset (${d-An})${d-i4-4}++,#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_POST_INCREMENT_4_S1_IMMEDIATE, "bset-d-indirect-with-post-increment-4-s1-immediate", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset ${d-i4-4}(${d-An})++,#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_IMMEDIATE, "bset-d-indirect-with-pre-increment-4-s1-immediate", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset ${d-direct-addr},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_BSET_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "bset-d-direct-s1-indirect-with-index-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset #${d-imm8},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_BSET_D_IMMEDIATE_4_S1_INDIRECT_WITH_INDEX_4, "bset-d-immediate-4-s1-indirect-with-index-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset (${d-An},${d-r}),(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_INDEX_4, "bset-d-indirect-with-index-4-s1-indirect-with-index-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_INDEX_4, "bset-d-indirect-with-offset-4-s1-indirect-with-index-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset (${d-An}),(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_4_S1_INDIRECT_WITH_INDEX_4, "bset-d-indirect-4-s1-indirect-with-index-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset (${d-An})${d-i4-4}++,(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "bset-d-indirect-with-post-increment-4-s1-indirect-with-index-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "bset-d-indirect-with-pre-increment-4-s1-indirect-with-index-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset ${d-direct-addr},${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BSET_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "bset-d-direct-s1-indirect-with-offset-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset #${d-imm8},${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BSET_D_IMMEDIATE_4_S1_INDIRECT_WITH_OFFSET_4, "bset-d-immediate-4-s1-indirect-with-offset-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset (${d-An},${d-r}),${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_OFFSET_4, "bset-d-indirect-with-index-4-s1-indirect-with-offset-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_OFFSET_4, "bset-d-indirect-with-offset-4-s1-indirect-with-offset-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset (${d-An}),${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_4_S1_INDIRECT_WITH_OFFSET_4, "bset-d-indirect-4-s1-indirect-with-offset-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "bset-d-indirect-with-post-increment-4-s1-indirect-with-offset-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "bset-d-indirect-with-pre-increment-4-s1-indirect-with-offset-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset ${d-direct-addr},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BSET_D_DIRECT_S1_INDIRECT_4, "bset-d-direct-s1-indirect-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset #${d-imm8},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BSET_D_IMMEDIATE_4_S1_INDIRECT_4, "bset-d-immediate-4-s1-indirect-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset (${d-An},${d-r}),(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_4, "bset-d-indirect-with-index-4-s1-indirect-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset ${d-imm7-4}(${d-An}),(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_4, "bset-d-indirect-with-offset-4-s1-indirect-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset (${d-An}),(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_4_S1_INDIRECT_4, "bset-d-indirect-4-s1-indirect-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset (${d-An})${d-i4-4}++,(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_4, "bset-d-indirect-with-post-increment-4-s1-indirect-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset ${d-i4-4}(${d-An})++,(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_4, "bset-d-indirect-with-pre-increment-4-s1-indirect-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset ${d-direct-addr},(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_BSET_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "bset-d-direct-s1-indirect-with-post-increment-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset #${d-imm8},(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_BSET_D_IMMEDIATE_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "bset-d-immediate-4-s1-indirect-with-post-increment-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset (${d-An},${d-r}),(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "bset-d-indirect-with-index-4-s1-indirect-with-post-increment-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "bset-d-indirect-with-offset-4-s1-indirect-with-post-increment-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset (${d-An}),(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "bset-d-indirect-4-s1-indirect-with-post-increment-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "bset-d-indirect-with-post-increment-4-s1-indirect-with-post-increment-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "bset-d-indirect-with-pre-increment-4-s1-indirect-with-post-increment-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset ${d-direct-addr},${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_BSET_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "bset-d-direct-s1-indirect-with-pre-increment-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset #${d-imm8},${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_BSET_D_IMMEDIATE_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "bset-d-immediate-4-s1-indirect-with-pre-increment-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset (${d-An},${d-r}),${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "bset-d-indirect-with-index-4-s1-indirect-with-pre-increment-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "bset-d-indirect-with-offset-4-s1-indirect-with-pre-increment-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset (${d-An}),${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "bset-d-indirect-4-s1-indirect-with-pre-increment-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "bset-d-indirect-with-post-increment-4-s1-indirect-with-pre-increment-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bset ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_BSET_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "bset-d-indirect-with-pre-increment-4-s1-indirect-with-pre-increment-4", "bset", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* btst ${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_BTST_S1_DIRECT_IMM_BIT5, "btst-s1-direct-imm-bit5", "btst", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* btst #${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_BTST_S1_IMMEDIATE_IMM_BIT5, "btst-s1-immediate-imm-bit5", "btst", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* btst (${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_BTST_S1_INDIRECT_WITH_INDEX_4_IMM_BIT5, "btst-s1-indirect-with-index-4-imm-bit5", "btst", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* btst ${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BTST_S1_INDIRECT_WITH_OFFSET_4_IMM_BIT5, "btst-s1-indirect-with-offset-4-imm-bit5", "btst", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* btst (${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BTST_S1_INDIRECT_4_IMM_BIT5, "btst-s1-indirect-4-imm-bit5", "btst", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* btst (${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_BTST_S1_INDIRECT_WITH_POST_INCREMENT_4_IMM_BIT5, "btst-s1-indirect-with-post-increment-4-imm-bit5", "btst", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* btst ${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_BTST_S1_INDIRECT_WITH_PRE_INCREMENT_4_IMM_BIT5, "btst-s1-indirect-with-pre-increment-4-imm-bit5", "btst", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* btst ${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_BTST_S1_DIRECT_DYN_REG, "btst-s1-direct-dyn-reg", "btst", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* btst #${s1-imm8},${s2} */
  {
    UBICOM32_INSN_BTST_S1_IMMEDIATE_DYN_REG, "btst-s1-immediate-dyn-reg", "btst", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* btst (${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_BTST_S1_INDIRECT_WITH_INDEX_4_DYN_REG, "btst-s1-indirect-with-index-4-dyn-reg", "btst", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* btst ${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_BTST_S1_INDIRECT_WITH_OFFSET_4_DYN_REG, "btst-s1-indirect-with-offset-4-dyn-reg", "btst", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* btst (${s1-An}),${s2} */
  {
    UBICOM32_INSN_BTST_S1_INDIRECT_4_DYN_REG, "btst-s1-indirect-4-dyn-reg", "btst", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* btst (${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_BTST_S1_INDIRECT_WITH_POST_INCREMENT_4_DYN_REG, "btst-s1-indirect-with-post-increment-4-dyn-reg", "btst", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* btst ${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_BTST_S1_INDIRECT_WITH_PRE_INCREMENT_4_DYN_REG, "btst-s1-indirect-with-pre-increment-4-dyn-reg", "btst", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.2 ${Dn},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_SHMRG_2_IMM_BIT5_S1_DIRECT, "shmrg.2-imm-bit5-s1-direct", "shmrg.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.2 ${Dn},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SHMRG_2_DYN_REG_S1_DIRECT, "shmrg.2-dyn-reg-s1-direct", "shmrg.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.2 ${Dn},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_SHMRG_2_IMM_BIT5_S1_IMMEDIATE, "shmrg.2-imm-bit5-s1-immediate", "shmrg.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.2 ${Dn},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SHMRG_2_DYN_REG_S1_IMMEDIATE, "shmrg.2-dyn-reg-s1-immediate", "shmrg.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.2 ${Dn},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_SHMRG_2_IMM_BIT5_S1_INDIRECT_WITH_INDEX_2, "shmrg.2-imm-bit5-s1-indirect-with-index-2", "shmrg.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.2 ${Dn},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SHMRG_2_DYN_REG_S1_INDIRECT_WITH_INDEX_2, "shmrg.2-dyn-reg-s1-indirect-with-index-2", "shmrg.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.2 ${Dn},${s1-imm7-2}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_SHMRG_2_IMM_BIT5_S1_INDIRECT_WITH_OFFSET_2, "shmrg.2-imm-bit5-s1-indirect-with-offset-2", "shmrg.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.2 ${Dn},${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SHMRG_2_DYN_REG_S1_INDIRECT_WITH_OFFSET_2, "shmrg.2-dyn-reg-s1-indirect-with-offset-2", "shmrg.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.2 ${Dn},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_SHMRG_2_IMM_BIT5_S1_INDIRECT_2, "shmrg.2-imm-bit5-s1-indirect-2", "shmrg.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.2 ${Dn},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SHMRG_2_DYN_REG_S1_INDIRECT_2, "shmrg.2-dyn-reg-s1-indirect-2", "shmrg.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.2 ${Dn},(${s1-An})${s1-i4-2}++,#${bit5} */
  {
    UBICOM32_INSN_SHMRG_2_IMM_BIT5_S1_INDIRECT_WITH_POST_INCREMENT_2, "shmrg.2-imm-bit5-s1-indirect-with-post-increment-2", "shmrg.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.2 ${Dn},(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_SHMRG_2_DYN_REG_S1_INDIRECT_WITH_POST_INCREMENT_2, "shmrg.2-dyn-reg-s1-indirect-with-post-increment-2", "shmrg.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.2 ${Dn},${s1-i4-2}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_SHMRG_2_IMM_BIT5_S1_INDIRECT_WITH_PRE_INCREMENT_2, "shmrg.2-imm-bit5-s1-indirect-with-pre-increment-2", "shmrg.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.2 ${Dn},${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SHMRG_2_DYN_REG_S1_INDIRECT_WITH_PRE_INCREMENT_2, "shmrg.2-dyn-reg-s1-indirect-with-pre-increment-2", "shmrg.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.1 ${Dn},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_SHMRG_1_IMM_BIT5_S1_DIRECT, "shmrg.1-imm-bit5-s1-direct", "shmrg.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.1 ${Dn},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SHMRG_1_DYN_REG_S1_DIRECT, "shmrg.1-dyn-reg-s1-direct", "shmrg.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.1 ${Dn},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_SHMRG_1_IMM_BIT5_S1_IMMEDIATE, "shmrg.1-imm-bit5-s1-immediate", "shmrg.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.1 ${Dn},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SHMRG_1_DYN_REG_S1_IMMEDIATE, "shmrg.1-dyn-reg-s1-immediate", "shmrg.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.1 ${Dn},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_SHMRG_1_IMM_BIT5_S1_INDIRECT_WITH_INDEX_1, "shmrg.1-imm-bit5-s1-indirect-with-index-1", "shmrg.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.1 ${Dn},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SHMRG_1_DYN_REG_S1_INDIRECT_WITH_INDEX_1, "shmrg.1-dyn-reg-s1-indirect-with-index-1", "shmrg.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.1 ${Dn},${s1-imm7-1}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_SHMRG_1_IMM_BIT5_S1_INDIRECT_WITH_OFFSET_1, "shmrg.1-imm-bit5-s1-indirect-with-offset-1", "shmrg.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.1 ${Dn},${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SHMRG_1_DYN_REG_S1_INDIRECT_WITH_OFFSET_1, "shmrg.1-dyn-reg-s1-indirect-with-offset-1", "shmrg.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.1 ${Dn},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_SHMRG_1_IMM_BIT5_S1_INDIRECT_1, "shmrg.1-imm-bit5-s1-indirect-1", "shmrg.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.1 ${Dn},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SHMRG_1_DYN_REG_S1_INDIRECT_1, "shmrg.1-dyn-reg-s1-indirect-1", "shmrg.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.1 ${Dn},(${s1-An})${s1-i4-1}++,#${bit5} */
  {
    UBICOM32_INSN_SHMRG_1_IMM_BIT5_S1_INDIRECT_WITH_POST_INCREMENT_1, "shmrg.1-imm-bit5-s1-indirect-with-post-increment-1", "shmrg.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.1 ${Dn},(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_SHMRG_1_DYN_REG_S1_INDIRECT_WITH_POST_INCREMENT_1, "shmrg.1-dyn-reg-s1-indirect-with-post-increment-1", "shmrg.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.1 ${Dn},${s1-i4-1}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_SHMRG_1_IMM_BIT5_S1_INDIRECT_WITH_PRE_INCREMENT_1, "shmrg.1-imm-bit5-s1-indirect-with-pre-increment-1", "shmrg.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shmrg.1 ${Dn},${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SHMRG_1_DYN_REG_S1_INDIRECT_WITH_PRE_INCREMENT_1, "shmrg.1-dyn-reg-s1-indirect-with-pre-increment-1", "shmrg.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* crcgen ${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_CRCGEN_S1_DIRECT_IMM_BIT5, "crcgen-s1-direct-imm-bit5", "crcgen", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* crcgen #${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_CRCGEN_S1_IMMEDIATE_IMM_BIT5, "crcgen-s1-immediate-imm-bit5", "crcgen", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* crcgen (${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_CRCGEN_S1_INDIRECT_WITH_INDEX_1_IMM_BIT5, "crcgen-s1-indirect-with-index-1-imm-bit5", "crcgen", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* crcgen ${s1-imm7-1}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_CRCGEN_S1_INDIRECT_WITH_OFFSET_1_IMM_BIT5, "crcgen-s1-indirect-with-offset-1-imm-bit5", "crcgen", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* crcgen (${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_CRCGEN_S1_INDIRECT_1_IMM_BIT5, "crcgen-s1-indirect-1-imm-bit5", "crcgen", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* crcgen (${s1-An})${s1-i4-1}++,#${bit5} */
  {
    UBICOM32_INSN_CRCGEN_S1_INDIRECT_WITH_POST_INCREMENT_1_IMM_BIT5, "crcgen-s1-indirect-with-post-increment-1-imm-bit5", "crcgen", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* crcgen ${s1-i4-1}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_CRCGEN_S1_INDIRECT_WITH_PRE_INCREMENT_1_IMM_BIT5, "crcgen-s1-indirect-with-pre-increment-1-imm-bit5", "crcgen", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* crcgen ${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_CRCGEN_S1_DIRECT_DYN_REG, "crcgen-s1-direct-dyn-reg", "crcgen", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* crcgen #${s1-imm8},${s2} */
  {
    UBICOM32_INSN_CRCGEN_S1_IMMEDIATE_DYN_REG, "crcgen-s1-immediate-dyn-reg", "crcgen", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* crcgen (${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_CRCGEN_S1_INDIRECT_WITH_INDEX_1_DYN_REG, "crcgen-s1-indirect-with-index-1-dyn-reg", "crcgen", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* crcgen ${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_CRCGEN_S1_INDIRECT_WITH_OFFSET_1_DYN_REG, "crcgen-s1-indirect-with-offset-1-dyn-reg", "crcgen", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* crcgen (${s1-An}),${s2} */
  {
    UBICOM32_INSN_CRCGEN_S1_INDIRECT_1_DYN_REG, "crcgen-s1-indirect-1-dyn-reg", "crcgen", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* crcgen (${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_CRCGEN_S1_INDIRECT_WITH_POST_INCREMENT_1_DYN_REG, "crcgen-s1-indirect-with-post-increment-1-dyn-reg", "crcgen", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* crcgen ${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_CRCGEN_S1_INDIRECT_WITH_PRE_INCREMENT_1_DYN_REG, "crcgen-s1-indirect-with-pre-increment-1-dyn-reg", "crcgen", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfextu ${Dn},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_BFEXTU_S1_DIRECT_IMM_BIT5, "bfextu-s1-direct-imm-bit5", "bfextu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfextu ${Dn},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_BFEXTU_S1_IMMEDIATE_IMM_BIT5, "bfextu-s1-immediate-imm-bit5", "bfextu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfextu ${Dn},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_BFEXTU_S1_INDIRECT_WITH_INDEX_4_IMM_BIT5, "bfextu-s1-indirect-with-index-4-imm-bit5", "bfextu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfextu ${Dn},${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BFEXTU_S1_INDIRECT_WITH_OFFSET_4_IMM_BIT5, "bfextu-s1-indirect-with-offset-4-imm-bit5", "bfextu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfextu ${Dn},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BFEXTU_S1_INDIRECT_4_IMM_BIT5, "bfextu-s1-indirect-4-imm-bit5", "bfextu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfextu ${Dn},(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_BFEXTU_S1_INDIRECT_WITH_POST_INCREMENT_4_IMM_BIT5, "bfextu-s1-indirect-with-post-increment-4-imm-bit5", "bfextu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfextu ${Dn},${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_BFEXTU_S1_INDIRECT_WITH_PRE_INCREMENT_4_IMM_BIT5, "bfextu-s1-indirect-with-pre-increment-4-imm-bit5", "bfextu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfextu ${Dn},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_BFEXTU_S1_DIRECT_DYN_REG, "bfextu-s1-direct-dyn-reg", "bfextu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfextu ${Dn},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_BFEXTU_S1_IMMEDIATE_DYN_REG, "bfextu-s1-immediate-dyn-reg", "bfextu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfextu ${Dn},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_BFEXTU_S1_INDIRECT_WITH_INDEX_4_DYN_REG, "bfextu-s1-indirect-with-index-4-dyn-reg", "bfextu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfextu ${Dn},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_BFEXTU_S1_INDIRECT_WITH_OFFSET_4_DYN_REG, "bfextu-s1-indirect-with-offset-4-dyn-reg", "bfextu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfextu ${Dn},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_BFEXTU_S1_INDIRECT_4_DYN_REG, "bfextu-s1-indirect-4-dyn-reg", "bfextu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfextu ${Dn},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_BFEXTU_S1_INDIRECT_WITH_POST_INCREMENT_4_DYN_REG, "bfextu-s1-indirect-with-post-increment-4-dyn-reg", "bfextu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfextu ${Dn},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_BFEXTU_S1_INDIRECT_WITH_PRE_INCREMENT_4_DYN_REG, "bfextu-s1-indirect-with-pre-increment-4-dyn-reg", "bfextu", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfrvrs ${Dn},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_BFRVRS_S1_DIRECT_IMM_BIT5, "bfrvrs-s1-direct-imm-bit5", "bfrvrs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfrvrs ${Dn},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_BFRVRS_S1_IMMEDIATE_IMM_BIT5, "bfrvrs-s1-immediate-imm-bit5", "bfrvrs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfrvrs ${Dn},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_BFRVRS_S1_INDIRECT_WITH_INDEX_4_IMM_BIT5, "bfrvrs-s1-indirect-with-index-4-imm-bit5", "bfrvrs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfrvrs ${Dn},${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BFRVRS_S1_INDIRECT_WITH_OFFSET_4_IMM_BIT5, "bfrvrs-s1-indirect-with-offset-4-imm-bit5", "bfrvrs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfrvrs ${Dn},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_BFRVRS_S1_INDIRECT_4_IMM_BIT5, "bfrvrs-s1-indirect-4-imm-bit5", "bfrvrs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfrvrs ${Dn},(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_BFRVRS_S1_INDIRECT_WITH_POST_INCREMENT_4_IMM_BIT5, "bfrvrs-s1-indirect-with-post-increment-4-imm-bit5", "bfrvrs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfrvrs ${Dn},${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_BFRVRS_S1_INDIRECT_WITH_PRE_INCREMENT_4_IMM_BIT5, "bfrvrs-s1-indirect-with-pre-increment-4-imm-bit5", "bfrvrs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfrvrs ${Dn},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_BFRVRS_S1_DIRECT_DYN_REG, "bfrvrs-s1-direct-dyn-reg", "bfrvrs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfrvrs ${Dn},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_BFRVRS_S1_IMMEDIATE_DYN_REG, "bfrvrs-s1-immediate-dyn-reg", "bfrvrs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfrvrs ${Dn},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_BFRVRS_S1_INDIRECT_WITH_INDEX_4_DYN_REG, "bfrvrs-s1-indirect-with-index-4-dyn-reg", "bfrvrs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfrvrs ${Dn},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_BFRVRS_S1_INDIRECT_WITH_OFFSET_4_DYN_REG, "bfrvrs-s1-indirect-with-offset-4-dyn-reg", "bfrvrs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfrvrs ${Dn},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_BFRVRS_S1_INDIRECT_4_DYN_REG, "bfrvrs-s1-indirect-4-dyn-reg", "bfrvrs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfrvrs ${Dn},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_BFRVRS_S1_INDIRECT_WITH_POST_INCREMENT_4_DYN_REG, "bfrvrs-s1-indirect-with-post-increment-4-dyn-reg", "bfrvrs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* bfrvrs ${Dn},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_BFRVRS_S1_INDIRECT_WITH_PRE_INCREMENT_4_DYN_REG, "bfrvrs-s1-indirect-with-pre-increment-4-dyn-reg", "bfrvrs", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* merge ${Dn},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_MERGE_S1_DIRECT_IMM_BIT5, "merge-s1-direct-imm-bit5", "merge", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* merge ${Dn},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_MERGE_S1_IMMEDIATE_IMM_BIT5, "merge-s1-immediate-imm-bit5", "merge", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* merge ${Dn},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_MERGE_S1_INDIRECT_WITH_INDEX_4_IMM_BIT5, "merge-s1-indirect-with-index-4-imm-bit5", "merge", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* merge ${Dn},${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_MERGE_S1_INDIRECT_WITH_OFFSET_4_IMM_BIT5, "merge-s1-indirect-with-offset-4-imm-bit5", "merge", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* merge ${Dn},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_MERGE_S1_INDIRECT_4_IMM_BIT5, "merge-s1-indirect-4-imm-bit5", "merge", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* merge ${Dn},(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_MERGE_S1_INDIRECT_WITH_POST_INCREMENT_4_IMM_BIT5, "merge-s1-indirect-with-post-increment-4-imm-bit5", "merge", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* merge ${Dn},${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_MERGE_S1_INDIRECT_WITH_PRE_INCREMENT_4_IMM_BIT5, "merge-s1-indirect-with-pre-increment-4-imm-bit5", "merge", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* merge ${Dn},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_MERGE_S1_DIRECT_DYN_REG, "merge-s1-direct-dyn-reg", "merge", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* merge ${Dn},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_MERGE_S1_IMMEDIATE_DYN_REG, "merge-s1-immediate-dyn-reg", "merge", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* merge ${Dn},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_MERGE_S1_INDIRECT_WITH_INDEX_4_DYN_REG, "merge-s1-indirect-with-index-4-dyn-reg", "merge", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* merge ${Dn},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_MERGE_S1_INDIRECT_WITH_OFFSET_4_DYN_REG, "merge-s1-indirect-with-offset-4-dyn-reg", "merge", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* merge ${Dn},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_MERGE_S1_INDIRECT_4_DYN_REG, "merge-s1-indirect-4-dyn-reg", "merge", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* merge ${Dn},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_MERGE_S1_INDIRECT_WITH_POST_INCREMENT_4_DYN_REG, "merge-s1-indirect-with-post-increment-4-dyn-reg", "merge", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* merge ${Dn},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_MERGE_S1_INDIRECT_WITH_PRE_INCREMENT_4_DYN_REG, "merge-s1-indirect-with-pre-increment-4-dyn-reg", "merge", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shftd ${Dn},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_SHFTD_S1_DIRECT_IMM_BIT5, "shftd-s1-direct-imm-bit5", "shftd", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shftd ${Dn},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_SHFTD_S1_IMMEDIATE_IMM_BIT5, "shftd-s1-immediate-imm-bit5", "shftd", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shftd ${Dn},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_SHFTD_S1_INDIRECT_WITH_INDEX_4_IMM_BIT5, "shftd-s1-indirect-with-index-4-imm-bit5", "shftd", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shftd ${Dn},${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_SHFTD_S1_INDIRECT_WITH_OFFSET_4_IMM_BIT5, "shftd-s1-indirect-with-offset-4-imm-bit5", "shftd", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shftd ${Dn},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_SHFTD_S1_INDIRECT_4_IMM_BIT5, "shftd-s1-indirect-4-imm-bit5", "shftd", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shftd ${Dn},(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_SHFTD_S1_INDIRECT_WITH_POST_INCREMENT_4_IMM_BIT5, "shftd-s1-indirect-with-post-increment-4-imm-bit5", "shftd", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shftd ${Dn},${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_SHFTD_S1_INDIRECT_WITH_PRE_INCREMENT_4_IMM_BIT5, "shftd-s1-indirect-with-pre-increment-4-imm-bit5", "shftd", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shftd ${Dn},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SHFTD_S1_DIRECT_DYN_REG, "shftd-s1-direct-dyn-reg", "shftd", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shftd ${Dn},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SHFTD_S1_IMMEDIATE_DYN_REG, "shftd-s1-immediate-dyn-reg", "shftd", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shftd ${Dn},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SHFTD_S1_INDIRECT_WITH_INDEX_4_DYN_REG, "shftd-s1-indirect-with-index-4-dyn-reg", "shftd", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shftd ${Dn},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SHFTD_S1_INDIRECT_WITH_OFFSET_4_DYN_REG, "shftd-s1-indirect-with-offset-4-dyn-reg", "shftd", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shftd ${Dn},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SHFTD_S1_INDIRECT_4_DYN_REG, "shftd-s1-indirect-4-dyn-reg", "shftd", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shftd ${Dn},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_SHFTD_S1_INDIRECT_WITH_POST_INCREMENT_4_DYN_REG, "shftd-s1-indirect-with-post-increment-4-dyn-reg", "shftd", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* shftd ${Dn},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SHFTD_S1_INDIRECT_WITH_PRE_INCREMENT_4_DYN_REG, "shftd-s1-indirect-with-pre-increment-4-dyn-reg", "shftd", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.1 ${Dn},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_ASR_1_IMM_BIT5_S1_DIRECT, "asr.1-imm-bit5-s1-direct", "asr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* asr.1 ${Dn},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ASR_1_DYN_REG_S1_DIRECT, "asr.1-dyn-reg-s1-direct", "asr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* asr.1 ${Dn},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_ASR_1_IMM_BIT5_S1_IMMEDIATE, "asr.1-imm-bit5-s1-immediate", "asr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* asr.1 ${Dn},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ASR_1_DYN_REG_S1_IMMEDIATE, "asr.1-dyn-reg-s1-immediate", "asr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* asr.1 ${Dn},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_ASR_1_IMM_BIT5_S1_INDIRECT_WITH_INDEX_1, "asr.1-imm-bit5-s1-indirect-with-index-1", "asr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* asr.1 ${Dn},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ASR_1_DYN_REG_S1_INDIRECT_WITH_INDEX_1, "asr.1-dyn-reg-s1-indirect-with-index-1", "asr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* asr.1 ${Dn},${s1-imm7-1}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_ASR_1_IMM_BIT5_S1_INDIRECT_WITH_OFFSET_1, "asr.1-imm-bit5-s1-indirect-with-offset-1", "asr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* asr.1 ${Dn},${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ASR_1_DYN_REG_S1_INDIRECT_WITH_OFFSET_1, "asr.1-dyn-reg-s1-indirect-with-offset-1", "asr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* asr.1 ${Dn},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_ASR_1_IMM_BIT5_S1_INDIRECT_1, "asr.1-imm-bit5-s1-indirect-1", "asr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* asr.1 ${Dn},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ASR_1_DYN_REG_S1_INDIRECT_1, "asr.1-dyn-reg-s1-indirect-1", "asr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* asr.1 ${Dn},(${s1-An})${s1-i4-1}++,#${bit5} */
  {
    UBICOM32_INSN_ASR_1_IMM_BIT5_S1_INDIRECT_WITH_POST_INCREMENT_1, "asr.1-imm-bit5-s1-indirect-with-post-increment-1", "asr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* asr.1 ${Dn},(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_ASR_1_DYN_REG_S1_INDIRECT_WITH_POST_INCREMENT_1, "asr.1-dyn-reg-s1-indirect-with-post-increment-1", "asr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* asr.1 ${Dn},${s1-i4-1}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_ASR_1_IMM_BIT5_S1_INDIRECT_WITH_PRE_INCREMENT_1, "asr.1-imm-bit5-s1-indirect-with-pre-increment-1", "asr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* asr.1 ${Dn},${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ASR_1_DYN_REG_S1_INDIRECT_WITH_PRE_INCREMENT_1, "asr.1-dyn-reg-s1-indirect-with-pre-increment-1", "asr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsl.1 ${Dn},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_LSL_1_IMM_BIT5_S1_DIRECT, "lsl.1-imm-bit5-s1-direct", "lsl.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsl.1 ${Dn},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_LSL_1_DYN_REG_S1_DIRECT, "lsl.1-dyn-reg-s1-direct", "lsl.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsl.1 ${Dn},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_LSL_1_IMM_BIT5_S1_IMMEDIATE, "lsl.1-imm-bit5-s1-immediate", "lsl.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsl.1 ${Dn},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_LSL_1_DYN_REG_S1_IMMEDIATE, "lsl.1-dyn-reg-s1-immediate", "lsl.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsl.1 ${Dn},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_LSL_1_IMM_BIT5_S1_INDIRECT_WITH_INDEX_1, "lsl.1-imm-bit5-s1-indirect-with-index-1", "lsl.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsl.1 ${Dn},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_LSL_1_DYN_REG_S1_INDIRECT_WITH_INDEX_1, "lsl.1-dyn-reg-s1-indirect-with-index-1", "lsl.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsl.1 ${Dn},${s1-imm7-1}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_LSL_1_IMM_BIT5_S1_INDIRECT_WITH_OFFSET_1, "lsl.1-imm-bit5-s1-indirect-with-offset-1", "lsl.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsl.1 ${Dn},${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_LSL_1_DYN_REG_S1_INDIRECT_WITH_OFFSET_1, "lsl.1-dyn-reg-s1-indirect-with-offset-1", "lsl.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsl.1 ${Dn},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_LSL_1_IMM_BIT5_S1_INDIRECT_1, "lsl.1-imm-bit5-s1-indirect-1", "lsl.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsl.1 ${Dn},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_LSL_1_DYN_REG_S1_INDIRECT_1, "lsl.1-dyn-reg-s1-indirect-1", "lsl.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsl.1 ${Dn},(${s1-An})${s1-i4-1}++,#${bit5} */
  {
    UBICOM32_INSN_LSL_1_IMM_BIT5_S1_INDIRECT_WITH_POST_INCREMENT_1, "lsl.1-imm-bit5-s1-indirect-with-post-increment-1", "lsl.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsl.1 ${Dn},(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_LSL_1_DYN_REG_S1_INDIRECT_WITH_POST_INCREMENT_1, "lsl.1-dyn-reg-s1-indirect-with-post-increment-1", "lsl.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsl.1 ${Dn},${s1-i4-1}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_LSL_1_IMM_BIT5_S1_INDIRECT_WITH_PRE_INCREMENT_1, "lsl.1-imm-bit5-s1-indirect-with-pre-increment-1", "lsl.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsl.1 ${Dn},${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_LSL_1_DYN_REG_S1_INDIRECT_WITH_PRE_INCREMENT_1, "lsl.1-dyn-reg-s1-indirect-with-pre-increment-1", "lsl.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsr.1 ${Dn},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_LSR_1_IMM_BIT5_S1_DIRECT, "lsr.1-imm-bit5-s1-direct", "lsr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsr.1 ${Dn},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_LSR_1_DYN_REG_S1_DIRECT, "lsr.1-dyn-reg-s1-direct", "lsr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsr.1 ${Dn},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_LSR_1_IMM_BIT5_S1_IMMEDIATE, "lsr.1-imm-bit5-s1-immediate", "lsr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsr.1 ${Dn},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_LSR_1_DYN_REG_S1_IMMEDIATE, "lsr.1-dyn-reg-s1-immediate", "lsr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsr.1 ${Dn},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_LSR_1_IMM_BIT5_S1_INDIRECT_WITH_INDEX_1, "lsr.1-imm-bit5-s1-indirect-with-index-1", "lsr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsr.1 ${Dn},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_LSR_1_DYN_REG_S1_INDIRECT_WITH_INDEX_1, "lsr.1-dyn-reg-s1-indirect-with-index-1", "lsr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsr.1 ${Dn},${s1-imm7-1}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_LSR_1_IMM_BIT5_S1_INDIRECT_WITH_OFFSET_1, "lsr.1-imm-bit5-s1-indirect-with-offset-1", "lsr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsr.1 ${Dn},${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_LSR_1_DYN_REG_S1_INDIRECT_WITH_OFFSET_1, "lsr.1-dyn-reg-s1-indirect-with-offset-1", "lsr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsr.1 ${Dn},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_LSR_1_IMM_BIT5_S1_INDIRECT_1, "lsr.1-imm-bit5-s1-indirect-1", "lsr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsr.1 ${Dn},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_LSR_1_DYN_REG_S1_INDIRECT_1, "lsr.1-dyn-reg-s1-indirect-1", "lsr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsr.1 ${Dn},(${s1-An})${s1-i4-1}++,#${bit5} */
  {
    UBICOM32_INSN_LSR_1_IMM_BIT5_S1_INDIRECT_WITH_POST_INCREMENT_1, "lsr.1-imm-bit5-s1-indirect-with-post-increment-1", "lsr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsr.1 ${Dn},(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_LSR_1_DYN_REG_S1_INDIRECT_WITH_POST_INCREMENT_1, "lsr.1-dyn-reg-s1-indirect-with-post-increment-1", "lsr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsr.1 ${Dn},${s1-i4-1}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_LSR_1_IMM_BIT5_S1_INDIRECT_WITH_PRE_INCREMENT_1, "lsr.1-imm-bit5-s1-indirect-with-pre-increment-1", "lsr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* lsr.1 ${Dn},${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_LSR_1_DYN_REG_S1_INDIRECT_WITH_PRE_INCREMENT_1, "lsr.1-dyn-reg-s1-indirect-with-pre-increment-1", "lsr.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* asr.2 ${Dn},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_ASR_2_IMM_BIT5_S1_DIRECT, "asr.2-imm-bit5-s1-direct", "asr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.2 ${Dn},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ASR_2_DYN_REG_S1_DIRECT, "asr.2-dyn-reg-s1-direct", "asr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.2 ${Dn},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_ASR_2_IMM_BIT5_S1_IMMEDIATE, "asr.2-imm-bit5-s1-immediate", "asr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.2 ${Dn},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ASR_2_DYN_REG_S1_IMMEDIATE, "asr.2-dyn-reg-s1-immediate", "asr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.2 ${Dn},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_ASR_2_IMM_BIT5_S1_INDIRECT_WITH_INDEX_2, "asr.2-imm-bit5-s1-indirect-with-index-2", "asr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.2 ${Dn},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ASR_2_DYN_REG_S1_INDIRECT_WITH_INDEX_2, "asr.2-dyn-reg-s1-indirect-with-index-2", "asr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.2 ${Dn},${s1-imm7-2}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_ASR_2_IMM_BIT5_S1_INDIRECT_WITH_OFFSET_2, "asr.2-imm-bit5-s1-indirect-with-offset-2", "asr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.2 ${Dn},${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ASR_2_DYN_REG_S1_INDIRECT_WITH_OFFSET_2, "asr.2-dyn-reg-s1-indirect-with-offset-2", "asr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.2 ${Dn},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_ASR_2_IMM_BIT5_S1_INDIRECT_2, "asr.2-imm-bit5-s1-indirect-2", "asr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.2 ${Dn},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ASR_2_DYN_REG_S1_INDIRECT_2, "asr.2-dyn-reg-s1-indirect-2", "asr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.2 ${Dn},(${s1-An})${s1-i4-2}++,#${bit5} */
  {
    UBICOM32_INSN_ASR_2_IMM_BIT5_S1_INDIRECT_WITH_POST_INCREMENT_2, "asr.2-imm-bit5-s1-indirect-with-post-increment-2", "asr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.2 ${Dn},(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_ASR_2_DYN_REG_S1_INDIRECT_WITH_POST_INCREMENT_2, "asr.2-dyn-reg-s1-indirect-with-post-increment-2", "asr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.2 ${Dn},${s1-i4-2}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_ASR_2_IMM_BIT5_S1_INDIRECT_WITH_PRE_INCREMENT_2, "asr.2-imm-bit5-s1-indirect-with-pre-increment-2", "asr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.2 ${Dn},${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ASR_2_DYN_REG_S1_INDIRECT_WITH_PRE_INCREMENT_2, "asr.2-dyn-reg-s1-indirect-with-pre-increment-2", "asr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.2 ${Dn},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_LSL_2_IMM_BIT5_S1_DIRECT, "lsl.2-imm-bit5-s1-direct", "lsl.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.2 ${Dn},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_LSL_2_DYN_REG_S1_DIRECT, "lsl.2-dyn-reg-s1-direct", "lsl.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.2 ${Dn},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_LSL_2_IMM_BIT5_S1_IMMEDIATE, "lsl.2-imm-bit5-s1-immediate", "lsl.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.2 ${Dn},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_LSL_2_DYN_REG_S1_IMMEDIATE, "lsl.2-dyn-reg-s1-immediate", "lsl.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.2 ${Dn},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_LSL_2_IMM_BIT5_S1_INDIRECT_WITH_INDEX_2, "lsl.2-imm-bit5-s1-indirect-with-index-2", "lsl.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.2 ${Dn},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_LSL_2_DYN_REG_S1_INDIRECT_WITH_INDEX_2, "lsl.2-dyn-reg-s1-indirect-with-index-2", "lsl.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.2 ${Dn},${s1-imm7-2}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_LSL_2_IMM_BIT5_S1_INDIRECT_WITH_OFFSET_2, "lsl.2-imm-bit5-s1-indirect-with-offset-2", "lsl.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.2 ${Dn},${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_LSL_2_DYN_REG_S1_INDIRECT_WITH_OFFSET_2, "lsl.2-dyn-reg-s1-indirect-with-offset-2", "lsl.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.2 ${Dn},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_LSL_2_IMM_BIT5_S1_INDIRECT_2, "lsl.2-imm-bit5-s1-indirect-2", "lsl.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.2 ${Dn},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_LSL_2_DYN_REG_S1_INDIRECT_2, "lsl.2-dyn-reg-s1-indirect-2", "lsl.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.2 ${Dn},(${s1-An})${s1-i4-2}++,#${bit5} */
  {
    UBICOM32_INSN_LSL_2_IMM_BIT5_S1_INDIRECT_WITH_POST_INCREMENT_2, "lsl.2-imm-bit5-s1-indirect-with-post-increment-2", "lsl.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.2 ${Dn},(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_LSL_2_DYN_REG_S1_INDIRECT_WITH_POST_INCREMENT_2, "lsl.2-dyn-reg-s1-indirect-with-post-increment-2", "lsl.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.2 ${Dn},${s1-i4-2}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_LSL_2_IMM_BIT5_S1_INDIRECT_WITH_PRE_INCREMENT_2, "lsl.2-imm-bit5-s1-indirect-with-pre-increment-2", "lsl.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.2 ${Dn},${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_LSL_2_DYN_REG_S1_INDIRECT_WITH_PRE_INCREMENT_2, "lsl.2-dyn-reg-s1-indirect-with-pre-increment-2", "lsl.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.2 ${Dn},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_LSR_2_IMM_BIT5_S1_DIRECT, "lsr.2-imm-bit5-s1-direct", "lsr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.2 ${Dn},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_LSR_2_DYN_REG_S1_DIRECT, "lsr.2-dyn-reg-s1-direct", "lsr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.2 ${Dn},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_LSR_2_IMM_BIT5_S1_IMMEDIATE, "lsr.2-imm-bit5-s1-immediate", "lsr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.2 ${Dn},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_LSR_2_DYN_REG_S1_IMMEDIATE, "lsr.2-dyn-reg-s1-immediate", "lsr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.2 ${Dn},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_LSR_2_IMM_BIT5_S1_INDIRECT_WITH_INDEX_2, "lsr.2-imm-bit5-s1-indirect-with-index-2", "lsr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.2 ${Dn},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_LSR_2_DYN_REG_S1_INDIRECT_WITH_INDEX_2, "lsr.2-dyn-reg-s1-indirect-with-index-2", "lsr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.2 ${Dn},${s1-imm7-2}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_LSR_2_IMM_BIT5_S1_INDIRECT_WITH_OFFSET_2, "lsr.2-imm-bit5-s1-indirect-with-offset-2", "lsr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.2 ${Dn},${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_LSR_2_DYN_REG_S1_INDIRECT_WITH_OFFSET_2, "lsr.2-dyn-reg-s1-indirect-with-offset-2", "lsr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.2 ${Dn},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_LSR_2_IMM_BIT5_S1_INDIRECT_2, "lsr.2-imm-bit5-s1-indirect-2", "lsr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.2 ${Dn},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_LSR_2_DYN_REG_S1_INDIRECT_2, "lsr.2-dyn-reg-s1-indirect-2", "lsr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.2 ${Dn},(${s1-An})${s1-i4-2}++,#${bit5} */
  {
    UBICOM32_INSN_LSR_2_IMM_BIT5_S1_INDIRECT_WITH_POST_INCREMENT_2, "lsr.2-imm-bit5-s1-indirect-with-post-increment-2", "lsr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.2 ${Dn},(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_LSR_2_DYN_REG_S1_INDIRECT_WITH_POST_INCREMENT_2, "lsr.2-dyn-reg-s1-indirect-with-post-increment-2", "lsr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.2 ${Dn},${s1-i4-2}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_LSR_2_IMM_BIT5_S1_INDIRECT_WITH_PRE_INCREMENT_2, "lsr.2-imm-bit5-s1-indirect-with-pre-increment-2", "lsr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.2 ${Dn},${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_LSR_2_DYN_REG_S1_INDIRECT_WITH_PRE_INCREMENT_2, "lsr.2-dyn-reg-s1-indirect-with-pre-increment-2", "lsr.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.4 ${Dn},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_ASR_4_IMM_BIT5_S1_DIRECT, "asr.4-imm-bit5-s1-direct", "asr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.4 ${Dn},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ASR_4_DYN_REG_S1_DIRECT, "asr.4-dyn-reg-s1-direct", "asr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.4 ${Dn},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_ASR_4_IMM_BIT5_S1_IMMEDIATE, "asr.4-imm-bit5-s1-immediate", "asr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.4 ${Dn},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ASR_4_DYN_REG_S1_IMMEDIATE, "asr.4-dyn-reg-s1-immediate", "asr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.4 ${Dn},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_ASR_4_IMM_BIT5_S1_INDIRECT_WITH_INDEX_4, "asr.4-imm-bit5-s1-indirect-with-index-4", "asr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.4 ${Dn},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ASR_4_DYN_REG_S1_INDIRECT_WITH_INDEX_4, "asr.4-dyn-reg-s1-indirect-with-index-4", "asr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.4 ${Dn},${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_ASR_4_IMM_BIT5_S1_INDIRECT_WITH_OFFSET_4, "asr.4-imm-bit5-s1-indirect-with-offset-4", "asr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.4 ${Dn},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ASR_4_DYN_REG_S1_INDIRECT_WITH_OFFSET_4, "asr.4-dyn-reg-s1-indirect-with-offset-4", "asr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.4 ${Dn},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_ASR_4_IMM_BIT5_S1_INDIRECT_4, "asr.4-imm-bit5-s1-indirect-4", "asr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.4 ${Dn},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ASR_4_DYN_REG_S1_INDIRECT_4, "asr.4-dyn-reg-s1-indirect-4", "asr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.4 ${Dn},(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_ASR_4_IMM_BIT5_S1_INDIRECT_WITH_POST_INCREMENT_4, "asr.4-imm-bit5-s1-indirect-with-post-increment-4", "asr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.4 ${Dn},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_ASR_4_DYN_REG_S1_INDIRECT_WITH_POST_INCREMENT_4, "asr.4-dyn-reg-s1-indirect-with-post-increment-4", "asr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.4 ${Dn},${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_ASR_4_IMM_BIT5_S1_INDIRECT_WITH_PRE_INCREMENT_4, "asr.4-imm-bit5-s1-indirect-with-pre-increment-4", "asr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* asr.4 ${Dn},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ASR_4_DYN_REG_S1_INDIRECT_WITH_PRE_INCREMENT_4, "asr.4-dyn-reg-s1-indirect-with-pre-increment-4", "asr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.4 ${Dn},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_LSL_4_IMM_BIT5_S1_DIRECT, "lsl.4-imm-bit5-s1-direct", "lsl.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.4 ${Dn},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_LSL_4_DYN_REG_S1_DIRECT, "lsl.4-dyn-reg-s1-direct", "lsl.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.4 ${Dn},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_LSL_4_IMM_BIT5_S1_IMMEDIATE, "lsl.4-imm-bit5-s1-immediate", "lsl.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.4 ${Dn},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_LSL_4_DYN_REG_S1_IMMEDIATE, "lsl.4-dyn-reg-s1-immediate", "lsl.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.4 ${Dn},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_LSL_4_IMM_BIT5_S1_INDIRECT_WITH_INDEX_4, "lsl.4-imm-bit5-s1-indirect-with-index-4", "lsl.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.4 ${Dn},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_LSL_4_DYN_REG_S1_INDIRECT_WITH_INDEX_4, "lsl.4-dyn-reg-s1-indirect-with-index-4", "lsl.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.4 ${Dn},${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_LSL_4_IMM_BIT5_S1_INDIRECT_WITH_OFFSET_4, "lsl.4-imm-bit5-s1-indirect-with-offset-4", "lsl.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.4 ${Dn},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_LSL_4_DYN_REG_S1_INDIRECT_WITH_OFFSET_4, "lsl.4-dyn-reg-s1-indirect-with-offset-4", "lsl.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.4 ${Dn},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_LSL_4_IMM_BIT5_S1_INDIRECT_4, "lsl.4-imm-bit5-s1-indirect-4", "lsl.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.4 ${Dn},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_LSL_4_DYN_REG_S1_INDIRECT_4, "lsl.4-dyn-reg-s1-indirect-4", "lsl.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.4 ${Dn},(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_LSL_4_IMM_BIT5_S1_INDIRECT_WITH_POST_INCREMENT_4, "lsl.4-imm-bit5-s1-indirect-with-post-increment-4", "lsl.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.4 ${Dn},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_LSL_4_DYN_REG_S1_INDIRECT_WITH_POST_INCREMENT_4, "lsl.4-dyn-reg-s1-indirect-with-post-increment-4", "lsl.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.4 ${Dn},${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_LSL_4_IMM_BIT5_S1_INDIRECT_WITH_PRE_INCREMENT_4, "lsl.4-imm-bit5-s1-indirect-with-pre-increment-4", "lsl.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsl.4 ${Dn},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_LSL_4_DYN_REG_S1_INDIRECT_WITH_PRE_INCREMENT_4, "lsl.4-dyn-reg-s1-indirect-with-pre-increment-4", "lsl.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.4 ${Dn},${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_LSR_4_IMM_BIT5_S1_DIRECT, "lsr.4-imm-bit5-s1-direct", "lsr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.4 ${Dn},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_LSR_4_DYN_REG_S1_DIRECT, "lsr.4-dyn-reg-s1-direct", "lsr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.4 ${Dn},#${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_LSR_4_IMM_BIT5_S1_IMMEDIATE, "lsr.4-imm-bit5-s1-immediate", "lsr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.4 ${Dn},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_LSR_4_DYN_REG_S1_IMMEDIATE, "lsr.4-dyn-reg-s1-immediate", "lsr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.4 ${Dn},(${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_LSR_4_IMM_BIT5_S1_INDIRECT_WITH_INDEX_4, "lsr.4-imm-bit5-s1-indirect-with-index-4", "lsr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.4 ${Dn},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_LSR_4_DYN_REG_S1_INDIRECT_WITH_INDEX_4, "lsr.4-dyn-reg-s1-indirect-with-index-4", "lsr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.4 ${Dn},${s1-imm7-4}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_LSR_4_IMM_BIT5_S1_INDIRECT_WITH_OFFSET_4, "lsr.4-imm-bit5-s1-indirect-with-offset-4", "lsr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.4 ${Dn},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_LSR_4_DYN_REG_S1_INDIRECT_WITH_OFFSET_4, "lsr.4-dyn-reg-s1-indirect-with-offset-4", "lsr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.4 ${Dn},(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_LSR_4_IMM_BIT5_S1_INDIRECT_4, "lsr.4-imm-bit5-s1-indirect-4", "lsr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.4 ${Dn},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_LSR_4_DYN_REG_S1_INDIRECT_4, "lsr.4-dyn-reg-s1-indirect-4", "lsr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.4 ${Dn},(${s1-An})${s1-i4-4}++,#${bit5} */
  {
    UBICOM32_INSN_LSR_4_IMM_BIT5_S1_INDIRECT_WITH_POST_INCREMENT_4, "lsr.4-imm-bit5-s1-indirect-with-post-increment-4", "lsr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.4 ${Dn},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_LSR_4_DYN_REG_S1_INDIRECT_WITH_POST_INCREMENT_4, "lsr.4-dyn-reg-s1-indirect-with-post-increment-4", "lsr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.4 ${Dn},${s1-i4-4}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_LSR_4_IMM_BIT5_S1_INDIRECT_WITH_PRE_INCREMENT_4, "lsr.4-imm-bit5-s1-indirect-with-pre-increment-4", "lsr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lsr.4 ${Dn},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_LSR_4_DYN_REG_S1_INDIRECT_WITH_PRE_INCREMENT_4, "lsr.4-dyn-reg-s1-indirect-with-pre-increment-4", "lsr.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* mac ${s1-direct-addr},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MAC_S1_DIRECT_DSP_SRC2_DATA_REG, "compatibility-mac-s1-direct-dsp-src2-data-reg", "mac", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mac #${s1-imm8},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MAC_S1_IMMEDIATE_DSP_SRC2_DATA_REG, "compatibility-mac-s1-immediate-dsp-src2-data-reg", "mac", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mac (${s1-An},${s1-r}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MAC_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_DATA_REG, "compatibility-mac-s1-indirect-with-index-2-dsp-src2-data-reg", "mac", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mac ${s1-imm7-2}(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MAC_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_DATA_REG, "compatibility-mac-s1-indirect-with-offset-2-dsp-src2-data-reg", "mac", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mac (${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MAC_S1_INDIRECT_2_DSP_SRC2_DATA_REG, "compatibility-mac-s1-indirect-2-dsp-src2-data-reg", "mac", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mac (${s1-An})${s1-i4-2}++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MAC_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_DATA_REG, "compatibility-mac-s1-indirect-with-post-increment-2-dsp-src2-data-reg", "mac", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mac ${s1-i4-2}(${s1-An})++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MAC_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_DATA_REG, "compatibility-mac-s1-indirect-with-pre-increment-2-dsp-src2-data-reg", "mac", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mac ${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MAC_S1_DIRECT_DSP_IMM_BIT5, "compatibility-mac-s1-direct-dsp-imm-bit5", "mac", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mac #${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MAC_S1_IMMEDIATE_DSP_IMM_BIT5, "compatibility-mac-s1-immediate-dsp-imm-bit5", "mac", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mac (${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MAC_S1_INDIRECT_WITH_INDEX_2_DSP_IMM_BIT5, "compatibility-mac-s1-indirect-with-index-2-dsp-imm-bit5", "mac", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mac ${s1-imm7-2}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MAC_S1_INDIRECT_WITH_OFFSET_2_DSP_IMM_BIT5, "compatibility-mac-s1-indirect-with-offset-2-dsp-imm-bit5", "mac", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mac (${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MAC_S1_INDIRECT_2_DSP_IMM_BIT5, "compatibility-mac-s1-indirect-2-dsp-imm-bit5", "mac", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mac (${s1-An})${s1-i4-2}++,#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MAC_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_IMM_BIT5, "compatibility-mac-s1-indirect-with-post-increment-2-dsp-imm-bit5", "mac", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mac ${s1-i4-2}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MAC_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_IMM_BIT5, "compatibility-mac-s1-indirect-with-pre-increment-2-dsp-imm-bit5", "mac", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mac ${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_MAC_S1_DIRECT_IMM_BIT5, "mac-s1-direct-imm-bit5", "mac", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mac #${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_MAC_S1_IMMEDIATE_IMM_BIT5, "mac-s1-immediate-imm-bit5", "mac", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mac (${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_MAC_S1_INDIRECT_WITH_INDEX_2_IMM_BIT5, "mac-s1-indirect-with-index-2-imm-bit5", "mac", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mac ${s1-imm7-2}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_MAC_S1_INDIRECT_WITH_OFFSET_2_IMM_BIT5, "mac-s1-indirect-with-offset-2-imm-bit5", "mac", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mac (${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_MAC_S1_INDIRECT_2_IMM_BIT5, "mac-s1-indirect-2-imm-bit5", "mac", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mac (${s1-An})${s1-i4-2}++,#${bit5} */
  {
    UBICOM32_INSN_MAC_S1_INDIRECT_WITH_POST_INCREMENT_2_IMM_BIT5, "mac-s1-indirect-with-post-increment-2-imm-bit5", "mac", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mac ${s1-i4-2}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_MAC_S1_INDIRECT_WITH_PRE_INCREMENT_2_IMM_BIT5, "mac-s1-indirect-with-pre-increment-2-imm-bit5", "mac", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mac ${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_MAC_S1_DIRECT_DYN_REG, "mac-s1-direct-dyn-reg", "mac", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mac #${s1-imm8},${s2} */
  {
    UBICOM32_INSN_MAC_S1_IMMEDIATE_DYN_REG, "mac-s1-immediate-dyn-reg", "mac", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mac (${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_MAC_S1_INDIRECT_WITH_INDEX_2_DYN_REG, "mac-s1-indirect-with-index-2-dyn-reg", "mac", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mac ${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_MAC_S1_INDIRECT_WITH_OFFSET_2_DYN_REG, "mac-s1-indirect-with-offset-2-dyn-reg", "mac", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mac (${s1-An}),${s2} */
  {
    UBICOM32_INSN_MAC_S1_INDIRECT_2_DYN_REG, "mac-s1-indirect-2-dyn-reg", "mac", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mac (${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_MAC_S1_INDIRECT_WITH_POST_INCREMENT_2_DYN_REG, "mac-s1-indirect-with-post-increment-2-dyn-reg", "mac", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mac ${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_MAC_S1_INDIRECT_WITH_PRE_INCREMENT_2_DYN_REG, "mac-s1-indirect-with-pre-increment-2-dyn-reg", "mac", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulf ${s1-direct-addr},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULF_S1_DIRECT_DSP_SRC2_DATA_REG, "compatibility-mulf-s1-direct-dsp-src2-data-reg", "mulf", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulf #${s1-imm8},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULF_S1_IMMEDIATE_DSP_SRC2_DATA_REG, "compatibility-mulf-s1-immediate-dsp-src2-data-reg", "mulf", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulf (${s1-An},${s1-r}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULF_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_DATA_REG, "compatibility-mulf-s1-indirect-with-index-2-dsp-src2-data-reg", "mulf", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulf ${s1-imm7-2}(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULF_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_DATA_REG, "compatibility-mulf-s1-indirect-with-offset-2-dsp-src2-data-reg", "mulf", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulf (${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULF_S1_INDIRECT_2_DSP_SRC2_DATA_REG, "compatibility-mulf-s1-indirect-2-dsp-src2-data-reg", "mulf", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulf (${s1-An})${s1-i4-2}++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULF_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_DATA_REG, "compatibility-mulf-s1-indirect-with-post-increment-2-dsp-src2-data-reg", "mulf", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulf ${s1-i4-2}(${s1-An})++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULF_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_DATA_REG, "compatibility-mulf-s1-indirect-with-pre-increment-2-dsp-src2-data-reg", "mulf", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulf ${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULF_S1_DIRECT_DSP_IMM_BIT5, "compatibility-mulf-s1-direct-dsp-imm-bit5", "mulf", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulf #${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULF_S1_IMMEDIATE_DSP_IMM_BIT5, "compatibility-mulf-s1-immediate-dsp-imm-bit5", "mulf", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulf (${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULF_S1_INDIRECT_WITH_INDEX_2_DSP_IMM_BIT5, "compatibility-mulf-s1-indirect-with-index-2-dsp-imm-bit5", "mulf", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulf ${s1-imm7-2}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULF_S1_INDIRECT_WITH_OFFSET_2_DSP_IMM_BIT5, "compatibility-mulf-s1-indirect-with-offset-2-dsp-imm-bit5", "mulf", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulf (${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULF_S1_INDIRECT_2_DSP_IMM_BIT5, "compatibility-mulf-s1-indirect-2-dsp-imm-bit5", "mulf", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulf (${s1-An})${s1-i4-2}++,#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULF_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_IMM_BIT5, "compatibility-mulf-s1-indirect-with-post-increment-2-dsp-imm-bit5", "mulf", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulf ${s1-i4-2}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULF_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_IMM_BIT5, "compatibility-mulf-s1-indirect-with-pre-increment-2-dsp-imm-bit5", "mulf", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulf ${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_MULF_S1_DIRECT_IMM_BIT5, "mulf-s1-direct-imm-bit5", "mulf", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulf #${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_MULF_S1_IMMEDIATE_IMM_BIT5, "mulf-s1-immediate-imm-bit5", "mulf", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulf (${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_MULF_S1_INDIRECT_WITH_INDEX_2_IMM_BIT5, "mulf-s1-indirect-with-index-2-imm-bit5", "mulf", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulf ${s1-imm7-2}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_MULF_S1_INDIRECT_WITH_OFFSET_2_IMM_BIT5, "mulf-s1-indirect-with-offset-2-imm-bit5", "mulf", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulf (${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_MULF_S1_INDIRECT_2_IMM_BIT5, "mulf-s1-indirect-2-imm-bit5", "mulf", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulf (${s1-An})${s1-i4-2}++,#${bit5} */
  {
    UBICOM32_INSN_MULF_S1_INDIRECT_WITH_POST_INCREMENT_2_IMM_BIT5, "mulf-s1-indirect-with-post-increment-2-imm-bit5", "mulf", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulf ${s1-i4-2}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_MULF_S1_INDIRECT_WITH_PRE_INCREMENT_2_IMM_BIT5, "mulf-s1-indirect-with-pre-increment-2-imm-bit5", "mulf", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulf ${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_MULF_S1_DIRECT_DYN_REG, "mulf-s1-direct-dyn-reg", "mulf", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulf #${s1-imm8},${s2} */
  {
    UBICOM32_INSN_MULF_S1_IMMEDIATE_DYN_REG, "mulf-s1-immediate-dyn-reg", "mulf", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulf (${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_MULF_S1_INDIRECT_WITH_INDEX_2_DYN_REG, "mulf-s1-indirect-with-index-2-dyn-reg", "mulf", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulf ${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_MULF_S1_INDIRECT_WITH_OFFSET_2_DYN_REG, "mulf-s1-indirect-with-offset-2-dyn-reg", "mulf", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulf (${s1-An}),${s2} */
  {
    UBICOM32_INSN_MULF_S1_INDIRECT_2_DYN_REG, "mulf-s1-indirect-2-dyn-reg", "mulf", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulf (${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_MULF_S1_INDIRECT_WITH_POST_INCREMENT_2_DYN_REG, "mulf-s1-indirect-with-post-increment-2-dyn-reg", "mulf", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulf ${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_MULF_S1_INDIRECT_WITH_PRE_INCREMENT_2_DYN_REG, "mulf-s1-indirect-with-pre-increment-2-dyn-reg", "mulf", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulu ${s1-direct-addr},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULU_S1_DIRECT_DSP_SRC2_DATA_REG, "compatibility-mulu-s1-direct-dsp-src2-data-reg", "mulu", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulu #${s1-imm8},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULU_S1_IMMEDIATE_DSP_SRC2_DATA_REG, "compatibility-mulu-s1-immediate-dsp-src2-data-reg", "mulu", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulu (${s1-An},${s1-r}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULU_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_DATA_REG, "compatibility-mulu-s1-indirect-with-index-2-dsp-src2-data-reg", "mulu", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulu ${s1-imm7-2}(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULU_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_DATA_REG, "compatibility-mulu-s1-indirect-with-offset-2-dsp-src2-data-reg", "mulu", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulu (${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULU_S1_INDIRECT_2_DSP_SRC2_DATA_REG, "compatibility-mulu-s1-indirect-2-dsp-src2-data-reg", "mulu", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulu (${s1-An})${s1-i4-2}++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULU_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_DATA_REG, "compatibility-mulu-s1-indirect-with-post-increment-2-dsp-src2-data-reg", "mulu", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulu ${s1-i4-2}(${s1-An})++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULU_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_DATA_REG, "compatibility-mulu-s1-indirect-with-pre-increment-2-dsp-src2-data-reg", "mulu", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulu ${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULU_S1_DIRECT_DSP_IMM_BIT5, "compatibility-mulu-s1-direct-dsp-imm-bit5", "mulu", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulu #${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULU_S1_IMMEDIATE_DSP_IMM_BIT5, "compatibility-mulu-s1-immediate-dsp-imm-bit5", "mulu", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulu (${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULU_S1_INDIRECT_WITH_INDEX_2_DSP_IMM_BIT5, "compatibility-mulu-s1-indirect-with-index-2-dsp-imm-bit5", "mulu", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulu ${s1-imm7-2}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULU_S1_INDIRECT_WITH_OFFSET_2_DSP_IMM_BIT5, "compatibility-mulu-s1-indirect-with-offset-2-dsp-imm-bit5", "mulu", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulu (${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULU_S1_INDIRECT_2_DSP_IMM_BIT5, "compatibility-mulu-s1-indirect-2-dsp-imm-bit5", "mulu", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulu (${s1-An})${s1-i4-2}++,#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULU_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_IMM_BIT5, "compatibility-mulu-s1-indirect-with-post-increment-2-dsp-imm-bit5", "mulu", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulu ${s1-i4-2}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULU_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_IMM_BIT5, "compatibility-mulu-s1-indirect-with-pre-increment-2-dsp-imm-bit5", "mulu", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* mulu ${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_MULU_S1_DIRECT_IMM_BIT5, "mulu-s1-direct-imm-bit5", "mulu", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulu #${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_MULU_S1_IMMEDIATE_IMM_BIT5, "mulu-s1-immediate-imm-bit5", "mulu", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulu (${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_MULU_S1_INDIRECT_WITH_INDEX_2_IMM_BIT5, "mulu-s1-indirect-with-index-2-imm-bit5", "mulu", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulu ${s1-imm7-2}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_MULU_S1_INDIRECT_WITH_OFFSET_2_IMM_BIT5, "mulu-s1-indirect-with-offset-2-imm-bit5", "mulu", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulu (${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_MULU_S1_INDIRECT_2_IMM_BIT5, "mulu-s1-indirect-2-imm-bit5", "mulu", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulu (${s1-An})${s1-i4-2}++,#${bit5} */
  {
    UBICOM32_INSN_MULU_S1_INDIRECT_WITH_POST_INCREMENT_2_IMM_BIT5, "mulu-s1-indirect-with-post-increment-2-imm-bit5", "mulu", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulu ${s1-i4-2}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_MULU_S1_INDIRECT_WITH_PRE_INCREMENT_2_IMM_BIT5, "mulu-s1-indirect-with-pre-increment-2-imm-bit5", "mulu", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulu ${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_MULU_S1_DIRECT_DYN_REG, "mulu-s1-direct-dyn-reg", "mulu", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulu #${s1-imm8},${s2} */
  {
    UBICOM32_INSN_MULU_S1_IMMEDIATE_DYN_REG, "mulu-s1-immediate-dyn-reg", "mulu", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulu (${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_MULU_S1_INDIRECT_WITH_INDEX_2_DYN_REG, "mulu-s1-indirect-with-index-2-dyn-reg", "mulu", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulu ${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_MULU_S1_INDIRECT_WITH_OFFSET_2_DYN_REG, "mulu-s1-indirect-with-offset-2-dyn-reg", "mulu", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulu (${s1-An}),${s2} */
  {
    UBICOM32_INSN_MULU_S1_INDIRECT_2_DYN_REG, "mulu-s1-indirect-2-dyn-reg", "mulu", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulu (${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_MULU_S1_INDIRECT_WITH_POST_INCREMENT_2_DYN_REG, "mulu-s1-indirect-with-post-increment-2-dyn-reg", "mulu", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* mulu ${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_MULU_S1_INDIRECT_WITH_PRE_INCREMENT_2_DYN_REG, "mulu-s1-indirect-with-pre-increment-2-dyn-reg", "mulu", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* muls ${s1-direct-addr},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULS_S1_DIRECT_DSP_SRC2_DATA_REG, "compatibility-muls-s1-direct-dsp-src2-data-reg", "muls", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* muls #${s1-imm8},${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULS_S1_IMMEDIATE_DSP_SRC2_DATA_REG, "compatibility-muls-s1-immediate-dsp-src2-data-reg", "muls", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* muls (${s1-An},${s1-r}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULS_S1_INDIRECT_WITH_INDEX_2_DSP_SRC2_DATA_REG, "compatibility-muls-s1-indirect-with-index-2-dsp-src2-data-reg", "muls", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* muls ${s1-imm7-2}(${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULS_S1_INDIRECT_WITH_OFFSET_2_DSP_SRC2_DATA_REG, "compatibility-muls-s1-indirect-with-offset-2-dsp-src2-data-reg", "muls", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* muls (${s1-An}),${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULS_S1_INDIRECT_2_DSP_SRC2_DATA_REG, "compatibility-muls-s1-indirect-2-dsp-src2-data-reg", "muls", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* muls (${s1-An})${s1-i4-2}++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULS_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_SRC2_DATA_REG, "compatibility-muls-s1-indirect-with-post-increment-2-dsp-src2-data-reg", "muls", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* muls ${s1-i4-2}(${s1-An})++,${dsp-S2-data-reg} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULS_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_SRC2_DATA_REG, "compatibility-muls-s1-indirect-with-pre-increment-2-dsp-src2-data-reg", "muls", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* muls ${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULS_S1_DIRECT_DSP_IMM_BIT5, "compatibility-muls-s1-direct-dsp-imm-bit5", "muls", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* muls #${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULS_S1_IMMEDIATE_DSP_IMM_BIT5, "compatibility-muls-s1-immediate-dsp-imm-bit5", "muls", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* muls (${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULS_S1_INDIRECT_WITH_INDEX_2_DSP_IMM_BIT5, "compatibility-muls-s1-indirect-with-index-2-dsp-imm-bit5", "muls", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* muls ${s1-imm7-2}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULS_S1_INDIRECT_WITH_OFFSET_2_DSP_IMM_BIT5, "compatibility-muls-s1-indirect-with-offset-2-dsp-imm-bit5", "muls", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* muls (${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULS_S1_INDIRECT_2_DSP_IMM_BIT5, "compatibility-muls-s1-indirect-2-dsp-imm-bit5", "muls", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* muls (${s1-An})${s1-i4-2}++,#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULS_S1_INDIRECT_WITH_POST_INCREMENT_2_DSP_IMM_BIT5, "compatibility-muls-s1-indirect-with-post-increment-2-dsp-imm-bit5", "muls", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* muls ${s1-i4-2}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_COMPATIBILITY_MULS_S1_INDIRECT_WITH_PRE_INCREMENT_2_DSP_IMM_BIT5, "compatibility-muls-s1-indirect-with-pre-increment-2-dsp-imm-bit5", "muls", 32,
    { 0, { { { (1<<MACH_IP3023COMPATIBILITY), 0 } } } }
  },
/* muls ${s1-direct-addr},#${bit5} */
  {
    UBICOM32_INSN_MULS_S1_DIRECT_IMM_BIT5, "muls-s1-direct-imm-bit5", "muls", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* muls #${s1-imm8},#${bit5} */
  {
    UBICOM32_INSN_MULS_S1_IMMEDIATE_IMM_BIT5, "muls-s1-immediate-imm-bit5", "muls", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* muls (${s1-An},${s1-r}),#${bit5} */
  {
    UBICOM32_INSN_MULS_S1_INDIRECT_WITH_INDEX_2_IMM_BIT5, "muls-s1-indirect-with-index-2-imm-bit5", "muls", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* muls ${s1-imm7-2}(${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_MULS_S1_INDIRECT_WITH_OFFSET_2_IMM_BIT5, "muls-s1-indirect-with-offset-2-imm-bit5", "muls", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* muls (${s1-An}),#${bit5} */
  {
    UBICOM32_INSN_MULS_S1_INDIRECT_2_IMM_BIT5, "muls-s1-indirect-2-imm-bit5", "muls", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* muls (${s1-An})${s1-i4-2}++,#${bit5} */
  {
    UBICOM32_INSN_MULS_S1_INDIRECT_WITH_POST_INCREMENT_2_IMM_BIT5, "muls-s1-indirect-with-post-increment-2-imm-bit5", "muls", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* muls ${s1-i4-2}(${s1-An})++,#${bit5} */
  {
    UBICOM32_INSN_MULS_S1_INDIRECT_WITH_PRE_INCREMENT_2_IMM_BIT5, "muls-s1-indirect-with-pre-increment-2-imm-bit5", "muls", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* muls ${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_MULS_S1_DIRECT_DYN_REG, "muls-s1-direct-dyn-reg", "muls", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* muls #${s1-imm8},${s2} */
  {
    UBICOM32_INSN_MULS_S1_IMMEDIATE_DYN_REG, "muls-s1-immediate-dyn-reg", "muls", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* muls (${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_MULS_S1_INDIRECT_WITH_INDEX_2_DYN_REG, "muls-s1-indirect-with-index-2-dyn-reg", "muls", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* muls ${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_MULS_S1_INDIRECT_WITH_OFFSET_2_DYN_REG, "muls-s1-indirect-with-offset-2-dyn-reg", "muls", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* muls (${s1-An}),${s2} */
  {
    UBICOM32_INSN_MULS_S1_INDIRECT_2_DYN_REG, "muls-s1-indirect-2-dyn-reg", "muls", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* muls (${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_MULS_S1_INDIRECT_WITH_POST_INCREMENT_2_DYN_REG, "muls-s1-indirect-with-post-increment-2-dyn-reg", "muls", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* muls ${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_MULS_S1_INDIRECT_WITH_PRE_INCREMENT_2_DYN_REG, "muls-s1-indirect-with-pre-increment-2-dyn-reg", "muls", 32,
    { 0, { { { (1<<MACH_IP3035), 0 } } } }
  },
/* swapb.4 ${d-direct-addr},${s1-direct-addr} */
  {
    UBICOM32_INSN_SWAPB_4_D_DIRECT_S1_DIRECT, "swapb.4-d-direct-s1-direct", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 #${d-imm8},${s1-direct-addr} */
  {
    UBICOM32_INSN_SWAPB_4_D_IMMEDIATE_4_S1_DIRECT, "swapb.4-d-immediate-4-s1-direct", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 (${d-An},${d-r}),${s1-direct-addr} */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_INDEX_4_S1_DIRECT, "swapb.4-d-indirect-with-index-4-s1-direct", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 ${d-imm7-4}(${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT, "swapb.4-d-indirect-with-offset-4-s1-direct", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 (${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_4_S1_DIRECT, "swapb.4-d-indirect-4-s1-direct", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 (${d-An})${d-i4-4}++,${s1-direct-addr} */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_DIRECT, "swapb.4-d-indirect-with-post-increment-4-s1-direct", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 ${d-i4-4}(${d-An})++,${s1-direct-addr} */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT, "swapb.4-d-indirect-with-pre-increment-4-s1-direct", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 ${d-direct-addr},#${s1-imm8} */
  {
    UBICOM32_INSN_SWAPB_4_D_DIRECT_S1_IMMEDIATE, "swapb.4-d-direct-s1-immediate", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 #${d-imm8},#${s1-imm8} */
  {
    UBICOM32_INSN_SWAPB_4_D_IMMEDIATE_4_S1_IMMEDIATE, "swapb.4-d-immediate-4-s1-immediate", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 (${d-An},${d-r}),#${s1-imm8} */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_INDEX_4_S1_IMMEDIATE, "swapb.4-d-indirect-with-index-4-s1-immediate", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 ${d-imm7-4}(${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_OFFSET_4_S1_IMMEDIATE, "swapb.4-d-indirect-with-offset-4-s1-immediate", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 (${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_4_S1_IMMEDIATE, "swapb.4-d-indirect-4-s1-immediate", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 (${d-An})${d-i4-4}++,#${s1-imm8} */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_IMMEDIATE, "swapb.4-d-indirect-with-post-increment-4-s1-immediate", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 ${d-i4-4}(${d-An})++,#${s1-imm8} */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_IMMEDIATE, "swapb.4-d-indirect-with-pre-increment-4-s1-immediate", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 ${d-direct-addr},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_SWAPB_4_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "swapb.4-d-direct-s1-indirect-with-index-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 #${d-imm8},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_SWAPB_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_INDEX_4, "swapb.4-d-immediate-4-s1-indirect-with-index-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 (${d-An},${d-r}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_INDEX_4, "swapb.4-d-indirect-with-index-4-s1-indirect-with-index-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_INDEX_4, "swapb.4-d-indirect-with-offset-4-s1-indirect-with-index-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 (${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_4_S1_INDIRECT_WITH_INDEX_4, "swapb.4-d-indirect-4-s1-indirect-with-index-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 (${d-An})${d-i4-4}++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "swapb.4-d-indirect-with-post-increment-4-s1-indirect-with-index-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "swapb.4-d-indirect-with-pre-increment-4-s1-indirect-with-index-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 ${d-direct-addr},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_4_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "swapb.4-d-direct-s1-indirect-with-offset-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 #${d-imm8},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_OFFSET_4, "swapb.4-d-immediate-4-s1-indirect-with-offset-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 (${d-An},${d-r}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_OFFSET_4, "swapb.4-d-indirect-with-index-4-s1-indirect-with-offset-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_OFFSET_4, "swapb.4-d-indirect-with-offset-4-s1-indirect-with-offset-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 (${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_4_S1_INDIRECT_WITH_OFFSET_4, "swapb.4-d-indirect-4-s1-indirect-with-offset-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "swapb.4-d-indirect-with-post-increment-4-s1-indirect-with-offset-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "swapb.4-d-indirect-with-pre-increment-4-s1-indirect-with-offset-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 ${d-direct-addr},(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_4_D_DIRECT_S1_INDIRECT_4, "swapb.4-d-direct-s1-indirect-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 #${d-imm8},(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_4_D_IMMEDIATE_4_S1_INDIRECT_4, "swapb.4-d-immediate-4-s1-indirect-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 (${d-An},${d-r}),(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_4, "swapb.4-d-indirect-with-index-4-s1-indirect-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 ${d-imm7-4}(${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_4, "swapb.4-d-indirect-with-offset-4-s1-indirect-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 (${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_4_S1_INDIRECT_4, "swapb.4-d-indirect-4-s1-indirect-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 (${d-An})${d-i4-4}++,(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_4, "swapb.4-d-indirect-with-post-increment-4-s1-indirect-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 ${d-i4-4}(${d-An})++,(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_4, "swapb.4-d-indirect-with-pre-increment-4-s1-indirect-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 ${d-direct-addr},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_SWAPB_4_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "swapb.4-d-direct-s1-indirect-with-post-increment-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 #${d-imm8},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_SWAPB_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "swapb.4-d-immediate-4-s1-indirect-with-post-increment-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 (${d-An},${d-r}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "swapb.4-d-indirect-with-index-4-s1-indirect-with-post-increment-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "swapb.4-d-indirect-with-offset-4-s1-indirect-with-post-increment-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 (${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "swapb.4-d-indirect-4-s1-indirect-with-post-increment-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "swapb.4-d-indirect-with-post-increment-4-s1-indirect-with-post-increment-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "swapb.4-d-indirect-with-pre-increment-4-s1-indirect-with-post-increment-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 ${d-direct-addr},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_SWAPB_4_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "swapb.4-d-direct-s1-indirect-with-pre-increment-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 #${d-imm8},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_SWAPB_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "swapb.4-d-immediate-4-s1-indirect-with-pre-increment-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 (${d-An},${d-r}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "swapb.4-d-indirect-with-index-4-s1-indirect-with-pre-increment-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "swapb.4-d-indirect-with-offset-4-s1-indirect-with-pre-increment-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 (${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "swapb.4-d-indirect-4-s1-indirect-with-pre-increment-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "swapb.4-d-indirect-with-post-increment-4-s1-indirect-with-pre-increment-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.4 ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_SWAPB_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "swapb.4-d-indirect-with-pre-increment-4-s1-indirect-with-pre-increment-4", "swapb.4", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 ${d-direct-addr},${s1-direct-addr} */
  {
    UBICOM32_INSN_SWAPB_2_D_DIRECT_S1_DIRECT, "swapb.2-d-direct-s1-direct", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 #${d-imm8},${s1-direct-addr} */
  {
    UBICOM32_INSN_SWAPB_2_D_IMMEDIATE_2_S1_DIRECT, "swapb.2-d-immediate-2-s1-direct", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 (${d-An},${d-r}),${s1-direct-addr} */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_INDEX_2_S1_DIRECT, "swapb.2-d-indirect-with-index-2-s1-direct", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 ${d-imm7-2}(${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_OFFSET_2_S1_DIRECT, "swapb.2-d-indirect-with-offset-2-s1-direct", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 (${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_2_S1_DIRECT, "swapb.2-d-indirect-2-s1-direct", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 (${d-An})${d-i4-2}++,${s1-direct-addr} */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_DIRECT, "swapb.2-d-indirect-with-post-increment-2-s1-direct", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 ${d-i4-2}(${d-An})++,${s1-direct-addr} */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_DIRECT, "swapb.2-d-indirect-with-pre-increment-2-s1-direct", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 ${d-direct-addr},#${s1-imm8} */
  {
    UBICOM32_INSN_SWAPB_2_D_DIRECT_S1_IMMEDIATE, "swapb.2-d-direct-s1-immediate", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 #${d-imm8},#${s1-imm8} */
  {
    UBICOM32_INSN_SWAPB_2_D_IMMEDIATE_2_S1_IMMEDIATE, "swapb.2-d-immediate-2-s1-immediate", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 (${d-An},${d-r}),#${s1-imm8} */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_INDEX_2_S1_IMMEDIATE, "swapb.2-d-indirect-with-index-2-s1-immediate", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 ${d-imm7-2}(${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_OFFSET_2_S1_IMMEDIATE, "swapb.2-d-indirect-with-offset-2-s1-immediate", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 (${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_2_S1_IMMEDIATE, "swapb.2-d-indirect-2-s1-immediate", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 (${d-An})${d-i4-2}++,#${s1-imm8} */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_IMMEDIATE, "swapb.2-d-indirect-with-post-increment-2-s1-immediate", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 ${d-i4-2}(${d-An})++,#${s1-imm8} */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_IMMEDIATE, "swapb.2-d-indirect-with-pre-increment-2-s1-immediate", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 ${d-direct-addr},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_SWAPB_2_D_DIRECT_S1_INDIRECT_WITH_INDEX_2, "swapb.2-d-direct-s1-indirect-with-index-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 #${d-imm8},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_SWAPB_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_INDEX_2, "swapb.2-d-immediate-2-s1-indirect-with-index-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 (${d-An},${d-r}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_INDEX_2, "swapb.2-d-indirect-with-index-2-s1-indirect-with-index-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 ${d-imm7-2}(${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_INDEX_2, "swapb.2-d-indirect-with-offset-2-s1-indirect-with-index-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 (${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_2_S1_INDIRECT_WITH_INDEX_2, "swapb.2-d-indirect-2-s1-indirect-with-index-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 (${d-An})${d-i4-2}++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_INDEX_2, "swapb.2-d-indirect-with-post-increment-2-s1-indirect-with-index-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 ${d-i4-2}(${d-An})++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_INDEX_2, "swapb.2-d-indirect-with-pre-increment-2-s1-indirect-with-index-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 ${d-direct-addr},${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_2_D_DIRECT_S1_INDIRECT_WITH_OFFSET_2, "swapb.2-d-direct-s1-indirect-with-offset-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 #${d-imm8},${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_OFFSET_2, "swapb.2-d-immediate-2-s1-indirect-with-offset-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 (${d-An},${d-r}),${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_OFFSET_2, "swapb.2-d-indirect-with-index-2-s1-indirect-with-offset-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 ${d-imm7-2}(${d-An}),${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_OFFSET_2, "swapb.2-d-indirect-with-offset-2-s1-indirect-with-offset-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 (${d-An}),${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_2_S1_INDIRECT_WITH_OFFSET_2, "swapb.2-d-indirect-2-s1-indirect-with-offset-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 (${d-An})${d-i4-2}++,${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_2, "swapb.2-d-indirect-with-post-increment-2-s1-indirect-with-offset-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 ${d-i4-2}(${d-An})++,${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_2, "swapb.2-d-indirect-with-pre-increment-2-s1-indirect-with-offset-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 ${d-direct-addr},(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_2_D_DIRECT_S1_INDIRECT_2, "swapb.2-d-direct-s1-indirect-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 #${d-imm8},(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_2_D_IMMEDIATE_2_S1_INDIRECT_2, "swapb.2-d-immediate-2-s1-indirect-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 (${d-An},${d-r}),(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_2, "swapb.2-d-indirect-with-index-2-s1-indirect-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 ${d-imm7-2}(${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_2, "swapb.2-d-indirect-with-offset-2-s1-indirect-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 (${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_2_S1_INDIRECT_2, "swapb.2-d-indirect-2-s1-indirect-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 (${d-An})${d-i4-2}++,(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_2, "swapb.2-d-indirect-with-post-increment-2-s1-indirect-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 ${d-i4-2}(${d-An})++,(${s1-An}) */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_2, "swapb.2-d-indirect-with-pre-increment-2-s1-indirect-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 ${d-direct-addr},(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_SWAPB_2_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_2, "swapb.2-d-direct-s1-indirect-with-post-increment-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 #${d-imm8},(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_SWAPB_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "swapb.2-d-immediate-2-s1-indirect-with-post-increment-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 (${d-An},${d-r}),(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "swapb.2-d-indirect-with-index-2-s1-indirect-with-post-increment-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 ${d-imm7-2}(${d-An}),(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "swapb.2-d-indirect-with-offset-2-s1-indirect-with-post-increment-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 (${d-An}),(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "swapb.2-d-indirect-2-s1-indirect-with-post-increment-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 (${d-An})${d-i4-2}++,(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "swapb.2-d-indirect-with-post-increment-2-s1-indirect-with-post-increment-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 ${d-i4-2}(${d-An})++,(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "swapb.2-d-indirect-with-pre-increment-2-s1-indirect-with-post-increment-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 ${d-direct-addr},${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_SWAPB_2_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_2, "swapb.2-d-direct-s1-indirect-with-pre-increment-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 #${d-imm8},${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_SWAPB_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "swapb.2-d-immediate-2-s1-indirect-with-pre-increment-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 (${d-An},${d-r}),${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "swapb.2-d-indirect-with-index-2-s1-indirect-with-pre-increment-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 ${d-imm7-2}(${d-An}),${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "swapb.2-d-indirect-with-offset-2-s1-indirect-with-pre-increment-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 (${d-An}),${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "swapb.2-d-indirect-2-s1-indirect-with-pre-increment-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 (${d-An})${d-i4-2}++,${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "swapb.2-d-indirect-with-post-increment-2-s1-indirect-with-pre-increment-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* swapb.2 ${d-i4-2}(${d-An})++,${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_SWAPB_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "swapb.2-d-indirect-with-pre-increment-2-s1-indirect-with-pre-increment-2", "swapb.2", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pdec ${d-direct-addr},${pdec-s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_PDEC_D_DIRECT_PDEC_S1_EA_INDIRECT_WITH_OFFSET_4, "pdec-d-direct-pdec-s1-ea-indirect-with-offset-4", "pdec", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* pdec #${d-imm8},${pdec-s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_PDEC_D_IMMEDIATE_4_PDEC_S1_EA_INDIRECT_WITH_OFFSET_4, "pdec-d-immediate-4-pdec-s1-ea-indirect-with-offset-4", "pdec", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* pdec (${d-An},${d-r}),${pdec-s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_PDEC_D_INDIRECT_WITH_INDEX_4_PDEC_S1_EA_INDIRECT_WITH_OFFSET_4, "pdec-d-indirect-with-index-4-pdec-s1-ea-indirect-with-offset-4", "pdec", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* pdec ${d-imm7-4}(${d-An}),${pdec-s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_PDEC_D_INDIRECT_WITH_OFFSET_4_PDEC_S1_EA_INDIRECT_WITH_OFFSET_4, "pdec-d-indirect-with-offset-4-pdec-s1-ea-indirect-with-offset-4", "pdec", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* pdec (${d-An}),${pdec-s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_PDEC_D_INDIRECT_4_PDEC_S1_EA_INDIRECT_WITH_OFFSET_4, "pdec-d-indirect-4-pdec-s1-ea-indirect-with-offset-4", "pdec", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* pdec (${d-An})${d-i4-4}++,${pdec-s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_PDEC_D_INDIRECT_WITH_POST_INCREMENT_4_PDEC_S1_EA_INDIRECT_WITH_OFFSET_4, "pdec-d-indirect-with-post-increment-4-pdec-s1-ea-indirect-with-offset-4", "pdec", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* pdec ${d-i4-4}(${d-An})++,${pdec-s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_PDEC_D_INDIRECT_WITH_PRE_INCREMENT_4_PDEC_S1_EA_INDIRECT_WITH_OFFSET_4, "pdec-d-indirect-with-pre-increment-4-pdec-s1-ea-indirect-with-offset-4", "pdec", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 ${d-direct-addr},(${s1-An}) */
  {
    UBICOM32_INSN_LEA_4_D_DIRECT_S1_EA_INDIRECT, "lea.4-d-direct-s1-ea-indirect", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 #${d-imm8},(${s1-An}) */
  {
    UBICOM32_INSN_LEA_4_D_IMMEDIATE_4_S1_EA_INDIRECT, "lea.4-d-immediate-4-s1-ea-indirect", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 (${d-An},${d-r}),(${s1-An}) */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_INDEX_4_S1_EA_INDIRECT, "lea.4-d-indirect-with-index-4-s1-ea-indirect", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 ${d-imm7-4}(${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_OFFSET_4_S1_EA_INDIRECT, "lea.4-d-indirect-with-offset-4-s1-ea-indirect", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 (${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_4_S1_EA_INDIRECT, "lea.4-d-indirect-4-s1-ea-indirect", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 (${d-An})${d-i4-4}++,(${s1-An}) */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_EA_INDIRECT, "lea.4-d-indirect-with-post-increment-4-s1-ea-indirect", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 ${d-i4-4}(${d-An})++,(${s1-An}) */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_EA_INDIRECT, "lea.4-d-indirect-with-pre-increment-4-s1-ea-indirect", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 ${d-direct-addr},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_LEA_4_D_DIRECT_S1_EA_INDIRECT_WITH_OFFSET_4, "lea.4-d-direct-s1-ea-indirect-with-offset-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 #${d-imm8},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_LEA_4_D_IMMEDIATE_4_S1_EA_INDIRECT_WITH_OFFSET_4, "lea.4-d-immediate-4-s1-ea-indirect-with-offset-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 (${d-An},${d-r}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_INDEX_4_S1_EA_INDIRECT_WITH_OFFSET_4, "lea.4-d-indirect-with-index-4-s1-ea-indirect-with-offset-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_OFFSET_4_S1_EA_INDIRECT_WITH_OFFSET_4, "lea.4-d-indirect-with-offset-4-s1-ea-indirect-with-offset-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 (${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_4_S1_EA_INDIRECT_WITH_OFFSET_4, "lea.4-d-indirect-4-s1-ea-indirect-with-offset-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_EA_INDIRECT_WITH_OFFSET_4, "lea.4-d-indirect-with-post-increment-4-s1-ea-indirect-with-offset-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_EA_INDIRECT_WITH_OFFSET_4, "lea.4-d-indirect-with-pre-increment-4-s1-ea-indirect-with-offset-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 ${d-direct-addr},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_LEA_4_D_DIRECT_S1_EA_INDIRECT_WITH_INDEX_4, "lea.4-d-direct-s1-ea-indirect-with-index-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 #${d-imm8},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_LEA_4_D_IMMEDIATE_4_S1_EA_INDIRECT_WITH_INDEX_4, "lea.4-d-immediate-4-s1-ea-indirect-with-index-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 (${d-An},${d-r}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_INDEX_4_S1_EA_INDIRECT_WITH_INDEX_4, "lea.4-d-indirect-with-index-4-s1-ea-indirect-with-index-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_OFFSET_4_S1_EA_INDIRECT_WITH_INDEX_4, "lea.4-d-indirect-with-offset-4-s1-ea-indirect-with-index-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 (${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_4_S1_EA_INDIRECT_WITH_INDEX_4, "lea.4-d-indirect-4-s1-ea-indirect-with-index-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 (${d-An})${d-i4-4}++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_EA_INDIRECT_WITH_INDEX_4, "lea.4-d-indirect-with-post-increment-4-s1-ea-indirect-with-index-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_EA_INDIRECT_WITH_INDEX_4, "lea.4-d-indirect-with-pre-increment-4-s1-ea-indirect-with-index-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 ${d-direct-addr},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_LEA_4_D_DIRECT_S1_EA_INDIRECT_WITH_POST_INCREMENT_4, "lea.4-d-direct-s1-ea-indirect-with-post-increment-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 #${d-imm8},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_LEA_4_D_IMMEDIATE_4_S1_EA_INDIRECT_WITH_POST_INCREMENT_4, "lea.4-d-immediate-4-s1-ea-indirect-with-post-increment-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 (${d-An},${d-r}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_INDEX_4_S1_EA_INDIRECT_WITH_POST_INCREMENT_4, "lea.4-d-indirect-with-index-4-s1-ea-indirect-with-post-increment-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_OFFSET_4_S1_EA_INDIRECT_WITH_POST_INCREMENT_4, "lea.4-d-indirect-with-offset-4-s1-ea-indirect-with-post-increment-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 (${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_4_S1_EA_INDIRECT_WITH_POST_INCREMENT_4, "lea.4-d-indirect-4-s1-ea-indirect-with-post-increment-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_EA_INDIRECT_WITH_POST_INCREMENT_4, "lea.4-d-indirect-with-post-increment-4-s1-ea-indirect-with-post-increment-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_EA_INDIRECT_WITH_POST_INCREMENT_4, "lea.4-d-indirect-with-pre-increment-4-s1-ea-indirect-with-post-increment-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 ${d-direct-addr},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_LEA_4_D_DIRECT_S1_EA_INDIRECT_WITH_PRE_INCREMENT_4, "lea.4-d-direct-s1-ea-indirect-with-pre-increment-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 #${d-imm8},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_LEA_4_D_IMMEDIATE_4_S1_EA_INDIRECT_WITH_PRE_INCREMENT_4, "lea.4-d-immediate-4-s1-ea-indirect-with-pre-increment-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 (${d-An},${d-r}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_INDEX_4_S1_EA_INDIRECT_WITH_PRE_INCREMENT_4, "lea.4-d-indirect-with-index-4-s1-ea-indirect-with-pre-increment-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_OFFSET_4_S1_EA_INDIRECT_WITH_PRE_INCREMENT_4, "lea.4-d-indirect-with-offset-4-s1-ea-indirect-with-pre-increment-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 (${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_4_S1_EA_INDIRECT_WITH_PRE_INCREMENT_4, "lea.4-d-indirect-4-s1-ea-indirect-with-pre-increment-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_EA_INDIRECT_WITH_PRE_INCREMENT_4, "lea.4-d-indirect-with-post-increment-4-s1-ea-indirect-with-pre-increment-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_EA_INDIRECT_WITH_PRE_INCREMENT_4, "lea.4-d-indirect-with-pre-increment-4-s1-ea-indirect-with-pre-increment-4", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 ${d-direct-addr},#${s1-imm8} */
  {
    UBICOM32_INSN_LEA_4_D_DIRECT_S1_EA_IMMEDIATE, "lea.4-d-direct-s1-ea-immediate", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 #${d-imm8},#${s1-imm8} */
  {
    UBICOM32_INSN_LEA_4_D_IMMEDIATE_4_S1_EA_IMMEDIATE, "lea.4-d-immediate-4-s1-ea-immediate", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 (${d-An},${d-r}),#${s1-imm8} */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_INDEX_4_S1_EA_IMMEDIATE, "lea.4-d-indirect-with-index-4-s1-ea-immediate", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 ${d-imm7-4}(${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_OFFSET_4_S1_EA_IMMEDIATE, "lea.4-d-indirect-with-offset-4-s1-ea-immediate", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 (${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_4_S1_EA_IMMEDIATE, "lea.4-d-indirect-4-s1-ea-immediate", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 (${d-An})${d-i4-4}++,#${s1-imm8} */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_EA_IMMEDIATE, "lea.4-d-indirect-with-post-increment-4-s1-ea-immediate", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.4 ${d-i4-4}(${d-An})++,#${s1-imm8} */
  {
    UBICOM32_INSN_LEA_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_EA_IMMEDIATE, "lea.4-d-indirect-with-pre-increment-4-s1-ea-immediate", "lea.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 ${d-direct-addr},(${s1-An}) */
  {
    UBICOM32_INSN_LEA_2_D_DIRECT_S1_EA_INDIRECT, "lea.2-d-direct-s1-ea-indirect", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 #${d-imm8},(${s1-An}) */
  {
    UBICOM32_INSN_LEA_2_D_IMMEDIATE_4_S1_EA_INDIRECT, "lea.2-d-immediate-4-s1-ea-indirect", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 (${d-An},${d-r}),(${s1-An}) */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_INDEX_4_S1_EA_INDIRECT, "lea.2-d-indirect-with-index-4-s1-ea-indirect", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 ${d-imm7-4}(${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_OFFSET_4_S1_EA_INDIRECT, "lea.2-d-indirect-with-offset-4-s1-ea-indirect", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 (${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_4_S1_EA_INDIRECT, "lea.2-d-indirect-4-s1-ea-indirect", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 (${d-An})${d-i4-4}++,(${s1-An}) */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_POST_INCREMENT_4_S1_EA_INDIRECT, "lea.2-d-indirect-with-post-increment-4-s1-ea-indirect", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 ${d-i4-4}(${d-An})++,(${s1-An}) */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_EA_INDIRECT, "lea.2-d-indirect-with-pre-increment-4-s1-ea-indirect", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 ${d-direct-addr},${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_LEA_2_D_DIRECT_S1_EA_INDIRECT_WITH_OFFSET_2, "lea.2-d-direct-s1-ea-indirect-with-offset-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 #${d-imm8},${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_LEA_2_D_IMMEDIATE_4_S1_EA_INDIRECT_WITH_OFFSET_2, "lea.2-d-immediate-4-s1-ea-indirect-with-offset-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 (${d-An},${d-r}),${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_INDEX_4_S1_EA_INDIRECT_WITH_OFFSET_2, "lea.2-d-indirect-with-index-4-s1-ea-indirect-with-offset-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 ${d-imm7-4}(${d-An}),${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_OFFSET_4_S1_EA_INDIRECT_WITH_OFFSET_2, "lea.2-d-indirect-with-offset-4-s1-ea-indirect-with-offset-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 (${d-An}),${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_4_S1_EA_INDIRECT_WITH_OFFSET_2, "lea.2-d-indirect-4-s1-ea-indirect-with-offset-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 (${d-An})${d-i4-4}++,${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_POST_INCREMENT_4_S1_EA_INDIRECT_WITH_OFFSET_2, "lea.2-d-indirect-with-post-increment-4-s1-ea-indirect-with-offset-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 ${d-i4-4}(${d-An})++,${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_EA_INDIRECT_WITH_OFFSET_2, "lea.2-d-indirect-with-pre-increment-4-s1-ea-indirect-with-offset-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 ${d-direct-addr},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_LEA_2_D_DIRECT_S1_EA_INDIRECT_WITH_INDEX_2, "lea.2-d-direct-s1-ea-indirect-with-index-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 #${d-imm8},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_LEA_2_D_IMMEDIATE_4_S1_EA_INDIRECT_WITH_INDEX_2, "lea.2-d-immediate-4-s1-ea-indirect-with-index-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 (${d-An},${d-r}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_INDEX_4_S1_EA_INDIRECT_WITH_INDEX_2, "lea.2-d-indirect-with-index-4-s1-ea-indirect-with-index-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_OFFSET_4_S1_EA_INDIRECT_WITH_INDEX_2, "lea.2-d-indirect-with-offset-4-s1-ea-indirect-with-index-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 (${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_4_S1_EA_INDIRECT_WITH_INDEX_2, "lea.2-d-indirect-4-s1-ea-indirect-with-index-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 (${d-An})${d-i4-4}++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_POST_INCREMENT_4_S1_EA_INDIRECT_WITH_INDEX_2, "lea.2-d-indirect-with-post-increment-4-s1-ea-indirect-with-index-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_EA_INDIRECT_WITH_INDEX_2, "lea.2-d-indirect-with-pre-increment-4-s1-ea-indirect-with-index-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 ${d-direct-addr},(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_LEA_2_D_DIRECT_S1_EA_INDIRECT_WITH_POST_INCREMENT_2, "lea.2-d-direct-s1-ea-indirect-with-post-increment-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 #${d-imm8},(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_LEA_2_D_IMMEDIATE_4_S1_EA_INDIRECT_WITH_POST_INCREMENT_2, "lea.2-d-immediate-4-s1-ea-indirect-with-post-increment-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 (${d-An},${d-r}),(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_INDEX_4_S1_EA_INDIRECT_WITH_POST_INCREMENT_2, "lea.2-d-indirect-with-index-4-s1-ea-indirect-with-post-increment-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_OFFSET_4_S1_EA_INDIRECT_WITH_POST_INCREMENT_2, "lea.2-d-indirect-with-offset-4-s1-ea-indirect-with-post-increment-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 (${d-An}),(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_4_S1_EA_INDIRECT_WITH_POST_INCREMENT_2, "lea.2-d-indirect-4-s1-ea-indirect-with-post-increment-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_POST_INCREMENT_4_S1_EA_INDIRECT_WITH_POST_INCREMENT_2, "lea.2-d-indirect-with-post-increment-4-s1-ea-indirect-with-post-increment-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_EA_INDIRECT_WITH_POST_INCREMENT_2, "lea.2-d-indirect-with-pre-increment-4-s1-ea-indirect-with-post-increment-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 ${d-direct-addr},${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_LEA_2_D_DIRECT_S1_EA_INDIRECT_WITH_PRE_INCREMENT_2, "lea.2-d-direct-s1-ea-indirect-with-pre-increment-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 #${d-imm8},${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_LEA_2_D_IMMEDIATE_4_S1_EA_INDIRECT_WITH_PRE_INCREMENT_2, "lea.2-d-immediate-4-s1-ea-indirect-with-pre-increment-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 (${d-An},${d-r}),${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_INDEX_4_S1_EA_INDIRECT_WITH_PRE_INCREMENT_2, "lea.2-d-indirect-with-index-4-s1-ea-indirect-with-pre-increment-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 ${d-imm7-4}(${d-An}),${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_OFFSET_4_S1_EA_INDIRECT_WITH_PRE_INCREMENT_2, "lea.2-d-indirect-with-offset-4-s1-ea-indirect-with-pre-increment-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 (${d-An}),${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_4_S1_EA_INDIRECT_WITH_PRE_INCREMENT_2, "lea.2-d-indirect-4-s1-ea-indirect-with-pre-increment-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 (${d-An})${d-i4-4}++,${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_POST_INCREMENT_4_S1_EA_INDIRECT_WITH_PRE_INCREMENT_2, "lea.2-d-indirect-with-post-increment-4-s1-ea-indirect-with-pre-increment-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 ${d-i4-4}(${d-An})++,${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_EA_INDIRECT_WITH_PRE_INCREMENT_2, "lea.2-d-indirect-with-pre-increment-4-s1-ea-indirect-with-pre-increment-2", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 ${d-direct-addr},#${s1-imm8} */
  {
    UBICOM32_INSN_LEA_2_D_DIRECT_S1_EA_IMMEDIATE, "lea.2-d-direct-s1-ea-immediate", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 #${d-imm8},#${s1-imm8} */
  {
    UBICOM32_INSN_LEA_2_D_IMMEDIATE_4_S1_EA_IMMEDIATE, "lea.2-d-immediate-4-s1-ea-immediate", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 (${d-An},${d-r}),#${s1-imm8} */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_INDEX_4_S1_EA_IMMEDIATE, "lea.2-d-indirect-with-index-4-s1-ea-immediate", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 ${d-imm7-4}(${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_OFFSET_4_S1_EA_IMMEDIATE, "lea.2-d-indirect-with-offset-4-s1-ea-immediate", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 (${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_4_S1_EA_IMMEDIATE, "lea.2-d-indirect-4-s1-ea-immediate", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 (${d-An})${d-i4-4}++,#${s1-imm8} */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_POST_INCREMENT_4_S1_EA_IMMEDIATE, "lea.2-d-indirect-with-post-increment-4-s1-ea-immediate", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.2 ${d-i4-4}(${d-An})++,#${s1-imm8} */
  {
    UBICOM32_INSN_LEA_2_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_EA_IMMEDIATE, "lea.2-d-indirect-with-pre-increment-4-s1-ea-immediate", "lea.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 ${d-direct-addr},(${s1-An}) */
  {
    UBICOM32_INSN_LEA_1_D_DIRECT_S1_EA_INDIRECT, "lea.1-d-direct-s1-ea-indirect", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 #${d-imm8},(${s1-An}) */
  {
    UBICOM32_INSN_LEA_1_D_IMMEDIATE_4_S1_EA_INDIRECT, "lea.1-d-immediate-4-s1-ea-indirect", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 (${d-An},${d-r}),(${s1-An}) */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_INDEX_4_S1_EA_INDIRECT, "lea.1-d-indirect-with-index-4-s1-ea-indirect", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 ${d-imm7-4}(${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_OFFSET_4_S1_EA_INDIRECT, "lea.1-d-indirect-with-offset-4-s1-ea-indirect", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 (${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_4_S1_EA_INDIRECT, "lea.1-d-indirect-4-s1-ea-indirect", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 (${d-An})${d-i4-4}++,(${s1-An}) */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_POST_INCREMENT_4_S1_EA_INDIRECT, "lea.1-d-indirect-with-post-increment-4-s1-ea-indirect", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 ${d-i4-4}(${d-An})++,(${s1-An}) */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_EA_INDIRECT, "lea.1-d-indirect-with-pre-increment-4-s1-ea-indirect", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 ${d-direct-addr},${s1-imm7-1}(${s1-An}) */
  {
    UBICOM32_INSN_LEA_1_D_DIRECT_S1_EA_INDIRECT_WITH_OFFSET_1, "lea.1-d-direct-s1-ea-indirect-with-offset-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 #${d-imm8},${s1-imm7-1}(${s1-An}) */
  {
    UBICOM32_INSN_LEA_1_D_IMMEDIATE_4_S1_EA_INDIRECT_WITH_OFFSET_1, "lea.1-d-immediate-4-s1-ea-indirect-with-offset-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 (${d-An},${d-r}),${s1-imm7-1}(${s1-An}) */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_INDEX_4_S1_EA_INDIRECT_WITH_OFFSET_1, "lea.1-d-indirect-with-index-4-s1-ea-indirect-with-offset-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 ${d-imm7-4}(${d-An}),${s1-imm7-1}(${s1-An}) */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_OFFSET_4_S1_EA_INDIRECT_WITH_OFFSET_1, "lea.1-d-indirect-with-offset-4-s1-ea-indirect-with-offset-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 (${d-An}),${s1-imm7-1}(${s1-An}) */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_4_S1_EA_INDIRECT_WITH_OFFSET_1, "lea.1-d-indirect-4-s1-ea-indirect-with-offset-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 (${d-An})${d-i4-4}++,${s1-imm7-1}(${s1-An}) */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_POST_INCREMENT_4_S1_EA_INDIRECT_WITH_OFFSET_1, "lea.1-d-indirect-with-post-increment-4-s1-ea-indirect-with-offset-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 ${d-i4-4}(${d-An})++,${s1-imm7-1}(${s1-An}) */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_EA_INDIRECT_WITH_OFFSET_1, "lea.1-d-indirect-with-pre-increment-4-s1-ea-indirect-with-offset-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 ${d-direct-addr},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_LEA_1_D_DIRECT_S1_EA_INDIRECT_WITH_INDEX_1, "lea.1-d-direct-s1-ea-indirect-with-index-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 #${d-imm8},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_LEA_1_D_IMMEDIATE_4_S1_EA_INDIRECT_WITH_INDEX_1, "lea.1-d-immediate-4-s1-ea-indirect-with-index-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 (${d-An},${d-r}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_INDEX_4_S1_EA_INDIRECT_WITH_INDEX_1, "lea.1-d-indirect-with-index-4-s1-ea-indirect-with-index-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_OFFSET_4_S1_EA_INDIRECT_WITH_INDEX_1, "lea.1-d-indirect-with-offset-4-s1-ea-indirect-with-index-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 (${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_4_S1_EA_INDIRECT_WITH_INDEX_1, "lea.1-d-indirect-4-s1-ea-indirect-with-index-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 (${d-An})${d-i4-4}++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_POST_INCREMENT_4_S1_EA_INDIRECT_WITH_INDEX_1, "lea.1-d-indirect-with-post-increment-4-s1-ea-indirect-with-index-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_EA_INDIRECT_WITH_INDEX_1, "lea.1-d-indirect-with-pre-increment-4-s1-ea-indirect-with-index-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 ${d-direct-addr},(${s1-An})${s1-i4-1}++ */
  {
    UBICOM32_INSN_LEA_1_D_DIRECT_S1_EA_INDIRECT_WITH_POST_INCREMENT_1, "lea.1-d-direct-s1-ea-indirect-with-post-increment-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 #${d-imm8},(${s1-An})${s1-i4-1}++ */
  {
    UBICOM32_INSN_LEA_1_D_IMMEDIATE_4_S1_EA_INDIRECT_WITH_POST_INCREMENT_1, "lea.1-d-immediate-4-s1-ea-indirect-with-post-increment-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 (${d-An},${d-r}),(${s1-An})${s1-i4-1}++ */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_INDEX_4_S1_EA_INDIRECT_WITH_POST_INCREMENT_1, "lea.1-d-indirect-with-index-4-s1-ea-indirect-with-post-increment-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-1}++ */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_OFFSET_4_S1_EA_INDIRECT_WITH_POST_INCREMENT_1, "lea.1-d-indirect-with-offset-4-s1-ea-indirect-with-post-increment-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 (${d-An}),(${s1-An})${s1-i4-1}++ */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_4_S1_EA_INDIRECT_WITH_POST_INCREMENT_1, "lea.1-d-indirect-4-s1-ea-indirect-with-post-increment-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-1}++ */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_POST_INCREMENT_4_S1_EA_INDIRECT_WITH_POST_INCREMENT_1, "lea.1-d-indirect-with-post-increment-4-s1-ea-indirect-with-post-increment-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-1}++ */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_EA_INDIRECT_WITH_POST_INCREMENT_1, "lea.1-d-indirect-with-pre-increment-4-s1-ea-indirect-with-post-increment-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 ${d-direct-addr},${s1-i4-1}(${s1-An})++ */
  {
    UBICOM32_INSN_LEA_1_D_DIRECT_S1_EA_INDIRECT_WITH_PRE_INCREMENT_1, "lea.1-d-direct-s1-ea-indirect-with-pre-increment-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 #${d-imm8},${s1-i4-1}(${s1-An})++ */
  {
    UBICOM32_INSN_LEA_1_D_IMMEDIATE_4_S1_EA_INDIRECT_WITH_PRE_INCREMENT_1, "lea.1-d-immediate-4-s1-ea-indirect-with-pre-increment-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 (${d-An},${d-r}),${s1-i4-1}(${s1-An})++ */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_INDEX_4_S1_EA_INDIRECT_WITH_PRE_INCREMENT_1, "lea.1-d-indirect-with-index-4-s1-ea-indirect-with-pre-increment-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 ${d-imm7-4}(${d-An}),${s1-i4-1}(${s1-An})++ */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_OFFSET_4_S1_EA_INDIRECT_WITH_PRE_INCREMENT_1, "lea.1-d-indirect-with-offset-4-s1-ea-indirect-with-pre-increment-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 (${d-An}),${s1-i4-1}(${s1-An})++ */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_4_S1_EA_INDIRECT_WITH_PRE_INCREMENT_1, "lea.1-d-indirect-4-s1-ea-indirect-with-pre-increment-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 (${d-An})${d-i4-4}++,${s1-i4-1}(${s1-An})++ */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_POST_INCREMENT_4_S1_EA_INDIRECT_WITH_PRE_INCREMENT_1, "lea.1-d-indirect-with-post-increment-4-s1-ea-indirect-with-pre-increment-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 ${d-i4-4}(${d-An})++,${s1-i4-1}(${s1-An})++ */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_EA_INDIRECT_WITH_PRE_INCREMENT_1, "lea.1-d-indirect-with-pre-increment-4-s1-ea-indirect-with-pre-increment-1", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 ${d-direct-addr},#${s1-imm8} */
  {
    UBICOM32_INSN_LEA_1_D_DIRECT_S1_EA_IMMEDIATE, "lea.1-d-direct-s1-ea-immediate", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 #${d-imm8},#${s1-imm8} */
  {
    UBICOM32_INSN_LEA_1_D_IMMEDIATE_4_S1_EA_IMMEDIATE, "lea.1-d-immediate-4-s1-ea-immediate", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 (${d-An},${d-r}),#${s1-imm8} */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_INDEX_4_S1_EA_IMMEDIATE, "lea.1-d-indirect-with-index-4-s1-ea-immediate", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 ${d-imm7-4}(${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_OFFSET_4_S1_EA_IMMEDIATE, "lea.1-d-indirect-with-offset-4-s1-ea-immediate", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 (${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_4_S1_EA_IMMEDIATE, "lea.1-d-indirect-4-s1-ea-immediate", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 (${d-An})${d-i4-4}++,#${s1-imm8} */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_POST_INCREMENT_4_S1_EA_IMMEDIATE, "lea.1-d-indirect-with-post-increment-4-s1-ea-immediate", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* lea.1 ${d-i4-4}(${d-An})++,#${s1-imm8} */
  {
    UBICOM32_INSN_LEA_1_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_EA_IMMEDIATE, "lea.1-d-indirect-with-pre-increment-4-s1-ea-immediate", "lea.1", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* cmpi ${s1-direct-addr},#${imm16-1} */
  {
    UBICOM32_INSN_CMPI_S1_DIRECT, "cmpi-s1-direct", "cmpi", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* cmpi #${s1-imm8},#${imm16-1} */
  {
    UBICOM32_INSN_CMPI_S1_IMMEDIATE, "cmpi-s1-immediate", "cmpi", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* cmpi (${s1-An},${s1-r}),#${imm16-1} */
  {
    UBICOM32_INSN_CMPI_S1_INDIRECT_WITH_INDEX_2, "cmpi-s1-indirect-with-index-2", "cmpi", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* cmpi ${s1-imm7-2}(${s1-An}),#${imm16-1} */
  {
    UBICOM32_INSN_CMPI_S1_INDIRECT_WITH_OFFSET_2, "cmpi-s1-indirect-with-offset-2", "cmpi", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* cmpi (${s1-An}),#${imm16-1} */
  {
    UBICOM32_INSN_CMPI_S1_INDIRECT_2, "cmpi-s1-indirect-2", "cmpi", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* cmpi (${s1-An})${s1-i4-2}++,#${imm16-1} */
  {
    UBICOM32_INSN_CMPI_S1_INDIRECT_WITH_POST_INCREMENT_2, "cmpi-s1-indirect-with-post-increment-2", "cmpi", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* cmpi ${s1-i4-2}(${s1-An})++,#${imm16-1} */
  {
    UBICOM32_INSN_CMPI_S1_INDIRECT_WITH_PRE_INCREMENT_2, "cmpi-s1-indirect-with-pre-increment-2", "cmpi", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* pxadds.u ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_DIRECT_S1_DIRECT, "pxadds.u-d-direct-s1-direct", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_IMMEDIATE_2_S1_DIRECT, "pxadds.u-d-immediate-2-s1-direct", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_INDEX_2_S1_DIRECT, "pxadds.u-d-indirect-with-index-2-s1-direct", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u ${d-imm7-2}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_OFFSET_2_S1_DIRECT, "pxadds.u-d-indirect-with-offset-2-s1-direct", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_2_S1_DIRECT, "pxadds.u-d-indirect-2-s1-direct", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u (${d-An})${d-i4-2}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_POST_INCREMENT_2_S1_DIRECT, "pxadds.u-d-indirect-with-post-increment-2-s1-direct", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u ${d-i4-2}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_DIRECT, "pxadds.u-d-indirect-with-pre-increment-2-s1-direct", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_DIRECT_S1_IMMEDIATE, "pxadds.u-d-direct-s1-immediate", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_IMMEDIATE_2_S1_IMMEDIATE, "pxadds.u-d-immediate-2-s1-immediate", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_INDEX_2_S1_IMMEDIATE, "pxadds.u-d-indirect-with-index-2-s1-immediate", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u ${d-imm7-2}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_OFFSET_2_S1_IMMEDIATE, "pxadds.u-d-indirect-with-offset-2-s1-immediate", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_2_S1_IMMEDIATE, "pxadds.u-d-indirect-2-s1-immediate", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u (${d-An})${d-i4-2}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_POST_INCREMENT_2_S1_IMMEDIATE, "pxadds.u-d-indirect-with-post-increment-2-s1-immediate", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u ${d-i4-2}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_IMMEDIATE, "pxadds.u-d-indirect-with-pre-increment-2-s1-immediate", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "pxadds.u-d-direct-s1-indirect-with-index-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_IMMEDIATE_2_S1_INDIRECT_WITH_INDEX_4, "pxadds.u-d-immediate-2-s1-indirect-with-index-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_INDEX_4, "pxadds.u-d-indirect-with-index-2-s1-indirect-with-index-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u ${d-imm7-2}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_INDEX_4, "pxadds.u-d-indirect-with-offset-2-s1-indirect-with-index-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_2_S1_INDIRECT_WITH_INDEX_4, "pxadds.u-d-indirect-2-s1-indirect-with-index-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u (${d-An})${d-i4-2}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_INDEX_4, "pxadds.u-d-indirect-with-post-increment-2-s1-indirect-with-index-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u ${d-i4-2}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_INDEX_4, "pxadds.u-d-indirect-with-pre-increment-2-s1-indirect-with-index-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u ${d-direct-addr},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "pxadds.u-d-direct-s1-indirect-with-offset-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u #${d-imm8},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_IMMEDIATE_2_S1_INDIRECT_WITH_OFFSET_4, "pxadds.u-d-immediate-2-s1-indirect-with-offset-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u (${d-An},${d-r}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_OFFSET_4, "pxadds.u-d-indirect-with-index-2-s1-indirect-with-offset-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u ${d-imm7-2}(${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_OFFSET_4, "pxadds.u-d-indirect-with-offset-2-s1-indirect-with-offset-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u (${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_2_S1_INDIRECT_WITH_OFFSET_4, "pxadds.u-d-indirect-2-s1-indirect-with-offset-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u (${d-An})${d-i4-2}++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_4, "pxadds.u-d-indirect-with-post-increment-2-s1-indirect-with-offset-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u ${d-i4-2}(${d-An})++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_4, "pxadds.u-d-indirect-with-pre-increment-2-s1-indirect-with-offset-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_DIRECT_S1_INDIRECT_4, "pxadds.u-d-direct-s1-indirect-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_IMMEDIATE_2_S1_INDIRECT_4, "pxadds.u-d-immediate-2-s1-indirect-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_4, "pxadds.u-d-indirect-with-index-2-s1-indirect-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u ${d-imm7-2}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_4, "pxadds.u-d-indirect-with-offset-2-s1-indirect-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_2_S1_INDIRECT_4, "pxadds.u-d-indirect-2-s1-indirect-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u (${d-An})${d-i4-2}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_4, "pxadds.u-d-indirect-with-post-increment-2-s1-indirect-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u ${d-i4-2}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_4, "pxadds.u-d-indirect-with-pre-increment-2-s1-indirect-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u ${d-direct-addr},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxadds.u-d-direct-s1-indirect-with-post-increment-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u #${d-imm8},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_IMMEDIATE_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxadds.u-d-immediate-2-s1-indirect-with-post-increment-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u (${d-An},${d-r}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxadds.u-d-indirect-with-index-2-s1-indirect-with-post-increment-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u ${d-imm7-2}(${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxadds.u-d-indirect-with-offset-2-s1-indirect-with-post-increment-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u (${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxadds.u-d-indirect-2-s1-indirect-with-post-increment-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u (${d-An})${d-i4-2}++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxadds.u-d-indirect-with-post-increment-2-s1-indirect-with-post-increment-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u ${d-i4-2}(${d-An})++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxadds.u-d-indirect-with-pre-increment-2-s1-indirect-with-post-increment-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u ${d-direct-addr},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxadds.u-d-direct-s1-indirect-with-pre-increment-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u #${d-imm8},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_IMMEDIATE_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxadds.u-d-immediate-2-s1-indirect-with-pre-increment-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u (${d-An},${d-r}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxadds.u-d-indirect-with-index-2-s1-indirect-with-pre-increment-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u ${d-imm7-2}(${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxadds.u-d-indirect-with-offset-2-s1-indirect-with-pre-increment-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u (${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxadds.u-d-indirect-2-s1-indirect-with-pre-increment-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u (${d-An})${d-i4-2}++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxadds.u-d-indirect-with-post-increment-2-s1-indirect-with-pre-increment-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds.u ${d-i4-2}(${d-An})++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXADDS_U_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxadds.u-d-indirect-with-pre-increment-2-s1-indirect-with-pre-increment-4", "pxadds.u", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXADDS_D_DIRECT_S1_DIRECT, "pxadds-d-direct-s1-direct", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXADDS_D_IMMEDIATE_2_S1_DIRECT, "pxadds-d-immediate-2-s1-direct", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_INDEX_2_S1_DIRECT, "pxadds-d-indirect-with-index-2-s1-direct", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds ${d-imm7-2}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_OFFSET_2_S1_DIRECT, "pxadds-d-indirect-with-offset-2-s1-direct", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_2_S1_DIRECT, "pxadds-d-indirect-2-s1-direct", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds (${d-An})${d-i4-2}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_POST_INCREMENT_2_S1_DIRECT, "pxadds-d-indirect-with-post-increment-2-s1-direct", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds ${d-i4-2}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_DIRECT, "pxadds-d-indirect-with-pre-increment-2-s1-direct", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXADDS_D_DIRECT_S1_IMMEDIATE, "pxadds-d-direct-s1-immediate", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXADDS_D_IMMEDIATE_2_S1_IMMEDIATE, "pxadds-d-immediate-2-s1-immediate", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_INDEX_2_S1_IMMEDIATE, "pxadds-d-indirect-with-index-2-s1-immediate", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds ${d-imm7-2}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_OFFSET_2_S1_IMMEDIATE, "pxadds-d-indirect-with-offset-2-s1-immediate", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_2_S1_IMMEDIATE, "pxadds-d-indirect-2-s1-immediate", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds (${d-An})${d-i4-2}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_POST_INCREMENT_2_S1_IMMEDIATE, "pxadds-d-indirect-with-post-increment-2-s1-immediate", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds ${d-i4-2}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_IMMEDIATE, "pxadds-d-indirect-with-pre-increment-2-s1-immediate", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXADDS_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "pxadds-d-direct-s1-indirect-with-index-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXADDS_D_IMMEDIATE_2_S1_INDIRECT_WITH_INDEX_4, "pxadds-d-immediate-2-s1-indirect-with-index-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_INDEX_4, "pxadds-d-indirect-with-index-2-s1-indirect-with-index-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds ${d-imm7-2}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_INDEX_4, "pxadds-d-indirect-with-offset-2-s1-indirect-with-index-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_2_S1_INDIRECT_WITH_INDEX_4, "pxadds-d-indirect-2-s1-indirect-with-index-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds (${d-An})${d-i4-2}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_INDEX_4, "pxadds-d-indirect-with-post-increment-2-s1-indirect-with-index-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds ${d-i4-2}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_INDEX_4, "pxadds-d-indirect-with-pre-increment-2-s1-indirect-with-index-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds ${d-direct-addr},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "pxadds-d-direct-s1-indirect-with-offset-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds #${d-imm8},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_D_IMMEDIATE_2_S1_INDIRECT_WITH_OFFSET_4, "pxadds-d-immediate-2-s1-indirect-with-offset-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds (${d-An},${d-r}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_OFFSET_4, "pxadds-d-indirect-with-index-2-s1-indirect-with-offset-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds ${d-imm7-2}(${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_OFFSET_4, "pxadds-d-indirect-with-offset-2-s1-indirect-with-offset-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds (${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_2_S1_INDIRECT_WITH_OFFSET_4, "pxadds-d-indirect-2-s1-indirect-with-offset-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds (${d-An})${d-i4-2}++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_4, "pxadds-d-indirect-with-post-increment-2-s1-indirect-with-offset-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds ${d-i4-2}(${d-An})++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_4, "pxadds-d-indirect-with-pre-increment-2-s1-indirect-with-offset-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_D_DIRECT_S1_INDIRECT_4, "pxadds-d-direct-s1-indirect-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_D_IMMEDIATE_2_S1_INDIRECT_4, "pxadds-d-immediate-2-s1-indirect-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_4, "pxadds-d-indirect-with-index-2-s1-indirect-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds ${d-imm7-2}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_4, "pxadds-d-indirect-with-offset-2-s1-indirect-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_2_S1_INDIRECT_4, "pxadds-d-indirect-2-s1-indirect-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds (${d-An})${d-i4-2}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_4, "pxadds-d-indirect-with-post-increment-2-s1-indirect-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds ${d-i4-2}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_4, "pxadds-d-indirect-with-pre-increment-2-s1-indirect-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds ${d-direct-addr},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXADDS_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxadds-d-direct-s1-indirect-with-post-increment-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds #${d-imm8},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXADDS_D_IMMEDIATE_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxadds-d-immediate-2-s1-indirect-with-post-increment-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds (${d-An},${d-r}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxadds-d-indirect-with-index-2-s1-indirect-with-post-increment-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds ${d-imm7-2}(${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxadds-d-indirect-with-offset-2-s1-indirect-with-post-increment-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds (${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxadds-d-indirect-2-s1-indirect-with-post-increment-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds (${d-An})${d-i4-2}++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxadds-d-indirect-with-post-increment-2-s1-indirect-with-post-increment-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds ${d-i4-2}(${d-An})++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxadds-d-indirect-with-pre-increment-2-s1-indirect-with-post-increment-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds ${d-direct-addr},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXADDS_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxadds-d-direct-s1-indirect-with-pre-increment-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds #${d-imm8},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXADDS_D_IMMEDIATE_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxadds-d-immediate-2-s1-indirect-with-pre-increment-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds (${d-An},${d-r}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxadds-d-indirect-with-index-2-s1-indirect-with-pre-increment-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds ${d-imm7-2}(${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxadds-d-indirect-with-offset-2-s1-indirect-with-pre-increment-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds (${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxadds-d-indirect-2-s1-indirect-with-pre-increment-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds (${d-An})${d-i4-2}++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxadds-d-indirect-with-post-increment-2-s1-indirect-with-pre-increment-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxadds ${d-i4-2}(${d-An})++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXADDS_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxadds-d-indirect-with-pre-increment-2-s1-indirect-with-pre-increment-4", "pxadds", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxhi.s ${Dn},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXHI_S_S1_DIRECT, "pxhi.s-s1-direct", "pxhi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxhi.s ${Dn},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXHI_S_S1_IMMEDIATE, "pxhi.s-s1-immediate", "pxhi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxhi.s ${Dn},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXHI_S_S1_INDIRECT_WITH_INDEX_4, "pxhi.s-s1-indirect-with-index-4", "pxhi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxhi.s ${Dn},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXHI_S_S1_INDIRECT_WITH_OFFSET_4, "pxhi.s-s1-indirect-with-offset-4", "pxhi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxhi.s ${Dn},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXHI_S_S1_INDIRECT_4, "pxhi.s-s1-indirect-4", "pxhi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxhi.s ${Dn},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXHI_S_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxhi.s-s1-indirect-with-post-increment-4", "pxhi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxhi.s ${Dn},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXHI_S_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxhi.s-s1-indirect-with-pre-increment-4", "pxhi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxhi ${Dn},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXHI_S1_DIRECT, "pxhi-s1-direct", "pxhi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxhi ${Dn},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXHI_S1_IMMEDIATE, "pxhi-s1-immediate", "pxhi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxhi ${Dn},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXHI_S1_INDIRECT_WITH_INDEX_4, "pxhi-s1-indirect-with-index-4", "pxhi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxhi ${Dn},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXHI_S1_INDIRECT_WITH_OFFSET_4, "pxhi-s1-indirect-with-offset-4", "pxhi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxhi ${Dn},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXHI_S1_INDIRECT_4, "pxhi-s1-indirect-4", "pxhi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxhi ${Dn},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXHI_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxhi-s1-indirect-with-post-increment-4", "pxhi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxhi ${Dn},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXHI_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxhi-s1-indirect-with-pre-increment-4", "pxhi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_DIRECT_S1_DIRECT, "pxvi.s-d-direct-s1-direct", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_IMMEDIATE_4_S1_DIRECT, "pxvi.s-d-immediate-4-s1-direct", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_INDEX_4_S1_DIRECT, "pxvi.s-d-indirect-with-index-4-s1-direct", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s ${d-imm7-4}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT, "pxvi.s-d-indirect-with-offset-4-s1-direct", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_4_S1_DIRECT, "pxvi.s-d-indirect-4-s1-direct", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s (${d-An})${d-i4-4}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_POST_INCREMENT_4_S1_DIRECT, "pxvi.s-d-indirect-with-post-increment-4-s1-direct", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s ${d-i4-4}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT, "pxvi.s-d-indirect-with-pre-increment-4-s1-direct", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_DIRECT_S1_IMMEDIATE, "pxvi.s-d-direct-s1-immediate", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_IMMEDIATE_4_S1_IMMEDIATE, "pxvi.s-d-immediate-4-s1-immediate", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_INDEX_4_S1_IMMEDIATE, "pxvi.s-d-indirect-with-index-4-s1-immediate", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s ${d-imm7-4}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_OFFSET_4_S1_IMMEDIATE, "pxvi.s-d-indirect-with-offset-4-s1-immediate", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_4_S1_IMMEDIATE, "pxvi.s-d-indirect-4-s1-immediate", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s (${d-An})${d-i4-4}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_POST_INCREMENT_4_S1_IMMEDIATE, "pxvi.s-d-indirect-with-post-increment-4-s1-immediate", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s ${d-i4-4}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_IMMEDIATE, "pxvi.s-d-indirect-with-pre-increment-4-s1-immediate", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "pxvi.s-d-direct-s1-indirect-with-index-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_IMMEDIATE_4_S1_INDIRECT_WITH_INDEX_4, "pxvi.s-d-immediate-4-s1-indirect-with-index-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_INDEX_4, "pxvi.s-d-indirect-with-index-4-s1-indirect-with-index-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_INDEX_4, "pxvi.s-d-indirect-with-offset-4-s1-indirect-with-index-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_4_S1_INDIRECT_WITH_INDEX_4, "pxvi.s-d-indirect-4-s1-indirect-with-index-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s (${d-An})${d-i4-4}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "pxvi.s-d-indirect-with-post-increment-4-s1-indirect-with-index-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "pxvi.s-d-indirect-with-pre-increment-4-s1-indirect-with-index-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s ${d-direct-addr},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "pxvi.s-d-direct-s1-indirect-with-offset-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s #${d-imm8},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_IMMEDIATE_4_S1_INDIRECT_WITH_OFFSET_4, "pxvi.s-d-immediate-4-s1-indirect-with-offset-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s (${d-An},${d-r}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_OFFSET_4, "pxvi.s-d-indirect-with-index-4-s1-indirect-with-offset-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_OFFSET_4, "pxvi.s-d-indirect-with-offset-4-s1-indirect-with-offset-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s (${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_4_S1_INDIRECT_WITH_OFFSET_4, "pxvi.s-d-indirect-4-s1-indirect-with-offset-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "pxvi.s-d-indirect-with-post-increment-4-s1-indirect-with-offset-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "pxvi.s-d-indirect-with-pre-increment-4-s1-indirect-with-offset-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_DIRECT_S1_INDIRECT_4, "pxvi.s-d-direct-s1-indirect-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_IMMEDIATE_4_S1_INDIRECT_4, "pxvi.s-d-immediate-4-s1-indirect-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_4, "pxvi.s-d-indirect-with-index-4-s1-indirect-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s ${d-imm7-4}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_4, "pxvi.s-d-indirect-with-offset-4-s1-indirect-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_4_S1_INDIRECT_4, "pxvi.s-d-indirect-4-s1-indirect-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s (${d-An})${d-i4-4}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_4, "pxvi.s-d-indirect-with-post-increment-4-s1-indirect-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s ${d-i4-4}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_4, "pxvi.s-d-indirect-with-pre-increment-4-s1-indirect-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s ${d-direct-addr},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxvi.s-d-direct-s1-indirect-with-post-increment-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s #${d-imm8},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_IMMEDIATE_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxvi.s-d-immediate-4-s1-indirect-with-post-increment-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s (${d-An},${d-r}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxvi.s-d-indirect-with-index-4-s1-indirect-with-post-increment-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxvi.s-d-indirect-with-offset-4-s1-indirect-with-post-increment-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s (${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxvi.s-d-indirect-4-s1-indirect-with-post-increment-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxvi.s-d-indirect-with-post-increment-4-s1-indirect-with-post-increment-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxvi.s-d-indirect-with-pre-increment-4-s1-indirect-with-post-increment-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s ${d-direct-addr},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxvi.s-d-direct-s1-indirect-with-pre-increment-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s #${d-imm8},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_IMMEDIATE_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxvi.s-d-immediate-4-s1-indirect-with-pre-increment-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s (${d-An},${d-r}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxvi.s-d-indirect-with-index-4-s1-indirect-with-pre-increment-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxvi.s-d-indirect-with-offset-4-s1-indirect-with-pre-increment-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s (${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxvi.s-d-indirect-4-s1-indirect-with-pre-increment-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxvi.s-d-indirect-with-post-increment-4-s1-indirect-with-pre-increment-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi.s ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXVI_S_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxvi.s-d-indirect-with-pre-increment-4-s1-indirect-with-pre-increment-4", "pxvi.s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXVI_D_DIRECT_S1_DIRECT, "pxvi-d-direct-s1-direct", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXVI_D_IMMEDIATE_4_S1_DIRECT, "pxvi-d-immediate-4-s1-direct", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_INDEX_4_S1_DIRECT, "pxvi-d-indirect-with-index-4-s1-direct", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi ${d-imm7-4}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT, "pxvi-d-indirect-with-offset-4-s1-direct", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_4_S1_DIRECT, "pxvi-d-indirect-4-s1-direct", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi (${d-An})${d-i4-4}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_POST_INCREMENT_4_S1_DIRECT, "pxvi-d-indirect-with-post-increment-4-s1-direct", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi ${d-i4-4}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT, "pxvi-d-indirect-with-pre-increment-4-s1-direct", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXVI_D_DIRECT_S1_IMMEDIATE, "pxvi-d-direct-s1-immediate", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXVI_D_IMMEDIATE_4_S1_IMMEDIATE, "pxvi-d-immediate-4-s1-immediate", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_INDEX_4_S1_IMMEDIATE, "pxvi-d-indirect-with-index-4-s1-immediate", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi ${d-imm7-4}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_OFFSET_4_S1_IMMEDIATE, "pxvi-d-indirect-with-offset-4-s1-immediate", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_4_S1_IMMEDIATE, "pxvi-d-indirect-4-s1-immediate", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi (${d-An})${d-i4-4}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_POST_INCREMENT_4_S1_IMMEDIATE, "pxvi-d-indirect-with-post-increment-4-s1-immediate", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi ${d-i4-4}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_IMMEDIATE, "pxvi-d-indirect-with-pre-increment-4-s1-immediate", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXVI_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "pxvi-d-direct-s1-indirect-with-index-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXVI_D_IMMEDIATE_4_S1_INDIRECT_WITH_INDEX_4, "pxvi-d-immediate-4-s1-indirect-with-index-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_INDEX_4, "pxvi-d-indirect-with-index-4-s1-indirect-with-index-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_INDEX_4, "pxvi-d-indirect-with-offset-4-s1-indirect-with-index-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_4_S1_INDIRECT_WITH_INDEX_4, "pxvi-d-indirect-4-s1-indirect-with-index-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi (${d-An})${d-i4-4}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "pxvi-d-indirect-with-post-increment-4-s1-indirect-with-index-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "pxvi-d-indirect-with-pre-increment-4-s1-indirect-with-index-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi ${d-direct-addr},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "pxvi-d-direct-s1-indirect-with-offset-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi #${d-imm8},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_D_IMMEDIATE_4_S1_INDIRECT_WITH_OFFSET_4, "pxvi-d-immediate-4-s1-indirect-with-offset-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi (${d-An},${d-r}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_OFFSET_4, "pxvi-d-indirect-with-index-4-s1-indirect-with-offset-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_OFFSET_4, "pxvi-d-indirect-with-offset-4-s1-indirect-with-offset-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi (${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_4_S1_INDIRECT_WITH_OFFSET_4, "pxvi-d-indirect-4-s1-indirect-with-offset-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "pxvi-d-indirect-with-post-increment-4-s1-indirect-with-offset-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "pxvi-d-indirect-with-pre-increment-4-s1-indirect-with-offset-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_D_DIRECT_S1_INDIRECT_4, "pxvi-d-direct-s1-indirect-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_D_IMMEDIATE_4_S1_INDIRECT_4, "pxvi-d-immediate-4-s1-indirect-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_4, "pxvi-d-indirect-with-index-4-s1-indirect-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi ${d-imm7-4}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_4, "pxvi-d-indirect-with-offset-4-s1-indirect-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_4_S1_INDIRECT_4, "pxvi-d-indirect-4-s1-indirect-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi (${d-An})${d-i4-4}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_4, "pxvi-d-indirect-with-post-increment-4-s1-indirect-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi ${d-i4-4}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_4, "pxvi-d-indirect-with-pre-increment-4-s1-indirect-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi ${d-direct-addr},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXVI_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxvi-d-direct-s1-indirect-with-post-increment-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi #${d-imm8},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXVI_D_IMMEDIATE_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxvi-d-immediate-4-s1-indirect-with-post-increment-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi (${d-An},${d-r}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxvi-d-indirect-with-index-4-s1-indirect-with-post-increment-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxvi-d-indirect-with-offset-4-s1-indirect-with-post-increment-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi (${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxvi-d-indirect-4-s1-indirect-with-post-increment-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxvi-d-indirect-with-post-increment-4-s1-indirect-with-post-increment-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxvi-d-indirect-with-pre-increment-4-s1-indirect-with-post-increment-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi ${d-direct-addr},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXVI_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxvi-d-direct-s1-indirect-with-pre-increment-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi #${d-imm8},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXVI_D_IMMEDIATE_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxvi-d-immediate-4-s1-indirect-with-pre-increment-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi (${d-An},${d-r}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxvi-d-indirect-with-index-4-s1-indirect-with-pre-increment-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxvi-d-indirect-with-offset-4-s1-indirect-with-pre-increment-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi (${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxvi-d-indirect-4-s1-indirect-with-pre-increment-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxvi-d-indirect-with-post-increment-4-s1-indirect-with-pre-increment-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxvi ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXVI_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxvi-d-indirect-with-pre-increment-4-s1-indirect-with-pre-increment-4", "pxvi", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_DIRECT_S1_DIRECT, "pxblend.t-d-direct-s1-direct", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_IMMEDIATE_4_S1_DIRECT, "pxblend.t-d-immediate-4-s1-direct", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_INDEX_4_S1_DIRECT, "pxblend.t-d-indirect-with-index-4-s1-direct", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t ${d-imm7-4}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT, "pxblend.t-d-indirect-with-offset-4-s1-direct", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_4_S1_DIRECT, "pxblend.t-d-indirect-4-s1-direct", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t (${d-An})${d-i4-4}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_POST_INCREMENT_4_S1_DIRECT, "pxblend.t-d-indirect-with-post-increment-4-s1-direct", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t ${d-i4-4}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT, "pxblend.t-d-indirect-with-pre-increment-4-s1-direct", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_DIRECT_S1_IMMEDIATE, "pxblend.t-d-direct-s1-immediate", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_IMMEDIATE_4_S1_IMMEDIATE, "pxblend.t-d-immediate-4-s1-immediate", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_INDEX_4_S1_IMMEDIATE, "pxblend.t-d-indirect-with-index-4-s1-immediate", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t ${d-imm7-4}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_OFFSET_4_S1_IMMEDIATE, "pxblend.t-d-indirect-with-offset-4-s1-immediate", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_4_S1_IMMEDIATE, "pxblend.t-d-indirect-4-s1-immediate", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t (${d-An})${d-i4-4}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_POST_INCREMENT_4_S1_IMMEDIATE, "pxblend.t-d-indirect-with-post-increment-4-s1-immediate", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t ${d-i4-4}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_IMMEDIATE, "pxblend.t-d-indirect-with-pre-increment-4-s1-immediate", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "pxblend.t-d-direct-s1-indirect-with-index-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_IMMEDIATE_4_S1_INDIRECT_WITH_INDEX_4, "pxblend.t-d-immediate-4-s1-indirect-with-index-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_INDEX_4, "pxblend.t-d-indirect-with-index-4-s1-indirect-with-index-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_INDEX_4, "pxblend.t-d-indirect-with-offset-4-s1-indirect-with-index-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_4_S1_INDIRECT_WITH_INDEX_4, "pxblend.t-d-indirect-4-s1-indirect-with-index-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t (${d-An})${d-i4-4}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "pxblend.t-d-indirect-with-post-increment-4-s1-indirect-with-index-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "pxblend.t-d-indirect-with-pre-increment-4-s1-indirect-with-index-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t ${d-direct-addr},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "pxblend.t-d-direct-s1-indirect-with-offset-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t #${d-imm8},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_IMMEDIATE_4_S1_INDIRECT_WITH_OFFSET_4, "pxblend.t-d-immediate-4-s1-indirect-with-offset-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t (${d-An},${d-r}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_OFFSET_4, "pxblend.t-d-indirect-with-index-4-s1-indirect-with-offset-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_OFFSET_4, "pxblend.t-d-indirect-with-offset-4-s1-indirect-with-offset-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t (${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_4_S1_INDIRECT_WITH_OFFSET_4, "pxblend.t-d-indirect-4-s1-indirect-with-offset-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "pxblend.t-d-indirect-with-post-increment-4-s1-indirect-with-offset-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "pxblend.t-d-indirect-with-pre-increment-4-s1-indirect-with-offset-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_DIRECT_S1_INDIRECT_4, "pxblend.t-d-direct-s1-indirect-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_IMMEDIATE_4_S1_INDIRECT_4, "pxblend.t-d-immediate-4-s1-indirect-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_4, "pxblend.t-d-indirect-with-index-4-s1-indirect-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t ${d-imm7-4}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_4, "pxblend.t-d-indirect-with-offset-4-s1-indirect-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_4_S1_INDIRECT_4, "pxblend.t-d-indirect-4-s1-indirect-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t (${d-An})${d-i4-4}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_4, "pxblend.t-d-indirect-with-post-increment-4-s1-indirect-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t ${d-i4-4}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_4, "pxblend.t-d-indirect-with-pre-increment-4-s1-indirect-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t ${d-direct-addr},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxblend.t-d-direct-s1-indirect-with-post-increment-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t #${d-imm8},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_IMMEDIATE_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxblend.t-d-immediate-4-s1-indirect-with-post-increment-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t (${d-An},${d-r}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxblend.t-d-indirect-with-index-4-s1-indirect-with-post-increment-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxblend.t-d-indirect-with-offset-4-s1-indirect-with-post-increment-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t (${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxblend.t-d-indirect-4-s1-indirect-with-post-increment-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxblend.t-d-indirect-with-post-increment-4-s1-indirect-with-post-increment-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxblend.t-d-indirect-with-pre-increment-4-s1-indirect-with-post-increment-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t ${d-direct-addr},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxblend.t-d-direct-s1-indirect-with-pre-increment-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t #${d-imm8},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_IMMEDIATE_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxblend.t-d-immediate-4-s1-indirect-with-pre-increment-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t (${d-An},${d-r}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxblend.t-d-indirect-with-index-4-s1-indirect-with-pre-increment-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxblend.t-d-indirect-with-offset-4-s1-indirect-with-pre-increment-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t (${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxblend.t-d-indirect-4-s1-indirect-with-pre-increment-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxblend.t-d-indirect-with-post-increment-4-s1-indirect-with-pre-increment-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend.t ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_T_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxblend.t-d-indirect-with-pre-increment-4-s1-indirect-with-pre-increment-4", "pxblend.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_DIRECT_S1_DIRECT, "pxblend-d-direct-s1-direct", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_IMMEDIATE_4_S1_DIRECT, "pxblend-d-immediate-4-s1-direct", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_INDEX_4_S1_DIRECT, "pxblend-d-indirect-with-index-4-s1-direct", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend ${d-imm7-4}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT, "pxblend-d-indirect-with-offset-4-s1-direct", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_4_S1_DIRECT, "pxblend-d-indirect-4-s1-direct", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend (${d-An})${d-i4-4}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_POST_INCREMENT_4_S1_DIRECT, "pxblend-d-indirect-with-post-increment-4-s1-direct", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend ${d-i4-4}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT, "pxblend-d-indirect-with-pre-increment-4-s1-direct", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_DIRECT_S1_IMMEDIATE, "pxblend-d-direct-s1-immediate", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_IMMEDIATE_4_S1_IMMEDIATE, "pxblend-d-immediate-4-s1-immediate", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_INDEX_4_S1_IMMEDIATE, "pxblend-d-indirect-with-index-4-s1-immediate", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend ${d-imm7-4}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_OFFSET_4_S1_IMMEDIATE, "pxblend-d-indirect-with-offset-4-s1-immediate", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_4_S1_IMMEDIATE, "pxblend-d-indirect-4-s1-immediate", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend (${d-An})${d-i4-4}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_POST_INCREMENT_4_S1_IMMEDIATE, "pxblend-d-indirect-with-post-increment-4-s1-immediate", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend ${d-i4-4}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_IMMEDIATE, "pxblend-d-indirect-with-pre-increment-4-s1-immediate", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "pxblend-d-direct-s1-indirect-with-index-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_IMMEDIATE_4_S1_INDIRECT_WITH_INDEX_4, "pxblend-d-immediate-4-s1-indirect-with-index-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_INDEX_4, "pxblend-d-indirect-with-index-4-s1-indirect-with-index-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_INDEX_4, "pxblend-d-indirect-with-offset-4-s1-indirect-with-index-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_4_S1_INDIRECT_WITH_INDEX_4, "pxblend-d-indirect-4-s1-indirect-with-index-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend (${d-An})${d-i4-4}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "pxblend-d-indirect-with-post-increment-4-s1-indirect-with-index-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "pxblend-d-indirect-with-pre-increment-4-s1-indirect-with-index-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend ${d-direct-addr},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "pxblend-d-direct-s1-indirect-with-offset-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend #${d-imm8},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_IMMEDIATE_4_S1_INDIRECT_WITH_OFFSET_4, "pxblend-d-immediate-4-s1-indirect-with-offset-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend (${d-An},${d-r}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_OFFSET_4, "pxblend-d-indirect-with-index-4-s1-indirect-with-offset-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_OFFSET_4, "pxblend-d-indirect-with-offset-4-s1-indirect-with-offset-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend (${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_4_S1_INDIRECT_WITH_OFFSET_4, "pxblend-d-indirect-4-s1-indirect-with-offset-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "pxblend-d-indirect-with-post-increment-4-s1-indirect-with-offset-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "pxblend-d-indirect-with-pre-increment-4-s1-indirect-with-offset-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_DIRECT_S1_INDIRECT_4, "pxblend-d-direct-s1-indirect-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_IMMEDIATE_4_S1_INDIRECT_4, "pxblend-d-immediate-4-s1-indirect-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_4, "pxblend-d-indirect-with-index-4-s1-indirect-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend ${d-imm7-4}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_4, "pxblend-d-indirect-with-offset-4-s1-indirect-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_4_S1_INDIRECT_4, "pxblend-d-indirect-4-s1-indirect-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend (${d-An})${d-i4-4}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_4, "pxblend-d-indirect-with-post-increment-4-s1-indirect-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend ${d-i4-4}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_4, "pxblend-d-indirect-with-pre-increment-4-s1-indirect-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend ${d-direct-addr},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxblend-d-direct-s1-indirect-with-post-increment-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend #${d-imm8},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_IMMEDIATE_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxblend-d-immediate-4-s1-indirect-with-post-increment-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend (${d-An},${d-r}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxblend-d-indirect-with-index-4-s1-indirect-with-post-increment-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxblend-d-indirect-with-offset-4-s1-indirect-with-post-increment-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend (${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxblend-d-indirect-4-s1-indirect-with-post-increment-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxblend-d-indirect-with-post-increment-4-s1-indirect-with-post-increment-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxblend-d-indirect-with-pre-increment-4-s1-indirect-with-post-increment-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend ${d-direct-addr},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxblend-d-direct-s1-indirect-with-pre-increment-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend #${d-imm8},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_IMMEDIATE_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxblend-d-immediate-4-s1-indirect-with-pre-increment-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend (${d-An},${d-r}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxblend-d-indirect-with-index-4-s1-indirect-with-pre-increment-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxblend-d-indirect-with-offset-4-s1-indirect-with-pre-increment-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend (${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxblend-d-indirect-4-s1-indirect-with-pre-increment-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxblend-d-indirect-with-post-increment-4-s1-indirect-with-pre-increment-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxblend ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_PXBLEND_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxblend-d-indirect-with-pre-increment-4-s1-indirect-with-pre-increment-4", "pxblend", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t ${d-direct-addr},${s1-direct-addr} */
  {
    UBICOM32_INSN_PXCNV_T_D_DIRECT_S1_DIRECT, "pxcnv.t-d-direct-s1-direct", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t #${d-imm8},${s1-direct-addr} */
  {
    UBICOM32_INSN_PXCNV_T_D_IMMEDIATE_2_S1_DIRECT, "pxcnv.t-d-immediate-2-s1-direct", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t (${d-An},${d-r}),${s1-direct-addr} */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_INDEX_2_S1_DIRECT, "pxcnv.t-d-indirect-with-index-2-s1-direct", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t ${d-imm7-2}(${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_OFFSET_2_S1_DIRECT, "pxcnv.t-d-indirect-with-offset-2-s1-direct", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t (${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_2_S1_DIRECT, "pxcnv.t-d-indirect-2-s1-direct", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t (${d-An})${d-i4-2}++,${s1-direct-addr} */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_POST_INCREMENT_2_S1_DIRECT, "pxcnv.t-d-indirect-with-post-increment-2-s1-direct", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t ${d-i4-2}(${d-An})++,${s1-direct-addr} */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_DIRECT, "pxcnv.t-d-indirect-with-pre-increment-2-s1-direct", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t ${d-direct-addr},#${s1-imm8} */
  {
    UBICOM32_INSN_PXCNV_T_D_DIRECT_S1_IMMEDIATE, "pxcnv.t-d-direct-s1-immediate", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t #${d-imm8},#${s1-imm8} */
  {
    UBICOM32_INSN_PXCNV_T_D_IMMEDIATE_2_S1_IMMEDIATE, "pxcnv.t-d-immediate-2-s1-immediate", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t (${d-An},${d-r}),#${s1-imm8} */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_INDEX_2_S1_IMMEDIATE, "pxcnv.t-d-indirect-with-index-2-s1-immediate", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t ${d-imm7-2}(${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_OFFSET_2_S1_IMMEDIATE, "pxcnv.t-d-indirect-with-offset-2-s1-immediate", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t (${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_2_S1_IMMEDIATE, "pxcnv.t-d-indirect-2-s1-immediate", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t (${d-An})${d-i4-2}++,#${s1-imm8} */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_POST_INCREMENT_2_S1_IMMEDIATE, "pxcnv.t-d-indirect-with-post-increment-2-s1-immediate", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t ${d-i4-2}(${d-An})++,#${s1-imm8} */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_IMMEDIATE, "pxcnv.t-d-indirect-with-pre-increment-2-s1-immediate", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t ${d-direct-addr},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_PXCNV_T_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "pxcnv.t-d-direct-s1-indirect-with-index-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t #${d-imm8},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_PXCNV_T_D_IMMEDIATE_2_S1_INDIRECT_WITH_INDEX_4, "pxcnv.t-d-immediate-2-s1-indirect-with-index-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t (${d-An},${d-r}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_INDEX_4, "pxcnv.t-d-indirect-with-index-2-s1-indirect-with-index-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t ${d-imm7-2}(${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_INDEX_4, "pxcnv.t-d-indirect-with-offset-2-s1-indirect-with-index-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t (${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_2_S1_INDIRECT_WITH_INDEX_4, "pxcnv.t-d-indirect-2-s1-indirect-with-index-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t (${d-An})${d-i4-2}++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_INDEX_4, "pxcnv.t-d-indirect-with-post-increment-2-s1-indirect-with-index-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t ${d-i4-2}(${d-An})++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_INDEX_4, "pxcnv.t-d-indirect-with-pre-increment-2-s1-indirect-with-index-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t ${d-direct-addr},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_T_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "pxcnv.t-d-direct-s1-indirect-with-offset-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t #${d-imm8},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_T_D_IMMEDIATE_2_S1_INDIRECT_WITH_OFFSET_4, "pxcnv.t-d-immediate-2-s1-indirect-with-offset-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t (${d-An},${d-r}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_OFFSET_4, "pxcnv.t-d-indirect-with-index-2-s1-indirect-with-offset-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t ${d-imm7-2}(${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_OFFSET_4, "pxcnv.t-d-indirect-with-offset-2-s1-indirect-with-offset-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t (${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_2_S1_INDIRECT_WITH_OFFSET_4, "pxcnv.t-d-indirect-2-s1-indirect-with-offset-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t (${d-An})${d-i4-2}++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_4, "pxcnv.t-d-indirect-with-post-increment-2-s1-indirect-with-offset-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t ${d-i4-2}(${d-An})++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_4, "pxcnv.t-d-indirect-with-pre-increment-2-s1-indirect-with-offset-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t ${d-direct-addr},(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_T_D_DIRECT_S1_INDIRECT_4, "pxcnv.t-d-direct-s1-indirect-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t #${d-imm8},(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_T_D_IMMEDIATE_2_S1_INDIRECT_4, "pxcnv.t-d-immediate-2-s1-indirect-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t (${d-An},${d-r}),(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_4, "pxcnv.t-d-indirect-with-index-2-s1-indirect-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t ${d-imm7-2}(${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_4, "pxcnv.t-d-indirect-with-offset-2-s1-indirect-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t (${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_2_S1_INDIRECT_4, "pxcnv.t-d-indirect-2-s1-indirect-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t (${d-An})${d-i4-2}++,(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_4, "pxcnv.t-d-indirect-with-post-increment-2-s1-indirect-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t ${d-i4-2}(${d-An})++,(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_4, "pxcnv.t-d-indirect-with-pre-increment-2-s1-indirect-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t ${d-direct-addr},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_PXCNV_T_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxcnv.t-d-direct-s1-indirect-with-post-increment-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t #${d-imm8},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_PXCNV_T_D_IMMEDIATE_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxcnv.t-d-immediate-2-s1-indirect-with-post-increment-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t (${d-An},${d-r}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxcnv.t-d-indirect-with-index-2-s1-indirect-with-post-increment-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t ${d-imm7-2}(${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxcnv.t-d-indirect-with-offset-2-s1-indirect-with-post-increment-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t (${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxcnv.t-d-indirect-2-s1-indirect-with-post-increment-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t (${d-An})${d-i4-2}++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxcnv.t-d-indirect-with-post-increment-2-s1-indirect-with-post-increment-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t ${d-i4-2}(${d-An})++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxcnv.t-d-indirect-with-pre-increment-2-s1-indirect-with-post-increment-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t ${d-direct-addr},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_PXCNV_T_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxcnv.t-d-direct-s1-indirect-with-pre-increment-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t #${d-imm8},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_PXCNV_T_D_IMMEDIATE_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxcnv.t-d-immediate-2-s1-indirect-with-pre-increment-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t (${d-An},${d-r}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxcnv.t-d-indirect-with-index-2-s1-indirect-with-pre-increment-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t ${d-imm7-2}(${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxcnv.t-d-indirect-with-offset-2-s1-indirect-with-pre-increment-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t (${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxcnv.t-d-indirect-2-s1-indirect-with-pre-increment-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t (${d-An})${d-i4-2}++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxcnv.t-d-indirect-with-post-increment-2-s1-indirect-with-pre-increment-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv.t ${d-i4-2}(${d-An})++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_PXCNV_T_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxcnv.t-d-indirect-with-pre-increment-2-s1-indirect-with-pre-increment-4", "pxcnv.t", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv ${d-direct-addr},${s1-direct-addr} */
  {
    UBICOM32_INSN_PXCNV_D_DIRECT_S1_DIRECT, "pxcnv-d-direct-s1-direct", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv #${d-imm8},${s1-direct-addr} */
  {
    UBICOM32_INSN_PXCNV_D_IMMEDIATE_2_S1_DIRECT, "pxcnv-d-immediate-2-s1-direct", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv (${d-An},${d-r}),${s1-direct-addr} */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_INDEX_2_S1_DIRECT, "pxcnv-d-indirect-with-index-2-s1-direct", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv ${d-imm7-2}(${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_OFFSET_2_S1_DIRECT, "pxcnv-d-indirect-with-offset-2-s1-direct", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv (${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_2_S1_DIRECT, "pxcnv-d-indirect-2-s1-direct", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv (${d-An})${d-i4-2}++,${s1-direct-addr} */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_POST_INCREMENT_2_S1_DIRECT, "pxcnv-d-indirect-with-post-increment-2-s1-direct", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv ${d-i4-2}(${d-An})++,${s1-direct-addr} */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_DIRECT, "pxcnv-d-indirect-with-pre-increment-2-s1-direct", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv ${d-direct-addr},#${s1-imm8} */
  {
    UBICOM32_INSN_PXCNV_D_DIRECT_S1_IMMEDIATE, "pxcnv-d-direct-s1-immediate", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv #${d-imm8},#${s1-imm8} */
  {
    UBICOM32_INSN_PXCNV_D_IMMEDIATE_2_S1_IMMEDIATE, "pxcnv-d-immediate-2-s1-immediate", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv (${d-An},${d-r}),#${s1-imm8} */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_INDEX_2_S1_IMMEDIATE, "pxcnv-d-indirect-with-index-2-s1-immediate", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv ${d-imm7-2}(${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_OFFSET_2_S1_IMMEDIATE, "pxcnv-d-indirect-with-offset-2-s1-immediate", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv (${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_2_S1_IMMEDIATE, "pxcnv-d-indirect-2-s1-immediate", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv (${d-An})${d-i4-2}++,#${s1-imm8} */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_POST_INCREMENT_2_S1_IMMEDIATE, "pxcnv-d-indirect-with-post-increment-2-s1-immediate", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv ${d-i4-2}(${d-An})++,#${s1-imm8} */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_IMMEDIATE, "pxcnv-d-indirect-with-pre-increment-2-s1-immediate", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv ${d-direct-addr},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_PXCNV_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "pxcnv-d-direct-s1-indirect-with-index-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv #${d-imm8},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_PXCNV_D_IMMEDIATE_2_S1_INDIRECT_WITH_INDEX_4, "pxcnv-d-immediate-2-s1-indirect-with-index-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv (${d-An},${d-r}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_INDEX_4, "pxcnv-d-indirect-with-index-2-s1-indirect-with-index-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv ${d-imm7-2}(${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_INDEX_4, "pxcnv-d-indirect-with-offset-2-s1-indirect-with-index-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv (${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_2_S1_INDIRECT_WITH_INDEX_4, "pxcnv-d-indirect-2-s1-indirect-with-index-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv (${d-An})${d-i4-2}++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_INDEX_4, "pxcnv-d-indirect-with-post-increment-2-s1-indirect-with-index-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv ${d-i4-2}(${d-An})++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_INDEX_4, "pxcnv-d-indirect-with-pre-increment-2-s1-indirect-with-index-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv ${d-direct-addr},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "pxcnv-d-direct-s1-indirect-with-offset-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv #${d-imm8},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_D_IMMEDIATE_2_S1_INDIRECT_WITH_OFFSET_4, "pxcnv-d-immediate-2-s1-indirect-with-offset-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv (${d-An},${d-r}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_OFFSET_4, "pxcnv-d-indirect-with-index-2-s1-indirect-with-offset-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv ${d-imm7-2}(${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_OFFSET_4, "pxcnv-d-indirect-with-offset-2-s1-indirect-with-offset-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv (${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_2_S1_INDIRECT_WITH_OFFSET_4, "pxcnv-d-indirect-2-s1-indirect-with-offset-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv (${d-An})${d-i4-2}++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_4, "pxcnv-d-indirect-with-post-increment-2-s1-indirect-with-offset-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv ${d-i4-2}(${d-An})++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_4, "pxcnv-d-indirect-with-pre-increment-2-s1-indirect-with-offset-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv ${d-direct-addr},(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_D_DIRECT_S1_INDIRECT_4, "pxcnv-d-direct-s1-indirect-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv #${d-imm8},(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_D_IMMEDIATE_2_S1_INDIRECT_4, "pxcnv-d-immediate-2-s1-indirect-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv (${d-An},${d-r}),(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_4, "pxcnv-d-indirect-with-index-2-s1-indirect-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv ${d-imm7-2}(${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_4, "pxcnv-d-indirect-with-offset-2-s1-indirect-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv (${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_2_S1_INDIRECT_4, "pxcnv-d-indirect-2-s1-indirect-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv (${d-An})${d-i4-2}++,(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_4, "pxcnv-d-indirect-with-post-increment-2-s1-indirect-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv ${d-i4-2}(${d-An})++,(${s1-An}) */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_4, "pxcnv-d-indirect-with-pre-increment-2-s1-indirect-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv ${d-direct-addr},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_PXCNV_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxcnv-d-direct-s1-indirect-with-post-increment-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv #${d-imm8},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_PXCNV_D_IMMEDIATE_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxcnv-d-immediate-2-s1-indirect-with-post-increment-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv (${d-An},${d-r}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxcnv-d-indirect-with-index-2-s1-indirect-with-post-increment-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv ${d-imm7-2}(${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxcnv-d-indirect-with-offset-2-s1-indirect-with-post-increment-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv (${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxcnv-d-indirect-2-s1-indirect-with-post-increment-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv (${d-An})${d-i4-2}++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxcnv-d-indirect-with-post-increment-2-s1-indirect-with-post-increment-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv ${d-i4-2}(${d-An})++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_4, "pxcnv-d-indirect-with-pre-increment-2-s1-indirect-with-post-increment-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv ${d-direct-addr},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_PXCNV_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxcnv-d-direct-s1-indirect-with-pre-increment-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv #${d-imm8},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_PXCNV_D_IMMEDIATE_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxcnv-d-immediate-2-s1-indirect-with-pre-increment-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv (${d-An},${d-r}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxcnv-d-indirect-with-index-2-s1-indirect-with-pre-increment-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv ${d-imm7-2}(${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxcnv-d-indirect-with-offset-2-s1-indirect-with-pre-increment-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv (${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxcnv-d-indirect-2-s1-indirect-with-pre-increment-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv (${d-An})${d-i4-2}++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxcnv-d-indirect-with-post-increment-2-s1-indirect-with-pre-increment-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* pxcnv ${d-i4-2}(${d-An})++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_PXCNV_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_4, "pxcnv-d-indirect-with-pre-increment-2-s1-indirect-with-pre-increment-4", "pxcnv", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* subc ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUBC_D_DIRECT_S1_DIRECT, "subc-d-direct-s1-direct", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUBC_D_IMMEDIATE_4_S1_DIRECT, "subc-d-immediate-4-s1-direct", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_INDEX_4_S1_DIRECT, "subc-d-indirect-with-index-4-s1-direct", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc ${d-imm7-4}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT, "subc-d-indirect-with-offset-4-s1-direct", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_4_S1_DIRECT, "subc-d-indirect-4-s1-direct", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc (${d-An})${d-i4-4}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_POST_INCREMENT_4_S1_DIRECT, "subc-d-indirect-with-post-increment-4-s1-direct", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc ${d-i4-4}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT, "subc-d-indirect-with-pre-increment-4-s1-direct", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUBC_D_DIRECT_S1_IMMEDIATE, "subc-d-direct-s1-immediate", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUBC_D_IMMEDIATE_4_S1_IMMEDIATE, "subc-d-immediate-4-s1-immediate", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_INDEX_4_S1_IMMEDIATE, "subc-d-indirect-with-index-4-s1-immediate", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc ${d-imm7-4}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_OFFSET_4_S1_IMMEDIATE, "subc-d-indirect-with-offset-4-s1-immediate", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_4_S1_IMMEDIATE, "subc-d-indirect-4-s1-immediate", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc (${d-An})${d-i4-4}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_POST_INCREMENT_4_S1_IMMEDIATE, "subc-d-indirect-with-post-increment-4-s1-immediate", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc ${d-i4-4}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_IMMEDIATE, "subc-d-indirect-with-pre-increment-4-s1-immediate", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUBC_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "subc-d-direct-s1-indirect-with-index-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUBC_D_IMMEDIATE_4_S1_INDIRECT_WITH_INDEX_4, "subc-d-immediate-4-s1-indirect-with-index-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_INDEX_4, "subc-d-indirect-with-index-4-s1-indirect-with-index-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_INDEX_4, "subc-d-indirect-with-offset-4-s1-indirect-with-index-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_4_S1_INDIRECT_WITH_INDEX_4, "subc-d-indirect-4-s1-indirect-with-index-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc (${d-An})${d-i4-4}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "subc-d-indirect-with-post-increment-4-s1-indirect-with-index-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "subc-d-indirect-with-pre-increment-4-s1-indirect-with-index-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc ${d-direct-addr},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUBC_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "subc-d-direct-s1-indirect-with-offset-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc #${d-imm8},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUBC_D_IMMEDIATE_4_S1_INDIRECT_WITH_OFFSET_4, "subc-d-immediate-4-s1-indirect-with-offset-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc (${d-An},${d-r}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_OFFSET_4, "subc-d-indirect-with-index-4-s1-indirect-with-offset-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_OFFSET_4, "subc-d-indirect-with-offset-4-s1-indirect-with-offset-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc (${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_4_S1_INDIRECT_WITH_OFFSET_4, "subc-d-indirect-4-s1-indirect-with-offset-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "subc-d-indirect-with-post-increment-4-s1-indirect-with-offset-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "subc-d-indirect-with-pre-increment-4-s1-indirect-with-offset-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUBC_D_DIRECT_S1_INDIRECT_4, "subc-d-direct-s1-indirect-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUBC_D_IMMEDIATE_4_S1_INDIRECT_4, "subc-d-immediate-4-s1-indirect-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_4, "subc-d-indirect-with-index-4-s1-indirect-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc ${d-imm7-4}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_4, "subc-d-indirect-with-offset-4-s1-indirect-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_4_S1_INDIRECT_4, "subc-d-indirect-4-s1-indirect-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc (${d-An})${d-i4-4}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_4, "subc-d-indirect-with-post-increment-4-s1-indirect-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc ${d-i4-4}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_4, "subc-d-indirect-with-pre-increment-4-s1-indirect-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc ${d-direct-addr},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_SUBC_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "subc-d-direct-s1-indirect-with-post-increment-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc #${d-imm8},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_SUBC_D_IMMEDIATE_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "subc-d-immediate-4-s1-indirect-with-post-increment-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc (${d-An},${d-r}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "subc-d-indirect-with-index-4-s1-indirect-with-post-increment-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "subc-d-indirect-with-offset-4-s1-indirect-with-post-increment-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc (${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "subc-d-indirect-4-s1-indirect-with-post-increment-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "subc-d-indirect-with-post-increment-4-s1-indirect-with-post-increment-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "subc-d-indirect-with-pre-increment-4-s1-indirect-with-post-increment-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc ${d-direct-addr},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUBC_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "subc-d-direct-s1-indirect-with-pre-increment-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc #${d-imm8},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUBC_D_IMMEDIATE_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "subc-d-immediate-4-s1-indirect-with-pre-increment-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc (${d-An},${d-r}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "subc-d-indirect-with-index-4-s1-indirect-with-pre-increment-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "subc-d-indirect-with-offset-4-s1-indirect-with-pre-increment-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc (${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "subc-d-indirect-4-s1-indirect-with-pre-increment-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "subc-d-indirect-with-post-increment-4-s1-indirect-with-pre-increment-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* subc ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUBC_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "subc-d-indirect-with-pre-increment-4-s1-indirect-with-pre-increment-4", "subc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADDC_D_DIRECT_S1_DIRECT, "addc-d-direct-s1-direct", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADDC_D_IMMEDIATE_4_S1_DIRECT, "addc-d-immediate-4-s1-direct", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_INDEX_4_S1_DIRECT, "addc-d-indirect-with-index-4-s1-direct", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc ${d-imm7-4}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT, "addc-d-indirect-with-offset-4-s1-direct", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_4_S1_DIRECT, "addc-d-indirect-4-s1-direct", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc (${d-An})${d-i4-4}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_POST_INCREMENT_4_S1_DIRECT, "addc-d-indirect-with-post-increment-4-s1-direct", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc ${d-i4-4}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT, "addc-d-indirect-with-pre-increment-4-s1-direct", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADDC_D_DIRECT_S1_IMMEDIATE, "addc-d-direct-s1-immediate", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADDC_D_IMMEDIATE_4_S1_IMMEDIATE, "addc-d-immediate-4-s1-immediate", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_INDEX_4_S1_IMMEDIATE, "addc-d-indirect-with-index-4-s1-immediate", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc ${d-imm7-4}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_OFFSET_4_S1_IMMEDIATE, "addc-d-indirect-with-offset-4-s1-immediate", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_4_S1_IMMEDIATE, "addc-d-indirect-4-s1-immediate", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc (${d-An})${d-i4-4}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_POST_INCREMENT_4_S1_IMMEDIATE, "addc-d-indirect-with-post-increment-4-s1-immediate", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc ${d-i4-4}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_IMMEDIATE, "addc-d-indirect-with-pre-increment-4-s1-immediate", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADDC_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "addc-d-direct-s1-indirect-with-index-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADDC_D_IMMEDIATE_4_S1_INDIRECT_WITH_INDEX_4, "addc-d-immediate-4-s1-indirect-with-index-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_INDEX_4, "addc-d-indirect-with-index-4-s1-indirect-with-index-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_INDEX_4, "addc-d-indirect-with-offset-4-s1-indirect-with-index-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_4_S1_INDIRECT_WITH_INDEX_4, "addc-d-indirect-4-s1-indirect-with-index-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc (${d-An})${d-i4-4}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "addc-d-indirect-with-post-increment-4-s1-indirect-with-index-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "addc-d-indirect-with-pre-increment-4-s1-indirect-with-index-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc ${d-direct-addr},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADDC_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "addc-d-direct-s1-indirect-with-offset-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc #${d-imm8},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADDC_D_IMMEDIATE_4_S1_INDIRECT_WITH_OFFSET_4, "addc-d-immediate-4-s1-indirect-with-offset-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc (${d-An},${d-r}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_OFFSET_4, "addc-d-indirect-with-index-4-s1-indirect-with-offset-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_OFFSET_4, "addc-d-indirect-with-offset-4-s1-indirect-with-offset-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc (${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_4_S1_INDIRECT_WITH_OFFSET_4, "addc-d-indirect-4-s1-indirect-with-offset-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "addc-d-indirect-with-post-increment-4-s1-indirect-with-offset-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "addc-d-indirect-with-pre-increment-4-s1-indirect-with-offset-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADDC_D_DIRECT_S1_INDIRECT_4, "addc-d-direct-s1-indirect-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADDC_D_IMMEDIATE_4_S1_INDIRECT_4, "addc-d-immediate-4-s1-indirect-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_4, "addc-d-indirect-with-index-4-s1-indirect-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc ${d-imm7-4}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_4, "addc-d-indirect-with-offset-4-s1-indirect-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_4_S1_INDIRECT_4, "addc-d-indirect-4-s1-indirect-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc (${d-An})${d-i4-4}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_4, "addc-d-indirect-with-post-increment-4-s1-indirect-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc ${d-i4-4}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_4, "addc-d-indirect-with-pre-increment-4-s1-indirect-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc ${d-direct-addr},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_ADDC_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "addc-d-direct-s1-indirect-with-post-increment-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc #${d-imm8},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_ADDC_D_IMMEDIATE_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "addc-d-immediate-4-s1-indirect-with-post-increment-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc (${d-An},${d-r}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "addc-d-indirect-with-index-4-s1-indirect-with-post-increment-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "addc-d-indirect-with-offset-4-s1-indirect-with-post-increment-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc (${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "addc-d-indirect-4-s1-indirect-with-post-increment-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "addc-d-indirect-with-post-increment-4-s1-indirect-with-post-increment-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "addc-d-indirect-with-pre-increment-4-s1-indirect-with-post-increment-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc ${d-direct-addr},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADDC_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "addc-d-direct-s1-indirect-with-pre-increment-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc #${d-imm8},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADDC_D_IMMEDIATE_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "addc-d-immediate-4-s1-indirect-with-pre-increment-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc (${d-An},${d-r}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "addc-d-indirect-with-index-4-s1-indirect-with-pre-increment-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "addc-d-indirect-with-offset-4-s1-indirect-with-pre-increment-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc (${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "addc-d-indirect-4-s1-indirect-with-pre-increment-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "addc-d-indirect-with-post-increment-4-s1-indirect-with-pre-increment-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* addc ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADDC_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "addc-d-indirect-with-pre-increment-4-s1-indirect-with-pre-increment-4", "addc", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.1 ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUB_1_D_DIRECT_S1_DIRECT, "sub.1-d-direct-s1-direct", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUB_1_D_IMMEDIATE_1_S1_DIRECT, "sub.1-d-immediate-1-s1-direct", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_INDEX_1_S1_DIRECT, "sub.1-d-indirect-with-index-1-s1-direct", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 ${d-imm7-1}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_OFFSET_1_S1_DIRECT, "sub.1-d-indirect-with-offset-1-s1-direct", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_1_S1_DIRECT, "sub.1-d-indirect-1-s1-direct", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 (${d-An})${d-i4-1}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_DIRECT, "sub.1-d-indirect-with-post-increment-1-s1-direct", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 ${d-i4-1}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_DIRECT, "sub.1-d-indirect-with-pre-increment-1-s1-direct", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUB_1_D_DIRECT_S1_IMMEDIATE, "sub.1-d-direct-s1-immediate", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUB_1_D_IMMEDIATE_1_S1_IMMEDIATE, "sub.1-d-immediate-1-s1-immediate", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_INDEX_1_S1_IMMEDIATE, "sub.1-d-indirect-with-index-1-s1-immediate", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 ${d-imm7-1}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_OFFSET_1_S1_IMMEDIATE, "sub.1-d-indirect-with-offset-1-s1-immediate", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_1_S1_IMMEDIATE, "sub.1-d-indirect-1-s1-immediate", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 (${d-An})${d-i4-1}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_IMMEDIATE, "sub.1-d-indirect-with-post-increment-1-s1-immediate", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 ${d-i4-1}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_IMMEDIATE, "sub.1-d-indirect-with-pre-increment-1-s1-immediate", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUB_1_D_DIRECT_S1_INDIRECT_WITH_INDEX_1, "sub.1-d-direct-s1-indirect-with-index-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUB_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_INDEX_1, "sub.1-d-immediate-1-s1-indirect-with-index-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_INDEX_1, "sub.1-d-indirect-with-index-1-s1-indirect-with-index-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 ${d-imm7-1}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_INDEX_1, "sub.1-d-indirect-with-offset-1-s1-indirect-with-index-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_1_S1_INDIRECT_WITH_INDEX_1, "sub.1-d-indirect-1-s1-indirect-with-index-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 (${d-An})${d-i4-1}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_INDEX_1, "sub.1-d-indirect-with-post-increment-1-s1-indirect-with-index-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 ${d-i4-1}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_INDEX_1, "sub.1-d-indirect-with-pre-increment-1-s1-indirect-with-index-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 ${d-direct-addr},${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_1_D_DIRECT_S1_INDIRECT_WITH_OFFSET_1, "sub.1-d-direct-s1-indirect-with-offset-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 #${d-imm8},${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_OFFSET_1, "sub.1-d-immediate-1-s1-indirect-with-offset-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 (${d-An},${d-r}),${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_OFFSET_1, "sub.1-d-indirect-with-index-1-s1-indirect-with-offset-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 ${d-imm7-1}(${d-An}),${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_OFFSET_1, "sub.1-d-indirect-with-offset-1-s1-indirect-with-offset-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 (${d-An}),${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_1_S1_INDIRECT_WITH_OFFSET_1, "sub.1-d-indirect-1-s1-indirect-with-offset-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 (${d-An})${d-i4-1}++,${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_OFFSET_1, "sub.1-d-indirect-with-post-increment-1-s1-indirect-with-offset-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 ${d-i4-1}(${d-An})++,${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_OFFSET_1, "sub.1-d-indirect-with-pre-increment-1-s1-indirect-with-offset-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_1_D_DIRECT_S1_INDIRECT_1, "sub.1-d-direct-s1-indirect-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_1_D_IMMEDIATE_1_S1_INDIRECT_1, "sub.1-d-immediate-1-s1-indirect-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_1, "sub.1-d-indirect-with-index-1-s1-indirect-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 ${d-imm7-1}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_1, "sub.1-d-indirect-with-offset-1-s1-indirect-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_1_S1_INDIRECT_1, "sub.1-d-indirect-1-s1-indirect-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 (${d-An})${d-i4-1}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_1, "sub.1-d-indirect-with-post-increment-1-s1-indirect-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 ${d-i4-1}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_1, "sub.1-d-indirect-with-pre-increment-1-s1-indirect-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 ${d-direct-addr},(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_SUB_1_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_1, "sub.1-d-direct-s1-indirect-with-post-increment-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 #${d-imm8},(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_SUB_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "sub.1-d-immediate-1-s1-indirect-with-post-increment-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 (${d-An},${d-r}),(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "sub.1-d-indirect-with-index-1-s1-indirect-with-post-increment-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 ${d-imm7-1}(${d-An}),(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "sub.1-d-indirect-with-offset-1-s1-indirect-with-post-increment-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 (${d-An}),(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "sub.1-d-indirect-1-s1-indirect-with-post-increment-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 (${d-An})${d-i4-1}++,(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "sub.1-d-indirect-with-post-increment-1-s1-indirect-with-post-increment-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 ${d-i4-1}(${d-An})++,(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "sub.1-d-indirect-with-pre-increment-1-s1-indirect-with-post-increment-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 ${d-direct-addr},${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUB_1_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_1, "sub.1-d-direct-s1-indirect-with-pre-increment-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 #${d-imm8},${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUB_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "sub.1-d-immediate-1-s1-indirect-with-pre-increment-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 (${d-An},${d-r}),${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "sub.1-d-indirect-with-index-1-s1-indirect-with-pre-increment-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 ${d-imm7-1}(${d-An}),${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "sub.1-d-indirect-with-offset-1-s1-indirect-with-pre-increment-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 (${d-An}),${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "sub.1-d-indirect-1-s1-indirect-with-pre-increment-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 (${d-An})${d-i4-1}++,${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "sub.1-d-indirect-with-post-increment-1-s1-indirect-with-pre-increment-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.1 ${d-i4-1}(${d-An})++,${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUB_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "sub.1-d-indirect-with-pre-increment-1-s1-indirect-with-pre-increment-1", "sub.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* sub.4 ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUB_4_D_DIRECT_S1_DIRECT, "sub.4-d-direct-s1-direct", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUB_4_D_IMMEDIATE_4_S1_DIRECT, "sub.4-d-immediate-4-s1-direct", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_INDEX_4_S1_DIRECT, "sub.4-d-indirect-with-index-4-s1-direct", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 ${d-imm7-4}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT, "sub.4-d-indirect-with-offset-4-s1-direct", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_4_S1_DIRECT, "sub.4-d-indirect-4-s1-direct", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 (${d-An})${d-i4-4}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_DIRECT, "sub.4-d-indirect-with-post-increment-4-s1-direct", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 ${d-i4-4}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT, "sub.4-d-indirect-with-pre-increment-4-s1-direct", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUB_4_D_DIRECT_S1_IMMEDIATE, "sub.4-d-direct-s1-immediate", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUB_4_D_IMMEDIATE_4_S1_IMMEDIATE, "sub.4-d-immediate-4-s1-immediate", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_INDEX_4_S1_IMMEDIATE, "sub.4-d-indirect-with-index-4-s1-immediate", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 ${d-imm7-4}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_OFFSET_4_S1_IMMEDIATE, "sub.4-d-indirect-with-offset-4-s1-immediate", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_4_S1_IMMEDIATE, "sub.4-d-indirect-4-s1-immediate", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 (${d-An})${d-i4-4}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_IMMEDIATE, "sub.4-d-indirect-with-post-increment-4-s1-immediate", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 ${d-i4-4}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_IMMEDIATE, "sub.4-d-indirect-with-pre-increment-4-s1-immediate", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUB_4_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "sub.4-d-direct-s1-indirect-with-index-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUB_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_INDEX_4, "sub.4-d-immediate-4-s1-indirect-with-index-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_INDEX_4, "sub.4-d-indirect-with-index-4-s1-indirect-with-index-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_INDEX_4, "sub.4-d-indirect-with-offset-4-s1-indirect-with-index-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_4_S1_INDIRECT_WITH_INDEX_4, "sub.4-d-indirect-4-s1-indirect-with-index-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 (${d-An})${d-i4-4}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "sub.4-d-indirect-with-post-increment-4-s1-indirect-with-index-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "sub.4-d-indirect-with-pre-increment-4-s1-indirect-with-index-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 ${d-direct-addr},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_4_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "sub.4-d-direct-s1-indirect-with-offset-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 #${d-imm8},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_OFFSET_4, "sub.4-d-immediate-4-s1-indirect-with-offset-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 (${d-An},${d-r}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_OFFSET_4, "sub.4-d-indirect-with-index-4-s1-indirect-with-offset-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_OFFSET_4, "sub.4-d-indirect-with-offset-4-s1-indirect-with-offset-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 (${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_4_S1_INDIRECT_WITH_OFFSET_4, "sub.4-d-indirect-4-s1-indirect-with-offset-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "sub.4-d-indirect-with-post-increment-4-s1-indirect-with-offset-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "sub.4-d-indirect-with-pre-increment-4-s1-indirect-with-offset-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_4_D_DIRECT_S1_INDIRECT_4, "sub.4-d-direct-s1-indirect-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_4_D_IMMEDIATE_4_S1_INDIRECT_4, "sub.4-d-immediate-4-s1-indirect-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_4, "sub.4-d-indirect-with-index-4-s1-indirect-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 ${d-imm7-4}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_4, "sub.4-d-indirect-with-offset-4-s1-indirect-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_4_S1_INDIRECT_4, "sub.4-d-indirect-4-s1-indirect-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 (${d-An})${d-i4-4}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_4, "sub.4-d-indirect-with-post-increment-4-s1-indirect-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 ${d-i4-4}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_4, "sub.4-d-indirect-with-pre-increment-4-s1-indirect-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 ${d-direct-addr},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_SUB_4_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "sub.4-d-direct-s1-indirect-with-post-increment-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 #${d-imm8},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_SUB_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "sub.4-d-immediate-4-s1-indirect-with-post-increment-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 (${d-An},${d-r}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "sub.4-d-indirect-with-index-4-s1-indirect-with-post-increment-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "sub.4-d-indirect-with-offset-4-s1-indirect-with-post-increment-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 (${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "sub.4-d-indirect-4-s1-indirect-with-post-increment-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "sub.4-d-indirect-with-post-increment-4-s1-indirect-with-post-increment-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "sub.4-d-indirect-with-pre-increment-4-s1-indirect-with-post-increment-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 ${d-direct-addr},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUB_4_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "sub.4-d-direct-s1-indirect-with-pre-increment-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 #${d-imm8},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUB_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "sub.4-d-immediate-4-s1-indirect-with-pre-increment-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 (${d-An},${d-r}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "sub.4-d-indirect-with-index-4-s1-indirect-with-pre-increment-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "sub.4-d-indirect-with-offset-4-s1-indirect-with-pre-increment-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 (${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "sub.4-d-indirect-4-s1-indirect-with-pre-increment-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "sub.4-d-indirect-with-post-increment-4-s1-indirect-with-pre-increment-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.4 ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUB_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "sub.4-d-indirect-with-pre-increment-4-s1-indirect-with-pre-increment-4", "sub.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUB_2_D_DIRECT_S1_DIRECT, "sub.2-d-direct-s1-direct", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUB_2_D_IMMEDIATE_2_S1_DIRECT, "sub.2-d-immediate-2-s1-direct", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_INDEX_2_S1_DIRECT, "sub.2-d-indirect-with-index-2-s1-direct", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 ${d-imm7-2}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_OFFSET_2_S1_DIRECT, "sub.2-d-indirect-with-offset-2-s1-direct", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_2_S1_DIRECT, "sub.2-d-indirect-2-s1-direct", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 (${d-An})${d-i4-2}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_DIRECT, "sub.2-d-indirect-with-post-increment-2-s1-direct", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 ${d-i4-2}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_DIRECT, "sub.2-d-indirect-with-pre-increment-2-s1-direct", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUB_2_D_DIRECT_S1_IMMEDIATE, "sub.2-d-direct-s1-immediate", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUB_2_D_IMMEDIATE_2_S1_IMMEDIATE, "sub.2-d-immediate-2-s1-immediate", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_INDEX_2_S1_IMMEDIATE, "sub.2-d-indirect-with-index-2-s1-immediate", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 ${d-imm7-2}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_OFFSET_2_S1_IMMEDIATE, "sub.2-d-indirect-with-offset-2-s1-immediate", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_2_S1_IMMEDIATE, "sub.2-d-indirect-2-s1-immediate", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 (${d-An})${d-i4-2}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_IMMEDIATE, "sub.2-d-indirect-with-post-increment-2-s1-immediate", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 ${d-i4-2}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_IMMEDIATE, "sub.2-d-indirect-with-pre-increment-2-s1-immediate", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUB_2_D_DIRECT_S1_INDIRECT_WITH_INDEX_2, "sub.2-d-direct-s1-indirect-with-index-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUB_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_INDEX_2, "sub.2-d-immediate-2-s1-indirect-with-index-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_INDEX_2, "sub.2-d-indirect-with-index-2-s1-indirect-with-index-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 ${d-imm7-2}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_INDEX_2, "sub.2-d-indirect-with-offset-2-s1-indirect-with-index-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_2_S1_INDIRECT_WITH_INDEX_2, "sub.2-d-indirect-2-s1-indirect-with-index-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 (${d-An})${d-i4-2}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_INDEX_2, "sub.2-d-indirect-with-post-increment-2-s1-indirect-with-index-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 ${d-i4-2}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_INDEX_2, "sub.2-d-indirect-with-pre-increment-2-s1-indirect-with-index-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 ${d-direct-addr},${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_2_D_DIRECT_S1_INDIRECT_WITH_OFFSET_2, "sub.2-d-direct-s1-indirect-with-offset-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 #${d-imm8},${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_OFFSET_2, "sub.2-d-immediate-2-s1-indirect-with-offset-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 (${d-An},${d-r}),${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_OFFSET_2, "sub.2-d-indirect-with-index-2-s1-indirect-with-offset-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 ${d-imm7-2}(${d-An}),${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_OFFSET_2, "sub.2-d-indirect-with-offset-2-s1-indirect-with-offset-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 (${d-An}),${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_2_S1_INDIRECT_WITH_OFFSET_2, "sub.2-d-indirect-2-s1-indirect-with-offset-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 (${d-An})${d-i4-2}++,${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_2, "sub.2-d-indirect-with-post-increment-2-s1-indirect-with-offset-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 ${d-i4-2}(${d-An})++,${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_2, "sub.2-d-indirect-with-pre-increment-2-s1-indirect-with-offset-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_2_D_DIRECT_S1_INDIRECT_2, "sub.2-d-direct-s1-indirect-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_2_D_IMMEDIATE_2_S1_INDIRECT_2, "sub.2-d-immediate-2-s1-indirect-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_2, "sub.2-d-indirect-with-index-2-s1-indirect-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 ${d-imm7-2}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_2, "sub.2-d-indirect-with-offset-2-s1-indirect-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_2_S1_INDIRECT_2, "sub.2-d-indirect-2-s1-indirect-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 (${d-An})${d-i4-2}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_2, "sub.2-d-indirect-with-post-increment-2-s1-indirect-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 ${d-i4-2}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_2, "sub.2-d-indirect-with-pre-increment-2-s1-indirect-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 ${d-direct-addr},(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_SUB_2_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_2, "sub.2-d-direct-s1-indirect-with-post-increment-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 #${d-imm8},(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_SUB_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "sub.2-d-immediate-2-s1-indirect-with-post-increment-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 (${d-An},${d-r}),(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "sub.2-d-indirect-with-index-2-s1-indirect-with-post-increment-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 ${d-imm7-2}(${d-An}),(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "sub.2-d-indirect-with-offset-2-s1-indirect-with-post-increment-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 (${d-An}),(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "sub.2-d-indirect-2-s1-indirect-with-post-increment-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 (${d-An})${d-i4-2}++,(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "sub.2-d-indirect-with-post-increment-2-s1-indirect-with-post-increment-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 ${d-i4-2}(${d-An})++,(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "sub.2-d-indirect-with-pre-increment-2-s1-indirect-with-post-increment-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 ${d-direct-addr},${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUB_2_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_2, "sub.2-d-direct-s1-indirect-with-pre-increment-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 #${d-imm8},${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUB_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "sub.2-d-immediate-2-s1-indirect-with-pre-increment-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 (${d-An},${d-r}),${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "sub.2-d-indirect-with-index-2-s1-indirect-with-pre-increment-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 ${d-imm7-2}(${d-An}),${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "sub.2-d-indirect-with-offset-2-s1-indirect-with-pre-increment-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 (${d-An}),${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "sub.2-d-indirect-2-s1-indirect-with-pre-increment-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 (${d-An})${d-i4-2}++,${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "sub.2-d-indirect-with-post-increment-2-s1-indirect-with-pre-increment-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* sub.2 ${d-i4-2}(${d-An})++,${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_SUB_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "sub.2-d-indirect-with-pre-increment-2-s1-indirect-with-pre-increment-2", "sub.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.1 ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADD_1_D_DIRECT_S1_DIRECT, "add.1-d-direct-s1-direct", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADD_1_D_IMMEDIATE_1_S1_DIRECT, "add.1-d-immediate-1-s1-direct", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_INDEX_1_S1_DIRECT, "add.1-d-indirect-with-index-1-s1-direct", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 ${d-imm7-1}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_OFFSET_1_S1_DIRECT, "add.1-d-indirect-with-offset-1-s1-direct", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_1_S1_DIRECT, "add.1-d-indirect-1-s1-direct", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 (${d-An})${d-i4-1}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_DIRECT, "add.1-d-indirect-with-post-increment-1-s1-direct", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 ${d-i4-1}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_DIRECT, "add.1-d-indirect-with-pre-increment-1-s1-direct", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADD_1_D_DIRECT_S1_IMMEDIATE, "add.1-d-direct-s1-immediate", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADD_1_D_IMMEDIATE_1_S1_IMMEDIATE, "add.1-d-immediate-1-s1-immediate", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_INDEX_1_S1_IMMEDIATE, "add.1-d-indirect-with-index-1-s1-immediate", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 ${d-imm7-1}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_OFFSET_1_S1_IMMEDIATE, "add.1-d-indirect-with-offset-1-s1-immediate", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_1_S1_IMMEDIATE, "add.1-d-indirect-1-s1-immediate", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 (${d-An})${d-i4-1}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_IMMEDIATE, "add.1-d-indirect-with-post-increment-1-s1-immediate", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 ${d-i4-1}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_IMMEDIATE, "add.1-d-indirect-with-pre-increment-1-s1-immediate", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADD_1_D_DIRECT_S1_INDIRECT_WITH_INDEX_1, "add.1-d-direct-s1-indirect-with-index-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADD_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_INDEX_1, "add.1-d-immediate-1-s1-indirect-with-index-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_INDEX_1, "add.1-d-indirect-with-index-1-s1-indirect-with-index-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 ${d-imm7-1}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_INDEX_1, "add.1-d-indirect-with-offset-1-s1-indirect-with-index-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_1_S1_INDIRECT_WITH_INDEX_1, "add.1-d-indirect-1-s1-indirect-with-index-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 (${d-An})${d-i4-1}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_INDEX_1, "add.1-d-indirect-with-post-increment-1-s1-indirect-with-index-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 ${d-i4-1}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_INDEX_1, "add.1-d-indirect-with-pre-increment-1-s1-indirect-with-index-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 ${d-direct-addr},${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_1_D_DIRECT_S1_INDIRECT_WITH_OFFSET_1, "add.1-d-direct-s1-indirect-with-offset-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 #${d-imm8},${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_OFFSET_1, "add.1-d-immediate-1-s1-indirect-with-offset-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 (${d-An},${d-r}),${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_OFFSET_1, "add.1-d-indirect-with-index-1-s1-indirect-with-offset-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 ${d-imm7-1}(${d-An}),${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_OFFSET_1, "add.1-d-indirect-with-offset-1-s1-indirect-with-offset-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 (${d-An}),${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_1_S1_INDIRECT_WITH_OFFSET_1, "add.1-d-indirect-1-s1-indirect-with-offset-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 (${d-An})${d-i4-1}++,${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_OFFSET_1, "add.1-d-indirect-with-post-increment-1-s1-indirect-with-offset-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 ${d-i4-1}(${d-An})++,${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_OFFSET_1, "add.1-d-indirect-with-pre-increment-1-s1-indirect-with-offset-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_1_D_DIRECT_S1_INDIRECT_1, "add.1-d-direct-s1-indirect-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_1_D_IMMEDIATE_1_S1_INDIRECT_1, "add.1-d-immediate-1-s1-indirect-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_1, "add.1-d-indirect-with-index-1-s1-indirect-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 ${d-imm7-1}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_1, "add.1-d-indirect-with-offset-1-s1-indirect-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_1_S1_INDIRECT_1, "add.1-d-indirect-1-s1-indirect-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 (${d-An})${d-i4-1}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_1, "add.1-d-indirect-with-post-increment-1-s1-indirect-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 ${d-i4-1}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_1, "add.1-d-indirect-with-pre-increment-1-s1-indirect-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 ${d-direct-addr},(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_ADD_1_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_1, "add.1-d-direct-s1-indirect-with-post-increment-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 #${d-imm8},(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_ADD_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "add.1-d-immediate-1-s1-indirect-with-post-increment-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 (${d-An},${d-r}),(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "add.1-d-indirect-with-index-1-s1-indirect-with-post-increment-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 ${d-imm7-1}(${d-An}),(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "add.1-d-indirect-with-offset-1-s1-indirect-with-post-increment-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 (${d-An}),(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "add.1-d-indirect-1-s1-indirect-with-post-increment-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 (${d-An})${d-i4-1}++,(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "add.1-d-indirect-with-post-increment-1-s1-indirect-with-post-increment-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 ${d-i4-1}(${d-An})++,(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "add.1-d-indirect-with-pre-increment-1-s1-indirect-with-post-increment-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 ${d-direct-addr},${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADD_1_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_1, "add.1-d-direct-s1-indirect-with-pre-increment-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 #${d-imm8},${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADD_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "add.1-d-immediate-1-s1-indirect-with-pre-increment-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 (${d-An},${d-r}),${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "add.1-d-indirect-with-index-1-s1-indirect-with-pre-increment-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 ${d-imm7-1}(${d-An}),${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "add.1-d-indirect-with-offset-1-s1-indirect-with-pre-increment-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 (${d-An}),${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "add.1-d-indirect-1-s1-indirect-with-pre-increment-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 (${d-An})${d-i4-1}++,${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "add.1-d-indirect-with-post-increment-1-s1-indirect-with-pre-increment-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.1 ${d-i4-1}(${d-An})++,${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADD_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "add.1-d-indirect-with-pre-increment-1-s1-indirect-with-pre-increment-1", "add.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* add.4 ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADD_4_D_DIRECT_S1_DIRECT, "add.4-d-direct-s1-direct", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADD_4_D_IMMEDIATE_4_S1_DIRECT, "add.4-d-immediate-4-s1-direct", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_INDEX_4_S1_DIRECT, "add.4-d-indirect-with-index-4-s1-direct", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 ${d-imm7-4}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT, "add.4-d-indirect-with-offset-4-s1-direct", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_4_S1_DIRECT, "add.4-d-indirect-4-s1-direct", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 (${d-An})${d-i4-4}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_DIRECT, "add.4-d-indirect-with-post-increment-4-s1-direct", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 ${d-i4-4}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT, "add.4-d-indirect-with-pre-increment-4-s1-direct", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADD_4_D_DIRECT_S1_IMMEDIATE, "add.4-d-direct-s1-immediate", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADD_4_D_IMMEDIATE_4_S1_IMMEDIATE, "add.4-d-immediate-4-s1-immediate", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_INDEX_4_S1_IMMEDIATE, "add.4-d-indirect-with-index-4-s1-immediate", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 ${d-imm7-4}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_OFFSET_4_S1_IMMEDIATE, "add.4-d-indirect-with-offset-4-s1-immediate", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_4_S1_IMMEDIATE, "add.4-d-indirect-4-s1-immediate", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 (${d-An})${d-i4-4}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_IMMEDIATE, "add.4-d-indirect-with-post-increment-4-s1-immediate", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 ${d-i4-4}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_IMMEDIATE, "add.4-d-indirect-with-pre-increment-4-s1-immediate", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADD_4_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "add.4-d-direct-s1-indirect-with-index-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADD_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_INDEX_4, "add.4-d-immediate-4-s1-indirect-with-index-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_INDEX_4, "add.4-d-indirect-with-index-4-s1-indirect-with-index-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_INDEX_4, "add.4-d-indirect-with-offset-4-s1-indirect-with-index-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_4_S1_INDIRECT_WITH_INDEX_4, "add.4-d-indirect-4-s1-indirect-with-index-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 (${d-An})${d-i4-4}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "add.4-d-indirect-with-post-increment-4-s1-indirect-with-index-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "add.4-d-indirect-with-pre-increment-4-s1-indirect-with-index-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 ${d-direct-addr},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_4_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "add.4-d-direct-s1-indirect-with-offset-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 #${d-imm8},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_OFFSET_4, "add.4-d-immediate-4-s1-indirect-with-offset-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 (${d-An},${d-r}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_OFFSET_4, "add.4-d-indirect-with-index-4-s1-indirect-with-offset-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_OFFSET_4, "add.4-d-indirect-with-offset-4-s1-indirect-with-offset-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 (${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_4_S1_INDIRECT_WITH_OFFSET_4, "add.4-d-indirect-4-s1-indirect-with-offset-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "add.4-d-indirect-with-post-increment-4-s1-indirect-with-offset-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "add.4-d-indirect-with-pre-increment-4-s1-indirect-with-offset-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_4_D_DIRECT_S1_INDIRECT_4, "add.4-d-direct-s1-indirect-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_4_D_IMMEDIATE_4_S1_INDIRECT_4, "add.4-d-immediate-4-s1-indirect-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_4, "add.4-d-indirect-with-index-4-s1-indirect-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 ${d-imm7-4}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_4, "add.4-d-indirect-with-offset-4-s1-indirect-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_4_S1_INDIRECT_4, "add.4-d-indirect-4-s1-indirect-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 (${d-An})${d-i4-4}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_4, "add.4-d-indirect-with-post-increment-4-s1-indirect-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 ${d-i4-4}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_4, "add.4-d-indirect-with-pre-increment-4-s1-indirect-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 ${d-direct-addr},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_ADD_4_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "add.4-d-direct-s1-indirect-with-post-increment-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 #${d-imm8},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_ADD_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "add.4-d-immediate-4-s1-indirect-with-post-increment-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 (${d-An},${d-r}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "add.4-d-indirect-with-index-4-s1-indirect-with-post-increment-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "add.4-d-indirect-with-offset-4-s1-indirect-with-post-increment-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 (${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "add.4-d-indirect-4-s1-indirect-with-post-increment-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "add.4-d-indirect-with-post-increment-4-s1-indirect-with-post-increment-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "add.4-d-indirect-with-pre-increment-4-s1-indirect-with-post-increment-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 ${d-direct-addr},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADD_4_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "add.4-d-direct-s1-indirect-with-pre-increment-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 #${d-imm8},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADD_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "add.4-d-immediate-4-s1-indirect-with-pre-increment-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 (${d-An},${d-r}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "add.4-d-indirect-with-index-4-s1-indirect-with-pre-increment-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "add.4-d-indirect-with-offset-4-s1-indirect-with-pre-increment-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 (${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "add.4-d-indirect-4-s1-indirect-with-pre-increment-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "add.4-d-indirect-with-post-increment-4-s1-indirect-with-pre-increment-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.4 ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADD_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "add.4-d-indirect-with-pre-increment-4-s1-indirect-with-pre-increment-4", "add.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADD_2_D_DIRECT_S1_DIRECT, "add.2-d-direct-s1-direct", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADD_2_D_IMMEDIATE_2_S1_DIRECT, "add.2-d-immediate-2-s1-direct", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_INDEX_2_S1_DIRECT, "add.2-d-indirect-with-index-2-s1-direct", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 ${d-imm7-2}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_OFFSET_2_S1_DIRECT, "add.2-d-indirect-with-offset-2-s1-direct", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_2_S1_DIRECT, "add.2-d-indirect-2-s1-direct", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 (${d-An})${d-i4-2}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_DIRECT, "add.2-d-indirect-with-post-increment-2-s1-direct", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 ${d-i4-2}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_DIRECT, "add.2-d-indirect-with-pre-increment-2-s1-direct", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADD_2_D_DIRECT_S1_IMMEDIATE, "add.2-d-direct-s1-immediate", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADD_2_D_IMMEDIATE_2_S1_IMMEDIATE, "add.2-d-immediate-2-s1-immediate", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_INDEX_2_S1_IMMEDIATE, "add.2-d-indirect-with-index-2-s1-immediate", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 ${d-imm7-2}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_OFFSET_2_S1_IMMEDIATE, "add.2-d-indirect-with-offset-2-s1-immediate", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_2_S1_IMMEDIATE, "add.2-d-indirect-2-s1-immediate", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 (${d-An})${d-i4-2}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_IMMEDIATE, "add.2-d-indirect-with-post-increment-2-s1-immediate", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 ${d-i4-2}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_IMMEDIATE, "add.2-d-indirect-with-pre-increment-2-s1-immediate", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADD_2_D_DIRECT_S1_INDIRECT_WITH_INDEX_2, "add.2-d-direct-s1-indirect-with-index-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADD_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_INDEX_2, "add.2-d-immediate-2-s1-indirect-with-index-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_INDEX_2, "add.2-d-indirect-with-index-2-s1-indirect-with-index-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 ${d-imm7-2}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_INDEX_2, "add.2-d-indirect-with-offset-2-s1-indirect-with-index-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_2_S1_INDIRECT_WITH_INDEX_2, "add.2-d-indirect-2-s1-indirect-with-index-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 (${d-An})${d-i4-2}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_INDEX_2, "add.2-d-indirect-with-post-increment-2-s1-indirect-with-index-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 ${d-i4-2}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_INDEX_2, "add.2-d-indirect-with-pre-increment-2-s1-indirect-with-index-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 ${d-direct-addr},${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_2_D_DIRECT_S1_INDIRECT_WITH_OFFSET_2, "add.2-d-direct-s1-indirect-with-offset-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 #${d-imm8},${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_OFFSET_2, "add.2-d-immediate-2-s1-indirect-with-offset-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 (${d-An},${d-r}),${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_OFFSET_2, "add.2-d-indirect-with-index-2-s1-indirect-with-offset-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 ${d-imm7-2}(${d-An}),${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_OFFSET_2, "add.2-d-indirect-with-offset-2-s1-indirect-with-offset-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 (${d-An}),${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_2_S1_INDIRECT_WITH_OFFSET_2, "add.2-d-indirect-2-s1-indirect-with-offset-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 (${d-An})${d-i4-2}++,${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_2, "add.2-d-indirect-with-post-increment-2-s1-indirect-with-offset-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 ${d-i4-2}(${d-An})++,${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_2, "add.2-d-indirect-with-pre-increment-2-s1-indirect-with-offset-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_2_D_DIRECT_S1_INDIRECT_2, "add.2-d-direct-s1-indirect-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_2_D_IMMEDIATE_2_S1_INDIRECT_2, "add.2-d-immediate-2-s1-indirect-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_2, "add.2-d-indirect-with-index-2-s1-indirect-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 ${d-imm7-2}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_2, "add.2-d-indirect-with-offset-2-s1-indirect-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_2_S1_INDIRECT_2, "add.2-d-indirect-2-s1-indirect-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 (${d-An})${d-i4-2}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_2, "add.2-d-indirect-with-post-increment-2-s1-indirect-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 ${d-i4-2}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_2, "add.2-d-indirect-with-pre-increment-2-s1-indirect-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 ${d-direct-addr},(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_ADD_2_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_2, "add.2-d-direct-s1-indirect-with-post-increment-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 #${d-imm8},(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_ADD_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "add.2-d-immediate-2-s1-indirect-with-post-increment-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 (${d-An},${d-r}),(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "add.2-d-indirect-with-index-2-s1-indirect-with-post-increment-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 ${d-imm7-2}(${d-An}),(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "add.2-d-indirect-with-offset-2-s1-indirect-with-post-increment-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 (${d-An}),(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "add.2-d-indirect-2-s1-indirect-with-post-increment-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 (${d-An})${d-i4-2}++,(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "add.2-d-indirect-with-post-increment-2-s1-indirect-with-post-increment-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 ${d-i4-2}(${d-An})++,(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "add.2-d-indirect-with-pre-increment-2-s1-indirect-with-post-increment-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 ${d-direct-addr},${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADD_2_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_2, "add.2-d-direct-s1-indirect-with-pre-increment-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 #${d-imm8},${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADD_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "add.2-d-immediate-2-s1-indirect-with-pre-increment-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 (${d-An},${d-r}),${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "add.2-d-indirect-with-index-2-s1-indirect-with-pre-increment-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 ${d-imm7-2}(${d-An}),${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "add.2-d-indirect-with-offset-2-s1-indirect-with-pre-increment-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 (${d-An}),${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "add.2-d-indirect-2-s1-indirect-with-pre-increment-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 (${d-An})${d-i4-2}++,${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "add.2-d-indirect-with-post-increment-2-s1-indirect-with-pre-increment-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* add.2 ${d-i4-2}(${d-An})++,${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_ADD_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "add.2-d-indirect-with-pre-increment-2-s1-indirect-with-pre-increment-2", "add.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 ${d-direct-addr},${s1-direct-addr} */
  {
    UBICOM32_INSN_NOT_4_D_DIRECT_S1_DIRECT, "not.4-d-direct-s1-direct", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 #${d-imm8},${s1-direct-addr} */
  {
    UBICOM32_INSN_NOT_4_D_IMMEDIATE_4_S1_DIRECT, "not.4-d-immediate-4-s1-direct", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 (${d-An},${d-r}),${s1-direct-addr} */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_INDEX_4_S1_DIRECT, "not.4-d-indirect-with-index-4-s1-direct", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 ${d-imm7-4}(${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT, "not.4-d-indirect-with-offset-4-s1-direct", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 (${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_4_S1_DIRECT, "not.4-d-indirect-4-s1-direct", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 (${d-An})${d-i4-4}++,${s1-direct-addr} */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_DIRECT, "not.4-d-indirect-with-post-increment-4-s1-direct", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 ${d-i4-4}(${d-An})++,${s1-direct-addr} */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT, "not.4-d-indirect-with-pre-increment-4-s1-direct", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 ${d-direct-addr},#${s1-imm8} */
  {
    UBICOM32_INSN_NOT_4_D_DIRECT_S1_IMMEDIATE, "not.4-d-direct-s1-immediate", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 #${d-imm8},#${s1-imm8} */
  {
    UBICOM32_INSN_NOT_4_D_IMMEDIATE_4_S1_IMMEDIATE, "not.4-d-immediate-4-s1-immediate", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 (${d-An},${d-r}),#${s1-imm8} */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_INDEX_4_S1_IMMEDIATE, "not.4-d-indirect-with-index-4-s1-immediate", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 ${d-imm7-4}(${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_OFFSET_4_S1_IMMEDIATE, "not.4-d-indirect-with-offset-4-s1-immediate", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 (${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_4_S1_IMMEDIATE, "not.4-d-indirect-4-s1-immediate", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 (${d-An})${d-i4-4}++,#${s1-imm8} */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_IMMEDIATE, "not.4-d-indirect-with-post-increment-4-s1-immediate", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 ${d-i4-4}(${d-An})++,#${s1-imm8} */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_IMMEDIATE, "not.4-d-indirect-with-pre-increment-4-s1-immediate", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 ${d-direct-addr},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_NOT_4_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "not.4-d-direct-s1-indirect-with-index-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 #${d-imm8},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_NOT_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_INDEX_4, "not.4-d-immediate-4-s1-indirect-with-index-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 (${d-An},${d-r}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_INDEX_4, "not.4-d-indirect-with-index-4-s1-indirect-with-index-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_INDEX_4, "not.4-d-indirect-with-offset-4-s1-indirect-with-index-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 (${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_4_S1_INDIRECT_WITH_INDEX_4, "not.4-d-indirect-4-s1-indirect-with-index-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 (${d-An})${d-i4-4}++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "not.4-d-indirect-with-post-increment-4-s1-indirect-with-index-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "not.4-d-indirect-with-pre-increment-4-s1-indirect-with-index-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 ${d-direct-addr},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_NOT_4_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "not.4-d-direct-s1-indirect-with-offset-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 #${d-imm8},${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_NOT_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_OFFSET_4, "not.4-d-immediate-4-s1-indirect-with-offset-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 (${d-An},${d-r}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_OFFSET_4, "not.4-d-indirect-with-index-4-s1-indirect-with-offset-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_OFFSET_4, "not.4-d-indirect-with-offset-4-s1-indirect-with-offset-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 (${d-An}),${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_4_S1_INDIRECT_WITH_OFFSET_4, "not.4-d-indirect-4-s1-indirect-with-offset-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "not.4-d-indirect-with-post-increment-4-s1-indirect-with-offset-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}) */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "not.4-d-indirect-with-pre-increment-4-s1-indirect-with-offset-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 ${d-direct-addr},(${s1-An}) */
  {
    UBICOM32_INSN_NOT_4_D_DIRECT_S1_INDIRECT_4, "not.4-d-direct-s1-indirect-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 #${d-imm8},(${s1-An}) */
  {
    UBICOM32_INSN_NOT_4_D_IMMEDIATE_4_S1_INDIRECT_4, "not.4-d-immediate-4-s1-indirect-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 (${d-An},${d-r}),(${s1-An}) */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_4, "not.4-d-indirect-with-index-4-s1-indirect-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 ${d-imm7-4}(${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_4, "not.4-d-indirect-with-offset-4-s1-indirect-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 (${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_4_S1_INDIRECT_4, "not.4-d-indirect-4-s1-indirect-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 (${d-An})${d-i4-4}++,(${s1-An}) */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_4, "not.4-d-indirect-with-post-increment-4-s1-indirect-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 ${d-i4-4}(${d-An})++,(${s1-An}) */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_4, "not.4-d-indirect-with-pre-increment-4-s1-indirect-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 ${d-direct-addr},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_NOT_4_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "not.4-d-direct-s1-indirect-with-post-increment-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 #${d-imm8},(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_NOT_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "not.4-d-immediate-4-s1-indirect-with-post-increment-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 (${d-An},${d-r}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "not.4-d-indirect-with-index-4-s1-indirect-with-post-increment-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "not.4-d-indirect-with-offset-4-s1-indirect-with-post-increment-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 (${d-An}),(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "not.4-d-indirect-4-s1-indirect-with-post-increment-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "not.4-d-indirect-with-post-increment-4-s1-indirect-with-post-increment-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++ */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "not.4-d-indirect-with-pre-increment-4-s1-indirect-with-post-increment-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 ${d-direct-addr},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_NOT_4_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "not.4-d-direct-s1-indirect-with-pre-increment-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 #${d-imm8},${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_NOT_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "not.4-d-immediate-4-s1-indirect-with-pre-increment-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 (${d-An},${d-r}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "not.4-d-indirect-with-index-4-s1-indirect-with-pre-increment-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "not.4-d-indirect-with-offset-4-s1-indirect-with-pre-increment-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 (${d-An}),${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "not.4-d-indirect-4-s1-indirect-with-pre-increment-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "not.4-d-indirect-with-post-increment-4-s1-indirect-with-pre-increment-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.4 ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++ */
  {
    UBICOM32_INSN_NOT_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "not.4-d-indirect-with-pre-increment-4-s1-indirect-with-pre-increment-4", "not.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 ${d-direct-addr},${s1-direct-addr} */
  {
    UBICOM32_INSN_NOT_2_D_DIRECT_S1_DIRECT, "not.2-d-direct-s1-direct", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 #${d-imm8},${s1-direct-addr} */
  {
    UBICOM32_INSN_NOT_2_D_IMMEDIATE_2_S1_DIRECT, "not.2-d-immediate-2-s1-direct", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 (${d-An},${d-r}),${s1-direct-addr} */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_INDEX_2_S1_DIRECT, "not.2-d-indirect-with-index-2-s1-direct", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 ${d-imm7-2}(${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_OFFSET_2_S1_DIRECT, "not.2-d-indirect-with-offset-2-s1-direct", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 (${d-An}),${s1-direct-addr} */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_2_S1_DIRECT, "not.2-d-indirect-2-s1-direct", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 (${d-An})${d-i4-2}++,${s1-direct-addr} */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_DIRECT, "not.2-d-indirect-with-post-increment-2-s1-direct", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 ${d-i4-2}(${d-An})++,${s1-direct-addr} */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_DIRECT, "not.2-d-indirect-with-pre-increment-2-s1-direct", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 ${d-direct-addr},#${s1-imm8} */
  {
    UBICOM32_INSN_NOT_2_D_DIRECT_S1_IMMEDIATE, "not.2-d-direct-s1-immediate", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 #${d-imm8},#${s1-imm8} */
  {
    UBICOM32_INSN_NOT_2_D_IMMEDIATE_2_S1_IMMEDIATE, "not.2-d-immediate-2-s1-immediate", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 (${d-An},${d-r}),#${s1-imm8} */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_INDEX_2_S1_IMMEDIATE, "not.2-d-indirect-with-index-2-s1-immediate", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 ${d-imm7-2}(${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_OFFSET_2_S1_IMMEDIATE, "not.2-d-indirect-with-offset-2-s1-immediate", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 (${d-An}),#${s1-imm8} */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_2_S1_IMMEDIATE, "not.2-d-indirect-2-s1-immediate", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 (${d-An})${d-i4-2}++,#${s1-imm8} */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_IMMEDIATE, "not.2-d-indirect-with-post-increment-2-s1-immediate", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 ${d-i4-2}(${d-An})++,#${s1-imm8} */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_IMMEDIATE, "not.2-d-indirect-with-pre-increment-2-s1-immediate", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 ${d-direct-addr},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_NOT_2_D_DIRECT_S1_INDIRECT_WITH_INDEX_2, "not.2-d-direct-s1-indirect-with-index-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 #${d-imm8},(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_NOT_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_INDEX_2, "not.2-d-immediate-2-s1-indirect-with-index-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 (${d-An},${d-r}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_INDEX_2, "not.2-d-indirect-with-index-2-s1-indirect-with-index-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 ${d-imm7-2}(${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_INDEX_2, "not.2-d-indirect-with-offset-2-s1-indirect-with-index-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 (${d-An}),(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_2_S1_INDIRECT_WITH_INDEX_2, "not.2-d-indirect-2-s1-indirect-with-index-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 (${d-An})${d-i4-2}++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_INDEX_2, "not.2-d-indirect-with-post-increment-2-s1-indirect-with-index-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 ${d-i4-2}(${d-An})++,(${s1-An},${s1-r}) */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_INDEX_2, "not.2-d-indirect-with-pre-increment-2-s1-indirect-with-index-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 ${d-direct-addr},${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_NOT_2_D_DIRECT_S1_INDIRECT_WITH_OFFSET_2, "not.2-d-direct-s1-indirect-with-offset-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 #${d-imm8},${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_NOT_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_OFFSET_2, "not.2-d-immediate-2-s1-indirect-with-offset-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 (${d-An},${d-r}),${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_OFFSET_2, "not.2-d-indirect-with-index-2-s1-indirect-with-offset-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 ${d-imm7-2}(${d-An}),${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_OFFSET_2, "not.2-d-indirect-with-offset-2-s1-indirect-with-offset-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 (${d-An}),${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_2_S1_INDIRECT_WITH_OFFSET_2, "not.2-d-indirect-2-s1-indirect-with-offset-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 (${d-An})${d-i4-2}++,${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_2, "not.2-d-indirect-with-post-increment-2-s1-indirect-with-offset-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 ${d-i4-2}(${d-An})++,${s1-imm7-2}(${s1-An}) */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_2, "not.2-d-indirect-with-pre-increment-2-s1-indirect-with-offset-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 ${d-direct-addr},(${s1-An}) */
  {
    UBICOM32_INSN_NOT_2_D_DIRECT_S1_INDIRECT_2, "not.2-d-direct-s1-indirect-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 #${d-imm8},(${s1-An}) */
  {
    UBICOM32_INSN_NOT_2_D_IMMEDIATE_2_S1_INDIRECT_2, "not.2-d-immediate-2-s1-indirect-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 (${d-An},${d-r}),(${s1-An}) */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_2, "not.2-d-indirect-with-index-2-s1-indirect-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 ${d-imm7-2}(${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_2, "not.2-d-indirect-with-offset-2-s1-indirect-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 (${d-An}),(${s1-An}) */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_2_S1_INDIRECT_2, "not.2-d-indirect-2-s1-indirect-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 (${d-An})${d-i4-2}++,(${s1-An}) */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_2, "not.2-d-indirect-with-post-increment-2-s1-indirect-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 ${d-i4-2}(${d-An})++,(${s1-An}) */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_2, "not.2-d-indirect-with-pre-increment-2-s1-indirect-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 ${d-direct-addr},(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_NOT_2_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_2, "not.2-d-direct-s1-indirect-with-post-increment-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 #${d-imm8},(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_NOT_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "not.2-d-immediate-2-s1-indirect-with-post-increment-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 (${d-An},${d-r}),(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "not.2-d-indirect-with-index-2-s1-indirect-with-post-increment-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 ${d-imm7-2}(${d-An}),(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "not.2-d-indirect-with-offset-2-s1-indirect-with-post-increment-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 (${d-An}),(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "not.2-d-indirect-2-s1-indirect-with-post-increment-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 (${d-An})${d-i4-2}++,(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "not.2-d-indirect-with-post-increment-2-s1-indirect-with-post-increment-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 ${d-i4-2}(${d-An})++,(${s1-An})${s1-i4-2}++ */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "not.2-d-indirect-with-pre-increment-2-s1-indirect-with-post-increment-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 ${d-direct-addr},${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_NOT_2_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_2, "not.2-d-direct-s1-indirect-with-pre-increment-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 #${d-imm8},${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_NOT_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "not.2-d-immediate-2-s1-indirect-with-pre-increment-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 (${d-An},${d-r}),${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "not.2-d-indirect-with-index-2-s1-indirect-with-pre-increment-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 ${d-imm7-2}(${d-An}),${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "not.2-d-indirect-with-offset-2-s1-indirect-with-pre-increment-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 (${d-An}),${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "not.2-d-indirect-2-s1-indirect-with-pre-increment-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 (${d-An})${d-i4-2}++,${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "not.2-d-indirect-with-post-increment-2-s1-indirect-with-pre-increment-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* not.2 ${d-i4-2}(${d-An})++,${s1-i4-2}(${s1-An})++ */
  {
    UBICOM32_INSN_NOT_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "not.2-d-indirect-with-pre-increment-2-s1-indirect-with-pre-increment-2", "not.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.1 ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_XOR_1_D_DIRECT_S1_DIRECT, "xor.1-d-direct-s1-direct", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_XOR_1_D_IMMEDIATE_1_S1_DIRECT, "xor.1-d-immediate-1-s1-direct", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_INDEX_1_S1_DIRECT, "xor.1-d-indirect-with-index-1-s1-direct", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 ${d-imm7-1}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_OFFSET_1_S1_DIRECT, "xor.1-d-indirect-with-offset-1-s1-direct", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_1_S1_DIRECT, "xor.1-d-indirect-1-s1-direct", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 (${d-An})${d-i4-1}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_DIRECT, "xor.1-d-indirect-with-post-increment-1-s1-direct", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 ${d-i4-1}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_DIRECT, "xor.1-d-indirect-with-pre-increment-1-s1-direct", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_XOR_1_D_DIRECT_S1_IMMEDIATE, "xor.1-d-direct-s1-immediate", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_XOR_1_D_IMMEDIATE_1_S1_IMMEDIATE, "xor.1-d-immediate-1-s1-immediate", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_INDEX_1_S1_IMMEDIATE, "xor.1-d-indirect-with-index-1-s1-immediate", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 ${d-imm7-1}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_OFFSET_1_S1_IMMEDIATE, "xor.1-d-indirect-with-offset-1-s1-immediate", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_1_S1_IMMEDIATE, "xor.1-d-indirect-1-s1-immediate", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 (${d-An})${d-i4-1}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_IMMEDIATE, "xor.1-d-indirect-with-post-increment-1-s1-immediate", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 ${d-i4-1}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_IMMEDIATE, "xor.1-d-indirect-with-pre-increment-1-s1-immediate", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_XOR_1_D_DIRECT_S1_INDIRECT_WITH_INDEX_1, "xor.1-d-direct-s1-indirect-with-index-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_XOR_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_INDEX_1, "xor.1-d-immediate-1-s1-indirect-with-index-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_INDEX_1, "xor.1-d-indirect-with-index-1-s1-indirect-with-index-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 ${d-imm7-1}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_INDEX_1, "xor.1-d-indirect-with-offset-1-s1-indirect-with-index-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_1_S1_INDIRECT_WITH_INDEX_1, "xor.1-d-indirect-1-s1-indirect-with-index-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 (${d-An})${d-i4-1}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_INDEX_1, "xor.1-d-indirect-with-post-increment-1-s1-indirect-with-index-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 ${d-i4-1}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_INDEX_1, "xor.1-d-indirect-with-pre-increment-1-s1-indirect-with-index-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 ${d-direct-addr},${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_1_D_DIRECT_S1_INDIRECT_WITH_OFFSET_1, "xor.1-d-direct-s1-indirect-with-offset-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 #${d-imm8},${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_OFFSET_1, "xor.1-d-immediate-1-s1-indirect-with-offset-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 (${d-An},${d-r}),${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_OFFSET_1, "xor.1-d-indirect-with-index-1-s1-indirect-with-offset-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 ${d-imm7-1}(${d-An}),${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_OFFSET_1, "xor.1-d-indirect-with-offset-1-s1-indirect-with-offset-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 (${d-An}),${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_1_S1_INDIRECT_WITH_OFFSET_1, "xor.1-d-indirect-1-s1-indirect-with-offset-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 (${d-An})${d-i4-1}++,${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_OFFSET_1, "xor.1-d-indirect-with-post-increment-1-s1-indirect-with-offset-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 ${d-i4-1}(${d-An})++,${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_OFFSET_1, "xor.1-d-indirect-with-pre-increment-1-s1-indirect-with-offset-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_1_D_DIRECT_S1_INDIRECT_1, "xor.1-d-direct-s1-indirect-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_1_D_IMMEDIATE_1_S1_INDIRECT_1, "xor.1-d-immediate-1-s1-indirect-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_1, "xor.1-d-indirect-with-index-1-s1-indirect-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 ${d-imm7-1}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_1, "xor.1-d-indirect-with-offset-1-s1-indirect-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_1_S1_INDIRECT_1, "xor.1-d-indirect-1-s1-indirect-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 (${d-An})${d-i4-1}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_1, "xor.1-d-indirect-with-post-increment-1-s1-indirect-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 ${d-i4-1}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_1, "xor.1-d-indirect-with-pre-increment-1-s1-indirect-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 ${d-direct-addr},(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_XOR_1_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_1, "xor.1-d-direct-s1-indirect-with-post-increment-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 #${d-imm8},(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_XOR_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "xor.1-d-immediate-1-s1-indirect-with-post-increment-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 (${d-An},${d-r}),(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "xor.1-d-indirect-with-index-1-s1-indirect-with-post-increment-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 ${d-imm7-1}(${d-An}),(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "xor.1-d-indirect-with-offset-1-s1-indirect-with-post-increment-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 (${d-An}),(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "xor.1-d-indirect-1-s1-indirect-with-post-increment-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 (${d-An})${d-i4-1}++,(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "xor.1-d-indirect-with-post-increment-1-s1-indirect-with-post-increment-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 ${d-i4-1}(${d-An})++,(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "xor.1-d-indirect-with-pre-increment-1-s1-indirect-with-post-increment-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 ${d-direct-addr},${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_XOR_1_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_1, "xor.1-d-direct-s1-indirect-with-pre-increment-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 #${d-imm8},${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_XOR_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "xor.1-d-immediate-1-s1-indirect-with-pre-increment-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 (${d-An},${d-r}),${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "xor.1-d-indirect-with-index-1-s1-indirect-with-pre-increment-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 ${d-imm7-1}(${d-An}),${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "xor.1-d-indirect-with-offset-1-s1-indirect-with-pre-increment-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 (${d-An}),${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "xor.1-d-indirect-1-s1-indirect-with-pre-increment-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 (${d-An})${d-i4-1}++,${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "xor.1-d-indirect-with-post-increment-1-s1-indirect-with-pre-increment-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.1 ${d-i4-1}(${d-An})++,${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_XOR_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "xor.1-d-indirect-with-pre-increment-1-s1-indirect-with-pre-increment-1", "xor.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_OR_1_D_DIRECT_S1_DIRECT, "or.1-d-direct-s1-direct", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_OR_1_D_IMMEDIATE_1_S1_DIRECT, "or.1-d-immediate-1-s1-direct", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_INDEX_1_S1_DIRECT, "or.1-d-indirect-with-index-1-s1-direct", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 ${d-imm7-1}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_OFFSET_1_S1_DIRECT, "or.1-d-indirect-with-offset-1-s1-direct", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_1_S1_DIRECT, "or.1-d-indirect-1-s1-direct", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 (${d-An})${d-i4-1}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_DIRECT, "or.1-d-indirect-with-post-increment-1-s1-direct", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 ${d-i4-1}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_DIRECT, "or.1-d-indirect-with-pre-increment-1-s1-direct", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_OR_1_D_DIRECT_S1_IMMEDIATE, "or.1-d-direct-s1-immediate", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_OR_1_D_IMMEDIATE_1_S1_IMMEDIATE, "or.1-d-immediate-1-s1-immediate", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_INDEX_1_S1_IMMEDIATE, "or.1-d-indirect-with-index-1-s1-immediate", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 ${d-imm7-1}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_OFFSET_1_S1_IMMEDIATE, "or.1-d-indirect-with-offset-1-s1-immediate", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_1_S1_IMMEDIATE, "or.1-d-indirect-1-s1-immediate", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 (${d-An})${d-i4-1}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_IMMEDIATE, "or.1-d-indirect-with-post-increment-1-s1-immediate", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 ${d-i4-1}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_IMMEDIATE, "or.1-d-indirect-with-pre-increment-1-s1-immediate", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_OR_1_D_DIRECT_S1_INDIRECT_WITH_INDEX_1, "or.1-d-direct-s1-indirect-with-index-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_OR_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_INDEX_1, "or.1-d-immediate-1-s1-indirect-with-index-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_INDEX_1, "or.1-d-indirect-with-index-1-s1-indirect-with-index-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 ${d-imm7-1}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_INDEX_1, "or.1-d-indirect-with-offset-1-s1-indirect-with-index-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_1_S1_INDIRECT_WITH_INDEX_1, "or.1-d-indirect-1-s1-indirect-with-index-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 (${d-An})${d-i4-1}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_INDEX_1, "or.1-d-indirect-with-post-increment-1-s1-indirect-with-index-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 ${d-i4-1}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_INDEX_1, "or.1-d-indirect-with-pre-increment-1-s1-indirect-with-index-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 ${d-direct-addr},${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_1_D_DIRECT_S1_INDIRECT_WITH_OFFSET_1, "or.1-d-direct-s1-indirect-with-offset-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 #${d-imm8},${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_OFFSET_1, "or.1-d-immediate-1-s1-indirect-with-offset-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 (${d-An},${d-r}),${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_OFFSET_1, "or.1-d-indirect-with-index-1-s1-indirect-with-offset-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 ${d-imm7-1}(${d-An}),${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_OFFSET_1, "or.1-d-indirect-with-offset-1-s1-indirect-with-offset-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 (${d-An}),${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_1_S1_INDIRECT_WITH_OFFSET_1, "or.1-d-indirect-1-s1-indirect-with-offset-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 (${d-An})${d-i4-1}++,${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_OFFSET_1, "or.1-d-indirect-with-post-increment-1-s1-indirect-with-offset-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 ${d-i4-1}(${d-An})++,${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_OFFSET_1, "or.1-d-indirect-with-pre-increment-1-s1-indirect-with-offset-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_1_D_DIRECT_S1_INDIRECT_1, "or.1-d-direct-s1-indirect-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_1_D_IMMEDIATE_1_S1_INDIRECT_1, "or.1-d-immediate-1-s1-indirect-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_1, "or.1-d-indirect-with-index-1-s1-indirect-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 ${d-imm7-1}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_1, "or.1-d-indirect-with-offset-1-s1-indirect-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_1_S1_INDIRECT_1, "or.1-d-indirect-1-s1-indirect-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 (${d-An})${d-i4-1}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_1, "or.1-d-indirect-with-post-increment-1-s1-indirect-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 ${d-i4-1}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_1, "or.1-d-indirect-with-pre-increment-1-s1-indirect-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 ${d-direct-addr},(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_OR_1_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_1, "or.1-d-direct-s1-indirect-with-post-increment-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 #${d-imm8},(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_OR_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "or.1-d-immediate-1-s1-indirect-with-post-increment-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 (${d-An},${d-r}),(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "or.1-d-indirect-with-index-1-s1-indirect-with-post-increment-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 ${d-imm7-1}(${d-An}),(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "or.1-d-indirect-with-offset-1-s1-indirect-with-post-increment-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 (${d-An}),(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "or.1-d-indirect-1-s1-indirect-with-post-increment-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 (${d-An})${d-i4-1}++,(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "or.1-d-indirect-with-post-increment-1-s1-indirect-with-post-increment-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 ${d-i4-1}(${d-An})++,(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "or.1-d-indirect-with-pre-increment-1-s1-indirect-with-post-increment-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 ${d-direct-addr},${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_OR_1_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_1, "or.1-d-direct-s1-indirect-with-pre-increment-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 #${d-imm8},${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_OR_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "or.1-d-immediate-1-s1-indirect-with-pre-increment-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 (${d-An},${d-r}),${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "or.1-d-indirect-with-index-1-s1-indirect-with-pre-increment-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 ${d-imm7-1}(${d-An}),${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "or.1-d-indirect-with-offset-1-s1-indirect-with-pre-increment-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 (${d-An}),${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "or.1-d-indirect-1-s1-indirect-with-pre-increment-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 (${d-An})${d-i4-1}++,${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "or.1-d-indirect-with-post-increment-1-s1-indirect-with-pre-increment-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* or.1 ${d-i4-1}(${d-An})++,${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_OR_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "or.1-d-indirect-with-pre-increment-1-s1-indirect-with-pre-increment-1", "or.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_AND_1_D_DIRECT_S1_DIRECT, "and.1-d-direct-s1-direct", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_AND_1_D_IMMEDIATE_1_S1_DIRECT, "and.1-d-immediate-1-s1-direct", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_INDEX_1_S1_DIRECT, "and.1-d-indirect-with-index-1-s1-direct", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 ${d-imm7-1}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_OFFSET_1_S1_DIRECT, "and.1-d-indirect-with-offset-1-s1-direct", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_1_S1_DIRECT, "and.1-d-indirect-1-s1-direct", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 (${d-An})${d-i4-1}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_DIRECT, "and.1-d-indirect-with-post-increment-1-s1-direct", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 ${d-i4-1}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_DIRECT, "and.1-d-indirect-with-pre-increment-1-s1-direct", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_AND_1_D_DIRECT_S1_IMMEDIATE, "and.1-d-direct-s1-immediate", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_AND_1_D_IMMEDIATE_1_S1_IMMEDIATE, "and.1-d-immediate-1-s1-immediate", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_INDEX_1_S1_IMMEDIATE, "and.1-d-indirect-with-index-1-s1-immediate", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 ${d-imm7-1}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_OFFSET_1_S1_IMMEDIATE, "and.1-d-indirect-with-offset-1-s1-immediate", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_1_S1_IMMEDIATE, "and.1-d-indirect-1-s1-immediate", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 (${d-An})${d-i4-1}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_IMMEDIATE, "and.1-d-indirect-with-post-increment-1-s1-immediate", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 ${d-i4-1}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_IMMEDIATE, "and.1-d-indirect-with-pre-increment-1-s1-immediate", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_AND_1_D_DIRECT_S1_INDIRECT_WITH_INDEX_1, "and.1-d-direct-s1-indirect-with-index-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_AND_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_INDEX_1, "and.1-d-immediate-1-s1-indirect-with-index-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_INDEX_1, "and.1-d-indirect-with-index-1-s1-indirect-with-index-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 ${d-imm7-1}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_INDEX_1, "and.1-d-indirect-with-offset-1-s1-indirect-with-index-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_1_S1_INDIRECT_WITH_INDEX_1, "and.1-d-indirect-1-s1-indirect-with-index-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 (${d-An})${d-i4-1}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_INDEX_1, "and.1-d-indirect-with-post-increment-1-s1-indirect-with-index-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 ${d-i4-1}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_INDEX_1, "and.1-d-indirect-with-pre-increment-1-s1-indirect-with-index-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 ${d-direct-addr},${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_1_D_DIRECT_S1_INDIRECT_WITH_OFFSET_1, "and.1-d-direct-s1-indirect-with-offset-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 #${d-imm8},${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_OFFSET_1, "and.1-d-immediate-1-s1-indirect-with-offset-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 (${d-An},${d-r}),${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_OFFSET_1, "and.1-d-indirect-with-index-1-s1-indirect-with-offset-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 ${d-imm7-1}(${d-An}),${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_OFFSET_1, "and.1-d-indirect-with-offset-1-s1-indirect-with-offset-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 (${d-An}),${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_1_S1_INDIRECT_WITH_OFFSET_1, "and.1-d-indirect-1-s1-indirect-with-offset-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 (${d-An})${d-i4-1}++,${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_OFFSET_1, "and.1-d-indirect-with-post-increment-1-s1-indirect-with-offset-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 ${d-i4-1}(${d-An})++,${s1-imm7-1}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_OFFSET_1, "and.1-d-indirect-with-pre-increment-1-s1-indirect-with-offset-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_1_D_DIRECT_S1_INDIRECT_1, "and.1-d-direct-s1-indirect-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_1_D_IMMEDIATE_1_S1_INDIRECT_1, "and.1-d-immediate-1-s1-indirect-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_1, "and.1-d-indirect-with-index-1-s1-indirect-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 ${d-imm7-1}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_1, "and.1-d-indirect-with-offset-1-s1-indirect-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_1_S1_INDIRECT_1, "and.1-d-indirect-1-s1-indirect-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 (${d-An})${d-i4-1}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_1, "and.1-d-indirect-with-post-increment-1-s1-indirect-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 ${d-i4-1}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_1, "and.1-d-indirect-with-pre-increment-1-s1-indirect-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 ${d-direct-addr},(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_AND_1_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_1, "and.1-d-direct-s1-indirect-with-post-increment-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 #${d-imm8},(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_AND_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "and.1-d-immediate-1-s1-indirect-with-post-increment-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 (${d-An},${d-r}),(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "and.1-d-indirect-with-index-1-s1-indirect-with-post-increment-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 ${d-imm7-1}(${d-An}),(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "and.1-d-indirect-with-offset-1-s1-indirect-with-post-increment-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 (${d-An}),(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "and.1-d-indirect-1-s1-indirect-with-post-increment-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 (${d-An})${d-i4-1}++,(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "and.1-d-indirect-with-post-increment-1-s1-indirect-with-post-increment-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 ${d-i4-1}(${d-An})++,(${s1-An})${s1-i4-1}++,${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_POST_INCREMENT_1, "and.1-d-indirect-with-pre-increment-1-s1-indirect-with-post-increment-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 ${d-direct-addr},${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_AND_1_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_1, "and.1-d-direct-s1-indirect-with-pre-increment-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 #${d-imm8},${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_AND_1_D_IMMEDIATE_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "and.1-d-immediate-1-s1-indirect-with-pre-increment-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 (${d-An},${d-r}),${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_INDEX_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "and.1-d-indirect-with-index-1-s1-indirect-with-pre-increment-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 ${d-imm7-1}(${d-An}),${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_OFFSET_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "and.1-d-indirect-with-offset-1-s1-indirect-with-pre-increment-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 (${d-An}),${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "and.1-d-indirect-1-s1-indirect-with-pre-increment-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 (${d-An})${d-i4-1}++,${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_POST_INCREMENT_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "and.1-d-indirect-with-post-increment-1-s1-indirect-with-pre-increment-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* and.1 ${d-i4-1}(${d-An})++,${s1-i4-1}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_AND_1_D_INDIRECT_WITH_PRE_INCREMENT_1_S1_INDIRECT_WITH_PRE_INCREMENT_1, "and.1-d-indirect-with-pre-increment-1-s1-indirect-with-pre-increment-1", "and.1", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* xor.4 ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_XOR_4_D_DIRECT_S1_DIRECT, "xor.4-d-direct-s1-direct", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_XOR_4_D_IMMEDIATE_4_S1_DIRECT, "xor.4-d-immediate-4-s1-direct", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_INDEX_4_S1_DIRECT, "xor.4-d-indirect-with-index-4-s1-direct", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 ${d-imm7-4}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT, "xor.4-d-indirect-with-offset-4-s1-direct", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_4_S1_DIRECT, "xor.4-d-indirect-4-s1-direct", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 (${d-An})${d-i4-4}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_DIRECT, "xor.4-d-indirect-with-post-increment-4-s1-direct", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 ${d-i4-4}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT, "xor.4-d-indirect-with-pre-increment-4-s1-direct", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_XOR_4_D_DIRECT_S1_IMMEDIATE, "xor.4-d-direct-s1-immediate", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_XOR_4_D_IMMEDIATE_4_S1_IMMEDIATE, "xor.4-d-immediate-4-s1-immediate", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_INDEX_4_S1_IMMEDIATE, "xor.4-d-indirect-with-index-4-s1-immediate", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 ${d-imm7-4}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_OFFSET_4_S1_IMMEDIATE, "xor.4-d-indirect-with-offset-4-s1-immediate", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_4_S1_IMMEDIATE, "xor.4-d-indirect-4-s1-immediate", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 (${d-An})${d-i4-4}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_IMMEDIATE, "xor.4-d-indirect-with-post-increment-4-s1-immediate", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 ${d-i4-4}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_IMMEDIATE, "xor.4-d-indirect-with-pre-increment-4-s1-immediate", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_XOR_4_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "xor.4-d-direct-s1-indirect-with-index-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_XOR_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_INDEX_4, "xor.4-d-immediate-4-s1-indirect-with-index-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_INDEX_4, "xor.4-d-indirect-with-index-4-s1-indirect-with-index-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_INDEX_4, "xor.4-d-indirect-with-offset-4-s1-indirect-with-index-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_4_S1_INDIRECT_WITH_INDEX_4, "xor.4-d-indirect-4-s1-indirect-with-index-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 (${d-An})${d-i4-4}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "xor.4-d-indirect-with-post-increment-4-s1-indirect-with-index-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "xor.4-d-indirect-with-pre-increment-4-s1-indirect-with-index-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 ${d-direct-addr},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_4_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "xor.4-d-direct-s1-indirect-with-offset-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 #${d-imm8},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_OFFSET_4, "xor.4-d-immediate-4-s1-indirect-with-offset-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 (${d-An},${d-r}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_OFFSET_4, "xor.4-d-indirect-with-index-4-s1-indirect-with-offset-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_OFFSET_4, "xor.4-d-indirect-with-offset-4-s1-indirect-with-offset-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 (${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_4_S1_INDIRECT_WITH_OFFSET_4, "xor.4-d-indirect-4-s1-indirect-with-offset-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "xor.4-d-indirect-with-post-increment-4-s1-indirect-with-offset-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "xor.4-d-indirect-with-pre-increment-4-s1-indirect-with-offset-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_4_D_DIRECT_S1_INDIRECT_4, "xor.4-d-direct-s1-indirect-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_4_D_IMMEDIATE_4_S1_INDIRECT_4, "xor.4-d-immediate-4-s1-indirect-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_4, "xor.4-d-indirect-with-index-4-s1-indirect-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 ${d-imm7-4}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_4, "xor.4-d-indirect-with-offset-4-s1-indirect-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_4_S1_INDIRECT_4, "xor.4-d-indirect-4-s1-indirect-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 (${d-An})${d-i4-4}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_4, "xor.4-d-indirect-with-post-increment-4-s1-indirect-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 ${d-i4-4}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_4, "xor.4-d-indirect-with-pre-increment-4-s1-indirect-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 ${d-direct-addr},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_XOR_4_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "xor.4-d-direct-s1-indirect-with-post-increment-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 #${d-imm8},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_XOR_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "xor.4-d-immediate-4-s1-indirect-with-post-increment-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 (${d-An},${d-r}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "xor.4-d-indirect-with-index-4-s1-indirect-with-post-increment-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "xor.4-d-indirect-with-offset-4-s1-indirect-with-post-increment-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 (${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "xor.4-d-indirect-4-s1-indirect-with-post-increment-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "xor.4-d-indirect-with-post-increment-4-s1-indirect-with-post-increment-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "xor.4-d-indirect-with-pre-increment-4-s1-indirect-with-post-increment-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 ${d-direct-addr},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_XOR_4_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "xor.4-d-direct-s1-indirect-with-pre-increment-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 #${d-imm8},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_XOR_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "xor.4-d-immediate-4-s1-indirect-with-pre-increment-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 (${d-An},${d-r}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "xor.4-d-indirect-with-index-4-s1-indirect-with-pre-increment-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "xor.4-d-indirect-with-offset-4-s1-indirect-with-pre-increment-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 (${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "xor.4-d-indirect-4-s1-indirect-with-pre-increment-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "xor.4-d-indirect-with-post-increment-4-s1-indirect-with-pre-increment-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.4 ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_XOR_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "xor.4-d-indirect-with-pre-increment-4-s1-indirect-with-pre-increment-4", "xor.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_XOR_2_D_DIRECT_S1_DIRECT, "xor.2-d-direct-s1-direct", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_XOR_2_D_IMMEDIATE_2_S1_DIRECT, "xor.2-d-immediate-2-s1-direct", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_INDEX_2_S1_DIRECT, "xor.2-d-indirect-with-index-2-s1-direct", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 ${d-imm7-2}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_OFFSET_2_S1_DIRECT, "xor.2-d-indirect-with-offset-2-s1-direct", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_2_S1_DIRECT, "xor.2-d-indirect-2-s1-direct", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 (${d-An})${d-i4-2}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_DIRECT, "xor.2-d-indirect-with-post-increment-2-s1-direct", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 ${d-i4-2}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_DIRECT, "xor.2-d-indirect-with-pre-increment-2-s1-direct", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_XOR_2_D_DIRECT_S1_IMMEDIATE, "xor.2-d-direct-s1-immediate", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_XOR_2_D_IMMEDIATE_2_S1_IMMEDIATE, "xor.2-d-immediate-2-s1-immediate", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_INDEX_2_S1_IMMEDIATE, "xor.2-d-indirect-with-index-2-s1-immediate", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 ${d-imm7-2}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_OFFSET_2_S1_IMMEDIATE, "xor.2-d-indirect-with-offset-2-s1-immediate", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_2_S1_IMMEDIATE, "xor.2-d-indirect-2-s1-immediate", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 (${d-An})${d-i4-2}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_IMMEDIATE, "xor.2-d-indirect-with-post-increment-2-s1-immediate", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 ${d-i4-2}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_IMMEDIATE, "xor.2-d-indirect-with-pre-increment-2-s1-immediate", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_XOR_2_D_DIRECT_S1_INDIRECT_WITH_INDEX_2, "xor.2-d-direct-s1-indirect-with-index-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_XOR_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_INDEX_2, "xor.2-d-immediate-2-s1-indirect-with-index-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_INDEX_2, "xor.2-d-indirect-with-index-2-s1-indirect-with-index-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 ${d-imm7-2}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_INDEX_2, "xor.2-d-indirect-with-offset-2-s1-indirect-with-index-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_2_S1_INDIRECT_WITH_INDEX_2, "xor.2-d-indirect-2-s1-indirect-with-index-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 (${d-An})${d-i4-2}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_INDEX_2, "xor.2-d-indirect-with-post-increment-2-s1-indirect-with-index-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 ${d-i4-2}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_INDEX_2, "xor.2-d-indirect-with-pre-increment-2-s1-indirect-with-index-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 ${d-direct-addr},${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_2_D_DIRECT_S1_INDIRECT_WITH_OFFSET_2, "xor.2-d-direct-s1-indirect-with-offset-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 #${d-imm8},${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_OFFSET_2, "xor.2-d-immediate-2-s1-indirect-with-offset-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 (${d-An},${d-r}),${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_OFFSET_2, "xor.2-d-indirect-with-index-2-s1-indirect-with-offset-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 ${d-imm7-2}(${d-An}),${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_OFFSET_2, "xor.2-d-indirect-with-offset-2-s1-indirect-with-offset-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 (${d-An}),${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_2_S1_INDIRECT_WITH_OFFSET_2, "xor.2-d-indirect-2-s1-indirect-with-offset-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 (${d-An})${d-i4-2}++,${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_2, "xor.2-d-indirect-with-post-increment-2-s1-indirect-with-offset-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 ${d-i4-2}(${d-An})++,${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_2, "xor.2-d-indirect-with-pre-increment-2-s1-indirect-with-offset-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_2_D_DIRECT_S1_INDIRECT_2, "xor.2-d-direct-s1-indirect-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_2_D_IMMEDIATE_2_S1_INDIRECT_2, "xor.2-d-immediate-2-s1-indirect-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_2, "xor.2-d-indirect-with-index-2-s1-indirect-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 ${d-imm7-2}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_2, "xor.2-d-indirect-with-offset-2-s1-indirect-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_2_S1_INDIRECT_2, "xor.2-d-indirect-2-s1-indirect-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 (${d-An})${d-i4-2}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_2, "xor.2-d-indirect-with-post-increment-2-s1-indirect-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 ${d-i4-2}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_2, "xor.2-d-indirect-with-pre-increment-2-s1-indirect-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 ${d-direct-addr},(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_XOR_2_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_2, "xor.2-d-direct-s1-indirect-with-post-increment-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 #${d-imm8},(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_XOR_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "xor.2-d-immediate-2-s1-indirect-with-post-increment-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 (${d-An},${d-r}),(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "xor.2-d-indirect-with-index-2-s1-indirect-with-post-increment-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 ${d-imm7-2}(${d-An}),(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "xor.2-d-indirect-with-offset-2-s1-indirect-with-post-increment-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 (${d-An}),(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "xor.2-d-indirect-2-s1-indirect-with-post-increment-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 (${d-An})${d-i4-2}++,(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "xor.2-d-indirect-with-post-increment-2-s1-indirect-with-post-increment-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 ${d-i4-2}(${d-An})++,(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "xor.2-d-indirect-with-pre-increment-2-s1-indirect-with-post-increment-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 ${d-direct-addr},${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_XOR_2_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_2, "xor.2-d-direct-s1-indirect-with-pre-increment-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 #${d-imm8},${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_XOR_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "xor.2-d-immediate-2-s1-indirect-with-pre-increment-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 (${d-An},${d-r}),${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "xor.2-d-indirect-with-index-2-s1-indirect-with-pre-increment-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 ${d-imm7-2}(${d-An}),${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "xor.2-d-indirect-with-offset-2-s1-indirect-with-pre-increment-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 (${d-An}),${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "xor.2-d-indirect-2-s1-indirect-with-pre-increment-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 (${d-An})${d-i4-2}++,${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "xor.2-d-indirect-with-post-increment-2-s1-indirect-with-pre-increment-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* xor.2 ${d-i4-2}(${d-An})++,${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_XOR_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "xor.2-d-indirect-with-pre-increment-2-s1-indirect-with-pre-increment-2", "xor.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_OR_4_D_DIRECT_S1_DIRECT, "or.4-d-direct-s1-direct", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_OR_4_D_IMMEDIATE_4_S1_DIRECT, "or.4-d-immediate-4-s1-direct", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_INDEX_4_S1_DIRECT, "or.4-d-indirect-with-index-4-s1-direct", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 ${d-imm7-4}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT, "or.4-d-indirect-with-offset-4-s1-direct", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_4_S1_DIRECT, "or.4-d-indirect-4-s1-direct", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 (${d-An})${d-i4-4}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_DIRECT, "or.4-d-indirect-with-post-increment-4-s1-direct", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 ${d-i4-4}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT, "or.4-d-indirect-with-pre-increment-4-s1-direct", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_OR_4_D_DIRECT_S1_IMMEDIATE, "or.4-d-direct-s1-immediate", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_OR_4_D_IMMEDIATE_4_S1_IMMEDIATE, "or.4-d-immediate-4-s1-immediate", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_INDEX_4_S1_IMMEDIATE, "or.4-d-indirect-with-index-4-s1-immediate", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 ${d-imm7-4}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_OFFSET_4_S1_IMMEDIATE, "or.4-d-indirect-with-offset-4-s1-immediate", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_4_S1_IMMEDIATE, "or.4-d-indirect-4-s1-immediate", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 (${d-An})${d-i4-4}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_IMMEDIATE, "or.4-d-indirect-with-post-increment-4-s1-immediate", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 ${d-i4-4}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_IMMEDIATE, "or.4-d-indirect-with-pre-increment-4-s1-immediate", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_OR_4_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "or.4-d-direct-s1-indirect-with-index-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_OR_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_INDEX_4, "or.4-d-immediate-4-s1-indirect-with-index-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_INDEX_4, "or.4-d-indirect-with-index-4-s1-indirect-with-index-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_INDEX_4, "or.4-d-indirect-with-offset-4-s1-indirect-with-index-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_4_S1_INDIRECT_WITH_INDEX_4, "or.4-d-indirect-4-s1-indirect-with-index-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 (${d-An})${d-i4-4}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "or.4-d-indirect-with-post-increment-4-s1-indirect-with-index-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "or.4-d-indirect-with-pre-increment-4-s1-indirect-with-index-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 ${d-direct-addr},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_4_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "or.4-d-direct-s1-indirect-with-offset-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 #${d-imm8},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_OFFSET_4, "or.4-d-immediate-4-s1-indirect-with-offset-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 (${d-An},${d-r}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_OFFSET_4, "or.4-d-indirect-with-index-4-s1-indirect-with-offset-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_OFFSET_4, "or.4-d-indirect-with-offset-4-s1-indirect-with-offset-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 (${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_4_S1_INDIRECT_WITH_OFFSET_4, "or.4-d-indirect-4-s1-indirect-with-offset-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "or.4-d-indirect-with-post-increment-4-s1-indirect-with-offset-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "or.4-d-indirect-with-pre-increment-4-s1-indirect-with-offset-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_4_D_DIRECT_S1_INDIRECT_4, "or.4-d-direct-s1-indirect-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_4_D_IMMEDIATE_4_S1_INDIRECT_4, "or.4-d-immediate-4-s1-indirect-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_4, "or.4-d-indirect-with-index-4-s1-indirect-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 ${d-imm7-4}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_4, "or.4-d-indirect-with-offset-4-s1-indirect-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_4_S1_INDIRECT_4, "or.4-d-indirect-4-s1-indirect-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 (${d-An})${d-i4-4}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_4, "or.4-d-indirect-with-post-increment-4-s1-indirect-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 ${d-i4-4}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_4, "or.4-d-indirect-with-pre-increment-4-s1-indirect-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 ${d-direct-addr},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_OR_4_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "or.4-d-direct-s1-indirect-with-post-increment-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 #${d-imm8},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_OR_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "or.4-d-immediate-4-s1-indirect-with-post-increment-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 (${d-An},${d-r}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "or.4-d-indirect-with-index-4-s1-indirect-with-post-increment-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "or.4-d-indirect-with-offset-4-s1-indirect-with-post-increment-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 (${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "or.4-d-indirect-4-s1-indirect-with-post-increment-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "or.4-d-indirect-with-post-increment-4-s1-indirect-with-post-increment-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "or.4-d-indirect-with-pre-increment-4-s1-indirect-with-post-increment-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 ${d-direct-addr},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_OR_4_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "or.4-d-direct-s1-indirect-with-pre-increment-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 #${d-imm8},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_OR_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "or.4-d-immediate-4-s1-indirect-with-pre-increment-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 (${d-An},${d-r}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "or.4-d-indirect-with-index-4-s1-indirect-with-pre-increment-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "or.4-d-indirect-with-offset-4-s1-indirect-with-pre-increment-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 (${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "or.4-d-indirect-4-s1-indirect-with-pre-increment-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "or.4-d-indirect-with-post-increment-4-s1-indirect-with-pre-increment-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.4 ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_OR_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "or.4-d-indirect-with-pre-increment-4-s1-indirect-with-pre-increment-4", "or.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_OR_2_D_DIRECT_S1_DIRECT, "or.2-d-direct-s1-direct", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_OR_2_D_IMMEDIATE_2_S1_DIRECT, "or.2-d-immediate-2-s1-direct", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_INDEX_2_S1_DIRECT, "or.2-d-indirect-with-index-2-s1-direct", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 ${d-imm7-2}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_OFFSET_2_S1_DIRECT, "or.2-d-indirect-with-offset-2-s1-direct", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_2_S1_DIRECT, "or.2-d-indirect-2-s1-direct", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 (${d-An})${d-i4-2}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_DIRECT, "or.2-d-indirect-with-post-increment-2-s1-direct", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 ${d-i4-2}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_DIRECT, "or.2-d-indirect-with-pre-increment-2-s1-direct", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_OR_2_D_DIRECT_S1_IMMEDIATE, "or.2-d-direct-s1-immediate", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_OR_2_D_IMMEDIATE_2_S1_IMMEDIATE, "or.2-d-immediate-2-s1-immediate", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_INDEX_2_S1_IMMEDIATE, "or.2-d-indirect-with-index-2-s1-immediate", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 ${d-imm7-2}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_OFFSET_2_S1_IMMEDIATE, "or.2-d-indirect-with-offset-2-s1-immediate", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_2_S1_IMMEDIATE, "or.2-d-indirect-2-s1-immediate", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 (${d-An})${d-i4-2}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_IMMEDIATE, "or.2-d-indirect-with-post-increment-2-s1-immediate", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 ${d-i4-2}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_IMMEDIATE, "or.2-d-indirect-with-pre-increment-2-s1-immediate", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_OR_2_D_DIRECT_S1_INDIRECT_WITH_INDEX_2, "or.2-d-direct-s1-indirect-with-index-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_OR_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_INDEX_2, "or.2-d-immediate-2-s1-indirect-with-index-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_INDEX_2, "or.2-d-indirect-with-index-2-s1-indirect-with-index-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 ${d-imm7-2}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_INDEX_2, "or.2-d-indirect-with-offset-2-s1-indirect-with-index-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_2_S1_INDIRECT_WITH_INDEX_2, "or.2-d-indirect-2-s1-indirect-with-index-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 (${d-An})${d-i4-2}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_INDEX_2, "or.2-d-indirect-with-post-increment-2-s1-indirect-with-index-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 ${d-i4-2}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_INDEX_2, "or.2-d-indirect-with-pre-increment-2-s1-indirect-with-index-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 ${d-direct-addr},${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_2_D_DIRECT_S1_INDIRECT_WITH_OFFSET_2, "or.2-d-direct-s1-indirect-with-offset-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 #${d-imm8},${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_OFFSET_2, "or.2-d-immediate-2-s1-indirect-with-offset-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 (${d-An},${d-r}),${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_OFFSET_2, "or.2-d-indirect-with-index-2-s1-indirect-with-offset-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 ${d-imm7-2}(${d-An}),${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_OFFSET_2, "or.2-d-indirect-with-offset-2-s1-indirect-with-offset-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 (${d-An}),${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_2_S1_INDIRECT_WITH_OFFSET_2, "or.2-d-indirect-2-s1-indirect-with-offset-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 (${d-An})${d-i4-2}++,${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_2, "or.2-d-indirect-with-post-increment-2-s1-indirect-with-offset-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 ${d-i4-2}(${d-An})++,${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_2, "or.2-d-indirect-with-pre-increment-2-s1-indirect-with-offset-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_2_D_DIRECT_S1_INDIRECT_2, "or.2-d-direct-s1-indirect-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_2_D_IMMEDIATE_2_S1_INDIRECT_2, "or.2-d-immediate-2-s1-indirect-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_2, "or.2-d-indirect-with-index-2-s1-indirect-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 ${d-imm7-2}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_2, "or.2-d-indirect-with-offset-2-s1-indirect-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_2_S1_INDIRECT_2, "or.2-d-indirect-2-s1-indirect-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 (${d-An})${d-i4-2}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_2, "or.2-d-indirect-with-post-increment-2-s1-indirect-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 ${d-i4-2}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_2, "or.2-d-indirect-with-pre-increment-2-s1-indirect-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 ${d-direct-addr},(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_OR_2_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_2, "or.2-d-direct-s1-indirect-with-post-increment-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 #${d-imm8},(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_OR_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "or.2-d-immediate-2-s1-indirect-with-post-increment-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 (${d-An},${d-r}),(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "or.2-d-indirect-with-index-2-s1-indirect-with-post-increment-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 ${d-imm7-2}(${d-An}),(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "or.2-d-indirect-with-offset-2-s1-indirect-with-post-increment-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 (${d-An}),(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "or.2-d-indirect-2-s1-indirect-with-post-increment-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 (${d-An})${d-i4-2}++,(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "or.2-d-indirect-with-post-increment-2-s1-indirect-with-post-increment-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 ${d-i4-2}(${d-An})++,(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "or.2-d-indirect-with-pre-increment-2-s1-indirect-with-post-increment-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 ${d-direct-addr},${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_OR_2_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_2, "or.2-d-direct-s1-indirect-with-pre-increment-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 #${d-imm8},${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_OR_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "or.2-d-immediate-2-s1-indirect-with-pre-increment-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 (${d-An},${d-r}),${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "or.2-d-indirect-with-index-2-s1-indirect-with-pre-increment-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 ${d-imm7-2}(${d-An}),${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "or.2-d-indirect-with-offset-2-s1-indirect-with-pre-increment-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 (${d-An}),${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "or.2-d-indirect-2-s1-indirect-with-pre-increment-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 (${d-An})${d-i4-2}++,${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "or.2-d-indirect-with-post-increment-2-s1-indirect-with-pre-increment-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* or.2 ${d-i4-2}(${d-An})++,${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_OR_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "or.2-d-indirect-with-pre-increment-2-s1-indirect-with-pre-increment-2", "or.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_AND_4_D_DIRECT_S1_DIRECT, "and.4-d-direct-s1-direct", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_AND_4_D_IMMEDIATE_4_S1_DIRECT, "and.4-d-immediate-4-s1-direct", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_INDEX_4_S1_DIRECT, "and.4-d-indirect-with-index-4-s1-direct", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 ${d-imm7-4}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT, "and.4-d-indirect-with-offset-4-s1-direct", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_4_S1_DIRECT, "and.4-d-indirect-4-s1-direct", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 (${d-An})${d-i4-4}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_DIRECT, "and.4-d-indirect-with-post-increment-4-s1-direct", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 ${d-i4-4}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT, "and.4-d-indirect-with-pre-increment-4-s1-direct", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_AND_4_D_DIRECT_S1_IMMEDIATE, "and.4-d-direct-s1-immediate", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_AND_4_D_IMMEDIATE_4_S1_IMMEDIATE, "and.4-d-immediate-4-s1-immediate", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_INDEX_4_S1_IMMEDIATE, "and.4-d-indirect-with-index-4-s1-immediate", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 ${d-imm7-4}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_OFFSET_4_S1_IMMEDIATE, "and.4-d-indirect-with-offset-4-s1-immediate", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_4_S1_IMMEDIATE, "and.4-d-indirect-4-s1-immediate", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 (${d-An})${d-i4-4}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_IMMEDIATE, "and.4-d-indirect-with-post-increment-4-s1-immediate", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 ${d-i4-4}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_IMMEDIATE, "and.4-d-indirect-with-pre-increment-4-s1-immediate", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_AND_4_D_DIRECT_S1_INDIRECT_WITH_INDEX_4, "and.4-d-direct-s1-indirect-with-index-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_AND_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_INDEX_4, "and.4-d-immediate-4-s1-indirect-with-index-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_INDEX_4, "and.4-d-indirect-with-index-4-s1-indirect-with-index-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 ${d-imm7-4}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_INDEX_4, "and.4-d-indirect-with-offset-4-s1-indirect-with-index-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_4_S1_INDIRECT_WITH_INDEX_4, "and.4-d-indirect-4-s1-indirect-with-index-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 (${d-An})${d-i4-4}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "and.4-d-indirect-with-post-increment-4-s1-indirect-with-index-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 ${d-i4-4}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_INDEX_4, "and.4-d-indirect-with-pre-increment-4-s1-indirect-with-index-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 ${d-direct-addr},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_4_D_DIRECT_S1_INDIRECT_WITH_OFFSET_4, "and.4-d-direct-s1-indirect-with-offset-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 #${d-imm8},${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_OFFSET_4, "and.4-d-immediate-4-s1-indirect-with-offset-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 (${d-An},${d-r}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_OFFSET_4, "and.4-d-indirect-with-index-4-s1-indirect-with-offset-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 ${d-imm7-4}(${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_OFFSET_4, "and.4-d-indirect-with-offset-4-s1-indirect-with-offset-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 (${d-An}),${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_4_S1_INDIRECT_WITH_OFFSET_4, "and.4-d-indirect-4-s1-indirect-with-offset-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 (${d-An})${d-i4-4}++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "and.4-d-indirect-with-post-increment-4-s1-indirect-with-offset-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 ${d-i4-4}(${d-An})++,${s1-imm7-4}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_OFFSET_4, "and.4-d-indirect-with-pre-increment-4-s1-indirect-with-offset-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_4_D_DIRECT_S1_INDIRECT_4, "and.4-d-direct-s1-indirect-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_4_D_IMMEDIATE_4_S1_INDIRECT_4, "and.4-d-immediate-4-s1-indirect-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_4, "and.4-d-indirect-with-index-4-s1-indirect-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 ${d-imm7-4}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_4, "and.4-d-indirect-with-offset-4-s1-indirect-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_4_S1_INDIRECT_4, "and.4-d-indirect-4-s1-indirect-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 (${d-An})${d-i4-4}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_4, "and.4-d-indirect-with-post-increment-4-s1-indirect-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 ${d-i4-4}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_4, "and.4-d-indirect-with-pre-increment-4-s1-indirect-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 ${d-direct-addr},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_AND_4_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_4, "and.4-d-direct-s1-indirect-with-post-increment-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 #${d-imm8},(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_AND_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "and.4-d-immediate-4-s1-indirect-with-post-increment-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 (${d-An},${d-r}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "and.4-d-indirect-with-index-4-s1-indirect-with-post-increment-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 ${d-imm7-4}(${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "and.4-d-indirect-with-offset-4-s1-indirect-with-post-increment-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 (${d-An}),(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "and.4-d-indirect-4-s1-indirect-with-post-increment-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 (${d-An})${d-i4-4}++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "and.4-d-indirect-with-post-increment-4-s1-indirect-with-post-increment-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 ${d-i4-4}(${d-An})++,(${s1-An})${s1-i4-4}++,${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_POST_INCREMENT_4, "and.4-d-indirect-with-pre-increment-4-s1-indirect-with-post-increment-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 ${d-direct-addr},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_AND_4_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_4, "and.4-d-direct-s1-indirect-with-pre-increment-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 #${d-imm8},${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_AND_4_D_IMMEDIATE_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "and.4-d-immediate-4-s1-indirect-with-pre-increment-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 (${d-An},${d-r}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_INDEX_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "and.4-d-indirect-with-index-4-s1-indirect-with-pre-increment-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 ${d-imm7-4}(${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_OFFSET_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "and.4-d-indirect-with-offset-4-s1-indirect-with-pre-increment-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 (${d-An}),${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "and.4-d-indirect-4-s1-indirect-with-pre-increment-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 (${d-An})${d-i4-4}++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_POST_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "and.4-d-indirect-with-post-increment-4-s1-indirect-with-pre-increment-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.4 ${d-i4-4}(${d-An})++,${s1-i4-4}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_AND_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_INDIRECT_WITH_PRE_INCREMENT_4, "and.4-d-indirect-with-pre-increment-4-s1-indirect-with-pre-increment-4", "and.4", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 ${d-direct-addr},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_AND_2_D_DIRECT_S1_DIRECT, "and.2-d-direct-s1-direct", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 #${d-imm8},${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_AND_2_D_IMMEDIATE_2_S1_DIRECT, "and.2-d-immediate-2-s1-direct", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 (${d-An},${d-r}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_INDEX_2_S1_DIRECT, "and.2-d-indirect-with-index-2-s1-direct", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 ${d-imm7-2}(${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_OFFSET_2_S1_DIRECT, "and.2-d-indirect-with-offset-2-s1-direct", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 (${d-An}),${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_2_S1_DIRECT, "and.2-d-indirect-2-s1-direct", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 (${d-An})${d-i4-2}++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_DIRECT, "and.2-d-indirect-with-post-increment-2-s1-direct", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 ${d-i4-2}(${d-An})++,${s1-direct-addr},${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_DIRECT, "and.2-d-indirect-with-pre-increment-2-s1-direct", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 ${d-direct-addr},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_AND_2_D_DIRECT_S1_IMMEDIATE, "and.2-d-direct-s1-immediate", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 #${d-imm8},#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_AND_2_D_IMMEDIATE_2_S1_IMMEDIATE, "and.2-d-immediate-2-s1-immediate", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 (${d-An},${d-r}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_INDEX_2_S1_IMMEDIATE, "and.2-d-indirect-with-index-2-s1-immediate", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 ${d-imm7-2}(${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_OFFSET_2_S1_IMMEDIATE, "and.2-d-indirect-with-offset-2-s1-immediate", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 (${d-An}),#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_2_S1_IMMEDIATE, "and.2-d-indirect-2-s1-immediate", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 (${d-An})${d-i4-2}++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_IMMEDIATE, "and.2-d-indirect-with-post-increment-2-s1-immediate", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 ${d-i4-2}(${d-An})++,#${s1-imm8},${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_IMMEDIATE, "and.2-d-indirect-with-pre-increment-2-s1-immediate", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 ${d-direct-addr},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_AND_2_D_DIRECT_S1_INDIRECT_WITH_INDEX_2, "and.2-d-direct-s1-indirect-with-index-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 #${d-imm8},(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_AND_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_INDEX_2, "and.2-d-immediate-2-s1-indirect-with-index-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 (${d-An},${d-r}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_INDEX_2, "and.2-d-indirect-with-index-2-s1-indirect-with-index-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 ${d-imm7-2}(${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_INDEX_2, "and.2-d-indirect-with-offset-2-s1-indirect-with-index-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 (${d-An}),(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_2_S1_INDIRECT_WITH_INDEX_2, "and.2-d-indirect-2-s1-indirect-with-index-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 (${d-An})${d-i4-2}++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_INDEX_2, "and.2-d-indirect-with-post-increment-2-s1-indirect-with-index-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 ${d-i4-2}(${d-An})++,(${s1-An},${s1-r}),${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_INDEX_2, "and.2-d-indirect-with-pre-increment-2-s1-indirect-with-index-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 ${d-direct-addr},${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_2_D_DIRECT_S1_INDIRECT_WITH_OFFSET_2, "and.2-d-direct-s1-indirect-with-offset-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 #${d-imm8},${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_OFFSET_2, "and.2-d-immediate-2-s1-indirect-with-offset-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 (${d-An},${d-r}),${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_OFFSET_2, "and.2-d-indirect-with-index-2-s1-indirect-with-offset-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 ${d-imm7-2}(${d-An}),${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_OFFSET_2, "and.2-d-indirect-with-offset-2-s1-indirect-with-offset-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 (${d-An}),${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_2_S1_INDIRECT_WITH_OFFSET_2, "and.2-d-indirect-2-s1-indirect-with-offset-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 (${d-An})${d-i4-2}++,${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_2, "and.2-d-indirect-with-post-increment-2-s1-indirect-with-offset-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 ${d-i4-2}(${d-An})++,${s1-imm7-2}(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_OFFSET_2, "and.2-d-indirect-with-pre-increment-2-s1-indirect-with-offset-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 ${d-direct-addr},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_2_D_DIRECT_S1_INDIRECT_2, "and.2-d-direct-s1-indirect-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 #${d-imm8},(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_2_D_IMMEDIATE_2_S1_INDIRECT_2, "and.2-d-immediate-2-s1-indirect-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 (${d-An},${d-r}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_2, "and.2-d-indirect-with-index-2-s1-indirect-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 ${d-imm7-2}(${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_2, "and.2-d-indirect-with-offset-2-s1-indirect-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 (${d-An}),(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_2_S1_INDIRECT_2, "and.2-d-indirect-2-s1-indirect-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 (${d-An})${d-i4-2}++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_2, "and.2-d-indirect-with-post-increment-2-s1-indirect-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 ${d-i4-2}(${d-An})++,(${s1-An}),${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_2, "and.2-d-indirect-with-pre-increment-2-s1-indirect-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 ${d-direct-addr},(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_AND_2_D_DIRECT_S1_INDIRECT_WITH_POST_INCREMENT_2, "and.2-d-direct-s1-indirect-with-post-increment-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 #${d-imm8},(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_AND_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "and.2-d-immediate-2-s1-indirect-with-post-increment-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 (${d-An},${d-r}),(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "and.2-d-indirect-with-index-2-s1-indirect-with-post-increment-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 ${d-imm7-2}(${d-An}),(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "and.2-d-indirect-with-offset-2-s1-indirect-with-post-increment-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 (${d-An}),(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "and.2-d-indirect-2-s1-indirect-with-post-increment-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 (${d-An})${d-i4-2}++,(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "and.2-d-indirect-with-post-increment-2-s1-indirect-with-post-increment-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 ${d-i4-2}(${d-An})++,(${s1-An})${s1-i4-2}++,${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_POST_INCREMENT_2, "and.2-d-indirect-with-pre-increment-2-s1-indirect-with-post-increment-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 ${d-direct-addr},${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_AND_2_D_DIRECT_S1_INDIRECT_WITH_PRE_INCREMENT_2, "and.2-d-direct-s1-indirect-with-pre-increment-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 #${d-imm8},${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_AND_2_D_IMMEDIATE_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "and.2-d-immediate-2-s1-indirect-with-pre-increment-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 (${d-An},${d-r}),${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_INDEX_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "and.2-d-indirect-with-index-2-s1-indirect-with-pre-increment-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 ${d-imm7-2}(${d-An}),${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_OFFSET_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "and.2-d-indirect-with-offset-2-s1-indirect-with-pre-increment-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 (${d-An}),${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "and.2-d-indirect-2-s1-indirect-with-pre-increment-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 (${d-An})${d-i4-2}++,${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_POST_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "and.2-d-indirect-with-post-increment-2-s1-indirect-with-pre-increment-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* and.2 ${d-i4-2}(${d-An})++,${s1-i4-2}(${s1-An})++,${s2} */
  {
    UBICOM32_INSN_AND_2_D_INDIRECT_WITH_PRE_INCREMENT_2_S1_INDIRECT_WITH_PRE_INCREMENT_2, "and.2-d-indirect-with-pre-increment-2-s1-indirect-with-pre-increment-2", "and.2", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* leai ${An},${leai-offset16}(${Am}) */
  {
    UBICOM32_INSN_LEAI, "leai", "leai", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* moveai ${An},#${imm25} */
  {
    UBICOM32_INSN_MOVEAI_V5, "moveai_v5", "moveai", 32,
    { 0, { { { (1<<MACH_UBICOM32_NEWMOVEAI), 0 } } } }
  },
/* moveaih ${An},#${imm24} */
  {
    UBICOM32_INSN_MOVEAIH, "moveaih", "moveaih", 32,
    { 0, { { { (1<<MACH_UBICOM32_NEWMOVEAISIMULATE), 0 } } } }
  },
/* moveai ${An},#${imm24} */
  {
    UBICOM32_INSN_MOVEAI, "moveai", "moveai", 32,
    { 0, { { { (1<<MACH_UBICOM32_OLDMOVEAI), 0 } } } }
  },
/* __nop__ */
  {
    UBICOM32_INSN_NOP_INSN, "nop-insn", "__nop__", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* jmp${cc}${C}${P} $offset21 */
  {
    UBICOM32_INSN_JMPCC, "jmpcc", "jmp", 32,
    { 0|A(COND_CTI), { { { (1<<MACH_BASE), 0 } } } }
  },
/* call $An,$offset24 */
  {
    UBICOM32_INSN_CALL, "call", "call", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_BASE), 0 } } } }
  },
/* calli ${An},${offset16}(${Am}) */
  {
    UBICOM32_INSN_CALLI, "calli", "calli", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_BASE), 0 } } } }
  },
/* calli.b ${An},${offset16}(${Am}) */
  {
    UBICOM32_INSN_CALLI_B, "calli_b", "calli.b", 32,
    { 0|A(UNCOND_CTI), { { { (1<<MACH_BASE), 0 } } } }
  },
/* suspend */
  {
    UBICOM32_INSN_SUSPEND, "suspend", "suspend", 32,
    { 0, { { { (1<<MACH_BASE), 0 } } } }
  },
/* __clracc__ ${dsp-destA} */
  {
    UBICOM32_INSN_DSP_CLRACC, "dsp-clracc", "__clracc__", 32,
    { 0, { { { (1<<MACH_UBICOM32DSP)|(1<<MACH_UBICOM32_VER4), 0 } } } }
  },
/* faddd ${fpu-64-dest-reg},${fpu-64-S1-data-reg},${fpu-64-S2-data-reg} */
  {
    UBICOM32_INSN_FADDD_DATA_DATA, "faddd_data_data", "faddd", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* faddd ${fpu-64-dest-reg},${fpu-64-S1-data-reg},${fpu-64-S2-acc-reg} */
  {
    UBICOM32_INSN_FADDD_DATA_ACC, "faddd_data_acc", "faddd", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* faddd ${fpu-64-dest-reg},${fpu-64-S1-acc-reg},${fpu-64-S2-data-reg} */
  {
    UBICOM32_INSN_FADDD_ACC_DATA, "faddd_acc_data", "faddd", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* faddd ${fpu-64-dest-reg},${fpu-64-S1-acc-reg},${fpu-64-S2-acc-reg} */
  {
    UBICOM32_INSN_FADDD_ACC_ACC, "faddd_acc_acc", "faddd", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsubd ${fpu-64-dest-reg},${fpu-64-S1-data-reg},${fpu-64-S2-data-reg} */
  {
    UBICOM32_INSN_FSUBD_DATA_DATA, "fsubd_data_data", "fsubd", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsubd ${fpu-64-dest-reg},${fpu-64-S1-data-reg},${fpu-64-S2-acc-reg} */
  {
    UBICOM32_INSN_FSUBD_DATA_ACC, "fsubd_data_acc", "fsubd", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsubd ${fpu-64-dest-reg},${fpu-64-S1-acc-reg},${fpu-64-S2-data-reg} */
  {
    UBICOM32_INSN_FSUBD_ACC_DATA, "fsubd_acc_data", "fsubd", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsubd ${fpu-64-dest-reg},${fpu-64-S1-acc-reg},${fpu-64-S2-acc-reg} */
  {
    UBICOM32_INSN_FSUBD_ACC_ACC, "fsubd_acc_acc", "fsubd", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fmuld ${fpu-64-dest-reg},${fpu-64-S1-data-reg},${fpu-64-S2-data-reg} */
  {
    UBICOM32_INSN_FMULD_DATA_DATA, "fmuld_data_data", "fmuld", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fmuld ${fpu-64-dest-reg},${fpu-64-S1-data-reg},${fpu-64-S2-acc-reg} */
  {
    UBICOM32_INSN_FMULD_DATA_ACC, "fmuld_data_acc", "fmuld", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fmuld ${fpu-64-dest-reg},${fpu-64-S1-acc-reg},${fpu-64-S2-data-reg} */
  {
    UBICOM32_INSN_FMULD_ACC_DATA, "fmuld_acc_data", "fmuld", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fmuld ${fpu-64-dest-reg},${fpu-64-S1-acc-reg},${fpu-64-S2-acc-reg} */
  {
    UBICOM32_INSN_FMULD_ACC_ACC, "fmuld_acc_acc", "fmuld", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fdivd ${fpu-64-dest-reg},${fpu-64-S1-data-reg},${fpu-64-S2-data-reg} */
  {
    UBICOM32_INSN_FDIVD_DATA_DATA, "fdivd_data_data", "fdivd", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fdivd ${fpu-64-dest-reg},${fpu-64-S1-data-reg},${fpu-64-S2-acc-reg} */
  {
    UBICOM32_INSN_FDIVD_DATA_ACC, "fdivd_data_acc", "fdivd", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fdivd ${fpu-64-dest-reg},${fpu-64-S1-acc-reg},${fpu-64-S2-data-reg} */
  {
    UBICOM32_INSN_FDIVD_ACC_DATA, "fdivd_acc_data", "fdivd", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fdivd ${fpu-64-dest-reg},${fpu-64-S1-acc-reg},${fpu-64-S2-acc-reg} */
  {
    UBICOM32_INSN_FDIVD_ACC_ACC, "fdivd_acc_acc", "fdivd", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsqrtd ${fpu-64-dest-reg},${fpu-64-S1-data-reg} */
  {
    UBICOM32_INSN_FSQRTD_DATA, "fsqrtd_data", "fsqrtd", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fsqrtd ${fpu-64-dest-reg},${fpu-64-S1-acc-reg} */
  {
    UBICOM32_INSN_FSQRTD_ACC, "fsqrtd_acc", "fsqrtd", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fnegd ${fpu-64-dest-reg},${fpu-64-S1-data-reg} */
  {
    UBICOM32_INSN_FNEGD_DATA, "fnegd_data", "fnegd", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fnegd ${fpu-64-dest-reg},${fpu-64-S1-acc-reg} */
  {
    UBICOM32_INSN_FNEGD_ACC, "fnegd_acc", "fnegd", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fabsd ${fpu-64-dest-reg},${fpu-64-S1-data-reg} */
  {
    UBICOM32_INSN_FABSD_DATA, "fabsd_data", "fabsd", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fabsd ${fpu-64-dest-reg},${fpu-64-S1-acc-reg} */
  {
    UBICOM32_INSN_FABSD_ACC, "fabsd_acc", "fabsd", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fl2s ${fpu-32-dest-reg},${fpu-64-S1-data-reg} */
  {
    UBICOM32_INSN_FL2S_DATA, "fl2s_data", "fl2s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fl2s ${fpu-32-dest-reg},${fpu-64-S1-acc-reg} */
  {
    UBICOM32_INSN_FL2S_ACC, "fl2s_acc", "fl2s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fl2d ${fpu-64-dest-reg},${fpu-64-S1-data-reg} */
  {
    UBICOM32_INSN_FL2D_DATA, "fl2d_data", "fl2d", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fl2d ${fpu-64-dest-reg},${fpu-64-S1-acc-reg} */
  {
    UBICOM32_INSN_FL2D_ACC, "fl2d_acc", "fl2d", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fd2s ${fpu-32-dest-reg},${fpu-64-S1-data-reg} */
  {
    UBICOM32_INSN_FD2S_DATA, "fd2s_data", "fd2s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fd2s ${fpu-32-dest-reg},${fpu-64-S1-acc-reg} */
  {
    UBICOM32_INSN_FD2S_ACC, "fd2s_acc", "fd2s", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fd2i ${fpu-32-dest-reg},${fpu-64-S1-data-reg} */
  {
    UBICOM32_INSN_FD2I_DATA, "fd2i_data", "fd2i", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fd2i ${fpu-32-dest-reg},${fpu-64-S1-acc-reg} */
  {
    UBICOM32_INSN_FD2I_ACC, "fd2i_acc", "fd2i", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fd2l ${fpu-64-dest-reg},${fpu-64-S1-data-reg} */
  {
    UBICOM32_INSN_FD2L_DATA, "fd2l_data", "fd2l", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fd2l ${fpu-64-dest-reg},${fpu-64-S1-acc-reg} */
  {
    UBICOM32_INSN_FD2L_ACC, "fd2l_acc", "fd2l", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fcmpd ${fpu-64-S1-data-reg},${fpu-64-S2-data-reg} */
  {
    UBICOM32_INSN_FCMPD_DATA_DATA, "fcmpd_data_data", "fcmpd", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fcmpd ${fpu-64-S1-data-reg},${fpu-64-S2-acc-reg} */
  {
    UBICOM32_INSN_FCMPD_DATA_ACC, "fcmpd_data_acc", "fcmpd", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fcmpd ${fpu-64-S1-acc-reg},${fpu-64-S2-acc-reg} */
  {
    UBICOM32_INSN_FCMPD_ACC_ACC, "fcmpd_acc_acc", "fcmpd", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* fcmpd ${fpu-64-S1-acc-reg},${fpu-64-S2-data-reg} */
  {
    UBICOM32_INSN_FCMPD_ACC_DATA, "fcmpd_acc_data", "fcmpd", 32,
    { 0, { { { (1<<MACH_UBICOM32_VER5), 0 } } } }
  },
/* __unused__02_04 */
  {
    UBICOM32_INSN_UNUSED_02_04, "unused.02_04", "__unused__02_04", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__02_07 */
  {
    UBICOM32_INSN_UNUSED_02_07, "unused.02_07", "__unused__02_07", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__02_0D */
  {
    UBICOM32_INSN_UNUSED_02_0D, "unused.02_0D", "__unused__02_0D", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__02_0E */
  {
    UBICOM32_INSN_UNUSED_02_0E, "unused.02_0E", "__unused__02_0E", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__02_0F */
  {
    UBICOM32_INSN_UNUSED_02_0F, "unused.02_0F", "__unused__02_0F", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__02_17 */
  {
    UBICOM32_INSN_UNUSED_02_17, "unused.02_17", "__unused__02_17", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__02_19 */
  {
    UBICOM32_INSN_UNUSED_02_19, "unused.02_19", "__unused__02_19", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__02_1B */
  {
    UBICOM32_INSN_UNUSED_02_1B, "unused.02_1B", "__unused__02_1B", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__02_1D */
  {
    UBICOM32_INSN_UNUSED_02_1D, "unused.02_1D", "__unused__02_1D", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__01 */
  {
    UBICOM32_INSN_UNUSED_01, "unused.01", "__unused__01", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__07 */
  {
    UBICOM32_INSN_UNUSED_07, "unused.07", "__unused__07", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__17 */
  {
    UBICOM32_INSN_UNUSED_17, "unused.17", "__unused__17", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__1F */
  {
    UBICOM32_INSN_UNUSED_1F, "unused.1F", "__unused__1F", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__DSP_0c */
  {
    UBICOM32_INSN_UNUSED_DSP_0C, "unused.DSP_0c", "__unused__DSP_0c", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__DSP_0d */
  {
    UBICOM32_INSN_UNUSED_DSP_0D, "unused.DSP_0d", "__unused__DSP_0d", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__DSP_0e */
  {
    UBICOM32_INSN_UNUSED_DSP_0E, "unused.DSP_0e", "__unused__DSP_0e", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__DSP_0f */
  {
    UBICOM32_INSN_UNUSED_DSP_0F, "unused.DSP_0f", "__unused__DSP_0f", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__DSP_14 */
  {
    UBICOM32_INSN_UNUSED_DSP_14, "unused.DSP_14", "__unused__DSP_14", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__DSP_15 */
  {
    UBICOM32_INSN_UNUSED_DSP_15, "unused.DSP_15", "__unused__DSP_15", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__DSP_16 */
  {
    UBICOM32_INSN_UNUSED_DSP_16, "unused.DSP_16", "__unused__DSP_16", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__DSP_17 */
  {
    UBICOM32_INSN_UNUSED_DSP_17, "unused.DSP_17", "__unused__DSP_17", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__DSP_18 */
  {
    UBICOM32_INSN_UNUSED_DSP_18, "unused.DSP_18", "__unused__DSP_18", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__DSP_19 */
  {
    UBICOM32_INSN_UNUSED_DSP_19, "unused.DSP_19", "__unused__DSP_19", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__DSP_1a */
  {
    UBICOM32_INSN_UNUSED_DSP_1A, "unused.DSP_1a", "__unused__DSP_1a", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__DSP_1b */
  {
    UBICOM32_INSN_UNUSED_DSP_1B, "unused.DSP_1b", "__unused__DSP_1b", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__DSP_1c */
  {
    UBICOM32_INSN_UNUSED_DSP_1C, "unused.DSP_1c", "__unused__DSP_1c", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__DSP_1d */
  {
    UBICOM32_INSN_UNUSED_DSP_1D, "unused.DSP_1d", "__unused__DSP_1d", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__DSP_1e */
  {
    UBICOM32_INSN_UNUSED_DSP_1E, "unused.DSP_1e", "__unused__DSP_1e", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
/* __unused__DSP_1f */
  {
    UBICOM32_INSN_UNUSED_DSP_1F, "unused.DSP_1f", "__unused__DSP_1f", 32,
    { 0|A(NO_DIS), { { { (1<<MACH_BASE), 0 } } } }
  },
};

#undef OP
#undef A

/* Initialize anything needed to be done once, before any cpu_open call.  */

static void
init_tables (void)
{
}

static const CGEN_MACH * lookup_mach_via_bfd_name (const CGEN_MACH *, const char *);
static void build_hw_table      (CGEN_CPU_TABLE *);
static void build_ifield_table  (CGEN_CPU_TABLE *);
static void build_operand_table (CGEN_CPU_TABLE *);
static void build_insn_table    (CGEN_CPU_TABLE *);
static void ubicom32_cgen_rebuild_tables (CGEN_CPU_TABLE *);

/* Subroutine of ubicom32_cgen_cpu_open to look up a mach via its bfd name.  */

static const CGEN_MACH *
lookup_mach_via_bfd_name (const CGEN_MACH *table, const char *name)
{
  while (table->name)
    {
      if (strcmp (name, table->bfd_name) == 0)
	return table;
      ++table;
    }
  abort ();
}

/* Subroutine of ubicom32_cgen_cpu_open to build the hardware table.  */

static void
build_hw_table (CGEN_CPU_TABLE *cd)
{
  int i;
  int machs = cd->machs;
  const CGEN_HW_ENTRY *init = & ubicom32_cgen_hw_table[0];
  /* MAX_HW is only an upper bound on the number of selected entries.
     However each entry is indexed by it's enum so there can be holes in
     the table.  */
  const CGEN_HW_ENTRY **selected =
    (const CGEN_HW_ENTRY **) xmalloc (MAX_HW * sizeof (CGEN_HW_ENTRY *));

  cd->hw_table.init_entries = init;
  cd->hw_table.entry_size = sizeof (CGEN_HW_ENTRY);
  memset (selected, 0, MAX_HW * sizeof (CGEN_HW_ENTRY *));
  /* ??? For now we just use machs to determine which ones we want.  */
  for (i = 0; init[i].name != NULL; ++i)
    if (CGEN_HW_ATTR_VALUE (&init[i], CGEN_HW_MACH)
	& machs)
      selected[init[i].type] = &init[i];
  cd->hw_table.entries = selected;
  cd->hw_table.num_entries = MAX_HW;
}

/* Subroutine of ubicom32_cgen_cpu_open to build the hardware table.  */

static void
build_ifield_table (CGEN_CPU_TABLE *cd)
{
  cd->ifld_table = & ubicom32_cgen_ifld_table[0];
}

/* Subroutine of ubicom32_cgen_cpu_open to build the hardware table.  */

static void
build_operand_table (CGEN_CPU_TABLE *cd)
{
  int i;
  int machs = cd->machs;
  const CGEN_OPERAND *init = & ubicom32_cgen_operand_table[0];
  /* MAX_OPERANDS is only an upper bound on the number of selected entries.
     However each entry is indexed by it's enum so there can be holes in
     the table.  */
  const CGEN_OPERAND **selected = xmalloc (MAX_OPERANDS * sizeof (* selected));

  cd->operand_table.init_entries = init;
  cd->operand_table.entry_size = sizeof (CGEN_OPERAND);
  memset (selected, 0, MAX_OPERANDS * sizeof (CGEN_OPERAND *));
  /* ??? For now we just use mach to determine which ones we want.  */
  for (i = 0; init[i].name != NULL; ++i)
    if (CGEN_OPERAND_ATTR_VALUE (&init[i], CGEN_OPERAND_MACH)
	& machs)
      selected[init[i].type] = &init[i];
  cd->operand_table.entries = selected;
  cd->operand_table.num_entries = MAX_OPERANDS;
}

/* Subroutine of ubicom32_cgen_cpu_open to build the hardware table.
   ??? This could leave out insns not supported by the specified mach/isa,
   but that would cause errors like "foo only supported by bar" to become
   "unknown insn", so for now we include all insns and require the app to
   do the checking later.
   ??? On the other hand, parsing of such insns may require their hardware or
   operand elements to be in the table [which they mightn't be].  */

static void
build_insn_table (CGEN_CPU_TABLE *cd)
{
  int i;
  const CGEN_IBASE *ib = & ubicom32_cgen_insn_table[0];
  CGEN_INSN *insns = xmalloc (MAX_INSNS * sizeof (CGEN_INSN));

  memset (insns, 0, MAX_INSNS * sizeof (CGEN_INSN));
  for (i = 0; i < MAX_INSNS; ++i)
    insns[i].base = &ib[i];
  cd->insn_table.init_entries = insns;
  cd->insn_table.entry_size = sizeof (CGEN_IBASE);
  cd->insn_table.num_init_entries = MAX_INSNS;
}

/* Subroutine of ubicom32_cgen_cpu_open to rebuild the tables.  */

static void
ubicom32_cgen_rebuild_tables (CGEN_CPU_TABLE *cd)
{
  int i;
  CGEN_BITSET *isas = cd->isas;
  unsigned int machs = cd->machs;

  cd->int_insn_p = CGEN_INT_INSN_P;

  /* Data derived from the isa spec.  */
#define UNSET (CGEN_SIZE_UNKNOWN + 1)
  cd->default_insn_bitsize = UNSET;
  cd->base_insn_bitsize = UNSET;
  cd->min_insn_bitsize = 65535; /* Some ridiculously big number.  */
  cd->max_insn_bitsize = 0;
  for (i = 0; i < MAX_ISAS; ++i)
    if (cgen_bitset_contains (isas, i))
      {
	const CGEN_ISA *isa = & ubicom32_cgen_isa_table[i];

	/* Default insn sizes of all selected isas must be
	   equal or we set the result to 0, meaning "unknown".  */
	if (cd->default_insn_bitsize == UNSET)
	  cd->default_insn_bitsize = isa->default_insn_bitsize;
	else if (isa->default_insn_bitsize == cd->default_insn_bitsize)
	  ; /* This is ok.  */
	else
	  cd->default_insn_bitsize = CGEN_SIZE_UNKNOWN;

	/* Base insn sizes of all selected isas must be equal
	   or we set the result to 0, meaning "unknown".  */
	if (cd->base_insn_bitsize == UNSET)
	  cd->base_insn_bitsize = isa->base_insn_bitsize;
	else if (isa->base_insn_bitsize == cd->base_insn_bitsize)
	  ; /* This is ok.  */
	else
	  cd->base_insn_bitsize = CGEN_SIZE_UNKNOWN;

	/* Set min,max insn sizes.  */
	if (isa->min_insn_bitsize < cd->min_insn_bitsize)
	  cd->min_insn_bitsize = isa->min_insn_bitsize;
	if (isa->max_insn_bitsize > cd->max_insn_bitsize)
	  cd->max_insn_bitsize = isa->max_insn_bitsize;
      }

  /* Data derived from the mach spec.  */
  for (i = 0; i < MAX_MACHS; ++i)
    if (((1 << i) & machs) != 0)
      {
	const CGEN_MACH *mach = & ubicom32_cgen_mach_table[i];

	if (mach->insn_chunk_bitsize != 0)
	{
	  if (cd->insn_chunk_bitsize != 0 && cd->insn_chunk_bitsize != mach->insn_chunk_bitsize)
	    {
	      fprintf (stderr, "ubicom32_cgen_rebuild_tables: conflicting insn-chunk-bitsize values: `%d' vs. `%d'\n",
		       cd->insn_chunk_bitsize, mach->insn_chunk_bitsize);
	      abort ();
	    }

 	  cd->insn_chunk_bitsize = mach->insn_chunk_bitsize;
	}
      }

  /* Determine which hw elements are used by MACH.  */
  build_hw_table (cd);

  /* Build the ifield table.  */
  build_ifield_table (cd);

  /* Determine which operands are used by MACH/ISA.  */
  build_operand_table (cd);

  /* Build the instruction table.  */
  build_insn_table (cd);
}

/* Initialize a cpu table and return a descriptor.
   It's much like opening a file, and must be the first function called.
   The arguments are a set of (type/value) pairs, terminated with
   CGEN_CPU_OPEN_END.

   Currently supported values:
   CGEN_CPU_OPEN_ISAS:    bitmap of values in enum isa_attr
   CGEN_CPU_OPEN_MACHS:   bitmap of values in enum mach_attr
   CGEN_CPU_OPEN_BFDMACH: specify 1 mach using bfd name
   CGEN_CPU_OPEN_ENDIAN:  specify endian choice
   CGEN_CPU_OPEN_END:     terminates arguments

   ??? Simultaneous multiple isas might not make sense, but it's not (yet)
   precluded.

   ??? We only support ISO C stdargs here, not K&R.
   Laziness, plus experiment to see if anything requires K&R - eventually
   K&R will no longer be supported - e.g. GDB is currently trying this.  */

CGEN_CPU_DESC
ubicom32_cgen_cpu_open (enum cgen_cpu_open_arg arg_type, ...)
{
  CGEN_CPU_TABLE *cd = (CGEN_CPU_TABLE *) xmalloc (sizeof (CGEN_CPU_TABLE));
  static int init_p;
  CGEN_BITSET *isas = 0;  /* 0 = "unspecified" */
  unsigned int machs = 0; /* 0 = "unspecified" */
  enum cgen_endian endian = CGEN_ENDIAN_UNKNOWN;
  va_list ap;

  if (! init_p)
    {
      init_tables ();
      init_p = 1;
    }

  memset (cd, 0, sizeof (*cd));

  va_start (ap, arg_type);
  while (arg_type != CGEN_CPU_OPEN_END)
    {
      switch (arg_type)
	{
	case CGEN_CPU_OPEN_ISAS :
	  isas = va_arg (ap, CGEN_BITSET *);
	  break;
	case CGEN_CPU_OPEN_MACHS :
	  machs = va_arg (ap, unsigned int);
	  break;
	case CGEN_CPU_OPEN_BFDMACH :
	  {
	    const char *name = va_arg (ap, const char *);
	    const CGEN_MACH *mach =
	      lookup_mach_via_bfd_name (ubicom32_cgen_mach_table, name);

	    machs |= 1 << mach->num;
	    break;
	  }
	case CGEN_CPU_OPEN_ENDIAN :
	  endian = va_arg (ap, enum cgen_endian);
	  break;
	default :
	  fprintf (stderr, "ubicom32_cgen_cpu_open: unsupported argument `%d'\n",
		   arg_type);
	  abort (); /* ??? return NULL? */
	}
      arg_type = va_arg (ap, enum cgen_cpu_open_arg);
    }
  va_end (ap);

  /* Mach unspecified means "all".  */
  if (machs == 0)
    machs = (1 << MAX_MACHS) - 1;
  /* Base mach is always selected.  */
  machs |= 1;
  if (endian == CGEN_ENDIAN_UNKNOWN)
    {
      /* ??? If target has only one, could have a default.  */
      fprintf (stderr, "ubicom32_cgen_cpu_open: no endianness specified\n");
      abort ();
    }

  cd->isas = cgen_bitset_copy (isas);
  cd->machs = machs;
  cd->endian = endian;
  /* FIXME: for the sparc case we can determine insn-endianness statically.
     The worry here is where both data and insn endian can be independently
     chosen, in which case this function will need another argument.
     Actually, will want to allow for more arguments in the future anyway.  */
  cd->insn_endian = endian;

  /* Table (re)builder.  */
  cd->rebuild_tables = ubicom32_cgen_rebuild_tables;
  ubicom32_cgen_rebuild_tables (cd);

  /* Default to not allowing signed overflow.  */
  cd->signed_overflow_ok_p = 0;
  
  return (CGEN_CPU_DESC) cd;
}

/* Cover fn to ubicom32_cgen_cpu_open to handle the simple case of 1 isa, 1 mach.
   MACH_NAME is the bfd name of the mach.  */

CGEN_CPU_DESC
ubicom32_cgen_cpu_open_1 (const char *mach_name, enum cgen_endian endian)
{
  return ubicom32_cgen_cpu_open (CGEN_CPU_OPEN_BFDMACH, mach_name,
			       CGEN_CPU_OPEN_ENDIAN, endian,
			       CGEN_CPU_OPEN_END);
}

/* Close a cpu table.
   ??? This can live in a machine independent file, but there's currently
   no place to put this file (there's no libcgen).  libopcodes is the wrong
   place as some simulator ports use this but they don't use libopcodes.  */

void
ubicom32_cgen_cpu_close (CGEN_CPU_DESC cd)
{
  unsigned int i;
  const CGEN_INSN *insns;

  if (cd->macro_insn_table.init_entries)
    {
      insns = cd->macro_insn_table.init_entries;
      for (i = 0; i < cd->macro_insn_table.num_init_entries; ++i, ++insns)
	if (CGEN_INSN_RX ((insns)))
	  regfree (CGEN_INSN_RX (insns));
    }

  if (cd->insn_table.init_entries)
    {
      insns = cd->insn_table.init_entries;
      for (i = 0; i < cd->insn_table.num_init_entries; ++i, ++insns)
	if (CGEN_INSN_RX (insns))
	  regfree (CGEN_INSN_RX (insns));
    }  

  if (cd->macro_insn_table.init_entries)
    free ((CGEN_INSN *) cd->macro_insn_table.init_entries);

  if (cd->insn_table.init_entries)
    free ((CGEN_INSN *) cd->insn_table.init_entries);

  if (cd->hw_table.entries)
    free ((CGEN_HW_ENTRY *) cd->hw_table.entries);

  if (cd->operand_table.entries)
    free ((CGEN_HW_ENTRY *) cd->operand_table.entries);

  free (cd);
}

