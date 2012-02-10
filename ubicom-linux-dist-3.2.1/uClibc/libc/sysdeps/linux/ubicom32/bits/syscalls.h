#ifndef _BITS_SYSCALLS_H
#define _BITS_SYSCALLS_H
#ifndef _SYSCALL_H
# error "Never use <bits/syscalls.h> directly; include <sys/syscall.h> instead."
#endif

/* m68k headers does stupid stuff with __NR_iopl / __NR_vm86:
 * #define __NR_iopl   not supported
 * #define __NR_vm86   not supported
 */
#undef __NR_iopl
#undef __NR_vm86

#include <config/autoconf.h>

#if defined(__ASSEMBLER__)
/*
 * Define assembly macros to hide syscall call differences
 */
#if (UBICOM32_ARCH_VERSION >= 3) && (UBICOM32_ARCH_VERSION <= 4)
.macro __syscall
	moveai a5, #%hi(0x3FFC0030)
	calli a5, 0x30(a5)
.endm
#elif (UBICOM32_ARCH_VERSION == 5)
.macro __syscall
	syscall a5
.endm
#else
#error "UInknown Ubicom32 processor"
#endif

#else // C code

#include <errno.h>

/* Ubicom32 Linux takes system call arguments in registers:

	syscall number	%d8
	arg 1		%d0
	arg 2		%d1
	arg 3		%d2
	arg 4		%d3
	arg 5		%d4
	arg 6		%d5
*/

#define __LOADARGS_0(name, dummy)					\
	d8 = name

#define __LOADARGS_1(name, __arg1)					\
	__LOADARGS_0(name, 0);						\
	d0_retval = (long int)(__arg1)

#define __LOADARGS_2(name, __arg1, __arg2)				\
	__LOADARGS_1(name, __arg1);					\
	d1 = (long int)(__arg2)

#define __LOADARGS_3(name, __arg1, __arg2, __arg3)			\
	__LOADARGS_2(name, __arg1, __arg2);				\
	d2 = (long int)(__arg3)

#define __LOADARGS_4(name, __arg1, __arg2, __arg3, __arg4)		\
	__LOADARGS_3(name, __arg1, __arg2, __arg3);			\
	d3 = (long int)(__arg4)

#define __LOADARGS_5(name, __arg1, __arg2, __arg3, __arg4, __arg5)	\
	__LOADARGS_4(name, __arg1, __arg2, __arg3, __arg4);		\
	d4 = (long int)(__arg5)

#define __LOADARGS_6(name, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
	__LOADARGS_5(name, __arg1, __arg2, __arg3, __arg4, __arg5);	\
	d5 = (long int)(__arg6)


/*
 * macro __INTERNAL_SYSCALL_BODY_NCS()
 *
 * Loads the nr args into the d0...d5 registers and set d8 to the syscall
 * number.  calls to 0x404000010 which is the syscall entry point, being
 * care-full to mark all the other registers as being clobbered (per default abi).
 *
 * The result from the syscall will be stored in d0_retval.
 */
#if (UBICOM32_ARCH_VERSION >= 3) && (UBICOM32_ARCH_VERSION <= 4)
#define __INTERNAL_SYSCALL_BODY_NCS(name, nr, args...)			\
	long int d0_copy;						\
	register long int d0_retval __asm__ ("d0");			\
	register long int d1 __asm__ ("d1");				\
	register long int d2 __asm__ ("d2");				\
	register long int d3 __asm__ ("d3");				\
	register long int d4 __asm__ ("d4");				\
	register long int d5 __asm__ ("d5");				\
	register long int d8 __asm__ ("d8");				\
	__LOADARGS_##nr (name, args);					\
	__asm__ __volatile__ (						\
	"	moveai a5, #%%hi(0x3FFC0030)\n\t"			\
	"	calli a5, 0x30(a5)\n\t"					\
		: "+r" (d0_retval), "+r" (d1), "+r" (d2), "+r" (d3),	\
		  "+r" (d4), "+r" (d5),  "+r" (d8) :			\
		: "cc", "memory",					\
		  "acc0_lo", "acc0_hi", "acc1_lo", "acc1_hi",		\
		  "source3",						\
		  "a0", "a3", "a4", "a5",				\
		  "d6", "d7", "d9", "d14", "d15"			\
	);
#elif (UBICOM32_ARCH_VERSION == 5)
#define __INTERNAL_SYSCALL_BODY_NCS(name, nr, args...)			\
	long int d0_copy;						\
	register long int d0_retval __asm__ ("d0");			\
	register long int d1 __asm__ ("d1");				\
	register long int d2 __asm__ ("d2");				\
	register long int d3 __asm__ ("d3");				\
	register long int d4 __asm__ ("d4");				\
	register long int d5 __asm__ ("d5");				\
	register long int d8 __asm__ ("d8");				\
	__LOADARGS_##nr (name, args);					\
	__asm__ __volatile__ (						\
	"	syscall a5	\n\t"					\
		: "+r" (d0_retval), "+r" (d1), "+r" (d2), "+r" (d3),	\
		  "+r" (d4), "+r" (d5),  "+r" (d8) :			\
		: "cc", "memory",					\
		  "acc0_lo", "acc0_hi", "acc1_lo", "acc1_hi",		\
		  "source3",						\
		  "a0", "a3", "a4", "a5",				\
		  "d6", "d7", "d9", "d14", "d15"			\
	);
#else
#error "Unknown Ubicom32 Processor"
#endif

#define INTERNAL_SYSCALL_NCS(name, err, nr, args...)			\
	({								\
		__INTERNAL_SYSCALL_BODY_NCS(name, nr, args);		\
		d0_retval;						\
	})

#define INLINE_SYSCALL_NCS(name, nr, args...)				\
	({								\
		__INTERNAL_SYSCALL_BODY_NCS(name, nr, args);		\
		if (unlikely((unsigned long)(d0_retval) >= (unsigned long)(-125))) { \
			d0_copy = d0_retval;				\
			__set_errno (-d0_copy);				\
			d0_retval = -1L;				\
		}							\
		d0_retval;						\
	})
/*
 * The following can be removed once we are on the top of the uClibc
 * trunk
 */
#define INTERNAL_SYSCALL_DECL(err)         do { } while (0)
#define INTERNAL_SYSCALL_ERROR_P(val, err) ((unsigned long)val >= (unsigned long)(-125))
#define INTERNAL_SYSCALL_ERRNO(val, err)   (-(val))
#define INTERNAL_SYSCALL(name, err, nr, args...) INTERNAL_SYSCALL_NCS(__NR_##name, err, nr, args)
#define INLINE_SYSCALL(name, nr, args...) INLINE_SYSCALL_NCS(__NR_##name, nr, args)

#define C_DECL_ARGS_0()			void
#define C_DECL_ARGS_1(t, v)		t v
#define C_DECL_ARGS_2(t, v, args...)	t v, C_DECL_ARGS_1(args)
#define C_DECL_ARGS_3(t, v, args...)	t v, C_DECL_ARGS_2(args)
#define C_DECL_ARGS_4(t, v, args...)	t v, C_DECL_ARGS_3(args)
#define C_DECL_ARGS_5(t, v, args...)	t v, C_DECL_ARGS_4(args)
#define C_DECL_ARGS_6(t, v, args...)	t v, C_DECL_ARGS_5(args)

#define C_ARGS_0()
#define C_ARGS_1(t, v)			v
#define C_ARGS_2(t, v, args...)		v, C_ARGS_1(args)
#define C_ARGS_3(t, v, args...)		v, C_ARGS_2(args)
#define C_ARGS_4(t, v, args...)		v, C_ARGS_3(args)
#define C_ARGS_5(t, v, args...)		v, C_ARGS_4(args)
#define C_ARGS_6(t, v, args...)		v, C_ARGS_5(args)

#define SYSCALL_FUNC(nargs, type, name, args...) \
type __attribute__ (( optimize("Os") )) name(C_DECL_ARGS_##nargs(args)) { \
	return (type) INLINE_SYSCALL(name, nargs, C_ARGS_##nargs(args)); \
}

#define _syscall0(args...)		SYSCALL_FUNC(0, args)
#define _syscall1(args...)		SYSCALL_FUNC(1, args)
#define _syscall2(args...)		SYSCALL_FUNC(2, args)
#define _syscall3(args...)		SYSCALL_FUNC(3, args)
#define _syscall4(args...)		SYSCALL_FUNC(4, args)
#define _syscall5(args...)		SYSCALL_FUNC(5, args)
#define _syscall6(args...)		SYSCALL_FUNC(6, args)

#endif /* __ASSEMBLER__ */
#endif /* _BITS_SYSCALLS_H */
