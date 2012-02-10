/*
 * arch/ubicom32/include/asm/ubicom32jog.h
 *   Ubicom32 architecture Jog dial driver platform data definitions.
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
 */
#ifndef _ASM_UBICOM32_JOG_H
#define _ASM_UBICOM32_JOG_H


struct ubicom32jog_platform_data {
	/*
	 * Input event code (KEY_*, SW_*, etc) (defaults to EV_KEY/KEY_MUTE)
	 */
	int		button_types;
	int		button_type;
	int		button_code;

	/*
	 * GPIO number of the optional button its polarity.
	 * The interval defaults to 50ms if not specified.
	 */
	int		has_button;
	unsigned	button_gpio;
	int		button_active_low;
	int		button_poll_interval;

	/*
	 * GPIO number of the jog dial, GPIO_A must be IRQ capable.
	 */
	unsigned	jog_gpio_a;
	unsigned	jog_gpio_b;
	
	/*
	 * Input abs/rel axis codes (ABS_VOLUME/REL_DIAL is default)
	 */
	int		jog_codes;
	int		jog_abs_code;
	int		jog_rel_code;

	/*
	 * Optional max and min limits (defaults to [INT_MIN, INT_MAX]
	 */
	int		jog_limits;
	int		jog_min;
	int		jog_max;

	/*
	 * debounce factor, the number of movements in a direction before
	 * changing direction, 0 to disable.
	 */
	int		jog_debounce;

	/*
	 * Set to 1 to reverse the direction of events relative to rotation
	 * of the dial.  Depends on the code output by the dial.
	 */
	int		jog_polarity;

	/*
	 * Optional name (defaults to "Ubicom32 Jog")
	 */
	const char	*name;
};

#endif /* _ASM_UBICOM32_JOG_H */
