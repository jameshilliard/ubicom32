/*
 * arch/ubicom32/include/asm/ftrace.h
 *   Ubicom32 Function Trace macros
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#ifndef _ASM_UBICOM32_FTRACE_H
#define _ASM_UBICOM32_FTRACE_H

/*
 * Ubicom32 compiler does not support __builtin_return_address()
 * information.  Instead use the approximate call/calli tracing
 * to determine best guess at caller information.
 */
#define HAVE_ARCH_CALLER_ADDR

#if defined(CONFIG_STACKTRACE) && defined(CONFIG_FRAME_POINTER)
#  define CALLER_ADDR0 ((unsigned long)__builtin_return_address(0))
#  define CALLER_ADDR1 return_address(1)
#  define CALLER_ADDR2 return_address(2)
#  define CALLER_ADDR3 return_address(3)
#  define CALLER_ADDR4 return_address(4)
#  define CALLER_ADDR5 return_address(5)
#  define CALLER_ADDR6 return_address(6)
extern unsigned long return_address(unsigned int);
#else
#  define CALLER_ADDR0 ((unsigned long)__builtin_return_address(0))
#  define CALLER_ADDR1 0UL
#  define CALLER_ADDR2 0UL
#  define CALLER_ADDR3 0UL
#  define CALLER_ADDR4 0UL
#  define CALLER_ADDR5 0UL
#  define CALLER_ADDR6 0UL
#endif

#endif /* __ASM_UBICOM32_FTACE_H */
