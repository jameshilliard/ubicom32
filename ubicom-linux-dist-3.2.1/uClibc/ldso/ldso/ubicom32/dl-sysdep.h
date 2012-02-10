     /* Copyright (C) 2003, 2004 Red Hat, Inc.
	Contributed by Alexandre Oliva <aoliva@redhat.com>
	Based on ../i386/dl-sysdep.h

This file is part of uClibc.

uClibc is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 2.1 of the
License, or (at your option) any later version.

uClibc is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with uClibc; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139,
USA.  */

/*
 * Various assembly language/system dependent  hacks that are required
 * so that we can minimize the amount of platform specific code.
 */

/*
 * Define this if the system uses RELOCA.
 */
#undef ELF_USES_RELOCA

/* JMPREL relocs are inside the DT_RELA table.  */
#define ELF_MACHINE_PLTREL_OVERLAP

#define DL_NO_COPY_RELOCS

#define HAVE_DL_INLINES_H

/*
 * Initialization sequence for a GOT.  Copy the resolver function
 * descriptor and the pointer to the elf_resolve/link_map data
 * structure.  Initialize the got_value in the module while at that.
 */
#define INIT_GOT(GOT_BASE,MODULE) \
{				\
  (MODULE)->loadaddr.got_value = (GOT_BASE); \
  GOT_BASE[0] = ((unsigned long *)&_dl_linux_resolve)[0]; \
  GOT_BASE[1] = ((unsigned long *)&_dl_linux_resolve)[1]; \
  GOT_BASE[2] = (unsigned long) MODULE; \
}

/* Here we define the magic numbers that this dynamic loader should accept */
#define MAGIC1 EM_UBICOM32
#undef  MAGIC2

/* Used for error messages */
#define ELF_TARGET "UBICOM32"

struct elf_resolve;

struct funcdesc_value
{
  void *entry_point;
  void *got_value;
/*int  relocation; not sure if this required, but it does exist */
};

extern int _dl_linux_resolve(void) __attribute__((__visibility__("hidden")));
extern struct funcdesc_value volatile *
	_dl_linux_resolver (struct elf_resolve *tpnt, int reloc_entry)
	__attribute__((__visibility__("hidden")));

/*
 * TODO: At some point try and make this dynamic based on getpagesize().
 * For now, we need to set the page size based on the actual kernel page
 * size that is in use.  This is 4K on (IP3K, IP5K and IP7K) and 16K on
 * IP8K.
 */
#if defined(CONFIG_UC_UBICOM32_V5)
#define PAGE_ALIGN 0xffffc000
#define ADDR_ALIGN 0x3fff
#define OFFS_ALIGN 0x7fffc000
#else
#define PAGE_ALIGN 0xfffff000
#define ADDR_ALIGN 0xfff
#define OFFS_ALIGN 0x7ffff000
#endif

struct funcdesc_ht;

/*
 * This asm does a quick relcation of string S (which is stored in the text
 * section as thats all we can use
 */
#undef SEND_EARLY_STDERR
#define SEND_EARLY_STDERR(S)						\
do {									\
	static const char __attribute__((section(".text"))) __s[] = (S); \
	const char *__p, *__scratch;					\
	__asm__ (							\
	"	call a3, 1f;				\n\t"		\
	"1:	movei	%0, #%%got_lo(1b)		\n\t"		\
	"	move.4	%0, (%3, %0)			\n\t"		\
	"	sub.4	%1, a3, %0;			\n\t"		\
	"	add.4	%1, %1, %2;			\n\t"		\
		: "=&d" (__scratch), "=&d" (__p)			\
		: "d" (__s), "a" (dl_boot_got_pointer)			\
		: "a3", "cc"						\
	);								\
	SEND_STDERR (__p);						\
	{	int __t;						\
		for (__t = 0; __t < 0x100000; __t++) __asm__ __volatile__ (""); } \
} while (0)

#define DL_LOADADDR_TYPE struct elf32_fdpic_loadaddr

#define DL_RELOC_ADDR(LOADADDR, ADDR) \
    ((ElfW(Addr))__reloc_pointer ((void*)(ADDR), (LOADADDR).map))

#define DL_ADDR_TO_FUNC_PTR(ADDR, LOADADDR) \
  ((void(*)(void)) _dl_funcdesc_for ((void*)(ADDR), (LOADADDR).got_value))

#define _dl_stabilize_funcdesc(val) \
  ({ __asm__ ("" : "+m" (*(val)) :: "memory"); (val); })

#define DL_CALL_FUNC_AT_ADDR(ADDR, LOADADDR, SIGNATURE, ...) \
  ({ struct funcdesc_value fd = { (void*)(ADDR), (LOADADDR).got_value }; \
     void (*pf)(void) = (void*) _dl_stabilize_funcdesc (&fd); \
     (* SIGNATURE pf)(__VA_ARGS__); })

#define DL_INIT_LOADADDR_BOOT(LOADADDR, BASEADDR) \
  (__dl_init_loadaddr_map (&(LOADADDR), dl_boot_got_pointer, \
			   dl_boot_ldsomap ?: dl_boot_progmap))

#define DL_INIT_LOADADDR_PROG(LOADADDR, BASEADDR) \
  (__dl_init_loadaddr_map (&(LOADADDR), 0, dl_boot_progmap))

#define DL_INIT_LOADADDR_EXTRA_DECLS \
  int dl_init_loadaddr_load_count;
#define DL_INIT_LOADADDR(LOADADDR, BASEADDR, PHDR, PHDRCNT) \
  (dl_init_loadaddr_load_count = \
     __dl_init_loadaddr (&(LOADADDR), (PHDR), (PHDRCNT)))
#define DL_INIT_LOADADDR_HDR(LOADADDR, ADDR, PHDR) \
  (__dl_init_loadaddr_hdr ((LOADADDR), (ADDR), (PHDR), \
			   dl_init_loadaddr_load_count))
#define DL_LOADADDR_UNMAP(LOADADDR, LEN) \
  (__dl_loadaddr_unmap ((LOADADDR), (NULL)))
#define DL_LIB_UNMAP(LIB, LEN) \
  (__dl_loadaddr_unmap ((LIB)->loadaddr, (LIB)->funcdesc_ht))
#define DL_LOADADDR_BASE(LOADADDR) \
  ((LOADADDR).got_value)

/* This is called from dladdr(), such that we map a function
   descriptor's address to the function's entry point before trying to
   find in which library it's defined.  */
#define DL_LOOKUP_ADDRESS(ADDRESS) (_dl_lookup_address (ADDRESS))

#define DL_ADDR_IN_LOADADDR(ADDR, TPNT, TFROM) \
  (! (TFROM) && __dl_addr_in_loadaddr ((void*)(ADDR), (TPNT)->loadaddr))

/*
 * Compute the GOT address.  On several platforms, we use assembly
 * here.  on FR-V FDPIC, there's no way to compute the GOT address,
 * since the offset between text and data is not fixed, so we arrange
 * for the assembly _dl_boot to pass this value as an argument to
 * _dl_boot.  */
#define DL_BOOT_COMPUTE_GOT(got) ((got) = dl_boot_got_pointer)

#define DL_BOOT_COMPUTE_DYN(dpnt, got, load_addr) \
  ((dpnt) = dl_boot_ldso_dyn_pointer)

/* We only support loading FDPIC independently-relocatable shared
   libraries.  It probably wouldn't be too hard to support loading
   shared libraries that require relocation by the same amount, but we
   don't know that they exist or would be useful, and the dynamic
   loader code could leak the whole-library map unless we keeping a
   bit more state for DL_LOADADDR_UNMAP and DL_LIB_UNMAP, so let's
   keep things simple for now.  */
#define DL_CHECK_LIB_TYPE(epnt, piclib, _dl_progname, libname) \
do \
{ \
  if (((epnt)->e_flags & EF_UBICOM32_FDPIC) && ! ((epnt)->e_flags & EF_UBICOM32_PIC)) \
    (piclib) = 2; \
  else \
    { \
      _dl_internal_error_number = LD_ERROR_NOTDYN; \
      _dl_dprintf(2, "%s: '%s' is not an FDPIC shared library" \
		  "\n", (_dl_progname), (libname)); \
      _dl_close(infile); \
      return NULL; \
    } \
\
} \
while (0)

/* We want want to apply all relocations in the interpreter during
   bootstrap.  Because of this, we have to skip the interpreter
   relocations in _dl_parse_relocation_information(), see
   elfinterp.c.  */
#define DL_SKIP_BOOTSTRAP_RELOC(SYMTAB, INDEX, STRTAB) 0

#ifdef __NR_pread64
#define _DL_PREAD(FD, BUF, SIZE, OFFSET) \
  (_dl_pread((FD), (BUF), (SIZE), (OFFSET)))
#endif

/* We want to return to dlsym() a function descriptor if the symbol
   turns out to be a function.  */
#define DL_FIND_HASH_VALUE(TPNT, TYPE_CLASS, SYM) \
  (((TYPE_CLASS) & ELF_RTYPE_CLASS_DLSYM) \
   && ELF32_ST_TYPE((SYM)->st_info) == STT_FUNC \
   ? _dl_funcdesc_for ((void *)DL_RELOC_ADDR ((TPNT)->loadaddr, (SYM)->st_value), \
		       (TPNT)->loadaddr.got_value)			     \
   : DL_RELOC_ADDR ((TPNT)->loadaddr, (SYM)->st_value))

#if 0 /* XXX TODO will look at enabling this if we decide to add support for OCM
       * code/data */

#define DL_IS_SPECIAL_SEGMENT(EPNT, PPNT) \
  __dl_is_special_segment(EPNT, PPNT)
#define DL_MAP_SEGMENT(EPNT, PPNT, INFILE, FLAGS) \
  __dl_map_segment (EPNT, PPNT, INFILE, FLAGS)

#endif


#define DL_GET_READY_TO_RUN_EXTRA_PARMS \
    , struct elf32_fdpic_loadmap *dl_boot_progmap, Elf32_Addr dl_boot_got_pointer
#define DL_GET_READY_TO_RUN_EXTRA_ARGS \
    , dl_boot_progmap, dl_boot_got_pointer


#ifdef __USE_GNU
# include <link.h>
#else
# define __USE_GNU
# include <link.h>
# undef __USE_GNU
#endif

#include <elf.h>
static __inline__ void
elf_machine_relative (DL_LOADADDR_TYPE load_off, const Elf32_Addr rel_addr,
		      Elf32_Word relative_count)
{
#if 0
	 Elf32_Rel * rpnt = (void *) rel_addr;
	--rpnt;
	do {
		Elf32_Addr *const reloc_addr = (void *) (load_off + (++rpnt)->r_offset);

		*reloc_addr = DL_RELOC_ADDR (load_off, *reloc_addr);
	} while (--relative_count);
#endif
}
