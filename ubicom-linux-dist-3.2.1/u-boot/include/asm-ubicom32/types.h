/*
 * (C) Copyright 2009
 * Ubicom, Inc. www.ubicom.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef _UBICOM32_TYPES_H
#define _UBICOM32_TYPES_H

/*
 * This file is never included by application software unless
 * explicitly requested (e.g., via linux/types.h) in which case the
 * application is Linux specific so (user-) name space pollution is
 * not a major issue.  However, for interoperability, libraries still
 * need to be careful to avoid a name clashes.
 */

#ifndef __ASSEMBLY__

typedef unsigned short umode_t;

/*
 * __xx is ok: it doesn't pollute the POSIX namespace. Use these in the
 * header files exported to user space
 */

typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;

#if defined(__GNUC__)
__extension__ typedef __signed__ long long __s64;
__extension__ typedef unsigned long long __u64;
#endif

#endif /* __ASSEMBLY__ */

/*
 * These aren't exported outside the kernel to avoid name space clashes
 */
#ifdef __KERNEL__

#define BITS_PER_LONG 32

#ifndef __ASSEMBLY__

typedef signed char s8;
typedef signed char s8_t;
typedef unsigned char u8;
typedef unsigned char u8_t;

typedef signed short s16;
typedef unsigned short u16;
typedef signed short s16_t;
typedef unsigned short u16_t;

typedef signed int s32;
typedef unsigned int u32;
typedef signed int s32_t;
typedef unsigned int u32_t;

typedef signed long long s64;
typedef unsigned long long u64;
typedef signed long long s64_t;
typedef unsigned long long u64_t;

/* DMA addresses are always 32-bits wide */

typedef u32 dma_addr_t;
typedef u32 dma64_addr_t;

typedef u32_t bool_t;

typedef u32_t mem_addr_t;
typedef u32_t addr_t;

typedef unsigned short int fast_bool_t;
typedef signed int fast_s8_t;
typedef unsigned int fast_u8_t;
typedef signed int fast_s16_t;
typedef unsigned int fast_u16_t;
typedef signed int fast_s32_t;
typedef unsigned int fast_u32_t;
typedef signed long long int fast_s64_t;
typedef unsigned long long int fast_u64_t;

#endif /* __ASSEMBLY__ */

typedef unsigned long phys_addr_t;
typedef unsigned long phys_size_t;

#endif /* __KERNEL__ */

#endif /* _UBICOM32_TYPES_H */

