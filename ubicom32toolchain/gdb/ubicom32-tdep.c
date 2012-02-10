/* Target-dependent code for Ubicom32, for GDB.
   Copyright (C) 2000, 2001, 2002 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "frame.h"
#include "frame-unwind.h"
#include "frame-base.h"
#include "trad-frame.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "inferior.h"
#include "symfile.h"
#include "arch-utils.h"
#include "regcache.h"
#include "gdb_assert.h"
#include "gdb_string.h"
#include "gdbthread.h"
#include "dis-asm.h"
#include "elf-bfd.h"
#include "elf/ubicom32.h"
#include "opcodes/ubicom32-desc.h"
#include "opcodes/ubicom32-opc.h"
#include "ubicom32-tdep.h"
#include "objfiles.h"
#include "regset.h"
#include "dwarf2-frame.h"

extern struct target_ops ubicom32v3_core_ops;
extern CORE_ADDR ubicom32v5_translate_address (struct target_ops *ops, CORE_ADDR addr);

static enum ubicom32_abi
ubicom32_abi (struct gdbarch *gdbarch)
{
  return gdbarch_tdep (gdbarch)->ubicom32_abi;
}

/* Fetch the interpreter and executable loadmap addresses (for shared
   library support) for the FDPIC ABI.  Return 0 if successful, -1 if
   not.  (E.g, -1 will be returned if the ABI isn't the FDPIC ABI.)  */
int
ubicom32_fdpic_loadmap_addresses (struct gdbarch *gdbarch,
				  CORE_ADDR *interp_addr,
				  CORE_ADDR *exec_addr)
{
  if (ubicom32_abi (gdbarch) != UBICOM32_ABI_FDPIC)
    return -1;
  else
    {
      struct regcache *regcache = get_current_regcache ();

      if (interp_addr != NULL)
	{
	  ULONGEST val;
	  regcache_cooked_read_unsigned (regcache,
					 UBICOM32_FDPIC_LOADMAP_INTERP_REGNUM, &val);
	  *interp_addr = val;
	}
      if (exec_addr != NULL)
	{
	  ULONGEST val;
	  regcache_cooked_read_unsigned (regcache,
					 UBICOM32_FDPIC_LOADMAP_EXEC_REGNUM, &val);
	  *exec_addr = val;
	}
      return 0;
    }
}

static int ubicom32v3_frame_chain_valid (struct frame_info *fi);
static struct type *void_func_ptr = NULL;

/* Per-register information.  */
struct ubicom32_reg
  {
    /* Compile-time-constant fields.  */
    char *name;			/* register name */
    int num;			/* target register number */
    int size;			/* raw size, in bytes */
    struct type **type;		/* virtual data type */
    CORE_ADDR mem;		/* register's memory location */
    int flags;			/* REG_* bitmask */

    /* Run-time-calculated fields.  */
    int offset;			/* offset in registers[] buffer */
  };

/* Information about all registers.  find_reg_num() and addr_to_num() rely
   on this array being sorted by increasing register number and memory
   location, respectively.  */

struct ubicom32_reg ubicom32_regs[] = {
  /* General data.  */
  { "d0", 0, 4, &builtin_type_int32, 0x0, 0 },
  { "d1", 1, 4, &builtin_type_int32, UBICOM32_D1_REGADDR, 0 },
  { "d2", 2, 4, &builtin_type_int32, 0x8, 0 },
  { "d3", 3, 4, &builtin_type_int32, 0xc, 0 },
  { "d4", 4, 4, &builtin_type_int32, 0x10, 0 },
  { "d5", 5, 4, &builtin_type_int32, 0x14, 0 },
  { "d6", 6, 4, &builtin_type_int32, 0x18, 0 },
  { "d7", 7, 4, &builtin_type_int32, 0x1c, 0 },
  { "d8", 8, 4, &builtin_type_int32, UBICOM32_D8_REGADDR, 0 },
  { "d9", 9, 4, &builtin_type_int32, 0x24, 0 },
  { "d10", 10, 4, &builtin_type_int32, 0x28, 0 },
  { "d11", 11, 4, &builtin_type_int32, 0x2c, 0 },
  { "d12", 12, 4, &builtin_type_int32, 0x30, 0 },
  { "d13", 13, 4, &builtin_type_int32, 0x34, 0 },
  { "d14", 14, 4, &builtin_type_int32, 0x38, 0 },
  { "d15", 15, 4, &builtin_type_int32, 0x3c, 0 },
 /* General address.  */
  { "a0", UBICOM32_A0_REGNUM, 4, &builtin_type_int32, 0x80, 0 },
  { "a1", UBICOM32_A1_REGNUM, 4, &builtin_type_int32, 0x84, 0 },
  { "a2", UBICOM32_A2_REGNUM, 4, &builtin_type_int32, 0x88, 0 },
  { "a3", UBICOM32_A3_REGNUM, 4, &builtin_type_int32, 0x8c, 0 },
  { "a4", UBICOM32_A4_REGNUM, 4, &builtin_type_int32, 0x90, 0 },
  { "a5", UBICOM32_A5_REGNUM, 4, &builtin_type_int32, 0x94, 0 },
  { "a6", UBICOM32_A6_REGNUM, 4, &builtin_type_int32, UBICOM32_A6_REGADDR, 0 },
  /* Stack pointer.  */
  { "sp", UBICOM32_SP_REGNUM, 4, &builtin_type_int32, UBICOM32_SP_REGADDR, 0 },
  /* High 16 bits of multiply accumulator.  */
  { "acc0_hi", 24, 4, &builtin_type_int32, 0xa0, REG_HIDESOME },
  /* Low 32 bits of multiply accumulator.  */
  { "acc0_lo", 25, 4, &builtin_type_int32, 0xa4, REG_HIDESOME },
  /* Rounded and clipped S16.15 image of multiply accumulator.  */
  { "mac_rc16", 26, 4, &builtin_type_int32, 0xa8, REG_HIDESOME | REG_RDONLY },
  /* 3rd source operand.  */
  { "source_3", 27, 4, &builtin_type_uint32, 0xac, 0 },
  /* Current thread's execution count. */
  { "context_cnt", 28, 4, &builtin_type_uint32, 0xb0, REG_HIDESOME | REG_RDONLY },
  /* Control/status.  */
  { "csr", 29, 4, &builtin_type_uint32, 0xb4, 0 },
  /* Read-only status.  */
  { "rosr", UBICOM32_ROSR_REGNUM, 4, &builtin_type_uint32, 0xb8, 0 | REG_RDONLY },
  /* Iread output.  */
  { "iread_data", 31, 4, &builtin_type_uint32, 0xbc, 0 },
  /* Low 32 bits of interrupt mask.  */
  { "int_mask0", 32, 4, &builtin_type_uint32, 0xc0, REG_HIDESOME },
  /* High 32 bits of interrupt mask.  */
  { "int_mask1", 33, 4, &builtin_type_uint32, 0xc4, REG_HIDESOME },
  /* Program counter.  */
  { "pc", UBICOM32_PC_REGNUM, 4, &void_func_ptr, 0xd0, 0 },
  /* Chip identification.  */
  { "chip_id", 35, 4, &builtin_type_uint32, 0x100, REG_HIDESOME | REG_RDONLY },
  /* Low 32 bits of interrupt status.  */
  { "int_stat0", 36, 4, &builtin_type_uint32, 0x104, REG_HIDESOME | REG_RDONLY },
  /* High 32 bits of interrupt status.  */
  { "int_stat1", 37, 4, &builtin_type_uint32, 0x108, REG_HIDESOME | REG_RDONLY },
  /* Set bits in int_stat0.  */
  { "int_set0", 38, 4, &builtin_type_uint32, 0x114, REG_HIDESOME | REG_WRONLY },
  /* Set bits in int_stat1.  */
  { "int_set1", 39, 4, &builtin_type_uint32, 0x118, REG_HIDESOME | REG_WRONLY },
  /* Clear bits in int_stat0.  */
  { "int_clr0", 40, 4, &builtin_type_uint32, 0x124, REG_HIDESOME | REG_WRONLY },
  /* Clear bits in int_stat1.  */
  { "int_clr1", 41, 4, &builtin_type_uint32, 0x128, REG_HIDESOME | REG_WRONLY },
  /* Global control.  */
  { "global_ctrl", 42, 4, &builtin_type_uint32, 0x134, 0 },
  /* Set bits in mt_active.  */
  { "mt_active_set", 43, 4, &builtin_type_uint32, 0x13c, REG_HIDESOME | REG_WRONLY },
  /* Clear bits in mt_active.  */
  { "mt_active_clr", 44, 4, &builtin_type_uint32, 0x140, REG_HIDESOME | REG_WRONLY },
  /* Threads active status.  */
  { "mt_active", UBICOM32_MT_ACTIVE_REGNUM, 4, &builtin_type_uint32, 0x138, REG_HIDESOME | REG_RDONLY },
  /* Set bits in mt_dbg_active.  */
  { "mt_dbg_active_set", 46, 4, &builtin_type_uint32, 0x148, REG_HIDESOME | REG_WRONLY },
  /* Debugging threads active status.  */
  { "mt_dbg_active", 47, 4, &builtin_type_uint32, 0x144, REG_HIDESOME | REG_RDONLY },
  /* Threads enabled.  */
  { "mt_en", UBICOM32_MT_EN_REGNUM, 4, &builtin_type_uint32, 0x14C, REG_HIDESOME },
  /* Thread priorities.  */
  { "mt_pri", UBICOM32_MT_PRI_REGNUM, 4, &builtin_type_uint32, 0x150, REG_HIDESOME },
  /* Thread scheduling policies.  */
  { "mt_sched", UBICOM32_MT_SCHED_REGNUM, 4, &builtin_type_uint32, 0x154, REG_HIDESOME },
  /* Clear bits in mt_break.  */
  { "mt_break_clr", 51, 4, &builtin_type_uint32, 0x15C, REG_HIDESOME | REG_WRONLY },
  /* Threads stopped on a break condition.  */
  { "mt_break", 52, 4, &builtin_type_uint32, 0x158, REG_HIDESOME | REG_RDONLY },
  /* Single-step threads.  */
  { "mt_single_step", 53, 4, &builtin_type_uint32, 0x160, REG_HIDESOME },
  /* Threads with minimum delay scheduling constraing.  */
  { "mt_min_del_en", 54, 4, &builtin_type_uint32, 0x164, REG_HIDESOME },

  /* Debugging threads active status clear register.  */
  { "mt_dbg_active_clr", 55, 4, &builtin_type_int32, 0x17c, REG_HIDESOME | REG_WRONLY },
  /* Parity Error Address.  */
  { "perr_addr", 56, 4, &builtin_type_uint32, 0x16c, REG_HIDESOME | REG_RDONLY },
  /* Thread that wrote to dcapt.  */
  { "dcapt_thread", 57, 4, &builtin_type_uint32, 0x178, REG_HIDESOME | REG_RDONLY },
  /* PC at which thread wrote to dcapt.  */
  { "dcapt_pc", 58, 4, &builtin_type_uint32, 0x174, REG_HIDESOME | REG_RDONLY },
  /* Data capture address.  */
  { "dcapt", 59, 4, &builtin_type_uint32, 0x170, REG_HIDESOME },
  /* scratchpad registers */
  { "scratchpad0", 60, 4, &builtin_type_uint32, 0x180, 0 },
  { "scratchpad1", 61, 4, &builtin_type_uint32, 0x184, 0 },
  { "scratchpad2", 62, 4, &builtin_type_uint32, 0x188, 0 },
  { "scratchpad3", 63, 4, &builtin_type_uint32, 0x18c, 0 },

  /* High 16 bits of multiply accumulator1.  */
  { "acc1_hi", 64, 4, &builtin_type_int32, 0xe0, REG_HIDESOME },
  /* Low 32 bits of multiply accumulator1.  */
  { "acc1_lo", 65, 4, &builtin_type_int32, 0xe4, REG_HIDESOME },
  /* Hidden register to change threads in SID.  */
  { NULL, UBICOM32_GDB_THREAD_REGNUM, 4, &builtin_type_uint32, -1, REG_RDONLY },
};

/* Number of registers.  */
enum {
  NUM_UBICOM32_REGS = sizeof (ubicom32_regs) / sizeof (ubicom32_regs[0])
};

/* Indices into 2-element arrays representing "info all-registers" and "info
   registers" formatting information.  */
enum {
  SOME = 0,
  ALL  = 1
};

/* Register display information.  */

struct ubicom32_regdisp
  {
    char *rawbuf;		/* space for any raw register value */
    int namemax[2];		/* longest name in "info registers" and "info
				   all-registers" */
  };

struct ubicom32_machine
{
  struct ubicom32_reg * regs;	/* Poiner to Machine register infromation database */
  unsigned int num_regs;	/* Number of entries in register data base. */
  struct gdbarch *gdbarch;	/* Associated gdbarch pointer. NULL if uninitialized */
  struct ubicom32_regdisp regdisp;	/* Register display information */
  struct gdbarch * (*arch_init_fn)(struct gdbarch_info info, struct gdbarch_list *arches); /* Arch initialization function */
  int (*frame_chain_valid) (struct frame_info *fi); /* Frame chain valid function. */
  unsigned int pc_regnum;	/* Index for PC register */
  unsigned int sp_regnum;	/* Index for SP register */
  CORE_ADDR (*translate_address) (struct target_ops *ops, CORE_ADDR addr); /* Translation fcn.  */
  
};

struct ubicom32_machine *current_machine;
int mt_active_regnum, mt_pri_regnum, mt_sched_regnum, mt_en_regnum;

/* Thread debugging
   ----------------

   The Ubicom32 has up to 8 hardware "threads", each of which is an execution
   context with its own stack and register set.  The hardware steps each
   thread one at a time in an order determined by various user-settable
   scheduling parameters.

   The multi-threading state is accessible through various registers:

     - Bits 12..16 of rosr contain the number 0..31 of the thread that
       will be executed next.

     - 32-bit register mt_en specifies which threads can be scheduled to
       run.

     - 32-bit registers mt_active, mt_sched, and mt_pri respectively indicate
       which threads are active, hard-real-time, and high priority.

     - Hidden register UBICOM32_GDB_THREAD_REGNUM serves as a communication
       channel for GDB to tell SID to switch to a new thread.

   This module reads and writes those registers to implement remote.c gdbarch
   hooks REMOTE_SET_THREAD, REMOTE_THREAD_ALIVE, REMOTE_CURRENT_THREAD,
   REMOTE_THREADS_INFO, REMOTE_THREADS_EXTRA_INFO, and REMOTE_PID_TO_STR.

   An alternative implementation would be to modify SID to understand
   gdb/remote.c's thread protocol ('H' and thread-related 'q' packets).  */

/* Return the hardware thread number corresponding to gdb-internal thread id
   TID.  */

#define TID_TO_TNUM(tid)	(((tid) & 31)-1)

/* Return the hardware thread number corresponding to gdb-internal
   process+thread id PTID.  */

#define PTID_TO_TNUM(ptid)	TID_TO_TNUM (ptid_get_pid (ptid))

/* Return a gdb-internal thread id corresponding to hardware thread number
   TNUM.  */

#define TNUM_TO_TID(tnum) \
  ((ptid_get_pid (inferior_ptid) & ~31) | (tnum+1))

/* Return a gdb-internal process+thread id corresponding to hardware thread
   number TNUM.  */

#define TNUM_TO_PTID(tnum)	pid_to_ptid (TNUM_TO_TID (tnum))

/* Memory file buffer for formatting, needed for padding the results of
   val_print().  */

static struct ui_file *memfile = NULL;

/* bsearch() comparison function: return -1, 0, or 1 according to whether
   register number *NUMP is less than, equal to, or greater than register
   *REGP's number.  */

static int
find_reg_num_cmp (const void *nump, const void *regp)
{
  int num;
  struct ubicom32_reg *reg;

  num = *(int *) nump;
  reg = (struct ubicom32_reg *) regp;
  return num < reg->num ? -1 : num > reg->num;
}

/* bsearch() comparison function: return -1, 0, or 1 according to whether
   memory region *MEMP precedes, equals, or follows register *REGP's memory
   location.  */

static int
addr_to_num_cmp (const void *memp, const void *regp)
{
  CORE_ADDR mem;
  const struct ubicom32_reg *reg;

  mem = *(int *) memp;
  reg = regp;
  return mem < reg->mem ? -1 : mem > reg->mem;
}

/* Return whether N1 == N2.  */
static int
eq (int n1, int n2)
{
  return n1 == n2;
}

/* Return whether N1 >= N2.  */
static int
ge (int n1, int n2)
{
  return n1 >= n2;
}

/* Return whether N1 < N2.  */
static int
lt (int n1, int n2)
{
  return n1 < n2;
}

enum addressing_modes {
  DIRECT,
  IMMEDIATE,
  INDIRECT_WITH_INDEX,
  INDIRECT_WITH_OFFSET,
  INDIRECT,
  INDIRECT_WITH_POST_INCREMENT,
  INDIRECT_WITH_PRE_INCREMENT,
};

enum instructions {
  MOVE4,
  LEA1,
  LEA4,
  ADD4,
  MOVEI,
  PDEC,
};

static int
analyze_general_operand(int *field, int *f_bit10, int *f_type,
			int *f_r, int *f_M, int *f_i4_1,
			int *f_An, int *f_direct, int *f_imm8,
			int *f_imm7_1)
{
  int ret;

  /* grab the An field any way */
  *f_An = (*field >> 5) & 0x7;

  if(*field & 0x400)
    {
      int imm7, imm7t;
      *f_bit10 = 1;

      /* 7 bit immediate with An register */

      imm7t = (*field >> 8) & 0x3;
      imm7 = *field & 0x1f;

      imm7 |= (imm7t << 5);
      *f_imm7_1 = imm7;
      if(imm7)
	ret = INDIRECT_WITH_OFFSET;
      else
	ret = INDIRECT;
    }
  else
    {
      /* one of the other addressing modes */
      *f_type = (*field >> 8) & 0x3;

      switch(*f_type)
	{
	case 0:
	  ret = IMMEDIATE;
	  *f_imm8 = *field & 0xff;

	  /* we need to sign extend it */
	  if(*f_imm8 & 0x80)
	    *f_imm8 |= 0xffffff00;
	  *f_An = 0;
	  break;
	case 1:
	  ret = DIRECT;
	  *f_direct = *field & 0xff;
	  *f_direct *= 4;
	  *f_An = 0;
	  break;
	case 2:
	  /* need to examine the M bit */
	  if(*field & 0x10)
	    {
	      *f_M = 1;
	      ret = INDIRECT_WITH_PRE_INCREMENT;
	    }
	  else
	    ret = INDIRECT_WITH_POST_INCREMENT;

	  /* grab the i4 field */
	  *f_i4_1 = *field & 0xf;

	  /* we have to sign extend it if needed */
	  if(*f_i4_1 & 0x8)
	    *f_i4_1 |= 0xfffffff0;
	  break;
	case 3:
	  ret = INDIRECT_WITH_INDEX;

	  /* grab the index register */
	  *f_r= *field & 0xf;
	  break;
	default:
	  ret = -1;
	}
    }
  return ret;
}

/* Try to fetch the instruction at *PC into INFO.  Return whether:
     - the fetch was successful,
     - the instruction number is NUM, and
     - the conditions specified by the variable argument list are all true,
   and if so advance *PC to the next instruction.

   Each condition is specified by 3 arguments:
     (1) a pointer to an operand field in INFO->cgen_fields,
     (2) a function returning whether (1) and (3) match, and
     (3) a value to compare to (1).

   The last condition is followed by a null argument.  */

static int
fetch_insn (int num, CORE_ADDR *pc, CGEN_FIELDS *fields, ...)
{
  int len;
  va_list args;
  int *opnd, val;
  int (*match) (int, int);
  int insn;
  int dest_type, s1_type;
  int adjust_offsets = 1;

  int instruction = -1;

  memset (fields, 255, sizeof (CGEN_FIELDS));

  /* read the instruction */
  insn = read_memory_integer (*pc, 4);

  /* start setting up the cgen_fields */
  /* grab the primary opcode field */
  fields->f_op1 = (insn >> 27) &0x1f;
  if((fields->f_op1 != 0)&&
     (fields->f_op1 != 0x19)&&
     (fields->f_op1 != 0x0f))
    return 0;

  /* grab the destination field */
  fields->f_d = (insn >> 16) &0x7ff;

  /* analyze the destination bit fields */
  dest_type = analyze_general_operand(&fields->f_d, &fields->f_d_bit10, &fields->f_d_type,
				      &fields->f_d_r, &fields->f_d_M, &fields->f_d_i4_1,
				      &fields->f_d_An, &fields->f_d_direct, &fields->f_d_imm8,
				      &fields->f_d_imm7_1);


  s1_type = -1;
  if(fields->f_op1 == 0 || fields->f_op1 == 0x0f)
    {
      if(fields->f_op1 == 0)
	{
	  /* move.4 or a lea opcode. Grab op2 */
	  fields->f_op2 = (insn >> 11) &0x1f;

	  if((fields->f_op2 != 0x0c)&&
	     (fields->f_op2 != 0x1f)&&
	     (fields->f_op2 != 0x1c)&&
	     (fields->f_op2 != 0x1e))
	    /* does not match move.4, lea.1, lea.4 or pdec*/
	    return 0;

	  if(fields->f_op2 == 0x1f)
	    {
	      /* lea.1 case. Leave the offsets alone */
	      adjust_offsets=0;
	      instruction = LEA1;
	    }
	  else if(fields->f_op2 == 0x1c)
	    instruction = LEA4;
	  else if(fields->f_op2 == 0x1e)
	    instruction = PDEC;
	  else
	    instruction = MOVE4;
	}
      else
	instruction = ADD4;

      if(instruction == ADD4)
	{
	  /* setup the f_s2 subfield */
	  fields->f_s2 = (insn >> 11) & 0xf;
	}

      /* setup the f_s1 subfields */
      fields->f_s1 = insn &0x7ff;

      s1_type = analyze_general_operand(&fields->f_s1, &fields->f_s1_bit10, &fields->f_s1_type,
					&fields->f_s1_r, &fields->f_s1_M, &fields->f_s1_i4_1,
					&fields->f_s1_An, &fields->f_s1_direct, &fields->f_s1_imm8,
					&fields->f_s1_imm7_1);

      if(adjust_offsets)
	{
	  if(fields->f_d_i4_1)
	    {
	      fields->f_d_i4_4 = fields->f_d_i4_1*4;
	      fields->f_d_i4_1 = 0;
	    }

	  if(fields->f_d_imm7_1)
	    {
	      fields->f_d_imm7_4 = fields->f_d_imm7_1*4;
	      fields->f_d_imm7_1 = 0;
	    }

	  if(fields->f_s1_i4_1)
	    {
	      fields->f_s1_i4_4 = fields->f_s1_i4_1*4;
	      fields->f_s1_i4_1 = 0;
	    }

	  if(instruction == PDEC)
	    {
	      fields->f_s1_imm7_1 *= -1;
	      fields->f_s1_imm7_1 &= 0x7f;
	    }

	  if(fields->f_s1_imm7_1)
	    {
	      fields->f_s1_imm7_4 = fields->f_s1_imm7_1*4;
	      fields->f_s1_imm7_1 = 0;
	    }
	}
    }
  else
    {
      instruction = MOVEI;
      /* this is the movei case. Need to pick up the 16 bits and sign extend it */
      fields->f_imm16_2 = insn & 0xffff;

      /* check for sign extension */
      if(fields->f_imm16_2 & 0x8000)
	fields->f_imm16_2 |= 0xffff0000;
    }

  /* now do the CGNE_INSN_NUM stuff */
  switch(num)
    {
    case UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT:
      if(instruction != MOVE4)
	return 0;

      if(dest_type != INDIRECT_WITH_PRE_INCREMENT)
	return 0;

      if(s1_type != DIRECT)
	return 0;
      break;
    case UBICOM32_INSN_MOVEI_D_DIRECT:
      if(instruction != MOVEI)
	return 0;

      if(dest_type != DIRECT)
	return 0;
      break;
    case UBICOM32_INSN_LEA_1_D_DIRECT_S1_EA_INDIRECT_WITH_INDEX_1:
      if(instruction != LEA1)
	return 0;

      if(dest_type != DIRECT)
	return 0;

      if(s1_type != INDIRECT_WITH_INDEX)
	return 0;

      break;
    case UBICOM32_INSN_ADD_4_D_DIRECT_S1_DIRECT:
      if(instruction != ADD4)
	return 0;

      if(dest_type != DIRECT)
	return 0;

      if(s1_type != DIRECT)
	return 0;
      break;
    case UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT:
      if(instruction != MOVE4)
	return 0;

      if(dest_type != INDIRECT_WITH_OFFSET)
	return 0;

      if(s1_type != DIRECT)
	return 0;
      break;
    case UBICOM32_INSN_MOVE_4_D_INDIRECT_4_S1_DIRECT:
      if(instruction != MOVE4)
	return 0;

      if(dest_type != INDIRECT)
	return 0;

      if(s1_type != DIRECT)
	return 0;
      break;
    case UBICOM32_INSN_LEA_4_D_DIRECT_S1_EA_INDIRECT_WITH_OFFSET_4:
      if(instruction != LEA4)
	return 0;

      if(dest_type != DIRECT)
	return 0;

      if(s1_type != INDIRECT_WITH_OFFSET)
	return 0;
      break;
    case UBICOM32_INSN_PDEC_D_DIRECT_PDEC_S1_EA_INDIRECT_WITH_OFFSET_4:
      if(instruction != PDEC)
	return 0;

      if(dest_type != DIRECT)
	return 0;

      if(s1_type != INDIRECT_WITH_OFFSET)
	return 0;
      break;
    }

  len = 4;

  va_start (args, fields);
  for (;;)
    {
      opnd = va_arg (args, int *);
      if (!opnd)
	break;

      match = (int (*)(int, int)) va_arg (args, void *);
      val = va_arg (args, int);
      if (!match (*opnd, val))
	return 0;
    }

  *pc += len;
  return 1;
}

/* Debug.  */
#define DEBUG_NONE 0
#define DEBUG_NORMAL 1
#define DEBUG_EXTRA 2
#define DEBUG_MAX 3
#define DEBUG_FRAME DEBUG_MAX
static int ubicom32_tdep_debug = DEBUG_NORMAL;

/* Frame info.  */
struct ubicom32_unwind_cache
{
  CORE_ADDR entry_pc;
  CORE_ADDR current_pc;
  CORE_ADDR entry_sp;
  CORE_ADDR current_sp;
  /* Table indicating the location of each and every register.  */
  struct trad_frame_saved_reg *saved_regs;
};

/* If a register has number NUM, return that register.  Otherwise, if
   !CALLER, return null, else throw an error message identifying CALLER.  */
static struct ubicom32_reg *
machine_find_reg_num (int num, char *caller)
{
  struct ubicom32_reg *found;

  found = bsearch (&num, current_machine->regs, current_machine->num_regs,
		   sizeof (struct ubicom32_reg), find_reg_num_cmp);
  if (found)
    return found;
  if (caller)
    internal_error (__FILE__, __LINE__,
		    "%s: invalid register target number %d", caller, num);
  return NULL;
}

/* If a register is at memory location ADDR, return that register's number.
   Otherwise, if !CALLER, return -1, else throw an error message identifying
   CALLER.  */
static int
machine_addr_to_num (CORE_ADDR addr, char *caller)
{
  struct ubicom32_reg *found;

  found = bsearch (&addr, current_machine->regs, current_machine->num_regs,
		   sizeof (struct ubicom32_reg), addr_to_num_cmp);
  if (found)
    return found->num;
  if (caller)
    internal_error (__FILE__, __LINE__,
		    "%s: invalid register address 0x%lx", caller, addr);
  return -1;
}

static CORE_ADDR
read_register(int regnum)
{
  char tmp, *ptr;
  CORE_ADDR tmp1;
  regcache_raw_read(get_current_regcache (), regnum, (gdb_byte *)&tmp1);

  /* we have to szizzle the data. */
  ptr = (char *)&tmp1;
  tmp = ptr[3];
  ptr[3] = ptr[0];
  ptr[0] = tmp;
  tmp = ptr[1];
  ptr[1] = ptr[2];
  ptr[2] = tmp;
  return tmp1;
}

/* Parse the prologue of the function starting at PC.

   If FRAME is null, return the address of the first instruction after the
   prologue.  Otherwise, store saved register and frame pointer information in
   FRAME.  */

static struct ubicom32_unwind_cache *
ubicom32_parse_prologue (struct frame_info *next_frame,
		void **this_prologue_cache)

{
  struct ubicom32_unwind_cache *ubicom32_frame_info;
  int const sp_regaddr = UBICOM32_SP_REGADDR;
  int const fp_regaddr = UBICOM32_A6_REGADDR;
  CGEN_FIELDS fields;
  int sp_offset, fp_offset;
  CORE_ADDR sp;
  CORE_ADDR entry_sp;
  CORE_ADDR fp;
  CORE_ADDR start, end, start1, end1;
  CORE_ADDR old_pc, pc;
  int fpSetUp = 0;
  int num_insns = 101;

  int large, i;
  int reg_val;

  /* Table of offsets describing where each register is stored.  -1
     denotes a register that was not saved.  */

  int *saved_regs_offset;
  int sizeof_saved_regs_offset = current_machine->num_regs * sizeof (*saved_regs_offset);
  saved_regs_offset = alloca (sizeof_saved_regs_offset);
  memset (saved_regs_offset, -1, sizeof_saved_regs_offset);

  if ((*this_prologue_cache))
    return (*this_prologue_cache);

  if (ubicom32_tdep_debug >= DEBUG_FRAME)
    fprintf_unfiltered (gdb_stdlog, "ubicom32_parse_prologue:\n");

  ubicom32_frame_info = FRAME_OBSTACK_ZALLOC (struct ubicom32_unwind_cache);
  (*this_prologue_cache) = ubicom32_frame_info;
  ubicom32_frame_info->saved_regs = trad_frame_alloc_saved_regs (next_frame);

  /* Starting point.  */
  ubicom32_frame_info->current_pc = frame_pc_unwind (next_frame);
  ubicom32_frame_info->entry_pc = frame_func_unwind (next_frame, NORMAL_FRAME);
  ubicom32_frame_info->entry_sp = 0;
  ubicom32_frame_info->current_sp = frame_sp_unwind (next_frame);

  fp_offset = 0;

  old_pc = pc = GDB_IMEM (ubicom32_frame_info->current_pc);
  sp = GDB_IMEM (ubicom32_frame_info->current_sp);

  /* find the start and the end of the function */
  if(find_pc_partial_function (pc, NULL, &start, &end))
    {
      int subtrahend_regaddr = -1;
      int subtrahend;

      /* Start out with an enclosing (up, prev, outer) frame's SP set to
	 NULL.  Hopefully the logic below will find a better value, if not
	 well the frame will just terminate a little early.  Note that
	 NULL is special - GDB assumes a ZERO and not GDB_DMEM(0)
	 terminates a frame chain.  (If you've got your thinking cap on,
	 you'll realize that this isn't even needed - saved_regs[] is
	 already zero - but what the heck set it to zero here explicitly).  */

      sp_offset = 0;
      while(start < pc)
	{
	  num_insns --;
	  if (num_insns == 0)
	    break;

	  if (num_insns == 100)
	    {
	      /* Do this only if this the first instruciton in the prologue. */
	      /* move.4 -4(sp)++,d9  Copy 1st varargs argument to stack.  */
	      if (fetch_insn (UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT,
			      &start, &fields,
			      &fields.f_s1_direct, eq, UBICOM32_D9_REGADDR,
			      &fields.f_d_An, eq, H_AR_SP,
			      (int *) 0))
		{
		  int i;
		  
		  sp_offset += -fields.f_d_i4_4;
		  
		  for (i = 8; i >= 0; i--)
		    {
		      /* move.4 -4(sp)++,d8  Copy 2nd - 13th varargs arguments to stack.  */
		      if(start < pc)
			{
			  if (!fetch_insn (UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT,
					   &start, &fields,
					   &fields.f_d_i4_4, eq, -4l,
					   &fields.f_s1_direct, eq, UBICOM32_D0_REGADDR + 4 * i,
					   &fields.f_d_An, eq, H_AR_SP,
					   (int *) 0))
			    {
			      break;
			    }
			  else
			    {
			      sp_offset += -fields.f_d_i4_4;
			    }
			}
		      else
			break;
		    }

		  if(i >= 0)
		    {
		      // did not quite complete the loop
		      break;
		    }
		}
	    }

	  /* Try to determine the SP_OFFSET (the value added to the stack on
	     entry).  If this fails, return the address of the instructions
	     being parsed.  Set LARGE if the frame is >8-bits.  */
	  /* movei d15,#-96  Copy negative frame size to a register.  */
	  if (fetch_insn (UBICOM32_INSN_MOVEI_D_DIRECT,
			       &start, &fields,
			       &fields.f_imm16_2, lt, 0l,
			       (int *) 0))
	    {
	      subtrahend = fields.f_imm16_2;
	      subtrahend_regaddr = fields.f_d_direct;
	    }
	  else if ((subtrahend_regaddr != -1) &&
		   fetch_insn (UBICOM32_INSN_LEA_1_D_DIRECT_S1_EA_INDIRECT_WITH_INDEX_1,
			       &start, &fields,
			       &fields.f_d_direct, eq, sp_regaddr,
			       &fields.f_s1_An, eq, H_AR_SP,
			       &fields.f_s1_r, eq, subtrahend_regaddr / UBICOM32_REG_SIZE,
			       (int *) 0))
	    {
	      /* lea.1 sp,(sp,d15)  Subtract frame size from sp.  */
	      sp_offset += -subtrahend;	
	      subtrahend_regaddr = -1;
	    }
	  else if ((subtrahend_regaddr != -1) &&
		   fetch_insn (UBICOM32_INSN_ADD_4_D_DIRECT_S1_DIRECT,
			       &start, &fields,
			       &fields.f_d_direct, eq, sp_regaddr,
			       &fields.f_s1_direct, eq, sp_regaddr,
			       &fields.f_s2, eq, subtrahend_regaddr / UBICOM32_REG_SIZE,
			       (int*) 0))
	    {
	      /* add.4 sp, sp, d15  Alternative insn.  */
	      sp_offset += -subtrahend;	
	      subtrahend_regaddr = -1;
	    }
	  /* move.4	-32(sp)++,a5 */
	  else if(fetch_insn (UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT,
			      &start, &fields,
			      &fields.f_d_An, eq, H_AR_SP,
			      (int *) 0))
	    {
	      int saved_regaddr = fields.f_s1_direct;
	      int regnum = machine_addr_to_num (saved_regaddr, "parse_prologue");
	      gdb_assert (regnum >= 0 && regnum < current_machine->num_regs);

	      sp_offset += -fields.f_d_i4_4;
	      if( regnum > 9)
		{
		  if (saved_regs_offset[regnum] == -1)
		    {
		      saved_regs_offset[regnum] = sp_offset;
		      
		      if(regnum == UBICOM32_A5_REGNUM)
			{
			  // this is the frame pointer offset ??
			  fp_offset = sp_offset;
			  fpSetUp = 1;
			}
		    }
		}
	    }
	  /* pdec sp, 48(sp) */
	  else if (fetch_insn (UBICOM32_INSN_PDEC_D_DIRECT_PDEC_S1_EA_INDIRECT_WITH_OFFSET_4,
			       &start, &fields,
			       &fields.f_s1_An, eq, H_AR_SP,
			       &fields.f_d_direct, eq, sp_regaddr,
			       (int *) 0))
	    {
	      sp_offset += fields.f_s1_imm7_4;
	      fp_offset = sp_offset;
	      fpSetUp = 1;
	    }

	  /* move.4 4(sp),a6  Save call-preserved registers.  */
	  else if (fetch_insn (UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT,
			       &start, &fields,
			       &fields.f_d_imm7_4, ge, 0l,
			       &fields.f_d_An, eq, H_AR_SP,
			       (int *) 0))
	    {
	      int saved_offset = sp_offset - fields.f_d_imm7_4;
	      int saved_regaddr = fields.f_s1_direct;
	      int regnum = machine_addr_to_num (saved_regaddr, "parse_prologue");
	      gdb_assert (regnum >= 0 && regnum < current_machine->num_regs);

	      if (regnum > 9)
		{
		  if (saved_regs_offset[regnum] == -1)
		    {
		      saved_regs_offset[regnum] = saved_offset;
		      
		      if(regnum == UBICOM32_A5_REGNUM)
			{
			  // this is the frame pointer offset ??
			  fp_offset = sp_offset;
			  fpSetUp = 1;
			}
		    }
		}
	    }
	  /* move.4 (sp),a6  Likewise, in leaf frame.  */
	  else if (fetch_insn (UBICOM32_INSN_MOVE_4_D_INDIRECT_4_S1_DIRECT,
			       &start, &fields,
			       &fields.f_d_An, eq, H_AR_SP,
			       (int *) 0))
	    {
	      int saved_offset = sp_offset;
	      int saved_regaddr = fields.f_s1_direct;
	      int regnum = machine_addr_to_num (saved_regaddr, "parse_prologue");
	      gdb_assert (regnum >= 0 && regnum < current_machine->num_regs);

	      if (regnum > 9)
		{
		  if (saved_regs_offset[regnum] == -1)
		    {
		      saved_regs_offset[regnum] = saved_offset;
		      if(regnum == UBICOM32_A5_REGNUM)
			{
			  // this is the frame pointer offset ??
			  fp_offset = sp_offset;
			  fpSetUp = 1;
			}
		    }
		}
	    }

	  /* lea.4 a6, offset(sp)  Setup frame pointer.  */
	  else if (fetch_insn (UBICOM32_INSN_LEA_4_D_DIRECT_S1_EA_INDIRECT_WITH_OFFSET_4,
			       &start, &fields,
			       &fields.f_s1_An, eq, H_AR_SP,
			       &fields.f_d_direct, eq, fp_regaddr,
			       (int *) 0))
	    {
	      fp_offset = sp_offset - fields.f_s1_imm7_4;
	      fpSetUp = 1;
	    }
	  else
	    {
	      int insn;
	      int major_opcode, opcode_extension;
	      
	      /* read the instruction */
	      insn = read_memory_integer (start, 4);
	  
	      major_opcode = (insn >> 27) & 0x1f;
	      opcode_extension = (insn >> 11) & 0x1f;
	  
	      /*
	       * If the instruction is either JMP, CALL, CALLI or RET we are done.
	       * RET Major opcode 0 Extension 4
	       * BKPT Major opcode 0 Ex       7
	       * SUSPEND                      1
	       * For Major opc 0 if Ex is < 0A the exit.
	       *
	       * CALL Major opc 1B
	       * CALLI Major opc 1C
	       * JMP Major opc 1A
	       */
	      if ((major_opcode == 0) && (opcode_extension < 0xA))
		{
		  /* Time to get out.*/
		  break;
		}

	      if ((major_opcode == 0x1A) || (major_opcode == 0x1B) || (major_opcode == 0x1E))
		{
		  break;
		}

	      /* nothing matches. Bump pc by 4 and try again.*/
	      //printf("no match to mj opc 0x%x ext = 0x%x\n", major_opcode, opcode_s
	      start += 4;
	      continue;
	    }
	}

      entry_sp = sp + sp_offset;
      fp_offset = entry_sp - fp_offset;
      ubicom32_frame_info->entry_sp = entry_sp;


      /* SP and FP contain the real address and not where it is saved on the stack. */
      trad_frame_set_value (ubicom32_frame_info->saved_regs, current_machine->sp_regnum, entry_sp);

      /* Compute the address of each register saved on the stack using the
	 FP and the SAVED_REG_OFFSETS.  */
      for (i = 0; i < current_machine->num_regs; i++)
	{
	  if (i == current_machine->sp_regnum)
	    continue;
	  if (saved_regs_offset[i] >= 0)
	    {
	      /* read the register value from where it has been saved and put it in as a value. */
	      int reg_val = read_memory_integer((entry_sp - saved_regs_offset[i]), 4);
	      trad_frame_set_value (ubicom32_frame_info->saved_regs, i, reg_val);
	    }
	}

      /* See if register A5 the return address was saved in the frame. */
      if(saved_regs_offset[UBICOM32_A5_REGNUM] >= 0)
	{
	  /* A5 is saved in the stack frame. That is the new PC value. */
	  reg_val = read_memory_integer((entry_sp - saved_regs_offset[UBICOM32_A5_REGNUM]), 4);
	}
      else
	{
	  /* Read A5 from the raw regcache. Usually that is the return address */
	  reg_val = read_register(UBICOM32_A5_REGNUM);
	}

      (void)find_pc_partial_function (reg_val, NULL, &start, &end);
      (void)find_pc_partial_function (reg_val-4, NULL, &start1, &end1);

      if (start != start1)
	reg_val -= 4;

      trad_frame_set_value (ubicom32_frame_info->saved_regs, current_machine->pc_regnum, reg_val);
    }
  else
    {
      ubicom32_frame_info->entry_sp = ubicom32_frame_info->current_sp;
    }

  return ubicom32_frame_info;
}

static CORE_ADDR
find_end_prologue (CORE_ADDR pc)
{
  int const sp_regaddr = UBICOM32_SP_REGADDR;
  int const fp_regaddr = UBICOM32_A6_REGADDR;
  CGEN_FIELDS fields;
  int sp_offset, fp_offset;
  CORE_ADDR start, end;
  CORE_ADDR old_pc = pc;
  int fpSetUp = 0;
  CORE_ADDR end_prologue;
  int large;
  int insns = 101;
  pc = GDB_IMEM (pc);

  end_prologue = pc;

  /* find the start and the end of the function */
  (void)find_pc_partial_function (pc, NULL, &start, &end);

  /* Go through all the save-register instructions computing an
     SAVED_REGS_OFFSET (from the SP) for each.  */
  pc = start;
  while(pc < end)
    {
      insns--;
      if (insns == 0)
	{
	  return end_prologue;
	}

      if (insns == 100)
	{
	  /* move.4 -4(sp)++,d9  Copy 1st varargs argument to stack.  */
	  if (fetch_insn (UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT,
			  &pc, &fields,
			  &fields.f_s1_direct, eq, UBICOM32_D9_REGADDR,
			  &fields.f_d_An, eq, H_AR_SP,
			  (int *) 0))
	    {
	      int i;
	      
	      for (i = 8; i >= 0; i--)
		{
		  /* move.4 -4(sp)++,d12  Copy 2nd - 13th varargs arguments to stack.  */
		  if (!fetch_insn (UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT,
				   &pc, &fields,
				   &fields.f_d_i4_4, eq, -4l,
				   &fields.f_s1_direct, eq, UBICOM32_D0_REGADDR + 4 * i,
				   &fields.f_d_An, eq, H_AR_SP,
				   (int *) 0))
		    {
		      return pc;
		    }
		}
	    }
	}

      /* movei d15,#-96  Copy negative frame size to a register.  */
      if (fetch_insn (UBICOM32_INSN_MOVEI_D_DIRECT,
		      &pc, &fields,
		      &fields.f_imm16_2, lt, 0l,
		      (int *) 0))
	{
	  int regaddr = fields.f_d_direct;
	  //sp_offset += -fields.f_imm16_2;

	  /* Check that the code sequence really did compute ``SP = SP -
	     SP_OFFSET''.  If it didn't bail out returning a PC pointing to
	     the first instruction before this entire mess started.  */

	  /* lea.1 sp,(sp,d15)  Subtract frame size from sp.  */
	  if (!fetch_insn (UBICOM32_INSN_LEA_1_D_DIRECT_S1_EA_INDIRECT_WITH_INDEX_1,
			   &pc, &fields,
			   &fields.f_d_direct, eq, sp_regaddr,
			   &fields.f_s1_An, eq, H_AR_SP,
			   &fields.f_s1_r, eq, regaddr / UBICOM32_REG_SIZE,
			   (int *) 0)
	      /* add.4 sp, sp, d15  Alternative insn.  */
	      && !fetch_insn (UBICOM32_INSN_ADD_4_D_DIRECT_S1_DIRECT,
			      &pc, &fields,
			      &fields.f_d_direct, eq, sp_regaddr,
			      &fields.f_s1_direct, eq, sp_regaddr,
			      &fields.f_s2, eq, regaddr / UBICOM32_REG_SIZE,
			      (int*) 0))
	    {
	      //return old_pc;
	      continue;
	    }

	  fpSetUp = 1;
	  end_prologue = pc;
	}
      /* move.4	-32(sp)++,a5 */
      else if(fetch_insn (UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT,
			  &pc, &fields,
			  &fields.f_d_An, eq, H_AR_SP,
			  (int *) 0))
	{
	  int saved_regaddr = fields.f_s1_direct;
	  int regnum = machine_addr_to_num (saved_regaddr, "parse_prologue");

	  if (regnum > 9)
	    {
	      fpSetUp = 1;
	      end_prologue = pc;
	    }
	}
      /* pdec sp, 48(sp) */
      else if (fetch_insn (UBICOM32_INSN_PDEC_D_DIRECT_PDEC_S1_EA_INDIRECT_WITH_OFFSET_4,
			   &pc, &fields,
			   &fields.f_s1_An, eq, H_AR_SP,
			   &fields.f_d_direct, eq, sp_regaddr,
			   (int *) 0))
	{
	  fpSetUp = 1;
	  end_prologue = pc;
	}
      /* move.4 +4(sp),a6  Save call-preserved registers.  */
      else if (fetch_insn (UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_OFFSET_4_S1_DIRECT,
			   &pc, &fields,
			   &fields.f_d_imm7_4, ge, 0l,
			   &fields.f_d_An, eq, H_AR_SP,
			   (int *) 0))
	{
	  int saved_regaddr = fields.f_s1_direct;
	  int regnum = machine_addr_to_num (saved_regaddr, "parse_prologue");
	  gdb_assert (regnum >= 0 && regnum < current_machine->num_regs);
	  
	  if (regnum > 9)
	    {
	      fpSetUp = 1;
	      end_prologue = pc;
	    }
	}
      /* move.4 (sp),a6  Likewise, in leaf frame.  */
      else if (fetch_insn (UBICOM32_INSN_MOVE_4_D_INDIRECT_4_S1_DIRECT,
			   &pc, &fields,
			   &fields.f_d_An, eq, H_AR_SP,
			   (int *) 0))
	{
	  int saved_regaddr = fields.f_s1_direct;
	  int regnum = machine_addr_to_num (saved_regaddr, "parse_prologue");
	  gdb_assert (regnum >= 0 && regnum < current_machine->num_regs);
	  
	  if (regnum > 9)
	    {
	      fpSetUp = 1;
	      end_prologue = pc;
	    }
	}
      else if(fetch_insn (UBICOM32_INSN_MOVE_4_D_INDIRECT_WITH_PRE_INCREMENT_4_S1_DIRECT,
			  &pc, &fields,
			  &fields.f_d_An, eq, H_AR_SP,
			  (int *) 0))
	{
	  fpSetUp = 1;
	  end_prologue = pc;
	}
      /* lea.4 a6, offset(sp)  Setup frame pointer.  */
      else if (fetch_insn (UBICOM32_INSN_LEA_4_D_DIRECT_S1_EA_INDIRECT_WITH_OFFSET_4,
			   &pc, &fields,
			   &fields.f_s1_An, eq, H_AR_SP,
			   &fields.f_d_direct, eq, fp_regaddr,
			   (int *) 0))
	{
	  fpSetUp = 1;
	  end_prologue = pc;
	}
      else
	{
	  int insn;
	  int major_opcode, opcode_extension;

	  /* read the instruction */
	  insn = read_memory_integer (pc, 4);

	  major_opcode = (insn >> 27) & 0x1f;
	  opcode_extension = (insn >> 11) & 0x1f;
	  
	  /*
	   * If the instruction is either JMP, CALL, CALLI or RET we are done.
	   * RET Major opcode 0 Extension 4
	   * BKPT Major opcode 0 Ex       7
	   * SUSPEND                      1
	   * For Major opc 0 if Ex is < 0A the exit.
	   *
	   * CALL Major opc 1B
	   * CALLI Major opc 1C
	   * JMP Major opc 1A
	   */
	  if ((major_opcode == 0) && (opcode_extension < 0xA))
	    {
	      /* Time to get out.*/
	      return end_prologue;
	    }

	  if ((major_opcode == 0x1A) || (major_opcode == 0x1B) || (major_opcode == 0x1E))
	    {
	      return end_prologue;
	    }

	  /* nothing matches. Bump pc by 4 and try again.*/
	  //printf("no match to mj opc 0x%x ext = 0x%x\n", major_opcode, opcode_extension);
	  pc += 4;
	  continue;
	}
    }

  /* If you got here nothing matched */
  return old_pc;
}

/* Return the address of the "real" code beyond the function entry prologue
   found at PC.  */

static CORE_ADDR
ubicom32_skip_prologue (struct gdbarch *gdbarch, CORE_ADDR start_pc)
{
  /* TOOD: make use of gdbarch? */
  return find_end_prologue (start_pc);
}

/* Assuming NEXT_FRAME->prev is a dummy, return the frame ID of that
   dummy frame.  The frame ID's base needs to match the TOS value
   saved by save_dummy_frame_tos(), and the PC match the dummy frame's
   breakpoint.  */
static struct frame_id
ubicom32_unwind_dummy_id (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  ULONGEST base;

  if (ubicom32_tdep_debug >= DEBUG_FRAME)
    fprintf_unfiltered (gdb_stdlog, "ubicom32_unwind_dummy_id\n");

  //frame_unwind_unsigned_register (next_frame, current_machine->sp_regnum, &base);
  base = frame_unwind_register_unsigned (next_frame, current_machine->sp_regnum);
  return frame_id_build (base, frame_pc_unwind (next_frame));
}

#if 0
#define UBICOM32_MAIN_STRING "__main__"
#endif
#define UBICOM32_MAIN_STRING "main"
#define UBICOM32_MAIN_STRING2 "system_main"
#define UBICOM32_MAIN_STRING3 "arch_thread_main"
#define UBICOM32_FLASH_ADDR 0x20000000
#define UBICOM32_FLASH_END	0x2ffffffc
#define UBICOM32_PRAM_START 0x40000000
#define UBICOM32_PRAM_END 0x4007fffc
#define UBICOM32_DATARAM_START 0x100000
#define UBICOM32_DATARAM_END 0x10fffc

//extern unsigned int entry_point_address(void);

static int
ubicom32_frame_chain_valid (struct frame_info *fi)
{
  int line;
  int offset;
  int unmapped;
  char *filename = NULL;
  char *name = NULL;
  CORE_ADDR pc = frame_pc_unwind(fi);
  CORE_ADDR sp = frame_sp_unwind(fi);
  unsigned int entry = entry_point_address();

  /* ubicom32v3 isn't handled correctly -- call from here */
  if(entry != 0x20000000)
    return ubicom32v3_frame_chain_valid(fi);

  if (ubicom32_tdep_debug >= DEBUG_FRAME)
    fprintf_unfiltered (gdb_stdlog, "ubicom32_frame_chain_valid pc = 0x%x sp = 0x%x\n", (unsigned int)pc, (unsigned int)sp);

  if(pc < UBICOM32_FLASH_ADDR ||
     (UBICOM32_FLASH_END < pc && pc < UBICOM32_PRAM_START) ||
     pc > UBICOM32_PRAM_END)
    {
      /* pc is in some invalid zone */
      return 0;
    }

  /* check if sp is valid at all */
  if(sp < UBICOM32_DATARAM_START ||
     sp > UBICOM32_DATARAM_END)
    return 0;

  /* check if the pc is valid at all */
  if (!build_address_symbolic (pc, 0, &name, &offset, &filename, &line, &unmapped))
    {
      /* we got a hit */
      if(name)
	{
	  if(strstr(name, UBICOM32_MAIN_STRING))
	    {
	      /* it is present */
	      return 0;
	    }
	  else if(strstr(name, UBICOM32_MAIN_STRING2))
	    {
	      /* it is present */
	      return 0;
	    }
	  else if(strstr(name, UBICOM32_MAIN_STRING3))
	    {
	      /* it is present */
	      return 0;
	    }
	}
    }

  return 1;
}

/* Given a GDB frame, determine the address of the calling function's
   frame.  This will be used to create a new GDB frame struct.  */
static void
ubicom32_frame_this_id (struct frame_info *next_frame,
		    void **this_prologue_cache,
		    struct frame_id *this_id)
{
  struct ubicom32_unwind_cache *info;
  CORE_ADDR base;
  CORE_ADDR func;
  struct frame_id id;

  if (ubicom32_tdep_debug >= DEBUG_FRAME)
    fprintf_unfiltered (gdb_stdlog, "ubicom32_frame_this_id\n");

  info = ubicom32_parse_prologue (next_frame, this_prologue_cache);

  /* The FUNC is easy.  */
  func = frame_func_unwind (next_frame, NORMAL_FRAME);

#if 0
  /* This is meant to halt the backtrace at "_start".  Make sure we
     don't halt it at a generic dummy frame. */
  if (inside_entry_file (func))
    return;
#endif

  if (!current_machine->frame_chain_valid(next_frame))
    {
      if (frame_relative_level (next_frame) >= 0)
	return;

      info->entry_sp = info->current_sp;
    }

  base = info->entry_sp;
  if (base == 0 && (frame_relative_level (next_frame) >= 0))
    return;

  id = frame_id_build (base, func);

  /* Check that we're not going round in circles with the same frame
     ID (but avoid applying the test to sentinel frames which do go
     round in circles).  Can't use frame_id_eq() as that doesn't yet
     compare the frame's PC value.  */
  if (frame_relative_level (next_frame) >= 0
      && get_frame_type (next_frame) != DUMMY_FRAME
      && frame_id_eq (get_frame_id (next_frame), id))
    {
      if (ubicom32_tdep_debug >= DEBUG_FRAME)
	fprintf_unfiltered (gdb_stdlog, "ubicom32_frame_this_id: circular\n");
      return;
    }

  (*this_id) = id;
}

static void
ubicom32_frame_prev_register (struct frame_info *next_frame,
			  void **this_prologue_cache,
			  int regnum, int *optimizedp,
			  enum lval_type *lvalp, CORE_ADDR *addrp,
			  int *realnump, gdb_byte *bufferp)
{
  struct ubicom32_unwind_cache *info
    = ubicom32_parse_prologue (next_frame, this_prologue_cache);

  if (ubicom32_tdep_debug >= DEBUG_FRAME)
    fprintf_unfiltered (gdb_stdlog, "\nubicom32_frame_prev_register\n\n");
  trad_frame_get_prev_register (next_frame, info->saved_regs, regnum,
				optimizedp, lvalp, addrp, realnump, bufferp);
}

#if 0
/* I we ever figure out how to read regs turn this on. */
/* Given a GDB frame, determine the address of the calling function's
   frame.  This will be used to create a new GDB frame struct.  */
static void
ubicom32_regs_frame_this_id (struct frame_info *next_frame,
			     void **this_prologue_cache,
			     struct frame_id *this_id)
{
  struct ubicom32_unwind_cache *info = (struct ubicom32_unwind_cache *) (*this_prologue_cache);
  CORE_ADDR base;
  CORE_ADDR func;
  struct frame_id id;
  struct value *val;
  int ret, len;
  char *memstr;
  unsigned int reg_data[16];
  struct cleanup *cleanups;
  int i, j;
  CORE_ADDR sp = frame_sp_unwind(next_frame);
  CORE_ADDR pc = frame_pc_unwind(next_frame);

  extern unsigned int ubicom32v3_lookup_symbol (char *name);
  if (ubicom32_tdep_debug >= DEBUG_FRAME)
    {
      fprintf_unfiltered (gdb_stdlog, "ubicom32_regs_frame_this_id\n");
    }

  val = parse_and_eval("regs->pc");

  if (info == NULL)
    {
      info = FRAME_OBSTACK_ZALLOC (struct ubicom32_unwind_cache);
      (*this_prologue_cache) = info;
      info->saved_regs = trad_frame_alloc_saved_regs (next_frame);
    }


  if (!memfile)
    memfile = mem_fileopen ();
  else
    ui_file_rewind (memfile);

  ret = value_print(val, memfile, 'x', Val_pretty_default);
  memstr = ui_file_xstrdup (memfile, &len);
  cleanups = make_cleanup (xfree, memstr);

  sscanf(memstr, "0x%x", &func);

  /* Get the sp from regs structure. */
  val = parse_and_eval("regs->an[7]");
  ui_file_rewind (memfile);

  ret = value_print(val, memfile, 'x', Val_pretty_default);
  memstr = ui_file_xstrdup (memfile, &len);
  cleanups = make_cleanup (xfree, memstr);

  sscanf(memstr, "0x%x", &base);

  id = frame_id_build (base, func);
  *this_id = id;

  /* Extract all the dn registers. */
  val = parse_and_eval("regs->dn");
  ui_file_rewind (memfile);

  ret = value_print(val, memfile, 'x', Val_pretty_default);
  memstr = ui_file_xstrdup (memfile, &len);
  cleanups = make_cleanup (xfree, memstr);

  sscanf(memstr, "{0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x}", 
	 &reg_data[0],
	 &reg_data[1],
	 &reg_data[2],
	 &reg_data[3],
	 &reg_data[4],
	 &reg_data[5],
	 &reg_data[6],
	 &reg_data[7],
	 &reg_data[8],
	 &reg_data[9],
	 &reg_data[10],
	 &reg_data[11],
	 &reg_data[12],
	 &reg_data[13],
	 &reg_data[14],
	 &reg_data[15]);

  /* Load all the data info storage area. */
  for (i = 0; i < 16; i++)
    {
      trad_frame_set_value (info->saved_regs, i, reg_data[i]);
    }

  /* Extract all the an registers. */
  val = parse_and_eval("regs->an");
  ui_file_rewind (memfile);

  ret = value_print(val, memfile, 'x', Val_pretty_default);
  memstr = ui_file_xstrdup (memfile, &len);
  cleanups = make_cleanup (xfree, memstr);

  sscanf(memstr, "{0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x}", 
	 &reg_data[0],
	 &reg_data[1],
	 &reg_data[2],
	 &reg_data[3],
	 &reg_data[4],
	 &reg_data[5],
	 &reg_data[6],
	 &reg_data[7]);

  /* Load an registers into frame. */
  for(i=0, j = 16; i< 8; i++, j++)
    {
      trad_frame_set_value (info->saved_regs, j, reg_data[i]);
    }

  /* Deal with PC. */
  trad_frame_set_value (info->saved_regs, current_machine->pc_regnum, func);
}

static void
ubicom32_regs_frame_prev_register (struct frame_info *next_frame,
			  void **this_prologue_cache,
			  int regnum, int *optimizedp,
			  enum lval_type *lvalp, CORE_ADDR *addrp,
			  int *realnump, gdb_byte *bufferp)
{
  struct ubicom32_unwind_cache *info = (struct ubicom32_unwind_cache *) (*this_prologue_cache);

  if (ubicom32_tdep_debug >= DEBUG_FRAME)
    fprintf_unfiltered (gdb_stdlog, "\nubicom32_regs_frame_prev_register\n\n");

  trad_frame_get_prev_register (next_frame, info->saved_regs, regnum,
				optimizedp, lvalp, addrp, realnump, bufferp);
}
#endif

/* Given a GDB frame, determine the address of the calling function's
   frame.  This will be used to create a new GDB frame struct.  */
static void
ubicom32_sig_frame_this_id (struct frame_info *next_frame,
			     void **this_prologue_cache,
			     struct frame_id *this_id)
{
  struct ubicom32_unwind_cache *info = (struct ubicom32_unwind_cache *) (*this_prologue_cache);
  CORE_ADDR sp = frame_sp_unwind(next_frame);
  CORE_ADDR base;
  CORE_ADDR func;
  struct frame_id id;
  int ret, len;
  unsigned int reg_data[40];
  int i, j;
  int elf_flags = elf_elfheader(exec_bfd)->e_flags;

  /*
   * Adjust sp beyond the signal_trampoline code.
   */
  sp += (8*4);

  /*
   * Read back 40 words from the stack.
   */
  for(base = sp, i = 0; i < 40; i++, base += 4)
    {
      reg_data[i] = read_memory_integer(base, 4);
    }

  if ((elf_flags & 0xffff) > 2)
    {
      /* Read UCSR and replace the CSR value with it. */
      int ucsr_data = read_memory_integer ((sp + UBICOM32_PT_REGS_UCSR * 4), 4);

      reg_data[UBICOM32_PT_REGS_CSR] = ucsr_data;
    }

  if (ubicom32_tdep_debug >= DEBUG_FRAME)
    {
      fprintf_unfiltered (gdb_stdlog, "ubicom32_sig_frame_this_id\n");
    }
  if (info == NULL)
    {
      info = FRAME_OBSTACK_ZALLOC (struct ubicom32_unwind_cache);
      (*this_prologue_cache) = info;
      info->saved_regs = trad_frame_alloc_saved_regs (next_frame);
    }

  /*
   * Supply registers from D0 to PC.
   */
  for (i = 0; i <= UBICOM32_PT_REGS_PC ; i++)
    {
      trad_frame_set_value (info->saved_regs, i, reg_data[i]);
    }

  /*
   * Supply previous PC.
   */
  trad_frame_set_value (info->saved_regs, UBICOM32_FDPIC_PREV_PC_REGNUM, reg_data[UBICOM32_PT_REGS_PREVIOUS_PC]);

  func = reg_data[UBICOM32_PT_REGS_PC];
  base = reg_data[UBICOM32_PT_REGS_SP];

  id = frame_id_build (base, func);
  *this_id = id;
}

static void
ubicom32_sig_frame_prev_register (struct frame_info *next_frame,
			  void **this_prologue_cache,
			  int regnum, int *optimizedp,
			  enum lval_type *lvalp, CORE_ADDR *addrp,
			  int *realnump, gdb_byte *bufferp)
{
  struct ubicom32_unwind_cache *info = (struct ubicom32_unwind_cache *) (*this_prologue_cache);

  if (ubicom32_tdep_debug >= DEBUG_FRAME)
    fprintf_unfiltered (gdb_stdlog, "\nubicom32_sig_frame_prev_register\n\n");

  trad_frame_get_prev_register (next_frame, info->saved_regs, regnum,
				optimizedp, lvalp, addrp, realnump, bufferp);
}

static CORE_ADDR
ubicom32_frame_base_address (struct frame_info *next_frame, void **this_cache)
{
  struct ubicom32_unwind_cache *info;

  if (ubicom32_tdep_debug >= DEBUG_FRAME)
    fprintf_unfiltered (gdb_stdlog, "ubicom32_frame_base_address\n");

  info = ubicom32_parse_prologue (next_frame, this_cache);
  /* Note that gcc provides all debug information relative to the
     post-locals SP, not the entry_sp!  */
  return info->entry_sp;
}

static const struct frame_unwind ubicom32_frame_unwind = {
  NORMAL_FRAME,
  ubicom32_frame_this_id,
  ubicom32_frame_prev_register
};

#if 0
/* I we ever figure out how to read regs turn this on. */
static const struct frame_unwind ubicom32_regs_frame_unwind = {
  NORMAL_FRAME,
  ubicom32_regs_frame_this_id,
  ubicom32_regs_frame_prev_register
};
#endif

static const struct frame_unwind ubicom32_sig_frame_unwind = {
  SIGTRAMP_FRAME,
  ubicom32_sig_frame_this_id,
  ubicom32_sig_frame_prev_register
};

static const struct frame_base ubicom32_frame_base = {
  &ubicom32_frame_unwind,
  ubicom32_frame_base_address,
  ubicom32_frame_base_address,
  ubicom32_frame_base_address
};

static int
ubicom32_has_regs_frame (struct frame_info *fi)
{
  int line;
  int offset;
  int unmapped;
  char *filename = NULL;
  char *name = NULL;
  CORE_ADDR pc = frame_pc_unwind(fi);
  CORE_ADDR sp = frame_sp_unwind(fi);
  struct cleanup *cleanups;

  /* check if the pc is valid at all */
  if (!build_address_symbolic (pc, 0, &name, &offset, &filename, &line, &unmapped))
    {
      /* we got a hit */
      if(name)
	{
	  if(strstr(name, "do_trap"))
	    {
	      return 1;
	    }
	  else if(strstr(name, "do_IRQ"))
	    {
	      return 1;
	    }
	  else if(strstr(name, "do_page_fault"))
	    {
	      return 1;
	    }
	}
    }

  return 0;
}

/*
 * If there is ever a changes in arch/ubicom32/includ/asm/unistd.h to _NR_rt_sigreturn then this
 * define must be changed also.
 */
#define __NR_rt_sigreturn	173

static int sigtramp_code[2] = { 0x01247103, 0xf0a00080};

static int
ubicom32_is_sig_frame (struct frame_info *fi)
{
  CORE_ADDR pc = frame_pc_unwind(fi);
  CORE_ADDR sp = frame_sp_unwind(fi);
  int stack_data;

  stack_data = read_memory_integer(sp, 4);
  if (stack_data != __NR_rt_sigreturn)
    return 0;

  stack_data = read_memory_integer(sp + 4, 4);
  if (stack_data != 0 && stack_data != 1)
    return 0;

  stack_data = read_memory_integer(sp + 8, 4);
  if (stack_data != sigtramp_code[0])
    return 0;

  stack_data = read_memory_integer(sp + 12, 4);
  if (stack_data != sigtramp_code[1])
    return 0;

  return 1;
}

static const struct frame_unwind *
ubicom32_frame_base_sniffer (struct frame_info *next_frame)
{
  const struct frame_unwind *fm;

  if (exec_bfd && (elf_elfheader(exec_bfd)->e_flags & EF_UBICOM32_FDPIC))
    {
      if (ubicom32_is_sig_frame(next_frame))
	{
	  return &ubicom32_sig_frame_unwind;
	}
    }

#if 0
  /* I we ever figure out how to get hold of regs turn this on. */
  else if(ubicom32_has_regs_frame(next_frame))
    {
      printf("Needs regs frames\n");
      return &ubicom32_regs_frame_unwind;
    }
#endif

  fm = dwarf2_frame_sniffer(next_frame);
  if (fm)
    return fm;
  else
    return &ubicom32_frame_unwind;
}

#define NO_PT_REG_MAP 	-1
/* Map gdb register number to position in pt_regs.   IP8K only.  */
static int pt_reg_map[] =
{
  /* UBICOM32_D0_REGNUM =   0 */  UBICOM32_PT_REGS_D0,
  /* UBICOM32_D1_REGNUM =   1 */  UBICOM32_PT_REGS_D1,
  /* UBICOM32_D2_REGNUM =   2 */  UBICOM32_PT_REGS_D2,
  /* UBICOM32_D3_REGNUM =   3 */  UBICOM32_PT_REGS_D3,
  /* UBICOM32_D4_REGNUM =   4 */  UBICOM32_PT_REGS_D4,
  /* UBICOM32_D5_REGNUM =   5 */  UBICOM32_PT_REGS_D5,
  /* UBICOM32_D6_REGNUM =   6 */  UBICOM32_PT_REGS_D6,
  /* UBICOM32_D7_REGNUM =   7 */  UBICOM32_PT_REGS_D7,
  /* UBICOM32_D8_REGNUM =   8 */  UBICOM32_PT_REGS_D8,
  /* UBICOM32_D9_REGNUM =   9 */  UBICOM32_PT_REGS_D9,
  /* UBICOM32_D10_REGNUM = 10 */  UBICOM32_PT_REGS_D10,
  /* UBICOM32_D11_REGNUM = 11 */  UBICOM32_PT_REGS_D11,
  /* UBICOM32_D12_REGNUM = 12 */  UBICOM32_PT_REGS_D12,
  /* UBICOM32_D13_REGNUM = 13 */  UBICOM32_PT_REGS_D13,
  /* UBICOM32_D14_REGNUM = 14 */  UBICOM32_PT_REGS_D14,
  /* UBICOM32_D15_REGNUM = 15 */  UBICOM32_PT_REGS_D15,
  /* UBICOM32_A0_REGNUM =  16 */  UBICOM32_PT_REGS_A0,
  /* UBICOM32_A1_REGNUM =  17 */  UBICOM32_PT_REGS_A1,
  /* UBICOM32_A2_REGNUM =  18 */  UBICOM32_PT_REGS_A2,
  /* UBICOM32_A3_REGNUM =  19 */  UBICOM32_PT_REGS_A3,
  /* UBICOM32_A4_REGNUM =  20 */  UBICOM32_PT_REGS_A4,
  /* UBICOM32_A5_REGNUM =  21 */  UBICOM32_PT_REGS_A5,
  /* UBICOM32_A6_REGNUM =  22 */  UBICOM32_PT_REGS_A6,
  /* UBICOM32_A7_REGNUM =  23 */  UBICOM32_PT_REGS_A7,
  /* acc0_hi 		= 24 */ UBICOM32_PT_REGS_ACC0HI,
  /* acc0_lo 		= 25 */ UBICOM32_PT_REGS_ACC0LO,
  /* mac_rc16 		= 26 */ UBICOM32_PT_REGS_MAC_RC16,
  /* source_3 		= 27 */ UBICOM32_PT_REGS_SOURCE3,
  /* context_cnt 	= 28 */ NO_PT_REG_MAP,
  /* csr 		= 29 */ UBICOM32_PT_REGS_CSR,
  /* UBICOM32_ROSR_REGNUM = 30 */ NO_PT_REG_MAP,
  /* iread_data 	= 31 */ NO_PT_REG_MAP,
  /* int_mask0 		= 32 */ UBICOM32_PT_REGS_INT_MASK0,
  /* int_mask1 		= 33 */ UBICOM32_PT_REGS_INT_MASK1,
  /* 			= 34 */ NO_PT_REG_MAP,
  /* UBICOM32V5_PC_REGNUM = 35 */ UBICOM32_PT_REGS_PC,
  /* trap_cause 	= 36 */ NO_PT_REG_MAP,
  /* acc1_hi 		= 37 */ UBICOM32_PT_REGS_ACC1HI,
  /* acc1_lo 		= 38 */ UBICOM32_PT_REGS_ACC1LO,
  /* previous_pc 	= 39 */ UBICOM32_PT_REGS_PREVIOUS_PC,
  /* ucsr 		= 40 */ UBICOM32_PT_REGS_UCSR,
  /* int_clr1 		= 41 */ NO_PT_REG_MAP,
  /* global_ctrl 	= 42 */ NO_PT_REG_MAP,
  /* mt_active_set 	= 43 */ NO_PT_REG_MAP,
  /* mt_active_clr 	= 44 */ NO_PT_REG_MAP,
  /* UBICOM32_MT_ACTIVE_REGNUM	= 45 */ NO_PT_REG_MAP,
  /* global_ctrl 	= 46 */ NO_PT_REG_MAP,
  /* mt_active_set 	= 47 */ NO_PT_REG_MAP,
  /* mt_active_clr 	= 48 */ NO_PT_REG_MAP,
  /* mt_dbg_active 	= 49 */ NO_PT_REG_MAP,
  /* mt_dbg_active_set 	= 50 */ NO_PT_REG_MAP,
  /* mt_dbg_active_set 	= 51 */ NO_PT_REG_MAP,
  /* UBICOM32V3_MT_EN_REGNUM = 52 */ NO_PT_REG_MAP,
  /* UBICOM32V3_MT_PRI_REGNUM = 53 */ NO_PT_REG_MAP,
  /* UBICOM32V3_MT_SCHED_REGNUM = 54 */ NO_PT_REG_MAP,
  /* mt_break 		= 55 */ NO_PT_REG_MAP,
  /* mt_break_clr 	= 56 */ NO_PT_REG_MAP,
  /* mt_single_step 	= 57 */ NO_PT_REG_MAP,
  /* mt_min_del_en 	= 58 */ NO_PT_REG_MAP,
  /* mt_break_set 	= 59 */ NO_PT_REG_MAP,
  /* dcapt 		= 60 */ NO_PT_REG_MAP,
  /* mt_dbg_active_clr 	= 61 */ NO_PT_REG_MAP,
  /* scratchpad0 	= 62 */ NO_PT_REG_MAP,
  /* scratchpad1 	= 63 */ NO_PT_REG_MAP,
  /* scratchpad2 	= 64 */ NO_PT_REG_MAP,
  /* scratchpad3 	= 65 */ NO_PT_REG_MAP,
  /* mt_dbg_active_clr 	= 66 */ NO_PT_REG_MAP,
  /* mt_i_blocked 	= 67 */ NO_PT_REG_MAP,
  /* mt_d_blocked 	= 68 */ NO_PT_REG_MAP,
  /* mt_i_blocked_set 	= 69 */ NO_PT_REG_MAP,
  /* mt_d_blocked_set 	= 70 */ NO_PT_REG_MAP,
  /* mt_blocked_clr 	= 71 */ NO_PT_REG_MAP,
  /* mt_trap_en 	= 72 */ NO_PT_REG_MAP,
  /* mt_trap 		= 73 */ NO_PT_REG_MAP,
  /* chip_cfg 		= 74 */ NO_PT_REG_MAP,
  /* mt_i_blocked 	= 75 */ NO_PT_REG_MAP,
  /* mt_d_blocked 	= 76 */ NO_PT_REG_MAP,
  /* mt_i_blocked_set 	= 77 */ NO_PT_REG_MAP,
  /* mt_d_blocked_set 	= 78 */ NO_PT_REG_MAP,
  /* mt_blocked_clr 	= 79 */ NO_PT_REG_MAP,
  /* mt_trap_en 	= 80 */ NO_PT_REG_MAP,
  /* mt_trap 		= 81 */ NO_PT_REG_MAP,
  /* mt_trap_set 	= 82 */ NO_PT_REG_MAP,
  /* mt_trap_clr 	= 83 */ NO_PT_REG_MAP,
  /* sep 		= 84 */ NO_PT_REG_MAP,
  /* i_range0_hi 	= 85 */ NO_PT_REG_MAP,
  /* i_range1_hi 	= 86 */ NO_PT_REG_MAP,
  /* i_range2_hi 	= 87 */ NO_PT_REG_MAP,
  /* i_range3_hi 	= 88 */ NO_PT_REG_MAP,
  /* i_range0_lo 	= 89 */ NO_PT_REG_MAP,
  /* i_range1_lo 	= 90 */ NO_PT_REG_MAP,
  /* i_range2_lo 	= 91 */ NO_PT_REG_MAP,
  /* i_range3_lo 	= 92 */ NO_PT_REG_MAP,
  /* i_range0_en 	= 93 */ NO_PT_REG_MAP,
  /* i_range1_en 	= 94 */ NO_PT_REG_MAP,
  /* i_range2_en 	= 95 */ NO_PT_REG_MAP,
  /* i_range3_en 	= 96 */ NO_PT_REG_MAP,
  /* d_range0_hi 	= 97 */ NO_PT_REG_MAP,
  /* d_range1_hi 	= 98 */ NO_PT_REG_MAP,
  /* d_range2_hi 	= 99 */ NO_PT_REG_MAP,
  /* d_range3_hi 	= 100 */ NO_PT_REG_MAP,
  /* d_range4_hi 	= 101 */ NO_PT_REG_MAP,
  /* d_range5_hi 	= 102 */ NO_PT_REG_MAP,
  /* d_range0_lo 	= 103 */ NO_PT_REG_MAP,
  /* d_range1_lo 	= 104 */ NO_PT_REG_MAP,
  /* d_range2_lo 	= 105 */ NO_PT_REG_MAP,
  /* d_range3_lo 	= 106 */ NO_PT_REG_MAP,
  /* d_range4_lo 	= 107 */ NO_PT_REG_MAP,
  /* d_range5_lo 	= 108 */ NO_PT_REG_MAP,
  /* d_range0_en 	= 109 */ NO_PT_REG_MAP,
  /* d_range1_en 	= 110 */ NO_PT_REG_MAP,
  /* d_range2_en 	= 111 */ NO_PT_REG_MAP,
  /* d_range3_en 	= 112 */ NO_PT_REG_MAP,
  /* d_range4_en 	= 113 */ NO_PT_REG_MAP,
  /* d_range5_en 	= 114 */ NO_PT_REG_MAP,
  /* i_range0_user_en 	= 115 */ NO_PT_REG_MAP,
  /* i_range1_user_en 	= 116 */ NO_PT_REG_MAP,
  /* i_range2_user_en 	= 117 */ NO_PT_REG_MAP,
  /* i_range3_user_en 	= 118 */ NO_PT_REG_MAP,
  /* d_range0_user_en 	= 119 */ NO_PT_REG_MAP,
  /* d_range1_user_en 	= 120 */ NO_PT_REG_MAP,
  /* d_range2_user_en 	= 121 */ NO_PT_REG_MAP,
  /* d_range3_user_en 	= 122 */ NO_PT_REG_MAP,
  /* d_range4_user_en 	= 123 */ NO_PT_REG_MAP,
  /* d_range5_user_en 	= 124 */ NO_PT_REG_MAP,
  /* mt_fp_flush 	= 125 */ NO_PT_REG_MAP,
  /* mt_fp_blocked 	= 126 */ NO_PT_REG_MAP,
  /* mt_fp_blocked_set 	= 127 */ NO_PT_REG_MAP,
  /* mt_btb_en         	= 128 */ NO_PT_REG_MAP,
  /* btb_ctrl          	= 129 */ NO_PT_REG_MAP,
};

static void
ubicom32_kernel_prev_register (struct frame_info *next_frame,
			  void **this_prologue_cache,
			  int regnum, int *optimizedp,
			  enum lval_type *lvalp, CORE_ADDR *addrp,
			  int *realnump, gdb_byte *bufferp)
{
  struct ubicom32_unwind_cache *info;
  CORE_ADDR pt_regs_addr;
  unsigned int value;

  info = ubicom32_parse_prologue (next_frame, this_prologue_cache);
  /* pt_regs structure is pushed on the stack.  Stack pointer at entry
     points to the start of the struct.  */
  pt_regs_addr = info->entry_sp;

  if (pt_reg_map[regnum] != NO_PT_REG_MAP)
    {
      value = read_memory_integer (pt_regs_addr + pt_reg_map[regnum] * 4, 4);
      *optimizedp = 0;
      *lvalp = not_lval;
      *addrp = 0;
      *realnump = -1;
      if (bufferp != NULL)
        store_unsigned_integer (bufferp, 4, value);
    }
  else 
    ubicom32_frame_prev_register (next_frame, this_prologue_cache, regnum, 
				  optimizedp, lvalp, addrp, realnump, bufferp);
}

static const struct frame_unwind ubicom32_kernel_unwind = {
  NORMAL_FRAME,
  ubicom32_frame_this_id,
  ubicom32_kernel_prev_register
};

struct kernel_address
{
  CORE_ADDR start;
  CORE_ADDR end;
};
static struct kernel_address *kernel_address_list;

static char *kernel_function_list[] = {"do_IRQ", "do_trap", "do_page_fault", NULL};
 
static void
init_kernel_function_list (void)
{
  int count;
  struct minimal_symbol *msym;

  for (count = 0; kernel_function_list[count]; count++) 
   ;

  kernel_address_list = (struct kernel_address *)xmalloc ((count+1) * sizeof (struct kernel_address));
  kernel_address_list[count].start = NULL;
  for (count = 0; kernel_function_list[count]; count++)
    {
      msym = lookup_minimal_symbol (kernel_function_list[count], NULL, NULL);
      if (msym != NULL) {
        kernel_address_list[count].start = SYMBOL_VALUE (msym);
        kernel_address_list[count].end = SYMBOL_VALUE (msym) + msym->size;
      }
    }
}

static const struct frame_unwind *
ubicom32_kernel_sniffer (struct frame_info *next_frame)
{
  struct kernel_address *paddr;
  CORE_ADDR pc = frame_pc_unwind (next_frame);

  if (pc == NULL)
    return NULL;

  if (kernel_address_list == NULL)
    init_kernel_function_list ();
  
  for (paddr = kernel_address_list; paddr->start; paddr++)
    {

      if (pc >= paddr->start && pc < paddr->end)
	{
	  return &ubicom32_kernel_unwind;
	}
    }
  
  return NULL;
}

/* ubicom32_unwind_pc.  */
static CORE_ADDR
ubicom32_unwind_pc (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  ULONGEST pc;
  if (ubicom32_tdep_debug >= DEBUG_FRAME)
    fprintf_unfiltered (gdb_stdlog, "ubicom32_unwind_pc\n");

  //frame_unwind_unsigned_register (next_frame, current_machine->pc_regnum, &pc);
  pc = frame_unwind_register_unsigned (next_frame, current_machine->pc_regnum);
  return (pc);
}

/* ubicom32_unwind_sp.  */
static CORE_ADDR
ubicom32_unwind_sp (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  ULONGEST sp;
  if (ubicom32_tdep_debug >= DEBUG_FRAME)
    fprintf_unfiltered (gdb_stdlog, "ubicom32_unwind_sp\n");

  //frame_unwind_unsigned_register (next_frame, current_machine->sp_regnum, &sp);
  sp = frame_unwind_register_unsigned (next_frame, current_machine->sp_regnum);
  return (sp);
}

/* Return PTID's program counter as an internal GDB address.  */

static CORE_ADDR
ubicom32_read_pc (struct regcache *regcache)
{
  CORE_ADDR pc;
  char tmp, *ptr;

  if (ubicom32_tdep_debug >= DEBUG_FRAME)
    fprintf_unfiltered (gdb_stdlog, "ubicom32_read_pc\n");

  regcache_raw_read(regcache, current_machine->pc_regnum, (gdb_byte *)&pc);

  /* we have to szizzle the data. */
  ptr = (char *)&pc;
  tmp = ptr[3];
  ptr[3] = ptr[0];
  ptr[0] = tmp;
  tmp = ptr[1];
  ptr[1] = ptr[2];
  ptr[2] = tmp;
  return pc;
}

static void
ubicom32_write_pc (struct regcache *regcache, CORE_ADDR pc)
{
  char tmp, *ptr;
  CORE_ADDR tmp1;
  /* we have to szizzle the data. */
  ptr = (char *)&pc;
  tmp = ptr[3];
  ptr[3] = ptr[0];
  ptr[0] = tmp;
  tmp = ptr[1];
  ptr[1] = ptr[2];
  ptr[2] = tmp;

  tmp1 = REAL_MEM(pc);
  regcache_raw_write (regcache, current_machine->pc_regnum, (gdb_byte *)&tmp1);
}

/* Print the TYPE value in BUF using val_print() FORMAT, right-justifying to
   WIDTH characters.  */

static void
display_val (struct type *type, char *buf, int format, int width,
	     struct ui_file *file)
{
  char *memstr;
  int len;
  struct cleanup *cleanups;

  /* Clear the buffer passed to val_print(), creating it first if needed.  */
  if (!memfile)
    memfile = mem_fileopen ();
  else
    ui_file_rewind (memfile);

  val_print (type, buf, 0, 0, memfile, format, 1, 0, Val_pretty_default);

  memstr = ui_file_xstrdup (memfile, &len);
  cleanups = make_cleanup (xfree, memstr);

  fprintf_filtered (file, "%*s", width, memstr);
  do_cleanups (cleanups);
}

/* Return the internal GDB address corresponding to the real TYPE address
   in BUF.  */

static CORE_ADDR
ubicom32_pointer_to_address (struct type *type, const gdb_byte *buf)
{
  CORE_ADDR addr;
  enum type_code code;

  addr = extract_unsigned_integer (buf, TYPE_LENGTH (type));

  code = TYPE_CODE (TYPE_TARGET_TYPE (type));
  if (code == TYPE_CODE_FUNC || code == TYPE_CODE_METHOD)
    addr = GDB_IMEM (addr);
  else
    addr = GDB_DMEM (addr);

  return addr;
}

/* Store in BUF the real TYPE address corresponding to internal GDB address
   ADDR.  */

static void
ubicom32_address_to_pointer (struct type *type, gdb_byte *buf, CORE_ADDR addr)
{
  store_unsigned_integer (buf, TYPE_LENGTH (type), REAL_MEM (addr));
}

/* Given an arbitrary value, convert it to an integer and hence into
   an address.  */

static CORE_ADDR
ubicom32_integer_to_address (struct gdbarch *gdbarch, struct type *type, const gdb_byte *buf)
{
  LONGEST i = unpack_long (type, buf);
  if (i == GDB_IMEM (i) || i == GDB_DMEM (i))
    /* Is the value already in one of the special address spaces? Yes,
       just return it as is.  */
    return i;
  else
    /* Assume the value is ment to live in data space.  If it isn't
       then the user should have explicitly cast it some how.  */
    return GDB_DMEM (i);
}

/* Extract a TYPE return value from raw register array REGBUF, and copy it
   into VALBUF in virtual format. */

static void
ubicom32_extract_return_value (struct type *type, struct regcache *rcache, gdb_byte *valbuf)
{
  int offset;
  int len = TYPE_LENGTH (type);
  unsigned char regbuf[UBICOM32_REG_SIZE *2];
  ULONGEST val;

  gdb_assert (len <= 8);
  /* read out UBICOM32_RET_REGNUM */
  regcache_cooked_read_unsigned (rcache, UBICOM32_RET_REGNUM, &val);
  store_unsigned_integer (valbuf, UBICOM32_REG_SIZE, val);

  if (len > 4) 
    {
      regcache_cooked_read_unsigned (rcache, UBICOM32_RET2_REGNUM, &val);
      store_unsigned_integer (valbuf + UBICOM32_REG_SIZE, UBICOM32_REG_SIZE, val);
    }
}

/* Store virtual-format VALBUF as a TYPE return value so that it will be
   returned if the current function returns now.  */

static void
ubicom32_store_return_value (struct type *type, struct regcache *regcache, const gdb_byte *valbuf)
{
  regcache_raw_write (regcache, UBICOM32_RET_REGNUM, valbuf);

  if (TYPE_LENGTH (type) == 8)
    regcache_raw_write (regcache, UBICOM32_RET2_REGNUM, ((char *)valbuf + UBICOM32_REG_SIZE));
}

/* Handle the ubicom32 return value convention.  */

static enum return_value_convention
ubicom32_return_value (struct gdbarch *gdbarch, struct type *type,
		       struct regcache *regcache, gdb_byte *readbuf,
		       const gdb_byte *writebuf)
{
  if (TYPE_LENGTH (type) > 8)
    /* Anything larger than 8 bytes (2 registers) is returned on the stack.  */
    return RETURN_VALUE_STRUCT_CONVENTION;

  if (readbuf)
    ubicom32_extract_return_value (type, regcache, readbuf);
  if (writebuf)
    ubicom32_store_return_value (type, regcache, writebuf);

  return RETURN_VALUE_REGISTER_CONVENTION;
}

/* Return an instruction to set a breakpoint at PCPTR, adjusting PCPTR if
   necessary, and store in LENPTR the size of the returned instruction.  */

static const unsigned char *
ubicom32_breakpoint_from_pc (struct gdbarch *gdbarch,
			     CORE_ADDR *pcptr, int *lenptr)
{
  /* BKPT instruction.  */
  static unsigned char breakpoint[] = { 0x00, 0x00, 0x38, 0x00 };

  /* REAL_MEM here is a temporary hack to make Z-packets work with SID.
     After that gets fixed, also revert the BREAKPOINT_FROM_PC move in
     remote.c.  */
  *pcptr = REAL_MEM (*pcptr);
  *lenptr = sizeof breakpoint;
  return breakpoint;
}

/* Return an instruction to set a breakpoint at PCPTR, adjusting PCPTR if
   necessary, and store in LENPTR the size of the returned instruction.  */

static const unsigned char *
ubicom32_software_breakpoint_from_pc (struct gdbarch *gdbarch,
				      CORE_ADDR *pcptr, int *lenptr)
{
  /* Software Breakpoint 'instruction' (not actually a valid
   * instruction). Note that this used to be 0xaabbccdd, but in the mach
   * v4 this now actually legal.
   */
  static unsigned char breakpoint[] = { 0xfa, 0xbb, 0xcc, 0xdd };

  /* REAL_MEM here is a temporary hack to make Z-packets work with SID.
     After that gets fixed, also revert the BREAKPOINT_FROM_PC move in
     remote.c.  */
  *pcptr = REAL_MEM (*pcptr);
  *lenptr = sizeof breakpoint;
  return breakpoint;
}

/* ubicom32_software_single_step() is called just before we want to
   resume the inferior, if we want to single-step it but there is no
   hardware or kernel single-step support (ubicom32uclinux on GNU/Linux
   for example).  We find the target of the coming instruction and
   breakpoint it.  */

int
ubicom32_software_single_step (struct frame_info *frame)
{
  /* The following is a derivative of ubicom32_posix_setup_single_step */
  gdb_byte cptr[4];
  unsigned int insn;
  unsigned char base_opcode;
  CORE_ADDR pc = get_frame_pc (frame);
  CORE_ADDR target1_pc = pc + 4; /* reasonable default */

  /* pull the current instruction from target. */
  int status = read_memory_nobpt (pc, cptr, 4);
  if (status) {
    memory_error (status, pc);
    return -1;
  };

  insn = cptr[0] << 24 | cptr[1] << 16 | cptr[2] << 8 | cptr[3];
  base_opcode = (cptr[0] >> 3) & 0x1f;

  /* analyze the instruction */
  switch(base_opcode)
    {
    case 0x1a:
      {
	/* jump conditionals */
	int offset = (insn & 0x1fffff)<<2;

	if (offset & 0x00400000)
	    offset |= 0xff800000; /* set up a negative number */

	insert_single_step_breakpoint (pc + 4);
	target1_pc = pc + offset;
	break;
      }
    case 0x1b:
      {
	/* CALL */
	int offset = ((int)(cptr[0] & 0x7))<<23 | ((insn & 0x1fffff)<<2);
	if(offset & 0x02000000)
	  offset |= 0xfc000000;
	target1_pc = pc + offset;
	break;
      }
    case 0x1e:
      {
	/* CALLI. Build the 18 bit offset from the instruction */
	int offset = (((int)(cptr[0] & 0x7))<<15 | ((int)(cptr[1] & 0x1f) <<10) |
		      ((int)(cptr[2] & 0x7) << 7) | ((int)(cptr[3] & 0x1f) << 2));
	int address_register = (cptr[3] >> 5) & 0x7;

	if (offset & (1<<17))
	  offset |= ~((1 << 18) - 1);

	/* Get the base address from the address register in the instruction. */
	target1_pc = get_frame_register_unsigned (frame, UBICOM32_A0_REGNUM + address_register);

	/* add the offset */
	target1_pc += offset;
	break;
      }
    case 0x00:
    {
	int op_extension = (int)(cptr[2] >> 3) & 0x1f;
	if (op_extension == 4)
	  {
	    /* RET. Extract the S1 field from the instruction. */
	    unsigned int ea;
	    int s1 = insn & 0x7ff;
	    int address_register = (s1 & 0xe0) >> 5;
	    int data_register = (s1 & 0xf);
	    int direct_register = (s1 & 0xff);
	    int pre = s1 & 0x10;
	    int indirect_offset = (((s1 & 0x300) >> 3) | (s1 & 0x1f)) << 2;
	    int four_bit_immediate = data_register << 28;
	    int eight_bit_immediate = direct_register << 24;
	    four_bit_immediate = four_bit_immediate >> 26;
	    eight_bit_immediate = eight_bit_immediate >> 22;

	    if ((s1 & 0x700) == (0x1 << 8))
	      {
		/* Direct register */
		if (direct_register < 32)
		  target1_pc = get_frame_register_unsigned (frame, UBICOM32_D0_REGNUM + direct_register);
		else if (direct_register < 40)
		  target1_pc = get_frame_register_unsigned (frame, UBICOM32_A0_REGNUM + direct_register - 32);
		else
		  {
		    printf_unfiltered("Unknown direct register %d\n", direct_register);
		    return -1;
		  }
	      }
	    else if ((s1 & 0x700) == 0)
	      {
		/* immediate mode. */
		printf_unfiltered("Immediate mode is wrong.\n");
		return -1;
	      }
	    else
	      {
		if (s1 & 0x400)
		  {
		    /* Indirect with offset. */
		    target1_pc = indirect_offset;
		    target1_pc += get_frame_register_unsigned (frame, UBICOM32_A0_REGNUM + address_register);
		  }
		else if ((s1 & 0x700) == (0x3 << 8))
		  {
		    /* Indirect with index. */
		    target1_pc =  get_frame_register_unsigned (frame, UBICOM32_D0_REGNUM + data_register) << 2;
		    target1_pc +=  get_frame_register_unsigned (frame, UBICOM32_A0_REGNUM + address_register);
		  }
		else if ((s1 & 0x700) == (0x2 << 8))
		  {
		    /* Indirect with pre/post increment .*/
		    target1_pc = get_frame_register_unsigned (frame, UBICOM32_A0_REGNUM + address_register);

		    if (pre)
		      target1_pc += four_bit_immediate;
		  }

		/* read the memory we have computed above. It is the return address. */
		status = read_memory_nobpt (target1_pc, cptr, 4);
		if (status) {
			memory_error (status, pc);
			return -1;
		}

		target1_pc = (cptr[0] <<24 | cptr[1] << 16 | cptr[2] << 8 | cptr[3]);
	      }
	  }
	else
	  {
	    target1_pc = pc+4;
	  }
	break;
      }
    default:
	    target1_pc = pc+4;
	    break;
    }

  insert_single_step_breakpoint (target1_pc);

  return 1;
}

static CORE_ADDR
find_func_descr (struct gdbarch *gdbarch, CORE_ADDR entry_point)
{
  CORE_ADDR descr;
  char valbuf[4];
  CORE_ADDR start_addr;

  /* If we can't find the function in the symbol table, then we assume
     that the function address is already in descriptor form.  */
  if (!find_pc_partial_function (entry_point, NULL, &start_addr, NULL)
      || entry_point != start_addr)
    return entry_point;

  descr = ubicom32_fdpic_find_canonical_descriptor (entry_point);

  if (descr != 0)
    return descr;

  /* Construct a non-canonical descriptor from space allocated on
     the stack.  */

  descr = value_as_long (value_allocate_space_in_inferior (8));
  store_unsigned_integer (valbuf, 4, entry_point);
  write_memory (descr, valbuf, 4);
  store_unsigned_integer (valbuf, 4,
			  ubicom32_fdpic_find_global_pointer (entry_point));
  write_memory (descr + 4, valbuf, 4);
  return descr;
}

static CORE_ADDR
ubicom32_convert_from_func_ptr_addr (struct gdbarch *gdbarch, CORE_ADDR addr,
				struct target_ops *targ)
{
  CORE_ADDR entry_point;
  CORE_ADDR got_address;

  entry_point = get_target_memory_unsigned (targ, addr, 4);
  got_address = get_target_memory_unsigned (targ, addr + 4, 4);

  if (got_address == ubicom32_fdpic_find_global_pointer (entry_point))
    return entry_point;
  else
    return addr;
}

/* Return a static string describing the state of thread TP->ptid.  */

char *
ubicom32_remote_threads_extra_info (struct thread_info *tp)
{
  static struct ui_file *buf = NULL;
  static char *bufcpy = NULL;
  int bit, mt_active, mt_pri, mt_sched, mt_en;
  int buflen;
  char mybuffer[1000];
  char *printBuf = mybuffer;
  char *buffer = mybuffer;

  unsigned tid;

  if (!buf)
    buf = mem_fileopen ();
  else
    ui_file_rewind (buf);

  mybuffer[0] = 0;
  bit = 1 << PTID_TO_TNUM (tp->ptid);
  tid = PIDGET(tp->ptid);
  tid --;
  mt_active = read_register (UBICOM32_MT_ACTIVE_REGNUM);
  mt_en = read_register(UBICOM32_MT_EN_REGNUM);
  mt_pri = read_register(UBICOM32_MT_PRI_REGNUM);
  mt_sched = read_register (UBICOM32_MT_SCHED_REGNUM);

  if(0<= tid && tid <= 32)
    {
      /* query about a Main processor thread */
      unsigned int mask = 1<<tid;

      if(!(mt_en & mask))
	{
	  // Not even enabled
	  sprintf(buffer, "%s", "Disabled");
	  buffer += strlen(buffer);
	}
      else
	{
	  if(!(mt_active & mask))
	    {
	      sprintf(buffer, "%s", "Suspended");
	      buffer += strlen(buffer);
	    }

	  if(mt_sched &mask)
	    {
	      //HRT
	      if(strlen(printBuf))
		{
		  sprintf(buffer, ", ");
		  buffer += strlen(buffer);
		}
	      sprintf(buffer, "HRT ");
	      buffer += strlen(buffer);
	    }
	  else
	    {
	      if(strlen(printBuf))
		{
		  sprintf(buffer, ", ");
		  buffer += strlen(buffer);
		}

	      if(!(mt_pri &mask))
		sprintf(buffer, "low priority");
	      else
		sprintf(buffer, "High priority");

	      buffer += strlen(buffer);
	    }
	}
    }
  else if(tid == 32)
    {
      sprintf(buffer, "Coprocessor");
    }

  fputs_unfiltered(printBuf, buf);

  if (bufcpy)
    free (bufcpy);
  bufcpy = ui_file_xstrdup (buf, &buflen);

  if (!buflen)
    return NULL;
  return bufcpy;
}

/*************************** 5K related.******************************************************/

struct ubicom32_reg ubicom32v3_regs[] = {
  /* General data.  */
  { "d0", 0, 4, &builtin_type_int32, 0x0, 0 },
  { "d1", 1, 4, &builtin_type_int32, UBICOM32_D1_REGADDR, 0 },
  { "d2", 2, 4, &builtin_type_int32, 0x8, 0 },
  { "d3", 3, 4, &builtin_type_int32, 0xc, 0 },
  { "d4", 4, 4, &builtin_type_int32, 0x10, 0 },
  { "d5", 5, 4, &builtin_type_int32, 0x14, 0 },
  { "d6", 6, 4, &builtin_type_int32, 0x18, 0 },
  { "d7", 7, 4, &builtin_type_int32, 0x1c, 0 },
  { "d8", 8, 4, &builtin_type_int32, UBICOM32_D8_REGADDR, 0 },
  { "d9", 9, 4, &builtin_type_int32, 0x24, 0 },
  { "d10", 10, 4, &builtin_type_int32, 0x28, 0 },
  { "d11", 11, 4, &builtin_type_int32, 0x2c, 0 },
  { "d12", 12, 4, &builtin_type_int32, 0x30, 0 },
  { "d13", 13, 4, &builtin_type_int32, 0x34, 0 },
  { "d14", 14, 4, &builtin_type_int32, 0x38, 0 },
  { "d15", 15, 4, &builtin_type_int32, 0x3c, 0 },

  /* 0x40 - 0x7f Reserved */

 /* General address.  */
  { "a0", UBICOM32_A0_REGNUM, 4, &builtin_type_int32, 0x80, 0 },
  { "a1", UBICOM32_A1_REGNUM, 4, &builtin_type_int32, 0x84, 0 },
  { "a2", UBICOM32_A2_REGNUM, 4, &builtin_type_int32, 0x88, 0 },
  { "a3", UBICOM32_A3_REGNUM, 4, &builtin_type_int32, 0x8c, 0 },
  { "a4", UBICOM32_A4_REGNUM, 4, &builtin_type_int32, 0x90, 0 },
  { "a5", UBICOM32_A5_REGNUM, 4, &builtin_type_int32, 0x94, 0 },
  { "a6", UBICOM32_A6_REGNUM, 4, &builtin_type_int32, UBICOM32_A6_REGADDR, 0 },
  /* Stack pointer.  */
  { "sp", UBICOM32_SP_REGNUM, 4, &builtin_type_int32, UBICOM32_SP_REGADDR, 0 },
  /* High 16 bits of multiply accumulator.  */
  { "acc0_hi", 24, 4, &builtin_type_int32, 0xa0, 0 },
  /* Low 32 bits of multiply accumulator.  */
  { "acc0_lo", 25, 4, &builtin_type_int32, 0xa4, 0 },
  /* Rounded and clipped S16.15 image of multiply accumulator.  */
  { "mac_rc16", 26, 4, &builtin_type_int32, 0xa8, REG_RDONLY },
  /* 3rd source operand.  */
  { "source_3", 27, 4, &builtin_type_uint32, 0xac, 0 },
  /* Current thread's execution count. */
  { "context_cnt", 28, 4, &builtin_type_uint32, 0xb0, REG_HIDESOME | REG_RDONLY },
  /* Control/status.  */
  { "csr", 29, 4, &builtin_type_uint32, 0xb4, 0 },
  /* Read-only status.  */
  { "rosr", UBICOM32_ROSR_REGNUM, 4, &builtin_type_uint32, 0xb8, 0 | REG_RDONLY },
  /* Iread output.  */
  { "iread_data", 31, 4, &builtin_type_uint32, 0xbc, 0 },
  /* Low 32 bits of interrupt mask.  */
  { "int_mask0", 32, 4, &builtin_type_uint32, 0xc0, REG_HIDESOME },
  /* High 32 bits of interrupt mask.  */
  { "int_mask1", 33, 4, &builtin_type_uint32, 0xc4, REG_HIDESOME },

  /* 0xc8 - 0xcf Reserved */
  /* Program counter.  */
  { "pc", UBICOM32_PC_REGNUM, 4, &void_func_ptr, 0xd0, 0 },
  { "trap_cause", 35, 4, &builtin_type_uint32, 0xd4, REG_HIDESOME },
  { "acc1_hi", 36, 4, &builtin_type_uint32, 0xd8, 0 },
  { "acc1_lo", 37, 4, &builtin_type_uint32, 0xdc, 0 },
  { "previous_pc", 38, 4, &void_func_ptr, 0xe0, REG_RDONLY },

  /* 0xd0 - 0xff  Reserved */
  /* Chip identification.  */
  { "chip_id", 39, 4, &builtin_type_uint32, 0x100, REG_HIDESOME | REG_RDONLY },
  /* Low 32 bits of interrupt status.  */
  { "int_stat0", 40, 4, &builtin_type_uint32, 0x104, REG_HIDESOME | REG_RDONLY },
  /* High 32 bits of interrupt status.  */
  { "int_stat1", 41, 4, &builtin_type_uint32, 0x108, REG_HIDESOME | REG_RDONLY },

  /* 0x10c - 0x113 Reserved */
  /* Set bits in int_stat0.  */
  { "int_set0", 42, 4, &builtin_type_uint32, 0x114, REG_HIDESOME | REG_WRONLY },
  /* Set bits in int_stat1.  */
  { "int_set1", 43, 4, &builtin_type_uint32, 0x118, REG_HIDESOME | REG_WRONLY },

  /* 0x11c - 0x123 Reserved */
  /* Clear bits in int_stat0.  */
  { "int_clr0", 44, 4, &builtin_type_uint32, 0x124, REG_HIDESOME | REG_WRONLY },
  /* Clear bits in int_stat1.  */
  { "int_clr1", 45, 4, &builtin_type_uint32, 0x128, REG_HIDESOME | REG_WRONLY },

  /* 0x12c - 0x133 Reserved */
  /* Global control.  */
  { "global_ctrl", 46, 4, &builtin_type_uint32, 0x134, 0 },
  /* Threads active status.  */
  { "mt_active", UBICOM32V3_MT_ACTIVE_REGNUM, 4, &builtin_type_uint32, 0x138, REG_HIDESOME | REG_RDONLY },
  /* Set bits in mt_active.  */
  { "mt_active_set", 48, 4, &builtin_type_uint32, 0x13c, REG_HIDESOME | REG_WRONLY },
  /* Clear bits in mt_active.  */
  { "mt_active_clr", 49, 4, &builtin_type_uint32, 0x140, REG_HIDESOME | REG_WRONLY },
  /* Debugging threads active status.  */
  { "mt_dbg_active", 50, 4, &builtin_type_uint32, 0x144, REG_HIDESOME | REG_RDONLY },
  /* Set bits in mt_dbg_active.  */
  { "mt_dbg_active_set", 51, 4, &builtin_type_uint32, 0x148, REG_HIDESOME | REG_WRONLY },
  /* Threads enabled.  */
  { "mt_en", UBICOM32V3_MT_EN_REGNUM, 4, &builtin_type_uint32, 0x14C, REG_HIDESOME },
  /* Thread priorities.  */
  { "mt_pri", UBICOM32V3_MT_PRI_REGNUM, 4, &builtin_type_uint32, 0x150, REG_HIDESOME },
  /* Thread scheduling policies.  */
  { "mt_sched", UBICOM32V3_MT_SCHED_REGNUM, 4, &builtin_type_uint32, 0x154, REG_HIDESOME },
  /* Threads stopped on a break condition.  */
  { "mt_break", 55, 4, &builtin_type_uint32, 0x158, REG_HIDESOME | REG_RDONLY },
  /* Clear bits in mt_break.  */
  { "mt_break_clr", 56, 4, &builtin_type_uint32, 0x15C, REG_HIDESOME | REG_WRONLY },
  /* Single-step threads.  */
  { "mt_single_step", 57, 4, &builtin_type_uint32, 0x160, REG_HIDESOME },
  /* Threads with minimum delay scheduling constraing.  */
  { "mt_min_del_en", 58, 4, &builtin_type_uint32, 0x164, REG_HIDESOME },
  { "mt_break_set", 59, 4, &builtin_type_uint32, 0x168, REG_HIDESOME | REG_WRONLY },

  /* 0x16c - 0x16f reserved */
  /* Data capture address.  */
  { "dcapt", 60, 4, &builtin_type_uint32, 0x170, REG_HIDESOME },

  /* 0x174 - 0x17b reserved */
  /* Debugging threads active status clear register.  */
  { "mt_dbg_active_clr", 61, 4, &builtin_type_int32, 0x17c, REG_HIDESOME | REG_WRONLY },
  /* scratchpad registers */
  { "scratchpad0", 62, 4, &builtin_type_uint32, 0x180, 0 },
  { "scratchpad1", 63, 4, &builtin_type_uint32, 0x184, 0 },
  { "scratchpad2", 64, 4, &builtin_type_uint32, 0x188, 0 },
  { "scratchpad3", 65, 4, &builtin_type_uint32, 0x18c, 0 },

  /* 0x190 - 0x19f Reserved */
  { "chip_cfg", 66, 4, &builtin_type_uint32, 0x1a0, REG_HIDESOME },
  { "mt_i_blocked", 67, 4, &builtin_type_uint32, 0x1a4, REG_HIDESOME|REG_RDONLY },
  { "mt_d_blocked", 68, 4, &builtin_type_uint32, 0x1a8, REG_HIDESOME|REG_RDONLY },
  { "mt_i_blocked_set", 69, 4, &builtin_type_uint32, 0x1ac, REG_HIDESOME|REG_WRONLY},
  { "mt_d_blocked_set", 70, 4, &builtin_type_uint32, 0x1b0, REG_HIDESOME|REG_WRONLY},
  { "mt_blocked_clr", 71, 4, &builtin_type_uint32, 0x1b4, REG_HIDESOME|REG_WRONLY},
  { "mt_trap_en", 72, 4, &builtin_type_uint32, 0x1b8, REG_HIDESOME },
  { "mt_trap", 73, 4, &builtin_type_uint32, 0x1bc, REG_HIDESOME|REG_RDONLY },
  { "mt_trap_set", 74, 4, &builtin_type_uint32, 0x1c0, REG_HIDESOME|REG_WRONLY },
  { "mt_trap_clr", 75, 4, &builtin_type_uint32, 0x1c4, REG_HIDESOME|REG_WRONLY },

  /* 0x1c8-0x1FF Reserved */
  { "i_range0_hi", 76, 4, &builtin_type_uint32, 0x200, REG_HIDESOME},
  { "i_range1_hi", 77, 4, &builtin_type_uint32, 0x204, REG_HIDESOME},
  { "i_range2_hi", 78, 4, &builtin_type_uint32, 0x208, REG_HIDESOME},

  /* 0x20c-0x21f Reserved */
  { "i_range0_lo", 79, 4, &builtin_type_uint32, 0x220, REG_HIDESOME},
  { "i_range1_lo", 80, 4, &builtin_type_uint32, 0x224, REG_HIDESOME},
  { "i_range2_lo", 81, 4, &builtin_type_uint32, 0x228, REG_HIDESOME},

  /* 0x22c-0x23f Reserved */
  { "i_range0_en", 82, 4, &builtin_type_uint32, 0x240, REG_HIDESOME},
  { "i_range1_en", 83, 4, &builtin_type_uint32, 0x244, REG_HIDESOME},
  { "i_range2_en", 84, 4, &builtin_type_uint32, 0x248, REG_HIDESOME},

  /* 0x24c-0x25f Reserved */
  { "d_range0_hi", 85, 4, &builtin_type_uint32, 0x260, REG_HIDESOME},
  { "d_range1_hi", 86, 4, &builtin_type_uint32, 0x264, REG_HIDESOME},
  { "d_range2_hi", 87, 4, &builtin_type_uint32, 0x268, REG_HIDESOME},
  { "d_range3_hi", 88, 4, &builtin_type_uint32, 0x26c, REG_HIDESOME},

  /* 0x270-0x27f Reserved */
  { "d_range0_lo", 89, 4, &builtin_type_uint32, 0x280, REG_HIDESOME},
  { "d_range1_lo", 90, 4, &builtin_type_uint32, 0x284, REG_HIDESOME},
  { "d_range2_lo", 91, 4, &builtin_type_uint32, 0x288, REG_HIDESOME},
  { "d_range3_lo", 92, 4, &builtin_type_uint32, 0x28c, REG_HIDESOME},

  /* 0x290-0x29f Reserved */
  { "d_range0_en", 93, 4, &builtin_type_uint32, 0x2a0, REG_HIDESOME},
  { "d_range1_en", 94, 4, &builtin_type_uint32, 0x2a4, REG_HIDESOME},
  { "d_range2_en", 95, 4, &builtin_type_uint32, 0x2a8, REG_HIDESOME},
  { "d_range3_en", 96, 4, &builtin_type_uint32, 0x2ac, REG_HIDESOME},
};

/* Number of registers.  */
enum {
  NUM_UBICOM32V3_REGS = sizeof (ubicom32v3_regs) / sizeof (ubicom32v3_regs[0])
};

struct ubicom32_reg ubicom32v3_ver4_regs[] = {
  /* General data.  */
  { "d0", 0, 4, &builtin_type_int32, 0x0, 0 },
  { "d1", 1, 4, &builtin_type_int32, UBICOM32_D1_REGADDR, 0 },
  { "d2", 2, 4, &builtin_type_int32, 0x8, 0 },
  { "d3", 3, 4, &builtin_type_int32, 0xc, 0 },
  { "d4", 4, 4, &builtin_type_int32, 0x10, 0 },
  { "d5", 5, 4, &builtin_type_int32, 0x14, 0 },
  { "d6", 6, 4, &builtin_type_int32, 0x18, 0 },
  { "d7", 7, 4, &builtin_type_int32, 0x1c, 0 },
  { "d8", 8, 4, &builtin_type_int32, UBICOM32_D8_REGADDR, 0 },
  { "d9", 9, 4, &builtin_type_int32, 0x24, 0 },
  { "d10", 10, 4, &builtin_type_int32, 0x28, 0 },
  { "d11", 11, 4, &builtin_type_int32, 0x2c, 0 },
  { "d12", 12, 4, &builtin_type_int32, 0x30, 0 },
  { "d13", 13, 4, &builtin_type_int32, 0x34, 0 },
  { "d14", 14, 4, &builtin_type_int32, 0x38, 0 },
  { "d15", 15, 4, &builtin_type_int32, 0x3c, 0 },

  /* 0x40 - 0x7f Reserved */

 /* General address.  */
  { "a0", UBICOM32_A0_REGNUM, 4, &builtin_type_int32, 0x80, 0 },
  { "a1", UBICOM32_A1_REGNUM, 4, &builtin_type_int32, 0x84, 0 },
  { "a2", UBICOM32_A2_REGNUM, 4, &builtin_type_int32, 0x88, 0 },
  { "a3", UBICOM32_A3_REGNUM, 4, &builtin_type_int32, 0x8c, 0 },
  { "a4", UBICOM32_A4_REGNUM, 4, &builtin_type_int32, 0x90, 0 },
  { "a5", UBICOM32_A5_REGNUM, 4, &builtin_type_int32, 0x94, 0 },
  { "a6", UBICOM32_A6_REGNUM, 4, &builtin_type_int32, UBICOM32_A6_REGADDR, 0 },
  /* Stack pointer.  */
  { "sp", UBICOM32_SP_REGNUM, 4, &builtin_type_int32, UBICOM32_SP_REGADDR, 0 },
  /* High 16 bits of multiply accumulator.  */
  { "acc0_hi", 24, 4, &builtin_type_int32, 0xa0, 0 },
  /* Low 32 bits of multiply accumulator.  */
  { "acc0_lo", 25, 4, &builtin_type_int32, 0xa4, 0 },
  /* Rounded and clipped S16.15 image of multiply accumulator.  */
  { "mac_rc16", 26, 4, &builtin_type_int32, 0xa8, REG_RDONLY },
  /* 3rd source operand.  */
  { "source_3", 27, 4, &builtin_type_uint32, 0xac, 0 },
  /* Current thread's execution count. */
  { "context_cnt", 28, 4, &builtin_type_uint32, 0xb0, REG_HIDESOME | REG_RDONLY },
  /* Control/status.  */
  { "csr", 29, 4, &builtin_type_uint32, 0xb4, 0 },
  /* Read-only status.  */
  { "rosr", UBICOM32_ROSR_REGNUM, 4, &builtin_type_uint32, 0xb8, 0 | REG_RDONLY },
  /* Iread output.  */
  { "iread_data", 31, 4, &builtin_type_uint32, 0xbc, 0 },
  /* Low 32 bits of interrupt mask.  */
  { "int_mask0", 32, 4, &builtin_type_uint32, 0xc0, REG_HIDESOME },
  /* High 32 bits of interrupt mask.  */
  { "int_mask1", 33, 4, &builtin_type_uint32, 0xc4, REG_HIDESOME },

  /* 0xc8 - 0xcf Reserved */
  /* Program counter.  */
  { "pc", UBICOM32_PC_REGNUM, 4, &void_func_ptr, 0xd0, 0 },
  { "trap_cause", 35, 4, &builtin_type_uint32, 0xd4, REG_HIDESOME },
  { "acc1_hi", 36, 4, &builtin_type_uint32, 0xd8, 0 },
  { "acc1_lo", 37, 4, &builtin_type_uint32, 0xdc, 0 },
  { "previous_pc", 38, 4, &void_func_ptr, 0xe0, REG_RDONLY },

  /* 0xd0 - 0xff  Reserved */
  /* Chip identification.  */
  { "chip_id", 39, 4, &builtin_type_uint32, 0x100, REG_HIDESOME | REG_RDONLY },
  /* Low 32 bits of interrupt status.  */
  { "int_stat0", 40, 4, &builtin_type_uint32, 0x104, REG_HIDESOME | REG_RDONLY },
  /* High 32 bits of interrupt status.  */
  { "int_stat1", 41, 4, &builtin_type_uint32, 0x108, REG_HIDESOME | REG_RDONLY },

  /* 0x10c - 0x113 Reserved */
  /* Set bits in int_stat0.  */
  { "int_set0", 42, 4, &builtin_type_uint32, 0x114, REG_HIDESOME | REG_WRONLY },
  /* Set bits in int_stat1.  */
  { "int_set1", 43, 4, &builtin_type_uint32, 0x118, REG_HIDESOME | REG_WRONLY },

  /* 0x11c - 0x123 Reserved */
  /* Clear bits in int_stat0.  */
  { "int_clr0", 44, 4, &builtin_type_uint32, 0x124, REG_HIDESOME | REG_WRONLY },
  /* Clear bits in int_stat1.  */
  { "int_clr1", 45, 4, &builtin_type_uint32, 0x128, REG_HIDESOME | REG_WRONLY },

  /* 0x12c - 0x133 Reserved */
  /* Global control.  */
  { "global_ctrl", 46, 4, &builtin_type_uint32, 0x134, 0 },
  /* Threads active status.  */
  { "mt_active", UBICOM32V3_MT_ACTIVE_REGNUM, 4, &builtin_type_uint32, 0x138, REG_HIDESOME | REG_RDONLY },
  /* Set bits in mt_active.  */
  { "mt_active_set", 48, 4, &builtin_type_uint32, 0x13c, REG_HIDESOME | REG_WRONLY },
  /* Clear bits in mt_active.  */
  { "mt_active_clr", 49, 4, &builtin_type_uint32, 0x140, REG_HIDESOME | REG_WRONLY },
  /* Debugging threads active status.  */
  { "mt_dbg_active", 50, 4, &builtin_type_uint32, 0x144, REG_HIDESOME | REG_RDONLY },
  /* Set bits in mt_dbg_active.  */
  { "mt_dbg_active_set", 51, 4, &builtin_type_uint32, 0x148, REG_HIDESOME | REG_WRONLY },
  /* Threads enabled.  */
  { "mt_en", UBICOM32V3_MT_EN_REGNUM, 4, &builtin_type_uint32, 0x14C, REG_HIDESOME },
  /* Thread priorities.  */
  { "mt_pri", UBICOM32V3_MT_PRI_REGNUM, 4, &builtin_type_uint32, 0x150, REG_HIDESOME },
  /* Thread scheduling policies.  */
  { "mt_sched", UBICOM32V3_MT_SCHED_REGNUM, 4, &builtin_type_uint32, 0x154, REG_HIDESOME },
  /* Threads stopped on a break condition.  */
  { "mt_break", 55, 4, &builtin_type_uint32, 0x158, REG_HIDESOME | REG_RDONLY },
  /* Clear bits in mt_break.  */
  { "mt_break_clr", 56, 4, &builtin_type_uint32, 0x15C, REG_HIDESOME | REG_WRONLY },
  /* Single-step threads.  */
  { "mt_single_step", 57, 4, &builtin_type_uint32, 0x160, REG_HIDESOME },
  /* Threads with minimum delay scheduling constraing.  */
  { "mt_min_del_en", 58, 4, &builtin_type_uint32, 0x164, REG_HIDESOME },
  { "mt_break_set", 59, 4, &builtin_type_uint32, 0x168, REG_HIDESOME | REG_WRONLY },

  /* 0x16c - 0x16f reserved */
  /* Data capture address.  */
  { "dcapt", 60, 4, &builtin_type_uint32, 0x170, REG_HIDESOME },

  /* 0x174 - 0x17b reserved */
  /* Debugging threads active status clear register.  */
  { "mt_dbg_active_clr", 61, 4, &builtin_type_int32, 0x17c, REG_HIDESOME | REG_WRONLY },
  /* scratchpad registers */
  { "scratchpad0", 62, 4, &builtin_type_uint32, 0x180, 0 },
  { "scratchpad1", 63, 4, &builtin_type_uint32, 0x184, 0 },
  { "scratchpad2", 64, 4, &builtin_type_uint32, 0x188, 0 },
  { "scratchpad3", 65, 4, &builtin_type_uint32, 0x18c, 0 },

  /* 0x190 - 0x19f Reserved */
  { "chip_cfg", 66, 4, &builtin_type_uint32, 0x1a0, REG_HIDESOME },
  { "mt_i_blocked", 67, 4, &builtin_type_uint32, 0x1a4, REG_HIDESOME|REG_RDONLY },
  { "mt_d_blocked", 68, 4, &builtin_type_uint32, 0x1a8, REG_HIDESOME|REG_RDONLY },
  { "mt_i_blocked_set", 69, 4, &builtin_type_uint32, 0x1ac, REG_HIDESOME|REG_WRONLY},
  { "mt_d_blocked_set", 70, 4, &builtin_type_uint32, 0x1b0, REG_HIDESOME|REG_WRONLY},
  { "mt_blocked_clr", 71, 4, &builtin_type_uint32, 0x1b4, REG_HIDESOME|REG_WRONLY},
  { "mt_trap_en", 72, 4, &builtin_type_uint32, 0x1b8, REG_HIDESOME },
  { "mt_trap", 73, 4, &builtin_type_uint32, 0x1bc, REG_HIDESOME|REG_RDONLY },
  { "mt_trap_set", 74, 4, &builtin_type_uint32, 0x1c0, REG_HIDESOME|REG_WRONLY },
  { "mt_trap_clr", 75, 4, &builtin_type_uint32, 0x1c4, REG_HIDESOME|REG_WRONLY },

  /* 0x1c8-0x1FF Reserved */
  { "i_range0_hi", 76, 4, &builtin_type_uint32, 0x200, REG_HIDESOME},
  { "i_range1_hi", 77, 4, &builtin_type_uint32, 0x204, REG_HIDESOME},
  { "i_range2_hi", 78, 4, &builtin_type_uint32, 0x208, REG_HIDESOME},
  { "i_range3_hi", 79, 4, &builtin_type_uint32, 0x20c, REG_HIDESOME},

  /* 0x210-0x21f Reserved */
  { "i_range0_lo", 80, 4, &builtin_type_uint32, 0x220, REG_HIDESOME},
  { "i_range1_lo", 81, 4, &builtin_type_uint32, 0x224, REG_HIDESOME},
  { "i_range2_lo", 82, 4, &builtin_type_uint32, 0x228, REG_HIDESOME},
  { "i_range3_lo", 83, 4, &builtin_type_uint32, 0x22c, REG_HIDESOME},

  /* 0x230-0x23f Reserved */
  { "i_range0_en", 84, 4, &builtin_type_uint32, 0x240, REG_HIDESOME},
  { "i_range1_en", 85, 4, &builtin_type_uint32, 0x244, REG_HIDESOME},
  { "i_range2_en", 86, 4, &builtin_type_uint32, 0x248, REG_HIDESOME},
  { "i_range3_en", 87, 4, &builtin_type_uint32, 0x24c, REG_HIDESOME},

  /* 0x250-0x25f Reserved */
  { "d_range0_hi", 88, 4, &builtin_type_uint32, 0x260, REG_HIDESOME},
  { "d_range1_hi", 89, 4, &builtin_type_uint32, 0x264, REG_HIDESOME},
  { "d_range2_hi", 90, 4, &builtin_type_uint32, 0x268, REG_HIDESOME},
  { "d_range3_hi", 91, 4, &builtin_type_uint32, 0x26c, REG_HIDESOME},
  { "d_range4_hi", 92, 4, &builtin_type_uint32, 0x270, REG_HIDESOME},

  /* 0x274-0x27f Reserved */
  { "d_range0_lo", 93, 4, &builtin_type_uint32, 0x280, REG_HIDESOME},
  { "d_range1_lo", 94, 4, &builtin_type_uint32, 0x284, REG_HIDESOME},
  { "d_range2_lo", 95, 4, &builtin_type_uint32, 0x288, REG_HIDESOME},
  { "d_range3_lo", 96, 4, &builtin_type_uint32, 0x28c, REG_HIDESOME},
  { "d_range4_lo", 97, 4, &builtin_type_uint32, 0x290, REG_HIDESOME},

  /* 0x294-0x29f Reserved */
  { "d_range0_en", 98, 4, &builtin_type_uint32, 0x2a0, REG_HIDESOME},
  { "d_range1_en", 99, 4, &builtin_type_uint32, 0x2a4, REG_HIDESOME},
  { "d_range2_en", 100, 4, &builtin_type_uint32, 0x2a8, REG_HIDESOME},
  { "d_range3_en", 101, 4, &builtin_type_uint32, 0x2ac, REG_HIDESOME},
  { "d_range4_en", 102, 4, &builtin_type_uint32, 0x2b0, REG_HIDESOME},
};

/* Number of registers.  */
enum {
  NUM_UBICOM32V3_VER4_REGS = sizeof (ubicom32v3_ver4_regs) / sizeof (ubicom32v3_ver4_regs[0])
};

struct ubicom32_reg ubicom32v3_ver5_regs[] = {
  /* General data.  */
  { "d0", 0, 4, &builtin_type_int32, 0x0, 0 },
  { "d1", 1, 4, &builtin_type_int32, UBICOM32_D1_REGADDR, 0 },
  { "d2", 2, 4, &builtin_type_int32, 0x8, 0 },
  { "d3", 3, 4, &builtin_type_int32, 0xc, 0 },
  { "d4", 4, 4, &builtin_type_int32, 0x10, 0 },
  { "d5", 5, 4, &builtin_type_int32, 0x14, 0 },
  { "d6", 6, 4, &builtin_type_int32, 0x18, 0 },
  { "d7", 7, 4, &builtin_type_int32, 0x1c, 0 },
  { "d8", 8, 4, &builtin_type_int32, UBICOM32_D8_REGADDR, 0 },
  { "d9", 9, 4, &builtin_type_int32, 0x24, 0 },
  { "d10", 10, 4, &builtin_type_int32, 0x28, 0 },
  { "d11", 11, 4, &builtin_type_int32, 0x2c, 0 },
  { "d12", 12, 4, &builtin_type_int32, 0x30, 0 },
  { "d13", 13, 4, &builtin_type_int32, 0x34, 0 },
  { "d14", 14, 4, &builtin_type_int32, 0x38, 0 },
  { "d15", 15, 4, &builtin_type_int32, 0x3c, 0 },

  /* 0x40 - 0x7f Reserved */

 /* General address.  */
  { "a0", UBICOM32_A0_REGNUM, 4, &builtin_type_int32, 0x80, 0 },
  { "a1", UBICOM32_A1_REGNUM, 4, &builtin_type_int32, 0x84, 0 },
  { "a2", UBICOM32_A2_REGNUM, 4, &builtin_type_int32, 0x88, 0 },
  { "a3", UBICOM32_A3_REGNUM, 4, &builtin_type_int32, 0x8c, 0 },
  { "a4", UBICOM32_A4_REGNUM, 4, &builtin_type_int32, 0x90, 0 },
  { "a5", UBICOM32_A5_REGNUM, 4, &builtin_type_int32, 0x94, 0 },
  { "a6", UBICOM32_A6_REGNUM, 4, &builtin_type_int32, UBICOM32_A6_REGADDR, 0 },
  /* Stack pointer.  */
  { "sp", UBICOM32_SP_REGNUM, 4, &builtin_type_int32, UBICOM32_SP_REGADDR, 0 },
  /* High 16 bits of multiply accumulator.  */
  { "acc0_hi", 24, 4, &builtin_type_int32, 0xa0, 0},
  /* Low 32 bits of multiply accumulator.  */
  { "acc0_lo", 25, 4, &builtin_type_int32, 0xa4, 0 },
  /* Rounded and clipped S16.15 image of multiply accumulator.  */
  { "mac_rc16", 26, 4, &builtin_type_int32, 0xa8, REG_RDONLY },
  /* 3rd source operand.  */
  { "source_3", 27, 4, &builtin_type_uint32, 0xac, 0 },
  /* Current thread's execution count. */
  { "context_cnt", 28, 4, &builtin_type_uint32, 0xb0, REG_HIDESOME | REG_RDONLY },
  /* Control/status.  */
  { "csr", 29, 4, &builtin_type_uint32, 0xb4, 0 },
  /* Read-only status.  */
  { "rosr", UBICOM32_ROSR_REGNUM, 4, &builtin_type_uint32, 0xb8, 0 | REG_RDONLY },
  /* Iread output.  */
  { "iread_data", 31, 4, &builtin_type_uint32, 0xbc, 0 },
  /* Low 32 bits of interrupt mask.  */
  { "int_mask0", 32, 4, &builtin_type_uint32, 0xc0, REG_HIDESOME },
  /* High 32 bits of interrupt mask.  */
  { "int_mask1", 33, 4, &builtin_type_uint32, 0xc4, REG_HIDESOME },
  { "int_mask2", 34, 4, &builtin_type_uint32, 0xc8, REG_HIDESOME },

  /* 0xc8 - 0xcf Reserved */
  /* Program counter.  */
  { "pc", 35, 4, &void_func_ptr, 0xd0, 0 },
  { "trap_cause", 36, 4, &builtin_type_uint32, 0xd4, REG_HIDESOME },
  { "acc1_hi", 37, 4, &builtin_type_uint32, 0xd8, 0 },
  { "acc1_lo", 38, 4, &builtin_type_uint32, 0xdc, 0 },
  { "previous_pc", 39, 4, &void_func_ptr, 0xe0, REG_RDONLY },
  { "ucsr", 40, 4, &void_func_ptr, 0xe4, 0 },

  /* 0xd0 - 0xff  Reserved */
  /* Chip identification.  */
  { "chip_id", 41, 4, &builtin_type_uint32, 0x100, REG_HIDESOME | REG_RDONLY },
  /* Low 32 bits of interrupt status.  */
  { "int_stat0", 42, 4, &builtin_type_uint32, 0x104, REG_HIDESOME | REG_RDONLY },
  /* High 32 bits of interrupt status.  */
  { "int_stat1", 43, 4, &builtin_type_uint32, 0x108, REG_HIDESOME | REG_RDONLY },
  { "int_stat2", 44, 4, &builtin_type_uint32, 0x10c, REG_HIDESOME | REG_RDONLY },

  /* 0x110 - 0x113 Reserved */
  /* Set bits in int_stat0.  */
  { "int_set0", 45, 4, &builtin_type_uint32, 0x114, REG_HIDESOME | REG_WRONLY },
  /* Set bits in int_stat1.  */
  { "int_set1", 46, 4, &builtin_type_uint32, 0x118, REG_HIDESOME | REG_WRONLY },
  { "int_set2", 47, 4, &builtin_type_uint32, 0x11c, REG_HIDESOME | REG_WRONLY },

  /* 0x120 - 0x123 Reserved */
  /* Clear bits in int_stat0.  */
  { "int_clr0", 48, 4, &builtin_type_uint32, 0x124, REG_HIDESOME | REG_WRONLY },
  /* Clear bits in int_stat1.  */
  { "int_clr1", 49, 4, &builtin_type_uint32, 0x128, REG_HIDESOME | REG_WRONLY },
  { "int_clr2", 50, 4, &builtin_type_uint32, 0x12c, REG_HIDESOME | REG_WRONLY },

  /* 0x130 - 0x133 Reserved */
  /* Global control.  */
  { "global_ctrl", 51, 4, &builtin_type_uint32, 0x134, 0 },
  /* Threads active status.  */
  { "mt_active", 52, 4, &builtin_type_uint32, 0x138, REG_HIDESOME | REG_RDONLY },
  /* Set bits in mt_active.  */
  { "mt_active_set", 53, 4, &builtin_type_uint32, 0x13c, REG_HIDESOME | REG_WRONLY },
  /* Clear bits in mt_active.  */
  { "mt_active_clr", 54, 4, &builtin_type_uint32, 0x140, REG_HIDESOME | REG_WRONLY },
  /* Debugging threads active status.  */
  { "mt_dbg_active", 55, 4, &builtin_type_uint32, 0x144, REG_HIDESOME | REG_RDONLY },
  /* Set bits in mt_dbg_active.  */
  { "mt_dbg_active_set", 56, 4, &builtin_type_uint32, 0x148, REG_HIDESOME | REG_WRONLY },
  /* Threads enabled.  */
  { "mt_en", 57, 4, &builtin_type_uint32, 0x14C, REG_HIDESOME },
  /* Thread priorities.  */
  { "mt_pri", 58, 4, &builtin_type_uint32, 0x150, REG_HIDESOME },
  /* Thread scheduling policies.  */
  { "mt_sched", 59, 4, &builtin_type_uint32, 0x154, REG_HIDESOME },
  /* Threads stopped on a break condition.  */
  { "mt_break", 60, 4, &builtin_type_uint32, 0x158, REG_HIDESOME | REG_RDONLY },
  /* Clear bits in mt_break.  */
  { "mt_break_clr", 61, 4, &builtin_type_uint32, 0x15C, REG_HIDESOME | REG_WRONLY },
  /* Single-step threads.  */
  { "mt_single_step", 62, 4, &builtin_type_uint32, 0x160, REG_HIDESOME },
  /* Threads with minimum delay scheduling constraing.  */
  { "mt_min_del_en", 63, 4, &builtin_type_uint32, 0x164, REG_HIDESOME },
  { "mt_break_set", 64, 4, &builtin_type_uint32, 0x168, REG_HIDESOME | REG_WRONLY },

  /* 0x16c mt_fp_flush is at the end of this structure.  */
  /* Data capture address.  */
  { "dcapt", 65, 4, &builtin_type_uint32, 0x170, REG_HIDESOME },

  /* 0x174 - 0x17b mt_fp_blocked and mt_fp_blocked_set are at the end of this structure. */
  /* Debugging threads active status clear register.  */
  { "mt_dbg_active_clr", 66, 4, &builtin_type_int32, 0x17c, REG_HIDESOME | REG_WRONLY },
  /* scratchpad registers */
  { "scratchpad0", 67, 4, &builtin_type_uint32, 0x180, 0 },
  { "scratchpad1", 68, 4, &builtin_type_uint32, 0x184, 0 },
  { "scratchpad2", 69, 4, &builtin_type_uint32, 0x188, 0 },
  { "scratchpad3", 70, 4, &builtin_type_uint32, 0x18c, 0 },
  { "scratchpad4", 71, 4, &builtin_type_uint32, 0x190, 0 },
  { "scratchpad5", 72, 4, &builtin_type_uint32, 0x194, 0 },
  { "scratchpad6", 73, 4, &builtin_type_uint32, 0x198, 0 },

  /* 0x19c - 0x19f Reserved */
  { "chip_cfg", 74, 4, &builtin_type_uint32, 0x1a0, REG_HIDESOME },
  { "mt_i_blocked", 75, 4, &builtin_type_uint32, 0x1a4, REG_HIDESOME|REG_RDONLY },
  { "mt_d_blocked", 76, 4, &builtin_type_uint32, 0x1a8, REG_HIDESOME|REG_RDONLY },
  { "mt_i_blocked_set", 77, 4, &builtin_type_uint32, 0x1ac, REG_HIDESOME|REG_WRONLY},
  { "mt_d_blocked_set", 78, 4, &builtin_type_uint32, 0x1b0, REG_HIDESOME|REG_WRONLY},
  { "mt_blocked_clr", 79, 4, &builtin_type_uint32, 0x1b4, REG_HIDESOME|REG_WRONLY},
  { "mt_trap_en", 80, 4, &builtin_type_uint32, 0x1b8, REG_HIDESOME },
  { "mt_trap", 81, 4, &builtin_type_uint32, 0x1bc, REG_HIDESOME|REG_RDONLY },
  { "mt_trap_set", 82, 4, &builtin_type_uint32, 0x1c0, REG_HIDESOME|REG_WRONLY },
  { "mt_trap_clr", 83, 4, &builtin_type_uint32, 0x1c4, REG_HIDESOME|REG_WRONLY },
  { "sep", 84, 4, &builtin_type_uint32, 0x1c8, 0 },

  /* 0x1cc-0x1FF Reserved */
  { "i_range0_hi", 85, 4, &builtin_type_uint32, 0x200, REG_HIDESOME},
  { "i_range1_hi", 86, 4, &builtin_type_uint32, 0x204, REG_HIDESOME},
  { "i_range2_hi", 87, 4, &builtin_type_uint32, 0x208, REG_HIDESOME},
  { "i_range3_hi", 88, 4, &builtin_type_uint32, 0x20c, REG_HIDESOME},

  /* 0x210-0x21f Reserved */
  { "i_range0_lo", 89, 4, &builtin_type_uint32, 0x220, REG_HIDESOME},
  { "i_range1_lo", 90, 4, &builtin_type_uint32, 0x224, REG_HIDESOME},
  { "i_range2_lo", 91, 4, &builtin_type_uint32, 0x228, REG_HIDESOME},
  { "i_range3_lo", 92, 4, &builtin_type_uint32, 0x22c, REG_HIDESOME},

  /* 0x230-0x23f Reserved */
  { "i_range0_en", 93, 4, &builtin_type_uint32, 0x240, REG_HIDESOME},
  { "i_range1_en", 94, 4, &builtin_type_uint32, 0x244, REG_HIDESOME},
  { "i_range2_en", 95, 4, &builtin_type_uint32, 0x248, REG_HIDESOME},
  { "i_range3_en", 96, 4, &builtin_type_uint32, 0x24c, REG_HIDESOME},

  /* 0x250-0x25f Reserved */
  { "d_range0_hi", 97, 4, &builtin_type_uint32, 0x260, REG_HIDESOME},
  { "d_range1_hi", 98, 4, &builtin_type_uint32, 0x264, REG_HIDESOME},
  { "d_range2_hi", 99, 4, &builtin_type_uint32, 0x268, REG_HIDESOME},
  { "d_range3_hi", 100, 4, &builtin_type_uint32, 0x26c, REG_HIDESOME},
  { "d_range4_hi", 101, 4, &builtin_type_uint32, 0x270, REG_HIDESOME},
  { "d_range5_hi", 102, 4, &builtin_type_uint32, 0x274, REG_HIDESOME},

  /* 0x278-0x27f Reserved */
  { "d_range0_lo", 103, 4, &builtin_type_uint32, 0x280, REG_HIDESOME},
  { "d_range1_lo", 104, 4, &builtin_type_uint32, 0x284, REG_HIDESOME},
  { "d_range2_lo", 105, 4, &builtin_type_uint32, 0x288, REG_HIDESOME},
  { "d_range3_lo", 106, 4, &builtin_type_uint32, 0x28c, REG_HIDESOME},
  { "d_range4_lo", 107, 4, &builtin_type_uint32, 0x290, REG_HIDESOME},
  { "d_range5_lo", 108, 4, &builtin_type_uint32, 0x294, REG_HIDESOME},

  /* 0x298-0x29f Reserved */
  { "d_range0_en", 109, 4, &builtin_type_uint32, 0x2a0, REG_HIDESOME},
  { "d_range1_en", 110, 4, &builtin_type_uint32, 0x2a4, REG_HIDESOME},
  { "d_range2_en", 111, 4, &builtin_type_uint32, 0x2a8, REG_HIDESOME},
  { "d_range3_en", 112, 4, &builtin_type_uint32, 0x2ac, REG_HIDESOME},
  { "d_range4_en", 113, 4, &builtin_type_uint32, 0x2b0, REG_HIDESOME},
  { "d_range5_en", 114, 4, &builtin_type_uint32, 0x2b4, REG_HIDESOME},

  /* 0x2b8 - 0x2bf Reserved */
  { "i_range0_user_en", 115, 4, &builtin_type_uint32, 0x2c0, REG_HIDESOME},
  { "i_range1_user_en", 116, 4, &builtin_type_uint32, 0x2c4, REG_HIDESOME},
  { "i_range2_user_en", 117, 4, &builtin_type_uint32, 0x2c8, REG_HIDESOME},
  { "i_range3_user_en", 118, 4, &builtin_type_uint32, 0x2cc, REG_HIDESOME},

  /* 0x2d0-0x2df Reserved */
  { "d_range0_user_en", 119, 4, &builtin_type_uint32, 0x2e0, REG_HIDESOME},
  { "d_range1_user_en", 120, 4, &builtin_type_uint32, 0x2e4, REG_HIDESOME},
  { "d_range2_user_en", 121, 4, &builtin_type_uint32, 0x2e8, REG_HIDESOME},
  { "d_range3_user_en", 122, 4, &builtin_type_uint32, 0x2ec, REG_HIDESOME},
  { "d_range4_user_en", 123, 4, &builtin_type_uint32, 0x2f0, REG_HIDESOME},
  { "d_range5_user_en", 124, 4, &builtin_type_uint32, 0x2f4, REG_HIDESOME},

  /* fp registers. */
  { "mt_fp_flush", 125, 4, &builtin_type_uint32, 0x16c, REG_HIDESOME|REG_WRONLY },
  { "mt_fp_blocked", 126, 4, &builtin_type_uint32, 0x174, REG_HIDESOME},
  { "mt_fp_blocked_set", 127, 4, &builtin_type_uint32, 0x178, REG_HIDESOME|REG_WRONLY },

  /* btb registers. */
  { "mt_btb_en", 128, 4, &builtin_type_uint32, 0x1cc, REG_HIDESOME },
  { "btb_ctrl", 129, 4, &builtin_type_uint32, 0x1d0, REG_HIDESOME },
};

/* Number of registers.  */
enum {
  NUM_UBICOM32V3_VER5_REGS = sizeof (ubicom32v3_ver5_regs) / sizeof (ubicom32v3_ver5_regs[0])
};

struct ubicom32_reg ubicom32_posix_regs[] = {
  /* General data.  */
  { "d0", 0, 4, &builtin_type_int32, 0x0, 0 },
  { "d1", 1, 4, &builtin_type_int32, UBICOM32_D1_REGADDR, 0 },
  { "d2", 2, 4, &builtin_type_int32, 0x8, 0 },
  { "d3", 3, 4, &builtin_type_int32, 0xc, 0 },
  { "d4", 4, 4, &builtin_type_int32, 0x10, 0 },
  { "d5", 5, 4, &builtin_type_int32, 0x14, 0 },
  { "d6", 6, 4, &builtin_type_int32, 0x18, 0 },
  { "d7", 7, 4, &builtin_type_int32, 0x1c, 0 },
  { "d8", 8, 4, &builtin_type_int32, UBICOM32_D8_REGADDR, 0 },
  { "d9", 9, 4, &builtin_type_int32, 0x24, 0 },
  { "d10", 10, 4, &builtin_type_int32, 0x28, 0 },
  { "d11", 11, 4, &builtin_type_int32, 0x2c, 0 },
  { "d12", 12, 4, &builtin_type_int32, 0x30, 0 },
  { "d13", 13, 4, &builtin_type_int32, 0x34, 0 },
  { "d14", 14, 4, &builtin_type_int32, 0x38, 0 },
  { "d15", 15, 4, &builtin_type_int32, 0x3c, 0 },

  /* 0x40 - 0x7f Reserved */

 /* General address.  */
  { "a0", UBICOM32_A0_REGNUM, 4, &builtin_type_int32, 0x80, 0 },
  { "a1", UBICOM32_A1_REGNUM, 4, &builtin_type_int32, 0x84, 0 },
  { "a2", UBICOM32_A2_REGNUM, 4, &builtin_type_int32, 0x88, 0 },
  { "a3", UBICOM32_A3_REGNUM, 4, &builtin_type_int32, 0x8c, 0 },
  { "a4", UBICOM32_A4_REGNUM, 4, &builtin_type_int32, 0x90, 0 },
  { "a5", UBICOM32_A5_REGNUM, 4, &builtin_type_int32, 0x94, 0 },
  { "a6", UBICOM32_A6_REGNUM, 4, &builtin_type_int32, UBICOM32_A6_REGADDR, 0 },
  /* Stack pointer.  */
  { "sp", UBICOM32_SP_REGNUM, 4, &builtin_type_int32, UBICOM32_SP_REGADDR, 0 },
  /* High 16 bits of multiply accumulator.  */
  { "acc0_hi", 24, 4, &builtin_type_int32, 0xa0, 0 },
  /* Low 32 bits of multiply accumulator.  */
  { "acc0_lo", 25, 4, &builtin_type_int32, 0xa4, 0 },
  /* Rounded and clipped S16.15 image of multiply accumulator.  */
  { "mac_rc16", 26, 4, &builtin_type_int32, 0xa8,  REG_RDONLY },
  { "acc1_hi", 27, 4, &builtin_type_uint32, 0xa9, 0 },
  { "acc1_lo", 28, 4, &builtin_type_uint32, 0xaa, 0 },
  /* 3rd source operand.  */
  { "source_3", 29, 4, &builtin_type_uint32, 0xac, 0 },
  /* Current thread's execution count. */
  { "context_cnt", 30, 4, &builtin_type_uint32, 0xb0, REG_HIDESOME | REG_RDONLY },
  /* Control/status.  */
  { "csr", 31, 4, &builtin_type_uint32, 0xb4, 0 },
  /* Iread output.  */
  { "iread_data", 32, 4, &builtin_type_uint32, 0xbc, 0 },
  /* Low 32 bits of interrupt mask.  */
  { "int_mask0", 33, 4, &builtin_type_uint32, 0xc0, REG_HIDESOME },
  /* High 32 bits of interrupt mask.  */
  { "int_mask1", 34, 4, &builtin_type_uint32, 0xc4, REG_HIDESOME },

  /* 0xc8 - 0xcf Reserved */
  /* Program counter.  */
  { "trap_cause", 35, 4, &builtin_type_uint32, 0xc8, 0 },
  { "pc", 36, 4, &void_func_ptr, 0xd0, 0 },
};

/* Number of registers.  */
enum {
  NUM_UBICOM32_POSIX_REGS = sizeof (ubicom32_posix_regs) / sizeof (ubicom32_posix_regs[0]),
};

#define UBICOM32_POSIX_PC_REGNUM 36

struct ubicom32_reg ubicom32_uclinux_regs[] = {
  /* General data.  */
  { "d0", 0, 4, &builtin_type_int32, 0x0, 0 },
  { "d1", 1, 4, &builtin_type_int32, UBICOM32_D1_REGADDR, 0 },
  { "d2", 2, 4, &builtin_type_int32, 0x8, 0 },
  { "d3", 3, 4, &builtin_type_int32, 0xc, 0 },
  { "d4", 4, 4, &builtin_type_int32, 0x10, 0 },
  { "d5", 5, 4, &builtin_type_int32, 0x14, 0 },
  { "d6", 6, 4, &builtin_type_int32, 0x18, 0 },
  { "d7", 7, 4, &builtin_type_int32, 0x1c, 0 },
  { "d8", 8, 4, &builtin_type_int32, UBICOM32_D8_REGADDR, 0 },
  { "d9", 9, 4, &builtin_type_int32, 0x24, 0 },
  { "d10", 10, 4, &builtin_type_int32, 0x28, 0 },
  { "d11", 11, 4, &builtin_type_int32, 0x2c, 0 },
  { "d12", 12, 4, &builtin_type_int32, 0x30, 0 },
  { "d13", 13, 4, &builtin_type_int32, 0x34, 0 },
  { "d14", 14, 4, &builtin_type_int32, 0x38, 0 },
  { "d15", 15, 4, &builtin_type_int32, 0x3c, 0 },

  /* 0x40 - 0x7f Reserved */

 /* General address.  */
  { "a0", UBICOM32_A0_REGNUM, 4, &builtin_type_int32, 0x80, 0 },
  { "a1", UBICOM32_A1_REGNUM, 4, &builtin_type_int32, 0x84, 0 },
  { "a2", UBICOM32_A2_REGNUM, 4, &builtin_type_int32, 0x88, 0 },
  { "a3", UBICOM32_A3_REGNUM, 4, &builtin_type_int32, 0x8c, 0 },
  { "a4", UBICOM32_A4_REGNUM, 4, &builtin_type_int32, 0x90, 0 },
  { "a5", UBICOM32_A5_REGNUM, 4, &builtin_type_int32, 0x94, 0 },
  { "a6", UBICOM32_A6_REGNUM, 4, &builtin_type_int32, UBICOM32_A6_REGADDR, 0 },
  /* Stack pointer.  */
  { "sp", UBICOM32_SP_REGNUM, 4, &builtin_type_int32, UBICOM32_SP_REGADDR, 0 },
  /* High 16 bits of multiply accumulator.  */
  { "acc0_hi", 24, 4, &builtin_type_int32, 0xa0, 0 },
  /* Low 32 bits of multiply accumulator.  */
  { "acc0_lo", 25, 4, &builtin_type_int32, 0xa4, 0 },
  /* Rounded and clipped S16.15 image of multiply accumulator.  */
  { "mac_rc16", 26, 4, &builtin_type_int32, 0xa8,  REG_RDONLY },
  { "acc1_hi", 27, 4, &builtin_type_uint32, 0xa9, 0 },
  { "acc1_lo", 28, 4, &builtin_type_uint32, 0xaa, 0 },
  /* 3rd source operand.  */
  { "source_3", 29, 4, &builtin_type_uint32, 0xac, 0 },
  /* Current thread's execution count. */
  { "context_cnt", 30, 4, &builtin_type_uint32, 0xb0, REG_HIDESOME | REG_RDONLY },
  /* Control/status.  */
  { "csr", 31, 4, &builtin_type_uint32, 0xb4, 0 },
  /* Iread output.  */
  { "iread_data", 32, 4, &builtin_type_uint32, 0xbc, 0 },
  /* Low 32 bits of interrupt mask.  */
  { "int_mask0", 33, 4, &builtin_type_uint32, 0xc0, REG_HIDESOME },
  /* High 32 bits of interrupt mask.  */
  { "int_mask1", 34, 4, &builtin_type_uint32, 0xc4, REG_HIDESOME },

  /* 0xc8 - 0xcf Reserved */
  /* Program counter.  */
  { "trap_cause", 35, 4, &builtin_type_uint32, 0xc8, 0 },
  { "pc", 36, 4, &void_func_ptr, 0xd0, 0 },

  { "text_addr", 37, 4, &builtin_type_uint32, 0xFF,  REG_RDONLY  },
  { "text_end_addr", 38, 4, &builtin_type_uint32, 0xFF,  REG_RDONLY  },
  { "data_addr", 39, 4, &builtin_type_uint32, 0xFF,  REG_RDONLY  },
  { "fdpic_exec", 40, 4, &builtin_type_uint32, 0xFF,  REG_RDONLY  },
  { "fdpic_interp", 41, 4, &builtin_type_uint32, 0xFF,  REG_RDONLY  },
  { "previous_pc", 42, 4, &builtin_type_uint32, 0xFF,  REG_RDONLY  },

};

/* Number of registers.  */
enum {
  NUM_UBICOM32_UCLINUX_REGS = sizeof (ubicom32_uclinux_regs) / sizeof (ubicom32_uclinux_regs[0]),
};

#define UBICOM32_UCLINUX_PC_REGNUM 36

#if 0
#define UBICOM32V3_MAIN_STRING "__main__"
#endif
#define UBICOM32V3_MAIN_STRING "main"
#define UBICOM32V3_MAIN_STRING2 "system_main"
#define UBICOM32V3_MAIN_STRING3 "arch_thread_main"
#define UBICOM32V3_POSIX_STRING "posix_syscall"
#define UBICOM32V3_LINUX_STRING "system_call"
#define UBICOM32V3_START_KERNEL_STRING "start_kernel"
#define UBICOM32V3_LINUX_INITRAMFS_STRING "__initramfs_start"
#define UBICOM32V3_FLASH_START	0x60000000
#define UBICOM32V3_FLASH_END		0x60fffffc
#define UBICOM32V3_PRAM_START	0x40000000
#define UBICOM32V3_PRAM_END		0x4fffffff
#define UBICOM32V3_DATARAM_START	0x3ffc0000
#define UBICOM32V3_DATARAM_END	0x3fffffff

static int
ubicom32v3_frame_chain_valid (struct frame_info *fi)
{
  int line;
  int offset;
  int unmapped;
  char *filename = NULL;
  char *name = NULL;
  CORE_ADDR pc = frame_pc_unwind(fi);
  CORE_ADDR sp = frame_sp_unwind(fi);

  if (ubicom32_tdep_debug >= DEBUG_FRAME)
    fprintf_unfiltered (gdb_stdlog, "ubicom32v3_frame_chain_valid pc = 0x%x sp = 0x%x\n", (unsigned int)pc, (unsigned int)sp);

  if(pc < UBICOM32V3_DATARAM_START ||
     (UBICOM32V3_PRAM_END < pc && pc < UBICOM32V3_FLASH_START) ||
     (UBICOM32V3_FLASH_END<pc))
    {
      /* pc is in some invalid zone */
      return 0;
    }

  /* check if sp is valid at all */
  if(sp < UBICOM32V3_DATARAM_START ||
     sp > UBICOM32V3_PRAM_END)
    return 0;

  /* check if the pc is valid at all */
  if (!build_address_symbolic (pc, 0, &name, &offset, &filename, &line, &unmapped))
    {
      /* we got a hit */
      if(name)
	{
	  if(strstr(name, UBICOM32V3_MAIN_STRING))
	    {
	      /* it is present */
	      return 0;
	    }
	  else if(strstr(name, UBICOM32V3_MAIN_STRING2))
	    {
	      /* it is present */
	      return 0;
	    }
	  else if(strstr(name, UBICOM32V3_MAIN_STRING3))
	    {
	      /* it is present */
	      return 0;
	    }
	  else if(strcmp(name, UBICOM32V3_POSIX_STRING)==0)
	    {
	      /* it is present */
	      return 0;
	    }
	  else if(strcmp(name, UBICOM32V3_LINUX_STRING)==0)
	    {
	      /* it is present */
	      return 0;
	    }
	  else if(strcmp(name, UBICOM32V3_START_KERNEL_STRING)==0)
	    {
	      /* it is present */
	      return 0;
	    }
	  else if(strcmp(name, UBICOM32V3_LINUX_INITRAMFS_STRING)==0)
	    {
	      /* it is present */
	      return 0;
	    }
	}
    }

  return 1;
}

#define UBICOM32V5_FLASH_START	0xb0000000
#define UBICOM32V5_MAPPED_SPACE_END UBICOM32V5_FLASH_START
#define UBICOM32V5_FLASH_END		0xb0ffffff
#define UBICOM32V5_OCM_START	0xbffc0000
#define UBICOM32V5_OCM_END	0xbfffffff
#define UBICOM32V5_DDR_START	0xc0000000
#define UBICOM32V5_DDR_END	0xcffffffc

static int
ubicom32v5_frame_chain_valid (struct frame_info *fi)
{
  int line;
  int offset;
  int unmapped;
  char *filename = NULL;
  char *name = NULL;
  CORE_ADDR pc = frame_pc_unwind(fi);
  CORE_ADDR sp = frame_sp_unwind(fi);
  struct cleanup *cleanups;

  if (ubicom32_tdep_debug >= DEBUG_FRAME)
    fprintf_unfiltered (gdb_stdlog, "ubicom32v5_frame_chain_valid pc = 0x%x sp = 0x%x\n", (unsigned int)pc, (unsigned int)sp);

  if((UBICOM32V5_FLASH_END < pc) && (pc < UBICOM32V5_OCM_START))
    {
      /* pc is in some invalid zone */
      return 0;
    }

  /* check if the pc is valid at all */
  if (!build_address_symbolic (pc, 0, &name, &offset, &filename, &line, &unmapped))
    {
      /* we got a hit */
      if(name)
	{
	  if(strstr(name, UBICOM32V3_MAIN_STRING))
	    {
	      /* it is present */
	      return 0;
	    }
	  else if(strstr(name, UBICOM32V3_MAIN_STRING2))
	    {
	      /* it is present */
	      return 0;
	    }
	  else if(strstr(name, UBICOM32V3_MAIN_STRING3))
	    {
	      /* it is present */
	      return 0;
	    }
	  else if(strcmp(name, UBICOM32V3_POSIX_STRING)==0)
	    {
	      /* it is present */
	      return 0;
	    }
	  else if(strcmp(name, UBICOM32V3_LINUX_STRING)==0)
	    {
	      /* it is present */
	      return 0;
	    }
	  else if(strcmp(name, UBICOM32V3_START_KERNEL_STRING)==0)
	    {
	      /* it is present */
	      return 0;
	    }
	  else if(strcmp(name, UBICOM32V3_LINUX_INITRAMFS_STRING)==0)
	    {
	      /* it is present */
	      return 0;
	    }
	}
    }

  return 1;
}

static int
ubicom32_posix_frame_chain_valid (struct frame_info *fi)
{
  int line;
  int offset;
  int unmapped;
  char *filename = NULL;
  char *name = NULL;
  CORE_ADDR pc = frame_pc_unwind(fi);
  CORE_ADDR sp = frame_sp_unwind(fi);

  if (ubicom32_tdep_debug >= DEBUG_FRAME)
    fprintf_unfiltered (gdb_stdlog, "ubicom32_posix_frame_chain_valid pc = 0x%x sp = 0x%x\n", (unsigned int)pc, (unsigned int)sp);

  if(pc < UBICOM32V3_PRAM_START ||
     pc > UBICOM32V3_PRAM_END)
    {
      /* pc is in some invalid zone */
      return 0;
    }

  /* check if sp is valid at all */
  if(sp < UBICOM32V3_PRAM_START ||
     sp > UBICOM32V3_PRAM_END)
    return 0;

  return 1;
}

/* Return the name of register number NUM, or null if no such register
   exists in the current architecture.  */
static const char *
machine_register_name (struct gdbarch *gdbarch, int num)
{
  struct ubicom32_reg *reg;

  reg = machine_find_reg_num (num, NULL);
  if (!reg)
    return NULL;
  if (!reg->name)
    return "";
  return reg->name;
}

/* If REGNUM != -1, display only that register.  Otherwise, display all
   registers if ALL and an interesting subset otherwise.  */
static void __attribute__((optimize(0)))
machine_do_registers_info (struct gdbarch *gdbarch,
			struct ui_file *file,
			struct frame_info *frame,
			int regnum, int all)
{
  int from, to, i, column, pad, width;
  struct ubicom32_reg *reg;

  if (regnum < 0)
    {
      from = 0;
      to = current_machine->num_regs - 1;
    }
  else
    {
      from = to = regnum;
      all = 1;
    }

  for (i = from, column = 0; i <= to; i++)
    {
      reg = current_machine->regs + i;
      if (!reg->name || (!all && (reg->flags & REG_HIDESOME)))
	continue;

      /* Separate from previous value.  */
      if (i > from)
	{
	  if (!column)
	    fprintf_filtered (file, "\n");
	  else
	    fprintf_filtered (file, "    ");
	}

      /* Display the name, right-justified.  */
      if (regnum >= 0)
	pad = 0;
      else
	pad = current_machine->regdisp.namemax[all] - strlen (reg->name);
      fprintf_filtered (file, "%*c%s  ", pad + 1, '$', reg->name);

      /* Don't attempt to retrieve write-only values.  */
      if (reg->flags & REG_WRONLY)
	fprintf_filtered (file, "%-22s", "<write-only>");

      /* Try to retrieve raw value.  */
      else if (!frame_register_read (frame, i, current_machine->regdisp.rawbuf))
	fprintf_filtered (file, "%-22s", "<unavailable>");

      /* Display the retrieved value in hex and decimal.  */
      else
	{
	  if (reg->type == &void_func_ptr && *reg->type == NULL)
	    void_func_ptr = builtin_type_void_func_ptr;
	  display_val (*reg->type, current_machine->regdisp.rawbuf, 'x', 10, file);
	  display_val (*reg->type, current_machine->regdisp.rawbuf, 'd', 12, file);
	}
      column = !column;
    }
  fprintf_filtered (file, "\n");
}

static struct type *
machine_register_type (struct gdbarch *gdbarch, int num)
{
  return builtin_type_uint32;
}

char *
ubicom32v3_remote_threads_extra_info (struct thread_info *tp)
{
  static struct ui_file *buf = NULL;
  static char *bufcpy = NULL;
  int bit, mt_active, mt_pri, mt_sched, mt_en;
  int buflen;
  char mybuffer[1000];
  char *printBuf = mybuffer;
  char *buffer = mybuffer;

  unsigned tid;

  if (!buf)
    buf = mem_fileopen ();
  else
    ui_file_rewind (buf);

  mybuffer[0] = 0;
  bit = 1 << PTID_TO_TNUM (tp->ptid);
  tid = PIDGET(tp->ptid);
  tid --;
  mt_active = read_register (mt_active_regnum);
  mt_en = read_register(mt_en_regnum);
  mt_pri = read_register(mt_pri_regnum);
  mt_sched = read_register (mt_sched_regnum);

  if(0<= tid && tid <= 32)
    {
      /* query about a Main processor thread */
      unsigned int mask = 1<<tid;

      if(!(mt_en & mask))
	{
	  // Not even enabled
	  sprintf(buffer, "%s", "Disabled");
	  buffer += strlen(buffer);
	}
      else
	{
	  if(!(mt_active & mask))
	    {
	      sprintf(buffer, "%s", "Suspended");
	      buffer += strlen(buffer);
	    }

	  if(mt_sched &mask)
	    {
	      //HRT
	      if(strlen(printBuf))
		{
		  sprintf(buffer, ", ");
		  buffer += strlen(buffer);
		}
	      sprintf(buffer, "HRT ");
	      buffer += strlen(buffer);
	    }
	  else
	    {
	      if(strlen(printBuf))
		{
		  sprintf(buffer, ", ");
		  buffer += strlen(buffer);
		}

	      if(!(mt_pri &mask))
		sprintf(buffer, "low priority");
	      else
		sprintf(buffer, "High priority");

	      buffer += strlen(buffer);
	    }
	}
    }

  fputs_unfiltered(printBuf, buf);

  if (bufcpy)
    free (bufcpy);
  bufcpy = ui_file_xstrdup (buf, &buflen);

  if (!buflen)
    return NULL;
  return bufcpy;
}

/*  Store val in buf in big-endian order.  */
static void
put_big_endian (int val, gdb_byte *buf)
{
  buf[3] = val & 0xFF;
  buf[2] = (val >> 8) & 0xFF;
  buf[1] = (val >> 16) & 0xFF;
  buf[0] = (val >> 24) & 0xFF;
}

/* Stack of arguments to be pushed onto the stack in reverse order.  */
struct saved_stack_item 
{
  struct saved_stack_item *prev;
  gdb_byte *val;
  int len;
};

static struct saved_stack_item *
push_stack_item (struct saved_stack_item *prev, const gdb_byte *val, int len)
{
  struct saved_stack_item *si;
  si = xmalloc (sizeof (struct saved_stack_item));
  si->val = xmalloc (len);
  si->len = len;
  si->prev = prev;
  memcpy (si->val, val, len);
  return si;
}

static struct saved_stack_item *
pop_stack_item (struct saved_stack_item *si)
{
  struct saved_stack_item *dead = si;
  si = si->prev;
  xfree (dead->val);
  xfree (dead);
  return si;
}

/* Save function argument in reg or on stack.  */
#define PUT_ARG(val) 					\
  if (argreg <= UBICOM32_LAST_ARG_REGNUM)		\
    /* Put argument piece in register.  */		\
    regcache_cooked_write (regcache, argreg++, val);	\
  else							\
    /* Push arguments on stack.  */			\
    si = push_stack_item (si, val, UBICOM32_REG_SIZE);

/* Pass arguments to target function. 
   The first 10 words of the argument list are passed in D0-D9.  8-, 16-, 
   and 32-bit values are passed in single registers, 64-bit values are passed 
   in register pairs.  This includes struct values, which are passed in registers
   if they are 64-bits or less.

   Additional parameters are passsed on the stack.  */

static CORE_ADDR
ubicom32_push_dummy_call (struct gdbarch *gdbarch, struct value *function,
			  struct regcache *regcache, CORE_ADDR bp_addr,
			  int nargs, struct value **args, CORE_ADDR sp,
			  int struct_return, CORE_ADDR struct_addr)
{
  int argreg = UBICOM32_FIRST_ARG_REGNUM;
  int argnum;
  struct saved_stack_item *si = NULL;
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  gdb_byte word[UBICOM32_REG_SIZE];
  CORE_ADDR got;

  /* Point function return to dummy breakpoint.  */
  regcache_cooked_write_unsigned (regcache, UBICOM32_LR_REGNUM, bp_addr);

  /* If -mfdpic, set a0 to GOT pointer.  */
  if (tdep->ubicom32_abi == UBICOM32_ABI_FDPIC)
    {
      got = ubicom32_fdpic_find_global_pointer (VALUE_ADDRESS(function));
      put_big_endian (got, word);
      regcache_cooked_write (regcache, UBICOM32_A0_REGNUM, word);
    }
 
  /* Struct return address passed in first argument register.  */
  if (struct_return) 
    {
      put_big_endian (struct_addr, word);
      regcache_cooked_write (regcache, argreg++, word);
    }
    
  /* Align stack if necessary.  */
  sp = align_down (sp, 4); 

  for (argnum = 0; argnum < nargs; argnum++)
    {
      struct value *arg = args[argnum];
      struct type *arg_type = check_typedef (value_type (arg));
      int len = TYPE_LENGTH (arg_type);
      enum type_code typecode = TYPE_CODE (arg_type);
      const gdb_byte *val = value_contents (arg);
      int argbytes;
  
      if (!(typecode == TYPE_CODE_STRUCT || typecode == TYPE_CODE_UNION)) 
        {
	  if (len == UBICOM32_REG_SIZE)
	    { 
	      /* Argument fits in one register. */
	      /* Promotions should already have been handled.  */
	      PUT_ARG (val);
	    } 
	  else if (len == 2 * UBICOM32_REG_SIZE)
	    {
	      /* Argument fits in register pair.  */
	      PUT_ARG (val);
	      PUT_ARG (val + UBICOM32_REG_SIZE);
	    }
        }
      else /* (typecode == TYPE_CODE_STRUCT || typecode == TYPE_CODE_UNION) */
        {
	  if (len <= 2 * UBICOM32_REG_SIZE) 
	    {
	      for (argbytes = 0; argbytes < len; argbytes += UBICOM32_REG_SIZE)
		{
		  memset (word, 0, sizeof (word));
		  memcpy (word, val + argbytes, 
			  len > UBICOM32_REG_SIZE ? UBICOM32_REG_SIZE : len);
		  PUT_ARG (word);
                }
	    }
	  else
	    {
	      /* Pass struct/union larger than 8 bytes by reference. */
	      CORE_ADDR ref = VALUE_ADDRESS(arg);
	      if (ref == 0)
		{
		   /* Value is gdb generated -- push on stack before args.  */
		   sp = align_down (sp - len, 4); 
		   write_memory (sp, val, len);
		   ref = sp;
		}
	      put_big_endian (ref, word);
	      PUT_ARG (word);
	    }
        }
    }

  /* Write saved arguments to stack in reverse order.  */
  while (si) 
    {
       sp -= si->len;
       write_memory (sp, si->val, si->len);
       si = pop_stack_item (si);
    }

  /* Set stack pointer.  */
  regcache_cooked_write_unsigned (regcache, UBICOM32_SP_REGNUM, sp);

  return sp; 
}

/* Initialize register information in GDBARCH.  */
/* Initialize the regdisp entry for each machine. */
static void
init_regsdisp (struct ubicom32_machine *machine)
{
  int offset, i, size, rawmax, virtmax;
  int namelen, namemax[2];
  struct ubicom32_reg *reg;

  offset = rawmax = virtmax = 0;
  namemax[SOME] = namemax[ALL] = 0;

  for (i = 0; i < machine->num_regs; i++)
    {
      reg = machine->regs + i;
      reg->offset = offset;
      offset += reg->size;

      if (reg->size > rawmax)
	rawmax = reg->size;
      if (reg->type == (&void_func_ptr))
	size = 4;
      else
	size = TYPE_LENGTH (*reg->type);
      if (size > virtmax)
	virtmax = size;

      if (reg->name)
	{
	  namelen = strlen (reg->name);
	  if (!(reg->flags & REG_HIDESOME) && namelen > namemax[SOME])
	    namemax[SOME] = namelen;
	  if (namelen > namemax[ALL])
	    namemax[ALL] = namelen;
	}
    }

  machine->regdisp.rawbuf = xmalloc (rawmax);
  machine->regdisp.namemax[SOME] = namemax[SOME];
  machine->regdisp.namemax[ALL] = namemax[ALL];
}


/* Initialize arch attributes common to all ubicom32
   platforms/machines */
static const struct regset *
ubicom32_linux_regset_from_core_section (struct gdbarch *gdbarch,
					 const char *sect_name, size_t sect_size);

static struct gdbarch *
ubicom32_arch_init_common(const struct gdbarch_info *info, const char *name, enum ubicom32_abi abi)
{
  struct gdbarch *gdbarch;
  struct gdbarch_tdep *tdep = XMALLOC (struct gdbarch_tdep);
  gdbarch = gdbarch_alloc (info, tdep);

  tdep->ubicom32_abi = abi;
  tdep->name = name;

  /* set_gdbarch_sp_regnum, set_gdbarch_pc_regnum and
     set_gdbarch_num_regs setup by caller*/

  set_gdbarch_read_pc (gdbarch, ubicom32_read_pc);
  set_gdbarch_write_pc (gdbarch, ubicom32_write_pc);

  set_gdbarch_num_pseudo_regs (gdbarch, 0);

  set_gdbarch_register_name (gdbarch, machine_register_name);
  set_gdbarch_register_type (gdbarch, machine_register_type);
  set_gdbarch_print_registers_info (gdbarch, machine_do_registers_info);

  set_gdbarch_ptr_bit (gdbarch, 32);
  set_gdbarch_short_bit (gdbarch, 16);
  set_gdbarch_int_bit (gdbarch, 32);
  set_gdbarch_long_bit (gdbarch, 32);
  set_gdbarch_long_long_bit (gdbarch, 64);
  set_gdbarch_float_bit (gdbarch, 32);
  set_gdbarch_double_bit (gdbarch, 64);
  set_gdbarch_long_double_bit (gdbarch, 64);
  set_gdbarch_float_format (gdbarch, floatformats_ieee_single);
  set_gdbarch_double_format (gdbarch, floatformats_ieee_double);
  set_gdbarch_long_double_format (gdbarch, floatformats_ieee_double);

  set_gdbarch_address_to_pointer (gdbarch, ubicom32_address_to_pointer);
  set_gdbarch_pointer_to_address (gdbarch, ubicom32_pointer_to_address);
  set_gdbarch_integer_to_address (gdbarch, ubicom32_integer_to_address);

  set_gdbarch_call_dummy_location (gdbarch, AT_ENTRY_POINT);

#if 0
  set_gdbarch_extract_return_value (gdbarch, ubicom32_extract_return_value);
  set_gdbarch_store_return_value (gdbarch, ubicom32_store_return_value);
#endif

  set_gdbarch_return_value (gdbarch, ubicom32_return_value);

  set_gdbarch_decr_pc_after_break (gdbarch, 0);
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);
  set_gdbarch_skip_prologue (gdbarch, ubicom32_skip_prologue);
  frame_base_set_default (gdbarch, &ubicom32_frame_base);

  /* Install sniffer for linux kernel entry points.  */
  frame_unwind_append_sniffer (gdbarch, ubicom32_kernel_sniffer);

  /* Hook in the DWARF CFI frame unwinder.  */
  frame_base_append_sniffer (gdbarch, dwarf2_frame_base_sniffer);
  //frame_unwind_append_sniffer (gdbarch, dwarf2_frame_sniffer);

  frame_unwind_append_sniffer (gdbarch, ubicom32_frame_base_sniffer);

  set_gdbarch_unwind_dummy_id (gdbarch, ubicom32_unwind_dummy_id);

  set_gdbarch_frame_args_skip (gdbarch, 0);

  set_gdbarch_unwind_pc(gdbarch, ubicom32_unwind_pc);
  set_gdbarch_unwind_sp(gdbarch, ubicom32_unwind_sp);
  set_gdbarch_print_insn (gdbarch, print_insn_ubicom32);
  set_gdbarch_regset_from_core_section (gdbarch,
                                        ubicom32_linux_regset_from_core_section);
  set_gdbarch_push_dummy_call (gdbarch, ubicom32_push_dummy_call);

  return gdbarch;
}

/* Initialize the current architecture based on INFO.  If possible, re-use an
   architecture from ARCHES, which is a list of architectures already created
   during this debugging session.

   Called e.g. at program startup, when reading a core file, and when reading
   a binary file. */

static struct gdbarch *
ubicom32v3_arch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  struct gdbarch *gdbarch;
  gdbarch = ubicom32_arch_init_common(&info, "Ubicom32 Version 3",
				      UBICOM32_ABI_FLAT);

  set_gdbarch_sp_regnum (gdbarch, UBICOM32V3_SP_REGNUM);
  set_gdbarch_pc_regnum (gdbarch, UBICOM32V3_PC_REGNUM);
  set_gdbarch_num_regs (gdbarch, NUM_UBICOM32V3_REGS);
  set_gdbarch_breakpoint_from_pc (gdbarch, ubicom32_breakpoint_from_pc);
  mt_active_regnum = UBICOM32V3_MT_ACTIVE_REGNUM;
  mt_en_regnum = UBICOM32V3_MT_EN_REGNUM;
  mt_pri_regnum = UBICOM32V3_MT_PRI_REGNUM;
  mt_sched_regnum = UBICOM32V3_MT_SCHED_REGNUM;
  return gdbarch;
}

static struct gdbarch *
ubicom32v3_ver4_arch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  struct gdbarch *gdbarch;
  gdbarch = ubicom32_arch_init_common(&info, "Ubicom32 Version 4",
				      UBICOM32_ABI_FLAT);

  set_gdbarch_sp_regnum (gdbarch, UBICOM32V3_SP_REGNUM);
  set_gdbarch_pc_regnum (gdbarch, UBICOM32V3_PC_REGNUM);
  set_gdbarch_num_regs (gdbarch, NUM_UBICOM32V3_VER4_REGS);
  set_gdbarch_breakpoint_from_pc (gdbarch, ubicom32_breakpoint_from_pc);
  mt_active_regnum = UBICOM32V3_MT_ACTIVE_REGNUM;
  mt_pri_regnum = UBICOM32V3_MT_PRI_REGNUM;
  mt_sched_regnum = UBICOM32V3_MT_SCHED_REGNUM;
  mt_en_regnum = UBICOM32V3_MT_EN_REGNUM;
  return gdbarch;
}

static struct gdbarch *
ubicom32v3_ver5_arch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  struct gdbarch *gdbarch;
  gdbarch = ubicom32_arch_init_common(&info, "Ubicom32 Version 5",
				      UBICOM32_ABI_FLAT);

  set_gdbarch_sp_regnum (gdbarch, UBICOM32V3_SP_REGNUM);
  set_gdbarch_pc_regnum (gdbarch, UBICOM32V5_PC_REGNUM);
  set_gdbarch_num_regs (gdbarch, NUM_UBICOM32V3_VER5_REGS);
  set_gdbarch_breakpoint_from_pc (gdbarch, ubicom32_breakpoint_from_pc);
  mt_active_regnum = UBICOM32V5_MT_ACTIVE_REGNUM;
  mt_en_regnum = UBICOM32V5_MT_EN_REGNUM;
  mt_pri_regnum = UBICOM32V5_MT_PRI_REGNUM;
  mt_sched_regnum = UBICOM32V5_MT_SCHED_REGNUM;
  return gdbarch;
}

static struct gdbarch *
ubicom32_posix_arch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  struct gdbarch *gdbarch;
  gdbarch = ubicom32_arch_init_common(&info, "Ubicom32 POSIX",
				      UBICOM32_ABI_FLAT);

  set_gdbarch_sp_regnum (gdbarch, UBICOM32V3_SP_REGNUM);
  set_gdbarch_pc_regnum (gdbarch, UBICOM32_POSIX_PC_REGNUM);
  set_gdbarch_num_regs (gdbarch, NUM_UBICOM32_POSIX_REGS);
//// NAT this is incorrect for posix. is it really needed?
  set_gdbarch_breakpoint_from_pc (gdbarch, ubicom32_breakpoint_from_pc);

  return gdbarch;
}

static struct gdbarch *
ubicom32_uclinux_arch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  struct gdbarch *gdbarch;
  enum ubicom32_abi abi;
  int elf_flags;

  /* Extract the ELF flags, if available.  */
  if (info.abfd && bfd_get_flavour (info.abfd) == bfd_target_elf_flavour)
    elf_flags = elf_elfheader (info.abfd)->e_flags;
  else
    elf_flags = 0;

  if (elf_flags & EF_UBICOM32_FDPIC)
    abi = UBICOM32_ABI_FDPIC;
  else
    abi = UBICOM32_ABI_FLAT;

  gdbarch = ubicom32_arch_init_common(&info, "Ubicom32 uClinux", abi);

  set_gdbarch_sp_regnum (gdbarch, UBICOM32V3_SP_REGNUM);
  set_gdbarch_pc_regnum (gdbarch, UBICOM32_UCLINUX_PC_REGNUM);
  set_gdbarch_num_regs (gdbarch, NUM_UBICOM32_UCLINUX_REGS);
  set_gdbarch_breakpoint_from_pc (gdbarch, ubicom32_software_breakpoint_from_pc);

  /* uCLinux cannot do single step in kernel so we need to do it in
   * software */
  set_gdbarch_software_single_step (gdbarch, ubicom32_software_single_step);

  if (ubicom32_abi (gdbarch) == UBICOM32_ABI_FDPIC)
    {
      set_gdbarch_convert_from_func_ptr_addr (gdbarch,
					      ubicom32_convert_from_func_ptr_addr);
    }

  /* Enable TLS support.  */
  set_gdbarch_fetch_tls_load_module_address (gdbarch,
					     ubicom32_fetch_objfile_link_map);

  return gdbarch;
}

struct gdbarch * ubicom32_arch_init(struct gdbarch_info info, struct gdbarch_list *arches)
{
  struct gdbarch *gdbarch;
  gdbarch = ubicom32_arch_init_common(&info, "Ubicom32 Version 2",
				      UBICOM32_ABI_FLAT);

  set_gdbarch_sp_regnum (gdbarch, UBICOM32_SP_REGNUM);
  set_gdbarch_pc_regnum (gdbarch, UBICOM32_PC_REGNUM);
  set_gdbarch_num_regs (gdbarch, NUM_UBICOM32_REGS);
  set_gdbarch_breakpoint_from_pc (gdbarch, ubicom32_breakpoint_from_pc);
  mt_active_regnum = UBICOM32_MT_ACTIVE_REGNUM;
  mt_pri_regnum = UBICOM32_MT_EN_REGNUM;
  mt_sched_regnum = UBICOM32_MT_PRI_REGNUM;
  mt_en_regnum = UBICOM32_MT_SCHED_REGNUM;
  return gdbarch;
}

char *bfd_print_names[] = {
  "ubicom32",
  "ubicom32dsp",
  "ubicom32ver4",
  "ubicom32posix",
  "ubicom32uclinux",
  "ubicom32ver5",
  NULL
};

struct ubicom32_machine machines[] = {
  { ubicom32_regs, NUM_UBICOM32_REGS, NULL, {NULL, {0, 0}}, ubicom32_arch_init, ubicom32_frame_chain_valid, UBICOM32_PC_REGNUM, UBICOM32_SP_REGNUM, NULL}, /* ubicom32 mercury entry */
  { ubicom32v3_regs, NUM_UBICOM32V3_REGS, NULL, {NULL, {0, 0}}, ubicom32v3_arch_init, ubicom32v3_frame_chain_valid, UBICOM32V3_PC_REGNUM, UBICOM32V3_SP_REGNUM, NULL}, /* ubicom32 mars entry */
  { ubicom32v3_ver4_regs, NUM_UBICOM32V3_VER4_REGS, NULL, {NULL, {0, 0}}, ubicom32v3_ver4_arch_init, ubicom32v3_frame_chain_valid, UBICOM32V3_PC_REGNUM, UBICOM32V3_SP_REGNUM, NULL}, /* ubicom32 ares entry */
  /* ipPOSIX */
  { ubicom32_posix_regs, NUM_UBICOM32_POSIX_REGS, NULL, {NULL, {0, 0}},
    ubicom32_posix_arch_init, ubicom32_posix_frame_chain_valid,
    UBICOM32_POSIX_PC_REGNUM, UBICOM32V3_SP_REGNUM, NULL}, /* ubicom32 posix entry */
  /* Linux without mmu */
  { ubicom32_uclinux_regs, NUM_UBICOM32_UCLINUX_REGS, NULL, {NULL, {0, 0}},
    ubicom32_uclinux_arch_init, ubicom32_posix_frame_chain_valid,
    UBICOM32_UCLINUX_PC_REGNUM, UBICOM32V3_SP_REGNUM, NULL}, /* ubicom32 uclinux entry */
  { ubicom32v3_ver5_regs, NUM_UBICOM32V3_VER5_REGS, NULL, {NULL, {0, 0}}, ubicom32v3_ver5_arch_init, ubicom32v5_frame_chain_valid, UBICOM32V5_PC_REGNUM, UBICOM32V3_SP_REGNUM, ubicom32v5_translate_address} /* ubicom32 jupiter entry */
};


static struct gdbarch *
ubicom32_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  int i;

  int elf_flags = 0;
  char *printable_name = NULL;

  if (info.abfd) 
    elf_flags = elf_elfheader (info.abfd)->e_flags;

  /* Find a candidate among extant architectures.  Only one architecture so
     far, so use simple search. */
  arches = gdbarch_list_lookup_by_info (arches, &info);
  if (arches != NULL)
    {
      return arches->gdbarch;
    }

  printable_name = info.bfd_arch_info->printable_name;

  /* hunt and find the entry that matches the printable name. */
  i = 0;
  while (bfd_print_names[i])
    {
      if (!strcmp(bfd_print_names[i], printable_name))
	break;

      i++;
    }

  if (bfd_print_names[i])
    {
      current_machine = &machines[i];
      /* Set address translation routine for core files.  
         Target is handled in ubicom32v3_open().  */
      core_ops.to_translate_address = current_machine->translate_address;
    }
  else
    error("Unable to find a machine entry that matches the selected bfd.");

  if(current_machine->gdbarch)
    {
      return current_machine->gdbarch;
    }

  current_machine->gdbarch = current_machine->arch_init_fn(info, arches);

  return current_machine->gdbarch;
}

/* Unpack an ubicom32_elf_fpregset_t into GDB's register cache.  */
static void
swizzled_supply_register (struct regcache *regcache, int regnum, const void *val)
{
  int i, j;
  //char * value = (char *)malloc(REGISTER_RAW_SIZE (regnum));
  char out_val[MAX_REGISTER_SIZE];
  char *value = out_val;
  char *ptr = (char *)val;

  for(i= 3, j=0;i >= 0; i--)
    {
      value[j++] = ptr[i];
    }

  regcache_raw_supply(regcache, regnum, (const void *)value);
  //free(value);
}


static void 
ubicom32_linux_supply_gregset (const struct regset *regset,
			       struct regcache *regcache,
			       int regnum, const void *gregs, size_t len)
{
  int regi;
  int *ptr = (int *) gregs;
  char *loadmap_ptr = (char *) gregs;
  int core_eflags = (elf_elfheader(core_bfd)->e_flags) & 0xffff;
  int *ptr2;
  int previous_pc = ptr[UBICOM32_PREV_PC_IN_PTREGS];

  loadmap_ptr += (len - 12);
  ptr2 = (int *)loadmap_ptr;

  /*
   * For 8k and future cpus register Data for CSR is the UCSR data in pt_regs.
   * The core_flags contains the ISA encoding. 2 = 7K 4 = 8K.
   */
  if (core_eflags > 2) {
    ptr[UBICOM32_FDPIC_CSR_REGNUM] = ptr[UBICOM32_UCSR_IN_PTREGS];
  }

  for (regi = 0; regi < current_machine->num_regs - 3;)
    {
      //swizzled_supply_register(regcache, regi, ptr++);
      regcache_raw_supply(regcache, regi++, (const void *)ptr++);
    }

  regcache_raw_supply(regcache, regi++, (const void *)&ptr2[0]);
  regcache_raw_supply(regcache, regi++, (const void *)&ptr2[1]);
  regcache_raw_supply(regcache, regi++, (const void *)&previous_pc);
}

void
create_ptregs_from_regcache (gdb_byte *destination, struct regcache *regcache)
{
  int i;
  char *regs = malloc(4);
  int core_eflags = elf_elfheader(core_bfd)->e_flags & 0xffff;

  /* Copy d0 - mac_rc16 */
  for (i = 0; i < 27; i++) {
    /* pull the data from regcache */
    regcache_raw_collect (regcache, i, regs);

    memcpy(destination, regs, 4);
    destination += 4;
  }

  /* Copy acc1hi and acc1lo */
  regcache_raw_collect (regcache, (current_machine->pc_regnum + 2), regs);

  memcpy(destination, regs, 4);
  destination += 4;

  regcache_raw_collect (regcache, (current_machine->pc_regnum + 3), regs);
  memcpy(destination, regs, 4);

  destination += 4;

  /* Copy source 3 */
  regcache_raw_collect (regcache, 27, regs);
  memcpy(destination, regs, 4);

  destination += 8;

  regcache_raw_collect (regcache, 29, regs);

  memcpy(destination, regs, 4);

  destination += 16;

  /* Get trap_cause */
  regcache_raw_collect (regcache, (current_machine->pc_regnum + 1), regs);
  memcpy(destination, regs, 4);

  destination += 4;

  /* Get PC */
  regcache_raw_collect (regcache, (current_machine->pc_regnum ), regs);
  memcpy(destination, regs, 4);

  destination += 12;

  /* Get previous PC. */
  regcache_raw_collect (regcache, (current_machine->pc_regnum + 4), regs);
  memcpy(destination, regs, 4);

  destination += 16;

  if (core_eflags > 2) {
    /* 8k + Get UCSR */
    regcache_raw_collect (regcache, 40, regs);

    memcpy(destination, regs, 4);
  }
}

static void
ubicom32_kernel_supply_gregset (const struct regset *regset,
				struct regcache *regcache,
				int regnum, const void *gregs, size_t len)
{
  int regi;
  int *ptr = (int *) gregs;

  for (regi = 0; regi < current_machine->num_regs;)
    {
      swizzled_supply_register(regcache, regi++, ptr++);
      //regcache_raw_supply(regcache, regi++, (const void *)ptr++);
    }
}

/* UBICOM32 Linux kernel register sets.  */

static char *
ubicom32v3_thread_pid_to_str (ptid_t ptid)
{
  static char buf[30];

  sprintf (buf, "Thread %d", (PIDGET (ptid))-1);
  return buf;
}


static const struct regset ubicom32_linux_gregset =
{
  NULL,
  ubicom32_linux_supply_gregset
};


static const struct regset ubicom32_kernel_gregset =
{
  NULL,
  ubicom32_kernel_supply_gregset
};

static const struct regset *
ubicom32_linux_regset_from_core_section (struct gdbarch *gdbarch,
				    const char *sect_name, size_t sect_size)
{
  struct gdbarch_info info;
  extern void gdbarch_info_init (struct gdbarch_info *info);

  if (strcmp (sect_name, ".reg") == 0) {
    int core_eflags = elf_elfheader(core_bfd)->e_flags;

    if (core_eflags & 0x40000000) {
      /* Looks like a linux core dump. */
      struct gdbarch_tdep *tdep = gdbarch_tdep(current_gdbarch);
      char *arch_string = (char *)tdep->name;

      if (strcmp(arch_string, "Ubicom32 uClinux"))
	{
	  /* Architecture mismatch. We need to go find the "ubicom32uclinux" arch. */
	  gdbarch_info_init (&info);
	  info.bfd_arch_info = bfd_scan_arch ("ubicom32uclinux");
	  
	  if (info.bfd_arch_info == NULL)
	    internal_error (__FILE__, __LINE__,
			    "set_architecture: bfd_scan_arch failed");
	  (void)gdbarch_update_p (info);
	}

      return &ubicom32_linux_gregset;
    } else {
      /* Looks like a kernel dump. Hijack some of the current_target ops. */
      current_target.to_pid_to_str = ubicom32v3_thread_pid_to_str;
      current_target.to_extra_thread_info = ubicom32v3_remote_threads_extra_info;
      return &ubicom32_kernel_gregset;
    }

  }

  return NULL;
}

/* Module initialization.  */

void
_initialize_ubicom32_tdep (void)
{
  int i;
  /* Initialize each register set regdisp entry.   */
#if 0
  if (void_func_ptr == NULL)
    void_func_ptr = builtin_type_void_func_ptr;
#endif
  /*
   * Initialize the machine structure.
   */
  init_regsdisp(&machines[0]);
  init_regsdisp(&machines[1]);
  init_regsdisp(&machines[2]);
  init_regsdisp(&machines[3]);
  init_regsdisp(&machines[4]);
  init_regsdisp(&machines[5]);

  gdbarch_register (bfd_arch_ubicom32, ubicom32_gdbarch_init, NULL);
}
