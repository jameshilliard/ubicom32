/*
 * max4550.c
 *      Support for Maxim MAX4550:
 *              Dual 4x2, Clickless Audio/Video Crosspoint Switch
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

/*
 * If this structure is not provided then the GPIO will not be added
 * to the system.  This data is not used past init time.
 */
struct max4550_platform_data {
	/*
	 * number of the first GPIO
	 */
	unsigned	gpio_base;

	/*
	 * Default value to set outputs to
	 */
	uint8_t		gpio_default;
};
