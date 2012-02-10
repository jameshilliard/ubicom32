/*
 * arch/ubicom32/include/asm/adv7180.h
 *   Common initialization routines for adv7180
 *
 * The init function should be called during late_init.
 *
 * This setup is for NTSC CVBS only
 *
 * (C) Copyright 2010, Ubicom, Inc.
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

#ifndef ADV7180_H
#define ADV7180_H
extern int __init adv7180_init(int i2c_adapt, int i2c_addr, int reset_gpio, int input);
#endif // ADV7180_H

