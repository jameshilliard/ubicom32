/* Startup code compliant to the default ubicom32 ABI.
   Copyright (C) 2009, Ubicom, Inc.
   Copyright (C) 1996, 1997, 1998, 2001, 2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   In addition to the permissions in the GNU Lesser General Public
   License, the Free Software Foundation gives you unlimited
   permission to link the compiled version of this file with other
   programs, and to distribute those programs without any restriction
   coming from the use of this file. (The GNU Lesser General Public
   License restrictions do apply in other respects; for example, they
   cover modification of the file, and distribution when not linked
   into another program.)

   Note that people who make modified versions of this file are not
   obligated to grant this special exception for their modified
   versions; it is their choice whether to do so. The GNU Lesser
   General Public License gives permission to release a modified
   version without this exception; this exception also makes it
   possible to release a modified version which carries forward this
   exception.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <features.h>

/* This is the canonical entry point, usually the first thing in the text
   segment.  The SVR4/ubicom32 FD-PIC ABI says that when the entry point
   runs, most registers' values are unspecified, except for:

   The kernel leaves the main apps args on the stack (positive offsets), and
   communicates the load maps as follows

	d1 is set to the exec_map_addr (struct elf32_fdpic_loadmap *), which is the
	current state of the mapped application.

	d2 is set to interp_map_addr (struct elf32_fdpic_loadmap *), which is use
	by ldso to relocate itself (not used when not using ld.so)

	d3 is dynamic_addr which as far as I can tell is the stack pointer.

	a1 is set to zero and will later be initialised to ldso _dl_fini
	funcdesc if ld.so is involved.  When set, will contains a function
	pointer to be registered with `atexit'.  This is how the dynamic linker
	arranges to have DT_FINI functions called for shared libraries that have
	been loaded before this code runs.

   The stack contains the arguments and environment:
		0(%sp)			argc
		4(%sp)			argv[0]
		...
		(4*argc)(%sp)		NULL
		(4*(argc+1))(%sp)	envp[0]
		...
					NULL

   The uclinux FLAT format conventions are different.  d1, d2, d3 and a1 are not
   defined on entry and the stack is laid out as follows:

		0(%sp)			argc
		4(%sp)			argv
		8(%sp)			envp
*/


	.text
	.type   main,@function
	.type	_init,%function
	.type	_fini,%function
#ifndef __UCLIBC_CTOR_DTOR__
	.weak	_init
	.weak	_fini
#endif
	.globl	_start
	.global __uClibc_main;
	.type	__uClibc_main,@function;
	.type	_start,@function
_start:

#if defined(__UBICOM32_FDPIC__) && !defined(L_Scrt1)
	/* d1 contains a pointer to the program's load map.  */
	call	a3, .Lcall;
.Lcall:
	move.4	d0, d1 ; load exec_map_addr
	moveai	a4, #%hi(.Lcall)
	lea.1	d15, %lo(.Lcall)(a4)
	sub.4	d15, a3, d15	;  difference between .Lcall and actual address of .Lcall
	moveai	a3, #%hi(__ROFIXUP_LIST__)
	lea.1	d1, %lo(__ROFIXUP_LIST__)(a3)
	moveai	a3, #%hi(__ROFIXUP_END__)
	lea.1	d2, %lo(__ROFIXUP_END__)(a3)
	add.4	d1, d1, d15
	add.4	d2, d2, d15
	call	a5, __self_reloc	; returns GOT in d0
	move.4	a0, d0	; set GOT
#endif

/*
	all this is setup to make the following call.

	void __uClibc_main(
		d0 - int (*main)(int, char **, char **),
		d1 - int argc, (d1)
		d2 - char **argv,
		d3 - void (*app_init)(void),
		d4 - void (*app_fini)(void),
		d5 - void (*rtld_fini)(void),
		d6 - void *stack_end
	);
 */

	/*
	 * Load pointer to main into d0
	 */
#ifdef __UBICOM32_FDPIC__
	movei	d0, #%got_funcdesc_lo(main)
	move.4	d0, (a0, d0)
#else
	moveai	a3, #%hi(main)
	lea.1	d0, %lo(main)(a3)
#endif

	/*
	 * Grab argc, argv[] from stack and set up d1, d2 with it.
	 * Note that for FDPIC the calling convention and stack layout is
	 * different than flat (see above)
	 */
	move.4	d1, (sp)	; argc
#ifdef __UBICOM32_FDPIC__
	lea.1	d2, 4(sp)	; argv = sp + 4
#else
	move.4	d2, 4(sp)	; argv = *(sp + 4)
#endif

#ifdef __UCLIBC_CTOR_DTOR__
	/*
	 * Load pointer to _init into d3
	 */
#ifdef __UBICOM32_FDPIC__
	movei	d3, #%got_funcdesc_lo(_init)
	move.4	d3, (a0, d3)
#else
	moveai	a3, #%hi(_init)
	lea.1	d3, %lo(_init)(a3)
#endif

	/*
	 * Load pointer to _fini into d4
	 */
#ifdef __UBICOM32_FDPIC__
	movei	d4, #%got_funcdesc_lo(_fini)
	move.4	d4, (a0, d4)
#else
	moveai	a3, #%hi(_fini)
	lea.1	d4, %lo(_fini)(a3)
#endif

#else /* !__UCLIBC_CTOR_DTOR__ */
	move.4	d3, #0		; _init
	move.4	d4, #0		; _fini
#endif

#ifdef __UBICOM32_FDPIC__
	move.4	d5, a1		; ldso _fini funcdesc (see dl-startup.h)
#else
	movei	d5, #0		; rtld_fini (not used)
#endif

	move.4	d6, sp		; Stack End.

	/* Call the user's main function, and exit with its value.  But
	   let the libc call main.  */
#ifdef __UBICOM32_FDPIC__
	call	a5, __uClibc_main
#else
	moveai	a5, #%hi(__uClibc_main)
	calli	a5, %lo(__uClibc_main)(a5)
#endif

	bkpt #-1		; Crash if somehow `exit' does return.
