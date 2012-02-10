/* tc-ubicom32.c -- Assembler for the Ubicom32
   Copyright (C) 2000, 2002 Free Software Foundation.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include <stdio.h>
#include <ctype.h>

#include "as.h"
#include "dwarf2dbg.h"
#include "subsegs.h"
#include "symcat.h"
#include "opcodes/ubicom32-desc.h"
#include "opcodes/ubicom32-opc.h"
#include "cgen.h"
#include "elf/common.h"
#include "elf/ubicom32.h"
#include "libbfd.h"

extern void gas_cgen_md_operand (expressionS *);

/* Structure to hold all of the different components describing
   an individual instruction.  */
typedef struct
{
  const CGEN_INSN *	insn;
  const CGEN_INSN *	orig_insn;
  CGEN_FIELDS		fields;
#if CGEN_INT_INSN_P
  CGEN_INSN_INT         buffer [1];
#define INSN_VALUE(buf) (*(buf))
#else
  unsigned char         buffer [CGEN_MAX_INSN_SIZE];
#define INSN_VALUE(buf) (buf)
#endif
  char *		addr;
  fragS *		frag;
  int                   num_fixups;
  fixS *                fixups [GAS_CGEN_MAX_FIXUPS];
  int                   indices [MAX_OPERAND_INSTANCES];
}
ubicom32_insn;

const char comment_chars[]        = ";";
const char line_comment_chars[]   = "#";
const char line_separator_chars[] = "";
const char EXP_CHARS[]            = "eE";
const char FLT_CHARS[]            = "dD";

/* Ubicom32 specific function to handle FD-PIC pointer initializations.  */

static void
ubicom32_pic_ptr (int nbytes)
{
  expressionS exp;
  char *p;

  if (nbytes != 4)
    abort ();

#ifdef md_flush_pending_output
  md_flush_pending_output ();
#endif

  if (is_it_end_of_statement ())
    {
      demand_empty_rest_of_line ();
      return;
    }

#ifdef md_cons_align
  md_cons_align (nbytes);
#endif

  do
    {
      bfd_reloc_code_real_type reloc_type = BFD_RELOC_UBICOM32_FUNCDESC;

      if (strncasecmp (input_line_pointer, "%funcdesc(", strlen("%funcdesc(")) == 0)
	{
	  input_line_pointer += strlen("%funcdesc(");
	  expression (&exp);
	  if (*input_line_pointer == ')')
	    input_line_pointer++;
	  else
	    as_bad (_("missing ')'"));
	}
      else
	error ("missing funcdesc in picptr");

      p = frag_more (4);
      memset (p, 0, 4);
      fix_new_exp (frag_now, p - frag_now->fr_literal, 4, &exp, 0,
		   reloc_type);
    }
  while (*input_line_pointer++ == ',');

  input_line_pointer--;			/* Put terminator back into stream. */
  demand_empty_rest_of_line ();
}

/* The target specific pseudo-ops which we support.  */
const pseudo_typeS md_pseudo_table[] =
{
    { "file",	(void (*)(int))dwarf2_directive_file,	0 },
    { "loc",	dwarf2_directive_loc,	0 },
    { "picptr", ubicom32_pic_ptr,	4 },
    { "word",	cons,			4 },
    { NULL,	NULL,			0 }
};

/* A table of the register symbols */
#if 0
static symbolS *ubicom32_register_table[40];	/* 32 data & 8 address */
#endif


#define OPTION_CPU_IP3035    (OPTION_MD_BASE)
#define OPTION_CPU_UBICOM32DSP   (OPTION_MD_BASE+1)
#define OPTION_CPU_UBICOM32VER4  (OPTION_MD_BASE+2)
#define OPTION_CPU_UBICOM32VER3FDPIC  (OPTION_MD_BASE+3)
#define OPTION_CPU_UBICOM32VER4FDPIC  (OPTION_MD_BASE+4)
#define OPTION_CPU_UBICOM32VER5  (OPTION_MD_BASE+5)
#define OPTION_CPU_UBICOM32_FDPIC  (OPTION_MD_BASE+6)

struct option md_longopts[] =
{
  { "mubicom32v1",	no_argument, NULL, OPTION_CPU_IP3035 },
  { "mubicom32v2",	no_argument, NULL, OPTION_CPU_UBICOM32DSP },
  { "mubicom32v3",	no_argument, NULL, OPTION_CPU_UBICOM32DSP },
  { "mubicom32v4",	no_argument, NULL, OPTION_CPU_UBICOM32VER4 },
  { "mubicom32v3fdpic", no_argument, NULL, OPTION_CPU_UBICOM32VER3FDPIC },
  { "mubicom32v4fdpic",	no_argument, NULL, OPTION_CPU_UBICOM32VER4FDPIC },
  { "mfdpic",	no_argument, NULL, OPTION_CPU_UBICOM32_FDPIC },
  { "mubicom32v5",	no_argument, NULL, OPTION_CPU_UBICOM32VER5 },
  { NULL,	no_argument, NULL, 0 },
};
size_t md_longopts_size = sizeof (md_longopts);

const char * md_shortopts = "";

/* Mach selected from command line.  */
int ubicom32_mach = 0;
unsigned ubicom32_mach_bitmask = 0;

int
md_parse_option (c, arg)
    int c ATTRIBUTE_UNUSED;
    char * arg ATTRIBUTE_UNUSED;
{
  int pic_state = ubicom32_mach & 0xffff0000;
  switch (c)
    {
    case OPTION_CPU_IP3035:
      ubicom32_mach = bfd_mach_ubicom32;
      ubicom32_mach_bitmask = (1 << MACH_IP3035) | (1 << MACH_UBICOM32_OLDMOVEAI);
      break;

    case OPTION_CPU_UBICOM32DSP:
      ubicom32_mach = bfd_mach_ubicom32dsp;
      ubicom32_mach_bitmask = (1 << MACH_UBICOM32DSP)| (1 << MACH_IP3023COMPATIBILITY)| (1 << MACH_UBICOM32_OLDMOVEAI);
      break;

    case OPTION_CPU_UBICOM32VER4:
      ubicom32_mach = bfd_mach_ubicom32ver4;
      ubicom32_mach_bitmask = (1 << MACH_UBICOM32DSP)| (1 << MACH_IP3023COMPATIBILITY) | (1 << MACH_UBICOM32_VER4)| (1 << MACH_UBICOM32_OLDMOVEAI);
      break;

    case OPTION_CPU_UBICOM32VER5:
      ubicom32_mach = bfd_mach_ubicom32ver5;
      ubicom32_mach_bitmask = (1 << MACH_UBICOM32DSP)| (1 << MACH_IP3023COMPATIBILITY) | (1 << MACH_UBICOM32_VER4) | (1 << MACH_UBICOM32_VER5)| (1 << MACH_UBICOM32_NEWMOVEAI);
      break;

    case OPTION_CPU_UBICOM32VER3FDPIC:
      ubicom32_mach = bfd_mach_ubicom32dsp | EF_UBICOM32_FDPIC;
      ubicom32_mach_bitmask = (1 << MACH_UBICOM32DSP)| (1 << MACH_IP3023COMPATIBILITY)| (1 << MACH_UBICOM32_OLDMOVEAI);
      break;

    case OPTION_CPU_UBICOM32VER4FDPIC:
      ubicom32_mach = bfd_mach_ubicom32ver4 | EF_UBICOM32_FDPIC;
      ubicom32_mach_bitmask = (1 << MACH_UBICOM32DSP)| (1 << MACH_IP3023COMPATIBILITY) | (1 << MACH_UBICOM32_VER4)| (1 << MACH_UBICOM32_OLDMOVEAI);
      break;

    case OPTION_CPU_UBICOM32_FDPIC:
      ubicom32_mach |= EF_UBICOM32_FDPIC;
      break;

    default:
      return 0;
    }
  ubicom32_mach |= pic_state;

  return 1;
}


void
md_show_usage (stream)
    FILE * stream;
{
  fprintf (stream, _("UBICOM32 specific command line options:\n"));
  fprintf (stream, _("  -mubicom32v1               restrict to IP3023 insns \n"));
  fprintf (stream, _("  -mubicom32v3               permit DSP extended insn\n"));
  fprintf (stream, _("  -mubicom32v4               permit DSP extended insn and additional .1 instructions.\n"));
  fprintf (stream, _("  -mfdpic                    This in addition to the v3 or v4 flags will produce a FDPIC .o.\n"));

}


void
md_begin ()
{
  /* Initialize the `cgen' interface.  */
  if(ubicom32_mach_bitmask == 0) {
	  /* md_parse_option has not been called */
	  ubicom32_mach_bitmask = 1<<MACH_IP3035;
	  ubicom32_mach = bfd_mach_ubicom32;
  }

  /* Record the specific machine in the elf header flags area */
  bfd_set_private_flags (stdoutput, ubicom32_mach);


  /* Set the machine number and endian.  */
  gas_cgen_cpu_desc = ubicom32_cgen_cpu_open (CGEN_CPU_OPEN_MACHS,
					  ubicom32_mach_bitmask,
					  CGEN_CPU_OPEN_ENDIAN,
					  CGEN_ENDIAN_BIG,
					  CGEN_CPU_OPEN_END);
  ubicom32_cgen_init_asm (gas_cgen_cpu_desc);

#if 0
  /* Construct symbols for each of the registers */

  for (i = 0; i < 32; ++i)
    {
      char name[4];
      sprintf(name, "d%d", i);
      ubicom32_register_table[i] = symbol_create(name, reg_section, i,
					      &zero_address_frag);
    }
  for (; i < 40; ++i)
    {
      char name[4];
      sprintf(name, "a%d", i-32);
      ubicom32_register_table[i] = symbol_create(name, reg_section, i,
					      &zero_address_frag);
    }
#endif

  /* This is a callback from cgen to gas to parse operands.  */
  cgen_set_parse_operand_fn (gas_cgen_cpu_desc, gas_cgen_parse_operand);

  /* Set the machine type */
  bfd_default_set_arch_mach (stdoutput, bfd_arch_ubicom32, ubicom32_mach & 0xffff);

  /* Cuz our bit fields are shifted from their values */
  flag_signed_overflow_ok = 1;
}

void
md_assemble (str)
     char * str;
{
  ubicom32_insn insn;
  char * errmsg;

  /* Initialize GAS's cgen interface for a new instruction.  */
  gas_cgen_init_parse ();
  gas_cgen_cpu_desc->signed_overflow_ok_p=1;

  /* need a way to detect when we have multiple increments to same An register */
  insn.fields.f_s1_i4_1 = 0;
  insn.fields.f_s1_i4_2 = 0;
  insn.fields.f_s1_i4_4 = 0;
  insn.fields.f_d_i4_1 = 0;
  insn.fields.f_d_i4_2 = 0;
  insn.fields.f_d_i4_4 = 0;
  insn.fields.f_s1_direct = 0;
  insn.fields.f_d_direct = 0;

  memset(&insn.fields, 0, sizeof(insn.fields));
  insn.insn = ubicom32_cgen_assemble_insn
      (gas_cgen_cpu_desc, str, & insn.fields, insn.buffer, & errmsg);

  if (!insn.insn)
    {
      as_bad ("%s", errmsg);
      return;
    }

  if (insn.fields.f_s1_An == insn.fields.f_d_An)
    {
      if ((insn.fields.f_s1_i4_1 != 0 && insn.fields.f_d_i4_1 != 0) ||
	  (insn.fields.f_s1_i4_2 != 0 && insn.fields.f_d_i4_2 != 0) ||
	  (insn.fields.f_s1_i4_4 != 0 && insn.fields.f_d_i4_4 != 0))
	{
	  /* user has tried to increment the same An register in both the s1
	     and d operands which is illegal */
	  static char errbuf[255];
	  char *first_part;
	  first_part = _("s1 and d operands update same An register");
	  if (strlen (str) > 50)
	    sprintf (errbuf, "%s `%.50s...'", first_part, str);
	  else
	    sprintf (errbuf, "%s `%.50s'", first_part, str);

	  as_bad ("%s", errbuf);
	  return;
	}
    }

  if(insn.fields.f_d_direct &&
     insn.fields.f_d_An == 0 &&
     insn.fields.f_d_imm7_4 == 0 &&
     insn.fields.f_d_imm7_2 == 0 &&
     insn.fields.f_d_imm7_1 == 0 &&
     insn.fields.f_d_i4_1 == 0 &&
     insn.fields.f_d_i4_2 == 0 &&
     insn.fields.f_d_i4_4 == 0)
    {
      if (insn.fields.f_d_direct >= A0_ADDRESS &&
	  insn.fields.f_d_direct <= A7_ADDRESS)
	{
	  long d_direct = (insn.fields.f_d_direct - A0_ADDRESS) >> 2;
	  if (d_direct == insn.fields.f_s1_An &&
	      (insn.fields.f_s1_i4_1 != 0 ||
	       insn.fields.f_s1_i4_2 != 0 ||
	       insn.fields.f_s1_i4_4 != 0))
	    {
	      /* user has tried to increment an An register that is also the destination register */
	      static char errbuf[255];
	      char *first_part;
	      first_part = _("s1 and d operands update same An register");
	      if (strlen (str) > 50)
		sprintf (errbuf, "%s `%.50s...'", first_part, str);
	      else
		sprintf (errbuf, "%s `%.50s'", first_part, str);

	      as_bad ("%s", errbuf);
	      return;
	    }
	}
    }

  /* Doesn't really matter what we pass for RELAX_P here.  */
  gas_cgen_finish_insn (insn.insn, insn.buffer,
			CGEN_FIELDS_BITSIZE (& insn.fields), 1, NULL);

}

/* The syntax in the manual says constants begin with '#'.
   We just ignore it.  */

void
md_operand (expressionP)
     expressionS * expressionP;
{
  /* In case of a syntax error, escape back to try next syntax combo. */
  if (expressionP->X_op == O_absent)
    gas_cgen_md_operand (expressionP);
}

valueT
md_section_align (segment, size)
     segT   segment;
     valueT size;
{
  int align = bfd_get_section_alignment (stdoutput, segment);
  return ((size + (1 << align) - 1) & (-1 << align));
}


/* Be sure to use our register symbols. */
symbolS *
md_undefined_symbol (char * name ATTRIBUTE_UNUSED)
{
#if 0
  char c;
  unsigned int u;

  if (sscanf(name, "%c%u", &c, &u) == 2)
    {
      if (c == 'd' && u < 32)
	return ubicom32_register_table[u];
      if (c == 'a' && u < 8)
	return ubicom32_register_table[u + 32];
    }
#endif
  return (0);
}

/* Interface to relax_segment.  */

/* Return an initial guess of the length by which a fragment must grow to
   hold a branch to reach its destination.
   Also updates fr_type/fr_subtype as necessary.

   Called just before doing relaxation.
   Any symbol that is now undefined will not become defined.
   The guess for fr_var is ACTUALLY the growth beyond fr_fix.
   Whatever we do to grow fr_fix or fr_var contributes to our returned value.
   Although it may not be explicit in the frag, pretend fr_var starts with a
   0 value.  */

int
md_estimate_size_before_relax (fragP, segment)
     fragS * fragP;
     segT    segment ATTRIBUTE_UNUSED;
{
  int    old_fr_fix = fragP->fr_fix;

  /* The only thing we have to handle here are symbols outside of the
     current segment.  They may be undefined or in a different segment in
     which case linker scripts may place them anywhere.
     However, we can't finish the fragment here and emit the reloc as insn
     alignment requirements may move the insn about.  */

  return (fragP->fr_var + fragP->fr_fix - old_fr_fix);
}

/* *fragP has been relaxed to its final size, and now needs to have
   the bytes inside it modified to conform to the new size.

   Called after relaxation is finished.
   fragP->fr_type == rs_machine_dependent.
   fragP->fr_subtype is the subtype of what the address relaxed to.  */

void
md_convert_frag (abfd, sec, fragP)
    bfd   * abfd  ATTRIBUTE_UNUSED;
    segT    sec   ATTRIBUTE_UNUSED;
    fragS * fragP ATTRIBUTE_UNUSED;
{
}


/* Functions concerning relocs.  */

long
md_pcrel_from_section (fixS *fixP ATTRIBUTE_UNUSED, segT   sec ATTRIBUTE_UNUSED)
{
  /* Leave it for the linker to figure out so relaxation can work*/
  return 0;
}

/* Return the bfd reloc type for OPERAND of INSN at fixup FIXP.
   Returns BFD_RELOC_NONE if no reloc type can be found.
   *FIXP may be modified if desired.  */

bfd_reloc_code_real_type
md_cgen_lookup_reloc (insn, operand, fixP)
     const CGEN_INSN *    insn     ATTRIBUTE_UNUSED;
     const CGEN_OPERAND * operand;
     fixS *               fixP;
{
  switch (operand->type)
    {
    case UBICOM32_OPERAND_IMM16_2:
    case UBICOM32_OPERAND_IMM24:
    case UBICOM32_OPERAND_IMM25:
    case UBICOM32_OPERAND_S1_IMM7_1:
    case UBICOM32_OPERAND_S1_IMM7_2:
    case UBICOM32_OPERAND_S1_IMM7_4:
    case UBICOM32_OPERAND_D_IMM7_1:
    case UBICOM32_OPERAND_D_IMM7_2:
    case UBICOM32_OPERAND_D_IMM7_4:
    case UBICOM32_OPERAND_OFFSET16:
    case UBICOM32_OPERAND_LEAI_OFFSET16:
     /* The relocation type should be recorded in opinfo */
      if (fixP->fx_cgen.opinfo != 0)
	return fixP->fx_cgen.opinfo;

    case UBICOM32_OPERAND_OFFSET21:
      fixP->fx_pcrel = TRUE;
      return BFD_RELOC_UBICOM32_21_PCREL;

    case UBICOM32_OPERAND_OFFSET24:
      fixP->fx_pcrel = TRUE;
      return BFD_RELOC_UBICOM32_24_PCREL;

    default:
      /* Pacify gcc -Wall. */
      return BFD_RELOC_NONE;
    }
}

/* See whether we need to force a relocation into the output file. */

int
ubicom32_force_relocation (fix)
     fixS * fix;
{
  if (fix->fx_r_type == BFD_RELOC_UNUSED)
    return 0;

  /* Force all relocations so linker relaxation can work.  */
  return 1;
}

/* Write a value out to the object file, using the appropriate endianness.  */

void
md_number_to_chars (buf, val, n)
     char * buf;
     valueT val;
     int    n;
{
  number_to_chars_bigendian (buf, val, n);
}

/* Turn a string in input_line_pointer into a floating point constant of type
   type, and store the appropriate bytes in *litP.  The number of LITTLENUMS
   emitted is stored in *sizeP .  An error message is returned, or NULL on OK.
*/

/* Equal to MAX_PRECISION in atof-ieee.c */
#define MAX_LITTLENUMS 6

char *
md_atof (int  type,
	 char * litP,
	 int *  sizeP)
{
  int              prec;
  LITTLENUM_TYPE   words [MAX_LITTLENUMS];
  LITTLENUM_TYPE  *wordP;
  char *           t;
  //char *           atof_ieee (void);

  switch (type)
    {
    case 'f':
    case 'F':
    case 's':
    case 'S':
      prec = 2;
      break;

    case 'd':
    case 'D':
    case 'r':
    case 'R':
      prec = 4;
      break;

   /* FIXME: Some targets allow other format chars for bigger sizes here.  */

    default:
      * sizeP = 0;
      return _("Bad call to md_atof()");
    }

  t = atof_ieee (input_line_pointer, type, words);
  if (t)
    input_line_pointer = t;
  * sizeP = prec * sizeof (LITTLENUM_TYPE);

  /* This loops outputs the LITTLENUMs in REVERSE order; in accord with
     the ubicom32 endianness.  */
  for (wordP = words; prec--;)
    {
      md_number_to_chars (litP, (valueT) (*wordP++), sizeof (LITTLENUM_TYPE));
      litP += sizeof (LITTLENUM_TYPE);
    }

  return 0;
}

bfd_boolean
ubicom32_fix_adjustable (fixP)
   fixS * fixP;
{
  bfd_reloc_code_real_type reloc_type;

  if ((int) fixP->fx_r_type >= (int) BFD_RELOC_UNUSED)
    {
      const CGEN_INSN *insn = NULL;
      int opindex = (int) fixP->fx_r_type - (int) BFD_RELOC_UNUSED;
      const CGEN_OPERAND *operand = cgen_operand_lookup_by_num(gas_cgen_cpu_desc, opindex);
      reloc_type = md_cgen_lookup_reloc (insn, operand, fixP);
    }
  else
    reloc_type = fixP->fx_r_type;

  if (fixP->fx_addsy == NULL)
    return 1;

  if (!S_IS_LOCAL (fixP->fx_addsy))
    /* Let the linker resolve all symbols not within the local function
       so the linker can relax correctly.  */
    return 0;

  if (S_IS_WEAK (fixP->fx_addsy))
    return 0;

  /* We need the symbol name for the VTABLE entries */
  if (   reloc_type == BFD_RELOC_VTABLE_INHERIT
      || reloc_type == BFD_RELOC_VTABLE_ENTRY)
    return 0;

  return 1;
}
