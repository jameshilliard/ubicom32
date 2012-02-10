     /* Copyright (C) 2003 Red Hat, Inc.
	Contributed by Alexandre Oliva <aoliva@redhat.com>

	Copyright (C) 2009 Ubicom Inc.
	Ported to Ubicom32 by Ubicom Inc.

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
 * The function below is tail-called by resolver stubs when a lazily-bound *
 * function is called.  It must preserve all registers that could * be used to
 * pass arguments to the actual function.
 *
 * On entry to the function d0-d13 contain parameters to the actual function of
 * interest
 *
 * a5 contains the return address
 * a0 is pointing to the GOT table for the original function
 *  (a0) _dl_linux_resolve
 * 4(a0) GOT for _dl_linux_resolve
 * 8(a0) is pointer to "structure elf_resolve" of the module where the call
 *	originated from
 *
 * a3 is pointing to the function descriptor in the GOT table, and 8(a3) the
 * resolver lookup information.
 *
 * _dl_linux_resolve calls _dl_linux_resolver passing it pointer to
 * struct elf_resolve and the relocation entry.
 *
 * _dl_linux_resolver() figures out where the jump symbol is _really_ supposed
 * to have jumped to and returns that to us. Once we have that, we prepare to
 * tail-call the actual function, clean up after ourselves, restoring the
 * original arguments, then jump to the fixed up address.
 */

#if 0
	/*
	 * Here is what the linker will use for the PLT.
	 */
sample_PLT_functionX:	/* (at entry a0 contains GOT for this library)  */
	movei	d15, # -%lo(got_funcdescX) ; load offset for GOT
	lea.4	a3, (a0, d15)	; a4 is now the function descriptor
	move.4	a4, 0(a3)	; get entry_point
	move.4	a0, 4(a3)	; set new GOT

	/*
	 * jump to resovled function OR PLT_trampoline Nat had some code here
	 * that uses the stack instead of a3 to reduces hazards.
	 */
	calli	a4, 0(a4)


sample_PLT_trampoline:
	/*
	 * find the old got by undoing what was done above. An alternative
	 * could have been to ‘save’ the old got in say d14 but that would add 1
	 * instruction to every PLT and there is only 1 PLT trampoline per library.
	 */
	ret	(a0)		; jump to _dl_linux_resolve
#endif

	.text
	.p2align 4

	.hidden	_dl_linux_resolve
	.global	_dl_linux_resolve
	.type	_dl_linux_resolve,@function
_dl_linux_resolve:
	/* Preserve arguments and return address  */
	move.4	-4(sp)++, d0
	move.4	-4(sp)++, d1
	move.4	-4(sp)++, d2
	move.4	-4(sp)++, d3
	move.4	-4(sp)++, d4
	move.4	-4(sp)++, d5
	move.4	-4(sp)++, d6
	move.4	-4(sp)++, d7
	move.4	-4(sp)++, d8
	move.4	-4(sp)++, d9
	move.4	-4(sp)++, d10
	move.4	-4(sp)++, d11
	move.4	-4(sp)++, d12
	move.4	-4(sp)++, d13
	move.4	-4(sp)++, a5

	/* Prepare to call _dl_linux_resolver.  */
	move.4 d0, 8(a0)	; reference to elf_resolve
	/* Not aligned for space reasons.  */
	move.4 d1, mac_hi 	; reference to GOT table entry which
				; contains the relocation information.

	move.4 a0, 4(a0) ; switch to GOT for _dl_linux_resolve
	call	a5, _dl_linux_resolver;

	/* Move aside return value that contains the FUNCDESC_VALUE.  */
	;P3 = R0;
	move.4 a3, d0;

	/* Restore arguments.  */
	move.4	a5, (sp)4++
	move.4	d13, (sp)4++
	move.4	d12, (sp)4++
	move.4	d11, (sp)4++
	move.4	d10, (sp)4++
	move.4	d9, (sp)4++
	move.4	d8, (sp)4++
	move.4	d7, (sp)4++
	move.4	d6, (sp)4++
	move.4	d5, (sp)4++
	move.4	d4, (sp)4++
	move.4	d3, (sp)4++
	move.4	d2, (sp)4++
	move.4	d1, (sp)4++
	move.4	d0, (sp)4++

	/* Now jump to the actual function.  */
	/* a3 contains func_desc resolution */
	move.4 a4, 0(a3)	; address of function X
	move.4 a0, 4(a3)	; switch to GOT for function X
	calli  a4, 0(a4)	; call through a4, a5 remains

	.size	_dl_linux_resolve, . - _dl_linux_resolve

	.hidden	_dl_ubicom32_resolve_pending
	.global	_dl_ubicom32_resolve_pending
	.type	_dl_ubicom32_resolve_pending,@function

_dl_ubicom32_resolve_pending:
	/*
	 * A special function that is used to ensure thread saftly when the fd
	 * for a particular resolution is being updated.
	 *
	 * At entry, a3 must point to the FD.  While the FD is being updated the
	 * entry_point will continue to point to _dl_ubicom32_resolve_pending so
	 * we will effectively spin until the resolver update is complete.
	 */

	move.4	a4, 0(a3)	; get entry_point
	move.4	a0, 4(a3)	; set new GOT

	/*
	 * jump to resovled function or back to _dl_ubicom32_resolve_pending.
	 */
	calli	a4, 0(a4)
	.size	_dl_ubicom32_resolve_pending, . - _dl_ubicom32_resolve_pending
