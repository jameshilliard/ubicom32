#include <linux/sys.h>
#include <linux/linkage.h>
#include <asm/unistd.h>

#ifdef CONFIG_SUPPORT_LIGHTWEIGHT_ATOMICS
	
.text
ALIGN
	.global lw_sys_call_table
lw_sys_call_table:	
	.long __lw_atomic_cmpxchg
	.long __lw_atomic_dec
	.long __lw_atomic_inc
	.long __lw_atomic_swap
	.long __lw_atomic_add
	.long __lw_atomic_and
	.long __lw_atomic_or
	.long __lw_atomic_invalid

#endif /* CONFIG_SUPPORT_LIGHTWEIGHT_ATOMICS */
