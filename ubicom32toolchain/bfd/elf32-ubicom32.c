/* Ubicom32 specific support for 32-bit ELF
   Copyright 2000 Free Software Foundation, Inc.

This file is part of BFD, the Binary File Descriptor library.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include <string.h>
#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/ubicom32.h"
#include "elf/dwarf2.h"

/* Call offset = signed 24bit word offset
   => 26bit signed byte offset.  */
#define UBICOM32_CALL_MAX_POS_OFFS ((1 << 25) - 1)
#define UBICOM32_CALL_MAX_NEG_OFFS (-(1 << 25))

#define UNDEFINED_SYMBOL (~(bfd_vma)0)
#define BASEADDR(SEC) ((SEC)->output_section->vma + (SEC)->output_offset)

#if 0
#define DPRINTF(fmt, ...) { printf("DBG %4d:"  fmt, __LINE__, __VA_ARGS__); fflush(stdout); }
#else
#define DPRINTF(fmt, ...) {}
#endif
struct debugLineInfo {
  unsigned int startOffset;
  unsigned int length;
  char *sectionName;
  unsigned int startRelocIndex;
  unsigned int endRelocIndex;
  unsigned int discard;
};

struct debugLineInfoHeader {
  unsigned int numEntries;
  struct debugLineInfo linfo[1];
};

/* we want RELA relocations, not REL */
#undef USE_REL
#define USE_RELA

static bfd_reloc_status_type ubicom32_elf_generic_reloc
  PARAMS ((bfd *abfd, arelent *reloc_entry, asymbol *symbol, PTR data,
	   asection *input_section, bfd *output_bfd, char **error_message));
static bfd_reloc_status_type ubicom32_elf_relocate_hi16
  PARAMS ((bfd *,  Elf_Internal_Rela *, bfd_byte *, bfd_vma));
static bfd_reloc_status_type ubicom32_elf_relocate_lo16
  PARAMS ((bfd *,  Elf_Internal_Rela *, bfd_byte *, bfd_vma));
static bfd_reloc_status_type ubicom32_elf_relocate_hi24
  PARAMS ((bfd *,  Elf_Internal_Rela *,   bfd_byte *, bfd_vma));
static bfd_reloc_status_type ubicom32_elf_relocate_lo7_s
  PARAMS ((bfd *,  Elf_Internal_Rela *,   bfd_byte *, bfd_vma));
static bfd_reloc_status_type ubicom32_elf_relocate_lo7_2_s
  PARAMS ((bfd *,  Elf_Internal_Rela *,   bfd_byte *, bfd_vma));
static bfd_reloc_status_type ubicom32_elf_relocate_lo7_4_s
  PARAMS ((bfd *,  Elf_Internal_Rela *,   bfd_byte *, bfd_vma));
static bfd_reloc_status_type ubicom32_elf_relocate_lo7_d
  PARAMS ((bfd *,  Elf_Internal_Rela *,   bfd_byte *, bfd_vma));
static bfd_reloc_status_type ubicom32_elf_relocate_lo7_2_d
  PARAMS ((bfd *,  Elf_Internal_Rela *,   bfd_byte *, bfd_vma));
static bfd_reloc_status_type ubicom32_elf_relocate_lo7_4_d
  PARAMS ((bfd *,  Elf_Internal_Rela *,   bfd_byte *, bfd_vma));
static bfd_reloc_status_type ubicom32_elf_relocate_pcrel24
  PARAMS ((bfd *, asection *, Elf_Internal_Rela *, bfd_byte *, bfd_vma));
static bfd_reloc_status_type ubicom32_elf_relocate_got_lo
  PARAMS ((bfd *, asection *, Elf_Internal_Rela *, bfd_byte *, bfd_vma));
static bfd_reloc_status_type ubicom32_elf_relocate_funcdesc_got_lo
  PARAMS ((bfd *, asection *, Elf_Internal_Rela *, bfd_byte *, bfd_vma));

#if 0
static bfd_reloc_status_type ubicom32_elf_relocate_funcdesc
  PARAMS ((bfd *, asection *, Elf_Internal_Rela *, bfd_byte *, bfd_vma));
#endif

static bfd_reloc_status_type ubicom32_elf_relocate_lo_calli
  PARAMS ((bfd *,  Elf_Internal_Rela *,   bfd_byte *, bfd_vma, int));
static bfd_reloc_status_type ubicom32_elf_relocate_lo_leai
  PARAMS ((bfd *,  Elf_Internal_Rela *,   bfd_byte *, bfd_vma, int));
static bfd_reloc_status_type ubicom32_elf_relocate_got_lo_leai
  PARAMS ((bfd *,  Elf_Internal_Rela *,   bfd_byte *, bfd_vma, int));
static bfd_reloc_status_type ubicom32_elf_relocate_funcdesc_got_lo_leai
  PARAMS ((bfd *,  Elf_Internal_Rela *,   bfd_byte *, bfd_vma, int));

static void ubicom32_info_to_howto_rela
  PARAMS ((bfd *, arelent *, Elf_Internal_Rela *));
static reloc_howto_type * ubicom32_reloc_type_lookup
  PARAMS ((bfd *abfd, bfd_reloc_code_real_type code));
static bfd_vma symbol_value
  PARAMS ((bfd *, Elf_Internal_Rela *));
static Elf_Internal_Shdr *file_symtab_hdr
  PARAMS ((bfd *));
static Elf_Internal_Sym *file_isymbuf
  PARAMS ((bfd *));
static Elf_Internal_Rela *section_relocs
  PARAMS ((bfd *, asection *));
static bfd_byte *section_contents
  PARAMS ((bfd *, asection *));
static bfd_boolean ubicom32_elf_relax_section
  PARAMS ((bfd *, asection *, struct bfd_link_info *, bfd_boolean *));
static bfd_boolean ubicom32_elf_relax_calli
  PARAMS ((bfd *, asection *, bfd_boolean *));
static bfd_boolean ubicom32_elf_relax_delete_bytes
  PARAMS ((bfd *, asection *, bfd_vma, int));
static void adjust_sec_relocations
  PARAMS ((bfd *, asection *, asection *, bfd_vma, int));
static void adjust_all_relocations
  PARAMS ((bfd *, asection *, bfd_vma, int));

static bfd_reloc_status_type ubicom32_final_link_relocate
  PARAMS ((reloc_howto_type *, bfd *, asection *, bfd_byte *,
	   Elf_Internal_Rela *, bfd_vma));
static bfd_boolean ubicom32_elf_relocate_section
  PARAMS ((bfd *, struct bfd_link_info *, bfd *, asection *,
	   bfd_byte *, Elf_Internal_Rela *, Elf_Internal_Sym *,
	   asection **));

static bfd_boolean ubicom32_elf_gc_sweep_hook
  PARAMS ((bfd *, struct bfd_link_info *, asection *, const
	   Elf_Internal_Rela *));
static asection * ubicom32_elf_gc_mark_hook
  PARAMS ((asection *, struct bfd_link_info *, Elf_Internal_Rela *, struct
	   elf_link_hash_entry *, Elf_Internal_Sym *));
static bfd_boolean ubicom32_elf_check_relocs
  PARAMS ((bfd *, struct bfd_link_info *, asection *,
	   const Elf_Internal_Rela *));
extern bfd_boolean ubicom32_elf_discard_info
  PARAMS ((bfd *, struct elf_reloc_cookie *, struct bfd_link_info *));

static bfd_boolean ubicom32_elf_object_p PARAMS ((bfd *));
static bfd_boolean ubicom32_elf_set_private_flags PARAMS ((bfd *, flagword));
static bfd_boolean ubicom32_elf_copy_private_bfd_data PARAMS ((bfd *, bfd *));
static bfd_boolean ubicom32_elf_merge_private_bfd_data PARAMS ((bfd *, bfd *));
static bfd_boolean ubicom32_elf_print_private_bfd_data PARAMS ((bfd *, PTR));

//static unsigned long read_unsigned_leb128 (bfd *, char *, unsigned int *);

//static long read_signed_leb128 (bfd *, char *, unsigned int *);

/* read dwarf information from a buffer */

#define UBICOM32_HOWTO(t,rs,s,bs,pr,bp,name,sm,dm) \
    HOWTO(t,			/* type */ \
	  rs,			/* rightshift */ \
	  s,			/* size (0 = byte, 1 = short, 2 = long) */ \
	  bs,			/* bitsize */ \
	  pr,			/* pc_relative */ \
	  bp,			/* bitpos */ \
	  complain_overflow_bitfield,	/* complain_on_overflow */ \
	  ubicom32_elf_generic_reloc,	/* special_function */ \
	  name,			/* name */ \
	  FALSE,		/* partial_inplace */ \
	  sm,			/* src_mask */ \
	  dm,			/* dst_mask */ \
	  pr)			/* pcrel_offset */

/* Special Note:  For addresses, we must always zero out the top byte of a
		  address because the harvard address space is represented as
		  a single virtual address space that uses the top byte to denote
		  whether the address belongs in the data or program space.  This is
		  done to accomodate GDB which cannot handle program and data addresses
		  overlapping.                                                       */

static reloc_howto_type ubicom32_elf_howto_table [] =
{
  /* This reloc does nothing.  */
  UBICOM32_HOWTO (R_UBICOM32_NONE, 0,	2, 32, FALSE, 0, "R_UBICOM32_NONE", 0, 0),

  /* A 16 bit absolute relocation.  */
  UBICOM32_HOWTO (R_UBICOM32_16, 0, 1, 16, FALSE, 0,	"R_UBICOM32_16", 0, 0xffff),

  /* A 32 bit absolute relocation.  Must zero top byte of virtual address. */
  UBICOM32_HOWTO (R_UBICOM32_32, 0, 2, 32, FALSE, 0, "R_UBICOM32_32", 0, 0xffffffff),

  /* A 16 bit indirect relocation, low 16 bits of 32 */
  UBICOM32_HOWTO (R_UBICOM32_LO16, 0, 2, 16, FALSE, 0, "R_UBICOM32_LO16", 0x0, 0x0000ffff),

  /* A 16 bit indirect relocation, high 16 bits of 32 - must zero top byte of virtual address */
  UBICOM32_HOWTO (R_UBICOM32_HI16, 0, 2, 16, FALSE, 0, "R_UBICOM32_HI16", 0x0, 0x0000ffff),

  /* A 21 bit relative relocation.  */
  UBICOM32_HOWTO (R_UBICOM32_21_PCREL, 2, 2, 21, TRUE, 0, "R_UBICOM32_21_PCREL", 0x0, 0x001fffff),

  /* A 24 bit relative relocation.  */
  UBICOM32_HOWTO (R_UBICOM32_24_PCREL, 2, 2, 24, TRUE, 0, "R_UBICOM32_24_PCREL", 0x0, 0x071fffff),

  /* A 24 bit indirect relocation, bits 31:7 - assume top byte zero. */
  UBICOM32_HOWTO (R_UBICOM32_HI24, 7, 2, 24, FALSE, 0, "R_UBICOM32_HI24", 0x0, 0x0001ffff),

  /* A source operand low 7 bit indirect relocation. */
  UBICOM32_HOWTO (R_UBICOM32_LO7_S, 0, 2, 7, FALSE, 0, "R_UBICOM32_LO7_S", 0x0, 0x0000031f),

  /* A source operand low 7 bit .2 insn indirect relocation. */
  UBICOM32_HOWTO (R_UBICOM32_LO7_2_S, 1, 2, 7, FALSE, 0, "R_UBICOM32_LO7_2_S", 0x0, 0x0000031f),

  /* A source operand low 7 bit .4 insn indirect relocation. */
  UBICOM32_HOWTO (R_UBICOM32_LO7_4_S, 2, 2, 7, FALSE, 0, "R_UBICOM32_LO7_4_S", 0x0, 0x0000031f),

  /* A destination operand low 7 bit indirect relocation. */
  UBICOM32_HOWTO (R_UBICOM32_LO7_D, 0, 2, 7, FALSE, 0, "R_UBICOM32_LO7_D", 0x0, 0x031f0000),

  /* A destination operand low 7 bit .2 insn indirect relocation. */
  UBICOM32_HOWTO (R_UBICOM32_LO7_2_D, 1, 2, 7, FALSE, 0, "R_UBICOM32_LO7_2_D", 0x0, 0x031f0000),

  /* A destination operand low 7 bit .2 insn indirect relocation. */
  UBICOM32_HOWTO (R_UBICOM32_LO7_4_D, 2, 2, 7, FALSE, 0, "R_UBICOM32_LO7_4_D", 0x0, 0x031f0000),

  /* A 32 bit absolute relocation in debug section.  Must retain top byte of virtual address. */
  UBICOM32_HOWTO (R_UBICOM32_32_HARVARD, 0, 2, 32, FALSE, 0, "R_UBICOM32_32_HARVARD", 0, 0xffffffff),

  /* A calli offset operand low 7 bit .4 insn indirect relocation. */
  UBICOM32_HOWTO (R_UBICOM32_LO7_CALLI, 2, 2, 7, FALSE, 0, "R_UBICOM32_LO7_CALLI", 0x0, 0x071f071f),

  /* A calli offset operand low 18 bit .4 insn indirect relocation. */
  UBICOM32_HOWTO (R_UBICOM32_LO16_CALLI, 2, 2, 16, FALSE, 0, "R_UBICOM32_LO16_CALLI", 0x0, 0x071f071f),

  /* A 24 bit indirect relocation, bits 31:7 - assume top byte zero. */
  UBICOM32_HOWTO (R_UBICOM32_GOT_HI24, 7, 2, 24, FALSE, 0, "R_UBICOM32_GOT_HI24", 0x0, 0x0001ffff),

  /* A source operand low 7 bit indirect relocation. */
  UBICOM32_HOWTO (R_UBICOM32_GOT_LO7_S, 0, 2, 7, FALSE, 0, "R_UBICOM32_GOT_LO7_S", 0x0, 0x0000031f),

  /* A source operand low 7 bit .2 insn indirect relocation. */
  UBICOM32_HOWTO (R_UBICOM32_GOT_LO7_2_S, 1, 2, 7, FALSE, 0, "R_UBICOM32_GOT_LO7_2_S", 0x0, 0x0000031f),

  /* A source operand low 7 bit .4 insn indirect relocation. */
  UBICOM32_HOWTO (R_UBICOM32_GOT_LO7_4_S, 2, 2, 7, FALSE, 0, "R_UBICOM32_GOT_LO7_4_S", 0x0, 0x0000031f),

  /* A destination operand low 7 bit indirect relocation. */
  UBICOM32_HOWTO (R_UBICOM32_GOT_LO7_D, 0, 2, 7, FALSE, 0, "R_UBICOM32_GOT_LO7_D", 0x0, 0x031f0000),

  /* A destination operand low 7 bit .2 insn indirect relocation. */
  UBICOM32_HOWTO (R_UBICOM32_GOT_LO7_2_D, 1, 2, 7, FALSE, 0, "R_UBICOM32_GOT_LO7_2_D", 0x0, 0x031f0000),

  /* A destination operand low 7 bit .2 insn indirect relocation. */
  UBICOM32_HOWTO (R_UBICOM32_GOT_LO7_4_D, 2, 2, 7, FALSE, 0, "R_UBICOM32_GOT_LO7_4_D", 0x0, 0x031f0000),

  /* A 24 bit indirect relocation, bits 31:7 - assume top byte zero. */
  UBICOM32_HOWTO (R_UBICOM32_FUNCDESC_GOT_HI24, 7, 2, 24, FALSE, 0, "R_UBICOM32_FUNCDESC_GOT_HI24", 0x0, 0x0001ffff),

  /* A source operand low 7 bit indirect relocation. */
  UBICOM32_HOWTO (R_UBICOM32_FUNCDESC_GOT_LO7_S, 0, 2, 7, FALSE, 0, "R_UBICOM32_FUNCDESC_GOT_LO7_S", 0x0, 0x0000031f),

  /* A source operand low 7 bit .2 insn indirect relocation. */
  UBICOM32_HOWTO (R_UBICOM32_FUNCDESC_GOT_LO7_2_S, 1, 2, 7, FALSE, 0, "R_UBICOM32_FUNCDESC_GOT_LO7_2_S", 0x0, 0x0000031f),

  /* A source operand low 7 bit .4 insn indirect relocation. */
  UBICOM32_HOWTO (R_UBICOM32_FUNCDESC_GOT_LO7_4_S, 2, 2, 7, FALSE, 0, "R_UBICOM32_FUNCDESC_GOT_LO7_4_S", 0x0, 0x0000031f),

  /* A destination operand low 7 bit indirect relocation. */
  UBICOM32_HOWTO (R_UBICOM32_FUNCDESC_GOT_LO7_D, 0, 2, 7, FALSE, 0, "R_UBICOM32_FUNCDESC_GOT_LO7_D", 0x0, 0x031f0000),

  /* A destination operand low 7 bit .2 insn indirect relocation. */
  UBICOM32_HOWTO (R_UBICOM32_FUNCDESC_GOT_LO7_2_D, 1, 2, 7, FALSE, 0, "R_UBICOM32_FUNCDESC_GOT_LO7_2_D", 0x0, 0x031f0000),

  /* A destination operand low 7 bit .2 insn indirect relocation. */
  UBICOM32_HOWTO (R_UBICOM32_FUNCDESC_GOT_LO7_4_D, 2, 2, 7, FALSE, 0, "R_UBICOM32_FUNCDESC_GOT_LO7_4_D", 0x0, 0x031f0000),

  /* A calli offset operand low 7 bit .4 insn indirect relocation. */
  UBICOM32_HOWTO (R_UBICOM32_GOT_LO7_CALLI, 2, 2, 7, FALSE, 0, "R_UBICOM32_GOT_LO7_CALLI", 0x0, 0x071f071f),

  /* A calli offset operand low 7 bit .4 insn indirect relocation. */
  UBICOM32_HOWTO (R_UBICOM32_FUNCDESC_GOT_LO7_CALLI, 2, 2, 7, FALSE, 0, "R_UBICOM32_FUNCDESC_GOT_LO7_CALLI", 0x0, 0x071f071f),

  /* A 32 bit absolute relocation.  Must zero top byte of virtual address. */
  UBICOM32_HOWTO (R_UBICOM32_FUNCDESC_VALUE, 0, 2, 32, FALSE, 0, "R_UBICOM32_FUNCDESC_VALUE", 0, 0xffffffff),

  /* A 32 bit absolute relocation.  Must zero top byte of virtual address. */
  UBICOM32_HOWTO (R_UBICOM32_FUNCDESC, 0, 2, 32, FALSE, 0, "R_UBICOM32_FUNCDESC", 0, 0xffffffff),

  /* A 16 bit absolute relocation.  */
  UBICOM32_HOWTO (R_UBICOM32_GOT_LO, 0, 1, 16, FALSE, 0, "R_UBICOM32_GOT_LO", 0, 0xffff),

  /* A 16 bit absolute relocation.  */
  UBICOM32_HOWTO (R_UBICOM32_GOT_HI, 0, 1, 16, FALSE, 0, "R_UBICOM32_GOT_HI", 0, 0xffff),

  /* A 16 bit absolute relocation.  */
  UBICOM32_HOWTO (R_UBICOM32_FUNCDESC_GOT_LO, 0, 1, 16, FALSE, 0, "R_UBICOM32_FUNCDESC_GOT_LO", 0, 0xffff),

  /* A 16 bit absolute relocation.  */
  UBICOM32_HOWTO (R_UBICOM32_FUNCDESC_GOT_HI, 0, 1, 16, FALSE, 0, "R_UBICOM32_FUNCDESC_GOT_HI", 0, 0xffff),

  /* A leai 16 bit offset operand */
  UBICOM32_HOWTO (R_UBICOM32_LO7_LEAI, 2, 2, 7, FALSE, 0, "R_UBICOM32_LO7_LEAI", 0x0, 0x071f071f),

  /* A leai 16 bit offset operand */
  UBICOM32_HOWTO (R_UBICOM32_LO16_LEAI, 2, 2, 16, FALSE, 0, "R_UBICOM32_LO16_LEAI", 0x0, 0x071f071f),

  /* A leai 16 bit offset operand */
  UBICOM32_HOWTO (R_UBICOM32_GOT_LO_LEAI, 0, 0, 16, FALSE, 0, "R_UBICOM32_GOT_LO_LEAI", 0x0, 0x071f071f),

  /* A leai 16 bit offset operand */
  UBICOM32_HOWTO (R_UBICOM32_FUNCDESC_GOT_LO_LEAI, 0, 0, 16, FALSE, 0, "R_UBICOM32_FUNCDESC_GOT_LO_LEAI", 0x0, 0x071f071f),
};

/* GNU extension to record C++ vtable hierarchy */
static reloc_howto_type ubicom32_elf_vtinherit_howto =
  HOWTO (R_UBICOM32_GNU_VTINHERIT,   /* type */
	 0,                     /* rightshift */
	 2,                     /* size (0 = byte, 1 = short, 2 = long) */
	 0,                     /* bitsize */
	 FALSE,                 /* pc_relative */
	 0,                     /* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 NULL,                  /* special_function */
	 "R_UBICOM32_GNU_VTINHERIT", /* name */
	 FALSE,                 /* partial_inplace */
	 0,                     /* src_mask */
	 0,                     /* dst_mask */
	 FALSE);                /* pcrel_offset */

  /* GNU extension to record C++ vtable member usage */
static reloc_howto_type ubicom32_elf_vtentry_howto =
  HOWTO (R_UBICOM32_GNU_VTENTRY,     /* type */
	 0,                     /* rightshift */
	 2,                     /* size (0 = byte, 1 = short, 2 = long) */
	 0,                     /* bitsize */
	 FALSE,                 /* pc_relative */
	 0,                     /* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_elf_rel_vtable_reloc_fn,  /* special_function */
	 "R_UBICOM32_GNU_VTENTRY",   /* name */
	 FALSE,                 /* partial_inplace */
	 0,                     /* src_mask */
	 0,                     /* dst_mask */
	 FALSE);                /* pcrel_offset */

extern const bfd_target bfd_elf32_ubicom32fdpic_vec;
#define IS_FDPIC(bfd) ((bfd)->xvec == &bfd_elf32_ubicom32fdpic_vec)

/* Relocation helpers */
bfd_reloc_status_type
ubicom32_elf_generic_reloc (abfd,
			    reloc_entry,
			    symbol,
			    data,
			    input_section,
			    output_bfd,
			    error_message)
     bfd *abfd ATTRIBUTE_UNUSED;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data ATTRIBUTE_UNUSED;
     asection *input_section;
     bfd *output_bfd;
     char **error_message ATTRIBUTE_UNUSED;
{
  if (output_bfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && (! reloc_entry->howto->partial_inplace
	  || reloc_entry->addend == 0))
    {
      reloc_entry->address += input_section->output_offset;
      symbol = *reloc_entry->sym_ptr_ptr;

      if((symbol->flags & BSF_OBJECT) == 0)
	{
	  reloc_entry->addend -= symbol->value;
	}
      else
	{
	  if (reloc_entry->addend >= symbol->value)
	    reloc_entry->addend -= symbol->value;
	}
      return bfd_reloc_ok;
    }

  return bfd_reloc_continue;
}

bfd_reloc_status_type
ubicom32_elf_relocate_hi16 (input_bfd, relhi, contents, value)
     bfd *input_bfd;
     Elf_Internal_Rela *relhi;
     bfd_byte *contents;
     bfd_vma value;
{
  bfd_vma insn;

  insn = bfd_get_32 (input_bfd, contents + relhi->r_offset);

  value += relhi->r_addend;
  value >>= 16;
  value &= 0xffff;  /* take off top byte of virtual address */
  insn = ((insn & ~0xFFFF) | value);

  bfd_put_32 (input_bfd, insn, contents + relhi->r_offset);
  return bfd_reloc_ok;
}

bfd_reloc_status_type
ubicom32_elf_relocate_lo16 (input_bfd, relhi, contents, value)
     bfd *input_bfd;
     Elf_Internal_Rela *relhi;
     bfd_byte *contents;
     bfd_vma value;
{
  bfd_vma insn;

  insn = bfd_get_32 (input_bfd, contents + relhi->r_offset);

  value += relhi->r_addend;
  value &= 0xFFFF;
  insn = ((insn & ~0xFFFF) | value);

  bfd_put_32 (input_bfd, insn, contents + relhi->r_offset);
  return bfd_reloc_ok;
}

bfd_reloc_status_type
ubicom32_elf_relocate_hi24 (input_bfd, relhi, contents, value)
     bfd *input_bfd;
     Elf_Internal_Rela *relhi;
     bfd_byte *contents;
     bfd_vma value;
{
  bfd_vma insn;

  insn = bfd_get_32 (input_bfd, contents + relhi->r_offset);

  value += relhi->r_addend;

  if ((elf_elfheader (input_bfd)->e_flags & 0xffff) < bfd_mach_ubicom32ver5)
    {
      /* We are dealing with Ubicom cpus that are version 4 or earlier. The moveai instruction assumes that the msb of the address is zero. */
      if (value & 0x80000000)
	fprintf (stderr,"@@@: You are trying load the address of something at %08lx\n  This is >= 0x80000000 and the moveai instruction does not support it!\n",value);

      value &= 0x7fffffff; /* zero off top bit of virtual address */
      value >>= 7;
      insn = (insn & ~0x071FFFFF);

      insn |= (value & 0x1FFFFF);
      insn |= (value & 0xe00000) << 3;
    }
  else
    {
      /* Cpu versions 5+. The address value is 25 bits in width. */
      value >>= 7;
      insn = (insn & ~0x0f1FFFFF);

      insn |= (value & 0x1FFFFF);
      insn |= (value & 0x1e00000) << 3;
    }

  bfd_put_32 (input_bfd, insn, contents + relhi->r_offset);
  return bfd_reloc_ok;
}

bfd_reloc_status_type
ubicom32_elf_relocate_lo7_s (input_bfd, relhi, contents, value)
     bfd *input_bfd;
     Elf_Internal_Rela *relhi;
     bfd_byte *contents;
     bfd_vma value;
{
  bfd_vma insn;
  bfd_vma top;
  bfd_vma bottom;

  insn = bfd_get_32 (input_bfd, contents + relhi->r_offset);

  value += relhi->r_addend;
  value &= 0x7f;

  /* must split up value into top 2 bits and bottom 5 bits */
  top = value >> 5;
  bottom = value & 0x1f;
  insn = ((insn & ~0x31f) | (top << 8) | bottom);

  bfd_put_32 (input_bfd, insn, contents + relhi->r_offset);
  return bfd_reloc_ok;
}

bfd_reloc_status_type
ubicom32_elf_relocate_lo7_2_s (input_bfd, relhi, contents, value)
     bfd *input_bfd;
     Elf_Internal_Rela *relhi;
     bfd_byte *contents;
     bfd_vma value;
{
  bfd_vma insn;
  bfd_vma top;
  bfd_vma bottom;

  insn = bfd_get_32 (input_bfd, contents + relhi->r_offset);

  value += relhi->r_addend;
  value &= 0x7f;
  value >>= 1;  /* must shift by 1 because this is .2 insn */

  /* must split up value into top 2 bits and bottom 5 bits */
  top = value >> 5;
  bottom = value & 0x1f;
  insn = ((insn & ~0x31f) | (top << 8) | bottom);

  bfd_put_32 (input_bfd, insn, contents + relhi->r_offset);
  return bfd_reloc_ok;
}

bfd_reloc_status_type
ubicom32_elf_relocate_lo7_4_s (input_bfd, relhi, contents, value)
     bfd *input_bfd;
     Elf_Internal_Rela *relhi;
     bfd_byte *contents;
     bfd_vma value;
{
  bfd_vma insn;
  bfd_vma top;
  bfd_vma bottom;

  insn = bfd_get_32 (input_bfd, contents + relhi->r_offset);

  value += relhi->r_addend;
  value &= 0x7f;
  value >>= 2;  /* must shift by 1 because this is .4 insn */

  /* must split up value into top 2 bits and bottom 5 bits */
  top = value >> 5;
  bottom = value & 0x1f;
  insn = ((insn & ~0x31f) | (top << 8) | bottom);

  bfd_put_32 (input_bfd, insn, contents + relhi->r_offset);
  return bfd_reloc_ok;
}

bfd_reloc_status_type
ubicom32_elf_relocate_lo7_d (input_bfd, relhi, contents, value)
     bfd *input_bfd;
     Elf_Internal_Rela *relhi;
     bfd_byte *contents;
     bfd_vma value;
{
  bfd_vma insn;
  bfd_vma top;
  bfd_vma bottom;

  insn = bfd_get_32 (input_bfd, contents + relhi->r_offset);

  value += relhi->r_addend;
  value &= 0x7f;

  /* must split up value into top 2 bits and bottom 5 bits */
  top = value >> 5;
  bottom = value & 0x1f;
  insn = ((insn & ~0x031f0000) | (top << 24) | (bottom << 16));

  bfd_put_32 (input_bfd, insn, contents + relhi->r_offset);
  return bfd_reloc_ok;
}

bfd_reloc_status_type
ubicom32_elf_relocate_lo7_2_d (input_bfd, relhi, contents, value)
     bfd *input_bfd;
     Elf_Internal_Rela *relhi;
     bfd_byte *contents;
     bfd_vma value;
{
  bfd_vma insn;
  bfd_vma top;
  bfd_vma bottom;

  insn = bfd_get_32 (input_bfd, contents + relhi->r_offset);

  value += relhi->r_addend;
  value &= 0x7f;
  value >>= 1; /* must shift by 1 because this is for a .2 insn */

  /* must split up value into top 2 bits and bottom 5 bits */
  top = value >> 5;
  bottom = value & 0x1f;
  insn = ((insn & ~0x031f0000) | (top << 24) | (bottom << 16));

  bfd_put_32 (input_bfd, insn, contents + relhi->r_offset);
  return bfd_reloc_ok;
}

bfd_reloc_status_type
ubicom32_elf_relocate_lo7_4_d (input_bfd, relhi, contents, value)
     bfd *input_bfd;
     Elf_Internal_Rela *relhi;
     bfd_byte *contents;
     bfd_vma value;
{
  bfd_vma insn;
  bfd_vma top;
  bfd_vma bottom;

  insn = bfd_get_32 (input_bfd, contents + relhi->r_offset);

  value += relhi->r_addend;
  value &= 0x7f;
  value >>= 2; /* must shift by 2 because this is for a .4 insn */

  /* must split up value into top 2 bits and bottom 5 bits */
  top = value >> 5;
  bottom = value & 0x1f;
  insn = ((insn & ~0x031f0000) | (top << 24) | (bottom << 16));

  bfd_put_32 (input_bfd, insn, contents + relhi->r_offset);
  return bfd_reloc_ok;
}

/* Perform the relocation for call instructions */
static bfd_reloc_status_type
ubicom32_elf_relocate_pcrel24 (input_bfd, input_section, rello, contents, value)
     bfd *input_bfd;
     asection *input_section;
     Elf_Internal_Rela *rello;
     bfd_byte *contents;
     bfd_vma value;
{
  bfd_vma insn;
  bfd_vma value_top;
  bfd_vma value_bottom;

  /* Grab the instruction */
  insn = bfd_get_32 (input_bfd, contents + rello->r_offset);

  value -= input_section->output_section->vma + input_section->output_offset;
  value -= rello->r_offset;
  value += rello->r_addend;

  /* insn uses bottom 24 bits of relocation value times 4 */
  if (value & 0x03)
    return bfd_reloc_dangerous;

  value = (value & 0x3ffffff) >> 2;

  if ((long) value > 0xffffff)
    return bfd_reloc_overflow;

  value_top = (value >> 21) << 24;
  value_bottom = value & 0x1fffff;

  insn = insn & 0xf8e00000;
  insn = insn | value_top | value_bottom;

  bfd_put_32 (input_bfd, insn, contents + rello->r_offset);

  return bfd_reloc_ok;
}

static bfd_reloc_status_type
ubicom32_elf_relocate_got_lo (input_bfd, input_section, rello, contents, value)
     bfd *input_bfd;
     asection *input_section ATTRIBUTE_UNUSED;
     Elf_Internal_Rela *rello;
     bfd_byte *contents;
     bfd_vma value;
{
  bfd_vma insn;

  /* Grab the instruction */
  insn = bfd_get_32 (input_bfd, contents + rello->r_offset);

  /* Truncte to 16 and store. */
  value &= 0xffff;

  insn = (insn & 0xffff0000) | value;

  /* output it. */
  bfd_put_32 (input_bfd, insn, contents + rello->r_offset);

  return bfd_reloc_ok;
}

static bfd_reloc_status_type
ubicom32_elf_relocate_funcdesc_got_lo (input_bfd, input_section, rello, contents, value)
     bfd *input_bfd;
     asection *input_section ATTRIBUTE_UNUSED;
     Elf_Internal_Rela *rello;
     bfd_byte *contents;
     bfd_vma value;
{
  bfd_vma insn;

  /* Grab the instruction */
  insn = bfd_get_32 (input_bfd, contents + rello->r_offset);

  /* Truncte to 16 and store. */
  value &= 0xffff;

  insn = (insn & 0xffff0000) | value;

  /* output it. */
  bfd_put_32 (input_bfd, insn, contents + rello->r_offset);

  return bfd_reloc_ok;
}
#if 0
static bfd_reloc_status_type
ubicom32_elf_relocate_funcdesc (input_bfd, input_section, rello, contents, value)
     bfd *input_bfd;
     asection *input_section ATTRIBUTE_UNUSED;
     Elf_Internal_Rela *rello;
     bfd_byte *contents;
     bfd_vma value;
{
  bfd_vma insn;

  /* Grab the instruction */
  insn = bfd_get_32 (input_bfd, contents + rello->r_offset);

  /* Truncte to 16 and store. */
  value &= 0xffff;

  insn = (insn & 0xffff0000) | value;

  /* output it. */
  bfd_put_32 (input_bfd, insn, contents + rello->r_offset);

  return bfd_reloc_ok;
}
#endif
bfd_reloc_status_type
ubicom32_elf_relocate_lo_calli (input_bfd, relhi, contents, value, bits)
     bfd *input_bfd;
     Elf_Internal_Rela *relhi;
     bfd_byte *contents;
     bfd_vma value;
     int bits;
{
  bfd_vma insn;

  insn = bfd_get_32 (input_bfd, contents + relhi->r_offset);

  value += relhi->r_addend;
  value &= (1 << bits) - 1;
  value >>= 2;  /* must shift by 2 because this is .4 insn */

  /* must split up value into top 2 bits and bottom 5 bits */
  insn &= ~0x071f071f;
  insn |= (value & 0x1f) << 0;
  value >>= 5;
  insn |= (value & 0x07) << 8;
  value >>= 3;
  insn |= (value & 0x1f) << 16;
  value >>= 5;
  insn |= (value & 0x07) << 24;

  bfd_put_32 (input_bfd, insn, contents + relhi->r_offset);
  return bfd_reloc_ok;
}

bfd_reloc_status_type
ubicom32_elf_relocate_lo_leai (input_bfd, relhi, contents, value, bits)
     bfd *input_bfd;
     Elf_Internal_Rela *relhi;
     bfd_byte *contents;
     bfd_vma value;
     int bits;
{
  bfd_vma insn;

  insn = bfd_get_32 (input_bfd, contents + relhi->r_offset);

  value += relhi->r_addend;
  value &= (1 << bits) - 1;
  value >>= 2;  /* must shift by 2 because this is a .4 insn */

  /* must split up value into 4 bit fields bits */
  insn &= ~0x071f071f;
  insn |= (value & 0x1f) << 0;
  value >>= 5;
  insn |= (value & 0x07) << 8;
  value >>= 3;
  insn |= (value & 0x1f) << 16;
  value >>= 5;
  insn |= (value & 0x07) << 24;

  bfd_put_32 (input_bfd, insn, contents + relhi->r_offset);
  return bfd_reloc_ok;
}

bfd_reloc_status_type
ubicom32_elf_relocate_got_lo_leai (input_bfd, relhi, contents, value, bits)
     bfd *input_bfd;
     Elf_Internal_Rela *relhi;
     bfd_byte *contents;
     bfd_vma value;
     int bits;
{
  bfd_vma insn;

  insn = bfd_get_32 (input_bfd, contents + relhi->r_offset);

  value &= (1 << bits) - 1;
  value >>= 2;  /* must shift by 2 because this is a .4 insn */

  /* must split up value into 4 bit fields bits */
  insn &= ~0x071f071f;
  insn |= (value & 0x1f) << 0;
  value >>= 5;
  insn |= (value & 0x07) << 8;
  value >>= 3;
  insn |= (value & 0x1f) << 16;
  value >>= 5;
  insn |= (value & 0x07) << 24;

  bfd_put_32 (input_bfd, insn, contents + relhi->r_offset);
  return bfd_reloc_ok;
}

bfd_reloc_status_type
ubicom32_elf_relocate_funcdesc_got_lo_leai (input_bfd, relhi, contents, value, bits)
     bfd *input_bfd;
     Elf_Internal_Rela *relhi;
     bfd_byte *contents;
     bfd_vma value;
     int bits;
{
  bfd_vma insn;

  insn = bfd_get_32 (input_bfd, contents + relhi->r_offset);

  value &= (1 << bits) - 1;
  value >>= 2;  /* must shift by 2 because this is a .4 insn */

  /* must split up value into 4 bit fields bits */
  insn &= ~0x071f071f;
  insn |= (value & 0x1f) << 0;
  value >>= 5;
  insn |= (value & 0x07) << 8;
  value >>= 3;
  insn |= (value & 0x1f) << 16;
  value >>= 5;
  insn |= (value & 0x07) << 24;

  bfd_put_32 (input_bfd, insn, contents + relhi->r_offset);
  return bfd_reloc_ok;
}


/* Set the howto pointer for a UBICOM32 ELF reloc.  */

static void
ubicom32_info_to_howto_rela (abfd, cache_ptr, dst)
     bfd * abfd ATTRIBUTE_UNUSED;
     arelent * cache_ptr;
     Elf_Internal_Rela * dst;
{
  unsigned int r_type;

  r_type = ELF32_R_TYPE (dst->r_info);
  switch (r_type)
    {
    case R_UBICOM32_GNU_VTINHERIT:
      cache_ptr->howto = &ubicom32_elf_vtinherit_howto;
      break;

    case R_UBICOM32_GNU_VTENTRY:
      cache_ptr->howto = &ubicom32_elf_vtentry_howto;
      break;

    default:
      cache_ptr->howto = &ubicom32_elf_howto_table[r_type];
      break;
    }
}


static reloc_howto_type *
ubicom32_reloc_type_lookup (abfd, code)
     bfd * abfd ATTRIBUTE_UNUSED;
     bfd_reloc_code_real_type code;
{
  switch (code)
    {
    case BFD_RELOC_NONE:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_NONE];

    case BFD_RELOC_16:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_16];

    case BFD_RELOC_32:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_32];

    case BFD_RELOC_LO16:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_LO16];

    case BFD_RELOC_HI16:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_HI16];

    case BFD_RELOC_UBICOM32_HI24:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_HI24];

    case BFD_RELOC_UBICOM32_LO7_S:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_LO7_S];

    case BFD_RELOC_UBICOM32_LO7_2_S:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_LO7_2_S];

    case BFD_RELOC_UBICOM32_LO7_4_S:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_LO7_4_S];

    case BFD_RELOC_UBICOM32_LO7_D:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_LO7_D];

    case BFD_RELOC_UBICOM32_LO7_2_D:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_LO7_2_D];

    case BFD_RELOC_UBICOM32_LO7_4_D:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_LO7_4_D];

    case BFD_RELOC_UBICOM32_21_PCREL:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_21_PCREL];

    case BFD_RELOC_UBICOM32_24_PCREL:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_24_PCREL];

    case BFD_RELOC_UBICOM32_GOT_HI24:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_GOT_HI24];

    case BFD_RELOC_UBICOM32_GOT_LO7_S:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_GOT_LO7_S];

    case BFD_RELOC_UBICOM32_GOT_LO7_2_S:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_GOT_LO7_2_S];

    case BFD_RELOC_UBICOM32_GOT_LO7_4_S:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_GOT_LO7_4_S];

    case BFD_RELOC_UBICOM32_GOT_LO7_D:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_GOT_LO7_D];

    case BFD_RELOC_UBICOM32_GOT_LO7_2_D:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_GOT_LO7_2_D];

    case BFD_RELOC_UBICOM32_GOT_LO7_4_D:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_GOT_LO7_4_D];

    case BFD_RELOC_UBICOM32_FUNCDESC_GOT_HI24:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_FUNCDESC_GOT_HI24];

    case BFD_RELOC_UBICOM32_FUNCDESC_GOT_LO7_S:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_FUNCDESC_GOT_LO7_S];

    case BFD_RELOC_UBICOM32_FUNCDESC_GOT_LO7_2_S:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_FUNCDESC_GOT_LO7_2_S];

    case BFD_RELOC_UBICOM32_FUNCDESC_GOT_LO7_4_S:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_FUNCDESC_GOT_LO7_4_S];

    case BFD_RELOC_UBICOM32_FUNCDESC_GOT_LO7_D:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_FUNCDESC_GOT_LO7_D];

    case BFD_RELOC_UBICOM32_FUNCDESC_GOT_LO7_2_D:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_FUNCDESC_GOT_LO7_2_D];

    case BFD_RELOC_UBICOM32_FUNCDESC_GOT_LO7_4_D:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_FUNCDESC_GOT_LO7_4_D];

    case BFD_RELOC_UBICOM32_LO7_CALLI:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_LO7_CALLI];

    case BFD_RELOC_UBICOM32_GOT_LO7_CALLI:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_GOT_LO7_CALLI];

    case BFD_RELOC_UBICOM32_FUNCDESC_GOT_LO7_CALLI:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_FUNCDESC_GOT_LO7_CALLI];

    case BFD_RELOC_UBICOM32_LO16_CALLI:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_LO16_CALLI];

    case BFD_RELOC_UBICOM32_FUNCDESC_VALUE:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_FUNCDESC_VALUE];

    case BFD_RELOC_UBICOM32_FUNCDESC:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_FUNCDESC];

    case BFD_RELOC_UBICOM32_GOT_LO:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_GOT_LO];

    case BFD_RELOC_UBICOM32_GOT_HI:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_GOT_HI];

    case BFD_RELOC_UBICOM32_FUNCDESC_GOT_LO:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_FUNCDESC_GOT_LO];

    case BFD_RELOC_UBICOM32_FUNCDESC_GOT_HI:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_FUNCDESC_GOT_HI];

    case BFD_RELOC_VTABLE_INHERIT:
      return &ubicom32_elf_vtinherit_howto;

    case BFD_RELOC_VTABLE_ENTRY:
      return &ubicom32_elf_vtentry_howto;

    case BFD_RELOC_UBICOM32_LO7_LEAI:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_LO7_LEAI];

    case BFD_RELOC_UBICOM32_LO16_LEAI:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_LO16_LEAI];

    case BFD_RELOC_UBICOM32_GOT_LO_LEAI:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_GOT_LO_LEAI];

    case BFD_RELOC_UBICOM32_FUNCDESC_GOT_LO_LEAI:
      return &ubicom32_elf_howto_table[(int)R_UBICOM32_FUNCDESC_GOT_LO_LEAI];

    default:
      /* Pacify gcc -Wall. */
      return NULL;
    }

  return NULL;
}

static reloc_howto_type *
ubicom32_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED,
			    const char *r_name)
{
  unsigned int i;

  for (i = 0;
       i < (sizeof (ubicom32_elf_howto_table)
	    / sizeof (ubicom32_elf_howto_table[0]));
       i++)
    if (ubicom32_elf_howto_table[i].name != NULL
	&& strcasecmp (ubicom32_elf_howto_table[i].name, r_name) == 0)
      return &ubicom32_elf_howto_table[i];

  return NULL;
}

/* Return the value of the symbol associated with the relocation IREL.  */

static bfd_vma
symbol_value (abfd, irel)
     bfd *abfd;
     Elf_Internal_Rela *irel;
{
  Elf_Internal_Shdr *symtab_hdr = file_symtab_hdr (abfd);
  Elf_Internal_Sym *isymbuf = file_isymbuf (abfd);

  if (ELF32_R_SYM (irel->r_info) < symtab_hdr->sh_info)
    {
      Elf_Internal_Sym *isym;
      asection *sym_sec;

      isym = isymbuf + ELF32_R_SYM (irel->r_info);
      if (isym->st_shndx == SHN_UNDEF)
	sym_sec = bfd_und_section_ptr;
      else if (isym->st_shndx == SHN_ABS)
	sym_sec = bfd_abs_section_ptr;
      else if (isym->st_shndx == SHN_COMMON)
	sym_sec = bfd_com_section_ptr;
      else
	sym_sec = bfd_section_from_elf_index (abfd, isym->st_shndx);

      return isym->st_value + BASEADDR (sym_sec);
    }
  else
    {
      unsigned long indx;
      struct elf_link_hash_entry *h;

      indx = ELF32_R_SYM (irel->r_info) - symtab_hdr->sh_info;
      h = elf_sym_hashes (abfd)[indx];
      BFD_ASSERT (h != NULL);

      if (h->root.type != bfd_link_hash_defined
	  && h->root.type != bfd_link_hash_defweak)
	return UNDEFINED_SYMBOL;

      return (h->root.u.def.value + BASEADDR (h->root.u.def.section));
    }
}


static Elf_Internal_Shdr *
file_symtab_hdr (abfd)
     bfd *abfd;
{
  return &elf_tdata (abfd)->symtab_hdr;
}

static Elf_Internal_Sym *
file_isymbuf (abfd)
     bfd *abfd;
{
  Elf_Internal_Shdr *symtab_hdr;

  symtab_hdr = file_symtab_hdr (abfd);
  if (symtab_hdr->sh_info == 0)
    return NULL;

  if (symtab_hdr->contents == NULL)
    {
      Elf_Internal_Sym * contents = bfd_elf_get_elf_syms (abfd, symtab_hdr, symtab_hdr->sh_info, 0,
							  NULL, NULL, NULL);
      symtab_hdr->contents = (unsigned char *) contents;
    }

  return (Elf_Internal_Sym *) symtab_hdr->contents;
}

static Elf_Internal_Rela *
section_relocs (abfd, sec)
     bfd *abfd;
     asection *sec;
{
  if ((sec->flags & SEC_RELOC) == 0)
    return NULL;

  if (sec->reloc_count == 0)
    return NULL;

  if (elf_section_data (sec)->relocs == NULL)
    elf_section_data (sec)->relocs =
      _bfd_elf_link_read_relocs (abfd, sec, NULL, NULL, 1);

  return elf_section_data (sec)->relocs;
}

static bfd_byte *
section_contents (abfd, sec)
     bfd *abfd;
     asection *sec;
{
  bfd_byte *contents;

  sec->rawsize = sec->rawsize ? sec->rawsize: sec->size;

  if (elf_section_data (sec)->this_hdr.contents)
    return elf_section_data (sec)->this_hdr.contents;

  contents = (bfd_byte *) bfd_malloc (sec->rawsize);
  if (contents == NULL)
    return NULL;

  if (! bfd_get_section_contents (abfd, sec, contents,
				  (file_ptr) 0, sec->rawsize))
    {
      free (contents);
      return NULL;
    }

  elf_section_data (sec)->this_hdr.contents = contents;
  return contents;
}

/* This function handles relaxing for the ubicom32.

   Principle: Start with the first page and remove page instructions that
   are not require on this first page. By removing page instructions more
   code will fit into this page - repeat until nothing more can be achieved
   for this page. Move on to the next page.

   Processing the pages one at a time from the lowest page allows a removal
   only policy to be used - pages can be removed but are never reinserted.  */

static bfd_boolean
ubicom32_elf_relax_section (abfd, sec, link_info, again)
     bfd *abfd;
     asection *sec;
     struct bfd_link_info *link_info;
     bfd_boolean *again;
{
  /* Assume nothing changes.  */
  *again = FALSE;

  /* We don't have to do anything for a relocatable link,
     if this section does not have relocs, or if this is
     not a code section.  */
  if (link_info->relocatable
      || (sec->flags & SEC_RELOC) == 0
      || sec->reloc_count == 0
      || (sec->flags & SEC_CODE) == 0)
    return TRUE;

  /* If this is the first time we have been called
      for this section, initialise the cooked size.
  if (sec->_cooked_size == 0)
    sec->_cooked_size = sec->rawsize;
  */

  /* This is where all the relaxation actually get done.  */
  if (!ubicom32_elf_relax_calli (abfd, sec, again))
    return FALSE;

  if (sec->rawsize != sec->size)
    sec->size = sec->rawsize;

  /* Success!  */
  return TRUE;
}

static bfd_boolean
ubicom32_elf_relax_calli (abfd, sec, again)
     bfd *abfd;
     asection *sec;
     bfd_boolean *again;
{
  bfd_byte *contents = section_contents (abfd, sec);
  Elf_Internal_Rela *irelbase = section_relocs (abfd, sec);
  Elf_Internal_Rela *irelend = irelbase + sec->reloc_count;
  Elf_Internal_Rela *irel_moveai = NULL;
  Elf_Internal_Rela *irel;
  unsigned long insn;
  bfd_vma symval;
  bfd_vma pc;
  bfd_vma dest;
  signed long offs;

  /* Walk thru the section looking for relaxation opertunities.  */
  for (irel = irelbase; irel < irelend; irel++)
    {
      /* Remember last moveai instruction */
      if (ELF32_R_TYPE (irel->r_info) == (int) R_UBICOM32_HI24)
	{
	  irel_moveai = irel;
	  continue;
	}

      /* Ignore non calli instructions */
      if (ELF32_R_TYPE (irel->r_info) != (int) R_UBICOM32_LO7_CALLI)
	continue;

      /* calli instruction => verify it is a calli instruction
	 using a5 with a 5 bit positive offset */
      insn = bfd_get_32 (abfd, (bfd_byte *)(contents + irel->r_offset));
      if ((insn & 0xffffffe0) != 0xf0a000a0)
	continue;
      symval = symbol_value (abfd, irel);
      if (symval == UNDEFINED_SYMBOL)
	continue;
      dest = symval + irel->r_addend;

      /* Check proceeding instruction for a valid moveai */
      if (!irel_moveai)
	continue;
      if (irel_moveai->r_offset != (irel->r_offset - 4))
	continue;
      insn = bfd_get_32 (abfd, (bfd_byte *)(contents + irel_moveai->r_offset));
      if ((insn & 0xf8e00000) !=  0xe0a00000)
	continue;
      symval = symbol_value (abfd, irel_moveai);
      if (symval == UNDEFINED_SYMBOL)
	continue;
      symval += irel_moveai->r_addend;
      if (symval != dest)
	continue;

      /* Check offset required */
      pc = BASEADDR (sec) + irel_moveai->r_offset;
      offs = dest - pc;
      if (offs > (UBICOM32_CALL_MAX_POS_OFFS + 4))
	continue;
      if (offs < UBICOM32_CALL_MAX_NEG_OFFS)
	continue;

      /* Replace calli with a call instruction */
      irel->r_info = ELF32_R_INFO (ELF32_R_SYM (irel->r_info), R_UBICOM32_24_PCREL);
      bfd_put_32 (abfd, 0xd8a00000, contents + irel->r_offset);

      /* Delete moveai instruction */
      irel_moveai->r_info = ELF32_R_INFO (ELF32_R_SYM (irel_moveai->r_info), R_UBICOM32_NONE);
      if (!ubicom32_elf_relax_delete_bytes (abfd, sec, irel_moveai->r_offset, 4))
	return FALSE;

      /* Modified => will need to iterate relaxation again.  */
      *again = TRUE;
    }

  return TRUE;
}

/* Delete some bytes from a section while relaxing.  */

static bfd_boolean
ubicom32_elf_relax_delete_bytes (abfd, sec, addr, count)
     bfd *abfd;
     asection *sec;
     bfd_vma addr;
     int count;
{
  bfd_byte *contents = elf_section_data (sec)->this_hdr.contents;
  bfd_vma endaddr = sec->rawsize;

  /* Actually delete the bytes.  */
  memmove (contents + addr, contents + addr + count,
	   endaddr - addr - count);

  sec->rawsize -= count;

  adjust_all_relocations (abfd, sec, addr + count, -count);
  return TRUE;
}

/* Adjust all the relocations entries after adding or inserting instructions.  */

static void
adjust_sec_relocations (abfd, sec_to_process, addr_sec, addr, count)
     bfd *abfd;
     asection *sec_to_process;
     asection *addr_sec;
     bfd_vma addr;
     int count;
{
  Elf_Internal_Shdr *symtab_hdr;
  Elf_Internal_Sym *isymbuf, *isym;
  Elf_Internal_Rela *irel, *irelend, *irelbase;
  unsigned int addr_shndx;

  irelbase = section_relocs (abfd, sec_to_process);
  if (irelbase == NULL)
    return;
  irelend = irelbase + sec_to_process->reloc_count;

  symtab_hdr = file_symtab_hdr (abfd);
  isymbuf = file_isymbuf (abfd);

  addr_shndx = _bfd_elf_section_from_bfd_section (abfd, addr_sec);

  for (irel = irelbase; irel < irelend; irel++)
    {
      if (ELF32_R_TYPE (irel->r_info) != R_UBICOM32_NONE)
	{
	  /* Get the value of the symbol referred to by the reloc.  */
	  if (ELF32_R_SYM (irel->r_info) < symtab_hdr->sh_info)
	    {
	      asection *sym_sec;
	      bfd_vma xaddr, symval, relval;

	      /* A local symbol.  */
	      isym = isymbuf + ELF32_R_SYM (irel->r_info);
	      sym_sec = bfd_section_from_elf_index (abfd, isym->st_shndx);
	      xaddr = BASEADDR (addr_sec) + addr;
	      symval = BASEADDR (sym_sec) + isym->st_value;
	      relval = symval + irel->r_addend;

	      if ((isym->st_shndx == addr_shndx)
		  && (xaddr > symval)
		  && (xaddr <= relval))
		irel->r_addend += count;
	    }
	}

      /* Adjust irel base address for PC space relocations after a deleted instruction.  */
      if (sec_to_process == addr_sec)
	{
	  if (addr <= irel->r_offset)
	    irel->r_offset += count;
	}
    }
}

static void
adjust_all_relocations (abfd, sec, addr, count)
     bfd *abfd;
     asection *sec;
     bfd_vma addr;
     int count;
{
  Elf_Internal_Shdr *symtab_hdr;
  Elf_Internal_Sym *isymbuf, *isym, *isymend;
  struct elf_link_hash_entry **sym_hashes;
  struct elf_link_hash_entry **end_hashes;
  unsigned int symcount;
  asection *section;
  unsigned int shndx;

  symtab_hdr = file_symtab_hdr (abfd);
  isymbuf = file_isymbuf (abfd);

  shndx = _bfd_elf_section_from_bfd_section (abfd, sec);

  /* Adjust all relocations that are affected.  */
  for (section = abfd->sections; section != NULL; section = section->next)
    adjust_sec_relocations (abfd, section, sec, addr, count);

  /* Adjust the local symbols defined in this section.  */
  isymend = isymbuf + symtab_hdr->sh_info;
  for (isym = isymbuf; isym < isymend; isym++)
    {
      if (isym->st_shndx == shndx
	  && addr <= isym->st_value)
	isym->st_value += count;
    }

  /* Now adjust the global symbols defined in this section.  */
  symcount = (symtab_hdr->sh_size / sizeof (Elf32_External_Sym)
	      - symtab_hdr->sh_info);
  sym_hashes = elf_sym_hashes (abfd);
  end_hashes = sym_hashes + symcount;
  for (; sym_hashes < end_hashes; sym_hashes++)
    {
      struct elf_link_hash_entry *sym_hash = *sym_hashes;

      if ((sym_hash->root.type == bfd_link_hash_defined
	   || sym_hash->root.type == bfd_link_hash_defweak)
	  && sym_hash->root.u.def.section == sec)
	{
	  if (addr <= sym_hash->root.u.def.value)
	    sym_hash->root.u.def.value += count;
	}
    }
}

/* Perform a single relocation.  By default we use the standard BFD
   routines. */

static bfd_reloc_status_type
ubicom32_final_link_relocate (howto, input_bfd, input_section, contents, rel, relocation)
     reloc_howto_type *  howto;
     bfd *               input_bfd;
     asection *          input_section;
     bfd_byte *          contents;
     Elf_Internal_Rela * rel;
     bfd_vma             relocation;
{
  bfd_reloc_status_type r = bfd_reloc_ok;

  switch (howto->type)
    {
    default:
      r = _bfd_final_link_relocate (howto, input_bfd, input_section,
				    contents, rel->r_offset,
				    relocation, rel->r_addend);
    }

  return r;
}

/* Relocate a UBICOM32 ELF section.
   There is some attempt to make this function usable for many architectures,
   both USE_REL and USE_RELA ['twould be nice if such a critter existed],
   if only to serve as a learning tool.

   The RELOCATE_SECTION function is called by the new ELF backend linker
   to handle the relocations for a section.

   The relocs are always passed as Rela structures; if the section
   actually uses Rel structures, the r_addend field will always be
   zero.

   This function is responsible for adjusting the section contents as
   necessary, and (if using Rela relocs and generating a relocatable
   output file) adjusting the reloc addend as necessary.

   This function does not have to worry about setting the reloc
   address or the reloc symbol index.

   LOCAL_SYMS is a pointer to the swapped in local symbols.

   LOCAL_SECTIONS is an array giving the section in the input file
   corresponding to the st_shndx field of each local symbol.

   The global hash table entry for the global symbols can be found
   via elf_sym_hashes (input_bfd).

   When generating relocatable output, this function must handle
   STB_LOCAL/STT_SECTION symbols specially.  The output symbol is
   going to be the section symbol corresponding to the output
   section, which means that the addend must be adjusted
   accordingly.  */

static bfd_boolean
ubicom32_elf_relocate_section (output_bfd, info, input_bfd, input_section,
			       contents, relocs, local_syms, local_sections)
     bfd *                   output_bfd ATTRIBUTE_UNUSED;
     struct bfd_link_info *  info;
     bfd *                   input_bfd;
     asection *              input_section;
     bfd_byte *              contents;
     Elf_Internal_Rela *     relocs;
     Elf_Internal_Sym *      local_syms;
     asection **             local_sections;
{
  Elf_Internal_Shdr *           symtab_hdr;
  struct elf_link_hash_entry ** sym_hashes;
  Elf_Internal_Rela *           rel;
  Elf_Internal_Rela *           relend;
  struct debugLineInfoHeader *lh = NULL;
  int cooked_size, discard_size;
  bfd_byte *src, *dest, *content_end;
  unsigned int i;

  symtab_hdr = & elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (input_bfd);
  relend     = relocs + input_section->reloc_count;

  for (rel = relocs; rel < relend; rel ++)
    {
      reloc_howto_type *           howto;
      unsigned long                r_symndx;
      Elf_Internal_Sym *           sym;
      asection *                   sec;
      struct elf_link_hash_entry * h;
      bfd_vma                      relocation;
      bfd_reloc_status_type        r;
      const char *                 name = NULL;
      int                          r_type;

      r_type = ELF32_R_TYPE (rel->r_info);

      if ( r_type == R_UBICOM32_GNU_VTINHERIT
	   || r_type == R_UBICOM32_GNU_VTENTRY)
	continue;

      r_symndx = ELF32_R_SYM (rel->r_info);

      if (info->relocatable)
	{
	  /* This is a relocatable link.  We don't have to change
	     anything, unless the reloc is against a section symbol,
	     in which case we have to adjust according to where the
	     section symbol winds up in the output section.  */
	  if (r_symndx < symtab_hdr->sh_info)
	    {
	      sym = local_syms + r_symndx;

	      if (ELF_ST_TYPE (sym->st_info) == STT_SECTION)
		{
		  sec = local_sections [r_symndx];
		  rel->r_addend += sec->output_offset + sym->st_value;
		}
	    }

	  continue;
	}

      /* This is a final link.  */
      howto  = ubicom32_elf_howto_table + ELF32_R_TYPE (rel->r_info);
      h      = NULL;
      sym    = NULL;
      sec    = NULL;

      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sec = local_sections [r_symndx];
	  relocation = (sec->output_section->vma
			+ sec->output_offset
			+ sym->st_value);

	  name = bfd_elf_string_from_elf_section
	    (input_bfd, symtab_hdr->sh_link, sym->st_name);
	  name = (name == NULL) ? bfd_section_name (input_bfd, sec) : name;
	}
      else
	{
	  h = sym_hashes [r_symndx - symtab_hdr->sh_info];

	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;

	  name = h->root.root.string;

	  if (h->root.type == bfd_link_hash_defined
	      || h->root.type == bfd_link_hash_defweak)
	    {
	      sec = h->root.u.def.section;
	      relocation = (h->root.u.def.value
			    + sec->output_section->vma
			    + sec->output_offset);
	    }
	  else if (h->root.type == bfd_link_hash_undefweak)
	    {
	      relocation = 0;
	    }
	  else
	    {
	      if (! ((*info->callbacks->undefined_symbol)
		     (info, h->root.root.string, input_bfd,
		      input_section, rel->r_offset,
		      (!info->shared ))))
		return FALSE;
	      relocation = 0;
	    }
	}

      switch (r_type)
	{
	case R_UBICOM32_LO16:
	  r = ubicom32_elf_relocate_lo16 (input_bfd, rel, contents, relocation);
	  break;

	case R_UBICOM32_HI16:
	  r = ubicom32_elf_relocate_hi16 (input_bfd, rel, contents, relocation);
	  break;

	case R_UBICOM32_HI24:
	  r = ubicom32_elf_relocate_hi24 (input_bfd, rel, contents, relocation);
	  break;

	case R_UBICOM32_LO7_S:
	  r = ubicom32_elf_relocate_lo7_s (input_bfd, rel, contents, relocation);
	  break;

	case R_UBICOM32_LO7_2_S:
	  r = ubicom32_elf_relocate_lo7_2_s (input_bfd, rel, contents, relocation);
	  break;

	case R_UBICOM32_LO7_4_S:
	  r = ubicom32_elf_relocate_lo7_4_s (input_bfd, rel, contents, relocation);
	  break;

	case R_UBICOM32_LO7_D:
	  r = ubicom32_elf_relocate_lo7_d (input_bfd, rel, contents, relocation);
	  break;

	case R_UBICOM32_LO7_2_D:
	  r = ubicom32_elf_relocate_lo7_2_d (input_bfd, rel, contents, relocation);
	  break;

	case R_UBICOM32_LO7_4_D:
	  r = ubicom32_elf_relocate_lo7_4_d (input_bfd, rel, contents, relocation);
	  break;

	case R_UBICOM32_24_PCREL:
	  r = ubicom32_elf_relocate_pcrel24 (input_bfd, input_section, rel, contents, relocation);
	  break;

	case R_UBICOM32_LO7_CALLI:
	  r = ubicom32_elf_relocate_lo_calli (input_bfd, rel, contents, relocation, 7);
	  break;

	case R_UBICOM32_LO16_CALLI:
	  r = ubicom32_elf_relocate_lo_calli (input_bfd, rel, contents, relocation, 18);
	  break;

	case R_UBICOM32_LO7_LEAI:
	  r = ubicom32_elf_relocate_lo_leai (input_bfd, rel, contents, relocation, 7);
	  break;

	case R_UBICOM32_LO16_LEAI:
	  r = ubicom32_elf_relocate_lo_leai (input_bfd, rel, contents, relocation, 18);
	  break;

	case R_UBICOM32_32:
	  /* relocation &= ~(0xff << 24); */
	  /* FALLTHROUGH */

	default:
	  r = ubicom32_final_link_relocate (howto, input_bfd, input_section,
					    contents, rel, relocation);
	  break;
	}

      if (r != bfd_reloc_ok)
	{
	  const char * msg = (const char *) NULL;

	  switch (r)
	    {
	    case bfd_reloc_overflow:
	      r = info->callbacks->reloc_overflow
		(info, NULL, name, howto->name, (bfd_vma) 0,
		 input_bfd, input_section, rel->r_offset);
	      break;

	    case bfd_reloc_undefined:
	      r = info->callbacks->undefined_symbol
		(info, name, input_bfd, input_section, rel->r_offset, TRUE);
	      break;

	    case bfd_reloc_outofrange:
	      msg = _("internal error: out of range error");
	      break;

	    case bfd_reloc_notsupported:
	      msg = _("internal error: unsupported relocation error");
	      break;

	    case bfd_reloc_dangerous:
	      msg = _("internal error: dangerous relocation");
	      break;

	    default:
	      msg = _("internal error: unknown error");
	      break;
	    }

	  if (msg)
	    r = info->callbacks->warning
	      (info, msg, name, input_bfd, input_section, rel->r_offset);

	  if (! r)
	    return FALSE;
	}
    }

  /*
   * now we have to collapse the .debug_line section if it has a
   * sec_info section
   */

  if(strcmp(input_section->name, ".debug_line"))
    return TRUE;

  /* this is a .debug_line section. See it has a sec_info entry */
  if(elf_section_data(input_section)->sec_info == NULL)
    return TRUE;

  lh = (struct debugLineInfoHeader *) elf_section_data(input_section)->sec_info;

  if(lh->numEntries == 0)
    return TRUE;

  dest = contents + lh->linfo[0].startOffset;

  cooked_size = input_section->rawsize;
  content_end = contents + cooked_size;
  discard_size = 0;

  for(i=0; i< lh->numEntries; i++)
    {
      if(lh->linfo[i].discard)
	discard_size += lh->linfo[i].length;
      else
	{
	  src = contents + lh->linfo[i].startOffset;
	  (void) memcpy(dest, src, lh->linfo[i].length);
	  dest += lh->linfo[i].length;
	}
    }

  src = contents + lh->linfo[lh->numEntries-1].startOffset + lh->linfo[lh->numEntries-1].length;
  if(src < content_end)
    (void) memcpy(dest, src, content_end - src);

  i = bfd_get_32(input_bfd, contents);
  i -= discard_size;
  bfd_put_32(input_bfd, i, contents);
  //input_section->rawsize -= discard_size;
  return TRUE;
}


/* Update the got entry reference counts for the section being
   removed.  */

static bfd_boolean
ubicom32_elf_gc_sweep_hook (abfd, info, sec, relocs)
     bfd *                     abfd ATTRIBUTE_UNUSED;
     struct bfd_link_info *    info ATTRIBUTE_UNUSED;
     asection *                sec ATTRIBUTE_UNUSED;
     const Elf_Internal_Rela * relocs ATTRIBUTE_UNUSED;
{
  return TRUE;
}

/* Return the section that should be marked against GC for a given
   relocation.  */

static asection *
ubicom32_elf_gc_mark_hook (sec, info, rel, h, sym)
     asection *                   sec;
     struct bfd_link_info *       info ATTRIBUTE_UNUSED;
     Elf_Internal_Rela *          rel;
     struct elf_link_hash_entry * h;
     Elf_Internal_Sym *           sym;
{
  if (h != NULL)
    {
      switch (ELF32_R_TYPE (rel->r_info))
	{
	case R_UBICOM32_GNU_VTINHERIT:
	case R_UBICOM32_GNU_VTENTRY:
	  break;

	default:
	  switch (h->root.type)
	    {
	    case bfd_link_hash_defined:
	    case bfd_link_hash_defweak:
	      return h->root.u.def.section;

	    case bfd_link_hash_common:
	      return h->root.u.c.p->section;

	    default:
	      break;
	    }
	}
    }
  else
    {
      if (!(elf_bad_symtab (sec->owner)
	    && ELF_ST_BIND (sym->st_info) != STB_LOCAL)
	  && ! ((sym->st_shndx <= 0 || sym->st_shndx >= SHN_LORESERVE)
		&& sym->st_shndx != SHN_COMMON))
	{
	  return bfd_section_from_elf_index (sec->owner, sym->st_shndx);
	}
    }

  return NULL;
}

/* Look through the relocs for a section during the first phase.
   Since we don't do .gots or .plts, we just need to consider the
   virtual table relocs for gc.  */

static bfd_boolean
ubicom32_elf_check_relocs (abfd, info, sec, relocs)
     bfd *abfd;
     struct bfd_link_info *info;
     asection *sec;
     const Elf_Internal_Rela *relocs;
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes, **sym_hashes_end;
  Elf_Internal_Rela *rel;
  Elf_Internal_Rela *rel_end;
  Elf_Internal_Rela *my_rel = ( Elf_Internal_Rela*)relocs;
  if (info->relocatable)
    return TRUE;

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (abfd);
  sym_hashes_end = sym_hashes + symtab_hdr->sh_size/sizeof(Elf32_External_Sym);
  if (!elf_bad_symtab (abfd))
    sym_hashes_end -= symtab_hdr->sh_info;

  rel_end = my_rel + sec->reloc_count;
  for (rel = my_rel; rel < rel_end; rel++)
    {
      struct elf_link_hash_entry *h;
      unsigned long r_symndx;

      r_symndx = ELF32_R_SYM (rel->r_info);
      if (r_symndx < symtab_hdr->sh_info)
	h = NULL;
      else
	h = sym_hashes [r_symndx - symtab_hdr->sh_info];

      switch (ELF32_R_TYPE (rel->r_info))
	{
	  /* This relocation describes the C++ object vtable hierarchy.
	     Reconstruct it for later use during GC.  */
	case R_UBICOM32_GNU_VTINHERIT:
	  if (!bfd_elf_gc_record_vtinherit (abfd, sec, h, rel->r_offset))
	    return FALSE;
	  break;

	  /* This relocation describes which C++ vtable entries are actually
	     used.  Record for later use during GC.  */
	case R_UBICOM32_GNU_VTENTRY:
	  if (!bfd_elf_gc_record_vtentry (abfd, sec, h, rel->r_addend))
	    return FALSE;
	  break;

	case R_UBICOM32_32:
	  /* For debug section, change to harvard relocations */
	  if (memcmp (sec->name, ".debug", 6) == 0
	      || memcmp (sec->name, ".stab", 5) == 0)
	    rel->r_info = ELF32_R_INFO (ELF32_R_SYM (rel->r_info), R_UBICOM32_32_HARVARD);
	  break;
	}
    }
  return TRUE;
}

static bfd_boolean
ubicom32_elf_object_p (abfd)
     bfd *abfd;
{
  flagword  mach = elf_elfheader (abfd)->e_flags & 0xffff;
  bfd_default_set_arch_mach (abfd, bfd_arch_ubicom32, mach);
  return (((elf_elfheader (abfd)->e_flags & EF_UBICOM32_FDPIC) != 0)
	  == (IS_FDPIC (abfd)));
}


/* Function to set the ELF flag bits */

static bfd_boolean
ubicom32_elf_set_private_flags (abfd, flags)
     bfd *abfd;
     flagword flags;
{
  elf_elfheader (abfd)->e_flags = flags;
  elf_flags_init (abfd) = TRUE;
  return TRUE;
}

static bfd_boolean
ubicom32_elf_copy_private_bfd_data (ibfd, obfd)
     bfd *ibfd;
     bfd *obfd;
{
  if (bfd_get_flavour (ibfd) != bfd_target_elf_flavour
      || bfd_get_flavour (obfd) != bfd_target_elf_flavour)
    return TRUE;

  BFD_ASSERT (!elf_flags_init (obfd)
	      || elf_elfheader (obfd)->e_flags == elf_elfheader (ibfd)->e_flags);

  elf_elfheader (obfd)->e_flags = elf_elfheader (ibfd)->e_flags;
  elf_flags_init (obfd) = TRUE;
  return TRUE;
}

/* Merge backend specific data from an object file to the output
   object file when linking. */
static bfd_boolean
ubicom32_elf_merge_private_bfd_data (ibfd, obfd)
     bfd *ibfd;
     bfd *obfd;
{
  flagword old_flags, new_flags;
  bfd_boolean error = FALSE;

  new_flags = elf_elfheader (ibfd)->e_flags;
  old_flags = elf_elfheader (obfd)->e_flags;

#ifdef DEBUG
  (*_bfd_error_handler) ("old_flags = 0x%.8lx, new_flags = 0x%.8lx, init = %s, filename = %s",
			 old_flags, new_flags, elf_flags_init (obfd) ? "yes" : "no",
			 bfd_get_filename (ibfd));
#endif

  if (!elf_flags_init (obfd))			/* First call, no flags set */
    {
      elf_flags_init (obfd) = TRUE;
      elf_elfheader (obfd)->e_flags = new_flags;
    }
  else
    {
      if (new_flags != old_flags)
	{
	  /* Mismatched flags. */
	  char *output_cpu_version = ((old_flags &0xffff) == 1) ? "V3" : (((old_flags &0xffff) == 2) ? "V4" : "unknown");
	  char *input_cpu_version = ((new_flags &0xffff) == 1) ? "V3" : (((new_flags &0xffff) == 2) ? "V4" : "unknown");
	  char *output_filename = bfd_get_filename (obfd);
	  char *input_filename = bfd_get_filename (ibfd);
	  char *output_pic = (old_flags & EF_UBICOM32_PIC_FLAGS) ? ((old_flags & EF_UBICOM32_PIC) ? "FPIC" : "FDPIC") : NULL;
	  char *input_pic = (new_flags & EF_UBICOM32_PIC_FLAGS) ? ((new_flags & EF_UBICOM32_PIC) ? "FPIC" : "FDPIC") : NULL;

	  (*_bfd_error_handler) ("Linking mismatched file types. Output file = %s file type 0x%.8lx, input file = %s file type 0x%.8lx",
				 output_filename, old_flags, input_filename, new_flags);

	  if (output_pic)
	    {
	      (*_bfd_error_handler)("Output file %s %s for cpu version %s", output_filename, output_pic, output_cpu_version);
	    }
	  else
	    {
	      (*_bfd_error_handler)("Output file %s for cpu version %s", output_filename, output_cpu_version);
	    }

	  if (input_pic)
	    {
	      (*_bfd_error_handler)("Input file %s %s for cpu version %s", input_filename, input_pic, input_cpu_version);
	    }
	  else
	    {
	      (*_bfd_error_handler)("Input file %s for cpu version %s", input_filename, input_cpu_version);
	    }

	  (*_bfd_error_handler) ("Link ABORTED.");
	  _exit(EXIT_FAILURE);
	}
    }
  if (error)
    bfd_set_error (bfd_error_bad_value);

  return !error;
}

static bfd_boolean
ubicom32_elf_print_private_bfd_data (abfd, ptr)
     bfd *abfd;
     PTR ptr;
{
  FILE *file = (FILE *) ptr;
  flagword flags;

  BFD_ASSERT (abfd != NULL && ptr != NULL);

  /* Print normal ELF private data.  */
  _bfd_elf_print_private_bfd_data (abfd, ptr);

  flags = elf_elfheader (abfd)->e_flags;
  fprintf (file, _("private flags = 0x%lx:"), (long)flags);

  fputc ('\n', file);

  return TRUE;
}

bfd_boolean
ubicom32_elf_discard_info(abfd, cookie, info)
     bfd *abfd;
     struct elf_reloc_cookie *cookie ATTRIBUTE_UNUSED;
     struct bfd_link_info *info;

{
  unsigned int hasDebugLine=0;
  unsigned needExclude = 0;
  asection *o;
  asection *sec= NULL;
  bfd_byte *contents = NULL;
  bfd_byte *contentsEnd;
  Elf_Internal_Rela *irel, *irelend, *irelbase;
  Elf_Internal_Shdr *symtab_hdr;
  Elf_Internal_Sym *isym;
  Elf_Internal_Sym *isymbuf = NULL;
  struct debugLineInfoHeader *lh = NULL;
  unsigned int maxLineInfoEntries = 10;
  unsigned int offset, contentLength;
  unsigned char *ptr, *sequence_start;
  unsigned int setupEntry=1;
  unsigned int opcode_base, op_code;
  unsigned int bytes_read;

  for (o = abfd->sections; o != NULL; o = o->next)
    {
      if(!hasDebugLine)
	if(!strcmp(o->name, ".debug_line"))
	  {
	    hasDebugLine =1;
	    sec = o;
	  }

      /* Keep special sections.  Keep .debug sections.  */
      if (o->flags & SEC_EXCLUDE)
	{
	  needExclude = 1;
	}
    }

  if(needExclude == 0 || hasDebugLine ==0)
    return FALSE;

  /*
   * you can be here only if we have .debug_line section and some
   * section is being excudled
   */

  /*
   * We need to extract .debug_line section contents and its
   * relocation contents.
   */

  /* We don't have to do anything for a relocatable link,
     if this section does not have relocs */
  if (info->relocatable
      || (sec->flags & SEC_RELOC) == 0
      || sec->reloc_count == 0)
    return FALSE;

  /* If this is the first time we have been called
     for this section, initialise the cooked size.
     if (sec->_cooked_size == 0)
     sec->_cooked_size = sec->rawsize;
  */

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;

  irelbase = _bfd_elf_link_read_relocs (abfd, sec, NULL,
					(Elf_Internal_Rela *)NULL,
					info->keep_memory);

  if(irelbase == NULL)
    return FALSE;

  irelend = irelbase +sec->reloc_count;

  /* Get section contents cached copy if it exists.  */
  if (contents == NULL)
    {
      contents = section_contents(abfd, sec);
    }

  if (isymbuf == NULL && symtab_hdr->sh_info != 0)
    {
      isymbuf = (Elf_Internal_Sym *) symtab_hdr->contents;
      if (isymbuf == NULL)
	isymbuf = bfd_elf_get_elf_syms (abfd, symtab_hdr,
					symtab_hdr->sh_info, 0,
					NULL, NULL, NULL);
      if (isymbuf == NULL)
	return FALSE;
    }

  /* allocate the line header and initialize it */
  lh = (struct debugLineInfoHeader *)
    realloc( (void *)lh, sizeof (struct debugLineInfo)*maxLineInfoEntries +
	     sizeof(unsigned int));

  lh->numEntries = 0;

  /* the first 4 bytes contains the length */
  contentLength = bfd_get_32 (abfd, (bfd_byte *)contents);
  contentsEnd = contents + contentLength + 4;

  ptr = (unsigned char *)contents;
  ptr +=6;
  /* read the header length */
  offset = bfd_get_32(abfd, (bfd_byte *)ptr);
  ptr += 4;
  ptr += offset;

  /* extract the base opcode */
  opcode_base = (unsigned char)contents[14];
  sequence_start = NULL;
  while(ptr < (unsigned char *) contentsEnd)
    {
      if(setupEntry)
	{
	  if(lh->numEntries == maxLineInfoEntries)
	    {
	      /* need to do some reallocing. Bump up the entries by 10 */
	      maxLineInfoEntries += 10;
	      lh = (struct debugLineInfoHeader *)
		realloc( (void *)lh,
			 sizeof (struct debugLineInfo)*maxLineInfoEntries +
			 sizeof(unsigned int));
	    }

	  /* zero out the entry */
	  memset((void *) &lh->linfo[lh->numEntries],
		 0,
		 sizeof(struct debugLineInfo));
	  lh->linfo[lh->numEntries].startOffset = (bfd_byte *)ptr - contents;
	  setupEntry = 0;
	  sequence_start = ptr;
	}

      /* We need to run the state machine */
      op_code = bfd_get_8 (abfd, (bfd_byte *)ptr);
      ptr += 1;

      if(op_code >= opcode_base)
	continue;

      switch(op_code)
	{
	case DW_LNS_extended_op:
	  ptr += 1;	/* ignore length */
	  op_code = bfd_get_8 (abfd, (bfd_byte *)ptr);
	  ptr += 1;
	  switch (op_code)
	    {
	    case DW_LNE_end_sequence:
	      /* end of sequence. Time to record stuff */
	      lh->linfo[lh->numEntries++].length =
		(bfd_byte *)ptr - sequence_start;
	      setupEntry = 1;
	      break;
	    case DW_LNE_set_address:
	      ptr += 4;
	      break;
	    case DW_LNE_define_file:
	      {
		ptr += (strlen((char *)ptr) + 1);
		(void) read_unsigned_leb128(abfd, ptr, &bytes_read);
		ptr += bytes_read;
		(void) read_unsigned_leb128(abfd, ptr, &bytes_read);
		ptr += bytes_read;
		(void) read_unsigned_leb128(abfd, ptr, &bytes_read);
		ptr += bytes_read;
		break;
	      }
	    }
	case DW_LNS_negate_stmt:
	case DW_LNS_set_basic_block:
	case DW_LNS_const_add_pc:
	case DW_LNS_copy:
	  break;
	case DW_LNS_advance_pc:
	case DW_LNS_set_file:
	case DW_LNS_set_column:
	  (void) read_unsigned_leb128 (abfd, ptr, &bytes_read);
	  ptr += bytes_read;
	  break;
	case DW_LNS_advance_line:
	  (void) read_signed_leb128 (abfd, ptr, &bytes_read);
	  ptr += bytes_read;
	  break;
	case DW_LNS_fixed_advance_pc:
	  ptr += 2;
	  break;
	}
    }

  /*
   * now scan through the relocations and match the
   * lineinfo to a section name
   */
  for(irel = irelbase; irel< irelend; irel++)
    {
      bfd_vma offset;
      asection *sym_sec;
      int i;

      offset = irel->r_offset;
      isym = isymbuf + ELF32_R_SYM (irel->r_info);

      sym_sec = bfd_section_from_elf_index (abfd, isym->st_shndx);

      /* find which line section this rel entry belongs to */
      for(i=0; i< (int) lh->numEntries; i++)
	{
	  if(lh->linfo[i].startOffset <= offset &&
	     offset < lh->linfo[i].startOffset + lh->linfo[i].length)
	    break;
	}

      if(lh->linfo[i].sectionName == NULL)
	lh->linfo[i].sectionName = strdup(sym_sec->name);
    }

  /* now scan through and find the exclude sections */
  for (o = abfd->sections; o != NULL; o = o->next)
    {
      if (o->flags & SEC_EXCLUDE)
	{
	  /* go through the lh entries and mark as discard */
	  int i;
	  for(i=0; i< (int) lh->numEntries; i++)
	    {
	      if(!strcmp(o->name, lh->linfo[i].sectionName))
		lh->linfo[i].discard = 1;
	    }
	}
    }

  elf_section_data(sec)->sec_info = (PTR)(lh);

  return TRUE;
}


/* An extension of the elf hash table data structure, containing some
   additional Blackfin-specific data.  */
struct ubicom32fdpic_elf_link_hash_table
{
  struct elf_link_hash_table elf;

  /* A pointer to the .got section.  */
  asection *sgot;
  /* A pointer to the .rel.got section.  */
  asection *sgotrel;
  /* A pointer to the .rofixup section.  */
  asection *sgotfixup;
  /* A pointer to the .plt section.  */
  asection *splt;
  /* A pointer to the .rel.plt section.  */
  asection *spltrel;
  /* GOT base offset.  */
  bfd_vma got0;
  /* Location of the first non-lazy PLT entry, i.e., the number of
     bytes taken by lazy PLT entries.  */
  bfd_vma plt0;
  /* A hash table holding information about which symbols were
     referenced with which PIC-related relocations.  */
  struct htab *relocs_info;
};

/* Get the Ubicom32 ELF linker hash table from a link_info structure.  */

#define ubicom32fdpic_hash_table(info) \
  ((struct ubicom32fdpic_elf_link_hash_table *) ((info)->hash))

#define ubicom32fdpic_got_section(info) \
  (ubicom32fdpic_hash_table (info)->sgot)
#define ubicom32fdpic_gotrel_section(info) \
  (ubicom32fdpic_hash_table (info)->sgotrel)
#define ubicom32fdpic_gotfixup_section(info) \
  (ubicom32fdpic_hash_table (info)->sgotfixup)
#define ubicom32fdpic_plt_section(info) \
  (ubicom32fdpic_hash_table (info)->splt)
#define ubicom32fdpic_pltrel_section(info) \
  (ubicom32fdpic_hash_table (info)->spltrel)
#define ubicom32fdpic_relocs_info(info) \
  (ubicom32fdpic_hash_table (info)->relocs_info)
#define ubicom32fdpic_got_initial_offset(info) \
  (ubicom32fdpic_hash_table (info)->got0)
#define ubicom32fdpic_plt_initial_offset(info) \
  (ubicom32fdpic_hash_table (info)->plt0)

/* The name of the dynamic interpreter.  This is put in the .interp
   section.  */

#define ELF_DYNAMIC_INTERPRETER "/lib/ld.so.1"

#define DEFAULT_STACK_SIZE 0x20000
long ubicom32fdpic_stack_size = DEFAULT_STACK_SIZE;

/* This structure is used to collect the number of entries present in
   each addressable range of the got.  */
struct _ubicom32fdpic_dynamic_got_info
{
  /* Several bits of information about the current link.  */
  struct bfd_link_info *info;
  /* Total size needed for GOT entries. */
  bfd_vma got_lo, got_hi;
  /* Total size needed for function descriptor entries. */
  bfd_vma fd_got_lo, fd_got_hi;
  /* Total size needed function descriptor entries referenced in PLT
     entries, that would be profitable to place in offsets close to
     the PIC register.  */
  bfd_vma fdplt, privfdplt;
  /* Total size needed by lazy PLT entries.  */
  bfd_vma lzplt;
  bfd_vma num_plts;

  /* Number of relocations carried over from input object files.  */
  unsigned long relocs;
  /* Number of fixups introduced by relocations in input object files.  */
  unsigned long fixups;
};

/* This structure is used to assign offsets to got entries, function
   descriptors, plt entries and lazy plt entries.  */
struct ubicom32fdpic_dynamic_got_plt_info
{
  /* Summary information collected with _bfinfdpic_count_got_plt_entries.  */
  struct _ubicom32fdpic_dynamic_got_info g;

  bfd_signed_vma current_got;	/* This will be used during got entry allocation */
  bfd_signed_vma current_fd;	/* This will be used for function descriptro allocation. The numbers will go negative */
  bfd_signed_vma current_privfd;	/* This will be used for function descriptro allocation. The numbers will go negative */
  bfd_vma current_plt;		/* This is the offset to the PLT entry. We will need this to resolve the call entries. */
  bfd_vma current_plt_trampoline; /* This is the offset to the PLT trampoline entry. */
  bfd_vma total_fdplt;		/* Total size of function descriptors. This is the memory above GOT pointer. */
  bfd_vma total_got;		/* This is the total of got entries for got_lo and got_funcdesc_lo references. */
  bfd_vma total_lzplt;		/* This is the total area for the PLT entries. This does not have the trampoline entry. */
  bfd_vma total_trampoline;	/* This is the total area for the PLT trampoline entries. */
};

/* Decide whether a reference to a symbol can be resolved locally or
   not.  If the symbol is protected, we want the local address, but
   its function descriptor must be assigned by the dynamic linker.  */
#define UBICOM32FDPIC_SYM_LOCAL(INFO, H) \
  (_bfd_elf_symbol_refs_local_p ((H), (INFO), 1) \
   || ! elf_hash_table (INFO)->dynamic_sections_created)
#define UBICOM32FDPIC_FUNCDESC_LOCAL(INFO, H) \
  ((H)->dynindx == -1 || ! elf_hash_table (INFO)->dynamic_sections_created)

/* This structure collects information on what kind of GOT, PLT or
   function descriptors are required by relocations that reference a
   certain symbol.  */
struct ubicom32fdpic_relocs_info
{
  /* The index of the symbol, as stored in the relocation r_info, if
     we have a local symbol; -1 otherwise.  */
  long symndx;
  union
  {
    /* The input bfd in which the symbol is defined, if it's a local
       symbol.  */
    bfd *abfd;
    /* If symndx == -1, the hash table entry corresponding to a global
       symbol (even if it turns out to bind locally, in which case it
       should ideally be replaced with section's symndx + addend).  */
    struct elf_link_hash_entry *h;
  } d;
  /* The addend of the relocation that references the symbol.  */
  bfd_vma addend;

  /* The fields above are used to identify an entry.  The fields below
     contain information on how an entry is used and, later on, which
     locations it was assigned.  */
  /* The following 2 fields record whether the symbol+addend above was
     ever referenced with a GOT relocation.  The 17M4 suffix indicates a
     GOT17M4 relocation; hilo is used for GOTLO/GOTHI pairs.  */
  unsigned got_lo;
  unsigned got_hi;
  /* Whether a FUNCDESC relocation references symbol+addend.  */
  unsigned fd;
  /* Whether a FUNCDESC_GOT relocation references symbol+addend.  */
  unsigned fd_got_lo;
  unsigned fd_got_hi;
  /* Whether symbol+addend is referenced with GOTOFF17M4, GOTOFFLO or
     GOTOFFHI relocations.  The addend doesn't really matter, since we
     envision that this will only be used to check whether the symbol
     is mapped to the same segment as the got.  */
  unsigned gotoff;
  /* Whether symbol+addend is referenced by a LABEL24 relocation.  */
  unsigned call;
  /* Whether symbol+addend is referenced by a 32 or FUNCDESC_VALUE
     relocation.  */
  unsigned sym;
  /* Whether we need a PLT entry for a symbol.  Should be implied by
     something like:
     (call && symndx == -1 && ! BFINFDPIC_SYM_LOCAL (info, d.h))  */
  unsigned plt:1;
  /* Whether a function descriptor should be created in this link unit
     for symbol+addend.  Should be implied by something like:
     (plt || fd_got_lo || fd_got_hi
     || ((fd || fdgot17m4 || fdgothilo)
     && (symndx != -1 || BFINFDPIC_FUNCDESC_LOCAL (info, d.h))))  */
  unsigned privfd:1;
  /* Whether a lazy PLT entry is needed for this symbol+addend.
     Should be implied by something like:
     (privfd && symndx == -1 && ! BFINFDPIC_SYM_LOCAL (info, d.h)
     && ! (info->flags & DF_BIND_NOW))  */
  unsigned lazyplt:1;
  /* Whether we've already emitted GOT relocations and PLT entries as
     needed for this symbol.  */
  unsigned done:1;

  /* The number of R_byte4_data, R_BFIN_FUNCDESC and R_BFIN_FUNCDESC_VALUE
     relocations referencing the symbol.  */
  unsigned relocs32, relocsfd, relocsfdv;

  /* The number of .rofixups entries and dynamic relocations allocated
     for this symbol, minus any that might have already been used.  */
  unsigned fixups, dynrelocs;

  /* The offsets of the GOT entries assigned to symbol+addend, to the
     function descriptor's address, and to a function descriptor,
     respectively.  Should be zero if unassigned.  The offsets are
     counted from the value that will be assigned to the PIC register,
     not from the beginning of the .got section.  */
  bfd_signed_vma got_entry, fdgot_entry, fd_entry;
  /* The offsets of the PLT entries assigned to symbol+addend,
     non-lazy and lazy, respectively.  If unassigned, should be
     (bfd_vma)-1.  */
  bfd_vma plt_entry;
  bfd_vma plt_trampoline_entry;

  /* plt_type is 1 for Sequence type 2 (0 - 255) it is 2 for > 255 */
  bfd_vma plt_type;

  /* rel_offset. Plt relocation offset need to be encoded into the plt entry. */
  bfd_vma rel_offset;

  /* bfd_vma lzplt_entry; not used in ubicom32 */
};

/* Compute the total GOT size required by each symbol in each range.
   Symbols may require up to 4 words in the GOT: an entry pointing to
   the symbol, an entry pointing to its function descriptor, and a
   private function descriptors taking two words.  */

#if 0
static bfd_vma plt_code[] = {
  0xc90f0000,	//movei d15,#0
  0x0123e30f,	//lea.4 a3,(a0,d15)
  0x0124630f,	//move.4 a4,(a0,d15)
  0x01206461,	//move.4 a0,4(a3)
  0xf0800080,	//calli a4,0(a4)
};
#endif

static bfd_vma plt_trampoline[] = {
  0xc9280000,   //	movei mac_hi,#0
  0x00002400,   //	ret (a0)
};

static bfd_vma plt_code_seq1[] = {
  0xc90fffe8,	//movei d15,#-24
  0x0123e30f,	//lea.4 a3,(a0,d15)
  0x01206461,	//move.4 a0,4(a3)
  0x00002460,	//ret (a3)
};

static bfd_vma plt_code_seq2[] = {
  0x0123f71f,   //	pdec a3,4(a0)
  0x01206461,   //	move.4 a0,4(a3)
  0x00002460,   //	ret (a3)
};

#define NUM_PLT_CODE_WORDS_SEQ1 (sizeof (plt_code_seq1) / sizeof (bfd_vma))
#define LZPLT_SIZE_SEQ1 ((NUM_PLT_CODE_WORDS_SEQ1) * 4)

#define NUM_PLT_CODE_WORDS_SEQ2 (sizeof (plt_code_seq2) / sizeof (bfd_vma))
#define LZPLT_SIZE_SEQ2 ((NUM_PLT_CODE_WORDS_SEQ2) * 4)

#define NUM_PLT_TRAMPOLINE_WORDS (sizeof (plt_trampoline) / sizeof (bfd_vma))
#define PLT_TRAMPOLINE_SIZE ((NUM_PLT_TRAMPOLINE_WORDS) * 4)

#define FUNCTION_DESCRIPTOR_SIZE 8
/* Decide whether a reference to a symbol can be resolved locally or
   not.  If the symbol is protected, we want the local address, but
   its function descriptor must be assigned by the dynamic linker.  */
#define UBICOM32FPIC_SYM_LOCAL(INFO, H) \
  (_bfd_elf_symbol_refs_local_p ((H), (INFO), 1) \
   || ! elf_hash_table (INFO)->dynamic_sections_created)
#define UBICOM32FPIC_FUNCDESC_LOCAL(INFO, H) \
  ((H)->dynindx == -1 || ! elf_hash_table (INFO)->dynamic_sections_created)


static int
ubicom32fdpic_count_got_plt_entries (void **entryp, void *dinfo_)
{
  struct ubicom32fdpic_relocs_info *entry = *entryp;
  struct _ubicom32fdpic_dynamic_got_info *dinfo = dinfo_;
  unsigned relocs = 0, fixups = 0;

  /* Allocate space for a GOT entry pointing to the symbol.  */
  if (entry->got_lo)
    {
      dinfo->got_lo += 4;
      entry->relocs32++;
    }

  /* Allocate space for a GOT entry pointing to the function
     descriptor.  */
  if (entry->fd_got_lo)
    {
      dinfo->got_lo += 4;
      entry->relocsfd++;
    }
  else if (entry->fd_got_hi)
    {
      dinfo->got_lo += 4;
      entry->relocsfd++;
    }

  /* Decide whether we need a PLT entry, a function descriptor in the
     GOT, and a lazy PLT entry for this symbol.  */
  entry->plt = entry->call
    && entry->symndx == -1 && ! UBICOM32FPIC_SYM_LOCAL (dinfo->info, entry->d.h)
    && elf_hash_table (dinfo->info)->dynamic_sections_created;
  entry->privfd = entry->plt
    || ((entry->fd_got_lo || entry->fd_got_hi || entry->fd)
	&& (entry->symndx != -1
	    || UBICOM32FPIC_FUNCDESC_LOCAL (dinfo->info, entry->d.h)));
  entry->lazyplt = entry->privfd
    && entry->symndx == -1 && ! UBICOM32FPIC_SYM_LOCAL (dinfo->info, entry->d.h)
    && ! (dinfo->info->flags & DF_BIND_NOW)
    && elf_hash_table (dinfo->info)->dynamic_sections_created;

  /* Allocate space for a function descriptor.  */
  if (entry->privfd && entry->plt)
    {
      dinfo->fdplt += FUNCTION_DESCRIPTOR_SIZE;
      entry->relocsfdv++;
    }
  else if (entry->privfd)
    {
      /* privfd with plt = 0 */
      //printf("Privfd set with plt 0 gotoff_lo = %d fd_got_lo = %d entry = 0x%x\n", entry->got_lo, entry->fd_got_lo, entry);
      //printf("symnxd = 0x%x sym_local = %d funcdesc_local = %d\n", entry->symndx,
      //     UBICOM32FPIC_SYM_LOCAL (dinfo->info, entry->d.h),
      //     UBICOM32FPIC_FUNCDESC_LOCAL (dinfo->info, entry->d.h));
      //printf("Name = %s\n\n", entry->d.h->root.root.string);
      dinfo->privfdplt += FUNCTION_DESCRIPTOR_SIZE;
      entry->relocsfdv++;
    }


  if (entry->lazyplt)
    {
      dinfo->num_plts++;
    }

  if (!dinfo->info->executable || dinfo->info->pie)
    relocs = entry->relocs32 + entry->relocsfd + entry->relocsfdv;
  else
    {
      if (entry->symndx != -1 || UBICOM32FPIC_SYM_LOCAL (dinfo->info, entry->d.h))
	{
	  if (entry->symndx != -1
	      || entry->d.h->root.type != bfd_link_hash_undefweak)
	    fixups += entry->relocs32 + 2 * entry->relocsfdv;
	}
      else
	relocs += entry->relocs32 + entry->relocsfdv;

      if (entry->symndx != -1
	  || UBICOM32FPIC_FUNCDESC_LOCAL (dinfo->info, entry->d.h))
	{
	  if (entry->symndx != -1
	      || entry->d.h->root.type != bfd_link_hash_undefweak)
	    fixups += entry->relocsfd;
	}
      else
	relocs += entry->relocsfd;
    }

  entry->dynrelocs += relocs;
  entry->fixups += fixups;
  dinfo->relocs += relocs;
  dinfo->fixups += fixups;

  return 1;
}

/* Create a Ubicom32 ELF linker hash table.  */
static struct bfd_link_hash_table *
ubicom32fdpic_elf_link_hash_table_create (bfd *abfd)
{
  struct ubicom32fdpic_elf_link_hash_table *ret;
  bfd_size_type amt = sizeof (struct ubicom32fdpic_elf_link_hash_table);

  ret = bfd_zalloc (abfd, amt);
  if (ret == NULL)
    return NULL;

  if (!_bfd_elf_link_hash_table_init (&ret->elf, abfd,
				      _bfd_elf_link_hash_newfunc,
				      sizeof (struct elf_link_hash_entry)))
    {
      free (ret);
      return NULL;
    }

  return &ret->elf.root;
}

/* Compute a hash with the key fields of an ubicom32fdpic_relocs_info entry.  */
static hashval_t
ubicom32fdpic_relocs_info_hash (const void *entry_)
{
  const struct ubicom32fdpic_relocs_info *entry = entry_;

  return (entry->symndx == -1
	  ? (long) entry->d.h->root.root.hash
	  : entry->symndx + (long) entry->d.abfd->id * 257) + entry->addend;
}

/* Test whether the key fields of two ubicom32fdpic_relocs_info entries are
   identical.  */
static int
ubicom32fdpic_relocs_info_eq (const void *entry1, const void *entry2)
{
  const struct ubicom32fdpic_relocs_info *e1 = entry1;
  const struct ubicom32fdpic_relocs_info *e2 = entry2;

  return e1->symndx == e2->symndx && e1->addend == e2->addend
    && (e1->symndx == -1 ? e1->d.h == e2->d.h : e1->d.abfd == e2->d.abfd);
}

/* Find or create an entry in a hash table HT that matches the key
   fields of the given ENTRY.  If it's not found, memory for a new
   entry is allocated in ABFD's obstack.  */
static struct ubicom32fdpic_relocs_info *
ubicom32fdpic_relocs_info_find (struct htab *ht,
				bfd *abfd,
				const struct ubicom32fdpic_relocs_info *entry,
				enum insert_option insert)
{
  struct ubicom32fdpic_relocs_info **loc =
    (struct ubicom32fdpic_relocs_info **) htab_find_slot (ht, entry, insert);

  if (! loc)
    return NULL;

  if (*loc)
    return *loc;

  *loc = bfd_zalloc (abfd, sizeof (**loc));

  if (! *loc)
    return *loc;

  (*loc)->symndx = entry->symndx;
  (*loc)->d = entry->d;
  (*loc)->addend = entry->addend;
  (*loc)->plt_entry = (bfd_vma)-1;
  /* (*loc)->lzplt_entry = (bfd_vma)-1; */

  return *loc;
}

/* Obtain the address of the entry in HT associated with H's symbol +
   addend, creating a new entry if none existed.  ABFD is only used
   for memory allocation purposes.  */
inline static struct ubicom32fdpic_relocs_info *
ubicom32fdpic_relocs_info_for_global (struct htab *ht,
				      bfd *abfd,
				      struct elf_link_hash_entry *h,
				      bfd_vma addend,
				      enum insert_option insert)
{
  struct ubicom32fdpic_relocs_info entry;

  entry.symndx = -1;
  entry.d.h = h;
  entry.addend = addend;

  return ubicom32fdpic_relocs_info_find (ht, abfd, &entry, insert);
}

/* Obtain the address of the entry in HT associated with the SYMNDXth
   local symbol of the input bfd ABFD, plus the addend, creating a new
   entry if none existed.  */
inline static struct ubicom32fdpic_relocs_info *
ubicom32fdpic_relocs_info_for_local (struct htab *ht,
				     bfd *abfd,
				     long symndx,
				     bfd_vma addend,
				     enum insert_option insert)
{
  struct ubicom32fdpic_relocs_info entry;

  entry.symndx = symndx;
  entry.d.abfd = abfd;
  entry.addend = addend;

  return ubicom32fdpic_relocs_info_find (ht, abfd, &entry, insert);
}

/* Merge fields set by check_relocs() of two entries that end up being
   mapped to the same (presumably global) symbol.  */

inline static void
ubicom32fdpic_pic_merge_early_relocs_info (struct ubicom32fdpic_relocs_info *e2,
					   struct ubicom32fdpic_relocs_info const *e1)
{
  e2->got_lo |= e1->got_lo;
  e2->got_hi |= e1->got_hi;
  e2->fd_got_lo |= e1->fd_got_lo;
  e2->fd_got_hi |= e1->fd_got_hi;
  e2->fd |= e1->fd;
  e2->gotoff |= e1->gotoff;
  e2->call |= e1->call;
  e2->sym |= e1->sym;
}

/* Add a dynamic relocation to the SRELOC section.  */

inline static bfd_vma
ubicom32fdpic_add_dyn_reloc (bfd *output_bfd, asection *sreloc, bfd_vma offset,
			     int reloc_type, long dynindx, bfd_vma addend,
			     struct ubicom32fdpic_relocs_info *entry)
{
  Elf_Internal_Rela outrel;
  bfd_vma reloc_offset;

  outrel.r_offset = offset;
  outrel.r_info = ELF32_R_INFO (dynindx, reloc_type);
  outrel.r_addend = addend;

  reloc_offset = sreloc->reloc_count * sizeof (Elf32_External_Rel);
  BFD_ASSERT (reloc_offset < sreloc->size);
  bfd_elf32_swap_reloc_out (output_bfd, &outrel,
			    sreloc->contents + reloc_offset);
  sreloc->reloc_count++;

  /* If the entry's index is zero, this relocation was probably to a
     linkonce section that got discarded.  We reserved a dynamic
     relocation, but it was for another entry than the one we got at
     the time of emitting the relocation.  Unfortunately there's no
     simple way for us to catch this situation, since the relocation
     is cleared right before calling relocate_section, at which point
     we no longer know what the relocation used to point to.  */
  if (entry->symndx)
    {
      BFD_ASSERT (entry->dynrelocs > 0);
      entry->dynrelocs--;
    }

  return reloc_offset;
}

/* Add a fixup to the ROFIXUP section.  */

static bfd_vma
ubicom32fdpic_add_rofixup (bfd *output_bfd, asection *rofixup, bfd_vma offset,
			   struct ubicom32fdpic_relocs_info *entry)
{
  bfd_vma fixup_offset;

  if (rofixup->flags & SEC_EXCLUDE)
    return -1;

  fixup_offset = rofixup->reloc_count * 4;
  if (rofixup->contents)
    {
      BFD_ASSERT (fixup_offset < rofixup->size);
      bfd_put_32 (output_bfd, offset, rofixup->contents + fixup_offset);
    }
  rofixup->reloc_count++;

  if (entry && entry->symndx)
    {
      /* See discussion about symndx == 0 in _ubicom32fdpic_add_dyn_reloc
	 above.  */
      BFD_ASSERT (entry->fixups > 0);
      entry->fixups--;
    }

  return fixup_offset;
}

/* Find the segment number in which OSEC, and output section, is
   located.  */

static unsigned
ubicom32fdpic_osec_to_segment (bfd *output_bfd, asection *osec)
{
  Elf_Internal_Phdr *p = _bfd_elf_find_segment_containing_section (output_bfd, osec);

  return (p != NULL) ? p - elf_tdata (output_bfd)->phdr : -1;
}

inline static bfd_boolean
ubicom32fdpic_osec_readonly_p (bfd *output_bfd, asection *osec)
{
  unsigned seg = ubicom32fdpic_osec_to_segment (output_bfd, osec);

  return ! (elf_tdata (output_bfd)->phdr[seg].p_flags & PF_W);
}

#if 0
static bfd_vma plt_trampoline[] = {
  0x00002400,	//ret (a0)
};
#endif

/* Generate relocations for GOT entries, function descriptors, and
   code for PLT and lazy PLT entries.  */

static bfd_boolean
ubicom32fdpic_emit_got_relocs_plt_entries (struct ubicom32fdpic_relocs_info *entry,
					   bfd *output_bfd,
					   struct bfd_link_info *info,
					   asection *sec,
					   Elf_Internal_Sym *sym,
					   bfd_vma addend)

{
  bfd_vma fd_lazy_rel_offset = (bfd_vma)-1;
  int dynindx = -1;

  if (entry->done)
    return TRUE;
  entry->done = 1;

  if (entry->got_entry || entry->fdgot_entry || entry->fd_entry)
    {
      DPRINTF(" emit %p got %d fdgot %d fd %d addend %d\n", entry, entry->got_entry, entry->fdgot_entry, entry->fd_entry, addend);
      /* If the symbol is dynamic, consider it for dynamic
	 relocations, otherwise decay to section + offset.  */
      if (entry->symndx == -1 && entry->d.h->dynindx != -1)
	dynindx = entry->d.h->dynindx;
      else
	{
	  if (sec 
	      && sec->output_section
	      && ! bfd_is_abs_section (sec->output_section)
	      && ! bfd_is_und_section (sec->output_section))
	    dynindx = elf_section_data (sec->output_section)->dynindx;
	  else
	    dynindx = 0;
	}
    }

  /* Generate relocation for GOT entry pointing to the symbol.  */
  if (entry->got_entry)
    {
      DPRINTF(" emit got entry %d:%p\n", entry->got_entry, entry);

      int idx = dynindx;
      bfd_vma ad = addend;

      /* If the symbol is dynamic but binds locally, use
	 section+offset.  */
      if (sec && (entry->symndx != -1
		  || UBICOM32FDPIC_SYM_LOCAL (info, entry->d.h)))
	{
	  if (entry->symndx == -1)
	    ad += entry->d.h->root.u.def.value;
	  else
	    ad += sym->st_value;
	  ad += sec->output_offset;
	  if (sec->output_section && elf_section_data (sec->output_section))
	    idx = elf_section_data (sec->output_section)->dynindx;
	  else
	    idx = 0;
	}

      /* If we're linking an executable at a fixed address, we can
	 omit the dynamic relocation as long as the symbol is local to
	 this module.  */
      if (info->executable && !info->pie
	  && (entry->symndx != -1
	      || UBICOM32FDPIC_SYM_LOCAL (info, entry->d.h)))
	{
	  if (sec)
	    ad += sec->output_section->vma;
	  if (entry->symndx != -1
	      || entry->d.h->root.type != bfd_link_hash_undefweak)
	    ubicom32fdpic_add_rofixup (output_bfd,
				       ubicom32fdpic_gotfixup_section (info),
				       ubicom32fdpic_got_section (info)->output_section->vma
				       + ubicom32fdpic_got_section (info)->output_offset
				       + ubicom32fdpic_got_initial_offset (info)
				       + entry->got_entry, entry);
	}
      else
	ubicom32fdpic_add_dyn_reloc (output_bfd, ubicom32fdpic_gotrel_section (info),
				     _bfd_elf_section_offset
				     (output_bfd, info,
				      ubicom32fdpic_got_section (info),
				      ubicom32fdpic_got_initial_offset (info)
				      + entry->got_entry)
				     + ubicom32fdpic_got_section (info)
				     ->output_section->vma
				     + ubicom32fdpic_got_section (info)->output_offset,
				     R_UBICOM32_32, idx, ad, entry);

      bfd_put_32 (output_bfd, ad,
		  ubicom32fdpic_got_section (info)->contents
		  + ubicom32fdpic_got_initial_offset (info)
		  + entry->got_entry);
    }

  /* Generate relocation for GOT entry pointing to a canonical
     function descriptor.  */
  if (entry->fdgot_entry)
    {
      DPRINTF(" emit got fdgot entry %d:%p\n", entry->fdgot_entry, entry);

      int reloc, idx;
      bfd_vma ad = 0;

      if (! (entry->symndx == -1
	     && entry->d.h->root.type == bfd_link_hash_undefweak
	     && UBICOM32FDPIC_SYM_LOCAL (info, entry->d.h)))
	{
	  /* If the symbol is dynamic and there may be dynamic symbol
	     resolution because we are, or are linked with, a shared
	     library, emit a FUNCDESC relocation such that the dynamic
	     linker will allocate the function descriptor.  If the
	     symbol needs a non-local function descriptor but binds
	     locally (e.g., its visibility is protected, emit a
	     dynamic relocation decayed to section+offset.  */
	  if (entry->symndx == -1
	      && ! UBICOM32FDPIC_FUNCDESC_LOCAL (info, entry->d.h)
	      && UBICOM32FDPIC_SYM_LOCAL (info, entry->d.h)
	      && !(info->executable && !info->pie))
	    {
	      reloc = R_UBICOM32_FUNCDESC;
	      idx = elf_section_data (entry->d.h->root.u.def.section
				      ->output_section)->dynindx;
	      ad = entry->d.h->root.u.def.section->output_offset
		+ entry->d.h->root.u.def.value;
	    }
	  else if (entry->symndx == -1
		   && ! UBICOM32FDPIC_FUNCDESC_LOCAL (info, entry->d.h))
	    {
	      reloc = R_UBICOM32_FUNCDESC;
	      idx = dynindx;
	      ad = addend;
	      if (ad)
		return FALSE;
	    }
	  else
	    {
	      /* Otherwise, we know we have a private function descriptor,
		 so reference it directly.  */
	      if (elf_hash_table (info)->dynamic_sections_created)
		BFD_ASSERT (entry->privfd);
	      reloc = R_UBICOM32_32;
	      idx = elf_section_data (ubicom32fdpic_got_section (info)
				      ->output_section)->dynindx;
	      ad = ubicom32fdpic_got_section (info)->output_offset
		+ ubicom32fdpic_got_initial_offset (info) + entry->fd_entry;
	    }

	  /* If there is room for dynamic symbol resolution, emit the
	     dynamic relocation.  However, if we're linking an
	     executable at a fixed location, we won't have emitted a
	     dynamic symbol entry for the got section, so idx will be
	     zero, which means we can and should compute the address
	     of the private descriptor ourselves.  */
	  if (info->executable && !info->pie
	      && (entry->symndx != -1
		  || UBICOM32FDPIC_FUNCDESC_LOCAL (info, entry->d.h)))
	    {
	      ad += ubicom32fdpic_got_section (info)->output_section->vma;
	      ubicom32fdpic_add_rofixup (output_bfd,
					 ubicom32fdpic_gotfixup_section (info),
					 ubicom32fdpic_got_section (info)
					 ->output_section->vma
					 + ubicom32fdpic_got_section (info)
					 ->output_offset
					 + ubicom32fdpic_got_initial_offset (info)
					 + entry->fdgot_entry, entry);
	    }
	  else
	    ubicom32fdpic_add_dyn_reloc (output_bfd,
					 ubicom32fdpic_gotrel_section (info),
					 _bfd_elf_section_offset
					 (output_bfd, info,
					  ubicom32fdpic_got_section (info),
					  ubicom32fdpic_got_initial_offset (info)
					  + entry->fdgot_entry)
					 + ubicom32fdpic_got_section (info)
					 ->output_section->vma
					 + ubicom32fdpic_got_section (info)
					 ->output_offset,
					 reloc, idx, ad, entry);
	}

      bfd_put_32 (output_bfd, ad,
		  ubicom32fdpic_got_section (info)->contents
		  + ubicom32fdpic_got_initial_offset (info)
		  + entry->fdgot_entry);
    }

  /* Generate relocation to fill in a private function descriptor in
     the GOT.  */
  if (entry->fd_entry)
    {

      int idx = dynindx;
      bfd_vma ad = addend;
      bfd_vma ofst;
      long lowword, highword;

      /* If the symbol is dynamic but binds locally, use
	 section+offset.  */
      if (sec && (entry->symndx != -1
		  || UBICOM32FDPIC_SYM_LOCAL (info, entry->d.h)))
	{
	  if (entry->symndx == -1)
	    ad += entry->d.h->root.u.def.value;
	  else
	    ad += sym->st_value;
	  ad += sec->output_offset;
	  if (sec->output_section && elf_section_data (sec->output_section))
	    idx = elf_section_data (sec->output_section)->dynindx;
	  else
	    idx = 0;
	}

      /* If we're linking an executable at a fixed address, we can
	 omit the dynamic relocation as long as the symbol is local to
	 this module.  */
      if (info->executable && !info->pie
	  && (entry->symndx != -1 || UBICOM32FDPIC_SYM_LOCAL (info, entry->d.h)))
	{
	  if (sec)
	    ad += sec->output_section->vma;
	  ofst = 0;
	  if (entry->symndx != -1
	      || entry->d.h->root.type != bfd_link_hash_undefweak)
	    {
	      ubicom32fdpic_add_rofixup (output_bfd,
					 ubicom32fdpic_gotfixup_section (info),
					 ubicom32fdpic_got_section (info)
					 ->output_section->vma
					 + ubicom32fdpic_got_section (info)
					 ->output_offset
					 + ubicom32fdpic_got_initial_offset (info)
					 + entry->fd_entry, entry);
	      ubicom32fdpic_add_rofixup (output_bfd,
					 ubicom32fdpic_gotfixup_section (info),
					 ubicom32fdpic_got_section (info)
					 ->output_section->vma
					 + ubicom32fdpic_got_section (info)
					 ->output_offset
					 + ubicom32fdpic_got_initial_offset (info)
					 + entry->fd_entry + 4, entry);
	    }
	}
      else
	{
	  ofst
	    = ubicom32fdpic_add_dyn_reloc (output_bfd,
					   entry->lazyplt
					   ? ubicom32fdpic_pltrel_section (info)
					   : ubicom32fdpic_gotrel_section (info),
					   _bfd_elf_section_offset
					   (output_bfd, info,
					    ubicom32fdpic_got_section (info),
					    ubicom32fdpic_got_initial_offset (info)
					    + entry->fd_entry)
					   + ubicom32fdpic_got_section (info)
					   ->output_section->vma
					   + ubicom32fdpic_got_section (info)
					   ->output_offset,
					   R_UBICOM32_FUNCDESC_VALUE, idx, ad, entry);
	}

      /* If we've omitted the dynamic relocation, just emit the fixed
	 addresses of the symbol and of the local GOT base offset.  */
      if (info->executable && !info->pie && sec && sec->output_section)
	{
	  lowword = ad;
	  highword = ubicom32fdpic_got_section (info)->output_section->vma
	    + ubicom32fdpic_got_section (info)->output_offset
	    + ubicom32fdpic_got_initial_offset (info);
	}
      else if (entry->lazyplt)
	{
	  if (ad)
	    return FALSE;

	  fd_lazy_rel_offset = ofst;

	  /* A function descriptor used for lazy or local resolving is
	     initialized such that its high word contains the output
	     section index in which the PLT entries are located, and
	     the low word contains the address to the base of the PLT.
	     That location contains the PLT trampoline instruction ret 0(a0).
	     assigned to that section.  */
	  lowword =  ubicom32fdpic_plt_section (info)->output_offset
	    + ubicom32fdpic_plt_section (info)->output_section->vma + entry->plt_trampoline_entry;
	  highword = ubicom32fdpic_osec_to_segment
	    (output_bfd, ubicom32fdpic_plt_section (info)->output_section);
	}
      else
	{
	  /* A function descriptor for a local function gets the index
	     of the section.  For a non-local function, it's
	     disregarded.  */
	  lowword = ad;
	  if (entry->symndx == -1 && entry->d.h->dynindx != -1
	      && entry->d.h->dynindx == idx)
	    highword = 0;
	  else
	    {
	      if (sec)
		highword = ubicom32fdpic_osec_to_segment (output_bfd, sec->output_section);
	      else
		highword = 0;
	    }	  
	}

      DPRINTF(" emit got fd_entry %d:%p lw 0x%x hw 0x%x fd_l_r_off 0x%x\n", entry->fd_entry, entry, lowword, highword, fd_lazy_rel_offset);


      bfd_put_32 (output_bfd, lowword,
		  ubicom32fdpic_got_section (info)->contents
		  + ubicom32fdpic_got_initial_offset (info)
		  + entry->fd_entry);
      bfd_put_32 (output_bfd, highword,
		  ubicom32fdpic_got_section (info)->contents
		  + ubicom32fdpic_got_initial_offset (info)
		  + entry->fd_entry + 4);

#if 0
      /* Load the fixup offset here. */
      bfd_put_32 (output_bfd, fd_lazy_rel_offset,
		  ubicom32fdpic_got_section (info)->contents
		  + ubicom32fdpic_got_initial_offset (info)
		  + entry->fd_entry + 8);
#endif

      entry->rel_offset = fd_lazy_rel_offset;
    }

  /* Generate code for the PLT entry.  */
  if (entry->plt_entry != (bfd_vma) -1)
    {
#if 0
      static int output_trampoline_code = 1;
#endif
      bfd_byte *plt_output_code = ubicom32fdpic_plt_section (info)->contents;
      unsigned int i;
      bfd_vma *plt_code;

      DPRINTF(" emit fd entry %x:%p plt=%2x code=%p\n", entry->fd_entry, entry, entry->plt_entry, plt_output_code);

#if 0
      if (output_trampoline_code)
	{
	  /* output the trampoline code.*/
	  bfd_put_32 (output_bfd, plt_trampoline[0], plt_output_code);
	}
#endif

      /* output the trampoline entry. */

      plt_output_code += entry->plt_trampoline_entry;
      plt_code = plt_trampoline;
      plt_code[0] = (plt_code[0] & 0xFFFF0000) | (entry->rel_offset & 0xffff);
      bfd_put_32 (output_bfd, plt_code[0], plt_output_code);
      bfd_put_32 (output_bfd, plt_code[1], plt_output_code + 4);


      /* output the plt itself. */
      plt_output_code = ubicom32fdpic_plt_section (info)->contents;
      plt_output_code += entry->plt_entry;
      BFD_ASSERT (entry->fd_entry);

      if (entry->plt_type == 2)
	{
	  bfd_vma data_lo = (entry->fd_entry >> 2) & 0xff;

	  /* Output seqence 2. */
	  plt_code = plt_code_seq2;

	  /* Code the entry into the PDEC instruction. */
	  plt_code[0] &= 0xFFFFF8E0;
	  plt_code[0] |= (data_lo & 0x1F);
	  plt_code[0] |= (data_lo & 0xE0) << 3;

	  /* Write out the sequence. */
	  for (i = 0; i < NUM_PLT_CODE_WORDS_SEQ2; i++)
	    {
	      bfd_put_32 (output_bfd, plt_code[i], plt_output_code);
	      plt_output_code += 4;
	    }
	}
      else if (entry->plt_type == 1)
	{
	  /* Outupt sequence 1 */
	  plt_code = plt_code_seq1;

	  /* Code the entry into the movei instruction. */
	  plt_code[0] = (plt_code[0] & 0xFFFF0000) | ((entry->fd_entry >> 2) & 0xFFFF);

	  /* Write out the sequence. */
	  for (i = 0; i < NUM_PLT_CODE_WORDS_SEQ1; i++)
	    {
	      bfd_put_32 (output_bfd, plt_code[i], plt_output_code);
	      plt_output_code += 4;
	    }
	}
      else
	BFD_ASSERT(0);
    }

  return TRUE;
}


/* Create  a .got section, as well as its additional info field.  This
   is almost entirely copied from
   elflink.c:_bfd_elf_create_got_section().  */

static bfd_boolean
ubicom32fdpic_elf_create_got_section (bfd *abfd, struct bfd_link_info *info)
{
  flagword flags, pltflags;
  asection *s;
  struct elf_link_hash_entry *h;
  const struct elf_backend_data *bed = get_elf_backend_data (abfd);
  int ptralign;
  int offset;

  /* This function may be called more than once.  */
  s = bfd_get_section_by_name (abfd, ".got");
  if (s != NULL && (s->flags & SEC_LINKER_CREATED) != 0)
    return TRUE;

  /* Machine specific: although pointers are 32-bits wide, we want the
     GOT to be aligned to a 64-bit boundary, such that function
     descriptors in it can be accessed with 64-bit loads and
     stores.  */
  ptralign = 3;

  flags = (SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS | SEC_IN_MEMORY
	   | SEC_LINKER_CREATED);
  pltflags = flags;

  s = bfd_make_section_with_flags (abfd, ".got", flags);
  if (s == NULL
      || !bfd_set_section_alignment (abfd, s, ptralign))
    return FALSE;

  if (bed->want_got_plt)
    {
      s = bfd_make_section_with_flags (abfd, ".got.plt", flags);
      if (s == NULL
	  || !bfd_set_section_alignment (abfd, s, ptralign))
	return FALSE;
    }

  if (bed->want_got_sym)
    {
      /* Define the symbol _GLOBAL_OFFSET_TABLE_ at the start of the .got
	 (or .got.plt) section.  We don't do this in the linker script
	 because we don't want to define the symbol if we are not creating
	 a global offset table.  */
      h = _bfd_elf_define_linkage_sym (abfd, info, s, "_GLOBAL_OFFSET_TABLE_");
      elf_hash_table (info)->hgot = h;
      if (h == NULL)
	return FALSE;

      /* Machine-specific: we want the symbol for executables as
	 well.  */
      if (! bfd_elf_link_record_dynamic_symbol (info, h))
	return FALSE;
    }

  /* The first bit of the global offset table is the header.  */
  s->size += bed->got_header_size;

  /* This is the machine-specific part.  Create and initialize section
     data for the got.  */
  if (IS_FDPIC (abfd))
    {
      ubicom32fdpic_got_section (info) = s;
      ubicom32fdpic_relocs_info (info) = htab_try_create (1,
							  ubicom32fdpic_relocs_info_hash,
							  ubicom32fdpic_relocs_info_eq,
							  (htab_del) NULL);
      if (! ubicom32fdpic_relocs_info (info))
	return FALSE;

      s = bfd_make_section_with_flags (abfd, ".rel.got",
				       (flags | SEC_READONLY));
      if (s == NULL
	  || ! bfd_set_section_alignment (abfd, s, 2))
	return FALSE;

      ubicom32fdpic_gotrel_section (info) = s;

      /* Machine-specific.  */
      s = bfd_make_section_with_flags (abfd, ".rofixup",
				       (flags | SEC_READONLY));
      if (s == NULL
	  || ! bfd_set_section_alignment (abfd, s, 2))
	return FALSE;

      ubicom32fdpic_gotfixup_section (info) = s;
      offset = -2048;
      flags = BSF_GLOBAL;
    }
  else
    {
      offset = 2048;
      flags = BSF_GLOBAL | BSF_WEAK;
    }

  return TRUE;
}

/* Make sure the got and plt sections exist, and that our pointers in
   the link hash table point to them.  */

static bfd_boolean
ubicom32fdpic_elf_create_dynamic_sections (bfd *abfd, struct bfd_link_info *info)
{  flagword flags, pltflags;
  asection *s;
  const struct elf_backend_data *bed = get_elf_backend_data (abfd);

  /* We need to create .plt, .rel[a].plt, .got, .got.plt, .dynbss, and
     .rel[a].bss sections.  */
  DPRINTF(" ubicom32fdpic_elf_create_dynamic_sections %p %p\n", abfd, info);

  flags = (SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS | SEC_IN_MEMORY
	   | SEC_LINKER_CREATED);

  pltflags = flags;
  pltflags |= SEC_CODE;
  if (bed->plt_not_loaded)
    pltflags &= ~ (SEC_CODE | SEC_LOAD | SEC_HAS_CONTENTS);
  if (bed->plt_readonly)
    pltflags |= SEC_READONLY;

  s = bfd_make_section_with_flags (abfd, ".plt", pltflags);
  if (s == NULL
      || ! bfd_set_section_alignment (abfd, s, bed->plt_alignment))
    return FALSE;
  /* Blackfin-specific: remember it.  */
  ubicom32fdpic_plt_section (info) = s;

  if (bed->want_plt_sym)
    {
      /* Define the symbol _PROCEDURE_LINKAGE_TABLE_ at the start of the
	 .plt section.  */
      struct elf_link_hash_entry *h;
      struct bfd_link_hash_entry *bh = NULL;

      if (! (_bfd_generic_link_add_one_symbol
	     (info, abfd, "_PROCEDURE_LINKAGE_TABLE_", BSF_GLOBAL, s, 0, NULL,
	      FALSE, get_elf_backend_data (abfd)->collect, &bh)))
	return FALSE;
      h = (struct elf_link_hash_entry *) bh;
      h->def_regular = 1;
      h->type = STT_OBJECT;

      if (! info->executable
	  && ! bfd_elf_link_record_dynamic_symbol (info, h))
	return FALSE;
    }

  /* Blackfin-specific: we want rel relocations for the plt.  */
  s = bfd_make_section_with_flags (abfd, ".rel.plt", flags | SEC_READONLY);
  if (s == NULL
      || ! bfd_set_section_alignment (abfd, s, bed->s->log_file_align))
    return FALSE;
  /* Blackfin-specific: remember it.  */
  ubicom32fdpic_pltrel_section (info) = s;

  /* Blackfin-specific: we want to create the GOT in the Blackfin way.  */
  if (! ubicom32fdpic_elf_create_got_section (abfd, info))
    return FALSE;

  /* Blackfin-specific: make sure we created everything we wanted.  */
  BFD_ASSERT (ubicom32fdpic_got_section (info) && ubicom32fdpic_gotrel_section (info)
	      /* && ubicom32fdpic_gotfixup_section (info) */
	      && ubicom32fdpic_plt_section (info)
	      && ubicom32fdpic_pltrel_section (info));

  if (bed->want_dynbss)
    {
      /* The .dynbss section is a place to put symbols which are defined
	 by dynamic objects, are referenced by regular objects, and are
	 not functions.  We must allocate space for them in the process
	 image and use a R_*_COPY reloc to tell the dynamic linker to
	 initialize them at run time.  The linker script puts the .dynbss
	 section into the .bss section of the final image.  */
      s = bfd_make_section_with_flags (abfd, ".dynbss",
				       SEC_ALLOC | SEC_LINKER_CREATED);
      if (s == NULL)
	return FALSE;

      /* The .rel[a].bss section holds copy relocs.  This section is not
     normally needed.  We need to create it here, though, so that the
     linker will map it to an output section.  We can't just create it
     only if we need it, because we will not know whether we need it
     until we have seen all the input files, and the first time the
     main linker code calls BFD after examining all the input files
     (size_dynamic_sections) the input sections have already been
     mapped to the output sections.  If the section turns out not to
     be needed, we can discard it later.  We will never need this
     section when generating a shared object, since they do not use
     copy relocs.  */
      if (! info->shared)
	{
	  s = bfd_make_section_with_flags (abfd,
					   (bed->default_use_rela_p
					    ? ".rela.bss" : ".rel.bss"),
					   flags | SEC_READONLY);
	  if (s == NULL
	      || ! bfd_set_section_alignment (abfd, s, bed->s->log_file_align))
	    return FALSE;
	}
    }

  return TRUE;
}

/* We need dynamic symbols for every section, since segments can
   relocate independently.  */
static bfd_boolean
ubicom32fdpic_elf_link_omit_section_dynsym (bfd *output_bfd ATTRIBUTE_UNUSED,
					    struct bfd_link_info *info
					    ATTRIBUTE_UNUSED,
					    asection *p ATTRIBUTE_UNUSED)
{
  switch (elf_section_data (p)->this_hdr.sh_type)
    {
    case SHT_PROGBITS:
    case SHT_NOBITS:
      /* If sh_type is yet undecided, assume it could be
	 SHT_PROGBITS/SHT_NOBITS.  */
    case SHT_NULL:
      return FALSE;

      /* There shouldn't be section relative relocations
	 against any other section.  */
    default:
      return TRUE;
    }
}

/* Look through the relocs for a section during the first phase.

   Besides handling virtual table relocs for gc, we have to deal with
   all sorts of PIC-related relocations.  We describe below the
   general plan on how to handle such relocations, even though we only
   collect information at this point, storing them in hash tables for
   perusal of later passes.

*/
static bfd_boolean
ubicom32fdpic_elf_check_relocs (bfd *abfd, struct bfd_link_info *info,
				asection *sec, const Elf_Internal_Rela *relocs)
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes, **sym_hashes_end;
  const Elf_Internal_Rela *rel;
  const Elf_Internal_Rela *rel_end;
  bfd *dynobj;
  struct ubicom32fdpic_relocs_info *picrel;

  if (info->relocatable)
    return TRUE;

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (abfd);
  sym_hashes_end = sym_hashes + symtab_hdr->sh_size/sizeof(Elf32_External_Sym);
  if (!elf_bad_symtab (abfd))
    sym_hashes_end -= symtab_hdr->sh_info;

  dynobj = elf_hash_table (info)->dynobj;
  rel_end = relocs + sec->reloc_count;
  for (rel = relocs; rel < rel_end; rel++)
    {
      struct elf_link_hash_entry *h;
      unsigned long r_symndx;

      r_symndx = ELF32_R_SYM (rel->r_info);
      if (r_symndx < symtab_hdr->sh_info)
	h = NULL;
      else
	h = sym_hashes[r_symndx - symtab_hdr->sh_info];

      switch (ELF32_R_TYPE (rel->r_info))
	{
	case R_UBICOM32_GOT_HI:
	case R_UBICOM32_GOT_LO:
	case R_UBICOM32_GOT_LO_LEAI:
	case R_UBICOM32_FUNCDESC_GOT_HI:
	case R_UBICOM32_FUNCDESC_GOT_LO:
	case R_UBICOM32_FUNCDESC_GOT_LO_LEAI:
	case R_UBICOM32_FUNCDESC:
	case R_UBICOM32_FUNCDESC_VALUE:
	  if (! IS_FDPIC (abfd))
	    goto bad_reloc;
	  /* Fall through.  */
	case R_UBICOM32_24_PCREL:
	case R_UBICOM32_32:
	  if (IS_FDPIC (abfd) && ! dynobj)
	    {
	      elf_hash_table (info)->dynobj = dynobj = abfd;
	      if (! ubicom32fdpic_elf_create_got_section (abfd, info))
		return FALSE;
	    }
	  if (! IS_FDPIC (abfd))
	    {
	      picrel = NULL;
	      break;
	    }
	  if (h != NULL)
	    {
	      if (h->dynindx == -1)
		switch (ELF_ST_VISIBILITY (h->other))
		  {
		  case STV_INTERNAL:
		  case STV_HIDDEN:
		    break;
		  default:
		    bfd_elf_link_record_dynamic_symbol (info, h);
		    break;
		  }
	      picrel
		= ubicom32fdpic_relocs_info_for_global (ubicom32fdpic_relocs_info (info),
							abfd, h,
							rel->r_addend, INSERT);
	    }
	  else
	    picrel = ubicom32fdpic_relocs_info_for_local (ubicom32fdpic_relocs_info (info),
							  abfd, r_symndx,
							  rel->r_addend, INSERT);
	  if (! picrel)
	    return FALSE;
	  break;

	default:
	  picrel = NULL;
	  break;
	}

      switch (ELF32_R_TYPE (rel->r_info))
	{
	case R_UBICOM32_24_PCREL:
	  if (IS_FDPIC (abfd))
	    picrel->call++;
	  break;

	case R_UBICOM32_FUNCDESC_VALUE:
	  picrel->relocsfdv++;
	  picrel->sym++;
	  break;

	case R_UBICOM32_32:
	  if (! IS_FDPIC (abfd))
	    break;

	  picrel->sym++;
	  if (bfd_get_section_flags (abfd, sec) & SEC_ALLOC)
	    picrel->relocs32++;
	  break;

	case R_UBICOM32_GOT_HI:
	  picrel->got_hi++;
	  break;

	case R_UBICOM32_GOT_LO:
	case R_UBICOM32_GOT_LO_LEAI:
	  picrel->got_lo++;
	  break;

	case R_UBICOM32_FUNCDESC_GOT_HI:
	  picrel->fd_got_hi++;
	  break;

	case R_UBICOM32_FUNCDESC_GOT_LO:
	case R_UBICOM32_FUNCDESC_GOT_LO_LEAI:
	  picrel->fd_got_lo++;
	  break;

	case R_UBICOM32_FUNCDESC:
	  picrel->fd++;
	  picrel->relocsfd++;
	  break;

	  /* This relocation describes the C++ object vtable hierarchy.
	     Reconstruct it for later use during GC.  */
	case R_UBICOM32_GNU_VTINHERIT:
	  if (!bfd_elf_gc_record_vtinherit (abfd, sec, h, rel->r_offset))
	    return FALSE;
	  break;

	  /* This relocation describes which C++ vtable entries are actually
	     used.  Record for later use during GC.  */
	case R_UBICOM32_GNU_VTENTRY:
	  if (!bfd_elf_gc_record_vtentry (abfd, sec, h, rel->r_addend))
	    return FALSE;
	  break;

	case R_UBICOM32_21_PCREL:
	case R_UBICOM32_HI24:
	case R_UBICOM32_LO7_S:
	  break;

	default:
	bad_reloc:
	  (*_bfd_error_handler)
	    (_("%B: unsupported (ubicom32) relocation type %i"),
	     abfd, ELF32_R_TYPE (rel->r_info));
	  return FALSE;
	}
    }

  return TRUE;
}

/* Follow indirect and warning hash entries so that each got entry
   points to the final symbol definition.  P must point to a pointer
   to the hash table we're traversing.  Since this traversal may
   modify the hash table, we set this pointer to NULL to indicate
   we've made a potentially-destructive change to the hash table, so
   the traversal must be restarted.  */
static int
ubicom32fdpic_resolve_final_relocs_info (void **entryp, void *p)
{
  struct ubicom32fdpic_relocs_info *entry = *entryp;
  htab_t *htab = p;

  if (entry->symndx == -1)
    {
      struct elf_link_hash_entry *h = entry->d.h;
      struct ubicom32fdpic_relocs_info *oentry;

      while (h->root.type == bfd_link_hash_indirect
	     || h->root.type == bfd_link_hash_warning)
	h = (struct elf_link_hash_entry *)h->root.u.i.link;

      if (entry->d.h == h)
	return 1;

      oentry = ubicom32fdpic_relocs_info_for_global (*htab, 0, h, entry->addend,
						     NO_INSERT);

      if (oentry)
	{
	  /* Merge the two entries.  */
	  ubicom32fdpic_pic_merge_early_relocs_info (oentry, entry);
	  htab_clear_slot (*htab, entryp);
	  return 1;
	}

      entry->d.h = h;

      /* If we can't find this entry with the new bfd hash, re-insert
	 it, and get the traversal restarted.  */
      if (! htab_find (*htab, entry))
	{
	  htab_clear_slot (*htab, entryp);
	  entryp = htab_find_slot (*htab, entry, INSERT);
	  if (! *entryp)
	    *entryp = entry;
	  /* Abort the traversal, since the whole table may have
	     moved, and leave it up to the parent to restart the
	     process.  */
	  *(htab_t *)p = NULL;
	  return 0;
	}
    }

  return 1;
}

/* Assign GOT offsets to private function descriptors used by PLT
   entries (or referenced by 32-bit offsets), as well as PLT entries
   and lazy PLT entries.  */
static int
ubicom32fdpic_assign_plt_entries (void **entryp, void *info_)
{
  struct ubicom32fdpic_relocs_info *entry = *entryp;
  struct ubicom32fdpic_dynamic_got_plt_info *dinfo = info_;

  if (entry->privfd && entry->fd_entry == 0)
    {
      //      dinfo->current_fd -= FUNCTION_DESCRIPTOR_SIZE;
      //      entry->fd_entry = dinfo->current_fd;
      DPRINTF(" late assign fd  % 5d:%p \n", entry->fd_entry, entry);
    }

  if (entry->plt)
    {
      /* We use the section's raw size to mark the location of the
	 next PLT entry.  */
      entry->plt_entry = dinfo->current_plt;
      entry->plt_trampoline_entry = dinfo->current_plt_trampoline;
      dinfo->current_plt_trampoline += PLT_TRAMPOLINE_SIZE;

      if (entry->fd_entry >= (-512))
	{
	  /* This entry is going to be of type seq2 */
	  dinfo->current_plt += LZPLT_SIZE_SEQ2;
	  entry->plt_type = 2;
	}
      else
	{
	  /* This entry is going to be of type seq1 */
	  dinfo->current_plt += LZPLT_SIZE_SEQ1;
	  entry->plt_type = 1;
	}
      DPRINTF(" assign plt % 4d for fd=% 4d:%p next %d plttype %d\n", entry->plt_entry, entry->fd_entry, entry,  dinfo->current_plt, entry->plt_type);

    }

  return 1;
}

/* Assign GOT offsets for every GOT entry and function descriptor.
   Doing everything in a single pass is tricky.  */
static int
ubicom32fdpic_assign_got_entries (void **entryp, void *info_)
{
  struct ubicom32fdpic_relocs_info *entry = *entryp;
  struct ubicom32fdpic_dynamic_got_plt_info *dinfo = info_;

  if (entry->got_lo || entry->got_hi)
    {
      entry->got_entry = dinfo->current_got;
      DPRINTF(" assign got % 5d:%p \n", entry->got_entry, entry);
      dinfo->current_got += 4;
    }

  if (entry->fd_got_lo || entry->fd_got_hi)
    {
      entry->fdgot_entry = dinfo->current_got;
      DPRINTF(" assign fdgot % 5d:%p \n", entry->fdgot_entry, entry);
      dinfo->current_got += 4;
    }

  if (entry->plt)
    {
      dinfo->current_fd -= FUNCTION_DESCRIPTOR_SIZE;
      entry->fd_entry = dinfo->current_fd;

      dinfo->total_trampoline += PLT_TRAMPOLINE_SIZE;

      if (entry->fd_entry >= (-512))
	{
	  /* This entry is going to be of type seq2 */
	  dinfo->total_lzplt += LZPLT_SIZE_SEQ2;
	  entry->plt_type = 2;
	}
      else
	{
	  /* This entry is going to be of type seq1 */
	  dinfo->total_lzplt += LZPLT_SIZE_SEQ1;
	  entry->plt_type = 1;
	}

      DPRINTF(" assign fd  % 5d:%p \n", entry->fd_entry, entry);
    }
  else if (entry->privfd)
    {
      dinfo->current_privfd -= FUNCTION_DESCRIPTOR_SIZE;
      entry->fd_entry = dinfo->current_privfd;
      DPRINTF(" assign private fd  % 5d:%p %p \n", entry->fd_entry, entry, entry->plt);
    }

  return 1;
}

/* Set the sizes of the dynamic sections.  */

static bfd_boolean
ubicom32fdpic_elf_size_dynamic_sections (bfd *output_bfd,
					 struct bfd_link_info *info)
{
  bfd *dynobj;
  asection *s;
  struct ubicom32fdpic_dynamic_got_plt_info gpinfo;
  bfd_vma total_plt_size;

  dynobj = elf_hash_table (info)->dynobj;
  BFD_ASSERT (dynobj != NULL);

  if (elf_hash_table (info)->dynamic_sections_created)
    {
      /* Set the contents of the .interp section to the interpreter.  */
      if (info->executable)
	{
	  s = bfd_get_section_by_name (dynobj, ".interp");
	  BFD_ASSERT (s != NULL);
	  s->size = sizeof ELF_DYNAMIC_INTERPRETER;
	  s->contents = (bfd_byte *) ELF_DYNAMIC_INTERPRETER;
	}
    }

  memset (&gpinfo, 0, sizeof (gpinfo));
  gpinfo.g.info = info;

  for (;;)
    {
      htab_t relocs = ubicom32fdpic_relocs_info (info);

      htab_traverse (relocs, ubicom32fdpic_resolve_final_relocs_info, &relocs);

      if (relocs == ubicom32fdpic_relocs_info (info))
	break;
    }

  htab_traverse (ubicom32fdpic_relocs_info (info), ubicom32fdpic_count_got_plt_entries,
		 &gpinfo.g);

  /* At this point we know how many PLT entries we need. We know how many got entries we need and the total number of function descriptors in this link. */
  gpinfo.total_fdplt = gpinfo.g.fdplt + gpinfo.g.privfdplt;
  gpinfo.total_got = gpinfo.g.got_lo;
  gpinfo.total_lzplt = 0;

  gpinfo.current_got = 12;	/* The first 12 bytes are reserved to get to resolver. */
  gpinfo.current_fd = 0;	/* We will decrement this by FUNCTION_DESCRIPTOR_SIZE for each allocation. */
  gpinfo.current_privfd = -gpinfo.g.fdplt;	/* We will decrement this by FUNCTION_DESCRIPTOR_SIZE for each allocation. */
  gpinfo.current_plt = 0;	/* Initialize this to 0. The trampoline code is at the start of the plt section.
				   We will decrement this by LZPLT_SIZE* each time we allocate. */
  gpinfo.current_plt_trampoline = 0;

  DPRINTF("Total plts = %d \n", gpinfo.g.num_plts);

  /* Now assign (most) GOT offsets.  */
  htab_traverse (ubicom32fdpic_relocs_info (info), ubicom32fdpic_assign_got_entries,
		 &gpinfo);


  ubicom32fdpic_got_section (info)->size = gpinfo.total_fdplt + gpinfo.total_got + 12;

  DPRINTF("GOT size = fd=%d, got=%d\n", gpinfo.total_fdplt, gpinfo.total_got);

  if (ubicom32fdpic_got_section (info)->size == 0)
    ubicom32fdpic_got_section (info)->flags |= SEC_EXCLUDE;
  else if (ubicom32fdpic_got_section (info)->size == 12
	   && ! elf_hash_table (info)->dynamic_sections_created)
    {
      ubicom32fdpic_got_section (info)->flags |= SEC_EXCLUDE;
      ubicom32fdpic_got_section (info)->size = 0;
    }
  else
    {
      DPRINTF(" Alloc GOT size = %d\n", ubicom32fdpic_got_section (info)->size);
      ubicom32fdpic_got_section (info)->contents =
	(bfd_byte *) bfd_zalloc (dynobj,
				 ubicom32fdpic_got_section (info)->size);
      if (ubicom32fdpic_got_section (info)->contents == NULL)
	return FALSE;
    }

  if (elf_hash_table (info)->dynamic_sections_created)
    /* Subtract the number of lzplt entries, since those will generate
       relocations in the pltrel section.  */
    ubicom32fdpic_gotrel_section (info)->size =
      (gpinfo.g.relocs - gpinfo.g.num_plts)
      * get_elf_backend_data (output_bfd)->s->sizeof_rel;
  else
    BFD_ASSERT (gpinfo.g.relocs == 0);
  if (ubicom32fdpic_gotrel_section (info)->size == 0)
    ubicom32fdpic_gotrel_section (info)->flags |= SEC_EXCLUDE;
  else
    {
      ubicom32fdpic_gotrel_section (info)->contents =
	(bfd_byte *) bfd_zalloc (dynobj,
				 ubicom32fdpic_gotrel_section (info)->size);
      if (ubicom32fdpic_gotrel_section (info)->contents == NULL)
	return FALSE;
    }

  ubicom32fdpic_gotfixup_section (info)->size = (gpinfo.g.fixups + 1) * 4;
  if (ubicom32fdpic_gotfixup_section (info)->size == 0)
    ubicom32fdpic_gotfixup_section (info)->flags |= SEC_EXCLUDE;
  else
    {
      ubicom32fdpic_gotfixup_section (info)->contents =
	(bfd_byte *) bfd_zalloc (dynobj,
				 ubicom32fdpic_gotfixup_section (info)->size);
      if (ubicom32fdpic_gotfixup_section (info)->contents == NULL)
	return FALSE;
    }

  if (elf_hash_table (info)->dynamic_sections_created)
    {
      ubicom32fdpic_pltrel_section (info)->size =
	gpinfo.g.num_plts * get_elf_backend_data (output_bfd)->s->sizeof_rel;
      if (ubicom32fdpic_pltrel_section (info)->size == 0)
	ubicom32fdpic_pltrel_section (info)->flags |= SEC_EXCLUDE;
      else
	{
	  ubicom32fdpic_pltrel_section (info)->contents =
	    (bfd_byte *) bfd_zalloc (dynobj,
				     ubicom32fdpic_pltrel_section (info)->size);
	  if (ubicom32fdpic_pltrel_section (info)->contents == NULL)
	    return FALSE;
	}
    }

  /* The Pltsection is g.lzplt . The 4 is for the trampoline code. */
  total_plt_size = gpinfo.total_lzplt + gpinfo.total_trampoline;
  gpinfo.current_plt_trampoline = gpinfo.total_lzplt;

  if (elf_hash_table (info)->dynamic_sections_created)
    {
      DPRINTF(" PLT size = %d\n", (total_plt_size ));
      ubicom32fdpic_plt_section (info)->size = (total_plt_size);
    }

  /* Save information that we're going to need to generate GOT and PLT
     entries.  */
  ubicom32fdpic_got_initial_offset (info) = gpinfo.total_fdplt;

  if (get_elf_backend_data (output_bfd)->want_got_sym)
    elf_hash_table (info)->hgot->root.u.def.value
      += ubicom32fdpic_got_initial_offset (info);

  /* Allocate the PLT section contents.  */
  if (elf_hash_table (info)->dynamic_sections_created)
    {
      if (ubicom32fdpic_plt_section (info)->size == 4)
	{
	  ubicom32fdpic_plt_section (info)->flags |= SEC_EXCLUDE;
	  ubicom32fdpic_plt_section (info)->size = 0;
	}
      else
	{
	  DPRINTF(" Alloc PLT size = %d\n", (total_plt_size));
	  ubicom32fdpic_plt_section (info)->contents =
	    (bfd_byte *) bfd_zalloc (dynobj,
				     ubicom32fdpic_plt_section (info)->size);
	  if (ubicom32fdpic_plt_section (info)->contents == NULL)
	    return FALSE;
	}
    }


  htab_traverse (ubicom32fdpic_relocs_info (info), ubicom32fdpic_assign_plt_entries,
		 &gpinfo);


  if (elf_hash_table (info)->dynamic_sections_created)
    {
      if (ubicom32fdpic_got_section (info)->size)
	if (!_bfd_elf_add_dynamic_entry (info, DT_PLTGOT, 0))
	  return FALSE;

      if (ubicom32fdpic_pltrel_section (info)->size)
	if (!_bfd_elf_add_dynamic_entry (info, DT_PLTRELSZ, 0)
	    || !_bfd_elf_add_dynamic_entry (info, DT_PLTREL, DT_REL)
	    || !_bfd_elf_add_dynamic_entry (info, DT_JMPREL, 0))
	  return FALSE;

      if (ubicom32fdpic_gotrel_section (info)->size)
	if (!_bfd_elf_add_dynamic_entry (info, DT_REL, 0)
	    || !_bfd_elf_add_dynamic_entry (info, DT_RELSZ, 0)
	    || !_bfd_elf_add_dynamic_entry (info, DT_RELENT,
					    sizeof (Elf32_External_Rel)))
	  return FALSE;
    }

  s = bfd_get_section_by_name (dynobj, ".rela.bss");
  if (s && s->size == 0)
    s->flags |= SEC_EXCLUDE;

  s = bfd_get_section_by_name (dynobj, ".rel.plt");
  if (s && s->size == 0)
    s->flags |= SEC_EXCLUDE;

  return TRUE;
}


/* Adjust a symbol defined by a dynamic object and referenced by a
   regular object.  */

static bfd_boolean
ubicom32fdpic_elf_adjust_dynamic_symbol
(struct bfd_link_info *info ATTRIBUTE_UNUSED,
 struct elf_link_hash_entry *h ATTRIBUTE_UNUSED)
{
  bfd * dynobj;

  dynobj = elf_hash_table (info)->dynobj;

  /* Make sure we know what is going on here.  */
  BFD_ASSERT (dynobj != NULL
	      && (h->u.weakdef != NULL
		  || (h->def_dynamic
		      && h->ref_regular
		      && !h->def_regular)));

  /* If this is a weak symbol, and there is a real definition, the
     processor independent code will have arranged for us to see the
     real definition first, and we can just use the same value.  */
  if (h->u.weakdef != NULL)
    {
      BFD_ASSERT (h->u.weakdef->root.type == bfd_link_hash_defined
		  || h->u.weakdef->root.type == bfd_link_hash_defweak);
      h->root.u.def.section = h->u.weakdef->root.u.def.section;
      h->root.u.def.value = h->u.weakdef->root.u.def.value;
    }

  return TRUE;
}

static bfd_boolean
ubicom32fdpic_elf_always_size_sections (bfd *output_bfd,
					struct bfd_link_info *info)
{
  if (!info->relocatable)
    {
      struct elf_link_hash_entry *h;

      /* Force a PT_GNU_STACK segment to be created.  */
      if (! elf_tdata (output_bfd)->stack_flags)
	elf_tdata (output_bfd)->stack_flags = PF_R | PF_W | PF_X;

      /* Define __stacksize__ if it's not defined yet.  */
      h = elf_link_hash_lookup (elf_hash_table (info), "__stacksize__",
				FALSE, FALSE, FALSE);
      if (! h || h->root.type != bfd_link_hash_defined
	  || h->type != STT_OBJECT
	  || !h->def_regular)
	{
	  struct bfd_link_hash_entry *bh = NULL;

	  if (!(_bfd_generic_link_add_one_symbol
		(info, output_bfd, "__stacksize__",
		 BSF_GLOBAL, bfd_abs_section_ptr, ubicom32fdpic_stack_size,
		 (const char *) NULL, FALSE,
		 get_elf_backend_data (output_bfd)->collect, &bh)))
	    return FALSE;

	  h = (struct elf_link_hash_entry *) bh;
	  h->def_regular = 1;
	  h->type = STT_OBJECT;
	}
    }

  return TRUE;
}

static bfd_boolean
ubicom32fdpic_elf_finish_dynamic_sections (bfd *output_bfd,
					   struct bfd_link_info *info)
{
  bfd *dynobj;
  asection *sdyn;

  dynobj = elf_hash_table (info)->dynobj;

  if (ubicom32fdpic_got_section (info))
    {
      BFD_ASSERT (ubicom32fdpic_gotrel_section (info)->size
		  == (ubicom32fdpic_gotrel_section (info)->reloc_count
		      * sizeof (Elf32_External_Rel)));

      if (ubicom32fdpic_gotfixup_section (info))
	{
	  struct elf_link_hash_entry *hgot = elf_hash_table (info)->hgot;
	  bfd_vma got_value = hgot->root.u.def.value
	    + hgot->root.u.def.section->output_section->vma
	    + hgot->root.u.def.section->output_offset;

	  ubicom32fdpic_add_rofixup (output_bfd, ubicom32fdpic_gotfixup_section (info),
				     got_value, 0);

	  if (ubicom32fdpic_gotfixup_section (info)->size
	      != (ubicom32fdpic_gotfixup_section (info)->reloc_count * 4))
	    {
	      (*_bfd_error_handler)
		("LINKER BUG: .rofixup section size mismatch Size %d, should be %d ",
		 ubicom32fdpic_gotfixup_section (info)->size, ubicom32fdpic_gotfixup_section (info)->reloc_count * 4);
	      return FALSE;
	    }
	}
    }
  if (elf_hash_table (info)->dynamic_sections_created)
    {
      BFD_ASSERT (ubicom32fdpic_pltrel_section (info)->size
		  == (ubicom32fdpic_pltrel_section (info)->reloc_count
		      * sizeof (Elf32_External_Rel)));
    }

  sdyn = bfd_get_section_by_name (dynobj, ".dynamic");

  if (elf_hash_table (info)->dynamic_sections_created)
    {
      Elf32_External_Dyn * dyncon;
      Elf32_External_Dyn * dynconend;

      BFD_ASSERT (sdyn != NULL);

      dyncon = (Elf32_External_Dyn *) sdyn->contents;
      dynconend = (Elf32_External_Dyn *) (sdyn->contents + sdyn->size);

      for (; dyncon < dynconend; dyncon++)
	{
	  Elf_Internal_Dyn dyn;

	  bfd_elf32_swap_dyn_in (dynobj, dyncon, &dyn);

	  switch (dyn.d_tag)
	    {
	    default:
	      break;

	    case DT_PLTGOT:
	      dyn.d_un.d_ptr = ubicom32fdpic_got_section (info)->output_section->vma
		+ ubicom32fdpic_got_section (info)->output_offset
		+ ubicom32fdpic_got_initial_offset (info);
	      bfd_elf32_swap_dyn_out (output_bfd, &dyn, dyncon);
	      break;

	    case DT_JMPREL:
	      dyn.d_un.d_ptr = ubicom32fdpic_pltrel_section (info)
		->output_section->vma
		+ ubicom32fdpic_pltrel_section (info)->output_offset;
	      bfd_elf32_swap_dyn_out (output_bfd, &dyn, dyncon);
	      break;

	    case DT_PLTRELSZ:
	      dyn.d_un.d_val = ubicom32fdpic_pltrel_section (info)->size;
	      bfd_elf32_swap_dyn_out (output_bfd, &dyn, dyncon);
	      break;
	    }
	}
    }

  return TRUE;
}

/* Perform any actions needed for dynamic symbols.  */
static bfd_boolean
ubicom32fdpic_elf_finish_dynamic_symbol
(bfd *output_bfd ATTRIBUTE_UNUSED,
 struct bfd_link_info *info ATTRIBUTE_UNUSED,
 struct elf_link_hash_entry *h ATTRIBUTE_UNUSED,
 Elf_Internal_Sym *sym ATTRIBUTE_UNUSED)
{
  return TRUE;
}

static bfd_boolean
ubicom32fdpic_elf_modify_program_headers (bfd *output_bfd,
					  struct bfd_link_info *info)
{
  struct elf_obj_tdata *tdata = elf_tdata (output_bfd);
  struct elf_segment_map *m;
  Elf_Internal_Phdr *p;

  if (! info)
    return TRUE;

  for (p = tdata->phdr, m = tdata->segment_map; m != NULL; m = m->next, p++)
    if (m->p_type == PT_GNU_STACK)
      break;

  if (m)
    {
      struct elf_link_hash_entry *h;

      /* Obtain the pointer to the __stacksize__ symbol.  */
      h = elf_link_hash_lookup (elf_hash_table (info), "__stacksize__",
				FALSE, FALSE, FALSE);
      if (h)
	{
	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;
	  BFD_ASSERT (h->root.type == bfd_link_hash_defined);
	}

      /* Set the header p_memsz from the symbol value.  We
	 intentionally ignore the symbol section.  */
      if (h && h->root.type == bfd_link_hash_defined)
	p->p_memsz = h->root.u.def.value;
      else
	p->p_memsz = ubicom32fdpic_stack_size;

      p->p_align = 8;
    }

  return TRUE;
}

static bfd_boolean
ubicom32fdpic_elf_gc_sweep_hook (bfd *abfd,
				 struct bfd_link_info *info,
				 asection *sec,
				 const Elf_Internal_Rela *relocs)
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes, **sym_hashes_end;
  const Elf_Internal_Rela *rel;
  const Elf_Internal_Rela *rel_end;
  struct ubicom32fdpic_relocs_info *picrel;

  BFD_ASSERT (IS_FDPIC (abfd));

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (abfd);
  sym_hashes_end = sym_hashes + symtab_hdr->sh_size/sizeof(Elf32_External_Sym);
  if (!elf_bad_symtab (abfd))
    sym_hashes_end -= symtab_hdr->sh_info;

  rel_end = relocs + sec->reloc_count;
  for (rel = relocs; rel < rel_end; rel++)
    {
      struct elf_link_hash_entry *h;
      unsigned long r_symndx;

      r_symndx = ELF32_R_SYM (rel->r_info);
      if (r_symndx < symtab_hdr->sh_info)
	h = NULL;
      else
	h = sym_hashes[r_symndx - symtab_hdr->sh_info];

      if (h != NULL)
	picrel = ubicom32fdpic_relocs_info_for_global (ubicom32fdpic_relocs_info (info),
						       abfd, h,
						       rel->r_addend, NO_INSERT);
      else
	picrel = ubicom32fdpic_relocs_info_for_local (ubicom32fdpic_relocs_info
						      (info), abfd, r_symndx,
						      rel->r_addend, NO_INSERT);

      if (!picrel)
	continue;

      switch (ELF32_R_TYPE (rel->r_info))
	{
	case R_UBICOM32_24_PCREL:
	  picrel->call--;
	  break;

	case R_UBICOM32_FUNCDESC_VALUE:
	  picrel->relocsfdv--;
	  picrel->sym--;
	  break;

	case R_UBICOM32_GOT_LO:
	case R_UBICOM32_GOT_LO_LEAI:
	  picrel->got_lo--;
	  break;

	case R_UBICOM32_FUNCDESC_GOT_LO:
	case R_UBICOM32_FUNCDESC_GOT_LO_LEAI:
	  picrel->fd_got_lo--;
	  break;

	case R_UBICOM32_FUNCDESC_GOT_HI:
	  picrel->fd_got_hi--;
	  break;

	case R_UBICOM32_FUNCDESC:
	  picrel->fd--;
	  picrel->relocsfd--;
	  break;

	case R_UBICOM32_32:
	  if (! IS_FDPIC (abfd))
	    break;

	  if (picrel->sym)
	    picrel->relocs32--;;

	  picrel->sym--;
	  break;

	default:
	  break;
	}
    }

  return TRUE;
}
/* Decide whether to attempt to turn absptr or lsda encodings in
   shared libraries into pcrel within the given input section.  */

static bfd_boolean
ubicom32fdpic_elf_use_relative_eh_frame
(bfd *input_bfd ATTRIBUTE_UNUSED,
 struct bfd_link_info *info ATTRIBUTE_UNUSED,
 asection *eh_frame_section ATTRIBUTE_UNUSED)
{
  /* We can't use PC-relative encodings in FDPIC binaries, in general.  */
  return FALSE;
}

/* Adjust the contents of an eh_frame_hdr section before they're output.  */

static bfd_byte
ubicom32fdpic_elf_encode_eh_address (bfd *abfd,
				     struct bfd_link_info *info,
				     asection *osec, bfd_vma offset,
				     asection *loc_sec, bfd_vma loc_offset,
				     bfd_vma *encoded)
{
  struct elf_link_hash_entry *h;

  h = elf_hash_table (info)->hgot;
  BFD_ASSERT (h && h->root.type == bfd_link_hash_defined);

  if (! h || (ubicom32fdpic_osec_to_segment (abfd, osec)
	      == ubicom32fdpic_osec_to_segment (abfd, loc_sec->output_section)))
    return _bfd_elf_encode_eh_address (abfd, info, osec, offset,
				       loc_sec, loc_offset, encoded);

  BFD_ASSERT (ubicom32fdpic_osec_to_segment (abfd, osec)
	      == (ubicom32fdpic_osec_to_segment
		  (abfd, h->root.u.def.section->output_section)));

  *encoded = osec->vma + offset
    - (h->root.u.def.value
       + h->root.u.def.section->output_section->vma
       + h->root.u.def.section->output_offset);

  return DW_EH_PE_datarel | DW_EH_PE_sdata4;
}
static bfd_boolean
ubicom32fdpic_elf_copy_private_bfd_data (bfd *ibfd, bfd *obfd)
{
  unsigned i;

  if (bfd_get_flavour (ibfd) != bfd_target_elf_flavour
      || bfd_get_flavour (obfd) != bfd_target_elf_flavour)
    return TRUE;

  if (! ubicom32_elf_copy_private_bfd_data (ibfd, obfd))
    return FALSE;

  if (! elf_tdata (ibfd) || ! elf_tdata (ibfd)->phdr
      || ! elf_tdata (obfd) || ! elf_tdata (obfd)->phdr)
    return TRUE;

  /* Copy the stack size.  */
  for (i = 0; i < elf_elfheader (ibfd)->e_phnum; i++)
    if (elf_tdata (ibfd)->phdr[i].p_type == PT_GNU_STACK)
      {
	Elf_Internal_Phdr *iphdr = &elf_tdata (ibfd)->phdr[i];

	for (i = 0; i < elf_elfheader (obfd)->e_phnum; i++)
	  if (elf_tdata (obfd)->phdr[i].p_type == PT_GNU_STACK)
	    {
	      memcpy (&elf_tdata (obfd)->phdr[i], iphdr, sizeof (*iphdr));

	      /* Rewrite the phdrs, since we're only called after they
		 were first written.  */
	      if (bfd_seek (obfd, (bfd_signed_vma) get_elf_backend_data (obfd)
			    ->s->sizeof_ehdr, SEEK_SET) != 0
		  || get_elf_backend_data (obfd)->s
		  ->write_out_phdrs (obfd, elf_tdata (obfd)->phdr,
				     elf_elfheader (obfd)->e_phnum) != 0)
		return FALSE;
	      break;
	    }

	break;
      }

  return TRUE;
}

static bfd_boolean
ubicom32fdpic_elf_relocate_section (bfd * output_bfd,
				    struct bfd_link_info *info,
				    bfd * input_bfd,
				    asection * input_section,
				    bfd_byte * contents,
				    Elf_Internal_Rela * relocs,
				    Elf_Internal_Sym * local_syms,
				    asection ** local_sections)
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  Elf_Internal_Rela *rel;
  Elf_Internal_Rela *relend;
  unsigned isec_segment, got_segment, plt_segment,
    check_segment[2];
  int silence_segment_error = !(info->shared || info->pie);

  if (info->relocatable)
    return TRUE;

  symtab_hdr = & elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (input_bfd);
  relend     = relocs + input_section->reloc_count;

  isec_segment = ubicom32fdpic_osec_to_segment (output_bfd,
						input_section->output_section);
  if (IS_FDPIC (output_bfd) && ubicom32fdpic_got_section (info))
    got_segment = ubicom32fdpic_osec_to_segment (output_bfd,
						 ubicom32fdpic_got_section (info)
						 ->output_section);
  else
    got_segment = -1;
  if (IS_FDPIC (output_bfd) && elf_hash_table (info)->dynamic_sections_created)
    plt_segment = ubicom32fdpic_osec_to_segment (output_bfd,
						 ubicom32fdpic_plt_section (info)
						 ->output_section);
  else
    plt_segment = -1;

  for (rel = relocs; rel < relend; rel ++)
    {
      reloc_howto_type *howto;
      unsigned long r_symndx;
      Elf_Internal_Sym *sym;
      asection *sec;
      struct elf_link_hash_entry *h;
      bfd_vma relocation;
      bfd_reloc_status_type r;
      const char * name = NULL;
      int r_type;
      asection *osec;
      struct ubicom32fdpic_relocs_info *picrel = NULL;
      bfd_vma orig_addend = rel->r_addend;

      r_type = ELF32_R_TYPE (rel->r_info);

      if (r_type == R_UBICOM32_GNU_VTINHERIT
	  || r_type == R_UBICOM32_GNU_VTENTRY)
	continue;

      /* This is a final link.  */
      r_symndx = ELF32_R_SYM (rel->r_info);

      //howto = ubicom32_reloc_type_lookup (input_bfd, r_type);
      howto  = ubicom32_elf_howto_table + ELF32_R_TYPE (rel->r_info);
      if (howto == NULL)
	{
	  bfd_set_error (bfd_error_bad_value);
	  return FALSE;
	}

      h      = NULL;
      sym    = NULL;
      sec    = NULL;

      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  osec = sec = local_sections [r_symndx];
	  relocation = _bfd_elf_rela_local_sym (output_bfd, sym, &sec, rel);

	  name = bfd_elf_string_from_elf_section
	    (input_bfd, symtab_hdr->sh_link, sym->st_name);
	  name = (name == NULL) ? bfd_section_name (input_bfd, sec) : name;
	}
      else
	{
	  h = sym_hashes [r_symndx - symtab_hdr->sh_info];

	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;

	  name = h->root.root.string;

	  if ((h->root.type == bfd_link_hash_defined
	       || h->root.type == bfd_link_hash_defweak)
	      && ! UBICOM32FDPIC_SYM_LOCAL (info, h))
	    {
	      sec = NULL;
	      relocation = 0;
	    }
	  else
	    if (h->root.type == bfd_link_hash_defined
		|| h->root.type == bfd_link_hash_defweak)
	      {
		sec = h->root.u.def.section;
		relocation = (h->root.u.def.value
			      + sec->output_section->vma
			      + sec->output_offset);
	      }
	    else if (h->root.type == bfd_link_hash_undefweak)
	      {
		relocation = 0;
	      }
	    else if (info->unresolved_syms_in_objects == RM_IGNORE
		     && ELF_ST_VISIBILITY (h->other) == STV_DEFAULT)
	      relocation = 0;
	    else
	      {
		if (! ((*info->callbacks->undefined_symbol)
		       (info, h->root.root.string, input_bfd,
			input_section, rel->r_offset,
			(info->unresolved_syms_in_objects == RM_GENERATE_ERROR
			 || ELF_ST_VISIBILITY (h->other)))))
		  return FALSE;
		relocation = 0;
	      }
	  osec = sec;
	}

      switch (r_type)
	{
	case R_UBICOM32_24_PCREL:
	case R_UBICOM32_32:
	  if (! IS_FDPIC (output_bfd))
	    goto non_fdpic;

	case R_UBICOM32_FUNCDESC_VALUE:
	case R_UBICOM32_FUNCDESC:
	case R_UBICOM32_GOT_LO:
	case R_UBICOM32_GOT_LO_LEAI:
	case R_UBICOM32_GOT_HI:
	case R_UBICOM32_FUNCDESC_GOT_LO:
	case R_UBICOM32_FUNCDESC_GOT_LO_LEAI:
	case R_UBICOM32_FUNCDESC_GOT_HI:
	  if (h != NULL)
	    picrel = ubicom32fdpic_relocs_info_for_global (ubicom32fdpic_relocs_info
							   (info), input_bfd, h,
							   orig_addend, INSERT);
	  else
	    /* In order to find the entry we created before, we must
	       use the original addend, not the one that may have been
	       modified by _bfd_elf_rela_local_sym().  */
	    picrel = ubicom32fdpic_relocs_info_for_local (ubicom32fdpic_relocs_info
							  (info), input_bfd, r_symndx,
							  orig_addend, INSERT);
	  if (! picrel)
	    return FALSE;

	  if (!ubicom32fdpic_emit_got_relocs_plt_entries (picrel, output_bfd, info,
							  osec, sym,
							  rel->r_addend))
	    {
	      (*_bfd_error_handler)
		(_("%B: relocation at `%A+0x%x' references symbol `%s' with nonzero addend"),
		 input_bfd, input_section, rel->r_offset, name);
	      return FALSE;

	    }

	  break;
	case R_UBICOM32_21_PCREL:
	case R_UBICOM32_HI24:
	case R_UBICOM32_LO7_S:
	  //printf("Seeing this stuff Don;t know what to do. r_type %d r_symndx %d %s %s\n", r_type, r_symndx, input_bfd->filename, input_section->name);
	  break;

	default:
	non_fdpic:
	  picrel = NULL;
	  //printf("h = 0x%x %d\n", h, UBICOM32FDPIC_SYM_LOCAL (info, h));
	  if (h && ! UBICOM32FDPIC_SYM_LOCAL (info, h))
	    {
	      printf("h = 0x%x %d\n", (unsigned int)h, UBICOM32FDPIC_SYM_LOCAL (info, h));
	      printf("Seeing this stuff. r_type %d r_symndx %ld %s %s\n", r_type, r_symndx, input_bfd->filename, input_section->name);
	      info->callbacks->warning
		(info, _("relocation references symbol not defined in the module"),
		 name, input_bfd, input_section, rel->r_offset);
	      return FALSE;
	    }
	  break;
	}

      switch (r_type)
	{
	case R_UBICOM32_21_PCREL:
	case R_UBICOM32_HI24:
	case R_UBICOM32_LO7_S:
	  //printf("Seeing this stuff. r_type %d r_symndx %d %s %s\n", r_type, r_symndx, input_bfd->filename, input_section->name);
	  check_segment[0] = check_segment[1] = got_segment;
	  break;

	case R_UBICOM32_24_PCREL:
	  check_segment[0] = isec_segment;
	  if (! IS_FDPIC (output_bfd))
	    check_segment[1] = isec_segment;
	  else if (picrel->plt)
	    {
	      relocation = ubicom32fdpic_plt_section (info)->output_section->vma
		+ ubicom32fdpic_plt_section (info)->output_offset
		+ picrel->plt_entry;

	      /* subtract rel->addend. This will get added back in the 23pcrel howto routine. */
	      relocation -= rel->r_addend;

	      check_segment[1] = plt_segment;
	    }
	  /* We don't want to warn on calls to undefined weak symbols,
	     as calls to them must be protected by non-NULL tests
	     anyway, and unprotected calls would invoke undefined
	     behavior.  */
	  else if (picrel->symndx == -1
		   && picrel->d.h->root.type == bfd_link_hash_undefweak)
	    check_segment[1] = check_segment[0];
	  else
	    check_segment[1] = sec
	      ? ubicom32fdpic_osec_to_segment (output_bfd, sec->output_section)
	      : (unsigned)-1;
	  break;

	case R_UBICOM32_GOT_LO:
	  relocation = picrel->got_entry >> 2;
	  check_segment[0] = check_segment[1] = got_segment;
	  break;

	case R_UBICOM32_GOT_LO_LEAI:
	  relocation = picrel->got_entry;
	  check_segment[0] = check_segment[1] = got_segment;
	  break;

	case R_UBICOM32_FUNCDESC_GOT_LO:
	  relocation = picrel->fdgot_entry >> 2;
	  check_segment[0] = check_segment[1] = got_segment;
	  break;

	case R_UBICOM32_FUNCDESC_GOT_LO_LEAI:
	  relocation = picrel->fdgot_entry;
	  check_segment[0] = check_segment[1] = got_segment;
	  break;

	case R_UBICOM32_FUNCDESC:
	  {
	    int dynindx;
	    bfd_vma addend = rel->r_addend;

	    if (! (h && h->root.type == bfd_link_hash_undefweak
		   && UBICOM32FDPIC_SYM_LOCAL (info, h)))
	      {
		/* If the symbol is dynamic and there may be dynamic
		   symbol resolution because we are or are linked with a
		   shared library, emit a FUNCDESC relocation such that
		   the dynamic linker will allocate the function
		   descriptor.  If the symbol needs a non-local function
		   descriptor but binds locally (e.g., its visibility is
		   protected, emit a dynamic relocation decayed to
		   section+offset.  */
		if (h && ! UBICOM32FDPIC_FUNCDESC_LOCAL (info, h)
		    && UBICOM32FDPIC_SYM_LOCAL (info, h)
		    && !(info->executable && !info->pie))
		  {
		    dynindx = elf_section_data (h->root.u.def.section
						->output_section)->dynindx;
		    addend += h->root.u.def.section->output_offset
		      + h->root.u.def.value;
		  }
		else if (h && ! UBICOM32FDPIC_FUNCDESC_LOCAL (info, h))
		  {
		    if (addend)
		      {
			info->callbacks->warning
			  (info, _("R_UBICOM32_FUNCDESC references dynamic symbol with nonzero addend"),
			   name, input_bfd, input_section, rel->r_offset);
			return FALSE;
		      }
		    dynindx = h->dynindx;
		  }
		else
		  {
		    /* Otherwise, we know we have a private function
		       descriptor, so reference it directly.  */
		    BFD_ASSERT (picrel->privfd);
		    r_type = R_UBICOM32_32; // was FUNCDESC but bfin uses 32 bit
		    dynindx = elf_section_data (ubicom32fdpic_got_section (info)
						->output_section)->dynindx;
		    addend = ubicom32fdpic_got_section (info)->output_offset
		      + ubicom32fdpic_got_initial_offset (info)
		      + picrel->fd_entry;
		  }

		/* If there is room for dynamic symbol resolution, emit
		   the dynamic relocation.  However, if we're linking an
		   executable at a fixed location, we won't have emitted a
		   dynamic symbol entry for the got section, so idx will
		   be zero, which means we can and should compute the
		   address of the private descriptor ourselves.  */
		if (info->executable && !info->pie
		    && (!h || UBICOM32FDPIC_FUNCDESC_LOCAL (info, h)))
		  {
		    addend += ubicom32fdpic_got_section (info)->output_section->vma;
		    if ((bfd_get_section_flags (output_bfd,
						input_section->output_section)
			 & (SEC_ALLOC | SEC_LOAD)) == (SEC_ALLOC | SEC_LOAD))
		      {
			if (ubicom32fdpic_osec_readonly_p (output_bfd,
							   input_section
							   ->output_section))
			  {
			    info->callbacks->warning
			      (info,
			       _("cannot emit fixups in read-only section"),
			       name, input_bfd, input_section, rel->r_offset);
			    return FALSE;
			  }
			ubicom32fdpic_add_rofixup (output_bfd,
						   ubicom32fdpic_gotfixup_section
						   (info),
						   _bfd_elf_section_offset
						   (output_bfd, info,
						    input_section, rel->r_offset)
						   + input_section
						   ->output_section->vma
						   + input_section->output_offset,
						   picrel);
		      }
		  }
		else if ((bfd_get_section_flags (output_bfd,
						 input_section->output_section)
			  & (SEC_ALLOC | SEC_LOAD)) == (SEC_ALLOC | SEC_LOAD))
		  {
		    bfd_vma offset;

		    if (ubicom32fdpic_osec_readonly_p (output_bfd,
						       input_section
						       ->output_section))
		      {
			info->callbacks->warning
			  (info,
			   _("cannot emit dynamic relocations in read-only section"),
			   name, input_bfd, input_section, rel->r_offset);
			return FALSE;
		      }
		    offset = _bfd_elf_section_offset (output_bfd, info,
						      input_section, rel->r_offset);
		    /* Only output a reloc for a not deleted entry.  */
		    if (offset >= (bfd_vma) -2)
		      ubicom32fdpic_add_dyn_reloc (output_bfd,
						   ubicom32fdpic_gotrel_section (info),
						   0,
						   R_UBICOM32_NONE,
						   dynindx, addend, picrel);
		    else
		      ubicom32fdpic_add_dyn_reloc (output_bfd,
						   ubicom32fdpic_gotrel_section (info),
						   offset + input_section
						   ->output_section->vma
						   + input_section->output_offset,
						   r_type,
						   dynindx, addend, picrel);
		  }
		else
		  addend += ubicom32fdpic_got_section (info)->output_section->vma;
	      }

	    /* We want the addend in-place because dynamic
	       relocations are REL.  Setting relocation to it should
	       arrange for it to be installed.  */
	    relocation = addend - rel->r_addend;
	  }
	  check_segment[0] = check_segment[1] = got_segment;
	  break;

	case R_UBICOM32_32:
	  if (! IS_FDPIC (output_bfd))
	    {
	      check_segment[0] = check_segment[1] = -1;
	      break;
	    }
	  /* Fall through.  */
	case R_UBICOM32_FUNCDESC_VALUE:
	  {
	    int dynindx;
	    bfd_vma addend = rel->r_addend;
	    bfd_vma offset;
	    offset = _bfd_elf_section_offset (output_bfd, info,
					      input_section, rel->r_offset);

	    /* If the symbol is dynamic but binds locally, use
	       section+offset.  */
	    if (h && ! UBICOM32FDPIC_SYM_LOCAL (info, h))
	      {
		if (addend && r_type == R_UBICOM32_FUNCDESC_VALUE)
		  {
		    info->callbacks->warning
		      (info, _("R_UBICOM32_FUNCDESC_VALUE references dynamic symbol with nonzero addend"),
		       name, input_bfd, input_section, rel->r_offset);
		    return FALSE;
		  }
		dynindx = h->dynindx;
	      }
	    else
	      {
		if (h)
		  addend += h->root.u.def.value;
		else
		  addend += sym->st_value;
		if (osec)
		  addend += osec->output_offset;
		if (osec && osec->output_section
		    && ! bfd_is_abs_section (osec->output_section)
		    && ! bfd_is_und_section (osec->output_section))
		  dynindx = elf_section_data (osec->output_section)->dynindx;
		else
		  dynindx = 0;
	      }

	    /* If we're linking an executable at a fixed address, we
	       can omit the dynamic relocation as long as the symbol
	       is defined in the current link unit (which is implied
	       by its output section not being NULL).  */
	    if (info->executable && !info->pie
		&& (!h || UBICOM32FDPIC_SYM_LOCAL (info, h)))
	      {
		if (osec)
		  addend += osec->output_section->vma;
		if (IS_FDPIC (input_bfd)
		    && (bfd_get_section_flags (output_bfd,
					       input_section->output_section)
			& (SEC_ALLOC | SEC_LOAD)) == (SEC_ALLOC | SEC_LOAD))
		  {
		    if (ubicom32fdpic_osec_readonly_p (output_bfd,
						       input_section
						       ->output_section))
		      {
			info->callbacks->warning
			  (info,
			   _("cannot emit fixups in read-only section"),
			   name, input_bfd, input_section, rel->r_offset);
			return FALSE;
		      }
		    if (!h || h->root.type != bfd_link_hash_undefweak)
		      {
			/* Only output a reloc for a not deleted entry.  */
			if (offset >= (bfd_vma)-2)
			  ubicom32fdpic_add_rofixup (output_bfd,
						     ubicom32fdpic_gotfixup_section
						     (info), -1, picrel);
			else
			  ubicom32fdpic_add_rofixup (output_bfd,
						     ubicom32fdpic_gotfixup_section
						     (info),
						     offset + input_section
						     ->output_section->vma
						     + input_section->output_offset,
						     picrel);

			if (r_type == R_UBICOM32_FUNCDESC_VALUE)
			  {
			    if (offset >= (bfd_vma)-2)
			      ubicom32fdpic_add_rofixup
				(output_bfd,
				 ubicom32fdpic_gotfixup_section (info),
				 -1, picrel);
			    else
			      ubicom32fdpic_add_rofixup
				(output_bfd,
				 ubicom32fdpic_gotfixup_section (info),
				 offset + input_section->output_section->vma
				 + input_section->output_offset + 4, picrel);
			  }
		      }
		  }
	      }
	    else
	      {
		if ((bfd_get_section_flags (output_bfd,
					    input_section->output_section)
		     & (SEC_ALLOC | SEC_LOAD)) == (SEC_ALLOC | SEC_LOAD))
		  {
		    if (ubicom32fdpic_osec_readonly_p (output_bfd,
						       input_section
						       ->output_section))
		      {
			info->callbacks->warning
			  (info,
			   _("cannot emit dynamic relocations in read-only section"),
			   name, input_bfd, input_section, rel->r_offset);
			return FALSE;
		      }
		    /* Only output a reloc for a not deleted entry.  */
		    if (offset >= (bfd_vma)-2)
		      ubicom32fdpic_add_dyn_reloc (output_bfd,
						   ubicom32fdpic_gotrel_section (info),
						   0, R_UBICOM32_NONE, dynindx, addend, picrel);
		    else
		      ubicom32fdpic_add_dyn_reloc (output_bfd,
						   ubicom32fdpic_gotrel_section (info),
						   offset
						   + input_section
						   ->output_section->vma
						   + input_section->output_offset,
						   r_type, dynindx, addend, picrel);
		  }
		else if (osec)
		  addend += osec->output_section->vma;
		/* We want the addend in-place because dynamic
		   relocations are REL.  Setting relocation to it
		   should arrange for it to be installed.  */
		relocation = addend - rel->r_addend;
	      }

	    if (r_type == R_UBICOM32_FUNCDESC_VALUE && offset < (bfd_vma)-2)
	      {
		/* If we've omitted the dynamic relocation, just emit
		   the fixed addresses of the symbol and of the local
		   GOT base offset.  */
		if (info->executable && !info->pie
		    && (!h || UBICOM32FDPIC_SYM_LOCAL (info, h)))
		  bfd_put_32 (output_bfd,
			      ubicom32fdpic_got_section (info)->output_section->vma
			      + ubicom32fdpic_got_section (info)->output_offset
			      + ubicom32fdpic_got_initial_offset (info),
			      contents + rel->r_offset + 4);
		else
		  /* A function descriptor used for lazy or local
		     resolving is initialized such that its high word
		     contains the output section index in which the
		     PLT entries are located, and the low word
		     contains the offset of the lazy PLT entry entry
		     point into that section.  */
		  bfd_put_32 (output_bfd,
			      h && ! UBICOM32FDPIC_SYM_LOCAL (info, h)
			      ? 0
			      : ubicom32fdpic_osec_to_segment (output_bfd,
							       sec
							       ->output_section),
			      contents + rel->r_offset + 4);
	      }
	  }
	  check_segment[0] = check_segment[1] = got_segment;
	  break;

	default:
	  check_segment[0] = isec_segment;
	  check_segment[1] = sec
	    ? ubicom32fdpic_osec_to_segment (output_bfd, sec->output_section)
	    : (unsigned)-1;
	  break;
	}

      if (check_segment[0] != check_segment[1] && IS_FDPIC (output_bfd))
	{
#if 1 /* If you take this out, remove the #error from fdpic-static-6.d
	 in the ld testsuite.  */
	  /* This helps catch problems in GCC while we can't do more
	     than static linking.  The idea is to test whether the
	     input file basename is crt0.o only once.  */
	  if (silence_segment_error == 1)
	    silence_segment_error =
	      (strlen (input_bfd->filename) == 6
	       && strcmp (input_bfd->filename, "crt0.o") == 0)
	      || (strlen (input_bfd->filename) > 6
		  && strcmp (input_bfd->filename
			     + strlen (input_bfd->filename) - 7,
			     "/crt0.o") == 0)
	      ? -1 : 0;
#endif
	  if (!silence_segment_error
	      /* We don't want duplicate errors for undefined
		 symbols.  */
	      && !(picrel && picrel->symndx == -1
		   && picrel->d.h->root.type == bfd_link_hash_undefined))
	    info->callbacks->warning
	      (info,
	       (info->shared || info->pie)
	       ? _("relocations between different segments are not supported")
	       : _("warning: relocation references a different segment"),
	       name, input_bfd, input_section, rel->r_offset);
	  if (!silence_segment_error && (info->shared || info->pie))
	    return FALSE;
	  elf_elfheader (output_bfd)->e_flags |= 0x80000000;
	}

      switch (r_type)
	{
	case R_UBICOM32_LO16:
	  r = ubicom32_elf_relocate_lo16 (input_bfd, rel, contents, relocation);
	  break;

	case R_UBICOM32_HI16:
	  r = ubicom32_elf_relocate_hi16 (input_bfd, rel, contents, relocation);
	  break;

	case R_UBICOM32_HI24:
	  r = ubicom32_elf_relocate_hi24 (input_bfd, rel, contents, relocation);
	  break;

	case R_UBICOM32_LO7_S:
	  r = ubicom32_elf_relocate_lo7_s (input_bfd, rel, contents, relocation);
	  break;

	case R_UBICOM32_LO7_2_S:
	  r = ubicom32_elf_relocate_lo7_2_s (input_bfd, rel, contents, relocation);
	  break;

	case R_UBICOM32_LO7_4_S:
	  r = ubicom32_elf_relocate_lo7_4_s (input_bfd, rel, contents, relocation);
	  break;

	case R_UBICOM32_LO7_D:
	  r = ubicom32_elf_relocate_lo7_d (input_bfd, rel, contents, relocation);
	  break;

	case R_UBICOM32_LO7_2_D:
	  r = ubicom32_elf_relocate_lo7_2_d (input_bfd, rel, contents, relocation);
	  break;

	case R_UBICOM32_LO7_4_D:
	  r = ubicom32_elf_relocate_lo7_4_d (input_bfd, rel, contents, relocation);
	  break;

	case R_UBICOM32_24_PCREL:
	  r = ubicom32_elf_relocate_pcrel24 (input_bfd, input_section, rel, contents, relocation);
	  break;

	case R_UBICOM32_LO7_CALLI:
	  r = ubicom32_elf_relocate_lo_calli (input_bfd, rel, contents, relocation, 7);
	  break;

	case R_UBICOM32_LO16_CALLI:
	  r = ubicom32_elf_relocate_lo_calli (input_bfd, rel, contents, relocation, 18);
	  break;

	case R_UBICOM32_LO7_LEAI:
	  r = ubicom32_elf_relocate_lo_leai (input_bfd, rel, contents, relocation, 7);
	  break;

	case R_UBICOM32_LO16_LEAI:
	  r = ubicom32_elf_relocate_lo_leai (input_bfd, rel, contents, relocation, 18);
	  break;

	case R_UBICOM32_GOT_LO_LEAI:
	  r = ubicom32_elf_relocate_got_lo_leai (input_bfd, rel, contents, relocation, 18);
	  break;

	case R_UBICOM32_FUNCDESC_GOT_LO_LEAI:
	  r = ubicom32_elf_relocate_funcdesc_got_lo_leai (input_bfd, rel, contents, relocation, 18);
	  break;

	case R_UBICOM32_GOT_LO:
	  r = ubicom32_elf_relocate_got_lo(input_bfd, input_section, rel, contents, relocation);
	  break;

	case R_UBICOM32_FUNCDESC_GOT_LO:
	  r = ubicom32_elf_relocate_funcdesc_got_lo(input_bfd, input_section, rel, contents, relocation);
	  break;

	case R_UBICOM32_32:
	case R_UBICOM32_FUNCDESC:
	  /* relocation &= ~(0xff << 24); */
	  /* FALLTHROUGH */

	default:
	  r = ubicom32_final_link_relocate (howto, input_bfd, input_section,
					    contents, rel, relocation);
	  break;
	}
    }

  return TRUE;
}

/* Support for core dump NOTE sections.  */

static bfd_boolean
elf32_ubicom32_grok_prstatus (bfd *abfd, Elf_Internal_Note *note)
{
  int offset;
  unsigned int raw_size;
  int flags = elf_elfheader (abfd)->e_flags;

  if (flags & 0xffff0000) 
    {
      /* The Linux/UBICOM32 elf_prstatus struct is 252 bytes long. For jupiter this
	 will change to 260. The only thing we know is that the register information offset is 72 
	 and we will shrink the provided size by 72.  The other
	 hardcoded offsets and sizes listed below (and contained within
	 this lexical block) refer to fields in the target's elf_prstatus
	 struct. 
	 The gdb backend routines that read the register infomation will know what
	 to do based on the size of the section.
      */
      
      /* `pr_cursig' is at offset 12.  */
      elf_tdata (abfd)->core_signal = bfd_get_16 (abfd, note->descdata + 12);
      
      /* `pr_pid' is at offset 24.  */
      elf_tdata (abfd)->core_pid = bfd_get_32 (abfd, note->descdata + 24);
      
      /* `pr_reg' is at offset 72.  */
      offset = 72;
      
      /* Most grok_prstatus implementations set `raw_size' to the size
	 of the pr_reg field.  For Linux/UBICOM32, we set `raw_size' to be
	 the size of `pr_reg' plus the size of `pr_exec_fdpic_loadmap'
	 and `pr_interp_fdpic_loadmap', both of which (by design)
	 immediately follow `pr_reg'.  This will allow these fields to
	 be viewed by GDB as registers.
	 
	 For a linux dump `pr_exec_fdpic_loadmap' and `pr_interp_fdpic_loadmap' 
	 are 4 bytes and are located 12 bytes from the end.  */
      raw_size = note->descsz - 72;
    }
  else
    {
      /* This is for a kernel core dump. At offset 0 is the pid. Set signal to be 0. */
      elf_tdata (abfd)->core_signal = 0;

      /* `pr_pid' is at offset 0.  */
      elf_tdata (abfd)->core_pid = *(int *)note->descdata;

      /* raw_size is the original size - 4 */
      raw_size = note->descsz - 4;

      offset = 4;
    }
  /* Make a ".reg/999" section.  */
  return _bfd_elfcore_make_pseudosection (abfd, ".reg", raw_size,
					  note->descpos + offset);
}

static bfd_boolean
elf32_ubicom32_grok_psinfo (bfd *abfd, Elf_Internal_Note *note)
{
  switch (note->descsz)
    {
      default:
	return FALSE;

      /* The Linux/UBICOM32 elf_prpsinfo struct is 124 bytes long.  */
      case 124:

	/* `pr_fname' is found at offset 28 and is 16 bytes long.  */
	elf_tdata (abfd)->core_program
	  = _bfd_elfcore_strndup (abfd, note->descdata + 28, 16);

	/* `pr_psargs' is found at offset 44 and is 80 bytes long.  */
	elf_tdata (abfd)->core_command
	  = _bfd_elfcore_strndup (abfd, note->descdata + 44, 80);
    }

  /* Note that for some reason, a spurious space is tacked
     onto the end of the args in some (at least one anyway)
     implementations, so strip it off if it exists.  */

  {
    char *command = elf_tdata (abfd)->core_command;
    int n = strlen (command);

    if (0 < n && command[n - 1] == ' ')
      command[n - 1] = '\0';
  }

  return TRUE;
}

#define elf_info_to_howto			ubicom32_info_to_howto_rela
#define elf_info_to_howto_rel			NULL

#define bfd_elf32_bfd_reloc_type_lookup		ubicom32_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup		ubicom32_reloc_name_lookup
#define bfd_elf32_bfd_relax_section		ubicom32_elf_relax_section

#define elf_backend_relocate_section		ubicom32_elf_relocate_section
#define elf_backend_gc_mark_hook		ubicom32_elf_gc_mark_hook
#define elf_backend_gc_sweep_hook		ubicom32_elf_gc_sweep_hook
#define elf_backend_check_relocs                ubicom32_elf_check_relocs
#define elf_backend_object_p		        ubicom32_elf_object_p

#define elf_backend_discard_info		ubicom32_elf_discard_info

#define elf_backend_can_gc_sections		1

#define bfd_elf32_bfd_set_private_flags		ubicom32_elf_set_private_flags
#define bfd_elf32_bfd_copy_private_bfd_data	ubicom32_elf_copy_private_bfd_data
#define bfd_elf32_bfd_merge_private_bfd_data	ubicom32_elf_merge_private_bfd_data
#define bfd_elf32_bfd_print_private_bfd_data	ubicom32_elf_print_private_bfd_data

#define elf_backend_grok_prstatus	elf32_ubicom32_grok_prstatus
#define elf_backend_grok_psinfo		elf32_ubicom32_grok_psinfo

#define bfd_elf32_bfd_extcode_relax		NULL

#define TARGET_BIG_SYM	 bfd_elf32_ubicom32_vec
#define TARGET_BIG_NAME  "elf32-ubicom32"

#define ELF_ARCH	 bfd_arch_ubicom32
#define ELF_MACHINE_CODE EM_UBICOM32
#define ELF_MAXPAGESIZE  0x4000

#include "elf32-target.h"

#undef TARGET_BIG_SYM
#define TARGET_BIG_SYM	 bfd_elf32_ubicom32fdpic_vec
#undef TARGET_BIG_NAME
#define TARGET_BIG_NAME  "elf32-ubicom32fdpic"
#undef	elf32_bed
#define	elf32_bed		elf32_ubicom32fdpic_bed

#undef elf_backend_relocate_section
#define elf_backend_relocate_section		ubicom32fdpic_elf_relocate_section

#undef elf_backend_check_relocs
#define elf_backend_check_relocs                ubicom32fdpic_elf_check_relocs

#undef elf_backend_gc_sweep_hook
#define elf_backend_gc_sweep_hook		ubicom32fdpic_elf_gc_sweep_hook
#undef bfd_elf32_bfd_link_hash_table_create
#define bfd_elf32_bfd_link_hash_table_create \
		ubicom32fdpic_elf_link_hash_table_create
#undef elf_backend_always_size_sections
#define elf_backend_always_size_sections \
		ubicom32fdpic_elf_always_size_sections
#undef elf_backend_modify_program_headers
#define elf_backend_modify_program_headers \
		ubicom32fdpic_elf_modify_program_headers
#undef bfd_elf32_bfd_copy_private_bfd_data
#define bfd_elf32_bfd_copy_private_bfd_data \
		ubicom32fdpic_elf_copy_private_bfd_data

#undef elf_backend_create_dynamic_sections
#define elf_backend_create_dynamic_sections \
		ubicom32fdpic_elf_create_dynamic_sections
#undef elf_backend_adjust_dynamic_symbol
#define elf_backend_adjust_dynamic_symbol \
		ubicom32fdpic_elf_adjust_dynamic_symbol
#undef elf_backend_size_dynamic_sections
#define elf_backend_size_dynamic_sections \
		ubicom32fdpic_elf_size_dynamic_sections
#undef elf_backend_finish_dynamic_symbol
#define elf_backend_finish_dynamic_symbol \
		ubicom32fdpic_elf_finish_dynamic_symbol
#undef elf_backend_finish_dynamic_sections
#define elf_backend_finish_dynamic_sections \
		ubicom32fdpic_elf_finish_dynamic_sections

#undef elf_backend_can_make_relative_eh_frame
#define elf_backend_can_make_relative_eh_frame \
		ubicom32fdpic_elf_use_relative_eh_frame
#undef elf_backend_can_make_lsda_relative_eh_frame
#define elf_backend_can_make_lsda_relative_eh_frame \
		ubicom32fdpic_elf_use_relative_eh_frame
#undef elf_backend_encode_eh_address
#define elf_backend_encode_eh_address \
		ubicom32fdpic_elf_encode_eh_address

#undef elf_backend_may_use_rel_p
#define elf_backend_may_use_rel_p       1
#undef elf_backend_may_use_rela_p
#define elf_backend_may_use_rela_p      1
/* We use REL for dynamic relocations only.  */
#undef elf_backend_default_use_rela_p
#define elf_backend_default_use_rela_p  1

#undef elf_backend_omit_section_dynsym
#define elf_backend_omit_section_dynsym ubicom32fdpic_elf_link_omit_section_dynsym

#undef elf_backend_can_refcount
#define elf_backend_can_refcount 1

#undef elf_backend_want_got_plt
#define elf_backend_want_got_plt 0

#undef elf_backend_plt_readonly
#define elf_backend_plt_readonly 1

#undef elf_backend_want_plt_sym
#define elf_backend_want_plt_sym 1

#undef elf_backend_got_header_size
#define elf_backend_got_header_size     12

#undef elf_backend_rela_normal
#define elf_backend_rela_normal         1

#define elf_backend_grok_prstatus	elf32_ubicom32_grok_prstatus
#define elf_backend_grok_psinfo		elf32_ubicom32_grok_psinfo

#include "elf32-target.h"
