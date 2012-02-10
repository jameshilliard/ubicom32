/* Instruction building/extraction support for ubicom32. -*- C -*-

   THIS FILE IS MACHINE GENERATED WITH CGEN: Cpu tools GENerator.
   - the resultant file is machine generated, cgen-ibld.in isn't

   Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2005, 2006, 2007
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
#include "dis-asm.h"
#include "bfd.h"
#include "symcat.h"
#include "ubicom32-desc.h"
#include "ubicom32-opc.h"
#include "opintl.h"
#include "safe-ctype.h"

#undef  min
#define min(a,b) ((a) < (b) ? (a) : (b))
#undef  max
#define max(a,b) ((a) > (b) ? (a) : (b))

/* Used by the ifield rtx function.  */
#define FLD(f) (fields->f)

static const char * insert_normal
  (CGEN_CPU_DESC, long, unsigned int, unsigned int, unsigned int,
   unsigned int, unsigned int, unsigned int, CGEN_INSN_BYTES_PTR);
static const char * insert_insn_normal
  (CGEN_CPU_DESC, const CGEN_INSN *,
   CGEN_FIELDS *, CGEN_INSN_BYTES_PTR, bfd_vma);
static int extract_normal
  (CGEN_CPU_DESC, CGEN_EXTRACT_INFO *, CGEN_INSN_INT,
   unsigned int, unsigned int, unsigned int, unsigned int,
   unsigned int, unsigned int, bfd_vma, long *);
static int extract_insn_normal
  (CGEN_CPU_DESC, const CGEN_INSN *, CGEN_EXTRACT_INFO *,
   CGEN_INSN_INT, CGEN_FIELDS *, bfd_vma);
#if CGEN_INT_INSN_P
static void put_insn_int_value
  (CGEN_CPU_DESC, CGEN_INSN_BYTES_PTR, int, int, CGEN_INSN_INT);
#endif
#if ! CGEN_INT_INSN_P
static CGEN_INLINE void insert_1
  (CGEN_CPU_DESC, unsigned long, int, int, int, unsigned char *);
static CGEN_INLINE int fill_cache
  (CGEN_CPU_DESC, CGEN_EXTRACT_INFO *,  int, int, bfd_vma);
static CGEN_INLINE long extract_1
  (CGEN_CPU_DESC, CGEN_EXTRACT_INFO *, int, int, int, unsigned char *, bfd_vma);
#endif

/* Operand insertion.  */

#if ! CGEN_INT_INSN_P

/* Subroutine of insert_normal.  */

static CGEN_INLINE void
insert_1 (CGEN_CPU_DESC cd,
	  unsigned long value,
	  int start,
	  int length,
	  int word_length,
	  unsigned char *bufp)
{
  unsigned long x,mask;
  int shift;

  x = cgen_get_insn_value (cd, bufp, word_length);

  /* Written this way to avoid undefined behaviour.  */
  mask = (((1L << (length - 1)) - 1) << 1) | 1;
  if (CGEN_INSN_LSB0_P)
    shift = (start + 1) - length;
  else
    shift = (word_length - (start + length));
  x = (x & ~(mask << shift)) | ((value & mask) << shift);

  cgen_put_insn_value (cd, bufp, word_length, (bfd_vma) x);
}

#endif /* ! CGEN_INT_INSN_P */

/* Default insertion routine.

   ATTRS is a mask of the boolean attributes.
   WORD_OFFSET is the offset in bits from the start of the insn of the value.
   WORD_LENGTH is the length of the word in bits in which the value resides.
   START is the starting bit number in the word, architecture origin.
   LENGTH is the length of VALUE in bits.
   TOTAL_LENGTH is the total length of the insn in bits.

   The result is an error message or NULL if success.  */

/* ??? This duplicates functionality with bfd's howto table and
   bfd_install_relocation.  */
/* ??? This doesn't handle bfd_vma's.  Create another function when
   necessary.  */

static const char *
insert_normal (CGEN_CPU_DESC cd,
	       long value,
	       unsigned int attrs,
	       unsigned int word_offset,
	       unsigned int start,
	       unsigned int length,
	       unsigned int word_length,
	       unsigned int total_length,
	       CGEN_INSN_BYTES_PTR buffer)
{
  static char errbuf[100];
  /* Written this way to avoid undefined behaviour.  */
  unsigned long mask = (((1L << (length - 1)) - 1) << 1) | 1;

  /* If LENGTH is zero, this operand doesn't contribute to the value.  */
  if (length == 0)
    return NULL;

  if (word_length > 32)
    abort ();

  /* For architectures with insns smaller than the base-insn-bitsize,
     word_length may be too big.  */
  if (cd->min_insn_bitsize < cd->base_insn_bitsize)
    {
      if (word_offset == 0
	  && word_length > total_length)
	word_length = total_length;
    }

  /* Ensure VALUE will fit.  */
  if (CGEN_BOOL_ATTR (attrs, CGEN_IFLD_SIGN_OPT))
    {
      long minval = - (1L << (length - 1));
      unsigned long maxval = mask;
      
      if ((value > 0 && (unsigned long) value > maxval)
	  || value < minval)
	{
	  /* xgettext:c-format */
	  sprintf (errbuf,
		   _("operand out of range (%ld not between %ld and %lu)"),
		   value, minval, maxval);
	  return errbuf;
	}
    }
  else if (! CGEN_BOOL_ATTR (attrs, CGEN_IFLD_SIGNED))
    {
      unsigned long maxval = mask;
      unsigned long val = (unsigned long) value;

      /* For hosts with a word size > 32 check to see if value has been sign
	 extended beyond 32 bits.  If so then ignore these higher sign bits
	 as the user is attempting to store a 32-bit signed value into an
	 unsigned 32-bit field which is allowed.  */
      if (sizeof (unsigned long) > 4 && ((value >> 32) == -1))
	val &= 0xFFFFFFFF;

      if (val > maxval)
	{
	  /* xgettext:c-format */
	  sprintf (errbuf,
		   _("operand out of range (0x%lx not between 0 and 0x%lx)"),
		   val, maxval);
	  return errbuf;
	}
    }
  else
    {
      if (! cgen_signed_overflow_ok_p (cd))
	{
	  long minval = - (1L << (length - 1));
	  long maxval =   (1L << (length - 1)) - 1;
	  
	  if (value < minval || value > maxval)
	    {
	      sprintf
		/* xgettext:c-format */
		(errbuf, _("operand out of range (%ld not between %ld and %ld)"),
		 value, minval, maxval);
	      return errbuf;
	    }
	}
    }

#if CGEN_INT_INSN_P

  {
    int shift;

    if (CGEN_INSN_LSB0_P)
      shift = (word_offset + start + 1) - length;
    else
      shift = total_length - (word_offset + start + length);
    *buffer = (*buffer & ~(mask << shift)) | ((value & mask) << shift);
  }

#else /* ! CGEN_INT_INSN_P */

  {
    unsigned char *bufp = (unsigned char *) buffer + word_offset / 8;

    insert_1 (cd, value, start, length, word_length, bufp);
  }

#endif /* ! CGEN_INT_INSN_P */

  return NULL;
}

/* Default insn builder (insert handler).
   The instruction is recorded in CGEN_INT_INSN_P byte order (meaning
   that if CGEN_INSN_BYTES_PTR is an int * and thus, the value is
   recorded in host byte order, otherwise BUFFER is an array of bytes
   and the value is recorded in target byte order).
   The result is an error message or NULL if success.  */

static const char *
insert_insn_normal (CGEN_CPU_DESC cd,
		    const CGEN_INSN * insn,
		    CGEN_FIELDS * fields,
		    CGEN_INSN_BYTES_PTR buffer,
		    bfd_vma pc)
{
  const CGEN_SYNTAX *syntax = CGEN_INSN_SYNTAX (insn);
  unsigned long value;
  const CGEN_SYNTAX_CHAR_TYPE * syn;

  CGEN_INIT_INSERT (cd);
  value = CGEN_INSN_BASE_VALUE (insn);

  /* If we're recording insns as numbers (rather than a string of bytes),
     target byte order handling is deferred until later.  */

#if CGEN_INT_INSN_P

  put_insn_int_value (cd, buffer, cd->base_insn_bitsize,
		      CGEN_FIELDS_BITSIZE (fields), value);

#else

  cgen_put_insn_value (cd, buffer, min ((unsigned) cd->base_insn_bitsize,
					(unsigned) CGEN_FIELDS_BITSIZE (fields)),
		       value);

#endif /* ! CGEN_INT_INSN_P */

  /* ??? It would be better to scan the format's fields.
     Still need to be able to insert a value based on the operand though;
     e.g. storing a branch displacement that got resolved later.
     Needs more thought first.  */

  for (syn = CGEN_SYNTAX_STRING (syntax); * syn; ++ syn)
    {
      const char *errmsg;

      if (CGEN_SYNTAX_CHAR_P (* syn))
	continue;

      errmsg = (* cd->insert_operand) (cd, CGEN_SYNTAX_FIELD (*syn),
				       fields, buffer, pc);
      if (errmsg)
	return errmsg;
    }

  return NULL;
}

#if CGEN_INT_INSN_P
/* Cover function to store an insn value into an integral insn.  Must go here
   because it needs <prefix>-desc.h for CGEN_INT_INSN_P.  */

static void
put_insn_int_value (CGEN_CPU_DESC cd ATTRIBUTE_UNUSED,
		    CGEN_INSN_BYTES_PTR buf,
		    int length,
		    int insn_length,
		    CGEN_INSN_INT value)
{
  /* For architectures with insns smaller than the base-insn-bitsize,
     length may be too big.  */
  if (length > insn_length)
    *buf = value;
  else
    {
      int shift = insn_length - length;
      /* Written this way to avoid undefined behaviour.  */
      CGEN_INSN_INT mask = (((1L << (length - 1)) - 1) << 1) | 1;

      *buf = (*buf & ~(mask << shift)) | ((value & mask) << shift);
    }
}
#endif

/* Operand extraction.  */

#if ! CGEN_INT_INSN_P

/* Subroutine of extract_normal.
   Ensure sufficient bytes are cached in EX_INFO.
   OFFSET is the offset in bytes from the start of the insn of the value.
   BYTES is the length of the needed value.
   Returns 1 for success, 0 for failure.  */

static CGEN_INLINE int
fill_cache (CGEN_CPU_DESC cd ATTRIBUTE_UNUSED,
	    CGEN_EXTRACT_INFO *ex_info,
	    int offset,
	    int bytes,
	    bfd_vma pc)
{
  /* It's doubtful that the middle part has already been fetched so
     we don't optimize that case.  kiss.  */
  unsigned int mask;
  disassemble_info *info = (disassemble_info *) ex_info->dis_info;

  /* First do a quick check.  */
  mask = (1 << bytes) - 1;
  if (((ex_info->valid >> offset) & mask) == mask)
    return 1;

  /* Search for the first byte we need to read.  */
  for (mask = 1 << offset; bytes > 0; --bytes, ++offset, mask <<= 1)
    if (! (mask & ex_info->valid))
      break;

  if (bytes)
    {
      int status;

      pc += offset;
      status = (*info->read_memory_func)
	(pc, ex_info->insn_bytes + offset, bytes, info);

      if (status != 0)
	{
	  (*info->memory_error_func) (status, pc, info);
	  return 0;
	}

      ex_info->valid |= ((1 << bytes) - 1) << offset;
    }

  return 1;
}

/* Subroutine of extract_normal.  */

static CGEN_INLINE long
extract_1 (CGEN_CPU_DESC cd,
	   CGEN_EXTRACT_INFO *ex_info ATTRIBUTE_UNUSED,
	   int start,
	   int length,
	   int word_length,
	   unsigned char *bufp,
	   bfd_vma pc ATTRIBUTE_UNUSED)
{
  unsigned long x;
  int shift;

  x = cgen_get_insn_value (cd, bufp, word_length);

  if (CGEN_INSN_LSB0_P)
    shift = (start + 1) - length;
  else
    shift = (word_length - (start + length));
  return x >> shift;
}

#endif /* ! CGEN_INT_INSN_P */

/* Default extraction routine.

   INSN_VALUE is the first base_insn_bitsize bits of the insn in host order,
   or sometimes less for cases like the m32r where the base insn size is 32
   but some insns are 16 bits.
   ATTRS is a mask of the boolean attributes.  We only need `SIGNED',
   but for generality we take a bitmask of all of them.
   WORD_OFFSET is the offset in bits from the start of the insn of the value.
   WORD_LENGTH is the length of the word in bits in which the value resides.
   START is the starting bit number in the word, architecture origin.
   LENGTH is the length of VALUE in bits.
   TOTAL_LENGTH is the total length of the insn in bits.

   Returns 1 for success, 0 for failure.  */

/* ??? The return code isn't properly used.  wip.  */

/* ??? This doesn't handle bfd_vma's.  Create another function when
   necessary.  */

static int
extract_normal (CGEN_CPU_DESC cd,
#if ! CGEN_INT_INSN_P
		CGEN_EXTRACT_INFO *ex_info,
#else
		CGEN_EXTRACT_INFO *ex_info ATTRIBUTE_UNUSED,
#endif
		CGEN_INSN_INT insn_value,
		unsigned int attrs,
		unsigned int word_offset,
		unsigned int start,
		unsigned int length,
		unsigned int word_length,
		unsigned int total_length,
#if ! CGEN_INT_INSN_P
		bfd_vma pc,
#else
		bfd_vma pc ATTRIBUTE_UNUSED,
#endif
		long *valuep)
{
  long value, mask;

  /* If LENGTH is zero, this operand doesn't contribute to the value
     so give it a standard value of zero.  */
  if (length == 0)
    {
      *valuep = 0;
      return 1;
    }

  if (word_length > 32)
    abort ();

  /* For architectures with insns smaller than the insn-base-bitsize,
     word_length may be too big.  */
  if (cd->min_insn_bitsize < cd->base_insn_bitsize)
    {
      if (word_offset + word_length > total_length)
	word_length = total_length - word_offset;
    }

  /* Does the value reside in INSN_VALUE, and at the right alignment?  */

  if (CGEN_INT_INSN_P || (word_offset == 0 && word_length == total_length))
    {
      if (CGEN_INSN_LSB0_P)
	value = insn_value >> ((word_offset + start + 1) - length);
      else
	value = insn_value >> (total_length - ( word_offset + start + length));
    }

#if ! CGEN_INT_INSN_P

  else
    {
      unsigned char *bufp = ex_info->insn_bytes + word_offset / 8;

      if (word_length > 32)
	abort ();

      if (fill_cache (cd, ex_info, word_offset / 8, word_length / 8, pc) == 0)
	return 0;

      value = extract_1 (cd, ex_info, start, length, word_length, bufp, pc);
    }

#endif /* ! CGEN_INT_INSN_P */

  /* Written this way to avoid undefined behaviour.  */
  mask = (((1L << (length - 1)) - 1) << 1) | 1;

  value &= mask;
  /* sign extend? */
  if (CGEN_BOOL_ATTR (attrs, CGEN_IFLD_SIGNED)
      && (value & (1L << (length - 1))))
    value |= ~mask;

  *valuep = value;

  return 1;
}

/* Default insn extractor.

   INSN_VALUE is the first base_insn_bitsize bits, translated to host order.
   The extracted fields are stored in FIELDS.
   EX_INFO is used to handle reading variable length insns.
   Return the length of the insn in bits, or 0 if no match,
   or -1 if an error occurs fetching data (memory_error_func will have
   been called).  */

static int
extract_insn_normal (CGEN_CPU_DESC cd,
		     const CGEN_INSN *insn,
		     CGEN_EXTRACT_INFO *ex_info,
		     CGEN_INSN_INT insn_value,
		     CGEN_FIELDS *fields,
		     bfd_vma pc)
{
  const CGEN_SYNTAX *syntax = CGEN_INSN_SYNTAX (insn);
  const CGEN_SYNTAX_CHAR_TYPE *syn;

  CGEN_FIELDS_BITSIZE (fields) = CGEN_INSN_BITSIZE (insn);

  CGEN_INIT_EXTRACT (cd);

  for (syn = CGEN_SYNTAX_STRING (syntax); *syn; ++syn)
    {
      int length;

      if (CGEN_SYNTAX_CHAR_P (*syn))
	continue;

      length = (* cd->extract_operand) (cd, CGEN_SYNTAX_FIELD (*syn),
					ex_info, insn_value, fields, pc);
      if (length <= 0)
	return length;
    }

  /* We recognized and successfully extracted this insn.  */
  return CGEN_INSN_BITSIZE (insn);
}

/* Machine generated code added here.  */

const char * ubicom32_cgen_insert_operand
  (CGEN_CPU_DESC, int, CGEN_FIELDS *, CGEN_INSN_BYTES_PTR, bfd_vma);

/* Main entry point for operand insertion.

   This function is basically just a big switch statement.  Earlier versions
   used tables to look up the function to use, but
   - if the table contains both assembler and disassembler functions then
     the disassembler contains much of the assembler and vice-versa,
   - there's a lot of inlining possibilities as things grow,
   - using a switch statement avoids the function call overhead.

   This function could be moved into `parse_insn_normal', but keeping it
   separate makes clear the interface between `parse_insn_normal' and each of
   the handlers.  It's also needed by GAS to insert operands that couldn't be
   resolved during parsing.  */

const char *
ubicom32_cgen_insert_operand (CGEN_CPU_DESC cd,
			     int opindex,
			     CGEN_FIELDS * fields,
			     CGEN_INSN_BYTES_PTR buffer,
			     bfd_vma pc ATTRIBUTE_UNUSED)
{
  const char * errmsg = NULL;
  unsigned int total_length = CGEN_FIELDS_BITSIZE (fields);

  switch (opindex)
    {
    case UBICOM32_OPERAND_AM :
      errmsg = insert_normal (cd, fields->f_Am, 0, 0, 7, 3, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_AN :
      errmsg = insert_normal (cd, fields->f_An, 0, 0, 23, 3, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_C :
      errmsg = insert_normal (cd, fields->f_C, 0, 0, 21, 1, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_DN :
      errmsg = insert_normal (cd, fields->f_Dn, 0, 0, 20, 5, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_P :
      errmsg = insert_normal (cd, fields->f_P, 0, 0, 22, 1, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_ACC1HI :
      break;
    case UBICOM32_OPERAND_ACC1LO :
      break;
    case UBICOM32_OPERAND_BIT5 :
      errmsg = insert_normal (cd, fields->f_bit5, 0, 0, 15, 5, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_BIT5_ADDSUB :
      errmsg = insert_normal (cd, fields->f_bit5, 0, 0, 15, 5, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_CC :
      errmsg = insert_normal (cd, fields->f_cond, 0, 0, 26, 4, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_D_AN :
      errmsg = insert_normal (cd, fields->f_d_An, 0, 0, 23, 3, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_D_DIRECT_ADDR :
      {
        long value = fields->f_d_direct;
        value = ((unsigned int) (value) >> (2));
        errmsg = insert_normal (cd, value, 0, 0, 23, 8, 32, total_length, buffer);
      }
      break;
    case UBICOM32_OPERAND_D_I4_1 :
      errmsg = insert_normal (cd, fields->f_d_i4_1, 0|(1<<CGEN_IFLD_SIGNED), 0, 19, 4, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_D_I4_2 :
      {
        long value = fields->f_d_i4_2;
        value = ((unsigned int) (value) >> (1));
        errmsg = insert_normal (cd, value, 0|(1<<CGEN_IFLD_SIGNED), 0, 19, 4, 32, total_length, buffer);
      }
      break;
    case UBICOM32_OPERAND_D_I4_4 :
      {
        long value = fields->f_d_i4_4;
        value = ((unsigned int) (value) >> (2));
        errmsg = insert_normal (cd, value, 0|(1<<CGEN_IFLD_SIGNED), 0, 19, 4, 32, total_length, buffer);
      }
      break;
    case UBICOM32_OPERAND_D_IMM7_1 :
      {
{
  FLD (f_d_imm7_t) = ((((unsigned int) (FLD (f_d_imm7_1)) >> (5))) & (3));
  FLD (f_d_imm7_b) = ((((unsigned int) (FLD (f_d_imm7_1)) >> (0))) & (31));
}
        errmsg = insert_normal (cd, fields->f_d_imm7_t, 0, 0, 25, 2, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_d_imm7_b, 0, 0, 20, 5, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case UBICOM32_OPERAND_D_IMM7_2 :
      {
{
  FLD (f_d_imm7_t) = ((((unsigned int) (FLD (f_d_imm7_2)) >> (6))) & (3));
  FLD (f_d_imm7_b) = ((((unsigned int) (FLD (f_d_imm7_2)) >> (1))) & (31));
}
        errmsg = insert_normal (cd, fields->f_d_imm7_t, 0, 0, 25, 2, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_d_imm7_b, 0, 0, 20, 5, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case UBICOM32_OPERAND_D_IMM7_4 :
      {
{
  FLD (f_d_imm7_t) = ((((unsigned int) (FLD (f_d_imm7_4)) >> (7))) & (3));
  FLD (f_d_imm7_b) = ((((unsigned int) (FLD (f_d_imm7_4)) >> (2))) & (31));
}
        errmsg = insert_normal (cd, fields->f_d_imm7_t, 0, 0, 25, 2, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_d_imm7_b, 0, 0, 20, 5, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case UBICOM32_OPERAND_D_IMM8 :
      errmsg = insert_normal (cd, fields->f_d_imm8, 0|(1<<CGEN_IFLD_SIGNED), 0, 23, 8, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_D_R :
      errmsg = insert_normal (cd, fields->f_d_r, 0, 0, 20, 5, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_DSP_S2_ACC_REG_ADDSUB :
      errmsg = insert_normal (cd, fields->f_dsp_S2, 0, 0, 14, 4, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_DSP_S2_ACC_REG_MUL :
      errmsg = insert_normal (cd, fields->f_dsp_S2, 0, 0, 14, 4, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_DSP_S2_DATA_REG :
      errmsg = insert_normal (cd, fields->f_dsp_S2, 0, 0, 14, 4, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_DSP_S2_DATA_REG_ADDSUB :
      errmsg = insert_normal (cd, fields->f_dsp_S2, 0, 0, 14, 4, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_DSP_S2_SEL :
      errmsg = insert_normal (cd, fields->f_dsp_S2_sel, 0, 0, 18, 1, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_DSP_C :
      errmsg = insert_normal (cd, fields->f_dsp_C, 0, 0, 20, 1, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_DSP_DESTA :
      errmsg = insert_normal (cd, fields->f_dsp_destA, 0, 0, 16, 1, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_DSP_T :
      errmsg = insert_normal (cd, fields->f_dsp_T, 0, 0, 19, 1, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_DSP_T_ADDSUB :
      errmsg = insert_normal (cd, fields->f_dsp_T, 0, 0, 19, 1, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_FPU_32_S2_ACC_REG :
      errmsg = insert_normal (cd, fields->f_FPS2_reg32, 0, 0, 14, 4, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_FPU_32_S2_DATA_REG :
      errmsg = insert_normal (cd, fields->f_FPS2_reg32, 0, 0, 14, 4, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_FPU_32_DEST_REG :
      errmsg = insert_normal (cd, fields->f_FPD32, 0, 0, 17, 2, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_FPU_64_S1_ACC_REG :
      errmsg = insert_normal (cd, fields->f_FPS1_reg64, 0, 0, 3, 3, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_FPU_64_S1_DATA_REG :
      errmsg = insert_normal (cd, fields->f_FPS1_reg64, 0, 0, 3, 3, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_FPU_64_S2_ACC_REG :
      errmsg = insert_normal (cd, fields->f_FPS2_reg64, 0, 0, 14, 3, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_FPU_64_S2_DATA_REG :
      errmsg = insert_normal (cd, fields->f_FPS2_reg64, 0, 0, 14, 3, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_FPU_64_DEST_REG :
      errmsg = insert_normal (cd, fields->f_FPD64, 0, 0, 17, 1, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_IMM16_1 :
      errmsg = insert_normal (cd, fields->f_imm16_1, 0|(1<<CGEN_IFLD_SIGNED), 0, 26, 16, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_IMM16_2 :
      errmsg = insert_normal (cd, fields->f_imm16_2, 0|(1<<CGEN_IFLD_SIGNED), 0, 15, 16, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_IMM24 :
      {
{
  FLD (f_imm23_21) = ((((unsigned int) (FLD (f_imm24)) >> (21))) & (7));
  FLD (f_o20_0) = ((FLD (f_imm24)) & (2097151));
}
        errmsg = insert_normal (cd, fields->f_imm23_21, 0, 0, 26, 3, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_o20_0, 0, 0, 20, 21, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case UBICOM32_OPERAND_IMM25 :
      {
{
  FLD (f_imm24_21) = ((((unsigned int) (FLD (f_imm25)) >> (21))) & (15));
  FLD (f_o20_0) = ((FLD (f_imm25)) & (2097151));
}
        errmsg = insert_normal (cd, fields->f_imm24_21, 0, 0, 27, 4, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_o20_0, 0, 0, 20, 21, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case UBICOM32_OPERAND_INTERRUPT :
      errmsg = insert_normal (cd, fields->f_int, 0, 0, 5, 6, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_IREAD :
      break;
    case UBICOM32_OPERAND_IRQ_0 :
      break;
    case UBICOM32_OPERAND_IRQ_1 :
      break;
    case UBICOM32_OPERAND_LEAI_OFFSET16 :
      {
        fields->f_leai_o16 = ((int) (fields->f_leai_o16) >> (2));
{
  FLD (f_o15_13) = ((((unsigned int) (FLD (f_leai_o16)) >> (13))) & (7));
  FLD (f_o12_8) = ((((unsigned int) (FLD (f_leai_o16)) >> (8))) & (31));
  FLD (f_o7_5) = ((((unsigned int) (FLD (f_leai_o16)) >> (5))) & (7));
  FLD (f_o4_0) = ((FLD (f_leai_o16)) & (31));
}
        errmsg = insert_normal (cd, fields->f_o15_13, 0|(1<<CGEN_IFLD_SIGNED), 0, 26, 3, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_o12_8, 0, 0, 20, 5, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_o7_5, 0, 0, 10, 3, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_o4_0, 0, 0, 4, 5, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case UBICOM32_OPERAND_MACHI :
      break;
    case UBICOM32_OPERAND_MACLO :
      break;
    case UBICOM32_OPERAND_OFFSET16 :
      {
        fields->f_o16 = ((int) (fields->f_o16) >> (2));
{
  FLD (f_o15_13) = ((((unsigned int) (FLD (f_o16)) >> (13))) & (7));
  FLD (f_o12_8) = ((((unsigned int) (FLD (f_o16)) >> (8))) & (31));
  FLD (f_o7_5) = ((((unsigned int) (FLD (f_o16)) >> (5))) & (7));
  FLD (f_o4_0) = ((FLD (f_o16)) & (31));
}
        errmsg = insert_normal (cd, fields->f_o15_13, 0|(1<<CGEN_IFLD_SIGNED), 0, 26, 3, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_o12_8, 0, 0, 20, 5, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_o7_5, 0, 0, 10, 3, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_o4_0, 0, 0, 4, 5, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case UBICOM32_OPERAND_OFFSET21 :
      {
        long value = fields->f_o21;
        value = ((unsigned int) (((value) - (pc))) >> (2));
        errmsg = insert_normal (cd, value, 0|(1<<CGEN_IFLD_SIGNED)|(1<<CGEN_IFLD_PCREL_ADDR), 0, 20, 21, 32, total_length, buffer);
      }
      break;
    case UBICOM32_OPERAND_OFFSET24 :
      {
        fields->f_o24 = ((int) (((fields->f_o24) - (pc))) >> (2));
{
  FLD (f_o23_21) = ((((unsigned int) (FLD (f_o24)) >> (21))) & (7));
  FLD (f_o20_0) = ((FLD (f_o24)) & (2097151));
}
        errmsg = insert_normal (cd, fields->f_o23_21, 0|(1<<CGEN_IFLD_SIGNED), 0, 26, 3, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_o20_0, 0, 0, 20, 21, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case UBICOM32_OPERAND_OPC1 :
      errmsg = insert_normal (cd, fields->f_op1, 0, 0, 31, 5, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_OPC2 :
      errmsg = insert_normal (cd, fields->f_op2, 0, 0, 15, 5, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_PDEC_S1_IMM7_4 :
      {
{
  FLD (f_s1_imm7_t) = ((((unsigned int) (FLD (f_s1_imm7_4)) >> (7))) & (3));
  FLD (f_s1_imm7_b) = ((((unsigned int) (FLD (f_s1_imm7_4)) >> (2))) & (31));
}
        errmsg = insert_normal (cd, fields->f_s1_imm7_t, 0, 0, 9, 2, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_s1_imm7_b, 0, 0, 4, 5, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case UBICOM32_OPERAND_S1_AN :
      errmsg = insert_normal (cd, fields->f_s1_An, 0, 0, 7, 3, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_S1_DIRECT_ADDR :
      {
        long value = fields->f_s1_direct;
        value = ((unsigned int) (value) >> (2));
        errmsg = insert_normal (cd, value, 0, 0, 7, 8, 32, total_length, buffer);
      }
      break;
    case UBICOM32_OPERAND_S1_I4_1 :
      errmsg = insert_normal (cd, fields->f_s1_i4_1, 0|(1<<CGEN_IFLD_SIGNED), 0, 3, 4, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_S1_I4_2 :
      {
        long value = fields->f_s1_i4_2;
        value = ((unsigned int) (value) >> (1));
        errmsg = insert_normal (cd, value, 0|(1<<CGEN_IFLD_SIGNED), 0, 3, 4, 32, total_length, buffer);
      }
      break;
    case UBICOM32_OPERAND_S1_I4_4 :
      {
        long value = fields->f_s1_i4_4;
        value = ((unsigned int) (value) >> (2));
        errmsg = insert_normal (cd, value, 0|(1<<CGEN_IFLD_SIGNED), 0, 3, 4, 32, total_length, buffer);
      }
      break;
    case UBICOM32_OPERAND_S1_IMM7_1 :
      {
{
  FLD (f_s1_imm7_t) = ((((unsigned int) (FLD (f_s1_imm7_1)) >> (5))) & (3));
  FLD (f_s1_imm7_b) = ((((unsigned int) (FLD (f_s1_imm7_1)) >> (0))) & (31));
}
        errmsg = insert_normal (cd, fields->f_s1_imm7_t, 0, 0, 9, 2, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_s1_imm7_b, 0, 0, 4, 5, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case UBICOM32_OPERAND_S1_IMM7_2 :
      {
{
  FLD (f_s1_imm7_t) = ((((unsigned int) (FLD (f_s1_imm7_2)) >> (6))) & (3));
  FLD (f_s1_imm7_b) = ((((unsigned int) (FLD (f_s1_imm7_2)) >> (1))) & (31));
}
        errmsg = insert_normal (cd, fields->f_s1_imm7_t, 0, 0, 9, 2, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_s1_imm7_b, 0, 0, 4, 5, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case UBICOM32_OPERAND_S1_IMM7_4 :
      {
{
  FLD (f_s1_imm7_t) = ((((unsigned int) (FLD (f_s1_imm7_4)) >> (7))) & (3));
  FLD (f_s1_imm7_b) = ((((unsigned int) (FLD (f_s1_imm7_4)) >> (2))) & (31));
}
        errmsg = insert_normal (cd, fields->f_s1_imm7_t, 0, 0, 9, 2, 32, total_length, buffer);
        if (errmsg)
          break;
        errmsg = insert_normal (cd, fields->f_s1_imm7_b, 0, 0, 4, 5, 32, total_length, buffer);
        if (errmsg)
          break;
      }
      break;
    case UBICOM32_OPERAND_S1_IMM8 :
      errmsg = insert_normal (cd, fields->f_s1_imm8, 0|(1<<CGEN_IFLD_SIGNED), 0, 7, 8, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_S1_R :
      errmsg = insert_normal (cd, fields->f_s1_r, 0, 0, 4, 5, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_S2 :
      errmsg = insert_normal (cd, fields->f_s2, 0, 0, 14, 4, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_SRC3 :
      break;
    case UBICOM32_OPERAND_X_BIT26 :
      errmsg = insert_normal (cd, fields->f_bit26, 0, 0, 26, 1, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_X_D :
      errmsg = insert_normal (cd, fields->f_d, 0, 0, 26, 11, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_X_DN :
      errmsg = insert_normal (cd, fields->f_Dn, 0, 0, 20, 5, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_X_OP2 :
      errmsg = insert_normal (cd, fields->f_op2, 0, 0, 15, 5, 32, total_length, buffer);
      break;
    case UBICOM32_OPERAND_X_S1 :
      errmsg = insert_normal (cd, fields->f_s1, 0, 0, 10, 11, 32, total_length, buffer);
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while building insn.\n"),
	       opindex);
      abort ();
  }

  return errmsg;
}

int ubicom32_cgen_extract_operand
  (CGEN_CPU_DESC, int, CGEN_EXTRACT_INFO *, CGEN_INSN_INT, CGEN_FIELDS *, bfd_vma);

/* Main entry point for operand extraction.
   The result is <= 0 for error, >0 for success.
   ??? Actual values aren't well defined right now.

   This function is basically just a big switch statement.  Earlier versions
   used tables to look up the function to use, but
   - if the table contains both assembler and disassembler functions then
     the disassembler contains much of the assembler and vice-versa,
   - there's a lot of inlining possibilities as things grow,
   - using a switch statement avoids the function call overhead.

   This function could be moved into `print_insn_normal', but keeping it
   separate makes clear the interface between `print_insn_normal' and each of
   the handlers.  */

int
ubicom32_cgen_extract_operand (CGEN_CPU_DESC cd,
			     int opindex,
			     CGEN_EXTRACT_INFO *ex_info,
			     CGEN_INSN_INT insn_value,
			     CGEN_FIELDS * fields,
			     bfd_vma pc)
{
  /* Assume success (for those operands that are nops).  */
  int length = 1;
  unsigned int total_length = CGEN_FIELDS_BITSIZE (fields);

  switch (opindex)
    {
    case UBICOM32_OPERAND_AM :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 7, 3, 32, total_length, pc, & fields->f_Am);
      break;
    case UBICOM32_OPERAND_AN :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 23, 3, 32, total_length, pc, & fields->f_An);
      break;
    case UBICOM32_OPERAND_C :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 21, 1, 32, total_length, pc, & fields->f_C);
      break;
    case UBICOM32_OPERAND_DN :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 20, 5, 32, total_length, pc, & fields->f_Dn);
      break;
    case UBICOM32_OPERAND_P :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 22, 1, 32, total_length, pc, & fields->f_P);
      break;
    case UBICOM32_OPERAND_ACC1HI :
      break;
    case UBICOM32_OPERAND_ACC1LO :
      break;
    case UBICOM32_OPERAND_BIT5 :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 15, 5, 32, total_length, pc, & fields->f_bit5);
      break;
    case UBICOM32_OPERAND_BIT5_ADDSUB :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 15, 5, 32, total_length, pc, & fields->f_bit5);
      break;
    case UBICOM32_OPERAND_CC :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 26, 4, 32, total_length, pc, & fields->f_cond);
      break;
    case UBICOM32_OPERAND_D_AN :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 23, 3, 32, total_length, pc, & fields->f_d_An);
      break;
    case UBICOM32_OPERAND_D_DIRECT_ADDR :
      {
        long value;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 23, 8, 32, total_length, pc, & value);
        value = ((value) << (2));
        fields->f_d_direct = value;
      }
      break;
    case UBICOM32_OPERAND_D_I4_1 :
      length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 19, 4, 32, total_length, pc, & fields->f_d_i4_1);
      break;
    case UBICOM32_OPERAND_D_I4_2 :
      {
        long value;
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 19, 4, 32, total_length, pc, & value);
        value = ((value) << (1));
        fields->f_d_i4_2 = value;
      }
      break;
    case UBICOM32_OPERAND_D_I4_4 :
      {
        long value;
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 19, 4, 32, total_length, pc, & value);
        value = ((value) << (2));
        fields->f_d_i4_4 = value;
      }
      break;
    case UBICOM32_OPERAND_D_IMM7_1 :
      {
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 25, 2, 32, total_length, pc, & fields->f_d_imm7_t);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 20, 5, 32, total_length, pc, & fields->f_d_imm7_b);
        if (length <= 0) break;
{
  FLD (f_d_imm7_1) = ((((((FLD (f_d_imm7_t)) << (5))) | (FLD (f_d_imm7_b)))) << (0));
}
      }
      break;
    case UBICOM32_OPERAND_D_IMM7_2 :
      {
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 25, 2, 32, total_length, pc, & fields->f_d_imm7_t);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 20, 5, 32, total_length, pc, & fields->f_d_imm7_b);
        if (length <= 0) break;
{
  FLD (f_d_imm7_2) = ((((((FLD (f_d_imm7_t)) << (5))) | (FLD (f_d_imm7_b)))) << (1));
}
      }
      break;
    case UBICOM32_OPERAND_D_IMM7_4 :
      {
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 25, 2, 32, total_length, pc, & fields->f_d_imm7_t);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 20, 5, 32, total_length, pc, & fields->f_d_imm7_b);
        if (length <= 0) break;
{
  FLD (f_d_imm7_4) = ((((((FLD (f_d_imm7_t)) << (5))) | (FLD (f_d_imm7_b)))) << (2));
}
      }
      break;
    case UBICOM32_OPERAND_D_IMM8 :
      length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 23, 8, 32, total_length, pc, & fields->f_d_imm8);
      break;
    case UBICOM32_OPERAND_D_R :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 20, 5, 32, total_length, pc, & fields->f_d_r);
      break;
    case UBICOM32_OPERAND_DSP_S2_ACC_REG_ADDSUB :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 14, 4, 32, total_length, pc, & fields->f_dsp_S2);
      break;
    case UBICOM32_OPERAND_DSP_S2_ACC_REG_MUL :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 14, 4, 32, total_length, pc, & fields->f_dsp_S2);
      break;
    case UBICOM32_OPERAND_DSP_S2_DATA_REG :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 14, 4, 32, total_length, pc, & fields->f_dsp_S2);
      break;
    case UBICOM32_OPERAND_DSP_S2_DATA_REG_ADDSUB :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 14, 4, 32, total_length, pc, & fields->f_dsp_S2);
      break;
    case UBICOM32_OPERAND_DSP_S2_SEL :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 18, 1, 32, total_length, pc, & fields->f_dsp_S2_sel);
      break;
    case UBICOM32_OPERAND_DSP_C :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 20, 1, 32, total_length, pc, & fields->f_dsp_C);
      break;
    case UBICOM32_OPERAND_DSP_DESTA :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 16, 1, 32, total_length, pc, & fields->f_dsp_destA);
      break;
    case UBICOM32_OPERAND_DSP_T :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 19, 1, 32, total_length, pc, & fields->f_dsp_T);
      break;
    case UBICOM32_OPERAND_DSP_T_ADDSUB :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 19, 1, 32, total_length, pc, & fields->f_dsp_T);
      break;
    case UBICOM32_OPERAND_FPU_32_S2_ACC_REG :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 14, 4, 32, total_length, pc, & fields->f_FPS2_reg32);
      break;
    case UBICOM32_OPERAND_FPU_32_S2_DATA_REG :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 14, 4, 32, total_length, pc, & fields->f_FPS2_reg32);
      break;
    case UBICOM32_OPERAND_FPU_32_DEST_REG :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 17, 2, 32, total_length, pc, & fields->f_FPD32);
      break;
    case UBICOM32_OPERAND_FPU_64_S1_ACC_REG :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 3, 3, 32, total_length, pc, & fields->f_FPS1_reg64);
      break;
    case UBICOM32_OPERAND_FPU_64_S1_DATA_REG :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 3, 3, 32, total_length, pc, & fields->f_FPS1_reg64);
      break;
    case UBICOM32_OPERAND_FPU_64_S2_ACC_REG :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 14, 3, 32, total_length, pc, & fields->f_FPS2_reg64);
      break;
    case UBICOM32_OPERAND_FPU_64_S2_DATA_REG :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 14, 3, 32, total_length, pc, & fields->f_FPS2_reg64);
      break;
    case UBICOM32_OPERAND_FPU_64_DEST_REG :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 17, 1, 32, total_length, pc, & fields->f_FPD64);
      break;
    case UBICOM32_OPERAND_IMM16_1 :
      length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 26, 16, 32, total_length, pc, & fields->f_imm16_1);
      break;
    case UBICOM32_OPERAND_IMM16_2 :
      length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 15, 16, 32, total_length, pc, & fields->f_imm16_2);
      break;
    case UBICOM32_OPERAND_IMM24 :
      {
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 26, 3, 32, total_length, pc, & fields->f_imm23_21);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 20, 21, 32, total_length, pc, & fields->f_o20_0);
        if (length <= 0) break;
{
  FLD (f_imm24) = ((FLD (f_o20_0)) | (((FLD (f_imm23_21)) << (21))));
}
      }
      break;
    case UBICOM32_OPERAND_IMM25 :
      {
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 27, 4, 32, total_length, pc, & fields->f_imm24_21);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 20, 21, 32, total_length, pc, & fields->f_o20_0);
        if (length <= 0) break;
{
  FLD (f_imm25) = ((FLD (f_o20_0)) | (((FLD (f_imm24_21)) << (21))));
}
      }
      break;
    case UBICOM32_OPERAND_INTERRUPT :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 5, 6, 32, total_length, pc, & fields->f_int);
      break;
    case UBICOM32_OPERAND_IREAD :
      break;
    case UBICOM32_OPERAND_IRQ_0 :
      break;
    case UBICOM32_OPERAND_IRQ_1 :
      break;
    case UBICOM32_OPERAND_LEAI_OFFSET16 :
      {
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 26, 3, 32, total_length, pc, & fields->f_o15_13);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 20, 5, 32, total_length, pc, & fields->f_o12_8);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 10, 3, 32, total_length, pc, & fields->f_o7_5);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 4, 5, 32, total_length, pc, & fields->f_o4_0);
        if (length <= 0) break;
{
  FLD (f_leai_o16) = ((FLD (f_o4_0)) | (((((FLD (f_o15_13)) << (13))) | (((((FLD (f_o12_8)) << (8))) | (((FLD (f_o7_5)) << (5))))))));
}
        fields->f_leai_o16 = ((fields->f_leai_o16) << (2));
      }
      break;
    case UBICOM32_OPERAND_MACHI :
      break;
    case UBICOM32_OPERAND_MACLO :
      break;
    case UBICOM32_OPERAND_OFFSET16 :
      {
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 26, 3, 32, total_length, pc, & fields->f_o15_13);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 20, 5, 32, total_length, pc, & fields->f_o12_8);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 10, 3, 32, total_length, pc, & fields->f_o7_5);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 4, 5, 32, total_length, pc, & fields->f_o4_0);
        if (length <= 0) break;
{
  FLD (f_o16) = ((FLD (f_o4_0)) | (((((FLD (f_o15_13)) << (13))) | (((((FLD (f_o12_8)) << (8))) | (((FLD (f_o7_5)) << (5))))))));
}
        fields->f_o16 = ((fields->f_o16) << (2));
      }
      break;
    case UBICOM32_OPERAND_OFFSET21 :
      {
        long value;
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED)|(1<<CGEN_IFLD_PCREL_ADDR), 0, 20, 21, 32, total_length, pc, & value);
        value = ((((((value) << (2))) + (pc))) & (0xfffffffc));
        fields->f_o21 = value;
      }
      break;
    case UBICOM32_OPERAND_OFFSET24 :
      {
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 26, 3, 32, total_length, pc, & fields->f_o23_21);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 20, 21, 32, total_length, pc, & fields->f_o20_0);
        if (length <= 0) break;
{
  FLD (f_o24) = ((FLD (f_o20_0)) | (((FLD (f_o23_21)) << (21))));
}
        fields->f_o24 = ((((fields->f_o24) << (2))) + (pc));
      }
      break;
    case UBICOM32_OPERAND_OPC1 :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 31, 5, 32, total_length, pc, & fields->f_op1);
      break;
    case UBICOM32_OPERAND_OPC2 :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 15, 5, 32, total_length, pc, & fields->f_op2);
      break;
    case UBICOM32_OPERAND_PDEC_S1_IMM7_4 :
      {
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 9, 2, 32, total_length, pc, & fields->f_s1_imm7_t);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 4, 5, 32, total_length, pc, & fields->f_s1_imm7_b);
        if (length <= 0) break;
{
  FLD (f_s1_imm7_4) = ((((((FLD (f_s1_imm7_t)) << (5))) | (FLD (f_s1_imm7_b)))) << (2));
}
      }
      break;
    case UBICOM32_OPERAND_S1_AN :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 7, 3, 32, total_length, pc, & fields->f_s1_An);
      break;
    case UBICOM32_OPERAND_S1_DIRECT_ADDR :
      {
        long value;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 7, 8, 32, total_length, pc, & value);
        value = ((value) << (2));
        fields->f_s1_direct = value;
      }
      break;
    case UBICOM32_OPERAND_S1_I4_1 :
      length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 3, 4, 32, total_length, pc, & fields->f_s1_i4_1);
      break;
    case UBICOM32_OPERAND_S1_I4_2 :
      {
        long value;
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 3, 4, 32, total_length, pc, & value);
        value = ((value) << (1));
        fields->f_s1_i4_2 = value;
      }
      break;
    case UBICOM32_OPERAND_S1_I4_4 :
      {
        long value;
        length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 3, 4, 32, total_length, pc, & value);
        value = ((value) << (2));
        fields->f_s1_i4_4 = value;
      }
      break;
    case UBICOM32_OPERAND_S1_IMM7_1 :
      {
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 9, 2, 32, total_length, pc, & fields->f_s1_imm7_t);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 4, 5, 32, total_length, pc, & fields->f_s1_imm7_b);
        if (length <= 0) break;
{
  FLD (f_s1_imm7_1) = ((((((FLD (f_s1_imm7_t)) << (5))) | (FLD (f_s1_imm7_b)))) << (0));
}
      }
      break;
    case UBICOM32_OPERAND_S1_IMM7_2 :
      {
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 9, 2, 32, total_length, pc, & fields->f_s1_imm7_t);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 4, 5, 32, total_length, pc, & fields->f_s1_imm7_b);
        if (length <= 0) break;
{
  FLD (f_s1_imm7_2) = ((((((FLD (f_s1_imm7_t)) << (5))) | (FLD (f_s1_imm7_b)))) << (1));
}
      }
      break;
    case UBICOM32_OPERAND_S1_IMM7_4 :
      {
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 9, 2, 32, total_length, pc, & fields->f_s1_imm7_t);
        if (length <= 0) break;
        length = extract_normal (cd, ex_info, insn_value, 0, 0, 4, 5, 32, total_length, pc, & fields->f_s1_imm7_b);
        if (length <= 0) break;
{
  FLD (f_s1_imm7_4) = ((((((FLD (f_s1_imm7_t)) << (5))) | (FLD (f_s1_imm7_b)))) << (2));
}
      }
      break;
    case UBICOM32_OPERAND_S1_IMM8 :
      length = extract_normal (cd, ex_info, insn_value, 0|(1<<CGEN_IFLD_SIGNED), 0, 7, 8, 32, total_length, pc, & fields->f_s1_imm8);
      break;
    case UBICOM32_OPERAND_S1_R :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 4, 5, 32, total_length, pc, & fields->f_s1_r);
      break;
    case UBICOM32_OPERAND_S2 :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 14, 4, 32, total_length, pc, & fields->f_s2);
      break;
    case UBICOM32_OPERAND_SRC3 :
      break;
    case UBICOM32_OPERAND_X_BIT26 :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 26, 1, 32, total_length, pc, & fields->f_bit26);
      break;
    case UBICOM32_OPERAND_X_D :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 26, 11, 32, total_length, pc, & fields->f_d);
      break;
    case UBICOM32_OPERAND_X_DN :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 20, 5, 32, total_length, pc, & fields->f_Dn);
      break;
    case UBICOM32_OPERAND_X_OP2 :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 15, 5, 32, total_length, pc, & fields->f_op2);
      break;
    case UBICOM32_OPERAND_X_S1 :
      length = extract_normal (cd, ex_info, insn_value, 0, 0, 10, 11, 32, total_length, pc, & fields->f_s1);
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while decoding insn.\n"),
	       opindex);
      abort ();
    }

  return length;
}

cgen_insert_fn * const ubicom32_cgen_insert_handlers[] = 
{
  insert_insn_normal,
};

cgen_extract_fn * const ubicom32_cgen_extract_handlers[] = 
{
  extract_insn_normal,
};

int ubicom32_cgen_get_int_operand     (CGEN_CPU_DESC, int, const CGEN_FIELDS *);
bfd_vma ubicom32_cgen_get_vma_operand (CGEN_CPU_DESC, int, const CGEN_FIELDS *);

/* Getting values from cgen_fields is handled by a collection of functions.
   They are distinguished by the type of the VALUE argument they return.
   TODO: floating point, inlining support, remove cases where result type
   not appropriate.  */

int
ubicom32_cgen_get_int_operand (CGEN_CPU_DESC cd ATTRIBUTE_UNUSED,
			     int opindex,
			     const CGEN_FIELDS * fields)
{
  int value;

  switch (opindex)
    {
    case UBICOM32_OPERAND_AM :
      value = fields->f_Am;
      break;
    case UBICOM32_OPERAND_AN :
      value = fields->f_An;
      break;
    case UBICOM32_OPERAND_C :
      value = fields->f_C;
      break;
    case UBICOM32_OPERAND_DN :
      value = fields->f_Dn;
      break;
    case UBICOM32_OPERAND_P :
      value = fields->f_P;
      break;
    case UBICOM32_OPERAND_ACC1HI :
      value = 0;
      break;
    case UBICOM32_OPERAND_ACC1LO :
      value = 0;
      break;
    case UBICOM32_OPERAND_BIT5 :
      value = fields->f_bit5;
      break;
    case UBICOM32_OPERAND_BIT5_ADDSUB :
      value = fields->f_bit5;
      break;
    case UBICOM32_OPERAND_CC :
      value = fields->f_cond;
      break;
    case UBICOM32_OPERAND_D_AN :
      value = fields->f_d_An;
      break;
    case UBICOM32_OPERAND_D_DIRECT_ADDR :
      value = fields->f_d_direct;
      break;
    case UBICOM32_OPERAND_D_I4_1 :
      value = fields->f_d_i4_1;
      break;
    case UBICOM32_OPERAND_D_I4_2 :
      value = fields->f_d_i4_2;
      break;
    case UBICOM32_OPERAND_D_I4_4 :
      value = fields->f_d_i4_4;
      break;
    case UBICOM32_OPERAND_D_IMM7_1 :
      value = fields->f_d_imm7_1;
      break;
    case UBICOM32_OPERAND_D_IMM7_2 :
      value = fields->f_d_imm7_2;
      break;
    case UBICOM32_OPERAND_D_IMM7_4 :
      value = fields->f_d_imm7_4;
      break;
    case UBICOM32_OPERAND_D_IMM8 :
      value = fields->f_d_imm8;
      break;
    case UBICOM32_OPERAND_D_R :
      value = fields->f_d_r;
      break;
    case UBICOM32_OPERAND_DSP_S2_ACC_REG_ADDSUB :
      value = fields->f_dsp_S2;
      break;
    case UBICOM32_OPERAND_DSP_S2_ACC_REG_MUL :
      value = fields->f_dsp_S2;
      break;
    case UBICOM32_OPERAND_DSP_S2_DATA_REG :
      value = fields->f_dsp_S2;
      break;
    case UBICOM32_OPERAND_DSP_S2_DATA_REG_ADDSUB :
      value = fields->f_dsp_S2;
      break;
    case UBICOM32_OPERAND_DSP_S2_SEL :
      value = fields->f_dsp_S2_sel;
      break;
    case UBICOM32_OPERAND_DSP_C :
      value = fields->f_dsp_C;
      break;
    case UBICOM32_OPERAND_DSP_DESTA :
      value = fields->f_dsp_destA;
      break;
    case UBICOM32_OPERAND_DSP_T :
      value = fields->f_dsp_T;
      break;
    case UBICOM32_OPERAND_DSP_T_ADDSUB :
      value = fields->f_dsp_T;
      break;
    case UBICOM32_OPERAND_FPU_32_S2_ACC_REG :
      value = fields->f_FPS2_reg32;
      break;
    case UBICOM32_OPERAND_FPU_32_S2_DATA_REG :
      value = fields->f_FPS2_reg32;
      break;
    case UBICOM32_OPERAND_FPU_32_DEST_REG :
      value = fields->f_FPD32;
      break;
    case UBICOM32_OPERAND_FPU_64_S1_ACC_REG :
      value = fields->f_FPS1_reg64;
      break;
    case UBICOM32_OPERAND_FPU_64_S1_DATA_REG :
      value = fields->f_FPS1_reg64;
      break;
    case UBICOM32_OPERAND_FPU_64_S2_ACC_REG :
      value = fields->f_FPS2_reg64;
      break;
    case UBICOM32_OPERAND_FPU_64_S2_DATA_REG :
      value = fields->f_FPS2_reg64;
      break;
    case UBICOM32_OPERAND_FPU_64_DEST_REG :
      value = fields->f_FPD64;
      break;
    case UBICOM32_OPERAND_IMM16_1 :
      value = fields->f_imm16_1;
      break;
    case UBICOM32_OPERAND_IMM16_2 :
      value = fields->f_imm16_2;
      break;
    case UBICOM32_OPERAND_IMM24 :
      value = fields->f_imm24;
      break;
    case UBICOM32_OPERAND_IMM25 :
      value = fields->f_imm25;
      break;
    case UBICOM32_OPERAND_INTERRUPT :
      value = fields->f_int;
      break;
    case UBICOM32_OPERAND_IREAD :
      value = 0;
      break;
    case UBICOM32_OPERAND_IRQ_0 :
      value = 0;
      break;
    case UBICOM32_OPERAND_IRQ_1 :
      value = 0;
      break;
    case UBICOM32_OPERAND_LEAI_OFFSET16 :
      value = fields->f_leai_o16;
      break;
    case UBICOM32_OPERAND_MACHI :
      value = 0;
      break;
    case UBICOM32_OPERAND_MACLO :
      value = 0;
      break;
    case UBICOM32_OPERAND_OFFSET16 :
      value = fields->f_o16;
      break;
    case UBICOM32_OPERAND_OFFSET21 :
      value = fields->f_o21;
      break;
    case UBICOM32_OPERAND_OFFSET24 :
      value = fields->f_o24;
      break;
    case UBICOM32_OPERAND_OPC1 :
      value = fields->f_op1;
      break;
    case UBICOM32_OPERAND_OPC2 :
      value = fields->f_op2;
      break;
    case UBICOM32_OPERAND_PDEC_S1_IMM7_4 :
      value = fields->f_s1_imm7_4;
      break;
    case UBICOM32_OPERAND_S1_AN :
      value = fields->f_s1_An;
      break;
    case UBICOM32_OPERAND_S1_DIRECT_ADDR :
      value = fields->f_s1_direct;
      break;
    case UBICOM32_OPERAND_S1_I4_1 :
      value = fields->f_s1_i4_1;
      break;
    case UBICOM32_OPERAND_S1_I4_2 :
      value = fields->f_s1_i4_2;
      break;
    case UBICOM32_OPERAND_S1_I4_4 :
      value = fields->f_s1_i4_4;
      break;
    case UBICOM32_OPERAND_S1_IMM7_1 :
      value = fields->f_s1_imm7_1;
      break;
    case UBICOM32_OPERAND_S1_IMM7_2 :
      value = fields->f_s1_imm7_2;
      break;
    case UBICOM32_OPERAND_S1_IMM7_4 :
      value = fields->f_s1_imm7_4;
      break;
    case UBICOM32_OPERAND_S1_IMM8 :
      value = fields->f_s1_imm8;
      break;
    case UBICOM32_OPERAND_S1_R :
      value = fields->f_s1_r;
      break;
    case UBICOM32_OPERAND_S2 :
      value = fields->f_s2;
      break;
    case UBICOM32_OPERAND_SRC3 :
      value = 0;
      break;
    case UBICOM32_OPERAND_X_BIT26 :
      value = fields->f_bit26;
      break;
    case UBICOM32_OPERAND_X_D :
      value = fields->f_d;
      break;
    case UBICOM32_OPERAND_X_DN :
      value = fields->f_Dn;
      break;
    case UBICOM32_OPERAND_X_OP2 :
      value = fields->f_op2;
      break;
    case UBICOM32_OPERAND_X_S1 :
      value = fields->f_s1;
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while getting int operand.\n"),
		       opindex);
      abort ();
  }

  return value;
}

bfd_vma
ubicom32_cgen_get_vma_operand (CGEN_CPU_DESC cd ATTRIBUTE_UNUSED,
			     int opindex,
			     const CGEN_FIELDS * fields)
{
  bfd_vma value;

  switch (opindex)
    {
    case UBICOM32_OPERAND_AM :
      value = fields->f_Am;
      break;
    case UBICOM32_OPERAND_AN :
      value = fields->f_An;
      break;
    case UBICOM32_OPERAND_C :
      value = fields->f_C;
      break;
    case UBICOM32_OPERAND_DN :
      value = fields->f_Dn;
      break;
    case UBICOM32_OPERAND_P :
      value = fields->f_P;
      break;
    case UBICOM32_OPERAND_ACC1HI :
      value = 0;
      break;
    case UBICOM32_OPERAND_ACC1LO :
      value = 0;
      break;
    case UBICOM32_OPERAND_BIT5 :
      value = fields->f_bit5;
      break;
    case UBICOM32_OPERAND_BIT5_ADDSUB :
      value = fields->f_bit5;
      break;
    case UBICOM32_OPERAND_CC :
      value = fields->f_cond;
      break;
    case UBICOM32_OPERAND_D_AN :
      value = fields->f_d_An;
      break;
    case UBICOM32_OPERAND_D_DIRECT_ADDR :
      value = fields->f_d_direct;
      break;
    case UBICOM32_OPERAND_D_I4_1 :
      value = fields->f_d_i4_1;
      break;
    case UBICOM32_OPERAND_D_I4_2 :
      value = fields->f_d_i4_2;
      break;
    case UBICOM32_OPERAND_D_I4_4 :
      value = fields->f_d_i4_4;
      break;
    case UBICOM32_OPERAND_D_IMM7_1 :
      value = fields->f_d_imm7_1;
      break;
    case UBICOM32_OPERAND_D_IMM7_2 :
      value = fields->f_d_imm7_2;
      break;
    case UBICOM32_OPERAND_D_IMM7_4 :
      value = fields->f_d_imm7_4;
      break;
    case UBICOM32_OPERAND_D_IMM8 :
      value = fields->f_d_imm8;
      break;
    case UBICOM32_OPERAND_D_R :
      value = fields->f_d_r;
      break;
    case UBICOM32_OPERAND_DSP_S2_ACC_REG_ADDSUB :
      value = fields->f_dsp_S2;
      break;
    case UBICOM32_OPERAND_DSP_S2_ACC_REG_MUL :
      value = fields->f_dsp_S2;
      break;
    case UBICOM32_OPERAND_DSP_S2_DATA_REG :
      value = fields->f_dsp_S2;
      break;
    case UBICOM32_OPERAND_DSP_S2_DATA_REG_ADDSUB :
      value = fields->f_dsp_S2;
      break;
    case UBICOM32_OPERAND_DSP_S2_SEL :
      value = fields->f_dsp_S2_sel;
      break;
    case UBICOM32_OPERAND_DSP_C :
      value = fields->f_dsp_C;
      break;
    case UBICOM32_OPERAND_DSP_DESTA :
      value = fields->f_dsp_destA;
      break;
    case UBICOM32_OPERAND_DSP_T :
      value = fields->f_dsp_T;
      break;
    case UBICOM32_OPERAND_DSP_T_ADDSUB :
      value = fields->f_dsp_T;
      break;
    case UBICOM32_OPERAND_FPU_32_S2_ACC_REG :
      value = fields->f_FPS2_reg32;
      break;
    case UBICOM32_OPERAND_FPU_32_S2_DATA_REG :
      value = fields->f_FPS2_reg32;
      break;
    case UBICOM32_OPERAND_FPU_32_DEST_REG :
      value = fields->f_FPD32;
      break;
    case UBICOM32_OPERAND_FPU_64_S1_ACC_REG :
      value = fields->f_FPS1_reg64;
      break;
    case UBICOM32_OPERAND_FPU_64_S1_DATA_REG :
      value = fields->f_FPS1_reg64;
      break;
    case UBICOM32_OPERAND_FPU_64_S2_ACC_REG :
      value = fields->f_FPS2_reg64;
      break;
    case UBICOM32_OPERAND_FPU_64_S2_DATA_REG :
      value = fields->f_FPS2_reg64;
      break;
    case UBICOM32_OPERAND_FPU_64_DEST_REG :
      value = fields->f_FPD64;
      break;
    case UBICOM32_OPERAND_IMM16_1 :
      value = fields->f_imm16_1;
      break;
    case UBICOM32_OPERAND_IMM16_2 :
      value = fields->f_imm16_2;
      break;
    case UBICOM32_OPERAND_IMM24 :
      value = fields->f_imm24;
      break;
    case UBICOM32_OPERAND_IMM25 :
      value = fields->f_imm25;
      break;
    case UBICOM32_OPERAND_INTERRUPT :
      value = fields->f_int;
      break;
    case UBICOM32_OPERAND_IREAD :
      value = 0;
      break;
    case UBICOM32_OPERAND_IRQ_0 :
      value = 0;
      break;
    case UBICOM32_OPERAND_IRQ_1 :
      value = 0;
      break;
    case UBICOM32_OPERAND_LEAI_OFFSET16 :
      value = fields->f_leai_o16;
      break;
    case UBICOM32_OPERAND_MACHI :
      value = 0;
      break;
    case UBICOM32_OPERAND_MACLO :
      value = 0;
      break;
    case UBICOM32_OPERAND_OFFSET16 :
      value = fields->f_o16;
      break;
    case UBICOM32_OPERAND_OFFSET21 :
      value = fields->f_o21;
      break;
    case UBICOM32_OPERAND_OFFSET24 :
      value = fields->f_o24;
      break;
    case UBICOM32_OPERAND_OPC1 :
      value = fields->f_op1;
      break;
    case UBICOM32_OPERAND_OPC2 :
      value = fields->f_op2;
      break;
    case UBICOM32_OPERAND_PDEC_S1_IMM7_4 :
      value = fields->f_s1_imm7_4;
      break;
    case UBICOM32_OPERAND_S1_AN :
      value = fields->f_s1_An;
      break;
    case UBICOM32_OPERAND_S1_DIRECT_ADDR :
      value = fields->f_s1_direct;
      break;
    case UBICOM32_OPERAND_S1_I4_1 :
      value = fields->f_s1_i4_1;
      break;
    case UBICOM32_OPERAND_S1_I4_2 :
      value = fields->f_s1_i4_2;
      break;
    case UBICOM32_OPERAND_S1_I4_4 :
      value = fields->f_s1_i4_4;
      break;
    case UBICOM32_OPERAND_S1_IMM7_1 :
      value = fields->f_s1_imm7_1;
      break;
    case UBICOM32_OPERAND_S1_IMM7_2 :
      value = fields->f_s1_imm7_2;
      break;
    case UBICOM32_OPERAND_S1_IMM7_4 :
      value = fields->f_s1_imm7_4;
      break;
    case UBICOM32_OPERAND_S1_IMM8 :
      value = fields->f_s1_imm8;
      break;
    case UBICOM32_OPERAND_S1_R :
      value = fields->f_s1_r;
      break;
    case UBICOM32_OPERAND_S2 :
      value = fields->f_s2;
      break;
    case UBICOM32_OPERAND_SRC3 :
      value = 0;
      break;
    case UBICOM32_OPERAND_X_BIT26 :
      value = fields->f_bit26;
      break;
    case UBICOM32_OPERAND_X_D :
      value = fields->f_d;
      break;
    case UBICOM32_OPERAND_X_DN :
      value = fields->f_Dn;
      break;
    case UBICOM32_OPERAND_X_OP2 :
      value = fields->f_op2;
      break;
    case UBICOM32_OPERAND_X_S1 :
      value = fields->f_s1;
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while getting vma operand.\n"),
		       opindex);
      abort ();
  }

  return value;
}

void ubicom32_cgen_set_int_operand  (CGEN_CPU_DESC, int, CGEN_FIELDS *, int);
void ubicom32_cgen_set_vma_operand  (CGEN_CPU_DESC, int, CGEN_FIELDS *, bfd_vma);

/* Stuffing values in cgen_fields is handled by a collection of functions.
   They are distinguished by the type of the VALUE argument they accept.
   TODO: floating point, inlining support, remove cases where argument type
   not appropriate.  */

void
ubicom32_cgen_set_int_operand (CGEN_CPU_DESC cd ATTRIBUTE_UNUSED,
			     int opindex,
			     CGEN_FIELDS * fields,
			     int value)
{
  switch (opindex)
    {
    case UBICOM32_OPERAND_AM :
      fields->f_Am = value;
      break;
    case UBICOM32_OPERAND_AN :
      fields->f_An = value;
      break;
    case UBICOM32_OPERAND_C :
      fields->f_C = value;
      break;
    case UBICOM32_OPERAND_DN :
      fields->f_Dn = value;
      break;
    case UBICOM32_OPERAND_P :
      fields->f_P = value;
      break;
    case UBICOM32_OPERAND_ACC1HI :
      break;
    case UBICOM32_OPERAND_ACC1LO :
      break;
    case UBICOM32_OPERAND_BIT5 :
      fields->f_bit5 = value;
      break;
    case UBICOM32_OPERAND_BIT5_ADDSUB :
      fields->f_bit5 = value;
      break;
    case UBICOM32_OPERAND_CC :
      fields->f_cond = value;
      break;
    case UBICOM32_OPERAND_D_AN :
      fields->f_d_An = value;
      break;
    case UBICOM32_OPERAND_D_DIRECT_ADDR :
      fields->f_d_direct = value;
      break;
    case UBICOM32_OPERAND_D_I4_1 :
      fields->f_d_i4_1 = value;
      break;
    case UBICOM32_OPERAND_D_I4_2 :
      fields->f_d_i4_2 = value;
      break;
    case UBICOM32_OPERAND_D_I4_4 :
      fields->f_d_i4_4 = value;
      break;
    case UBICOM32_OPERAND_D_IMM7_1 :
      fields->f_d_imm7_1 = value;
      break;
    case UBICOM32_OPERAND_D_IMM7_2 :
      fields->f_d_imm7_2 = value;
      break;
    case UBICOM32_OPERAND_D_IMM7_4 :
      fields->f_d_imm7_4 = value;
      break;
    case UBICOM32_OPERAND_D_IMM8 :
      fields->f_d_imm8 = value;
      break;
    case UBICOM32_OPERAND_D_R :
      fields->f_d_r = value;
      break;
    case UBICOM32_OPERAND_DSP_S2_ACC_REG_ADDSUB :
      fields->f_dsp_S2 = value;
      break;
    case UBICOM32_OPERAND_DSP_S2_ACC_REG_MUL :
      fields->f_dsp_S2 = value;
      break;
    case UBICOM32_OPERAND_DSP_S2_DATA_REG :
      fields->f_dsp_S2 = value;
      break;
    case UBICOM32_OPERAND_DSP_S2_DATA_REG_ADDSUB :
      fields->f_dsp_S2 = value;
      break;
    case UBICOM32_OPERAND_DSP_S2_SEL :
      fields->f_dsp_S2_sel = value;
      break;
    case UBICOM32_OPERAND_DSP_C :
      fields->f_dsp_C = value;
      break;
    case UBICOM32_OPERAND_DSP_DESTA :
      fields->f_dsp_destA = value;
      break;
    case UBICOM32_OPERAND_DSP_T :
      fields->f_dsp_T = value;
      break;
    case UBICOM32_OPERAND_DSP_T_ADDSUB :
      fields->f_dsp_T = value;
      break;
    case UBICOM32_OPERAND_FPU_32_S2_ACC_REG :
      fields->f_FPS2_reg32 = value;
      break;
    case UBICOM32_OPERAND_FPU_32_S2_DATA_REG :
      fields->f_FPS2_reg32 = value;
      break;
    case UBICOM32_OPERAND_FPU_32_DEST_REG :
      fields->f_FPD32 = value;
      break;
    case UBICOM32_OPERAND_FPU_64_S1_ACC_REG :
      fields->f_FPS1_reg64 = value;
      break;
    case UBICOM32_OPERAND_FPU_64_S1_DATA_REG :
      fields->f_FPS1_reg64 = value;
      break;
    case UBICOM32_OPERAND_FPU_64_S2_ACC_REG :
      fields->f_FPS2_reg64 = value;
      break;
    case UBICOM32_OPERAND_FPU_64_S2_DATA_REG :
      fields->f_FPS2_reg64 = value;
      break;
    case UBICOM32_OPERAND_FPU_64_DEST_REG :
      fields->f_FPD64 = value;
      break;
    case UBICOM32_OPERAND_IMM16_1 :
      fields->f_imm16_1 = value;
      break;
    case UBICOM32_OPERAND_IMM16_2 :
      fields->f_imm16_2 = value;
      break;
    case UBICOM32_OPERAND_IMM24 :
      fields->f_imm24 = value;
      break;
    case UBICOM32_OPERAND_IMM25 :
      fields->f_imm25 = value;
      break;
    case UBICOM32_OPERAND_INTERRUPT :
      fields->f_int = value;
      break;
    case UBICOM32_OPERAND_IREAD :
      break;
    case UBICOM32_OPERAND_IRQ_0 :
      break;
    case UBICOM32_OPERAND_IRQ_1 :
      break;
    case UBICOM32_OPERAND_LEAI_OFFSET16 :
      fields->f_leai_o16 = value;
      break;
    case UBICOM32_OPERAND_MACHI :
      break;
    case UBICOM32_OPERAND_MACLO :
      break;
    case UBICOM32_OPERAND_OFFSET16 :
      fields->f_o16 = value;
      break;
    case UBICOM32_OPERAND_OFFSET21 :
      fields->f_o21 = value;
      break;
    case UBICOM32_OPERAND_OFFSET24 :
      fields->f_o24 = value;
      break;
    case UBICOM32_OPERAND_OPC1 :
      fields->f_op1 = value;
      break;
    case UBICOM32_OPERAND_OPC2 :
      fields->f_op2 = value;
      break;
    case UBICOM32_OPERAND_PDEC_S1_IMM7_4 :
      fields->f_s1_imm7_4 = value;
      break;
    case UBICOM32_OPERAND_S1_AN :
      fields->f_s1_An = value;
      break;
    case UBICOM32_OPERAND_S1_DIRECT_ADDR :
      fields->f_s1_direct = value;
      break;
    case UBICOM32_OPERAND_S1_I4_1 :
      fields->f_s1_i4_1 = value;
      break;
    case UBICOM32_OPERAND_S1_I4_2 :
      fields->f_s1_i4_2 = value;
      break;
    case UBICOM32_OPERAND_S1_I4_4 :
      fields->f_s1_i4_4 = value;
      break;
    case UBICOM32_OPERAND_S1_IMM7_1 :
      fields->f_s1_imm7_1 = value;
      break;
    case UBICOM32_OPERAND_S1_IMM7_2 :
      fields->f_s1_imm7_2 = value;
      break;
    case UBICOM32_OPERAND_S1_IMM7_4 :
      fields->f_s1_imm7_4 = value;
      break;
    case UBICOM32_OPERAND_S1_IMM8 :
      fields->f_s1_imm8 = value;
      break;
    case UBICOM32_OPERAND_S1_R :
      fields->f_s1_r = value;
      break;
    case UBICOM32_OPERAND_S2 :
      fields->f_s2 = value;
      break;
    case UBICOM32_OPERAND_SRC3 :
      break;
    case UBICOM32_OPERAND_X_BIT26 :
      fields->f_bit26 = value;
      break;
    case UBICOM32_OPERAND_X_D :
      fields->f_d = value;
      break;
    case UBICOM32_OPERAND_X_DN :
      fields->f_Dn = value;
      break;
    case UBICOM32_OPERAND_X_OP2 :
      fields->f_op2 = value;
      break;
    case UBICOM32_OPERAND_X_S1 :
      fields->f_s1 = value;
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while setting int operand.\n"),
		       opindex);
      abort ();
  }
}

void
ubicom32_cgen_set_vma_operand (CGEN_CPU_DESC cd ATTRIBUTE_UNUSED,
			     int opindex,
			     CGEN_FIELDS * fields,
			     bfd_vma value)
{
  switch (opindex)
    {
    case UBICOM32_OPERAND_AM :
      fields->f_Am = value;
      break;
    case UBICOM32_OPERAND_AN :
      fields->f_An = value;
      break;
    case UBICOM32_OPERAND_C :
      fields->f_C = value;
      break;
    case UBICOM32_OPERAND_DN :
      fields->f_Dn = value;
      break;
    case UBICOM32_OPERAND_P :
      fields->f_P = value;
      break;
    case UBICOM32_OPERAND_ACC1HI :
      break;
    case UBICOM32_OPERAND_ACC1LO :
      break;
    case UBICOM32_OPERAND_BIT5 :
      fields->f_bit5 = value;
      break;
    case UBICOM32_OPERAND_BIT5_ADDSUB :
      fields->f_bit5 = value;
      break;
    case UBICOM32_OPERAND_CC :
      fields->f_cond = value;
      break;
    case UBICOM32_OPERAND_D_AN :
      fields->f_d_An = value;
      break;
    case UBICOM32_OPERAND_D_DIRECT_ADDR :
      fields->f_d_direct = value;
      break;
    case UBICOM32_OPERAND_D_I4_1 :
      fields->f_d_i4_1 = value;
      break;
    case UBICOM32_OPERAND_D_I4_2 :
      fields->f_d_i4_2 = value;
      break;
    case UBICOM32_OPERAND_D_I4_4 :
      fields->f_d_i4_4 = value;
      break;
    case UBICOM32_OPERAND_D_IMM7_1 :
      fields->f_d_imm7_1 = value;
      break;
    case UBICOM32_OPERAND_D_IMM7_2 :
      fields->f_d_imm7_2 = value;
      break;
    case UBICOM32_OPERAND_D_IMM7_4 :
      fields->f_d_imm7_4 = value;
      break;
    case UBICOM32_OPERAND_D_IMM8 :
      fields->f_d_imm8 = value;
      break;
    case UBICOM32_OPERAND_D_R :
      fields->f_d_r = value;
      break;
    case UBICOM32_OPERAND_DSP_S2_ACC_REG_ADDSUB :
      fields->f_dsp_S2 = value;
      break;
    case UBICOM32_OPERAND_DSP_S2_ACC_REG_MUL :
      fields->f_dsp_S2 = value;
      break;
    case UBICOM32_OPERAND_DSP_S2_DATA_REG :
      fields->f_dsp_S2 = value;
      break;
    case UBICOM32_OPERAND_DSP_S2_DATA_REG_ADDSUB :
      fields->f_dsp_S2 = value;
      break;
    case UBICOM32_OPERAND_DSP_S2_SEL :
      fields->f_dsp_S2_sel = value;
      break;
    case UBICOM32_OPERAND_DSP_C :
      fields->f_dsp_C = value;
      break;
    case UBICOM32_OPERAND_DSP_DESTA :
      fields->f_dsp_destA = value;
      break;
    case UBICOM32_OPERAND_DSP_T :
      fields->f_dsp_T = value;
      break;
    case UBICOM32_OPERAND_DSP_T_ADDSUB :
      fields->f_dsp_T = value;
      break;
    case UBICOM32_OPERAND_FPU_32_S2_ACC_REG :
      fields->f_FPS2_reg32 = value;
      break;
    case UBICOM32_OPERAND_FPU_32_S2_DATA_REG :
      fields->f_FPS2_reg32 = value;
      break;
    case UBICOM32_OPERAND_FPU_32_DEST_REG :
      fields->f_FPD32 = value;
      break;
    case UBICOM32_OPERAND_FPU_64_S1_ACC_REG :
      fields->f_FPS1_reg64 = value;
      break;
    case UBICOM32_OPERAND_FPU_64_S1_DATA_REG :
      fields->f_FPS1_reg64 = value;
      break;
    case UBICOM32_OPERAND_FPU_64_S2_ACC_REG :
      fields->f_FPS2_reg64 = value;
      break;
    case UBICOM32_OPERAND_FPU_64_S2_DATA_REG :
      fields->f_FPS2_reg64 = value;
      break;
    case UBICOM32_OPERAND_FPU_64_DEST_REG :
      fields->f_FPD64 = value;
      break;
    case UBICOM32_OPERAND_IMM16_1 :
      fields->f_imm16_1 = value;
      break;
    case UBICOM32_OPERAND_IMM16_2 :
      fields->f_imm16_2 = value;
      break;
    case UBICOM32_OPERAND_IMM24 :
      fields->f_imm24 = value;
      break;
    case UBICOM32_OPERAND_IMM25 :
      fields->f_imm25 = value;
      break;
    case UBICOM32_OPERAND_INTERRUPT :
      fields->f_int = value;
      break;
    case UBICOM32_OPERAND_IREAD :
      break;
    case UBICOM32_OPERAND_IRQ_0 :
      break;
    case UBICOM32_OPERAND_IRQ_1 :
      break;
    case UBICOM32_OPERAND_LEAI_OFFSET16 :
      fields->f_leai_o16 = value;
      break;
    case UBICOM32_OPERAND_MACHI :
      break;
    case UBICOM32_OPERAND_MACLO :
      break;
    case UBICOM32_OPERAND_OFFSET16 :
      fields->f_o16 = value;
      break;
    case UBICOM32_OPERAND_OFFSET21 :
      fields->f_o21 = value;
      break;
    case UBICOM32_OPERAND_OFFSET24 :
      fields->f_o24 = value;
      break;
    case UBICOM32_OPERAND_OPC1 :
      fields->f_op1 = value;
      break;
    case UBICOM32_OPERAND_OPC2 :
      fields->f_op2 = value;
      break;
    case UBICOM32_OPERAND_PDEC_S1_IMM7_4 :
      fields->f_s1_imm7_4 = value;
      break;
    case UBICOM32_OPERAND_S1_AN :
      fields->f_s1_An = value;
      break;
    case UBICOM32_OPERAND_S1_DIRECT_ADDR :
      fields->f_s1_direct = value;
      break;
    case UBICOM32_OPERAND_S1_I4_1 :
      fields->f_s1_i4_1 = value;
      break;
    case UBICOM32_OPERAND_S1_I4_2 :
      fields->f_s1_i4_2 = value;
      break;
    case UBICOM32_OPERAND_S1_I4_4 :
      fields->f_s1_i4_4 = value;
      break;
    case UBICOM32_OPERAND_S1_IMM7_1 :
      fields->f_s1_imm7_1 = value;
      break;
    case UBICOM32_OPERAND_S1_IMM7_2 :
      fields->f_s1_imm7_2 = value;
      break;
    case UBICOM32_OPERAND_S1_IMM7_4 :
      fields->f_s1_imm7_4 = value;
      break;
    case UBICOM32_OPERAND_S1_IMM8 :
      fields->f_s1_imm8 = value;
      break;
    case UBICOM32_OPERAND_S1_R :
      fields->f_s1_r = value;
      break;
    case UBICOM32_OPERAND_S2 :
      fields->f_s2 = value;
      break;
    case UBICOM32_OPERAND_SRC3 :
      break;
    case UBICOM32_OPERAND_X_BIT26 :
      fields->f_bit26 = value;
      break;
    case UBICOM32_OPERAND_X_D :
      fields->f_d = value;
      break;
    case UBICOM32_OPERAND_X_DN :
      fields->f_Dn = value;
      break;
    case UBICOM32_OPERAND_X_OP2 :
      fields->f_op2 = value;
      break;
    case UBICOM32_OPERAND_X_S1 :
      fields->f_s1 = value;
      break;

    default :
      /* xgettext:c-format */
      fprintf (stderr, _("Unrecognized field %d while setting vma operand.\n"),
		       opindex);
      abort ();
  }
}

/* Function to call before using the instruction builder tables.  */

void
ubicom32_cgen_init_ibld_table (CGEN_CPU_DESC cd)
{
  cd->insert_handlers = & ubicom32_cgen_insert_handlers[0];
  cd->extract_handlers = & ubicom32_cgen_extract_handlers[0];

  cd->insert_operand = ubicom32_cgen_insert_operand;
  cd->extract_operand = ubicom32_cgen_extract_operand;

  cd->get_int_operand = ubicom32_cgen_get_int_operand;
  cd->set_int_operand = ubicom32_cgen_set_int_operand;
  cd->get_vma_operand = ubicom32_cgen_get_vma_operand;
  cd->set_vma_operand = ubicom32_cgen_set_vma_operand;
}
