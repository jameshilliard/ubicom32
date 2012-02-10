/* Subroutines for insn-output.c for Ubicom32

   Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008,
   2009, 2010 Free Software Foundation, Inc.
   Contributed by Ubicom, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "rtl.h"
#include "tree.h"
#include "regs.h"
#include "hard-reg-set.h"
#include "real.h"
#include "insn-config.h"
#include "conditions.h"
#include "insn-flags.h"
#include "output.h"
#include "insn-attr.h"
#include "insn-codes.h"
#include "flags.h"
#include "recog.h"
#include "expr.h"
#include "function.h"
#include "obstack.h"
#include "toplev.h"
#include "tm_p.h"
#include "tm-constrs.h"
#include "basic-block.h"
#include "integrate.h"
#include "target.h"
#include "target-def.h"
#include "reload.h"
#include "df.h"
#include "langhooks.h"
#include "optabs.h"

static int ubicom32_reloc_rw_mask (void);
static tree ubicom32_handle_fndecl_attribute (tree *, tree, tree, int, bool *);
static void ubicom32_function_prologue (FILE *, HOST_WIDE_INT);
static void ubicom32_function_epilogue (FILE *, HOST_WIDE_INT);
static section * ubicom32_function_rodata_section (tree);
static int ubicom32_address_cost (rtx, bool);
static bool ubicom32_rtx_costs (rtx, int, int, int *, bool speed);
static bool ubicom32_fixed_condition_code_regs (unsigned int *,
						unsigned int *);
static enum machine_mode ubicom32_cc_modes_compatible (enum machine_mode,
						       enum machine_mode);
static int ubicom32_naked_function_p (void);
static void ubicom32_machine_dependent_reorg (void);
static bool ubicom32_assemble_integer (rtx, unsigned int, int);
static int ubicom32_arg_partial_bytes (CUMULATIVE_ARGS *, enum machine_mode, tree,
				       bool);
static bool ubicom32_pass_by_reference (CUMULATIVE_ARGS *, enum machine_mode,
					const_tree, bool);
static bool ubicom32_callee_copies (CUMULATIVE_ARGS *, enum machine_mode mode,
				    const_tree type, bool);

static bool ubicom32_return_in_memory (const_tree, const_tree);
static bool ubicom32_is_base_reg (rtx, int);
static void ubicom32_init_builtins (void);
static rtx ubicom32_expand_builtin (tree, rtx, rtx, enum machine_mode, int);
static tree ubicom32_fold_builtin (tree, tree, bool);
static int ubicom32_get_valid_offset_mask (enum machine_mode);
static bool ubicom32_cannot_force_const_mem (rtx);
static int ubicom32_multiply_dep_p (rtx, rtx);
static int ubicom32_fpu_dep_p (rtx, rtx);
static unsigned char ubicom32_function_ok_for_sibcall (tree decl, tree exp);
static bool ubicom32_handle_option (size_t, const char *, int);

/* Nonzero if this chip supports the Ubicom32 v3 ISA.  */
int ubicom32_v3 = 1;

/* Nonzero if this chip supports the Ubicom32 v4 ISA.  */
int ubicom32_v4 = 1;

/* Nonzero if this chip supports the Ubicom32 v5 ISA.  */
int ubicom32_v5 = 0;

/* Scheduling rate in terms of expected concurrent threads.  */
int ubicom32_sched_rate = 0;

/* Which instruction set architecture to use.  */
enum ubicom32_processor_type ubicom32_arch = UBICOM32_V4;

/* Valid attributes:
   naked - don't generate function prologue/epilogue and `ret' command.  */
const struct attribute_spec ubicom32_attribute_table[] =
{
  /* { name, min_len, max_len, decl_req, type_req, fn_type_req, handler } */
  { "naked", 0, 0, true,  false, false, ubicom32_handle_fndecl_attribute },
  { NULL,    0, 0, false, false, false, NULL }
};

#undef TARGET_ASM_RELOC_RW_MASK
#define TARGET_ASM_RELOC_RW_MASK ubicom32_reloc_rw_mask

#undef TARGET_ASM_FUNCTION_PROLOGUE
#define TARGET_ASM_FUNCTION_PROLOGUE ubicom32_function_prologue

#undef TARGET_ASM_FUNCTION_EPILOGUE
#define TARGET_ASM_FUNCTION_EPILOGUE ubicom32_function_epilogue

#undef TARGET_ASM_FUNCTION_RODATA_SECTION
#define TARGET_ASM_FUNCTION_RODATA_SECTION ubicom32_function_rodata_section

#undef TARGET_ATTRIBUTE_TABLE
#define TARGET_ATTRIBUTE_TABLE ubicom32_attribute_table

#undef TARGET_ADDRESS_COST
#define TARGET_ADDRESS_COST ubicom32_address_cost

#undef TARGET_RTX_COSTS
#define TARGET_RTX_COSTS ubicom32_rtx_costs

#undef TARGET_FIXED_CONDITION_CODE_REGS
#define TARGET_FIXED_CONDITION_CODE_REGS ubicom32_fixed_condition_code_regs

#undef TARGET_CC_MODES_COMPATIBLE
#define TARGET_CC_MODES_COMPATIBLE ubicom32_cc_modes_compatible

#undef TARGET_MACHINE_DEPENDENT_REORG
#define TARGET_MACHINE_DEPENDENT_REORG ubicom32_machine_dependent_reorg

#undef TARGET_FUNCTION_OK_FOR_SIBCALL
#define TARGET_FUNCTION_OK_FOR_SIBCALL ubicom32_function_ok_for_sibcall

#undef TARGET_ASM_INTEGER
#define TARGET_ASM_INTEGER ubicom32_assemble_integer

#undef TARGET_ARG_PARTIAL_BYTES
#define TARGET_ARG_PARTIAL_BYTES ubicom32_arg_partial_bytes

#undef TARGET_PASS_BY_REFERENCE
#define TARGET_PASS_BY_REFERENCE ubicom32_pass_by_reference

#undef TARGET_CALLEE_COPIES
#define TARGET_CALLEE_COPIES ubicom32_callee_copies

#undef TARGET_RETURN_IN_MEMORY
#define TARGET_RETURN_IN_MEMORY ubicom32_return_in_memory

#undef TARGET_INIT_BUILTINS
#define TARGET_INIT_BUILTINS ubicom32_init_builtins

#undef TARGET_EXPAND_BUILTIN
#define TARGET_EXPAND_BUILTIN ubicom32_expand_builtin

#undef TARGET_FOLD_BUILTIN
#define TARGET_FOLD_BUILTIN ubicom32_fold_builtin

#undef TARGET_CANNOT_FORCE_CONST_MEM
#define TARGET_CANNOT_FORCE_CONST_MEM ubicom32_cannot_force_const_mem

#undef TARGET_MAX_ANCHOR_OFFSET
#define TARGET_MAX_ANCHOR_OFFSET 0x1ff

#undef TARGET_DEFAULT_TARGET_FLAGS 
#define TARGET_DEFAULT_TARGET_FLAGS TARGET_DEFAULT

#undef TARGET_HANDLE_OPTION
#define TARGET_HANDLE_OPTION ubicom32_handle_option

struct gcc_target targetm = TARGET_INITIALIZER;

static int prologue_epilogue_type;
static bool save_link_reg;
static char save_regs_order[FIRST_PSEUDO_REGISTER];
static int save_regs;
static int save_regs_size;

#define STACK_UNIT_BOUNDARY (STACK_BOUNDARY / BITS_PER_UNIT)
#define ROUND_CALL_BLOCK_SIZE(BYTES) \
  (((BYTES) + (STACK_UNIT_BOUNDARY - 1)) & ~(STACK_UNIT_BOUNDARY - 1))

/* In case of a PRE_INC, POST_INC, PRE_DEC, POST_DEC memory reference, we
   must report the mode of the memory reference from PRINT_OPERAND to
   PRINT_OPERAND_ADDRESS.  */
enum machine_mode output_memory_reference_mode;

enum reg_class const ubicom32_regclass_map[FIRST_PSEUDO_REGISTER] =
{
  DATA_REGS,		/*  0 - d0  */
  DATA_REGS,		/*  1 - d1  */
  DATA_REGS,		/*  2 - d2  */
  DATA_REGS,		/*  3 - d3  */
  DATA_REGS,		/*  4 - d4  */
  DATA_REGS,		/*  5 - d5  */
  DATA_REGS,		/*  6 - d6  */
  DATA_REGS,		/*  7 - d7  */
  DATA_REGS,		/*  8 - d8  */
  DATA_REGS,		/*  9 - d9  */
  DATA_REGS,		/* 10 - d10 */
  DATA_REGS,		/* 11 - d11 */
  DATA_REGS,		/* 12 - d12 */
  DATA_REGS,		/* 13 - d13 */
  DATA_REGS,		/* 14 - d14 */
  DATA_REGS,		/* 15 - d15 */
  FDPIC_REG,		/* 16 - a0  */
  ADDRESS_REGS,		/* 17 - a1  */
  ADDRESS_REGS,		/* 18 - a2  */
  SIBCALL_ADDRESS_REGS,	/* 19 - a3  */
  SIBCALL_ADDRESS_REGS,	/* 20 - a4  */
  LINK_REG,		/* 21 - a5  */
  ADDRESS_REGS,		/* 22 - a6  */
  ADDRESS_REGS,		/* 23 - a7  */
  ACC_REGS,		/* 24 - acc0_hi */
  ACC_LO_REGS,		/* 25 - acc0_lo */
  ACC_REGS,		/* 26 - acc1_hi */
  ACC_LO_REGS,		/* 27 - acc1_lo */
  SOURCE3_REG,		/* 28 */
  ADDRESS_REGS,		/* 29 */
  NO_REGS,		/* CC_REG must be NO_REGS */
  SPECIAL_REGS,		/* 31 */
  SPECIAL_REGS,		/* 32 */
  SPECIAL_REGS,		/* 33 */
  SPECIAL_REGS,		/* 34 */
  SPECIAL_REGS,		/* 35 */
  SPECIAL_REGS,		/* 36 */
  SPECIAL_REGS,		/* 37 */
  SPECIAL_REGS		/* 38 */
};

rtx ubicom32_compare_op0;
rtx ubicom32_compare_op1;

static int force_soft_float;

/* Handle options. */
static bool
ubicom32_handle_option (size_t code, const char *arg ATTRIBUTE_UNUSED, 
			int value ATTRIBUTE_UNUSED)
{
  switch (code)
    {
    case OPT_msoft_float:
      force_soft_float = 1;
      break;
    }
  return true;
}

/* Handle command line option overrides.  */

void
ubicom32_override_options (void)
{
  flag_pic = 0;

  if (strcmp (ubicom32_arch_name, "ubicom32v1") == 0)
    {
      /* If we have a version 1 architecture then we want to avoid using jump
	 tables.  */
      ubicom32_v3 = 0;
      ubicom32_v4 = 0;
      ubicom32_v5 = 0;
      ubicom32_arch = UBICOM32_V3;
    }
  else if (strcmp (ubicom32_arch_name, "ubicom32v2") == 0)
    {
      ubicom32_v3 = 0;
      ubicom32_v4 = 0;
      ubicom32_v5 = 0;
      ubicom32_arch = UBICOM32_V3;
    }
  else if (strcmp (ubicom32_arch_name, "ubicom32v3") == 0)
    {
      ubicom32_v3 = 1;
      ubicom32_v4 = 0;
      ubicom32_v5 = 0;
      ubicom32_arch = UBICOM32_V3;
    }
  else if (strcmp (ubicom32_arch_name, "ubicom32v4") == 0)
    {
      ubicom32_v3 = 1;
      ubicom32_v4 = 1;
      ubicom32_v5 = 0;
      ubicom32_arch = UBICOM32_V4;
    }
  else if (strcmp (ubicom32_arch_name, "ubicom32v5") == 0)
    {
      ubicom32_v3 = 1;
      ubicom32_v4 = 1;
      ubicom32_v5 = 1;
      ubicom32_arch = UBICOM32_V5;
      if (!force_soft_float) 
	target_flags &= ~MASK_SOFT_FLOAT;
      ubicom32_tune = TUNE_SHUTTLE_IP8K;
    }

  /*  Only ubicom32ver5 can have HARD_FLOAT.  */
  if (TARGET_HARD_FLOAT && !ubicom32_v5) 
    {
      warning (0, "-mhard_float invalid for -march=%s", ubicom32_arch_name);
      target_flags |= MASK_SOFT_FLOAT;
    }

  if (strcmp (ubicom32_sched_rate_name, "1") == 0)
    ubicom32_sched_rate = 1;
  else if (strcmp (ubicom32_sched_rate_name, "2") == 0)
    ubicom32_sched_rate = 2;
  else if (strcmp (ubicom32_sched_rate_name, "3") == 0)
    ubicom32_sched_rate = 3;
  else if (strcmp (ubicom32_sched_rate_name, "4") == 0)
    ubicom32_sched_rate = 4;
  else
    ubicom32_sched_rate = 0;

  /* There is no single unaligned SI op for PIC code.  Sometimes we
     need to use ".4byte" and sometimes we need to use ".picptr".
     See ubicom32_assemble_integer for details.  */
  if (TARGET_FDPIC)
    targetm.asm_out.unaligned_op.si = 0;

  /* Supply a default value for align_functions.  */
  if (align_functions == 0 && !optimize_size)
    align_functions = 32;

  /* The priority register allocator works better for the Ubicom32 ISA,
     but newer versions of the compiler should eliminate this.  */
  flag_ira_algorithm = IRA_ALGORITHM_PRIORITY;

  /* RTL sequence abstractions have been removed from later versions of
     the compiler.  Prevent them from causing test failures now.  */
  flag_rtl_seqabstr = 0;
}

void
ubicom32_conditional_register_usage (void)
{
  /* If we're using the old ipOS ABI we need to make D10 through D13
     caller-clobbered.  */
  if (TARGET_IPOS_ABI)
    {
      call_used_regs[D10_REGNUM] = 1;
      call_used_regs[D11_REGNUM] = 1;
      call_used_regs[D12_REGNUM] = 1;
      call_used_regs[D13_REGNUM] = 1;
    }

  if (TARGET_UBICOM32_TLS)
    {
      fixed_regs[THREAD_REGNUM] = 1;
    }
}

/* We have some number of optimizations that don't really work for the Ubicom32
   architecture so we deal with them here.  */

void
ubicom32_optimization_options (int level ATTRIBUTE_UNUSED,
			       int size ATTRIBUTE_UNUSED)
{
  /* The tree IVOPTs pass seems to do really bad things for the Ubicom32
     architecture - it tends to turn things that would happily use pre/post
     increment/decrement into operations involving unecessary loop
     indicies.  */
  flag_ivopts = 0;

  /* We have problems where DSE at the RTL level misses partial stores
     to the stack.  For now we disable it to avoid this.  */
  flag_dse = 0;

#if 0
// XXX - this really should be profitable but crashes Linux right now.
  /* Enable section anchors by default at -O1 or higher.
     Use 2 to distinguish from an explicit -fsection-anchors
     given on the command line.  */
  if (level > 0)
    flag_section_anchors = 2;
#endif
}

/* For FD-PIC, relocations to local or global data cannot go in read-only
   segments.  */

static int
ubicom32_reloc_rw_mask (void)
{
  return TARGET_FDPIC ? 3 : 0;
}

/* Print operand X using operand code CODE to assembly language output file
   FILE.  */

void
ubicom32_print_operand (FILE *file, rtx x, int code)
{
  switch (code)
    {
    case 'h':
      switch (REGNO (x))
	{
	  case ACC0_HI_REGNUM:
	    fprintf (file, "acc0_hi");  break;
	  case ACC0_LO_REGNUM:
	    fprintf (file, "acc0_lo");  break;
	  case ACC1_HI_REGNUM:
	    fprintf (file, "acc1_hi");  break;
	  case ACC1_LO_REGNUM:
	    fprintf (file, "acc1_lo");  break;
	}
      break;

    case 'A':
      /* Identify the correct accumulator to use.  */
      if (REGNO (x) == ACC0_HI_REGNUM || REGNO (x) == ACC0_LO_REGNUM)
	fprintf (file, "acc0");
      else if (REGNO (x) == ACC1_HI_REGNUM || REGNO (x) == ACC1_LO_REGNUM)
	fprintf (file, "acc1");
      else
	fprintf (file, "%s", reg_names[REGNO (x)]);
      break;

    case 'b':
    case 'B':
      {
	enum machine_mode mode;

	mode = GET_MODE (XEXP (x, 0));

	/* These are normal and reversed branches.  */
	switch (code == 'b' ? GET_CODE (x) : reverse_condition (GET_CODE (x)))
	  {
	  case NE:
	    fprintf (file, "ne");
	    break;

	  case EQ:
	    fprintf (file, "eq");
	    break;

	  case GE:
	    if (mode == CCSZNmode || mode == CCWZNmode)
	      fprintf (file, "pl");
	    else
	      fprintf (file, "ge");
	    break;

	  case GT:
	    fprintf (file, "gt");
	    break;

	  case LE:
	    fprintf (file, "le");
	    break;

	  case LT:
	    if (mode == CCSZNmode || mode == CCWZNmode)
	      fprintf (file, "mi");
	    else
	      fprintf (file, "lt");
	    break;

	  case GEU:
	    fprintf (file, "cs");
	    break;

	  case GTU:
	    fprintf (file, "hi");
	    break;

	  case LEU:
	    fprintf (file, "ls");
	    break;

	  case LTU:
	    fprintf (file, "cc");
	    break;

	  default:
	    abort ();
	  }
      }
      break;

    case 'C':
      /* This is used for the operand to a call instruction;
	 if it's a REG, enclose it in parens, else output
	 the operand normally.  */
      if (REG_P (x))
	{
	  fputc ('(', file);
	  ubicom32_print_operand (file, x, 0);
	  fputc (')', file);
	}
      else
	ubicom32_print_operand (file, x, 0);
      break;

    case 'd':
      /* Bit operations we need bit numbers. */
      fprintf (file, "%d", exact_log2 (0xffffffff & INTVAL (x)));
      break;

    case 'D':
      /* Bit operations we need bit numbers. */
      fprintf (file, "%d", exact_log2 (0xffffffff & ~INTVAL (x)));
      break;

    case 'E':
      /* For lea, which we use to add address registers.
	 We don't want the '#' on a constant. */
      if (CONST_INT_P (x))
	{
	  fprintf (file, "%ld", INTVAL (x));
	  break;
	}
      else if (GET_CODE (x) == UNSPEC)
	{
	  switch (XINT (x, 1))
	    {
	    case UNSPEC_FDPIC_GOT_1:
	    case UNSPEC_FDPIC_GOT_4:
	      fprintf (file, "%%got_lo(");
	      ubicom32_print_operand_address (file, XVECEXP (x, 0, 0));
	      fprintf (file, ")");
	      break;

	    case UNSPEC_FDPIC_FUNCDESC_GOT_1:
	    case UNSPEC_FDPIC_FUNCDESC_GOT_4:
	      fprintf (file, "%%funcdesc_got_lo(");
	      ubicom32_print_operand_address (file, XVECEXP (x, 0, 0));
	      fprintf (file, ")");
	      break;

	    default:
	      abort ();
	    }
	  break;
	}

      /* FALL THROUGH */

    default:
      switch (GET_CODE (x))
	{
	case MEM:
	  output_memory_reference_mode = GET_MODE (x);
	  output_address (XEXP (x, 0));
	  break;

	case PLUS:
	  output_address (x);
	  break;

	case REG:
	  fprintf (file, "%s", reg_names[REGNO (x)]);
	  break;

	case SUBREG:
	  fprintf (file, "%s", reg_names[subreg_regno (x)]);
	  break;

	/* This will only be single precision....  */
	case CONST_DOUBLE:
	  {
	    unsigned long val;
	    REAL_VALUE_TYPE rv;

	    REAL_VALUE_FROM_CONST_DOUBLE (rv, x);
	    REAL_VALUE_TO_TARGET_SINGLE (rv, val);
	    fprintf (file, "0x%lx", val);
	    break;
	  }

	case CONST_INT:
	case SYMBOL_REF:
	case LABEL_REF:
	case CODE_LABEL:
	case LO_SUM:
	case CONST:
	  ubicom32_print_operand_address (file, x);
	  break;

	case UNSPEC:
	  switch (XINT (x, 1))
	    {
	    case UNSPEC_FDPIC_GOT_1:
	    case UNSPEC_FDPIC_GOT_4:
	      fprintf (file, "#%%got_lo(");
	      ubicom32_print_operand_address (file, XVECEXP (x, 0, 0));
	      fprintf (file, ")");
	      break;

	    case UNSPEC_FDPIC_FUNCDESC_GOT_1:
	    case UNSPEC_FDPIC_FUNCDESC_GOT_4:
	      fprintf (file, "#%%funcdesc_got_lo(");
	      ubicom32_print_operand_address (file, XVECEXP (x, 0, 0));
	      fprintf (file, ")");
	      break;

	    default:
	      abort ();
	    }
	  break;

	case HIGH:
	  fprintf (file, "#%%hi(");
	  ubicom32_print_operand_address (file, XEXP (x, 0));
	  fprintf (file, ")");
	  break;

	default:
	  abort ();
	}
      break;
   }
}

/* Output assembly language output for the address ADDR to FILE.  */

void
ubicom32_print_operand_address (FILE *file, rtx addr)
{
  switch (GET_CODE (addr))
    {
    case POST_INC:
      ubicom32_print_operand_address (file, XEXP (addr, 0));
      fprintf (file, "%d++", GET_MODE_SIZE (output_memory_reference_mode));
      break;

    case PRE_INC:
      fprintf (file, "%d", GET_MODE_SIZE (output_memory_reference_mode));
      ubicom32_print_operand_address (file, XEXP (addr, 0));
      fprintf (file, "++");
      break;

    case POST_DEC:
      ubicom32_print_operand_address (file, XEXP (addr, 0));
      fprintf (file, "%d++", -GET_MODE_SIZE (output_memory_reference_mode));
      break;

    case PRE_DEC:
      fprintf (file, "%d", -GET_MODE_SIZE (output_memory_reference_mode));
      ubicom32_print_operand_address (file, XEXP (addr, 0));
      fprintf (file, "++");
      break;

    case POST_MODIFY:
      ubicom32_print_operand_address (file, XEXP (addr, 0));
      fprintf (file, "%ld++", INTVAL (XEXP (XEXP (addr,1), 1)));
      break;

    case PRE_MODIFY:
      fprintf (file, "%ld", INTVAL (XEXP (XEXP (addr,1), 1)));
      ubicom32_print_operand_address (file, XEXP (addr, 0));
      fprintf (file, "++");
      break;

    case REG:
      fputc ('(', file);
      fprintf (file, "%s", reg_names[REGNO (addr)]);
      fputc (')', file);
      break;

    case PLUS:
      {
	rtx base = XEXP (addr, 0);
	rtx index = XEXP (addr, 1);

 	/* Switch around addresses of the form index * scaling + base.  */
 	if (! ubicom32_is_base_reg (base, 1))
 	  {
 	    rtx tmp = base;
 	    base = index;
 	    index = tmp;
 	  }

	if (CONST_INT_P (index))
	  {
	    fprintf (file, "%ld", INTVAL (index));
	    fputc ('(', file);
	    fputs (reg_names[REGNO (base)], file);
	  }
 	else if (GET_CODE (index) == MULT
 		 || REG_P (index))
	  {
 	    if (GET_CODE (index) == MULT)
 	      index = XEXP (index, 0);
	    fputc ('(', file);
	    fputs (reg_names[REGNO (base)], file);
	    fputc (',', file);
	    fputs (reg_names[REGNO (index)], file);
	  }
	else
	  abort ();

	fputc (')', file);
	break;
      }

    case LO_SUM:
      fprintf (file, "%%lo(");
      ubicom32_print_operand (file, XEXP (addr, 1), 'L');
      fprintf (file, ")(");
      ubicom32_print_operand (file, XEXP (addr, 0), 0);
      fprintf (file, ")");
      break;

    case CONST_INT:
      fputc ('#', file);
      output_addr_const (file, addr);
      break;

    default:
      output_addr_const (file, addr);
      break;
    }
}

/* Given a comparison code (EQ, NE, etc.) and the first operand of a COMPARE,
   return the mode to be used for the comparison.  */

enum machine_mode
ubicom32_select_cc_mode (enum rtx_code op, rtx x, rtx y)
{
  /* Is this a short compare?  */
  if (GET_MODE (x) == QImode
      || GET_MODE (x) == HImode
      || GET_MODE (y) == QImode
      || GET_MODE (y) == HImode)
    {
      switch (op)
	{
	case EQ :
	case NE :
	  return CCSZmode;

	case GE:
	case LT:
	  if (y == const0_rtx)
	    return CCSZNmode;

	default :
	  return CCSmode;
	}
    }

  /* We have a word compare.  */
  switch (op)
    {
    case EQ :
    case NE :
      return CCWZmode;

    case GE :
    case LT :
      if (y == const0_rtx)
	return CCWZNmode;

    default :
      return CCWmode;
    }
}

/* Return TRUE or FALSE depending on whether the first SET in INSN
   has source and destination with matching CC modes, and that the
   CC mode is at least as constrained as REQ_MODE.  */
bool
ubicom32_match_cc_mode (rtx insn, enum machine_mode req_mode)
{
  rtx set;
  enum machine_mode set_mode;

  set = PATTERN (insn);
  if (GET_CODE (set) == PARALLEL)
    set = XVECEXP (set, 0, 0);
  gcc_assert (GET_CODE (set) == SET);
  gcc_assert (GET_CODE (SET_SRC (set)) == COMPARE);

  /* SET_MODE is the mode we have in the instruction.  This must either
     be the same or less restrictive that the required mode REQ_MODE.  */
  set_mode = GET_MODE (SET_DEST (set));

  switch (req_mode)
    {
    case CCSZmode:
      if (set_mode != CCSZmode)
	return 0;
      break;

    case CCSZNmode:
      if (set_mode != CCSZmode
	  && set_mode != CCSZNmode)
	return 0;
      break;

    case CCSmode:
      if (set_mode != CCSmode
	  && set_mode != CCSZmode
	  && set_mode != CCSZNmode)
	return 0;
      break;

    case CCWZmode:
      if (set_mode != CCWZmode)
	return 0;
      break;

    case CCWZNmode:
      if (set_mode != CCWZmode
	  && set_mode != CCWZNmode)
	return 0;
      break;

    case CCWmode:
      if (set_mode != CCWmode
	  && set_mode != CCWZmode
	  && set_mode != CCWZNmode)
	return 0;
      break;

    default:
      gcc_unreachable ();
    }

  return (GET_MODE (SET_SRC (set)) == set_mode);
}

/* Replace the comparison OP0 CODE OP1 by a semantically equivalent one
   that we can implement more efficiently.  */

void
ubicom32_canonicalize_comparison (enum rtx_code *code, rtx *op0, rtx *op1)
{
  /* If we have a REG and a MEM then compare the MEM with the REG and not
     the other way round.  */
  if (REG_P (*op0) && MEM_P (*op1))
    {
      rtx tem = *op0;
      *op0 = *op1;
      *op1 = tem;
      *code = swap_condition (*code);
      return;
    }

  /* If we have a REG and a CONST_INT then we may want to reverse things
     if the constant can be represented as an "I" constraint.  */
  if (REG_P (*op0) && CONST_INT_P (*op1) && satisfies_constraint_I (*op1))
    {
      rtx tem = *op0;
      *op0 = *op1;
      *op1 = tem;
      *code = swap_condition (*code);
      return;
    }
}

/* Return the fixed registers used for condition codes.  */

static bool
ubicom32_fixed_condition_code_regs (unsigned int *p1, unsigned int *p2)
{
  *p1 = CC_REGNUM;
  *p2 = INVALID_REGNUM;

  return true;
}

/* If two condition code modes are compatible, return a condition code
   mode which is compatible with both.  Otherwise, return
   VOIDmode.  */

static enum machine_mode
ubicom32_cc_modes_compatible (enum machine_mode m1, enum machine_mode m2)
{
  if (m1 == m2)
    return m1;

  if (GET_MODE_CLASS (m1) != MODE_CC || GET_MODE_CLASS (m2) != MODE_CC)
    return VOIDmode;

  switch (m1)
    {
    case CCWmode:
      if (m2 == CCWZNmode || m2 == CCWZmode)
	return m1;

      return VOIDmode;

    case CCWZNmode:
      if (m2 == CCWmode)
	return m2;

      if (m2 == CCWZmode)
	return m1;

      return VOIDmode;

    case CCWZmode:
      if (m2 == CCWmode || m2 == CCWZNmode)
	return m2;

      return VOIDmode;

    case CCSmode:
      if (m2 == CCSZNmode || m2 == CCSZmode)
	return m1;

      return VOIDmode;

    case CCSZNmode:
      if (m2 == CCSmode)
	return m2;

      if (m2 == CCSZmode)
	return m1;

      return VOIDmode;

    case CCSZmode:
      if (m2 == CCSmode || m2 == CCSZNmode)
	return m2;

      return VOIDmode;

    default:
      gcc_unreachable ();
    }
}

static rtx
ubicom32_legitimize_fdpic_address_symbol (rtx orig, rtx reg)
{
  rtx fdpic_reg;

  fdpic_reg = get_hard_reg_initial_val (SImode, FDPIC_REGNUM);
  emit_insn (gen_movsi_fdpic_desc(reg, orig, fdpic_reg));

  return reg;
}

static rtx
ubicom32_legitimize_fdpic_address (rtx orig, rtx reg)
{
  rtx addr = orig;
  rtx new_rtx = orig;

  if (GET_CODE (addr) == CONST || GET_CODE (addr) == PLUS)
    {
      rtx base;

      if (GET_CODE (addr) == CONST)
	{
	  addr = XEXP (addr, 0);
	  gcc_assert (GET_CODE (addr) == PLUS);
	}

      base = ubicom32_legitimize_fdpic_address_symbol (XEXP (addr, 0), reg);
      return gen_rtx_PLUS (Pmode, base, XEXP (addr, 1));
    }

  return new_rtx;
}

/* X and Y are two things to compare using CODE.  Emit the compare insn and
   return the rtx for the cc reg in the proper mode.  */

rtx
ubicom32_gen_compare (enum rtx_code code, rtx x, rtx y)
{
  enum machine_mode compare_mode = GET_MODE (x);
  enum machine_mode cc_mode = SELECT_CC_MODE (code, x, y);
  rtx cc_reg;

  switch (compare_mode)
    {
    case QImode:
      /* We can handle some limited compares with zero.  */
      if (y == const0_rtx
	  && (cc_mode == CCSZmode
	      || cc_mode == CCSZNmode))
	break;

      /* Must be a sub.1 - if necessary copy an operand into a reg.  */
      if (! ubicom32_data_register_operand (y, QImode))
	y = copy_to_mode_reg (QImode, y);
      break;

    case HImode:
      /* Is this a cmpi? */
      if (CONST_INT_P (y))
	break;

      /* Must be a sub.2 - if necessary copy an operand into a reg.  */
      if (! ubicom32_data_register_operand (y, HImode))
	y = copy_to_mode_reg (HImode, y);
      break;

    case SImode:
      /* Is this a cmpi?  We can't take a memory address as cmpi takes
	 16-bit operands.  */
      if (register_operand (x, SImode)
	  && CONST_INT_P (y)
	  && satisfies_constraint_N1 (y))
	break;

      /* We can handle some limited compares with zero.  */
      if (y == const0_rtx
	  && (cc_mode == CCWZmode
	      || cc_mode == CCWZNmode))
	break;

      /* Must be a sub.4 - if necessary copy an operand into a reg.  */
      if (! ubicom32_data_register_operand (y, SImode))
	y = copy_to_mode_reg (SImode, y);
      break;

    case DImode:
      /* We can handle some limited compares with zero.  */
      if (y == const0_rtx
	  && cc_mode == CCWZmode)
	break;

      /* Must be a sub.4/subc - if necessary copy an operand into a reg.  */
      if (! ubicom32_data_register_operand (y, DImode))
	y = copy_to_mode_reg (DImode, y);
      break;

    case SFmode:
    case DFmode:
      break;

    default:
      gcc_assert(false);
    }

  cc_reg = gen_rtx_REG (cc_mode, CC_REGNUM);
  emit_insn (gen_rtx_SET (VOIDmode, cc_reg,
			  gen_rtx_COMPARE (cc_mode, x, y)));

  return cc_reg;
}

/* Split OP[0] into OP[2,3] and likewise for OP[1] into OP[4,5].  */

void
ubicom32_split_operand_pair (rtx operands[6])
{
  switch (GET_CODE (operands[1]))
    {
    case REG:
      operands[5] = gen_rtx_REG (SImode, REGNO (operands[1]) + 1);
      operands[4] = gen_rtx_REG (SImode, REGNO (operands[1]));
      break;

    case MEM:
      operands[5] = adjust_address (operands[1], SImode, GET_MODE_SIZE (SImode));
      operands[4] = adjust_address (operands[1], SImode, 0);
      break;

    case CONST_INT:
    case CONST_DOUBLE:
      split_double (operands[1], &operands[4], &operands[5]);
      break;

    default:
      gcc_unreachable ();
    }

  switch (GET_CODE (operands[0]))
    {
    case REG:
      operands[3] = gen_rtx_REG (SImode, REGNO (operands[0]) + 1);
      operands[2] = gen_rtx_REG (SImode, REGNO (operands[0]));
      break;

    case MEM:
      operands[3] = adjust_address (operands[0], SImode, GET_MODE_SIZE (SImode));
      operands[2] = adjust_address (operands[0], SImode, 0);
      break;

    default:
      gcc_unreachable ();
    }
}

static void
ubicom32_gen_lshrsi3 (rtx op0, rtx op1, rtx op2)
{
  if (ubicom32_data_register_operand (op0, SImode))
    emit_insn (gen_lshrsi3 (op0, op1, op2));
  else
    {
      rtx lshr;
      rtx scratch;

      scratch = gen_reg_rtx (SImode);
      lshr = gen_lshrsi3 (scratch, op1, op2);
      emit_insn (lshr);
      emit_move_insn (op0, scratch);
    }
}

static void
ubicom32_gen_addsi3_ccwzn_null (rtx src1, rtx src2)
{
  rtx neg;
  rtx compare;
  rtx cc_reg;
  rtx cc_set;

  neg = gen_rtx_NEG (GET_MODE (src1), src1);
  compare = gen_rtx_COMPARE (CCWZNmode, neg, src2);
  cc_reg = gen_rtx_REG (CCWZNmode, CC_REGNUM);
  cc_set = gen_rtx_SET (VOIDmode, cc_reg, compare);

  emit_insn (cc_set);
}

static void
ubicom32_gen_cmpsi_sub4 (rtx src1, rtx src2)
{
  rtx compare;
  rtx cc_reg;
  rtx cc_set;

  compare = gen_rtx_COMPARE (CCWZNmode, src1, src2);
  cc_reg = gen_rtx_REG (CCWZNmode, CC_REGNUM);
  cc_set = gen_rtx_SET (CCWZNmode, cc_reg, compare);

  emit_insn (cc_set);
}

/* Code generation.  */

void
ubicom32_expand_movsi (rtx *operands)
{
  if (GET_CODE (operands[1]) == SYMBOL_REF
      || (GET_CODE (operands[1]) == CONST
	  && GET_CODE (XEXP (operands[1], 0)) == PLUS
	  && GET_CODE (XEXP (XEXP (operands[1], 0), 0)) == SYMBOL_REF)
      || CONSTANT_ADDRESS_P (operands[1]))
    {
      rtx tmp;
      enum machine_mode mode;
      rtx high;

      if (TARGET_FDPIC)
	{
	  gcc_assert (can_create_pseudo_p ());
	  tmp = gen_reg_rtx (Pmode);
	  if (GET_CODE (operands[1]) == SYMBOL_REF
	      || GET_CODE (operands[1]) == LABEL_REF)
	    operands[1] = ubicom32_legitimize_fdpic_address_symbol (operands[1], tmp);
	  else
	    operands[1] = ubicom32_legitimize_fdpic_address (operands[1], tmp);

	  emit_insn (gen_movsi_internal (operands[0], operands[1]));
	  return;
	}

      /* We want to avoid reusing operand 0 if we can because it limits
	 our ability to optimize later.  */
      tmp = ! can_create_pseudo_p () ? operands[0] : gen_reg_rtx (Pmode);

      mode = GET_MODE (operands[0]);
      high = gen_rtx_HIGH (mode, operands[1]);
      emit_move_insn (tmp, high);

      operands[1] = gen_rtx_LO_SUM (mode, tmp, operands[1]);
      if (can_create_pseudo_p() && ! REG_P (operands[0]))
	{
	  tmp = gen_reg_rtx (mode);
	  emit_insn (gen_rtx_SET (VOIDmode, tmp, operands[1]));
	  operands[1] = tmp;
	}

      emit_insn (gen_movsi_internal (operands[0], operands[1]));
      return;
    }

  emit_insn (gen_movsi_internal (operands[0], operands[1]));
}

/* Emit code for addsi3.  */

void
ubicom32_expand_addsi3 (rtx *operands)
{
  rtx op, clob;

  if (can_create_pseudo_p ())
    {
      /* If we have a non-data reg for operand 1 then prefer that over
	 a CONST_INT in operand 2.  */
      if (! ubicom32_data_register_operand (operands[1], GET_MODE (operands[1]))
	  && CONST_INT_P (operands[2]))
	operands[2] = copy_to_mode_reg (SImode, operands[2]);

      if (CONST_INT_P (operands[2]) && ! satisfies_constraint_I (operands[2]))
	operands[2] = copy_to_mode_reg (SImode, operands[2]);
    }

  /* Emit the instruction.  */

  op = gen_rtx_SET (VOIDmode, operands[0],
		    gen_rtx_PLUS (SImode, operands[1], operands[2]));

  if (! can_create_pseudo_p ())
    {
      /* Reload doesn't know about the flags register, and doesn't know that
	 it doesn't want to clobber it.  We can only do this with PLUS.  */
      emit_insn (op);
    }
  else
    {
      clob = gen_rtx_CLOBBER (VOIDmode, gen_rtx_REG (CCmode, CC_REGNUM));
      emit_insn (gen_rtx_PARALLEL (VOIDmode, gen_rtvec (2, op, clob)));
    }
}

/* Expander for mulsi3.  Return 1 if we have generated all the code
   necessary to do the multiplication.  */

bool
ubicom32_expand_mulsi3 (rtx *operands)
{
  if (! ubicom32_v4)
    {
      rtx a1, a1_1, a2;
      rtx b1, b1_1, b2;
      rtx mac_lo_rtx;
      rtx t1, t2, t3;

      /* Give up if we cannot create new pseudos.  */
      if (!can_create_pseudo_p())
	return false;

      /* Synthesize 32-bit multiplication using 16-bit operations:

	 a1 = highpart (a)
	 a2 = lowpart (a)

	 b1 = highpart (b)
	 b2 = lowpart (b)

	 c = (a1 * b1) << 32 + (a1 * b2) << 16 + (a2 * b1) << 16 + a2 * b2
	   =        0        + (a1 * b2) << 16 + (a2 * b1) << 16 + a2 * b2
	                       ^^^^^^^^^^^^^^^   ^^^^^^^^^^^^^^^   ^^^^^^^
			           Signed             Signed      Unsigned  */

      if (!ubicom32_data_register_operand (operands[1], GET_MODE (operands[1])))
	{
	  rtx op1;

	  op1 = gen_reg_rtx (SImode);
	  emit_move_insn (op1, operands[1]);
	  operands[1] = op1;
	}

      if (!ubicom32_data_register_operand (operands[2], GET_MODE (operands[2])))
	{
	  rtx op2;

	  op2 = gen_reg_rtx (SImode);
	  emit_move_insn (op2, operands[2]);
	  operands[2] = op2;
	}

      /* a1 = highpart (a)  */
      a1 = gen_reg_rtx (HImode);
      a1_1 = gen_reg_rtx (SImode);
      emit_insn (gen_ashrsi3 (a1_1, operands[1], GEN_INT (16)));
      emit_move_insn (a1, gen_lowpart (HImode, a1_1));

      /* a2 = lowpart (a)  */
      a2 = gen_reg_rtx (HImode);
      emit_move_insn (a2, gen_lowpart (HImode, operands[1]));

      /* b1 = highpart (b)  */
      b1 = gen_reg_rtx (HImode);
      b1_1 = gen_reg_rtx (SImode);
      emit_insn (gen_ashrsi3 (b1_1, operands[2], GEN_INT (16)));
      emit_move_insn (b1, gen_lowpart (HImode, b1_1));

      /* b2 = lowpart (b)  */
      b2 = gen_reg_rtx (HImode);
      emit_move_insn (b2, gen_lowpart (HImode, operands[2]));

      /* t1 = (a1 * b2) << 16  */
      t1 = gen_reg_rtx (SImode);
      mac_lo_rtx = gen_rtx_REG (SImode, ACC0_LO_REGNUM);
      emit_insn (gen_mulhisi3 (mac_lo_rtx, a1, b2));
      emit_insn (gen_ashlsi3 (t1, mac_lo_rtx, GEN_INT (16)));

      /* t2 = (a2 * b1) << 16  */
      t2 = gen_reg_rtx (SImode);
      emit_insn (gen_mulhisi3 (mac_lo_rtx, a2, b1));
      emit_insn (gen_ashlsi3 (t2, mac_lo_rtx, GEN_INT (16)));

      /* mac_lo = a2 * b2  */
      emit_insn (gen_umulhisi3 (mac_lo_rtx, a2, b2));

      /* t3 = t1 + t2  */
      t3 = gen_reg_rtx (SImode);
      emit_insn (gen_addsi3 (t3, t1, t2));

      /* c = t3 + mac_lo_rtx  */
      emit_insn (gen_addsi3 (operands[0], mac_lo_rtx, t3));

      return true;
    }
  else
    {
      rtx acc_rtx;

      /* Give up if we cannot create new pseudos.  */
      if (!can_create_pseudo_p())
	return false;

      if (!ubicom32_data_register_operand (operands[1], GET_MODE (operands[1])))
	{
	  rtx op1;

	  op1 = gen_reg_rtx (SImode);
	  emit_move_insn (op1, operands[1]);
	  operands[1] = op1;
	}

      if (!ubicom32_data_register_operand (operands[2], GET_MODE (operands[2])))
	{
	  rtx op2;

	  op2 = gen_reg_rtx (SImode);
	  emit_move_insn (op2, operands[2]);
	  operands[2] = op2;
	}

      acc_rtx = gen_reg_rtx (DImode);
      emit_insn (gen_umulsidi3 (acc_rtx, operands[1], operands[2]));
      emit_move_insn (operands[0], gen_lowpart (SImode, acc_rtx));

      return true;
    }
}

static bool
ubicom32_expand_seq (rtx cmp_op0, rtx cmp_op1, rtx *operands)
{
  rtx scratch1;
  rtx seq;

  /* Check to see if the args work better swapped around?  */
  if (ubicom32_arith_operand (cmp_op1, SImode)
      && ! ubicom32_data_register_operand (cmp_op1, SImode)
      && ubicom32_data_register_operand (cmp_op0, SImode))
    {
      rtx tmp = cmp_op0;
      cmp_op0 = cmp_op1;
      cmp_op1 = tmp;
    }

  if (! ubicom32_arith_operand (cmp_op0, SImode))
    {
      if (GET_MODE (cmp_op0) != SImode && GET_MODE (cmp_op0) != VOIDmode)
	return false;

      cmp_op0 = copy_to_mode_reg (SImode, cmp_op0);
    }

  if (! ubicom32_data_register_operand (cmp_op1, SImode))
    {
      if (GET_MODE (cmp_op1) != SImode && GET_MODE (cmp_op1) != VOIDmode)
	return false;

      cmp_op1 = copy_to_mode_reg (SImode, cmp_op1);
    }

  if (cmp_op0 == const0_rtx)
    scratch1 = cmp_op1;
  else
    {
      rtx sub;

      scratch1 = gen_reg_rtx (SImode);
      sub = gen_subsi3 (scratch1, cmp_op0, cmp_op1);
      emit_insn (sub);
    }

  seq = gen_seq_1 (operands[0], scratch1);
  emit_insn (seq);

  return true;
}

static bool
ubicom32_expand_sne (rtx cmp_op0, rtx cmp_op1, rtx *operands)
{
  rtx scratch1;
  rtx sne;

  /* Check to see if the args work better swapped around?  */
  if (ubicom32_arith_operand (cmp_op1, SImode)
      && ! ubicom32_data_register_operand (cmp_op1, SImode)
      && ubicom32_data_register_operand (cmp_op0, SImode))
    {
      rtx tmp = cmp_op0;
      cmp_op0 = cmp_op1;
      cmp_op1 = tmp;
    }

  if (! ubicom32_arith_operand (cmp_op0, SImode))
    {
      if (GET_MODE (cmp_op0) != SImode && GET_MODE (cmp_op0) != VOIDmode)
	return false;

      cmp_op0 = copy_to_mode_reg (SImode, cmp_op0);
    }

  if (! ubicom32_data_register_operand (cmp_op1, SImode))
    {
      if (GET_MODE (cmp_op1) != SImode && GET_MODE (cmp_op1) != VOIDmode)
	return false;

      cmp_op1 = copy_to_mode_reg (SImode, cmp_op1);
    }

  if (cmp_op0 == const0_rtx)
    scratch1 = cmp_op1;
  else
    {
       rtx sub;

       scratch1 = gen_reg_rtx (SImode);
       sub = gen_subsi3 (scratch1, cmp_op0, cmp_op1);
       emit_insn (sub);
    }

  sne = gen_sne_1 (operands[0], scratch1);
  emit_insn (sne);

  return true;
}

static bool
ubicom32_expand_sgt (rtx cmp_op0, rtx cmp_op1, rtx *operands)
{
  rtx slt;

  if (cmp_op1 == const0_rtx)
    {
      rtx scratch1;
      rtx scratch2;
      rtx ashr;
      rtx sub;

      if (! ubicom32_data_register_operand (cmp_op0, SImode))
	{
	  if (GET_MODE (cmp_op0) != SImode && GET_MODE (cmp_op0) != VOIDmode)
	    return false;

	  cmp_op0 = copy_to_mode_reg (SImode, cmp_op0);
	}

      scratch1 = gen_reg_rtx (SImode);
      ashr = gen_ashrsi3 (scratch1, cmp_op0, GEN_INT (31));
      emit_insn (ashr);

      scratch2 = gen_reg_rtx (SImode);
      sub = gen_subsi3 (scratch2, scratch1, cmp_op0);
      emit_insn (sub);

      ubicom32_gen_lshrsi3 (operands[0], scratch2, GEN_INT (31));
      return true;
    }

  if (! ubicom32_arith_operand (cmp_op1, SImode))
    {
      if (GET_MODE (cmp_op1) != SImode && GET_MODE (cmp_op1) != VOIDmode)
	return false;

      cmp_op1 = copy_to_mode_reg (SImode, cmp_op1);
    }

  if (! ubicom32_compare_operand (cmp_op0, SImode))
    {
      if (GET_MODE (cmp_op0) != SImode && GET_MODE (cmp_op0) != VOIDmode)
	return false;

      cmp_op0 = copy_to_mode_reg (SImode, cmp_op0);
    }

  slt = gen_slt_1 (operands[0], cmp_op1, cmp_op0);
  emit_insn (slt);

  return true;
}

static bool
ubicom32_expand_slt (rtx cmp_op0, rtx cmp_op1, rtx *operands)
{
  rtx slt;

  if (cmp_op1 == const0_rtx)
    {
      ubicom32_gen_lshrsi3 (operands[0], cmp_op0, GEN_INT (31));
      return true;
    }

  if (! ubicom32_arith_operand (cmp_op0, SImode))
    {
      if (GET_MODE (cmp_op0) != SImode && GET_MODE (cmp_op0) != VOIDmode)
	return false;

      cmp_op0 = copy_to_mode_reg (SImode, cmp_op0);
    }

  if (! ubicom32_compare_operand (cmp_op1, SImode))
    {
      if (GET_MODE (cmp_op1) != SImode && GET_MODE (cmp_op1) != VOIDmode)
	return false;

      cmp_op1 = copy_to_mode_reg (SImode, cmp_op1);
    }

  slt = gen_slt_1 (operands[0], cmp_op0, cmp_op1);
  emit_insn (slt);

  return true;
}

static bool
ubicom32_expand_sge (rtx cmp_op0, rtx cmp_op1, rtx *operands)
{
  rtx sge;

  if (cmp_op1 == const0_rtx)
    {
      rtx scratch1;
      rtx one_cmpl;

      scratch1 = gen_reg_rtx (SImode);
      one_cmpl = gen_one_cmplsi2 (scratch1, cmp_op0);
      emit_insn (one_cmpl);

      ubicom32_gen_lshrsi3 (operands[0], scratch1, GEN_INT (31));
      return true;
    }

  if (! ubicom32_arith_operand (cmp_op0, SImode))
    {
      if (GET_MODE (cmp_op0) != SImode && GET_MODE (cmp_op0) != VOIDmode)
	return false;

      cmp_op0 = copy_to_mode_reg (SImode, cmp_op0);
    }

  if (! ubicom32_compare_operand (cmp_op1, SImode))
    {
      if (GET_MODE (cmp_op1) != SImode && GET_MODE (cmp_op1) != VOIDmode)
	return false;

      cmp_op1 = copy_to_mode_reg (SImode, cmp_op1);
    }

  sge = gen_sge_1 (operands[0], cmp_op0, cmp_op1);
  emit_insn (sge);

  return true;
}

static bool
ubicom32_expand_sle (rtx cmp_op0, rtx cmp_op1, rtx *operands)
{
  rtx sge;

  if (! ubicom32_arith_operand (cmp_op1, SImode))
    {
      if (GET_MODE (cmp_op1) != SImode && GET_MODE (cmp_op1) != VOIDmode)
	return false;

      cmp_op1 = copy_to_mode_reg (SImode, cmp_op1);
    }

  if (! ubicom32_compare_operand (cmp_op0, SImode))
    {
      if (GET_MODE (cmp_op0) != SImode && GET_MODE (cmp_op0) != VOIDmode)
	return false;

      cmp_op0 = copy_to_mode_reg (SImode, cmp_op0);
    }

  sge = gen_sge_1 (operands[0], cmp_op1, cmp_op0);
  emit_insn (sge);

  return true;
}

static bool
ubicom32_expand_sgtu (rtx cmp_op0, rtx cmp_op1, rtx *operands)
{
  rtx sltu;

  /* If we are comparing with zero then we can simplify this test to
     "sne".  */
  if (cmp_op1 == const0_rtx)
    return ubicom32_expand_sne(cmp_op0, cmp_op1, operands);

  if (! ubicom32_arith_operand (cmp_op1, SImode))
    {
      if (GET_MODE (cmp_op1) != SImode && GET_MODE (cmp_op1) != VOIDmode)
	return false;

      cmp_op1 = copy_to_mode_reg (SImode, cmp_op1);
    }

  if (! ubicom32_data_register_operand (cmp_op0, SImode))
    {
      if (GET_MODE (cmp_op0) != SImode && GET_MODE (cmp_op0) != VOIDmode)
	return false;

      cmp_op0 = copy_to_mode_reg (SImode, cmp_op0);
    }

  sltu = gen_sltu_1 (operands[0], cmp_op1, cmp_op0);
  emit_insn (sltu);

  return true;
}

static bool
ubicom32_expand_sltu (rtx cmp_op0, rtx cmp_op1, rtx *operands)
{
  rtx sltu;

  /* If we are comparing with zero then we can simplify this test to
     0 (the test can't succeed).  */
  if (cmp_op0 == const0_rtx)
    {
      emit_move_insn (operands[0], const0_rtx);
      return true;
    }

  if (! ubicom32_arith_operand (cmp_op0, SImode))
    {
      if (GET_MODE (cmp_op0) != SImode && GET_MODE (cmp_op0) != VOIDmode)
	return false;

      cmp_op0 = copy_to_mode_reg (SImode, cmp_op0);
    }

  if (! ubicom32_data_register_operand (cmp_op1, SImode))
    {
      if (GET_MODE (cmp_op1) != SImode && GET_MODE (cmp_op1) != VOIDmode)
	return false;

      cmp_op1 = copy_to_mode_reg (SImode, cmp_op1);
    }

  sltu = gen_sltu_1 (operands[0], cmp_op0, cmp_op1);
  emit_insn (sltu);

  return true;
}

static bool
ubicom32_expand_sgeu (rtx cmp_op0, rtx cmp_op1, rtx *operands)
{
  rtx sgeu;
  rtx scratch;

  /* If we are comparing with zero then we can simplify this test to
     1 (the test always succeeds).  */
  if (cmp_op0 == const0_rtx)
    {
      emit_move_insn (operands[0], const1_rtx);
      return true;
    }

  if (! ubicom32_arith_operand (cmp_op0, SImode))
    {
      if (GET_MODE (cmp_op0) != SImode && GET_MODE (cmp_op0) != VOIDmode)
	return false;

      cmp_op0 = copy_to_mode_reg (SImode, cmp_op0);
    }

  if (! ubicom32_data_register_operand (cmp_op1, SImode))
    {
      if (GET_MODE (cmp_op1) != SImode && GET_MODE (cmp_op1) != VOIDmode)
	return false;

      cmp_op1 = copy_to_mode_reg (SImode, cmp_op1);
    }

  scratch = gen_reg_rtx (SImode);
  sgeu = gen_sgeu_1 (operands[0], cmp_op0, cmp_op1);
  emit_insn (sgeu);

  return true;
}

static bool
ubicom32_expand_sleu (rtx cmp_op0, rtx cmp_op1, rtx *operands)
{
  rtx sgeu;
  rtx scratch;

  /* If we are comparing with zero then we can simplify this test to
     "seq".  */
  if (cmp_op1 == const0_rtx)
    return ubicom32_expand_seq(cmp_op0, cmp_op1, operands);

  if (! ubicom32_arith_operand (cmp_op1, SImode))
    {
      if (GET_MODE (cmp_op1) != SImode && GET_MODE (cmp_op1) != VOIDmode)
	return false;

      cmp_op1 = copy_to_mode_reg (SImode, cmp_op1);
    }

  if (! ubicom32_data_register_operand (cmp_op0, SImode))
    {
      if (GET_MODE (cmp_op0) != SImode && GET_MODE (cmp_op0) != VOIDmode)
	return false;

      cmp_op0 = copy_to_mode_reg (SImode, cmp_op0);
    }

  scratch = gen_reg_rtx (SImode);
  sgeu = gen_sgeu_1 (operands[0], cmp_op1, cmp_op0);
  emit_insn (sgeu);
  return true;
}

bool
ubicom32_expand_scode (enum rtx_code code, rtx cmp_op0, rtx cmp_op1, rtx *operands)
{
  if (GET_MODE_CLASS (GET_MODE (cmp_op0)) == MODE_FLOAT 
      || GET_MODE_CLASS (GET_MODE (cmp_op1)) == MODE_FLOAT)
    return false;

  switch (code)
    {
    case EQ:
      return ubicom32_expand_seq (cmp_op0, cmp_op1, operands);

    case NE:
      return ubicom32_expand_sne (cmp_op0, cmp_op1, operands);

    case GT:
      return ubicom32_expand_sgt (cmp_op0, cmp_op1, operands);

    case LT:
      return ubicom32_expand_slt (cmp_op0, cmp_op1, operands);

    case GE:
      return ubicom32_expand_sge (cmp_op0, cmp_op1, operands);

    case LE:
      return ubicom32_expand_sle (cmp_op0, cmp_op1, operands);

    case GTU:
      return ubicom32_expand_sgtu (cmp_op0, cmp_op1, operands);

    case LTU:
      return ubicom32_expand_sltu (cmp_op0, cmp_op1, operands);

    case GEU:
      return ubicom32_expand_sgeu (cmp_op0, cmp_op1, operands);

    case LEU:
      return ubicom32_expand_sleu (cmp_op0, cmp_op1, operands);

    default:
      return false;
    }
}

static bool
ubicom32_expand_addeq (rtx cmp_op0, rtx cmp_op1,
		       rtx dest, rtx src, rtx increment)
{
  rtx sub_1;
  rtx scratch;

  if (increment != const1_rtx && increment != constm1_rtx)
    return false;

  /* Check to see if the args work better swapped around?  */
  if (ubicom32_arith_operand (cmp_op1, SImode)
      && ! ubicom32_data_register_operand (cmp_op1, SImode)
      && ubicom32_data_register_operand (cmp_op0, SImode))
    {
      rtx tmp = cmp_op0;
      cmp_op0 = cmp_op1;
      cmp_op1 = tmp;
    }

  if (! ubicom32_arith_operand (cmp_op0, SImode))
    {
      if (GET_MODE (cmp_op0) != SImode && GET_MODE (cmp_op0) != VOIDmode)
	return false;

      cmp_op0 = copy_to_mode_reg (SImode, cmp_op0);
    }

  if (! ubicom32_data_register_operand (cmp_op1, SImode))
    {
      if (GET_MODE (cmp_op1) != SImode && GET_MODE (cmp_op1) != VOIDmode)
	return false;

      cmp_op1 = copy_to_mode_reg (SImode, cmp_op1);
    }

  if (cmp_op0 == const0_rtx)
    scratch = cmp_op1;
  else
    {
      scratch = gen_reg_rtx (SImode);
      sub_1 = gen_subsi3 (scratch, cmp_op0, cmp_op1);
      emit_insn (sub_1);
    }

  if (! ubicom32_data_register_operand (src, SImode))
    {
      if (GET_MODE (src) != SImode && GET_MODE (src) != VOIDmode)
	return false;

      src = copy_to_mode_reg (SImode, src);
    }

  if (increment == const1_rtx)
    {
      rtx addc;

      ubicom32_gen_cmpsi_sub4 (const0_rtx, scratch);

      addc = gen_addsi3_addc (dest, src, const0_rtx);
      emit_insn (addc);
    }
  else
    {
      rtx addc;

      ubicom32_gen_addsi3_ccwzn_null (scratch, constm1_rtx);

      addc = gen_addsi3_addc (dest, src, constm1_rtx);
      emit_insn (addc);
    }

  return true;
}

static bool
ubicom32_expand_addne (rtx cmp_op0, rtx cmp_op1,
		       rtx dest, rtx src, rtx increment)
{
  rtx sub_1;
  rtx scratch;

  if (increment != const1_rtx && increment != constm1_rtx)
    return false;

  /* Check to see if the args work better swapped around?  */
  if (ubicom32_arith_operand (cmp_op1, SImode)
      && ! ubicom32_data_register_operand (cmp_op1, SImode)
      && ubicom32_data_register_operand (cmp_op0, SImode))
    {
      rtx tmp = cmp_op0;
      cmp_op0 = cmp_op1;
      cmp_op1 = tmp;
    }

  if (! ubicom32_arith_operand (cmp_op0, SImode))
    {
      if (GET_MODE (cmp_op0) != SImode && GET_MODE (cmp_op0) != VOIDmode)
	return false;

      cmp_op0 = copy_to_mode_reg (SImode, cmp_op0);
    }

  if (! ubicom32_data_register_operand (cmp_op1, SImode))
    {
      if (GET_MODE (cmp_op1) != SImode && GET_MODE (cmp_op1) != VOIDmode)
	return false;

      cmp_op1 = copy_to_mode_reg (SImode, cmp_op1);
    }

  if (cmp_op0 == const0_rtx)
    scratch = cmp_op1;
  else
    {
      scratch = gen_reg_rtx (SImode);
      sub_1 = gen_subsi3 (scratch, cmp_op0, cmp_op1);
      emit_insn (sub_1);
    }

  if (! ubicom32_data_register_operand (src, SImode))
    {
      if (GET_MODE (src) != SImode && GET_MODE (src) != VOIDmode)
	return false;

      src = copy_to_mode_reg (SImode, src);
    }

  if (increment == const1_rtx)
    {
      rtx addc;

      ubicom32_gen_addsi3_ccwzn_null (scratch, constm1_rtx);

      addc = gen_addsi3_addc (dest, src, const0_rtx);
      emit_insn (addc);
    }
  else
    {
      rtx addc;

      ubicom32_gen_cmpsi_sub4 (const0_rtx, scratch);

      addc = gen_addsi3_addc (dest, src, constm1_rtx);
      emit_insn (addc);
    }

  return true;
}

bool
ubicom32_expand_addcc (enum rtx_code code, rtx cmp_op0, rtx cmp_op1,
		       rtx dest, rtx src, rtx increment)
{
  switch (code)
    {
    case EQ:
      return ubicom32_expand_addeq (cmp_op0, cmp_op1, dest, src, increment);

    case NE:
      return ubicom32_expand_addne (cmp_op0, cmp_op1, dest, src, increment);

    default:
      return false;
    }
}

/* Splitter for seq.  */

void
ubicom32_split_seq (rtx *operands)
{
  operands[2] = gen_reg_rtx (SImode);
}

/* Splitter for sne.  */

void
ubicom32_split_sne (rtx *operands)
{
  operands[2] = gen_reg_rtx (SImode);
}

/* Splitter for slt.  */

void
ubicom32_split_slt (rtx *operands)
{
  if (CONST_INT_P (operands[1]))
    operands[3] = (INTVAL (operands[1]) >= 0) ? const0_rtx : constm1_rtx;
  else
    {
      rtx ashrsi3;

      operands[3] = gen_reg_rtx (SImode);
      ashrsi3 = gen_ashrsi3 (operands[3], operands[1], GEN_INT (31));
      emit_insn (ashrsi3);
    }

  operands[7] = operands[2];
  if (MEM_P (operands[2])
      || (REG_P (operands[2])
	  && !ubicom32_data_register_operand (operands[2],
					      GET_MODE (operands[2]))))
    {
      operands[7] = gen_reg_rtx (SImode);
      emit_move_insn (operands[7], operands[2]);
    }

  operands[6] = operands[1];
  if (CONST_INT_P (operands[7]))
    {
      operands[4] = operands[3];
      operands[3] = (INTVAL (operands[7]) >= 0) ? const0_rtx : const1_rtx;

      if (! REG_P (operands[1]))
	{
	  operands[6] = gen_reg_rtx (SImode);
	  emit_move_insn (operands[6], operands[1]);
	}
    }
  else
    {
      rtx lshrsi3;

      operands[4] = gen_reg_rtx (SImode);
      lshrsi3 = gen_lshrsi3 (operands[4], operands[7], GEN_INT (31));
      emit_insn (lshrsi3);
    }

  operands[5] = gen_reg_rtx (SImode);
}

/* Splitter for sge.  */

void
ubicom32_split_sge (rtx *operands)
{
  if (CONST_INT_P (operands[1]))
    operands[3] = (INTVAL (operands[1]) >= 0) ? const0_rtx : constm1_rtx;
  else
    {
      rtx ashrsi3;

      operands[3] = gen_reg_rtx (SImode);
      ashrsi3 = gen_ashrsi3 (operands[3], operands[1], GEN_INT (31));
      emit_insn (ashrsi3);
    }

  operands[7] = operands[2];
  if (MEM_P (operands[2])
      || (REG_P (operands[2])
	  && !ubicom32_data_register_operand (operands[2],
					      GET_MODE (operands[2]))))
    {
      operands[7] = gen_reg_rtx (SImode);
      emit_move_insn (operands[7], operands[2]);
    }

  operands[6] = operands[1];
  if (CONST_INT_P (operands[7]))
    {
      operands[4] = operands[3];
      operands[3] = (INTVAL (operands[7]) >= 0) ? const0_rtx : const1_rtx;

      if (! REG_P (operands[1]))
	{
	  operands[6] = gen_reg_rtx (SImode);
	  emit_move_insn (operands[6], operands[1]);
	}
    }
  else
    {
      rtx lshrsi3;

      operands[4] = gen_reg_rtx (SImode);
      lshrsi3 = gen_lshrsi3 (operands[4], operands[7], GEN_INT (31));
      emit_insn (lshrsi3);
    }
}

/* Splitter for sltu.  */

void
ubicom32_split_sltu (rtx *operands)
{
  operands[3] = gen_reg_rtx (SImode);

  operands[7] = operands[2];
  if (MEM_P (operands[2])
      || (REG_P (operands[2])
	  && !ubicom32_data_register_operand (operands[2],
					      GET_MODE (operands[2]))))
    {
      operands[7] = gen_reg_rtx (SImode);
      emit_move_insn (operands[7], operands[2]);
    }

  operands[6] = operands[1];
  if (CONST_INT_P (operands[7]))
    {
      if (! REG_P (operands[1]))
	{
	  operands[6] = gen_reg_rtx (SImode);
	  emit_move_insn (operands[6], operands[1]);
	}
    }

  if (ubicom32_data_register_operand (operands[6], GET_MODE (operands[6])))
    operands[4] = operands[6];
  else
    operands[4] = operands[7];

}

/* Splitter for sgeu.  */

void
ubicom32_split_sgeu (rtx *operands)
{
  operands[3] = gen_reg_rtx (SImode);

  operands[7] = operands[2];
  if (MEM_P (operands[2])
      || (REG_P (operands[2])
	  && !ubicom32_data_register_operand (operands[2],
					      GET_MODE (operands[2]))))
    {
      operands[7] = gen_reg_rtx (SImode);
      emit_move_insn (operands[7], operands[2]);
    }

  operands[6] = operands[1];
  if (CONST_INT_P (operands[7]))
    {
      if (! REG_P (operands[1]))
	{
	  operands[6] = gen_reg_rtx (SImode);
	  emit_move_insn (operands[6], operands[1]);
	}
    }
}

/* Move the integer value VAL into OPERANDS[0].  */

void
ubicom32_emit_move_const_int (rtx dest, rtx imm)
{
  rtx xoperands[2];

  xoperands[0] = dest;
  xoperands[1] = imm;

  /* Treat mem destinations separately.  Values must be explicitly sign
     extended.  */
  if (MEM_P (dest))
    {
      rtx low_hword_mem;
      rtx low_hword_addr;

      /* Emit shorter sequence for signed 7-bit quantities.  */
      if (satisfies_constraint_I (imm))
	{
	  output_asm_insn ("move.4\t%0, %1", xoperands);
	  return;
	}

      /* Special case for pushing constants.  */
      if (GET_CODE (XEXP (dest, 0)) == PRE_DEC
	  && XEXP (XEXP (dest, 0), 0) == stack_pointer_rtx)
	{
	  output_asm_insn ("movei\t-4(sp)++, #%%hi(%E1)", xoperands);
	  output_asm_insn ("movei\t2(sp), #%%lo(%E1)", xoperands);
	  return;
	}

      /* See if we can add 2 to the original address.  This is only
	 possible if the original address is of the form REG or
	 REG+const.  */
      low_hword_addr = plus_constant (XEXP (dest, 0), 2);
      if (ubicom32_legitimate_address_p (HImode, low_hword_addr, 1))
	{
	  low_hword_mem = gen_rtx_MEM (HImode, low_hword_addr);
	  MEM_COPY_ATTRIBUTES (low_hword_mem, dest);
	  output_asm_insn ("movei\t%0, #%%hi(%E1)", xoperands);
	  xoperands[0] = low_hword_mem;
	  output_asm_insn ("movei\t%0, #%%lo(%E1)", xoperands);
	  return;
	}

      /* The original address is too complex.  We need to use a
	 scratch memory by (sp) and move that to the original
	 destination.  */
      if (! reg_mentioned_p (stack_pointer_rtx, dest))
	{
	  output_asm_insn ("movei\t-4(sp)++, #%%hi(%E1)", xoperands);
	  output_asm_insn ("movei\t2(sp), #%%lo(%E1)", xoperands);
	  output_asm_insn ("move.4\t%0, (sp)4++", xoperands);
	  return;
	}

      /* Our address mentions the stack pointer so we need to
	 use our scratch data register here as well as scratch
	 memory.  */
      output_asm_insn ("movei\t-4(sp)++, #%%hi(%E1)", xoperands);
      output_asm_insn ("movei\t2(sp), #%%lo(%E1)", xoperands);
      output_asm_insn ("move.4\td15, (sp)4++", xoperands);
      output_asm_insn ("move.4\t%0, d15", xoperands);
      return;
    }

  /* Move into registers are zero extended by default.  */
  if (! REG_P (dest))
    abort ();

  if (satisfies_constraint_N1 (imm))
    {
      output_asm_insn ("movei\t%0, %1", xoperands);
      return;
    }

  if (INTVAL (xoperands[1]) >= 0xff80
      && INTVAL (xoperands[1]) < 0x10000)
    {
      xoperands[1] = GEN_INT (INTVAL (xoperands[1]) - 0x10000);
      output_asm_insn ("move.2\t%0, %1", xoperands);
      return;
    }

  if ((REGNO_REG_CLASS (REGNO (xoperands[0])) == ADDRESS_REGS
       || REGNO_REG_CLASS (REGNO (xoperands[0])) == SIBCALL_ADDRESS_REGS
       || REGNO_REG_CLASS (REGNO (xoperands[0])) == LINK_REG
       || REGNO_REG_CLASS (REGNO (xoperands[0])) == FDPIC_REG)
      && ((INTVAL (xoperands[1]) & 0x80000000) == 0))
    {
      output_asm_insn ("moveai\t%0, #%%hi(%E1)", xoperands);
      if ((INTVAL (xoperands[1]) & 0x7f) != 0)
	output_asm_insn ("lea.1\t%0, %%lo(%E1)(%0)", xoperands);
      return;
    }

  if ((INTVAL (xoperands[1]) & 0xffff0000) == 0)
    {
      output_asm_insn ("movei\t%0, #%%lo(%E1)", xoperands);
      output_asm_insn ("move.2\t%0, %0", xoperands);
      return;
    }

  /* This is very expensive.  The constant is so large that we
     need to use the stack to do the load.  */
  output_asm_insn ("movei\t-4(sp)++, #%%hi(%E1)", xoperands);
  output_asm_insn ("movei\t2(sp), #%%lo(%E1)", xoperands);
  output_asm_insn ("move.4\t%0, (sp)4++", xoperands);
}

static void
ubicom32_layout_prologue_frame (int adj)
{
  rtx insn;
  char already_saved[FIRST_PSEUDO_REGISTER];
  int i;
  int nregs = 0;

  memset (already_saved, 0, sizeof (already_saved));
  already_saved[LINK_REGNO] = 1;
  already_saved[FRAME_POINTER_REGNUM] = 1;
  for (i = 0; i < FIRST_PSEUDO_REGISTER; i++)
    if (call_used_regs[i]
	|| fixed_regs[i]
	|| ! df_regs_ever_live_p (i))
      already_saved[i] = 1;

  memset (save_regs_order, 0, sizeof (save_regs_order));

  if (current_function_is_leaf && (! df_regs_ever_live_p (LINK_REGNO)))
    save_link_reg = false;
  else
    {
      save_link_reg = true;
      save_regs_order[nregs] = LINK_REGNO;
      nregs++;
    }

  if (frame_pointer_needed || df_regs_ever_live_p (FRAME_POINTER_REGNUM))
    {
      save_regs_order[nregs] = FRAME_POINTER_REGNUM;
      nregs++;
    }

  push_topmost_sequence ();

  /* Walk the function to see the order in which registers are set.
     Our ideal save order will match this so that instruction
     scheduling has the most chance to be useful.  */
  for (insn = get_insns (); insn; insn = NEXT_INSN (insn))
    {
      rtx pat;

      if (!INSN_P (insn))
	continue;

      pat = PATTERN (insn);
      if (GET_CODE (pat) == SET)
	{
	  rtx dest = SET_DEST (pat);
	  if (REG_P (dest))
	    {
	      int dreg = REGNO (dest);
	      if (dreg < FIRST_PSEUDO_REGISTER
		  && ! already_saved[dreg])
		{
		  save_regs_order[nregs] = dreg;
		  nregs++;
		  already_saved[dreg] = 1;
		}
	    }
	}
      else if (GET_CODE (pat) == PARALLEL)
	{
	  int v;

	  for (v = 0; v < XVECLEN (pat, 0); v++)
	    {
	      rtx sub_pat = XVECEXP (pat, 0, v);
	      if (GET_CODE (sub_pat) == SET)
		{
		  rtx dest = SET_DEST (sub_pat);
		  if (REG_P (dest))
		    {
		      int dreg = REGNO (dest);
		      if (dreg < FIRST_PSEUDO_REGISTER
			  && ! already_saved[dreg])
			{
			  save_regs_order[nregs] = dreg;
			  nregs++;
			  already_saved[dreg] = 1;
			}
		    }
		}
	    }
	}
    }

  pop_topmost_sequence ();

  /* Figure out which register(s) needs to be saved.  */
  for (i = 0; i <= LAST_ADDRESS_REGNUM; i++)
    if (! already_saved[i])
      {
	save_regs_order[nregs] = i;
	nregs++;
      }

  save_regs_size = 4 * nregs;
  save_regs = nregs;

  if (save_regs)
    {
      if (adj + save_regs_size <= 28)
	prologue_epilogue_type = 1;
      else if (adj > 24)
	prologue_epilogue_type = 2;
      else
	prologue_epilogue_type = 3;
    }
 }

static void
ubicom32_expand_prologue_1 (int adj)
{
  rtx addr;
  rtx mem;
  rtx reg;
  rtx x;
  int i;

  addr = gen_rtx_PRE_MODIFY (Pmode,
			     stack_pointer_rtx,
			     plus_constant (stack_pointer_rtx, -(adj + save_regs_size)));
  mem = gen_frame_mem (SImode, addr);
  reg = gen_rtx_REG (SImode, save_regs_order[0]);
  x = emit_move_insn (mem, reg);
  RTX_FRAME_RELATED_P (x) = 1;

  for (i = 1; i < save_regs; i++)
    {
      mem = gen_frame_mem (SImode,
			   gen_rtx_PLUS (Pmode,
					 stack_pointer_rtx,
					 GEN_INT (i * 4)));
      reg = gen_rtx_REG (SImode, save_regs_order[i]);
      x = emit_move_insn (mem, reg);
      RTX_FRAME_RELATED_P (x) = 1;
    }
}

static void
ubicom32_expand_prologue_2 (int adj)
{
  rtx x;
  int i;

  x = gen_addsi3 (stack_pointer_rtx,
		  stack_pointer_rtx,
		  GEN_INT (-(adj + save_regs_size)));
  x = emit_insn (x);
  RTX_FRAME_RELATED_P (x) = 1;

  for (i = 0; i < save_regs; i++)
    {
      rtx mem;
      rtx reg;

      mem = gen_frame_mem (SImode,
			   gen_rtx_PLUS (Pmode,
					 stack_pointer_rtx,
					 GEN_INT (i * 4)));
      reg = gen_rtx_REG (SImode, save_regs_order[i]);
      x = emit_move_insn (mem, reg);
      RTX_FRAME_RELATED_P (x) = 1;
    }
}

static void
ubicom32_expand_prologue_3 (int adj)
{
  rtx addr;
  rtx mem;
  rtx reg;
  rtx x;
  int i;

  addr = gen_rtx_PRE_MODIFY (Pmode,
			     stack_pointer_rtx,
			     gen_rtx_PLUS (Pmode,
					   stack_pointer_rtx,
					   GEN_INT (-(adj + 4))));
  mem = gen_frame_mem (SImode, addr);
  reg = gen_rtx_REG (SImode, save_regs_order[0]);
  x = emit_move_insn (mem, reg);
  RTX_FRAME_RELATED_P (x) = 1;

  for (i = 1; i < save_regs; i++)
    {
      mem = gen_frame_mem (SImode,
			   gen_rtx_PRE_DEC (Pmode, stack_pointer_rtx));
      reg = gen_rtx_REG (SImode, save_regs_order[i]);
      x = emit_move_insn (mem, reg);
      RTX_FRAME_RELATED_P (x) = 1;
    }
}

void
ubicom32_expand_prologue (void)
{
  rtx x;
  int outgoing_args_size = crtl->outgoing_args_size;
  int adj;

  /* For naked functions we don't emit any prologue.  */
  if (ubicom32_naked_function_p ())
    return;

  /* Reserve space for the stack frame for the local function.  */
  adj = get_frame_size () + crtl->args.pretend_args_size;

  ubicom32_layout_prologue_frame (adj);

  /* If we have a function using stdargs then flush the argument
     registers to the stack.  */
  if (cfun->stdarg)
    {
      int regno;
      for (regno = UBICOM32_FUNCTION_ARG_REGS - 1; regno >= 0; --regno)
	{
	  rtx mem;

	  mem = gen_rtx_MEM (SImode,
			     gen_rtx_PRE_DEC (Pmode, stack_pointer_rtx));
	  x = emit_move_insn (mem, gen_rtx_REG (SImode, regno));
	  RTX_FRAME_RELATED_P (x) = 1;
	}
    }

  if (save_regs)
    {
      switch (prologue_epilogue_type)
	{
	case 1:
	  ubicom32_expand_prologue_1 (adj);
	  break;

	case 2:
	  ubicom32_expand_prologue_2 (adj);
	  break;

	case 3:
	  ubicom32_expand_prologue_3 (adj);
	  break;

	default:
	  gcc_assert(false);
	}

      adj = 0;
    }

  if (adj + outgoing_args_size)
    {
      x = gen_addsi3 (stack_pointer_rtx,
		      stack_pointer_rtx,
		      GEN_INT (-(outgoing_args_size + adj)));
      x = emit_insn (x);
      RTX_FRAME_RELATED_P (x) = 1;
    }

  /* If we're using a frame pointer then generate it now.  */
  if (frame_pointer_needed)
    {
      int fp_adj = save_regs_size + outgoing_args_size;
      x = gen_addsi3 (frame_pointer_rtx,
		      stack_pointer_rtx,
		      GEN_INT (fp_adj));
      x = emit_insn (x);
      RTX_FRAME_RELATED_P (x) = 1;
    }
}

static bool
ubicom32_expand_epilogue_1 (int adj, bool sibcall)
{
  int i;
  int regno;
  rtx reg;
  rtx addr;
  rtx mem;

  for (i = save_regs - 1; i >= 1; i--)
    {
      reg = gen_rtx_REG (SImode, save_regs_order[i]);
      mem = gen_frame_mem (SImode,
			   gen_rtx_PLUS (Pmode,
					 stack_pointer_rtx,
					 GEN_INT (i * 4)));
      emit_move_insn (reg, mem);
    }

  regno = save_regs_order[0];
  addr = gen_rtx_POST_MODIFY (Pmode,
			      stack_pointer_rtx,
			      plus_constant (stack_pointer_rtx, adj + save_regs_size));
  mem = gen_frame_mem (SImode, addr);

  if (regno == LINK_REGNO && ! cfun->stdarg && ! sibcall)
    {
      emit_jump_insn (gen_return_internal (mem));
      return true;
    }

  emit_move_insn (gen_rtx_REG (SImode, regno), mem);

  /* If we have a sibcall and our link reg is not otherwise deemed to be
     live we need to make sure that it's classed as used so that we don't
     delete the restore.  */
  if (sibcall
      && save_link_reg
      && regno == LINK_REGNO)
    emit_use (gen_rtx_REG (SImode, LINK_REGNO));

  return false;
}

static bool
ubicom32_expand_epilogue_2 (int adj, bool sibcall)
{
  int i;
  rtx x;

  for (i = save_regs - 1; i >= 0; i--)
    {
      rtx reg;
      rtx mem;

      reg = gen_rtx_REG (SImode, save_regs_order[i]);
      mem = gen_frame_mem (SImode,
			   gen_rtx_PLUS (Pmode,
					 stack_pointer_rtx,
					 GEN_INT (i * 4)));
      emit_move_insn (reg, mem);
    }

  /* If we have a sibcall and our link reg is not otherwise deemed to be
     live we need to make sure that it's classed as used so that we don't
     delete the restore.  */
  if (sibcall
      && save_link_reg
      && save_regs_order[0] == LINK_REGNO)
    emit_use (gen_rtx_REG (SImode, LINK_REGNO));

  x = gen_addsi3 (stack_pointer_rtx,
		  stack_pointer_rtx,
		  GEN_INT (adj + save_regs_size));
  emit_insn (x);
  return false;
}

static bool
ubicom32_expand_epilogue_3 (int adj, bool sibcall)
{
  int i;
  int regno;
  rtx reg;
  rtx addr;
  rtx mem;

  for (i = save_regs - 1; i >= 1; i--)
    {
      reg = gen_rtx_REG (SImode, save_regs_order[i]);
      mem = gen_frame_mem (SImode, gen_rtx_POST_INC (Pmode, stack_pointer_rtx));
      emit_move_insn (reg, mem);
    }

  regno = save_regs_order[0];
  addr = gen_rtx_POST_MODIFY (Pmode,
			      stack_pointer_rtx,
			      plus_constant (stack_pointer_rtx, adj + 4));
  mem = gen_frame_mem (SImode, addr);

  if (regno == LINK_REGNO && ! cfun->stdarg && ! sibcall)
    {
      emit_jump_insn (gen_return_internal (mem));
      return true;
    }

  emit_move_insn (gen_rtx_REG (SImode, regno), mem);

  /* If we have a sibcall and our link reg is not otherwise deemed to be
     live we need to make sure that it's classed as used so that we don't
     delete the restore.  */
  if (sibcall
      && save_link_reg
      && regno == LINK_REGNO)
    emit_use (gen_rtx_REG (SImode, LINK_REGNO));

  return false;
}

void
ubicom32_expand_epilogue (bool sibcall)
{
  rtx x;
  int outgoing_args_size = crtl->outgoing_args_size;
  int adj;

  /* For naked functions we simply emit a return.
     Do we even need to do this?  */
  if (ubicom32_naked_function_p ())
    {
      emit_jump_insn (gen_return_internal (gen_rtx_REG (SImode,
							LINK_REGNO)));
      return;
    }

  /* If we've been using alloca then we need to adjust the stack
     pointer.  We have a frame pointer in these cases so we simply
     use that as the base and add an offset the size of the register
     save area.  */
  if (cfun->calls_alloca)
    {
      x = gen_addsi3 (stack_pointer_rtx,
		      frame_pointer_rtx,
		      GEN_INT (-save_regs_size));
      emit_insn (x);

      /* We've gone past the outgoing args area now.  */
      outgoing_args_size = 0;
    }

  /* If we have an area reserved for outgoing args then offset past
     that too.  */
  if (outgoing_args_size)
    {
      x = gen_addsi3 (stack_pointer_rtx,
		      stack_pointer_rtx,
		      GEN_INT (outgoing_args_size));
      emit_insn (x);
    }

  /* Do we have to adjust the stack after we've finished restoring regs?  */
  adj = get_frame_size() + crtl->args.pretend_args_size;
  if (save_regs)
    {
      switch (prologue_epilogue_type)
	{
	case 1:
	  if (ubicom32_expand_epilogue_1 (adj, sibcall))
	    return;
	  break;

	case 2:
	  if (ubicom32_expand_epilogue_2 (adj, sibcall))
	    return;
	  break;

	case 3:
	  if (ubicom32_expand_epilogue_3 (adj, sibcall))
	    return;
	  break;
	}

      adj = 0;
    }

  if (cfun->stdarg)
    adj += (UBICOM32_FUNCTION_ARG_REGS * UNITS_PER_WORD);

  if (adj)
    {
      x = gen_addsi3 (stack_pointer_rtx, stack_pointer_rtx, GEN_INT (adj));
      x = emit_insn (x);
    }

  if (! sibcall)
    emit_jump_insn (gen_return_internal (gen_rtx_REG (SImode, LINK_REGNO)));
}

void
ubicom32_expand_eh_return (rtx *operands)
{
  if (REG_P (operands[0])
      || REGNO (operands[0]) != EH_RETURN_STACKADJ_REGNO)
    {
      rtx sp = EH_RETURN_STACKADJ_RTX;
      emit_move_insn (sp, operands[0]);
      operands[0] = sp;
    }

  if (REG_P (operands[1])
      || REGNO (operands[1]) != EH_RETURN_HANDLER_REGNO)
    {
      rtx ra = EH_RETURN_HANDLER_RTX;
      emit_move_insn (ra, operands[1]);
      operands[1] = ra;
    }
}

/* Decide whether we can make a sibling call to a function.  DECL is the
   declaration of the function being targeted by the call and EXP is the
   CALL_EXPR representing the call.  */
static bool
ubicom32_function_ok_for_sibcall (tree decl ATTRIBUTE_UNUSED, tree exp ATTRIBUTE_UNUSED)
{
  /* We don't try to be clever with naked functions.  */
  if (ubicom32_naked_function_p ())
    return false;

  return true;
}

/* Compute the offsets between eliminable registers.  */

int
ubicom32_initial_elimination_offset (int from, int to)
{
  int i;
  int nregs = 0;

  if (! current_function_is_leaf || df_regs_ever_live_p (LINK_REGNO))
    nregs++;

  if (frame_pointer_needed || df_regs_ever_live_p (FRAME_POINTER_REGNUM))
    nregs++;

  /* Figure out which register(s) needs to be saved.  */
  for (i = 0; i <= LAST_ADDRESS_REGNUM; i++)
    if (df_regs_ever_live_p (i)
	&& ! call_used_regs[i]
	&& ! fixed_regs[i]
	&& i != LINK_REGNO
	&& i != FRAME_POINTER_REGNUM)
      nregs++;

  if (from == FRAME_POINTER_REGNUM && to == STACK_POINTER_REGNUM)
    return (4 * nregs)
	   + crtl->outgoing_args_size;

  if (from == ARG_POINTER_REGNUM && to == FRAME_POINTER_REGNUM)
    return get_frame_size ();

  if (from == ARG_POINTER_REGNUM && to == STACK_POINTER_REGNUM)
    return get_frame_size ()
	   + (4 * nregs)
	   + crtl->outgoing_args_size;

  return 0;
}

/* Attributes and CC handling.  */

/* Handle an attribute requiring a FUNCTION_DECL; arguments as in
   struct attribute_spec.handler.  */
static tree
ubicom32_handle_fndecl_attribute (tree *node, tree name,
			      tree args ATTRIBUTE_UNUSED,
			      int flags ATTRIBUTE_UNUSED,
			      bool *no_add_attrs)
{
  if (TREE_CODE (*node) != FUNCTION_DECL)
    {
      warning (0, "'%s' attribute only applies to functions",
	       IDENTIFIER_POINTER (name));
      *no_add_attrs = true;
    }

  return NULL_TREE;
}

/* A C expression that places additional restrictions on the register class to
   use when it is necessary to copy value X into a register in class CLASS.
   The value is a register class; perhaps CLASS, or perhaps another, smaller
   class.  On many machines, the following definition is safe:

	#define PREFERRED_RELOAD_CLASS(X,CLASS) CLASS

   Sometimes returning a more restrictive class makes better code.  For
   example, on the 68000, when X is an integer constant that is in range for a
   `moveq' instruction, the value of this macro is always `DATA_REGS' as long
   as CLASS includes the data registers.  Requiring a data register guarantees
   that a `moveq' will be used.

   If X is a `const_double', by returning `NO_REGS' you can force X into a
   memory constant.  This is useful on certain machines where immediate
   floating values cannot be loaded into certain kinds of registers.  */

enum reg_class
ubicom32_preferred_reload_class (rtx x, enum reg_class rclass)
{
  /* If we're targetting FD-PIC then if we find an attempt to reload the
     FD-PIC GOT pointer we have a strong preference for putting it back
     in the FDPIC_REGNUM!  */
  if (TARGET_FDPIC)
    {
      rtx fdpic_reg;
      fdpic_reg = has_hard_reg_initial_val (SImode, FDPIC_REGNUM);

      if (fdpic_reg
	  && rtx_equal_p (x, fdpic_reg)
	  && ORIGINAL_REGNO (x) == ORIGINAL_REGNO (fdpic_reg))
	{
	  if (reg_class_subset_p (FDPIC_REG, rclass))
	    return FDPIC_REG;
	}
    }

  /* GOT-based references are handled as UNSPECs.  We want put the _1 versions
     in a DATA_REGS register because they're used in conjunction with an address
     register to form an address.  */
  if (GET_CODE (x) == UNSPEC)
    {
      if (XINT (x, 1) == UNSPEC_FDPIC_GOT_1
	  || XINT (x, 1) == UNSPEC_FDPIC_FUNCDESC_GOT_1)
	return DATA_REGS;
    }

  /* If a symbolic constant, HIGH or a PLUS is reloaded,
     it is most likely being used as an address, so
     prefer ADDRESS_REGS.  If 'rclass' is not a superset
     of ADDRESS_REGS, e.g. DATA_REGS, then reject this reload.  */
  if (GET_CODE (x) == CONST
      || GET_CODE (x) == PLUS
      || GET_CODE (x) == HIGH
      || GET_CODE (x) == LABEL_REF
      || GET_CODE (x) == SYMBOL_REF)
    {
      if (reg_class_subset_p (ALL_ADDRESS_REGS, rclass))
	return ALL_ADDRESS_REGS;

      return NO_REGS;
    }

  return rclass;
}

/* Function arguments and varargs.  */

int
ubicom32_reg_parm_stack_space (tree fndecl)
{
  return 0;

  if (fndecl
      && TYPE_ARG_TYPES (TREE_TYPE (fndecl)) != 0
      && (TREE_VALUE (tree_last (TYPE_ARG_TYPES (TREE_TYPE (fndecl))))
	  != void_type_node))
    return UBICOM32_FUNCTION_ARG_REGS * UNITS_PER_WORD;

  return 0;
}

void
ubicom32_va_start (tree valist, rtx nextarg)
{
  std_expand_builtin_va_start (valist, nextarg);
}

rtx
ubicom32_va_arg (tree valist, tree type)
{
  HOST_WIDE_INT size, rsize;
  tree addr, incr, tmp;
  rtx addr_rtx;
  int indirect = 0;

  /* Round up sizeof(type) to a word.  */
  size = int_size_in_bytes (type);
  rsize = (size + UNITS_PER_WORD - 1) & -UNITS_PER_WORD;

  /* Large types are passed by reference.  */
  if (size > 8)
    {
      indirect = 1;
      size = rsize = UNITS_PER_WORD;
    }

  incr = valist;
  addr = incr = save_expr (incr);

  /* FIXME Nat's version - is it correct?  */
  tmp = fold_convert (ptr_type_node, size_int (rsize));
  tmp = build2 (PLUS_EXPR, ptr_type_node, incr, tmp);
  incr = fold (tmp);

  /* FIXME Nat's version - is it correct? */
  incr = build2 (MODIFY_EXPR, ptr_type_node, valist, incr);

  TREE_SIDE_EFFECTS (incr) = 1;
  expand_expr (incr, const0_rtx, VOIDmode, EXPAND_NORMAL);

  addr_rtx = expand_expr (addr, NULL, Pmode, EXPAND_NORMAL);

  if (size < UNITS_PER_WORD)
    emit_insn (gen_addsi3 (addr_rtx,
			   addr_rtx,
			   GEN_INT (UNITS_PER_WORD - size)));

  if (indirect)
    {
      addr_rtx = force_reg (Pmode, addr_rtx);
      addr_rtx = gen_rtx_MEM (Pmode, addr_rtx);
      set_mem_alias_set (addr_rtx, get_varargs_alias_set ());
    }

  return addr_rtx;
}

void
ubicom32_init_cumulative_args (CUMULATIVE_ARGS *cum, tree fntype, rtx libname)
{
  cum->nbytes = 0;

  if (!libname)
    {
      cum->stdarg = (TYPE_ARG_TYPES (fntype) != 0
		     && (TREE_VALUE (tree_last (TYPE_ARG_TYPES (fntype)))
			 != void_type_node));
    }
}

/* Return an RTX to represent where a value in mode MODE will be passed
   to a function.  If the result is 0, the argument will be pushed.  */

rtx
function_arg (CUMULATIVE_ARGS *cum, enum machine_mode mode, tree type,
	      int named ATTRIBUTE_UNUSED)
{
  rtx result = 0;
  int size, align;
  int nregs = UBICOM32_FUNCTION_ARG_REGS;

  /* Figure out the size of the object to be passed.  */
  if (mode == BLKmode)
    size = int_size_in_bytes (type);
  else
    size = GET_MODE_SIZE (mode);

  /* Figure out the alignment of the object to be passed.  */
  align = size;

  cum->nbytes = (cum->nbytes + 3) & ~3;

  /* Don't pass this arg via a register if all the argument registers
     are used up.  */
  if (cum->nbytes >= nregs * UNITS_PER_WORD)
    return 0;

  /* Don't pass this arg via a register if it would be split between
     registers and memory.  */
  result = gen_rtx_REG (mode, cum->nbytes / UNITS_PER_WORD);

  return result;
}

rtx
function_incoming_arg (CUMULATIVE_ARGS *cum, enum machine_mode mode, tree type,
		       int named ATTRIBUTE_UNUSED)
{
  if (cfun->stdarg)
    return 0;

  return function_arg (cum, mode, type, named);
}


/* Implement hook TARGET_ARG_PARTIAL_BYTES.

   Returns the number of bytes at the beginning of an argument that
   must be put in registers.  The value must be zero for arguments
   that are passed entirely in registers or that are entirely pushed
   on the stack.  */
static int
ubicom32_arg_partial_bytes (CUMULATIVE_ARGS *cum, enum machine_mode mode,
			    tree type, bool named ATTRIBUTE_UNUSED)
{
  int size, diff;

  int nregs = UBICOM32_FUNCTION_ARG_REGS;

  /* round up to full word */
  cum->nbytes = (cum->nbytes + 3) & ~3;

  if (targetm.calls.pass_by_reference (cum, mode, type, named))
      return 0;

  /* number of bytes left in registers */
  diff = nregs * UNITS_PER_WORD - cum->nbytes;

  /* regs all used up */
  if (diff <= 0)
    return 0;

  /* Figure out the size of the object to be passed.  */
  if (mode == BLKmode)
    size = int_size_in_bytes (type);
  else
    size = GET_MODE_SIZE (mode);

  /* enough space left in regs for size */
  if (size <= diff)
    return 0;

  /* put diff bytes in regs and rest on stack */
  return diff;

}

static bool
ubicom32_pass_by_reference (CUMULATIVE_ARGS *ca ATTRIBUTE_UNUSED,
			    enum machine_mode mode, const_tree type,
			    bool named ATTRIBUTE_UNUSED)
{
  int size;

  if (type)
    size = int_size_in_bytes (type);
  else
    size = GET_MODE_SIZE (mode);

  return size <= 0 || size > 8;
}

static bool
ubicom32_callee_copies (CUMULATIVE_ARGS *ca ATTRIBUTE_UNUSED,
			enum machine_mode mode, const_tree type,
			bool named ATTRIBUTE_UNUSED)
{
  int size;

  if (type)
    size = int_size_in_bytes (type);
  else
    size = GET_MODE_SIZE (mode);

  return size <= 0 || size > 8;
}

static bool
ubicom32_return_in_memory (const_tree type, const_tree fntype ATTRIBUTE_UNUSED)
{
  int size, mode;

  if (!type)
    return true;

  size = int_size_in_bytes(type);
  if (size > 8)
    return true;

  mode = TYPE_MODE(type);
  if (mode == BLKmode)
    return true;

  return false;
}

/* Return true if a given register number REGNO is acceptable for machine
   mode MODE.  */
bool
ubicom32_hard_regno_mode_ok (unsigned int regno, enum machine_mode mode)
{
  /* If we're not at least a v3 ISA then ACC0_HI is only 16 bits.  */
  if (! ubicom32_v3)
    {
      if (regno == ACC0_HI_REGNUM)
	return (mode == QImode || mode == HImode);
    }

  /* Only the flags reg can hold CCmode.  */
  if (GET_MODE_CLASS (mode) == MODE_CC)
    return regno == CC_REGNUM;

  /* We restrict the choice of DImode and DFmode registers to only being 
     address, data or accumulator regs.  We also restrict them to only start
     on even register numbers so we never have to worry about partial
     overlaps between operands in instructions.  */
  if (GET_MODE_SIZE (mode) > 4)
    {
      switch (REGNO_REG_CLASS (regno))
	{
	case ADDRESS_REGS:
	case SIBCALL_ADDRESS_REGS:
	case DATA_REGS:
	case ACC_REGS:
	  return (regno & 1) == 0;

	default:
	  return false;
	}
    }

  return true;
}

/* The macros REG_OK_FOR..._P assume that the arg is a REG rtx
   and check its validity for a certain class.
   We have two alternate definitions for each of them.
   The usual definition accepts all pseudo regs; the other rejects
   them unless they have been allocated suitable hard regs.
   The symbol REG_OK_STRICT causes the latter definition to be used.

   Most source files want to accept pseudo regs in the hope that
   they will get allocated to the class that the insn wants them to be in.
   Source files for reload pass need to be strict.
   After reload, it makes no difference, since pseudo regs have
   been eliminated by then.

   These assume that REGNO is a hard or pseudo reg number.
   They give nonzero only if REGNO is a hard reg of the suitable class
   or a pseudo reg currently allocated to a suitable hard reg.
   Since they use reg_renumber, they are safe only once reg_renumber
   has been allocated, which happens in local-alloc.c.  */

int
ubicom32_regno_ok_for_base_p (int regno, int strict)
{
  if ((regno >= FIRST_ADDRESS_REGNUM && regno <= STACK_POINTER_REGNUM)
      || (!strict
	  && (regno >= FIRST_PSEUDO_REGISTER
	      || regno == ARG_POINTER_REGNUM))
      || (strict && (reg_renumber
		     && reg_renumber[regno] >= FIRST_ADDRESS_REGNUM
		     && reg_renumber[regno] <= STACK_POINTER_REGNUM)))
    return 1;

  return 0;
}

int
ubicom32_regno_ok_for_index_p (int regno, int strict)
{
  if ((regno >= FIRST_DATA_REGNUM && regno <= LAST_DATA_REGNUM)
      || (!strict && regno >= FIRST_PSEUDO_REGISTER)
      || (strict && (reg_renumber
		     && reg_renumber[regno] >= FIRST_DATA_REGNUM
		     && reg_renumber[regno] <= LAST_DATA_REGNUM)))
    return 1;

  return 0;
}

/* Returns 1 if X is a valid index register.  STRICT is 1 if only hard
   registers should be accepted.  Accept either REG or SUBREG where a
   register is valid.  */

static bool
ubicom32_is_index_reg (rtx x, int strict)
{
  if ((REG_P (x) && ubicom32_regno_ok_for_index_p (REGNO (x), strict))
      || (GET_CODE (x) == SUBREG
	  && REG_P (SUBREG_REG (x))
	  && ubicom32_regno_ok_for_index_p (REGNO (SUBREG_REG (x)), strict)))
    return true;

  return false;
}

/* Return 1 if X is a valid index for a memory address.  */

static bool
ubicom32_is_index_expr (enum machine_mode mode, rtx x, int strict)
{
  /* Immediate index must be an unsigned 7-bit offset multiple of 1, 2
     or 4 depending on mode.  */
  if (CONST_INT_P (x))
    {
      switch (mode)
	{
	case QImode:
	  return satisfies_constraint_J (x);

	case HImode:
	  return satisfies_constraint_K (x);

	case SImode:
	case SFmode:
	  return satisfies_constraint_L (x);

	case DImode:
	case DFmode:
	  return satisfies_constraint_L (x)
		 && satisfies_constraint_L (GEN_INT (INTVAL (x) + 4));

	default:
	  return false;
	}
    }

  if (mode != SFmode && mode != SImode && mode != HImode && mode != QImode)
    return false;

  /* Register index scaled by mode of operand: REG + REG * modesize.
     Valid scaled index registers are:

     SImode	(mult (dreg) 4))
     HImode	(mult (dreg) 2))
     QImode	(mult (dreg) 1))  */
  if (GET_CODE (x) == MULT
      && ubicom32_is_index_reg (XEXP (x, 0), strict)
      && CONST_INT_P (XEXP (x, 1))
      && INTVAL (XEXP (x, 1)) == (HOST_WIDE_INT)GET_MODE_SIZE (mode))
    return true;

  /* REG + REG addressing is allowed for QImode.  */
  if (ubicom32_is_index_reg (x, strict) && mode == QImode)
    return true;

  return false;
}

static bool
ubicom32_is_valid_offset (enum machine_mode mode, HOST_WIDE_INT offs)
{
  if (offs < 0)
    return false;

  switch (mode)
    {
    case QImode:
      return offs <= 127;

    case HImode:
      return offs <= 254;

    case SImode:
    case SFmode:
      return offs <= 508;

    case DImode:
    case DFmode:
      return offs <= 504;

    default:
      return false;
    }
}

static int
ubicom32_get_valid_offset_mask (enum machine_mode mode)
{
  switch (mode)
    {
    case QImode:
      return 127;

    case HImode:
      return 255;

    case SImode:
    case SFmode:
      return 511;

    case DImode:
    case DFmode:
      return 255;

    default:
      return 0;
    }
}

/* Returns 1 if X is a valid base register.  STRICT is 1 if only hard
   registers should be accepted.  Accept either REG or SUBREG where a
   register is valid.  */

static bool
ubicom32_is_base_reg (rtx x, int strict)
{
  if ((REG_P (x) && ubicom32_regno_ok_for_base_p (REGNO (x), strict))
      || (GET_CODE (x) == SUBREG
	  && REG_P (SUBREG_REG (x))
	  && ubicom32_regno_ok_for_base_p (REGNO (SUBREG_REG (x)), strict)))
    return true;

  return false;
}

static bool
ubicom32_cannot_force_const_mem (rtx x ATTRIBUTE_UNUSED)
{
  return TARGET_FDPIC;
}

/* Determine if X is a legitimate constant.  */

bool
ubicom32_legitimate_constant_p (rtx x)
{
  /* Among its other duties, LEGITIMATE_CONSTANT_P decides whether
     a constant can be entered into reg_equiv_constant[].  If we return true,
     reload can create new instances of the constant whenever it likes.

     The idea is therefore to accept as many constants as possible (to give
     reload more freedom) while rejecting constants that can only be created
     at certain times.  In particular, anything with a symbolic component will
     require use of the pseudo FDPIC register, which is only available before
     reload.  */
  if (TARGET_FDPIC)
    {
      if (GET_CODE (x) == SYMBOL_REF
	  || (GET_CODE (x) == CONST
	      && GET_CODE (XEXP (x, 0)) == PLUS
	      && GET_CODE (XEXP (XEXP (x, 0), 0)) == SYMBOL_REF)
	  || CONSTANT_ADDRESS_P (x))
	return false;

      return true;
    }

  /* For non-PIC code anything goes!  */
  return true;
}

/* Address validation.  */

bool
ubicom32_legitimate_address_p (enum machine_mode mode, rtx x, int strict)
{
  if (CONSTANT_ADDRESS_P (x))
    return false;

  if (ubicom32_is_base_reg (x, strict))
    return true;

  if ((GET_CODE (x) == POST_INC
       || GET_CODE (x) == PRE_INC
       || GET_CODE (x) == POST_DEC
       || GET_CODE (x) == PRE_DEC)
      && REG_P (XEXP (x, 0))
      && ubicom32_is_base_reg (XEXP (x, 0), strict)
      && mode != DImode
      && mode != DFmode)
    return true;

  if ((GET_CODE (x) == PRE_MODIFY || GET_CODE (x) == POST_MODIFY)
      && ubicom32_is_base_reg (XEXP (x, 0), strict)
      && GET_CODE (XEXP (x, 1)) == PLUS
      && rtx_equal_p (XEXP (x, 0), XEXP (XEXP (x, 1), 0))
      && CONST_INT_P (XEXP (XEXP (x, 1), 1))
      && mode != DImode
      && mode != DFmode)
    {
      HOST_WIDE_INT disp = INTVAL (XEXP (XEXP (x, 1), 1));
      switch (mode)
	{
	case QImode:
	  return disp >= -8 && disp <= 7;

	case HImode:
	  return disp >= -16 && disp <= 14 && ! (disp & 1);

	case SImode:
	  return disp >= -32 && disp <= 28 && ! (disp & 3);

	default:
	  return false;
	}
    }

  /* Accept base + index * scale.

     Our index must be valid except in cases where we're not yet using
     strict addressing and the index is into the stack - in those cases
     we're likely to see register elimination adjust those offsets anyway
     so we leave it to later to legitimize things.  */
  if (GET_CODE (x) == PLUS
      && ubicom32_is_base_reg (XEXP (x, 0), strict)
      && (ubicom32_is_index_expr (mode, XEXP (x, 1), strict)
	  || (!strict
	      && CONST_INT_P (XEXP (x, 1))
	      && REGNO_PTR_FRAME_P (REGNO (XEXP (x, 0))))))
    return true;

  /* Accept index * scale + base.

     Our index must be valid except in cases where we're not yet using
     strict addressing and the index is into the stack - in those cases
     we're likely to see register elimination adjust those offsets anyway
     so we leave it to later to legitimize things.  */
  if (GET_CODE (x) == PLUS
      && ubicom32_is_base_reg (XEXP (x, 1), strict)
      && (ubicom32_is_index_expr (mode, XEXP (x, 0), strict)
	  || (!strict
	      && CONST_INT_P (XEXP (x, 0))
	      && REGNO_PTR_FRAME_P (REGNO (XEXP (x, 1))))))
    return true;

  if (! TARGET_FDPIC)
    {
      /* Accept (lo_sum (reg) (symbol_ref)) that can be used as a mem+7bits
	 displacement operand:

	 moveai a1, #%hi(SYM)
	 move.4 d3, %lo(SYM)(a1)  */
      if (GET_CODE (x) == LO_SUM
	  && ubicom32_is_base_reg (XEXP (x, 0), strict)
	  && (GET_CODE (XEXP (x, 1)) == SYMBOL_REF
	      || GET_CODE (XEXP (x, 1)) == LABEL_REF /* FIXME: wrong */)
	  && mode != DImode
	  && mode != DFmode)
	return true;
    }

  return false;
}

rtx
ubicom32_legitimize_address (rtx x, rtx oldx ATTRIBUTE_UNUSED,
			     enum machine_mode mode)
{
  if (mode == BLKmode)
    return NULL_RTX;

  if (GET_CODE (x) == PLUS
      && REG_P (XEXP (x, 0))
      && ! REGNO_PTR_FRAME_P (REGNO (XEXP (x, 0)))
      && CONST_INT_P (XEXP (x, 1))
      && ! ubicom32_is_valid_offset (mode, INTVAL (XEXP (x, 1))))
    {
      rtx base;
      rtx plus;
      rtx new_rtx;
      HOST_WIDE_INT val = INTVAL (XEXP (x, 1));
      HOST_WIDE_INT low = val & ubicom32_get_valid_offset_mask (mode);
      HOST_WIDE_INT high = val ^ low;

      if (val < 0)
	return NULL_RTX;

      if (! low)
	return NULL_RTX;

      /* Reload the high part into a base reg; leave the low part
	 in the mem directly.  */
      base = XEXP (x, 0);
      if (! ubicom32_is_base_reg (base, 0))
	base = copy_to_mode_reg (Pmode, base);

      plus = expand_simple_binop (Pmode, PLUS,
				  gen_int_mode (high, Pmode),
				  base, NULL, 0, OPTAB_WIDEN);
      new_rtx = plus_constant (plus, low);

      return new_rtx;
    }

  return NULL_RTX;
}

/* Try a machine-dependent way of reloading an illegitimate address AD
   operand.  If we find one, push the reload and and return the new address.

   MODE is the mode of the enclosing MEM.  OPNUM is the operand number
   and TYPE is the reload type of the current reload.  */

rtx
ubicom32_legitimize_reload_address (rtx ad, enum machine_mode mode,
				    int opnum, int type)
{
  /* Is this an address that we've already fixed up?  If it is then
     recognize it and move on.  */
  if (GET_CODE (ad) == PLUS
      && GET_CODE (XEXP (ad, 0)) == PLUS
      && REG_P (XEXP (XEXP (ad, 0), 0))
      && CONST_INT_P (XEXP (XEXP (ad, 0), 1))
      && CONST_INT_P (XEXP (ad, 1)))
    {
      push_reload (XEXP (ad, 0), NULL_RTX, &XEXP (ad, 0), NULL,
		   BASE_REG_CLASS, Pmode, VOIDmode, 0, 0,
		   opnum, (enum reload_type) type);
      return ad;
    }

  /* Have we got an address where the offset is simply out of range?  If
     yes then reload the range as a high part and smaller offset.  */
  if (GET_CODE (ad) == PLUS
      && REG_P (XEXP (ad, 0))
      && REGNO (XEXP (ad, 0)) < FIRST_PSEUDO_REGISTER
      && REGNO_OK_FOR_BASE_P (REGNO (XEXP (ad, 0)))
      && CONST_INT_P (XEXP (ad, 1))
      && ! ubicom32_is_valid_offset (mode, INTVAL (XEXP (ad, 1))))
    {
      rtx temp;
      rtx new_rtx;

      HOST_WIDE_INT val = INTVAL (XEXP (ad, 1));
      HOST_WIDE_INT low = val & ubicom32_get_valid_offset_mask (mode);
      HOST_WIDE_INT high = val ^ low;

      /* Reload the high part into a base reg; leave the low part
	 in the mem directly.  */
      temp = gen_rtx_PLUS (Pmode, XEXP (ad, 0), GEN_INT (high));
      new_rtx = gen_rtx_PLUS (Pmode, temp, GEN_INT (low));

      push_reload (XEXP (new_rtx, 0), NULL_RTX, &XEXP (new_rtx, 0), NULL,
		   BASE_REG_CLASS, Pmode, VOIDmode, 0, 0,
		   opnum, (enum reload_type) type);
      return new_rtx;
    }

  /* If we're presented with an pre/post inc/dec then we must force this
     to be done in an address register.  The register allocator should
     work this out for itself but at times ends up trying to use the wrong
     class.  If we get the wrong class then reload will end up generating
     at least 3 instructions whereas this way we can hopefully keep it to
     just 2.  */
  if ((GET_CODE (ad) == POST_INC
       || GET_CODE (ad) == PRE_INC
       || GET_CODE (ad) == POST_DEC
       || GET_CODE (ad) == PRE_DEC)
      && REG_P (XEXP (ad, 0))
      && REGNO (XEXP (ad, 0)) < FIRST_PSEUDO_REGISTER
      && ! REGNO_OK_FOR_BASE_P (REGNO (XEXP (ad, 0))))
    {
      push_reload (XEXP (ad, 0), XEXP (ad, 0), &XEXP (ad, 0), &XEXP (ad, 0),
		   BASE_REG_CLASS, GET_MODE (XEXP (ad, 0)), GET_MODE (XEXP (ad, 0)), 0, 0,
		   opnum, RELOAD_OTHER);
      return ad;
    }

  return NULL_RTX;
}

static int
ubicom32_address_cost (rtx x, bool speed ATTRIBUTE_UNUSED)
{
  enum rtx_code c  = GET_CODE (x);

  if (c == PRE_INC
      || c == PRE_DEC
      || c == POST_INC
      || c == POST_DEC
      || c == PRE_MODIFY
      || c == POST_MODIFY)
    return 0;

  if (c == LABEL_REF)
    return 0;

  if (c == SYMBOL_REF)
    return 10;

  if (c == CONST)
    return 10;

  if (c == HIGH)
    return 10;

  if (c == LO_SUM)
    return 0;

  if (c == REG)
    return REGNO (x) <  FIRST_PSEUDO_REGISTER ? 5 : 6;

  if (c == PLUS
      && GET_CODE (XEXP (x, 0)) == REG
      && GET_CODE (XEXP (x, 1)) == CONST_INT)
    return REGNO (XEXP (x, 0)) <  FIRST_PSEUDO_REGISTER ? 1 : 2;

  return 3;
}

/* Compute a (partial) cost for rtx X.  Return true if the complete
   cost has been computed, and false if subexpressions should be
   scanned.  In either case, *TOTAL contains the cost result.  */

static bool
ubicom32_rtx_costs (rtx x, int code, int outer_code, int *total,
		    bool speed ATTRIBUTE_UNUSED)
{
  enum machine_mode mode = GET_MODE (x);

  switch (code)
    {
    case CONST_INT:
      /* Very short constants often fold into instructions so
	 we pretend that they don't cost anything!  This is
	 really important as regards zero values as otherwise
	 the compiler has a nasty habit of wanting to reuse
	 zeroes that are in regs but that tends to pessimize
	 the code.  */
      if (satisfies_constraint_I (x))
	{
	  *total = 0;
	  return true;
	}

      /* Bit clearing costs nothing  */
      if (outer_code == AND
	  && exact_log2 (0xffffffff & ~INTVAL (x)) != -1)
	{
	  *total = 0;
	  return true;
	}

      /* Masking the lower set of bits costs nothing.  */
      if (outer_code == AND
	  && exact_log2 (INTVAL (x) + 1) != -1)
	{
	  *total = 0;
	  return true;
	}

      /* Bit setting costs nothing.  */
      if (outer_code == IOR
	  && exact_log2 (0xffffffff & INTVAL (x)) != -1)
	{
	  *total = 0;
	  return true;
	}

      /* Larger constants that can be loaded via movei aren't too
	 bad.  If we're just doing a set they cost nothing extra.  */
      if (satisfies_constraint_N1 (x))
	{
	  if (mode == DImode)
	    *total = COSTS_N_INSNS (2);
	  else
	    *total = COSTS_N_INSNS (1);
	  return true;
	}

      /* This might be an address literal which we can load via moveai.
	 This doesn't work that well as an accurate measure because we can
	 only load address registers this way so we weight it slightly
	 more expensive than we might otherwise want to.  */
      if (mode == SImode
	  && (((ubicom32_v5 ? 0xffffff80 : 0x7fffff80) & INTVAL (x))
	      == (0xffffffff & INTVAL (x))))
	{
	  *total = COSTS_N_INSNS (1) + 1;
	  return true;
	}

      /* This might be something we can load via a moveai/lea.1 pair.  This
	 doesn't work that well as an accurate measure because we can only
	 load this way if we have a spare address register so we weight it
	 slightly more expensive than we might otherwise want to.  */
      if (mode == SImode)
	{
	  if (ubicom32_v5
	      || ((0x7fffffff & INTVAL (x)) == INTVAL (x)))
	    *total = COSTS_N_INSNS (2) + 1;
	  else
	    *total = COSTS_N_INSNS (3);
	  return true;
	}

      if (mode == DImode)
	*total = COSTS_N_INSNS (5);
      else
	*total = COSTS_N_INSNS (3);
      return true;

    case CONST_DOUBLE:
      /* We don't optimize CONST_DOUBLEs well nor do we relax them well,
	 so their cost is very high.  */
      *total = COSTS_N_INSNS (6);
      return true;

    case CONST:
      if (GET_CODE (x) == UNSPEC)
	*total = COSTS_N_INSNS (1);
      else
	*total = 0;
      return true;

    case SYMBOL_REF:
    case LABEL_REF:
    case HIGH:
    case LO_SUM:
      *total = 0;
      return true;

    case UNSPEC:
      *total = COSTS_N_INSNS (1);
      return true;

    case MEM:
      if (mode == DImode || mode == DFmode)
	*total = COSTS_N_INSNS (2 + 1);
      else
	*total = COSTS_N_INSNS (1 + 1);
      return true;

    case IF_THEN_ELSE:
      *total = COSTS_N_INSNS (1);
      return true;

    case BSWAP:
    case PLUS:
    case MINUS:
    case AND:
    case IOR:
    case XOR:
    case ASHIFT:
    case ASHIFTRT:
    case LSHIFTRT:
    case NEG:
    case NOT:
    case SIGN_EXTEND:
      if (outer_code != SET)
	*total = 0;
      else
	{
	  if (TARGET_HARD_FLOAT && (mode == SFmode || mode == DFmode))
	    *total = COSTS_N_INSNS (2);
	  else if (mode == DImode)
	    *total = COSTS_N_INSNS (2);
	  else
	    *total = COSTS_N_INSNS (1);
	}
      return true;

    case ZERO_EXTRACT:
      if (outer_code != SET)
	*total = 0;
      else
	{
	  /* Zero extracts are either done with shift instructions or bfextu.
	     In general anything with a bit offset of 0 can be handled either
	     way and costs one ihstruction, but bfextu operations with a
	     non-zero offset require an extra register load first.  */
	  if (INTVAL (XEXP (x, 2)) == 0)
	    *total = COSTS_N_INSNS (1);
	  else
	    *total = COSTS_N_INSNS (2);
	}
      return true;

    case ZERO_EXTEND:
      *total = COSTS_N_INSNS (1);
      return true;

    case COMPARE:
      if (outer_code != SET)
	*total = 0;
      else
	{
	  if (GET_MODE (XEXP (x, 0)) == DImode
	      || GET_MODE (XEXP (x, 1)) == DImode)
	    *total = COSTS_N_INSNS (2);
	  else
	    *total = COSTS_N_INSNS (1);
	}
      return true;

    case UMOD:
    case UDIV:
    case MOD:
    case DIV:
      if (outer_code != SET)
	*total = 0;
      else
	{
	  if (TARGET_HARD_FLOAT && mode == SFmode)
	    *total = COSTS_N_INSNS (18);
	  if (TARGET_HARD_FLOAT && mode == DFmode)
	    *total = COSTS_N_INSNS (33);
	  else if (mode == DImode)
	    *total = COSTS_N_INSNS (600);
	  else
	    *total = COSTS_N_INSNS (135);
	}
      return true;

    case MULT:
      if (outer_code != SET)
	*total = 0;
      else
	{
	  if (TARGET_HARD_FLOAT && (mode == SFmode || mode == DFmode))
	    *total = COSTS_N_INSNS (2);
	  else if (! ubicom32_v4)
	    {
	      if (mode == DImode)
		*total = COSTS_N_INSNS (15);
	      else
		*total = COSTS_N_INSNS (5);
	    }
	  else
	    {
	      if (mode == DImode)
		*total = COSTS_N_INSNS (6);
	      else
		*total = COSTS_N_INSNS (2);
	    }
	}
      return true;

    default:
      return false;
    }
}

/* Return 1 if ADDR can have different meanings depending on the machine
   mode of the memory reference it is used for or if the address is
   valid for some modes but not others.

   Autoincrement and autodecrement addresses typically have
   mode-dependent effects because the amount of the increment or
   decrement is the size of the operand being addressed.  Some machines
   have other mode-dependent addresses. Many RISC machines have no
   mode-dependent addresses.

   You may assume that ADDR is a valid address for the machine.  */

int
ubicom32_mode_dependent_address_p (rtx addr)
{
  if (GET_CODE (addr) == POST_INC
      || GET_CODE (addr) == PRE_INC
      || GET_CODE (addr) == POST_DEC
      || GET_CODE (addr) == PRE_DEC
      || GET_CODE (addr) == POST_MODIFY
      || GET_CODE (addr) == PRE_MODIFY)
    return 1;

  return 0;
}

static void
ubicom32_function_prologue (FILE *file, HOST_WIDE_INT size ATTRIBUTE_UNUSED)
{
  fprintf (file, "/* frame/pretend: %ld/%d save_regs: %d out_args: %d  %s */\n",
	   get_frame_size (), crtl->args.pretend_args_size,
	   save_regs_size, crtl->outgoing_args_size,
	   current_function_is_leaf ? "leaf" : "nonleaf");
}

static void
ubicom32_function_epilogue (FILE *file ATTRIBUTE_UNUSED,
			    HOST_WIDE_INT size ATTRIBUTE_UNUSED)
{
}

/* Implement TARGET_ASM_FUNCTION_RODATA_SECTION.

   The complication here is that, with the combination TARGET_ABICALLS
   && !TARGET_ABSOLUTE_ABICALLS && !TARGET_GPWORD, jump tables will use
   absolute addresses, and should therefore not be included in the
   read-only part of a DSO.  Handle such cases by selecting a normal
   data section instead of a read-only one.  The logic apes that in
   default_function_rodata_section.  */

static section *
ubicom32_function_rodata_section (tree decl)
{
#if 0
  /* This was borrowed from the MIPS port - wonder if we can do something
     more clever for Ubicom32 too?  */
  if (!TARGET_ABICALLS || TARGET_ABSOLUTE_ABICALLS || TARGET_GPWORD)
    return default_function_rodata_section (decl);
#endif

  if (decl && DECL_SECTION_NAME (decl))
    {
      const char *name = TREE_STRING_POINTER (DECL_SECTION_NAME (decl));
      if (DECL_ONE_ONLY (decl) && strncmp (name, ".gnu.linkonce.t.", 16) == 0)
	{
	  char *rname = ASTRDUP (name);
	  rname[14] = 'd';
	  return get_section (rname, SECTION_LINKONCE | SECTION_WRITE, decl);
	}
      else if (flag_function_sections
	       && flag_data_sections
	       && strncmp (name, ".text.", 6) == 0)
	{
	  char *rname = ASTRDUP (name);
	  memcpy (rname + 1, "data", 4);
	  return get_section (rname, SECTION_WRITE, decl);
	}
    }
  return data_section;
}

/* Return attribute type of insn.  */

static enum attr_type
ubicom32_safe_attr_type (rtx insn)
{
  if (recog_memoized (insn) < 0)
    return TYPE_OTHER;

  return get_attr_type (insn);
}

/* Return attribute nop value of insn.  */

static int
ubicom32_safe_attr_nop (rtx insn)
{
  enum attr_nop an;
  if (recog_memoized (insn) < 0)
    return 0;

  an = get_attr_nop (insn);
  return (int)an;
}

static void
ubicom32_hazard_scan (basic_block bb, int range, int addr_pad, int mac_pad, int fpu_pad)
{
  rtx def_insn;
  FOR_BB_INSNS (bb, def_insn)
    {
      int j;
      rtx use_insn;
      rtx insert_before = NULL_RTX;
      int nop_padding;

      /* Look for real instructions that have at least one more before the
	 end of the basic block.  */
      if (! (INSN_P (def_insn)
	     && GET_CODE (PATTERN (def_insn)) != USE
	     && GET_CODE (PATTERN (def_insn)) != CLOBBER))
	continue;

      /* If we have a call instruction then we're not going to see a hazard
	 from anything it defines.  Strictly this might not be completely
	 true if we have a BTB but we have no way of knowing and usually
	 a return value is defined at least a few instructions before any
	 epilogue.  */
      if (CALL_P (def_insn))
	continue;

      if (def_insn == BB_END (bb))
	continue;

      /* Scan forward I instructions to find a possible use insn.  */
      use_insn = def_insn;
      for (j = 0; j < range; j++)
	{
	  int ilen;

	  do
	    use_insn = NEXT_INSN (use_insn);
	  while (! (INSN_P (use_insn)
		    && GET_CODE (PATTERN (use_insn)) != USE
		    && GET_CODE (PATTERN (use_insn)) != CLOBBER)
		 && ! CALL_P (use_insn)
		 && use_insn != BB_END (bb));

	  /* If we hit the end of the basic block then we're done.  */
	  if (use_insn == BB_END (bb))
	    {
	      if ((! INSN_P (use_insn)
		   && ! CALL_P (use_insn))
		  || j != range - 1)
		{
		  j = range + 1;
		  break;
		}
	    }

	  /* If we have a call instruction then it will cause all hazards
	     to be masked and so there's no point scanning any further.  */
	  if (CALL_P (use_insn))
	    {
	      if (j != range - 1)
		{
		  j = range + 1;
		  break;
		}
	    }

	  /* If we have a NOP then skip forwards.  */
	  if (ubicom32_safe_attr_type (use_insn) == TYPE_NOP)
	    {
	      j += ubicom32_safe_attr_nop (use_insn) - 1;
	      continue;
	    }

	  /* If our insn is actually more than one opcode then skip forwards
	     by the number of opcodes.  */
	  ilen = get_attr_length (use_insn) / 4;
	  if (ilen > 1)
	    {
	      if (j != range - 1)
		{
		  j += ilen - 1;
		  continue;
		}
	    }
	}

      /* If we skipped forwards past where we were looking for then we're
	 done.  */
      if (j > range)
	continue;

      if (mac_pad > 0)
	{
	  if (ubicom32_multiply_dep_p (def_insn, use_insn))
	    {
	      insert_before = use_insn;
	      nop_padding = mac_pad;
	    }
	}

      if (addr_pad > 0)
	{
	  if (ubicom32_address_dep_p (def_insn, use_insn))
	    {
	      insert_before = use_insn;
	      nop_padding = addr_pad;
	    }
	}

      if (fpu_pad > 0)
	{
	  if (ubicom32_fpu_dep_p (def_insn, use_insn))
	    {
	      insert_before = use_insn;
	      nop_padding = fpu_pad;
	    }
	}

      /* If we found the need to insert a NOP then do so now.  */
      if (insert_before)
	{
	  rtx prev_insn;
	  rtx nop_rtx;

	  /* Check if the instruction before the one we're going to insert
	     is already a NOP.  If it is then we want to combine it with
	     the new one.  */
	  prev_insn = prev_real_insn (insert_before);
	  if (prev_insn != NULL_RTX)
	    {
	      if (ubicom32_safe_attr_type (prev_insn) != TYPE_NOP)
		prev_insn = NULL_RTX;
	      else
		{
		  int prev_nop = ubicom32_safe_attr_nop (prev_insn);
		  nop_padding += prev_nop;
		}
	    }

	  /* Generate the correct type of nop instruction sequence.  */
	  switch (nop_padding)
	    {
	    case 1:
	      nop_rtx = gen_nop_1 ();
	      break;

	    case 2:
	      nop_rtx = gen_nop_2 ();
	      break;

	    case 3:
	      nop_rtx = gen_nop_3 ();
	      break;

	    case 4:
	      nop_rtx = gen_nop_4 ();
	      break;

	    case 5:
	      nop_rtx = gen_nop_5 ();
	      break;

	    default:
	      gcc_unreachable();
	    }

	  /* If we had a previous NOP then delete it.  */
	  if (prev_insn)
	    SET_INSN_DELETED (prev_insn);

	  emit_insn_before (nop_rtx, insert_before);
	}
    }
}

/* Count the number of memory refs in R.  */
static int
ubicom32_get_mem_refs (rtx x)
{
  const char *fmt;
  int i, j;
  int refs = 0;
  enum rtx_code code = GET_CODE (x);

  if (code == MEM)
    return 1;

  fmt = GET_RTX_FORMAT (code);
  for (i = GET_RTX_LENGTH (code) - 1; i >= 0; i--)
    {
      if (fmt[i] == 'e'
	  && ubicom32_get_mem_refs (XEXP (x, i)))
	refs++;
      else if (fmt[i] == 'E')
	for (j = 0; j < XVECLEN (x, i); j++)
	  if (ubicom32_get_mem_refs (XVECEXP (x, i, j)))
	    refs++;
    }

  return refs;
}

static int
ubicom32_get_overcommits_1 (rtx r)
{
  int reads;
  int writes;

// XXX - need to account for more than a single word write.
  reads = ubicom32_get_mem_refs (SET_DEST (r)) ? 1 : 0;
  writes = (MEM_P (SET_SRC (r))) ? 0 : -1;
  return reads + writes;
}

/* Return the number of overcommits incurred by an instruction.  Overcommits
   are the number of writes relative to reads so if we have a read and a
   write then we return +1, if we have a read or write only we return 0 and
   if we do neither we return -1.  */

static int
ubicom32_get_overcommits (rtx insn)
{
  rtx i_rtx;

  i_rtx = PATTERN (insn);

  /* Is the instruction on which we might depend a simple set?  If yes
     then see if we have a dependency.  */
  if (GET_CODE (i_rtx) == SET)
    return ubicom32_get_overcommits_1 (i_rtx);

  /* If the instruction is a PARALLEL then check all of the parts for
     possible multiply dependencies.  */
  if (GET_CODE (i_rtx) == PARALLEL)
    {
      int i;

      for (i = 0; i < XVECLEN (i_rtx, 0); i++)
	if (GET_CODE (XVECEXP (i_rtx, 0, i)) == SET)
	  return ubicom32_get_overcommits_1 (XVECEXP (i_rtx, 0, i));
    }

  return 0;
}

static void
ubicom32_write_buffer_scan (basic_block bb)
{
  rtx insn;
  int overcommits = 0;
  FOR_BB_INSNS (bb, insn)
    {
      int o;

      /* If we have a call instruction then reset our counter.  */
      if (CALL_P (insn))
	{
	  overcommits = 0;
	  continue;
	}

      /* If this isn't an instruction then skip it.  */
      if (! (INSN_P (insn)
	     && GET_CODE (PATTERN (insn)) != USE
	     && GET_CODE (PATTERN (insn)) != CLOBBER))
	continue;

      /* Does the current instruction cause an overcommit of the memory
	 ports?  If it does then we need to see if the write buffer is
	 now going to be full.  */
      o = ubicom32_get_overcommits (insn);
      if (o >= 1)
	{
	  /* If we've hit the limit of the write buffer then insert
	     a nop padding and reduce the overcommit accordingly.  */
	  if (overcommits >= 5)
	    {
	      if (ubicom32_v5)
		{
		  /* A 4 cycle nop is really efficient as it's done with a
		     jmpt.t and thus only takes one real instruction.  It
		     allows a nice amount of time for the write buffer to
		     drain though!  */
		  emit_insn_before (gen_nop_4 (), insn);
		  o -= 4;
		}
	      else
		{
		  /* It's not obvious why the v4 parts don't benefit from
		     the 4 cycle nop the same way as v5 parts but that's
		     what the measurements show.  We do this the hard
		     way!  */
		  emit_insn_before (gen_nop_1 (), insn);
		  o--;
		}
	    }
	}

      overcommits += o;
      if (overcommits < 0)
	overcommits = 0;
    }
}

static void
ubicom32_machine_dependent_reorg (void)
{
  basic_block bb;
  int addr_ext;
  int mac_ext;
  int fpu_ext;
  int largest_ext;

  if (! ubicom32_sched_rate)
    return;

  /* Compute the scanning windows for each type of hazard.  */
  if (ubicom32_v5)
    {
      addr_ext = 5 / ubicom32_sched_rate;
      mac_ext = 2 / ubicom32_sched_rate;
      if (TARGET_HARD_FLOAT)
	fpu_ext = 5 / ubicom32_sched_rate;
      else
	fpu_ext = 0;
      largest_ext = addr_ext;
    }
  else
    {
      addr_ext = 4 / ubicom32_sched_rate;
      mac_ext = 3 / ubicom32_sched_rate;
      fpu_ext = 0;
      largest_ext = addr_ext;
    }

  FOR_EACH_BB (bb)
    {
      int i;
      for (i = 0; i < largest_ext; i++)
	ubicom32_hazard_scan(bb, i + 1, addr_ext - i , mac_ext - i,
			     fpu_ext - i);
    }

  /* Ubicom32 v3, v4 and v5 are both implemented with a write buffer that
     can overfill at a scheduling rate of 1.  Look at whether we should
     try to work-around that with nop padding.  */
  if ((ubicom32_v3 || ubicom32_v4 || ubicom32_v5)
      && ubicom32_sched_rate == 1)
    FOR_EACH_BB (bb)
      ubicom32_write_buffer_scan(bb);
}

void
ubicom32_output_cond_jump (rtx insn, rtx cond, rtx target)
{
  rtx note;
  int mostly_false_jump;
  rtx xoperands[2];
  rtx cc_reg;
  enum machine_mode cc_mode;

  note = find_reg_note (insn, REG_BR_PROB, 0);
  mostly_false_jump = !note || (INTVAL (XEXP (note, 0))
				<= REG_BR_PROB_BASE / 2);

  xoperands[0] = target;
  xoperands[1] = cond;
  cc_reg = XEXP (cond, 0);
  cc_mode = GET_MODE (cc_reg);

  if (cc_mode == CCWmode
      || cc_mode == CCWZmode
      || cc_mode == CCWZNmode)
    {
      if (mostly_false_jump)
	output_asm_insn ("jmp%b1.w.f\t%0", xoperands);
      else
	output_asm_insn ("jmp%b1.w.t\t%0", xoperands);
      return;
    }

  if (cc_mode == CCSmode
      || cc_mode == CCSZmode
      || cc_mode == CCSZNmode)
    {
      if (mostly_false_jump)
	output_asm_insn ("jmp%b1.s.f\t%0", xoperands);
      else
	output_asm_insn ("jmp%b1.s.t\t%0", xoperands);
      return;
    }

  abort ();
}

/* Return non-zero if FUNC is a naked function.  */

static int
ubicom32_naked_function_p (void)
{
  return lookup_attribute ("naked", DECL_ATTRIBUTES (current_function_decl)) != NULL_TREE;
}

/* Return an RTX indicating where the return address to the
   calling function can be found.  */
rtx
ubicom32_return_addr_rtx (int count, rtx frame ATTRIBUTE_UNUSED)
{
  if (count != 0)
    return NULL_RTX;

  return get_hard_reg_initial_val (Pmode, LINK_REGNO);
}

/* Initialise the builtin functions.  Start by initialising
   descriptions of different types of functions (e.g., void fn(int),
   int fn(void)), and then use these to define the builtins. */
static void
ubicom32_init_builtins (void)
{
  tree vptr_type_node;
  tree short_unsigned_ftype_short_unsigned;
  tree unsigned_ftype_unsigned;
  tree void_ftype_vptr_unsigned;
  tree unsigned_ftype_vptr_unsigned;
  tree ptr_ftype_void;
  tree void_ftype_ptr;
  tree decl;

  vptr_type_node
    = build_pointer_type (build_type_variant (void_type_node, 0, 1));

  short_unsigned_ftype_short_unsigned
    = build_function_type_list (short_unsigned_type_node,
				short_unsigned_type_node,
				NULL_TREE);
  unsigned_ftype_unsigned
    = build_function_type_list (unsigned_type_node,
				unsigned_type_node,
				NULL_TREE);
  ptr_ftype_void
    = build_function_type_list (ptr_type_node,
				void_list_node,
				NULL_TREE);
  void_ftype_ptr
    = build_function_type_list (void_type_node,
				ptr_type_node,
				NULL_TREE);
  void_ftype_vptr_unsigned
    = build_function_type_list (void_type_node,
				vptr_type_node,
				unsigned_type_node,
				NULL_TREE);
  unsigned_ftype_vptr_unsigned
    = build_function_type_list (unsigned_type_node,
				vptr_type_node,
				unsigned_type_node,
				NULL_TREE);

  /* Initialise the byte swap functions. */
  add_builtin_function ("__builtin_ubicom32_swapb_2",
  			short_unsigned_ftype_short_unsigned,
			UBICOM32_BUILTIN_UBICOM32_SWAPB_2,
			BUILT_IN_MD,
			NULL,
			NULL_TREE);
  add_builtin_function ("__builtin_ubicom32_swapb_4",
  			unsigned_ftype_unsigned,
			UBICOM32_BUILTIN_UBICOM32_SWAPB_4,
			BUILT_IN_MD,
			NULL,
			NULL_TREE);

  /* Initialize the thread pointer functions.  */
  decl = add_builtin_function ("__builtin_thread_pointer",
			       ptr_ftype_void,
			       UBICOM32_BUILTIN_THREAD_POINTER,
			       BUILT_IN_MD,
			       NULL,
			       NULL_TREE);
  TREE_NOTHROW (decl) = 1;

  add_builtin_function ("__builtin_set_thread_pointer",
			void_ftype_ptr,
			UBICOM32_BUILTIN_SET_THREAD_POINTER,
			BUILT_IN_MD,
			NULL,
			NULL_TREE);

  /* Initialize the spinlock functions.  */
  add_builtin_function ("__builtin_ubicom32_spinlock_lock",
			void_ftype_vptr_unsigned,
			UBICOM32_BUILTIN_SPINLOCK_LOCK,
			BUILT_IN_MD,
			NULL,
			NULL_TREE);
  add_builtin_function ("__builtin_ubicom32_spinlock_unlock",
			void_ftype_vptr_unsigned,
			UBICOM32_BUILTIN_SPINLOCK_UNLOCK,
			BUILT_IN_MD,
			NULL,
			NULL_TREE);
  add_builtin_function ("__builtin_ubicom32_spinlock_try_lock",
			unsigned_ftype_vptr_unsigned,
			UBICOM32_BUILTIN_SPINLOCK_TRY_LOCK,
			BUILT_IN_MD,
			NULL,
			NULL_TREE);
  add_builtin_function ("__builtin_ubicom32_spinlock_is_locked",
			unsigned_ftype_vptr_unsigned,
			UBICOM32_BUILTIN_SPINLOCK_IS_LOCKED,
			BUILT_IN_MD,
			NULL,
			NULL_TREE);
}

/* Given a builtin function taking one target operand and one source operand,
   emit the RTL for the underlying instruction. */
static rtx
ubicom32_expand_builtin_1t_1s (enum insn_code icode, tree exp, rtx target)
{
  tree arg0;
  rtx op0;
  rtx pat;
  enum machine_mode tmode;
  enum machine_mode mode0;

  /* Grab the incoming argument and emit its RTL. */
  arg0 = CALL_EXPR_ARG (exp, 0);
  op0 = expand_normal (arg0);

  /* Determine the modes of the instruction operands. */
  tmode = insn_data[icode].operand[0].mode;
  mode0 = insn_data[icode].operand[1].mode;

  /* If there isn't a suitable target, emit a target register. */
  if (! target
      || GET_MODE (target) != tmode
      || !(*insn_data[icode].operand[0].predicate) (target, tmode))
    target = gen_reg_rtx (tmode);

  /* Ensure that the incoming argument RTL is in a register of the
     correct mode. */
  if (!(*insn_data[icode].operand[1].predicate) (op0, mode0))
    op0 = copy_to_mode_reg (mode0, op0);

  /* Emit and return the new instruction. */
  pat = GEN_FCN (icode) (target, op0);
  if (!pat)
    return NULL_RTX;
  emit_insn (pat);

  return target;
}

/* Given a builtin function taking two source operands and one target operands,
   emit the RTL for the underlying instruction. */
static rtx
ubicom32_expand_builtin_1t_2s (enum insn_code icode, tree exp, rtx target)
{
  tree arg0;
  tree arg1;
  rtx op0;
  rtx op1;
  rtx pat;
  enum machine_mode tmode;
  enum machine_mode mode0;
  enum machine_mode mode1;

  /* Grab the incoming argument and emit its RTL. */
  arg0 = CALL_EXPR_ARG (exp, 0);
  arg1 = CALL_EXPR_ARG (exp, 1);
  op0 = expand_normal (arg0);
  op1 = expand_normal (arg1);

  /* Determine the modes of the instruction operands. */
  tmode = insn_data[icode].operand[0].mode;
  mode0 = insn_data[icode].operand[1].mode;
  mode1 = insn_data[icode].operand[2].mode;

  /* If there isn't a suitable target, emit a target register. */
  if (! target
      || GET_MODE (target) != tmode
      || !(*insn_data[icode].operand[0].predicate) (target, tmode))
    target = gen_reg_rtx (tmode);

  /* Ensure that the incoming argument RTL is in a register of the
     correct mode. */
  if (!(*insn_data[icode].operand[1].predicate) (op0, mode0))
    op0 = copy_to_mode_reg (mode0, op0);
  if (!(*insn_data[icode].operand[2].predicate) (op1, mode1))
    op1 = copy_to_mode_reg (mode1, op1);

  /* Emit and return the new instruction. */
  pat = GEN_FCN (icode) (target, op0, op1);
  if (!pat)
    return NULL_RTX;
  emit_insn (pat);

  return target;
}

/* Expand the RTL for spinlock operations that do not return any value.  */
static rtx
ubicom32_expand_builtin_spinlock_0t_2s (enum insn_code icode, tree exp)
{
  tree arg0;
  tree arg1;
  rtx op0;
  rtx op1;
  rtx pat;
  enum machine_mode mode1;
  rtx op0_reg;
  rtx seq_start;

  /* Grab the incoming argument and emit its RTL. */
  arg0 = CALL_EXPR_ARG (exp, 0);
  arg1 = CALL_EXPR_ARG (exp, 1);
  op1 = expand_normal (arg1);

  /* Determine the modes of the instruction operands. */
  mode1 = insn_data[icode].operand[1].mode;

  /* Check our lock bit position is of the correct type.  */
  if (!(*insn_data[icode].operand[1].predicate) (op1, mode1))
    return NULL_RTX;

  /* Make a place to hold the lock address.  */
  op0_reg = gen_reg_rtx (Pmode);

  seq_start = get_last_insn ();

  /* Emit and return the new instruction. */
  pat = GEN_FCN (icode) (gen_rtx_MEM (Pmode, op0_reg), op1);
  if (!pat)
    return NULL_RTX;
  emit_insn (pat);

  /* Now that we are assured of success, expand the source.  */
  start_sequence ();
  op0 = expand_expr (arg0, op0_reg, ptr_mode, EXPAND_NORMAL);
  if (op0 != op0_reg)
    emit_move_insn (op0_reg, op0);
  pat = get_insns ();
  end_sequence ();

  if (seq_start)
    emit_insn_after (pat, seq_start);
  else
    emit_insn_before (pat, get_insns ());

  return const0_rtx;
}

/* Expand the RTL for spinlock operations that do return a value.  */
static rtx
ubicom32_expand_builtin_spinlock_1t_2s (enum insn_code icode, tree exp,
					rtx target)
{
  tree arg0;
  tree arg1;
  rtx op0;
  rtx op1;
  rtx pat;
  enum machine_mode tmode;
  enum machine_mode mode1;
  rtx op0_reg;
  rtx seq_start;

  /* Grab the incoming argument and emit its RTL. */
  arg0 = CALL_EXPR_ARG (exp, 0);
  arg1 = CALL_EXPR_ARG (exp, 1);
  op1 = expand_normal (arg1);

  /* Determine the modes of the instruction operands. */
  tmode = insn_data[icode].operand[0].mode;
  mode1 = insn_data[icode].operand[2].mode;

  /* If there isn't a suitable target, emit a target register. */
  if (! target
      || GET_MODE (target) != tmode
      || !(*insn_data[icode].operand[0].predicate) (target, tmode))
    target = gen_reg_rtx (tmode);

  /* Check our lock bit position is of the correct type.  */
  if (!(*insn_data[icode].operand[2].predicate) (op1, mode1))
    return NULL_RTX;

  /* Make a place to hold the lock address.  */
  op0_reg = gen_reg_rtx (Pmode);

  seq_start = get_last_insn ();

  /* Emit and return the new instruction. */
  pat = GEN_FCN (icode) (target, gen_rtx_MEM (Pmode, op0_reg), op1);
  if (!pat)
    return NULL_RTX;
  emit_insn (pat);

  /* Now that we are assured of success, expand the source.  */
  start_sequence ();
  op0 = expand_expr (arg0, op0_reg, ptr_mode, EXPAND_NORMAL);
  if (op0 != op0_reg)
    emit_move_insn (op0_reg, op0);
  pat = get_insns ();
  end_sequence ();

  if (seq_start)
    emit_insn_after (pat, seq_start);
  else
    emit_insn_before (pat, get_insns ());

  return target;
}

/* Expand a call to a builtin function. */
static rtx
ubicom32_expand_builtin (tree exp, rtx target, rtx subtarget ATTRIBUTE_UNUSED,
			 enum machine_mode mode ATTRIBUTE_UNUSED,
			 int ignore ATTRIBUTE_UNUSED)
{
  tree fndecl = TREE_OPERAND (CALL_EXPR_FN (exp), 0);
  int fcode = DECL_FUNCTION_CODE (fndecl);
  rtx arg;

  switch (fcode)
    {
    case UBICOM32_BUILTIN_UBICOM32_SWAPB_2:
      return ubicom32_expand_builtin_1t_1s (CODE_FOR_bswaphi, exp, target);

    case UBICOM32_BUILTIN_UBICOM32_SWAPB_4:
      return ubicom32_expand_builtin_1t_1s (CODE_FOR_bswapsi, exp, target);

    case UBICOM32_BUILTIN_THREAD_POINTER:
      if (!target || !register_operand (target, Pmode))
	target = gen_reg_rtx (Pmode);
      emit_move_insn (target, gen_rtx_REG (Pmode, THREAD_REGNUM));
      return target;

    case UBICOM32_BUILTIN_SET_THREAD_POINTER:
      arg = expand_normal (CALL_EXPR_ARG (exp, 0));
      if (!register_operand (arg, Pmode))
	arg = copy_to_mode_reg (Pmode, arg);
      emit_insn (gen_movsi_set_thread_pointer (arg));
      return const0_rtx;

    case UBICOM32_BUILTIN_SPINLOCK_LOCK:
      return ubicom32_expand_builtin_spinlock_0t_2s (CODE_FOR_spinlock_lock,
						     exp);

    case UBICOM32_BUILTIN_SPINLOCK_UNLOCK:
      return ubicom32_expand_builtin_spinlock_0t_2s (CODE_FOR_spinlock_unlock,
						     exp);

    case UBICOM32_BUILTIN_SPINLOCK_TRY_LOCK:
      return ubicom32_expand_builtin_spinlock_1t_2s (CODE_FOR_spinlock_try_lock,
						     exp, target);

    case UBICOM32_BUILTIN_SPINLOCK_IS_LOCKED:
      return ubicom32_expand_builtin_spinlock_1t_2s (CODE_FOR_spinlock_is_locked,
						     exp, target);

    default:
      gcc_unreachable();
    }

  /* Should really do something sensible here.  */
  return NULL_RTX;
}

/* Fold any constant argument for a swapb.2 instruction.  */
static tree
ubicom32_fold_builtin_ubicom32_swapb_2 (tree fndecl, tree arglist)
{
  tree arg0;

  arg0 = TREE_VALUE (arglist);

  /* Optimize constant value.  */
  if (TREE_CODE (arg0) == INTEGER_CST)
    {
      HOST_WIDE_INT v;
      HOST_WIDE_INT res;

      v = TREE_INT_CST_LOW (arg0);
      res = ((v >> 8) & 0xff)
	     | ((v & 0xff) << 8);

      return build_int_cst (TREE_TYPE (TREE_TYPE (fndecl)), res);
    }

  return NULL_TREE;
}

/* Fold any constant argument for a swapb.4 instruction.  */
static tree
ubicom32_fold_builtin_ubicom32_swapb_4 (tree fndecl, tree arglist)
{
  tree arg0;

  arg0 = TREE_VALUE (arglist);

  /* Optimize constant value.  */
  if (TREE_CODE (arg0) == INTEGER_CST)
    {
      unsigned HOST_WIDE_INT v;
      unsigned HOST_WIDE_INT res;

      v = TREE_INT_CST_LOW (arg0);
      res = ((v >> 24) & 0xff)
	     | (((v >> 16) & 0xff) << 8)
	     | (((v >> 8) & 0xff) << 16)
	     | ((v & 0xff) << 24);

      return build_int_cst_wide (TREE_TYPE (TREE_TYPE (fndecl)), res, 0);
    }

  return NULL_TREE;
}

/* Fold any constant arguments for builtin functions.  */
static tree
ubicom32_fold_builtin (tree fndecl, tree arglist, bool ignore ATTRIBUTE_UNUSED)
{
  switch (DECL_FUNCTION_CODE (fndecl))
    {
    case UBICOM32_BUILTIN_UBICOM32_SWAPB_2:
      return ubicom32_fold_builtin_ubicom32_swapb_2 (fndecl, arglist);

    case UBICOM32_BUILTIN_UBICOM32_SWAPB_4:
      return ubicom32_fold_builtin_ubicom32_swapb_4 (fndecl, arglist);

    case UBICOM32_BUILTIN_THREAD_POINTER:
    case UBICOM32_BUILTIN_SET_THREAD_POINTER:
    case UBICOM32_BUILTIN_SPINLOCK_LOCK:
    case UBICOM32_BUILTIN_SPINLOCK_UNLOCK:
    case UBICOM32_BUILTIN_SPINLOCK_TRY_LOCK:
    case UBICOM32_BUILTIN_SPINLOCK_IS_LOCKED:
    default:
      return NULL;
    }
}

/* Implementation of TARGET_ASM_INTEGER.  When using FD-PIC, we need to
   tell the assembler to generate pointers to function descriptors in
   some cases.  */
static bool
ubicom32_assemble_integer (rtx value, unsigned int size, int aligned_p)
{
  if (TARGET_FDPIC && size == UNITS_PER_WORD)
    {
      if (GET_CODE (value) == SYMBOL_REF
	  && SYMBOL_REF_FUNCTION_P (value))
	{
	  fputs ("\t.picptr\t%funcdesc(", asm_out_file);
	  output_addr_const (asm_out_file, value);
	  fputs (")\n", asm_out_file);
	  return true;
	}

      if (!aligned_p)
	{
	  /* We've set the unaligned SI op to NULL, so we always have to
	     handle the unaligned case here.  */
	  assemble_integer_with_op ("\t.4byte\t", value);
	  return true;
	}
    }

  return default_assemble_integer (value, size, aligned_p);
}

/* If the constant I can be constructed by shifting a source-1 immediate
   by a constant number of bits then return the bit count.  If not
   return 0.  */

int
ubicom32_shiftable_const_int (int i)
{
  int shift = 0;

  /* Note that any constant that can be represented as an immediate to
     a movei instruction is automatically ignored here in the interests
     of the clarity of the output asm code.  */
  if (i >= -32768 && i <= 32767)
    return 0;

  /* Find the number of trailing zeroes.  We could use __builtin_ctz
     here but it's not obvious if this is supported on all build
     compilers so we err on the side of caution.  */
  if ((i & 0xffff) == 0)
    {
      shift += 16;
      i >>= 16;
    }

  if ((i & 0xff) == 0)
    {
      shift += 8;
      i >>= 8;
    }

  if ((i & 0xf) == 0)
    {
      shift += 4;
      i >>= 4;
    }

  if ((i & 0x3) == 0)
    {
      shift += 2;
      i >>= 2;
    }

  if ((i & 0x1) == 0)
    {
      shift += 1;
      i >>= 1;
    }

  if (i >= -128 && i <= 127)
    return shift;

  return 0;
}

/* Return the cost of moving data of mode M between a
   register and memory.  A value of 2 is the default; this cost is
   relative to those in `REGISTER_MOVE_COST'.  */
int
ubicom32_memory_move_cost (enum machine_mode mode,
			   enum reg_class regclass ATTRIBUTE_UNUSED,
			   int in)
{
  /* If we're reading from memory we apply a weighting to indicate that
     the first word access is about 4x the cost of a register read while
     a second (DImode or DFmode) is about half as much again.  */
  if (in)
    {
      if (mode == DImode || mode == DFmode)
	return COSTS_N_INSNS (3);

      return COSTS_N_INSNS (2);
    }

  /* For writes to memory we apply a higher weighting than for reads
     to account for the more serious impact on the cache from write buffers
     that can overflow.  This is more problematic when we're running
     multiple threads because writes from different threads can prevent
     sub-word writes from coalescing in the write buffer!  */
  if (mode == DImode || mode == DFmode)
    return COSTS_N_INSNS (5);

  return COSTS_N_INSNS (4);
}

/* Return the cost of moving data from a register in class CLASS1 to
   one in class CLASS2.

   Moves into address registers from anything other than data or
   other address registers can be expensive because they can
   trigger address hazards.  */
int
ubicom32_register_move_cost (enum machine_mode mode ATTRIBUTE_UNUSED,
			     enum reg_class class1 ATTRIBUTE_UNUSED,
			     enum reg_class class2 ATTRIBUTE_UNUSED)
{
#if 0
  /* Moves into address registers from anything other than data or
     other address registers can be expensive because they can
     trigger address hazards.  */
  if ((class2 == ADDRESS_REGS
       || class2 == SIBCALL_ADDRESS_REGS
       || class2 == LINK_REG
       || class2 == FDPIC_REG)
      && class1 != DATA_REGS
      && class1 != ADDRESS_REGS
      && class1 != SIBCALL_ADDRESS_REGS
      && class1 != LINK_REG
      && class1 != FDPIC_REG)
    return 3;
#endif

  return COSTS_N_INSNS (1);
}

/* Returns true if register REGNO is used for forming
   a memory address in expression X.  */

static bool
ubicom32_reg_used_in_mem_of_rtx_p (int regno, rtx x)
{
  enum rtx_code code = GET_CODE (x);
  int i, j;
  const char *fmt;

  if (code == SET
      && GET_CODE (SET_DEST (x)) == PC)
    {
      rtx src;

      src = SET_SRC (x);
      if (MEM_P (src))
	src = XEXP (src, 0);

      if (refers_to_regno_p (regno, regno + 1, src, 0))
	return true;
    }
  else if (code == MEM)
    {
      if (refers_to_regno_p (regno, regno + 1, XEXP (x, 0), 0))
	return true;
    }

  fmt = GET_RTX_FORMAT (code);
  for (i = GET_RTX_LENGTH (code) - 1; i >= 0; i--)
    {
      if (fmt[i] == 'e'
	  && ubicom32_reg_used_in_mem_of_rtx_p (regno, XEXP (x, i)))
	return true;

      if (fmt[i] == 'E')
	for (j = 0; j < XVECLEN (x, i); j++)
	  if (ubicom32_reg_used_in_mem_of_rtx_p (regno, XVECEXP (x, i, j)))
	    return true;
    }
  return false;
}

/* Returns true if expression DEP_RTX sets an address register
   used by instruction INSN to address memory.  */

static bool
ubicom32_addr_generation_dependency_p (rtx dep_rtx, rtx insn)
{
  rtx i_rtx;
  rtx target;
  int regno;

  if (GET_CODE (dep_rtx) != SET)
    return false;

  target = SET_DEST (dep_rtx);

  if (GET_CODE (target) == STRICT_LOW_PART)
    target = XEXP (target, 0);

  while (GET_CODE (target) == SUBREG)
    target = SUBREG_REG (target);

  if (! REG_P (target))
    return false;

  regno = REGNO (target);

  /* If our REG is CC then we don't have an address generation
     dependency.  */
  if (regno == CC_REGNUM)
    return false;

  i_rtx = PATTERN (insn);

  /* If we have an LEA-like instruction then we want to try
     and ensure the source elements are defined sufficiently
     early.  */
  if (ubicom32_safe_attr_type (insn) == TYPE_LEA)
    {
      gcc_assert (GET_CODE (i_rtx) == SET);
      return refers_to_regno_p (regno, regno + 1, SET_SRC (i_rtx), 0) ? true : false;
    }

  /* If we have an RET-like instruction then we want to try
     and ensure the source elements are defined sufficiently
     early.  Note that if we're building for FD-PIC then we
     want to ignore the "use" of the FD-PIC reg as that doesn't
     have an address generation dependency.  */
  if (ubicom32_safe_attr_type (insn) == TYPE_CALL)
    {
      if (TARGET_FDPIC && regno == FDPIC_REGNUM)
	return false;

      return refers_to_regno_p (regno, regno + 1, i_rtx, 0) ? true : false;
    }

  /* If we have a move into an address register then we also
     want to avoid any hazards that might occur in a bypass.  */
  if (GET_CODE (i_rtx) == SET
      && ubicom32_address_register_operand (SET_DEST (i_rtx), VOIDmode)
      && REG_P (SET_SRC (i_rtx))
      && REGNO (SET_SRC (i_rtx)) == regno)
    return true;

  return ubicom32_reg_used_in_mem_of_rtx_p (regno, i_rtx);
}

/* Return 1 if DEP_INSN sets register used in INSN in the agen unit.  */

int
ubicom32_address_dep_p (rtx dep_insn, rtx insn)
{
  rtx dep_rtx;

  /* If we have an LEA-like instruction as the one on which we
     potentially depend then we're safe.  There's a bypass that means
     this can't be the cause of an address hazard.  */
  if (ubicom32_safe_attr_type (dep_insn) == TYPE_LEA)
    return 0;

  dep_rtx = PATTERN (dep_insn);

  /* Is the instruction on which we potentially depend a register to
     register move into an address register?  If it is then we use an
     instruction that has a bypass and so there's no address hazard.  */
  if (GET_CODE (dep_rtx) == SET
      && ubicom32_address_register_operand (SET_DEST (dep_rtx), VOIDmode)
      && REG_P (SET_SRC (dep_rtx)))
    return 0;

  /* Is the instruction on which we might depend a simple set?  If yes
     then see if we have a dependency.  */
  if (GET_CODE (dep_rtx) == SET
      && ubicom32_addr_generation_dependency_p (dep_rtx, insn))
    return 1;

  /* If the instruction is a PARALLEL then check all of the parts for
     possible address generation dependencies.  */
  if (GET_CODE (dep_rtx) == PARALLEL)
    {
      int i;

      for (i = 0; i < XVECLEN (dep_rtx, 0); i++)
	if (ubicom32_addr_generation_dependency_p (XVECEXP (dep_rtx, 0, i), insn))
	  return 1;
    }

  return 0;
}

/* Returns true if expression DEP_RTX sets an accumulator register via an
   integer multiply that is used by instruction INSN.  */

static bool
ubicom32_multiply_dependency_p (rtx dep_rtx, rtx insn)
{
  rtx i_rtx;
  rtx target;
  int regno;
  rtx dep_src;

  if (GET_CODE (dep_rtx) != SET)
    return false;

  target = SET_DEST (dep_rtx);

  if (! REG_P (target))
    return false;

  regno = REGNO (target);
  if (regno != ACC0_HI_REGNUM
      && regno != ACC0_LO_REGNUM
      && regno != ACC1_HI_REGNUM
      && regno != ACC1_LO_REGNUM)
    return false;

  dep_src = SET_SRC (dep_rtx);
  if (GET_CODE (dep_src) != MULT)
    return false;

  if (GET_MODE (dep_src) != SImode && GET_MODE (dep_src) != DImode)
    return false;

  if (regno == ACC0_LO_REGNUM)
    regno = ACC0_HI_REGNUM;
  else if (regno == ACC1_LO_REGNUM)
    regno = ACC1_HI_REGNUM;

  i_rtx = PATTERN (insn);
  if (refers_to_regno_p (regno, regno + 2, i_rtx, 0))
    return true;

  return false;
}

/* Return 1 if DEP_INSN sets register used in INSN in a multiply.  */

static int
ubicom32_multiply_dep_p (rtx dep_insn, rtx insn)
{
  rtx dep_rtx;

  dep_rtx = PATTERN (dep_insn);

  /* Is the instruction on which we might depend a simple set?  If yes
     then see if we have a dependency.  */
  if (GET_CODE (dep_rtx) == SET
      && ubicom32_multiply_dependency_p (dep_rtx, insn))
    return 1;

  /* If the instruction is a PARALLEL then check all of the parts for
     possible multiply dependencies.  */
  if (GET_CODE (dep_rtx) == PARALLEL)
    {
      int i;

      for (i = 0; i < XVECLEN (dep_rtx, 0); i++)
	if (ubicom32_multiply_dependency_p (XVECEXP (dep_rtx, 0, i), insn))
	  return 1;
    }

  return 0;
}

/* Returns true if expression DEP_RTX sets a register via FPU operation
   that is used by instruction INSN.  */

static bool
ubicom32_fpu_dependency_p (rtx dep_rtx, rtx insn)
{
  rtx i_rtx;
  rtx target;
  int regno;
  int regsz;

  if (GET_CODE (dep_rtx) != SET)
    return false;

  target = SET_DEST (dep_rtx);

  if (! REG_P (target))
    return false;

  regno = REGNO (target);
  regsz = GET_MODE (target) == DFmode ? 2 : 1;

  i_rtx = PATTERN (insn);
  if (refers_to_regno_p (regno, regno + regsz, i_rtx, 0))
    return true;

  return false;
}

/* Return 1 if DEP_INSN sets register used in INSN in an FPU instruction
   that has a hazard window.  */

static int
ubicom32_fpu_dep_p (rtx dep_insn, rtx insn)
{
  rtx dep_rtx;

  /* If DEP_INSN isn't an FPU instruction then we don't need to worry.  */
  if (ubicom32_safe_attr_type (dep_insn) != TYPE_FPU)
    return 0;

  dep_rtx = PATTERN (dep_insn);

  /* Is the instruction on which we might depend a simple set?  If yes
     then see if we have a dependency.  */
  if (GET_CODE (dep_rtx) == SET
      && ubicom32_fpu_dependency_p (dep_rtx, insn))
    return 1;

  return 0;
}

/* Return a unique alias set for the GOT.  */

alias_set_type
ubicom32_GOT_alias_set (void)
{
  static alias_set_type set = -1;
  if (set == -1)
    set = new_alias_set ();
  return set;
}

/* Return nonzero if register FROM can be renamed to register TO.  */

bool
ubicom32_hard_regno_rename_ok (unsigned int from ATTRIBUTE_UNUSED, unsigned int to)
{
  /* We can't rename to the FD-PIC register if we're using the FD-PIC
     mode.  */
  if (TARGET_FDPIC && to == FDPIC_REGNUM)
    return false;

  /* We can't rename to the LINK register (A5) if we've not already
     saved it in the prologue.  */
  if (to == LINK_REGNO && !save_link_reg)
    return false;

  return true;
}

