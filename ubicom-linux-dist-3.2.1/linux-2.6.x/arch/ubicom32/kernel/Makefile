#
# arch/ubicom32/kernel/Makefile
#	Main Makefile for the Ubicom32 arch directory.
#
# (C) Copyright 2009, Ubicom, Inc.
#
# This file is part of the Ubicom32 Linux Kernel Port.
#
# The Ubicom32 Linux Kernel Port is free software: you can redistribute
# it and/or modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation, either version 2 of the
# License, or (at your option) any later version.
#
# The Ubicom32 Linux Kernel Port is distributed in the hope that it
# will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
# the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with the Ubicom32 Linux Kernel Port.  If not,
# see <http://www.gnu.org/licenses/>.
#
# Ubicom32 implementation derived from (with many thanks):
#   arch/m68knommu
#   arch/blackfin
#   arch/parisc
#
cflags-$(CONFIG_UBICOM32_V3)		:= -march=ubicom32v3 -DIP5000 -DUBICOM32_ARCH_VERSION=3
cflags-$(CONFIG_UBICOM32_V4)		:= -march=ubicom32v4 -DIP7000 -DUBICOM32_ARCH_VERSION=4
cflags-$(CONFIG_UBICOM32_V5)		:= -march=ubicom32v5 -DIP8000 -DUBICOM32_ARCH_VERSION=5
CPPFLAGS_vmlinux.lds := $(cflags-y)

extra-y := head.o vmlinux.lds

obj-y += \
	devtree.o \
	flat.o \
	init_task.o \
	irq.o \
	os_node.o \
	pic.o \
	process.o \
	processor.o \
	ptrace.o \
	reboot.o \
	setup.o \
	signal.o \
	stacktrace.o \
	switch_to.o \
	syscalltable.o \
	sys_ubicom32.o \
	thread.o \
	time.o \
	traps.o \
	ubicom32_ksyms.o \
	ubicom32_syscall.o

obj-$(CONFIG_MODULES)				+= module.o
obj-$(CONFIG_COMEMPCI)				+= comempci.o
obj-$(CONFIG_SMP)				+= smp.o topology.o
obj-$(CONFIG_ACCESS_OK_CHECKS_ENABLED)		+= uaccess.o
obj-$(CONFIG_GENERIC_CLOCKEVENTS)		+= timer_device.o
obj-$(CONFIG_GENERIC_CLOCKEVENTS_BROADCAST)	+= timer_broadcast.o
obj-$(CONFIG_UBICOM32_STOPWATCH)		+= stopwatch.o

obj-$(CONFIG_UBICOM32_V3)			+= entry_v34.o
obj-$(CONFIG_UBICOM32_V4)			+= entry_v34.o
obj-$(CONFIG_UBICOM32_V5)			+= entry_v5.o
obj-$(CONFIG_UBICOM32_V5)			+= lwatomics.o
obj-$(CONFIG_UBICOM32_V5)			+= lwsyscalltable.o

ifndef CONFIG_UNALIGNED_ACCESS_DISABLED
obj-y	+= emulate.o
obj-y	+= unaligned_trap.o
endif

ifndef CONFIG_GENERIC_CLOCKEVENTS
obj-y	+= timer_tick.o
endif
