/*
 * arch/ubicom32/include/asm/uaccess.h
 *   User space memory access functions for Ubicom32 architecture.
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
 *   arch/alpha
 */
#ifndef _ASM_UBICOM32_UACCESS_H
#define _ASM_UBICOM32_UACCESS_H

/*
 * User space memory access functions
 */
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/string.h>

#include <asm/machdep.h>
#include <asm/segment.h>
#include <asm/memoryops.h>

#define VERIFY_READ	0
#define VERIFY_WRITE	1

/*
 * The exception table consists of pairs of addresses: the first is the
 * address of an instruction that is allowed to fault, and the second is
 * the address at which the program should continue.  No registers are
 * modified, so it is entirely up to the continuation code to figure out
 * what to do.
 *
 * All the routines below use bits of fixup code that are out of line
 * with the main instruction path.  This means when everything is well,
 * we don't even have to jump over them.  Further, they do not intrude
 * on our cache or tlb entries.
 */
struct exception_table_entry
{
	unsigned long insn, fixup;
};

/*
 * Ubicom32 does not currently support the exception table handling.
 */
extern unsigned long search_exception_table(unsigned long);


#if defined(CONFIG_ACCESS_OK_CHECKS_ENABLED)
extern int __access_ok(unsigned long addr, unsigned long size);
#else
static inline int __access_ok(unsigned long addr, unsigned long size)
{
#if defined(CONFIG_MMU)
	return ((addr + size) <= current_thread_info()->addr_limit.seg);
#else
	return 1;
#endif
}
#endif
#define access_ok(type, addr, size) \
	likely(__access_ok((unsigned long)(addr), (size)))

/*
 * The following functions do not exist.  They keep callers
 * of put_user and get_user from passing unsupported argument
 * types.  They result in a link time error.
 */
extern int __put_user_bad(void);
extern int __get_user_bad(void);

/*
 * The following function is used to transfer to and from user space when
 * the transfer size is not one of the basic sizes such as 1, 2 or 4.  It
 * can handle any arbitrary buffer size.
 */
extern unsigned long __copy_user(void *to, const void *from, unsigned long size);

/*
 * __put_user_no_check()
 *	Put the requested data into the user space.
 *
 * Careful not to:
 * (a) re-use the arguments for side effects (sizeof/typeof is ok)
 * (b) require any knowledge of processes at this stage
 *
 * We use the linker script to remove the __ex_table and fixup sections
 * for NOMMU port.
 */
#define __put_user_no_check(x, ptr, size)					\
({										\
	int __punc_err = 0;							\
	void *__punc_tmp_addr = 0;						\
	__typeof__(*(ptr)) __user *__punc_addr = (ptr);				\
	switch (size) {								\
	case 1:									\
		asm volatile (							\
		";						\n\t"		\
		"; Place the addr of the move instruction in	\n\t"		\
		"; the ex_table, followed by the fixup addr.	\n\t"		\
		"; On a fault, the fixup code will be invoked	\n\t"		\
		"; by do_page_fault() changing the PC.		\n\t"		\
		";						\n\t"		\
		".section __ex_table, \"ax\"			\n\t"		\
		".word 1f					\n\t"		\
		".word 3f					\n\t"		\
		".previous					\n\t"		\
		"1:	move.1		(%[dptr]), %[val]	\n\t"		\
		"2:						\n\t"		\
		"						\n\t"		\
		";						\n\t"		\
		"; The following code is moved to the fixup	\n\t"		\
		"; section so that no jumps are used in the 	\n\t"		\
		"; default case.			 	\n\t"		\
		";						\n\t"		\
		".section .fixup, \"ax\"			\n\t"		\
		"3:	movei		%[ret], #"D(-EFAULT)"	\n\t"		\
		"	call		%[tmp], 2b		\n\t"		\
		".previous					\n\t"		\
		: [ret] "+r" (__punc_err),					\
		  [tmp] "=a" (__punc_tmp_addr) 					\
		: [dptr] "a" (__punc_addr),					\
		  [val] "r" ((__typeof__(*(ptr)))x)				\
		);								\
		break;								\
	case 2:									\
		asm volatile (							\
		";						\n\t"		\
		"; Place the addr of the move instruction in	\n\t"		\
		"; the ex_table, followed by the fixup addr.	\n\t"		\
		"; On a fault, the fixup code will be invoked	\n\t"		\
		"; by do_page_fault() changing the PC.		\n\t"		\
		";						\n\t"		\
		".section __ex_table, \"ax\"			\n\t"		\
		".word 1f					\n\t"		\
		".word 3f					\n\t"		\
		".previous					\n\t"		\
		"1:	move.2		(%[dptr]), %[val]	\n\t"		\
		"2:						\n\t"		\
		"						\n\t"		\
		";						\n\t"		\
		"; The following code is moved to the fixup	\n\t"		\
		"; section so that no jumps are used in the 	\n\t"		\
		"; default case.			 	\n\t"		\
		";						\n\t"		\
		".section .fixup, \"ax\"			\n\t"		\
		"3:	movei		%[ret], #"D(-EFAULT)"	\n\t"		\
		"	call		%[tmp], 2b		\n\t"		\
		".previous					\n\t"		\
		: [ret] "+r" (__punc_err),					\
		  [tmp] "=a" (__punc_tmp_addr) 					\
		: [dptr] "a" (__punc_addr),					\
		  [val] "r" ((__typeof__(*(ptr)))x)				\
		);								\
		break;								\
	case 4:									\
		asm volatile (							\
		";						\n\t"		\
		"; Place the addr of the move instruction in	\n\t"		\
		"; the ex_table, followed by the fixup addr.	\n\t"		\
		"; On a fault, the fixup code will be invoked	\n\t"		\
		"; by do_page_fault() changing the PC.		\n\t"		\
		";						\n\t"		\
		".section __ex_table, \"ax\"			\n\t"		\
		".word 1f					\n\t"		\
		".word 3f					\n\t"		\
		".previous					\n\t"		\
		"1:	move.4		(%[dptr]), %[val]	\n\t"		\
		"2:						\n\t"		\
		"						\n\t"		\
		";						\n\t"		\
		"; The following code is moved to the fixup	\n\t"		\
		"; section so that no jumps are used in the 	\n\t"		\
		"; default case.			 	\n\t"		\
		";						\n\t"		\
		".section .fixup, \"ax\"			\n\t"		\
		"3:	movei		%[ret], #"D(-EFAULT)"	\n\t"		\
		"	call		%[tmp], 2b		\n\t"		\
		".previous					\n\t"		\
		: [ret] "+r" (__punc_err),					\
		  [tmp] "=a" (__punc_tmp_addr) 					\
		: [dptr] "a" (__punc_addr),					\
		  [val] "r" ((__typeof__(*(ptr)))x)				\
		);								\
		break;								\
	case 8:									\
	{									\
		/*								\
		 * Force the compiler into using our only 64 bit register	\
		 * type.							\
		 */								\
		register const __typeof__(*(ptr))				\
			__punc_val asm("acc0_hi") = (x);			\
										\
		asm volatile (							\
		";						\n\t"		\
		"; Place the addr of the move instruction in	\n\t"		\
		"; the ex_table, followed by the fixup addr.	\n\t"		\
		"; On a fault, the fixup code will be invoked	\n\t"		\
		"; by do_page_fault() changing the PC.		\n\t"		\
		";						\n\t"		\
		".section __ex_table, \"ax\"			\n\t"		\
		".word 1f					\n\t"		\
		".word 4f					\n\t"		\
		".word 2f					\n\t"		\
		".word 4f					\n\t"		\
		".previous					\n\t"		\
		"1:	move.4		(%[dptr]), acc0_hi	\n\t"		\
		"2:	move.4		4(%[dptr]), acc0_lo	\n\t"		\
		"3:						\n\t"		\
		"						\n\t"		\
		";						\n\t"		\
		"; The following code is moved to the fixup	\n\t"		\
		"; section so that no jumps are used in the 	\n\t"		\
		"; default case.			 	\n\t"		\
		";						\n\t"		\
		".section .fixup, \"ax\"			\n\t"		\
		"4:	movei		%[ret], #"D(-EFAULT)"	\n\t"		\
		"	call		%[tmp], 3b		\n\t"		\
		".previous					\n\t"		\
		: [ret] "+r" (__punc_err),					\
		  [tmp] "=a" (__punc_tmp_addr) 					\
		: [dptr] "a" (__punc_addr),					\
		  [val] "h" (__punc_val)					\
		);								\
		break;								\
	}									\
	default:								\
		__punc_err = __put_user_bad();					\
		break;								\
	}									\
	__punc_err;								\
})

/*
 * __get_user_no_check()
 *	Read the value at ptr into x.
 *
 * If requested, access_ok() will verify that ptr is a valid user
 * pointer.  If the caller passes a modifying argument for ptr (e.g. x++)
 * this macro will not work.
 */
#define __get_user_no_check(x, ptr, size)					\
({										\
	int __gunc_err = 0;							\
	void *__gunc_tmp_addr = 0;						\
	__typeof__((x)) __gunc_val = 0;						\
	const __typeof__(*(ptr)) __user *__gunc_addr = (ptr);			\
	switch (size) {								\
	case 1:									\
		asm volatile (							\
		";						\n\t"		\
		"; Place the addr of the move instruction in	\n\t"		\
		"; the ex_table, followed by the fixup addr.	\n\t"		\
		"; On a fault, the fixup code will be invoked	\n\t"		\
		"; by do_page_fault() changing the PC.		\n\t"		\
		";						\n\t"		\
		".section __ex_table, \"ax\"			\n\t"		\
		".word 1f					\n\t"		\
		".word 3f					\n\t"		\
		".previous					\n\t"		\
		"1:	move.1		%[val], (%[sptr])	\n\t"		\
		"2:						\n\t"		\
		"						\n\t"		\
		";						\n\t"		\
		"; The following code is moved to the fixup	\n\t"		\
		"; section so that no jumps are used in the 	\n\t"		\
		"; default case.			 	\n\t"		\
		";						\n\t"		\
		".section .fixup, \"ax\"			\n\t"		\
		"3:	movei		%[ret], #"D(-EFAULT)"	\n\t"		\
		"	call		%[tmp], 2b		\n\t"		\
		".previous					\n\t"		\
		: [ret] "+r" (__gunc_err),					\
		  [tmp] "=a" (__gunc_tmp_addr),					\
		  [val] "+r" (__gunc_val)					\
		: [sptr] "a"  (__gunc_addr)					\
		);								\
		break;								\
	case 2:									\
		asm volatile (							\
		";						\n\t"		\
		"; Place the addr of the move instruction in	\n\t"		\
		"; the ex_table, followed by the fixup addr.	\n\t"		\
		"; On a fault, the fixup code will be invoked	\n\t"		\
		"; by do_page_fault() changing the PC.		\n\t"		\
		";						\n\t"		\
		".section __ex_table, \"ax\"			\n\t"		\
		".word 1f					\n\t"		\
		".word 3f					\n\t"		\
		".previous					\n\t"		\
		"1:	move.2		%[val], (%[sptr])	\n\t"		\
		"2:						\n\t"		\
		"						\n\t"		\
		";						\n\t"		\
		"; The following code is moved to the fixup	\n\t"		\
		"; section so that no jumps are used in the 	\n\t"		\
		"; default case.			 	\n\t"		\
		";						\n\t"		\
		".section .fixup, \"ax\"			\n\t"		\
		"3:	movei		%[ret], #"D(-EFAULT)"	\n\t"		\
		"	call		%[tmp], 2b		\n\t"		\
		".previous					\n\t"		\
		: [ret] "+r" (__gunc_err),					\
		  [tmp] "=a" (__gunc_tmp_addr),					\
		  [val] "+r" (__gunc_val)					\
		: [sptr] "a"  (__gunc_addr)					\
		);								\
		break;								\
	case 4:									\
		asm volatile (							\
		";						\n\t"		\
		"; Place the addr of the move instruction in	\n\t"		\
		"; the ex_table, followed by the fixup addr.	\n\t"		\
		"; On a fault, the fixup code will be invoked	\n\t"		\
		"; by do_page_fault() changing the PC.		\n\t"		\
		";						\n\t"		\
		".section __ex_table, \"ax\"			\n\t"		\
		".word 1f					\n\t"		\
		".word 3f					\n\t"		\
		".previous					\n\t"		\
		"1:	move.4		%[val], (%[sptr])	\n\t"		\
		"2:						\n\t"		\
		"						\n\t"		\
		";						\n\t"		\
		"; The following code is moved to the fixup	\n\t"		\
		"; section so that no jumps are used in the 	\n\t"		\
		"; default case.			 	\n\t"		\
		";						\n\t"		\
		".section .fixup, \"ax\"			\n\t"		\
		"3:	movei		%[ret], #"D(-EFAULT)"	\n\t"		\
		"	call		%[tmp], 2b		\n\t"		\
		".previous					\n\t"		\
		: [ret] "+r" (__gunc_err),					\
		  [tmp] "=a" (__gunc_tmp_addr),					\
		  [val] "+r" (__gunc_val)					\
		: [sptr] "a"  (__gunc_addr)					\
		);								\
		break;								\
	case 8:									\
		asm volatile (							\
		";						\n\t"		\
		"; Place the addr of the move instruction in	\n\t"		\
		"; the ex_table, followed by the fixup addr.	\n\t"		\
		"; On a fault, the fixup code will be invoked	\n\t"		\
		"; by do_page_fault() changing the PC.		\n\t"		\
		";						\n\t"		\
		".section __ex_table, \"ax\"			\n\t"		\
		".word 1f					\n\t"		\
		".word 4f					\n\t"		\
		".word 2f					\n\t"		\
		".word 4f					\n\t"		\
		".previous					\n\t"		\
		"1:	move.4		acc0_hi, (%[sptr])	\n\t"		\
		"2:	move.4		acc0_lo, 4(%[sptr])	\n\t"		\
		"3:						\n\t"		\
		"						\n\t"		\
		";						\n\t"		\
		"; The following code is moved to the fixup	\n\t"		\
		"; section so that no jumps are used in the 	\n\t"		\
		"; default case.			 	\n\t"		\
		";						\n\t"		\
		".section .fixup, \"ax\"			\n\t"		\
		"4:	movei		%[ret], #"D(-EFAULT)"	\n\t"		\
		"	call		%[tmp], 3b		\n\t"		\
		".previous					\n\t"		\
		: [ret] "+r" (__gunc_err),					\
		  [tmp] "=a" (__gunc_tmp_addr),					\
		  [val] "=h" (__gunc_val)					\
		: [sptr] "a" (__gunc_addr)					\
		: "acc0_lo", "acc0_hi"						\
		);								\
		break;								\
	default:								\
		__gunc_err = __get_user_bad();					\
		break;								\
	}									\
	(x) = __gunc_val;							\
	__gunc_err;								\
})

/*
 * __put_user_check()
 *	Put the requested data into the user space verifying the address
 *
 * Careful to not
 * (a) re-use the arguments for side effects (sizeof/typeof is ok)
 * (b) require any knowledge of processes at this stage
 *
 * If requested, access_ok() will verify that ptr is a valid user
 * pointer.
 */
#define __put_user_check(x, ptr, size)					\
({									\
	int __pu_err = -EFAULT;						\
	__typeof__(*(ptr)) __user *__pu_addr = (ptr);			\
	if (access_ok(VERIFY_WRITE, __pu_addr, size)) {			\
		__pu_err = __put_user_no_check(x, __pu_addr, size);	\
	}								\
	__pu_err;							\
})

/*
 * __get_user_check()
 *	Read the value at ptr into x.
 *
 * Careful to not
 * (a) re-use the arguments for side effects (sizeof/typeof is ok)
 * (b) require any knowledge of processes at this stage
 *
 * If requested, access_ok() will verify that ptr is a valid user
 * pointer.
 */
#define __get_user_check(x, ptr, size)					\
({									\
	int __gu_err = -EFAULT;						\
	const __typeof__(*(ptr)) __user *__gu_addr = (ptr);		\
	if (access_ok(VERIFY_READ, __gu_addr, size)) {			\
		__gu_err = __get_user_no_check(x, __gu_addr, size);	\
	}								\
	__gu_err;							\
})

/*
 * The "xxx" versions are allowed to perform some amount of address
 * space checking.  See access_ok().
 */
#define put_user(x,ptr) \
	__put_user_check((__typeof__(*(ptr)))(x),(ptr), sizeof(*(ptr)))
#define get_user(x,ptr) \
	__get_user_check((x), (ptr), sizeof(*(ptr)))

/*
 * The "__xxx" versions do not do address space checking, useful when
 * doing multiple accesses to the same area (the programmer has to do the
 * checks by hand with "access_ok()")
 */
#define __put_user(x,ptr) \
	__put_user_no_check((__typeof__(*(ptr)))(x),(ptr), sizeof(*(ptr)))
#define __get_user(x,ptr) \
	__get_user_no_check((x), (ptr), sizeof(*(ptr)))

/*
 * __copy_to_user_no_check()
 *	Copy the data to user space.
 *
 * Return the number of bytes NOT copied.
 */
static inline unsigned long
__copy_to_user_no_check(void *to, const void *from, unsigned long n)
{
	return __copy_user((__force void *)to, from, n);
}

/*
 * __copy_from_user_no_check()
 *	Copy the data either to or from user space.
 *
 * Return the number of bytes NOT copied.
 */
static inline unsigned long
__copy_from_user_no_check(void *to, const void *from, unsigned long n)
{
	return __copy_from_user_zeroing(to, (__force void *)from, n);
}

/*
 * copy_to_user()
 * 	Copy the kernel data to user space.
 *
 * Return the number of bytes that were copied.
 */
static inline unsigned long
copy_to_user(void __user *to, const void *from, unsigned long n)
{
	BUG_ON(irqs_disabled());
	if (!access_ok(VERIFY_WRITE, to, n)) {
		return n;
	}
	return __copy_to_user_no_check((__force void *)to, from, n);
}

/*
 * copy_from_user()
 * 	Copy the user data to kernel space.
 *
 * Return the number of bytes that were copied.  On error, we zero
 * out the destination.
 */
static inline unsigned long
copy_from_user(void *to, const void __user *from, unsigned long n)
{
	BUG_ON(irqs_disabled());
	if (!access_ok(VERIFY_READ, from, n)) {
		return n;
	}
	return __copy_from_user_no_check(to, (__force void *)from, n);
}

#define __copy_to_user(to, from, n) \
	__copy_to_user_no_check((__force void *)to, from, n)
#define __copy_from_user(to, from, n) \
	__copy_from_user_no_check(to, (__force void *)from, n)
#define __copy_to_user_inatomic(to, from, n) \
	__copy_to_user_no_check((__force void *)to, from, n)
#define __copy_from_user_inatomic(to, from, n) \
	__copy_from_user_no_check(to, (__force void *)from, n)

/*
 * strncpy_from_user()
 *	Copy a null terminated string from userspace.
 *
 * dst - Destination in kernel space.  The buffer must be at least count.
 * src - Address of string in user space.
 * count - Maximum number of bytes to copy (including the trailing NULL).
 *
 * Returns the length of the string (not including the trailing NULL.  If
 * count is smaller than the length of the string, we copy count bytes
 * and return count.  Return -EFAULT if there is an access violation.
 *
 */
static inline long strncpy_from_user(char *dst, const __user char *src, long count)
{
	if (!access_ok(VERIFY_READ, src, 1)) {
		return -EFAULT;
	}

	return __strncpy_from_user(dst, src, count);
}

/*
 * strnlen_user()
 *	Return the size of a string (including the ending 0)
 *
 * Return -EFAULT on exception, a value greater than <n> if too long
 */
static inline long strnlen_user(const __user char *src, long n)
{
	if (!access_ok(VERIFY_READ, src, 1)) {
		return -EFAULT;
	}
	return __strnlen_user(src, n);
}

#define strlen_user(str) strnlen_user(str, 0x7fffffff)

/*
 * clear_user()
 *	Zero user space (check for valid addresses)
 */
static inline unsigned long clear_user(__user void *to, unsigned long n)
{
	if (!access_ok(VERIFY_WRITE, to, n)) {
		return -EFAULT;
	}
	return __clear_user(to, n);
}

#endif /* _ASM_UBICOM32_UACCESS_H */
