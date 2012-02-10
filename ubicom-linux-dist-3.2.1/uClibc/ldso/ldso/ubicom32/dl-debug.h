/* vi: set sw=4 ts=4: */
/* Ubicom32 ELF shared library loader suppport
 *
 * Copyright (c) 2009		Ubicom Inc.
 * Copyright (c) 1994-2000 Eric Youngdale, Peter MacDonald,
 *                              David Engel, Hongjiu Lu and Mitch D'Souza
 * Copyright (C) 2001-2004 Erik Andersen
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. The name of the above contributors may not be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

static const char *_dl_reltypes_tab[] =
{
	"R_UBICOM32_NONE", /* 0 */
	"R_UBICOM32_16",
	"R_UBICOM32_32",
	"R_UBICOM32_LO16",
	"R_UBICOM32_HI16",
	"R_UBICOM32_21_PCREL", /* 5 */
	"R_UBICOM32_24_PCREL",
	"R_UBICOM32_HI24",
	"R_UBICOM32_LO7_S",
	"R_UBICOM32_LO7_2_S",
	"R_UBICOM32_LO7_4_S", /* 10 */
	"R_UBICOM32_LO7_D",
	"R_UBICOM32_LO7_2_D",
	"R_UBICOM32_LO7_4_D",
	"R_UBICOM32_32_HARVARD",
	"R_UBICOM32_LO7_CALLI", /* 15 */
	"R_UBICOM32_LO16_CALLI",
	"R_UBICOM32_GOT_HI24",
	"R_UBICOM32_GOT_LO7_S",
	"R_UBICOM32_GOT_LO7_2_S"
	"R_UBICOM32_GOT_LO7_4_S", /* 20 */
	"R_UBICOM32_GOT_LO7_D",
	"R_UBICOM32_GOT_LO7_2_D",
	"R_UBICOM32_GOT_LO7_4_D",
	"R_UBICOM32_FUNCDESC_GOT_HI24	24",
	"R_UBICOM32_FUNCDESC_GOT_LO7_S", /* 25 */
	"R_UBICOM32_FUNCDESC_GOT_LO7_2_S",
	"R_UBICOM32_FUNCDESC_GOT_LO7_4_S",
	"R_UBICOM32_FUNCDESC_GOT_LO7_D",
	"R_UBICOM32_FUNCDESC_GOT_LO7_2_D",
	"R_UBICOM32_FUNCDESC_GOT_LO7_4_D", /* 30 */
	"R_UBICOM32_GOT_LO7_CALLI",
	"R_UBICOM32_FUNCDESC_VALUE",
	"R_UBICOM32_FUNCDESC",
#if 1
	[200] "R_UBICOM32_GNU_VTINHERIT" , "R_UBICOM32_GNU_VTENTRY"
#endif
};
