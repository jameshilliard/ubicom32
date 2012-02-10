/*
 * arch/ubicom32/kernel/processor.c
 *   Ubicom32 architecture processor info implementation.
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
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/profile.h>
#include <linux/clocksource.h>
#include <linux/types.h>
#include <linux/seq_file.h>
#include <linux/delay.h>
#include <linux/cpu.h>
#include <asm/devtree.h>
#include <asm/processor.h>
#include <asm/cpu.h>
#include <asm/ocm_size.h>

struct procnode {
	struct devtree_node dn;
	unsigned int threads;
	unsigned int timers;
	unsigned int frequency;
	unsigned int ddr_frequency;
	unsigned int interrupt0;
	unsigned int interrupt1;
	void *socm;
	void *eocm;
	void *sdram;
	void *edram;
	unsigned int arch_version;
	void *os_syscall_begin;
	void *os_syscall_end;
};

struct procnode *pn;

static char *procnode_feature_state[] = {"none", "disabled", "enabled"};

/*
 * show_processorinfo()
 *	Print the actual processor information.
 */
static void show_processorinfo(struct seq_file *m)
{
	char *cpu;
	unsigned int clockfreq, ddrfreq;
	unsigned int mmu = 0, fpu = 0;

	u32 pll_setting = *(u32_t *)(GENERAL_CFG_BASE + GEN_CLK_CORE_CFG);
	u32_t nr = GEN_GET_CLK_PLL_NR(pll_setting);
	u32_t nf = GEN_GET_CLK_PLL_NF(pll_setting);
	u32_t od = GEN_GET_CLK_PLL_OD(pll_setting);
	clockfreq = ((OSC1_FREQ / nr) * nf) / od;
	if (pn) {
		pn->frequency = clockfreq;
	}

	pll_setting = *(u32_t *)(GENERAL_CFG_BASE + GEN_CLK_DDR_CFG);
	nr = GEN_GET_CLK_PLL_NR(pll_setting);
	nf = GEN_GET_CLK_PLL_NF(pll_setting);
	//od = GEN_GET_CLK_PLL_OD(pll_setting);
	ddrfreq = ((OSC1_FREQ / nr) * nf) / DDR_PLL_OD_VALUE;
	if (pn) {
		pn->ddr_frequency = ddrfreq;
	}

#if (UBICOM32_ARCH_VERSION >= 5)
#if !defined(CONFIG_MMU)
	mmu = 1;
#else
	mmu = 2;
#endif
#if !defined(CONFIG_FPU)
	fpu = 1;
#else
	fpu = 2;
#endif
#endif // (UBICOM32_ARCH_VERSION >= 5)

	/*
	 * General Processor Information.
	 */
	cpu = CPU;
	seq_printf(m, "Vendor\t\t: %s\n", "Ubicom");
	seq_printf(m, "CPU\t\t: %s\n", cpu);
	seq_printf(m, "MMU\t\t: %s\n", procnode_feature_state[mmu]);
	seq_printf(m, "FPU\t\t: %s\n", procnode_feature_state[fpu]);
	seq_printf(m, "Arch\t\t: %hx\n", ubicom32_get_chip_id() >> 16);
	seq_printf(m, "Rev\t\t: %hx\n", (ubicom32_get_chip_id() & 0xffff));

	/*
	 * Now compute the clock frequency in Mhz.
	 */
	seq_printf(m, "Clock Freq\t: %u.0 MHz\n", clockfreq / 1000000);
	seq_printf(m, "DDR Freq\t: %u.0 MHz\n", ddrfreq / 1000000);
	seq_printf(m, "BogoMips\t: %lu.%02lu\n",
		   (loops_per_jiffy * HZ) / 500000,
		   ((loops_per_jiffy * HZ) / 5000) % 100);
	seq_printf(m, "Calibration\t: %lu loops\n", (loops_per_jiffy * HZ));

	/*
	 * We add this repeat of the CPU for Android.
	 */
	seq_printf(m, "Hardware\t: Ubicom%s\n", cpu);
}

/*
 * show_cpuinfo()
 *	Get CPU information for use by the procfs.
 */
static int show_cpuinfo(struct seq_file *m, void *v)
{
	unsigned long n = (unsigned long)v - 1;

#if defined(CONFIG_SMP)
	struct cpuinfo_ubicom32 *p = &per_cpu(cpu_data, n);
#endif

	/*
	 * Print the general processor information on the first
	 * call.
	 */
	if (n == 0) {
		show_processorinfo(m);
	}

#if defined(CONFIG_SMP)
	/*
	 * For each hwthread, print if this hwthread is running Linux
	 * or is an I/O thread.
	 */
	if (cpu_isset(n, cpu_online_map)) {
		seq_printf(m, "cpu[%02lu]\t\t: thread id - %lu\n", n, p->tid);
	} else {
		seq_printf(m, "cpu[%02lu]\t\t: off-line\n", n);
	}
#endif
	return 0;

}

static void *c_start(struct seq_file *m, loff_t *pos)
{
	unsigned long i = *pos;

	return i < NR_CPUS ? (void *)(i + 1) : NULL;
}

static void *c_next(struct seq_file *m, void *v, loff_t *pos)
{
	++*pos;
	return c_start(m, pos);
}

static void c_stop(struct seq_file *m, void *v)
{
}

const struct seq_operations cpuinfo_op = {
	.start	= c_start,
	.next	= c_next,
	.stop	= c_stop,
	.show	= show_cpuinfo,
};

/*
 * processor_timers()
 *	Returns the timers available to Linux.
 */
unsigned int processor_timers(void)
{
	if (!pn) {
		return 0;
	}
	return pn->timers;
}

/*
 * processor_threads()
 *	Returns the threads available to Linux.
 */
unsigned int processor_threads(void)
{
	if (!pn) {
		return 0;
	}
	return pn->threads;
}

/*
 * processor_frequency()
 *	Returns the frequency of the system clock.
 */
unsigned int processor_frequency(void)
{
	if (!pn) {
		return 0;
	}
	return pn->frequency;
}
EXPORT_SYMBOL(processor_frequency);

/*
 * processor_ddr_frequency()
 *	Returns the frequency of the system DDR clock.
 */
unsigned int processor_ddr_frequency(void)
{
	if (!pn) {
		return 0;
	}
	return pn->ddr_frequency;
}
EXPORT_SYMBOL(processor_ddr_frequency);

/*
 * processor_interrupts()
 *	Return the interrupts that are setup at boot time.
 */
int processor_interrupts(unsigned int *int0, unsigned int *int1)
{
	if (!pn) {
		return -EFAULT;
	}

	if (int0) {
		*int0 = pn->interrupt0;
	}

	if (int1) {
		*int1 = pn->interrupt1;
	}
	return 0;
}

/*
 * processor_ocm()
 *	Returns the start and end of OCM available to Linux.
 */
void processor_ocm(unsigned long *socm, unsigned long *eocm)
{
	*socm = (unsigned long)pn->socm;
	*eocm = (unsigned long)pn->eocm;
}

/*
 * processor_dram()
 *	Returns the start and end of dram available to Linux.
 */
void processor_dram(unsigned long *sdram, unsigned long *edram)
{
	if (pn != NULL) {
		*sdram = (unsigned long)pn->sdram;
		*edram = (unsigned long)pn->edram;
	} else {
		*sdram = 0;
		*edram = 0;
	}
}

/*
 * processor_validate_failed()
 *	Returns the dram available to Linux.
 */
static noinline void processor_validate_failed(void)
{
	while (1)
		THREAD_STALL;
}

/*
 * processor_validate()
 *	Validates the procnode against limitations of this link/built.
 */
static void processor_validate(void)
{
	void *dram_start = (void *)(KERNELSTART);
	void *dram_end   = (void *)(SDRAMSTART + CONFIG_MIN_RAMSIZE);
#if APP_OCM_CODE_SIZE || APP_OCM_DATA_SIZE
	void *ocm_code_start = (void *)(OCMSTART + APP_OCM_CODE_SIZE);
	void *ocm_data_end   = (void *)(OCMEND   - APP_OCM_DATA_SIZE);
#endif
	extern void __os_syscall_begin;
	extern void __os_syscall_end;
	int proc_node_valid = 1;

	if (!pn) {
		printk(KERN_ERR "ERROR: processor node not found\n");
		goto error;
	}


	if (dram_start < pn->sdram || dram_end > pn->edram) {
		printk(KERN_ERR "ERROR: processor dram mismatch %p-%p "
		       "available but we are expecting %p-%p\n",
		       pn->sdram, pn->edram, dram_start, dram_end);
		proc_node_valid = 0;
	} else {
		printk(KERN_ERR "processor dram %p-%p, expecting %p-%p\n",
		       pn->sdram, pn->edram, dram_start, dram_end);
	}
	if (&__os_syscall_begin < pn->os_syscall_begin ||
	    &__os_syscall_end > pn->os_syscall_end) {
		printk(KERN_ERR "ERROR: processor syscall area mismatch "
		       "%p-%p available but we are expecting %p-%p\n",
		       pn->os_syscall_begin, pn->os_syscall_end,
		       &__os_syscall_begin, &__os_syscall_end);
		proc_node_valid = 0;
	} else {
		printk(KERN_ERR "processor dram %p-%p, expecting %p-%p\n",
		       pn->sdram, pn->edram, dram_start, dram_end);
	}
#if APP_OCM_CODE_SIZE || APP_OCM_DATA_SIZE
	if (ocm_code_start < pn->socm ||  ocm_data_end > pn->eocm) {
		printk(KERN_ERR "ERROR: processor ocm mismatch %p-%p "
		       "available but we are expecting %p-%p\n",
		       pn->socm, pn->eocm, ocm_code_start, ocm_data_end);
		proc_node_valid = 0;
	} else {
		printk(KERN_INFO "processor ocm %p-%p, expecting %p-%p\n",
		       pn->socm, pn->eocm, ocm_code_start, ocm_data_end);

	}
#endif

	if (UBICOM32_ARCH_VERSION != pn->arch_version) {
		printk(KERN_ERR "ERROR: processor arch mismatch, kernel"
		       "compiled for %d found %d\n",
		       UBICOM32_ARCH_VERSION, pn->arch_version);
		proc_node_valid = 0;
	}

	if (proc_node_valid)
		return;
error:
	processor_validate_failed();
}

#if (UBICOM32_ARCH_VERSION <= 4)
/*
 * ubicom32_range_init()
 *	Preliminary set up of range registers for Ultra. The main OS will overwrite these settings.
 */
static void ubicom32_range_init(void)
{
	extern unsigned long __syscall_text_run_begin, __syscall_text_run_end;
	extern unsigned long _edata, _data_protection_end;

	/*
	 * Set the spare I range 2 and D range 2 to cover entire memory space 
	 * for execution and data access. We will shut things down later.
	 */
	UBICOM32_SET_RANGE_REGISTER("I", "2", 0x0, 0xffffffff, ALL_THREADS);
	UBICOM32_SET_RANGE_REGISTER("D", "2", 0x0, 0xffffffff, ALL_THREADS);

	/*
	 * Set I and D range 0 to cover from OCMSTART to end of DDR.
	 * This allows Ultra and Kernel to access all of OCM and DDR space.
	 * Access to this range register set is disabled for all Linux threads
	 * when they transition over to user space.
	 */
	UBICOM32_SET_RANGE_REGISTER("I", "0", OCMSTART, (pn->edram - 4), ALL_THREADS);
	UBICOM32_SET_RANGE_REGISTER("D", "0", OCMSTART, (pn->edram - 4), ALL_THREADS);

	/*
	 * Set I range 1 to cover from _edata to end of DDR.
	 * This range is the Linux User Code space.
	 */
	UBICOM32_SET_RANGE_REGISTER("I", "1", ((unsigned long) &_edata), (pn->edram - 4), ALL_THREADS);

	/*
	 * Set D range 1 to cover from _data_protection_end to end of DDR.
	 * This range is the Linux User data space.
	 */
	UBICOM32_SET_RANGE_REGISTER("D", "1", ((unsigned long) &_data_protection_end), (pn->edram - 4), ALL_THREADS);

	/*
	 * Set I range 3 to cover syscall entry/exit and signal trampoline etc.
	 */
	UBICOM32_SET_RANGE_REGISTER("I", "3", &__syscall_text_run_begin, &__syscall_text_run_end, ALL_THREADS);

	/*
	 * Set D range 3 to cover from HRT_TABLE0_BASE to RLATCH (end of IO block).
	 * Access to this range is disabled when Linux threads transitions to User space.
	 */
	UBICOM32_SET_RANGE_REGISTER("D", "3", HRT_TABLE0_BASE, (IO_PORT_RLATCH - 4), ALL_THREADS);

	/*
	 * Shut down all spare range registers I2, D2 and D4.
	 */
	UBICOM32_SET_RANGE_REGISTER("I", "2", 0x0, 0xffffffff, NO_THREADS);
	UBICOM32_SET_RANGE_REGISTER("D", "2", 0x0, 0xffffffff, NO_THREADS);

#if (UBICOM32_ARCH_VERSION == 4)
	UBICOM32_SET_RANGE_REGISTER("D", "4", 0x0, 0xffffffff, NO_THREADS);
#endif
}
#else
/*
 * ubicom32_range_init()
 *	Preliminary set up of range registers for Ultra. The main OS will overwrite these settings.
 */
static void ubicom32_range_init(void)
{
	/*
	 * Set the spare I range 3 and D range 4 to cover entire memory space 
	 * for execution and data access. We will shut things down later.
	 */
	UBICOM32_SET_RANGE_REGISTER("I", "3", 0x0, 0xffffffff, ALL_THREADS, NO_THREADS);
	UBICOM32_SET_RANGE_REGISTER("D", "4", 0x0, 0xffffffff, ALL_THREADS, NO_THREADS);

	/*
	 * Set I and D range 0 to cover from OCMSTART to end of DDR.
	 * Code and data access via this range is only avaialble to Ultra
	 * and Linux threads in Kernel space.
	 */
	UBICOM32_SET_RANGE_REGISTER("I", "0", OCMSTART, (pn->edram - 4), ALL_THREADS, NO_THREADS);
	UBICOM32_SET_RANGE_REGISTER("D", "0", OCMSTART, (pn->edram - 4), ALL_THREADS, NO_THREADS);

	/*
	 * Set I and D range 1 to cover the mapped space from 0 to UBICOM32_FDPIC_STACK_END -4.
	 * Linux threads can access mapped space though this range in User mode as well
	 * as kernel mode.
	 */
	UBICOM32_SET_RANGE_REGISTER("I", "1", 0, (UBICOM32_FDPIC_STACK_END -4), ALL_THREADS, ALL_THREADS);
	UBICOM32_SET_RANGE_REGISTER("D", "1", 0, (UBICOM32_FDPIC_STACK_END -4), ALL_THREADS, ALL_THREADS);

	/*
	 * Set I and D range 2 to cover UBICOM32_FDPIC_KERNEL_START to UBICOM32_FDPIC_KERNEL_END.
	 * This is the VMALLOC zone and is only available for Linux threads in kernel space only.
	 */
	UBICOM32_SET_RANGE_REGISTER("I", "2", UBICOM32_FDPIC_KERNEL_START,  (UBICOM32_FDPIC_KERNEL_END - 4),  ALL_THREADS, NO_THREADS);
	UBICOM32_SET_RANGE_REGISTER("D", "2", UBICOM32_FDPIC_KERNEL_START,  (UBICOM32_FDPIC_KERNEL_END - 4),  ALL_THREADS, NO_THREADS);

	/*
	 * Set D range 3 to cover from HRT_TABLE0_BASE to RLATCH (end of IO block) for kernel only
	 */
	UBICOM32_SET_RANGE_REGISTER("D", "3", HRT_TABLE0_BASE, (IO_PORT_RLATCH - 4), ALL_THREADS, NO_THREADS);

	/*
	 * Shut down spare ranges. D4, D5 and I3
	 */
	UBICOM32_SET_RANGE_REGISTER("D", "4", 0x0, 0xffffffff, NO_THREADS, NO_THREADS);
	UBICOM32_SET_RANGE_REGISTER("D", "5", 0x0, 0xffffffff, NO_THREADS, NO_THREADS);
	UBICOM32_SET_RANGE_REGISTER("I", "3", 0x0, 0xffffffff, NO_THREADS, NO_THREADS);
}
#endif

void __init processor_init(void)
{
	/*
	 * If we do not have a trap node in the device tree, we leave the fault
	 * handling to the underlying hardware.
	 */
	pn = (struct procnode *)devtree_find_node("processor");

	processor_validate();

	ubicom32_range_init();

	/*
	 * The watchdog may be started when kernel is initialized.
	 * Stop it and let application to decide for runtime.
	 */
	ubicom32_write_reg(&UBICOM32_IO_TIMER->tkey, TIMER_TKEYVAL);
	ubicom32_write_reg(&UBICOM32_IO_TIMER->wdcfg, TIMER_WATCHDOG_DISABLE);
	ubicom32_write_reg(&UBICOM32_IO_TIMER->tkey, 0);
}
