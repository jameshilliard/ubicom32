/*
  Copyright (C) 2009 Ubicom, Inc.
  Copyright (C) 2003 Red Hat, Inc.
  Contributed by Alexandre Oliva <aoliva@redhat.com>

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

/* Any assembly language/system dependent hacks needed to setup
 * boot1.c so it will work as expected and cope with whatever platform
 * specific wierdness is needed for this architecture.

 * We override the default _dl_boot function, and replace it with a
 * bit of asm.  Then call the real _dl_boot function, which is now
 * named _dl_boot2.  */

/* At program start-up, p0 contains a pointer to a
   elf32_fdpic_loadmap that describes how the executable was loaded
   into memory.  p1 contains a pointer to the interpreter (our!)
   loadmap, if there is an interpreter, or 0 if we're being run as an
   executable.  p2 holds a pointer to the interpreter's dynamic
   section, if there is an interpreter, or to the executable's dynamic
   section, otherwise.  If the executable is not dynamic, gr18 is 0.

   We rely on the fact that the linker adds a pointer to the
   _GLOBAL_OFFSET_TABLE_ as the last ROFIXUP entry, and that
   __self_reloc returns the relocated pointer to us, so that we can
   use this value to initialize the PIC register.  */


/*
 *  _dl_boot is the entry point for ldso.
 *
 * The kernel leaves the main apps args on the stack (positive offsets), and
 * communicates the load maps as follows
 *
 * - d1 is set to the exec_map_addr (struct elf32_fdpic_loadmap *), which is the
 *   current state of the mapped application.
 *
 * - d2 is set to interp_map_addr (struct elf32_fdpic_loadmap *), which is use
 *    by ldso to relocate itself.
 *
 * - d3 is dynamic_addr which as far as I can tell is the stack pointer.
 *
 * - a1 is set to zero and will later be initialised to ldso _dl_fini funcdesc
 */

__asm__(
	"	.text							\n\t"
	"	.global	_start						\n\t"
	"	.type	_start,@function				\n\t"
	"_start:\n\t"
	/*
	 * Jump to .Lcall storing the location .Lcall in a5
	 */
	"	call	a5, .Lcall					\n\t"
	".Lcall:							\n\t"
	"	pdec	sp, 256(sp); reserve 64 words for dummy tls block on stack 		\n\t"
	"	lea.4	12(sp), 256(sp) ; store the argv ptr into the PRENIT slot in tls	\n\t"
	"	move.4	d14, sp ; Set up tls pointer to point to dummy tls block		\n\t"
	/*
	 * Reserve and Push inputs to stack.
	 * 32 byte  stack layout as follows
	 * offset	0  exec_map_addr
	 *		4  interp_map_addr
	 *		8  dynamic_addr
	 *		12 not used
	 *		16 dl_main_funcdesc structure for _dl_boot
	 *		   (12 bytes sizeof funcdesc_value)
	 *		28 reserved
	 *		32  start of &original stack
	 */
	"	pdec	sp, 32(sp); reserve stack space			\n\t"
	"	move.4	0(sp), d1 ; exec_map_addr			\n\t"
	"	move.4	4(sp), d2 ; interp_map_addr			\n\t"
	"	move.4	8(sp), d3; dynamic_addr				\n\t"

	/*
	 * Load linker version of .Lcall in to d0
	 */
	"	moveai	a3, #%hi(.Lcall)				\n\t"
	"	lea.1	d0, %lo(.Lcall)(a3)				\n\t"

	/*
	 * Determine the offset from loaded .Lcall and put it in d0.
	 */
	"	sub.4 d0, a5, d0					\n\t"

	/*
	 * Load linker version of __ROFIXUP_LIST__ and __ROFIXUP_END__ in to d1
	 * and d2.
	 */
	"	moveai	a3, #%hi(__ROFIXUP_LIST__)			\n\t"
	"	lea.1	d1, %lo(__ROFIXUP_LIST__)(a3)			\n\t"
	"	moveai	a3, #%hi(__ROFIXUP_END__)			\n\t"
	"	lea.1	d2, %lo(__ROFIXUP_END__)(a3)			\n\t"
	/*
	 * Use offset determined above to find the loaded versions.
	 */
	"	add.4	d1, d1, d0					\n\t"
	"	add.4	d2, d2, d0					\n\t"

	/*
	 * Load interp_map_addr into d0, if interp_map_addr is null use
	 * exec_map_addr.
	 */
	"	move.4	d0, 4(sp) ; load interp_map_addr	       	\n\t"
	"	cmpi	d0, #0						\n\t"
	"	jmpne.f	1f						\n\t"
	"	move.4	d0, 0(sp) ; load exec_map_addr			\n\t"

	/*
	 * call __self_reloc(map, void ***p, void ***e)
	 * it returns the the GOT in d0.
	 */
	"1:	call	a5, __self_reloc ; returns a0 in d0		\n\t"
	"	move.4	a0, d0 ; set GOT				\n\t"
	"	move.4	a2, d0 ; save GOT				\n\t"

	/*
	 * now setup to call
	 * _dl_start (Elf32_Addr dl_boot_got_pointer,		(d0)
	 *	struct elf32_fdpic_loadmap *dl_boot_progmap,	(d1)
	 *	struct elf32_fdpic_loadmap *dl_boot_ldsomap,	(d2)
	 *	Elf32_Dyn *dl_boot_ldso_dyn_pointer,		(d3)
	 *	struct funcdesc_value *dl_main_funcdesc,	(d4)
	 *	unsigned long args				(d5)
	 *
	 * NOTE: for dl_main_funcdesc we reserved space on the stack for this
	 * structure.
	 */
	"	move.4	d1, 0(sp)	; d1 = exec_map_addr		\n\t"
	"	move.4	d2, 4(sp)	; d2 = interp_map_addr		\n\t"
	"	move.4	d3, 8(sp)	; d3 = dynamic_addr		\n\t"
	"	lea.1	d4, 16(sp)	; d4 = &dl_main_funcdesc	\n\t"
	"	lea.4	d5, 288(sp)	; d5 = original stack ptr (args)\n\t"
	"	call	a5, _dl_start					\n\t"

	/*
	 * Setup to call the main entry point, starting with passing our FINI
	 * ptr() to the user in a1 (remember we saved the got in a2)
	 */
	"	movei	d15, #%got_funcdesc_lo(_dl_fini)		\n\t"
	"	move.4	a1, (a2, d15)					\n\t"

	/*
	 * restore the load map from the kernel.
	 */
	"	move.4	d1, 0(sp)	; restore exec_map_addr		\n\t"
	"	move.4	d2, 4(sp)	; restore interp_map_addr	\n\t"
	"	move.4	d3, 8(sp)	; restore dynamic_addr		\n\t"

	/*
	 * _dl_start returned the main apps entry point in dl_main_funcdesc,
	 * load that information now.
	 */
	"	move.4	a5, 16(sp) ; a5 = dl_main_funcdesc.entry_point	\n\t"
	"	move.4	a0, 20(sp) ; a0 = dl_main_funcdesc.got_value	\n\t"
	"	lea.4	sp, 288(sp); restore original stack ptr (args)	\n\t"
	"	calli	a5, 0(a5)					\n\t"
	"	bkpt	-1; should never get here			\n\t"
	"	.size	_start, . - _start				\n\t"
);

#undef DL_START
#define DL_START(X)   \
static void  __attribute__ ((used)) \
_dl_start (Elf32_Addr dl_boot_got_pointer, \
	   struct elf32_fdpic_loadmap *dl_boot_progmap, \
	   struct elf32_fdpic_loadmap *dl_boot_ldsomap, \
	   Elf32_Dyn *dl_boot_ldso_dyn_pointer, \
	   struct funcdesc_value *dl_main_funcdesc, \
	   X)

struct elf32_fdpic_loadmap;

/*
 * Get a pointer to the argv array.  On many platforms this can be just
 * the address if the first argument, on other platforms we need to
 * do something a little more subtle here.
 */
#define GET_ARGV(ARGVP, ARGS) ARGVP = (((unsigned long*) ARGS) + 1)

/*
 * Here is a macro to perform a relocation.  This is only used when
 * bootstrapping the dynamic loader.  RELP is the relocation that we
 * are performing, REL is the pointer to the address we are relocating.
 * SYMBOL is the symbol involved in the relocation, and LOAD is the
 * load address.
 */
#define PERFORM_BOOTSTRAP_RELOC(RELP,REL,SYMBOL,LOAD,SYMTAB) \
	switch(ELF32_R_TYPE((RELP)->r_info)){				\
	case R_UBICOM32_32:						\
	  *(REL) += (SYMBOL);						\
	  break;							\
	case R_UBICOM32_FUNCDESC_VALUE:					\
	  {								\
	    struct funcdesc_value fv = {				\
	      (void*)((SYMBOL) + *(REL)),				\
	      (LOAD).got_value						\
	    };								\
	    *(struct funcdesc_value volatile *)(REL) = fv;		\
	    break;							\
	  }								\
	default:							\
	  _dl_exit(1);							\
	}

/*
 * Transfer control to the user's application, once the dynamic loader
 * is done.  We return the address of the function's entry point to
 * _dl_boot, see boot1_arch.h.
 */
#define START()	do {							\
  struct elf_resolve *exec_mod = _dl_loaded_modules;			\
  dl_main_funcdesc->entry_point = _dl_elf_main;				\
  while (exec_mod->libtype != elf_executable)				\
    exec_mod = exec_mod->next;						\
  dl_main_funcdesc->got_value = exec_mod->loadaddr.got_value;		\
  return;								\
} while (0)
