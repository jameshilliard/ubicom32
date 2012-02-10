/*
 * arch/ubicom32/include/asm/tfp410.h
 *   Common initialization routines for TFP410
 *
 * The init function should be called during late_init.
 *
 * (C) Copyright 2009-2010, Ubicom, Inc.
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
 */
#ifndef __UBICOM32_ASM_TFP410_H__
#define __UBICOM32_ASM_TFP410_H__

extern int __init tfp410_init(int i2c_adapt, int i2c_addr, int vdac_reset_gpio);

#endif
