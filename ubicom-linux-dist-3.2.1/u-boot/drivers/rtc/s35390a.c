/*
 * (C) Copyright 2010
 * Ubicom, Inc.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Date & Time support for Seiko S35390A RTC
 */

#ifdef DEBUG
#define S35390A_DEBUG(x...) x
#else
#define S35390A_DEBUG(...)
#endif

#include <common.h>
#include <command.h>
#include <rtc.h>
#include <i2c.h>

#if defined(CONFIG_CMD_DATE)

#define S35390A_BASE	0x30
static int hr24 = -1;

static uchar bitrev8(uchar n)
{
	uchar out = 0;
	int i;
	for (i = 0; i < 8; i++) {
		if (n & (1 << i)) {
			out |= (1 << (7 - i));
		}
	}
	return out;
}
static unsigned bcd2bin (uchar n)
{
	return ((((n >> 4) & 0x0F) * 10) + (n & 0x0F));
}

static unsigned char bin2bcd (unsigned int n)
{
	return (((n / 10) << 4) | (n % 10));
}

static void rtc_read_hr24(void)
{
	uchar data;
	i2c_read(S35390A_BASE + 0x00, 0, 0, &data, 1);
	data = bitrev8(data);
	S35390A_DEBUG("stat1 is %02x\n", data);

	if (data & (1 << 1)) {
		hr24 = 1;
		return;
	}

	hr24 = 0;
}

/* ------------------------------------------------------------------------- */

int rtc_get (struct rtc_time *tmp)
{
	uchar data[7];
	int i;

	/*
	 * First time find out what mode we're in
	 */
	if (hr24 == -1) {
		rtc_read_hr24();
	}

	i2c_read(S35390A_BASE + 0x02, 0, 0, (uchar *)&data, 7);
	for (i = 0; i < 7; i++) {
		data[i] = bitrev8(data[i]);
	}
	S35390A_DEBUG( "Get RTC year: %02x mon: %02x mday: %02x wday: %02x "
		"hr: %02x min: %02x sec: %02x\n",
		data[0], data[1], data[2], data[3],
		data[4], data[5], data[6]);

	tmp->tm_sec  = bcd2bin(data[6] & 0x7F);
	tmp->tm_min  = bcd2bin(data[5] & 0x7F);
	tmp->tm_hour = bcd2bin(data[4] & 0x3F);
	tmp->tm_mday = bcd2bin(data[2] & 0x3F);
	tmp->tm_mon  = bcd2bin(data[1] & 0x1F);
	tmp->tm_year = bcd2bin(data[0]) + 2000;
	tmp->tm_wday = bcd2bin(data[3] & 0x07);
	tmp->tm_yday = 0;
	tmp->tm_isdst= 0;

	S35390A_DEBUG( "Get DATE: %4d-%02d-%02d (wday=%d)  TIME: %2d:%02d:%02d\n",
		tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,
		tmp->tm_hour, tmp->tm_min, tmp->tm_sec);

	return 0;
}

int rtc_set (struct rtc_time *tmp)
{
	uchar data[7];
	int i;
	
	/*
	 * First time find out what mode we're in
	 */
	if (hr24 == -1) {
		rtc_read_hr24();
	}

	S35390A_DEBUG( "Set DATE: %4d-%02d-%02d (wday=%d)  TIME: %2d:%02d:%02d\n",
		tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,
		tmp->tm_hour, tmp->tm_min, tmp->tm_sec);

	data[0] = bin2bcd(tmp->tm_year - 2000);
	data[1] = bin2bcd(tmp->tm_mon);
	data[2] = bin2bcd(tmp->tm_mday);
	data[3] = bin2bcd(tmp->tm_wday);
	data[4] = bin2bcd(tmp->tm_hour);
	if (!hr24 && (tmp->tm_hour >= 12)) {
		data[4] = 0x40 | bin2bcd(tmp->tm_hour - 12);
	}
	data[5] = bin2bcd(tmp->tm_min);
	data[6] = bin2bcd(tmp->tm_sec);

	for (i = 0; i < 7; i++) {
		data[i] = bitrev8(data[i]);
	}

	return i2c_write(S35390A_BASE + 0x02, 0, 0, (uchar *)&data, 7);
}

int rtc_set_calib (int calib)
{
	uchar data = bitrev8(calib);

	return i2c_write(S35390A_BASE + 0x06, 0, 0, &data, 1);
}

int rtc_get_calib (void)
{
	uchar data;

	i2c_read(S35390A_BASE + 0x06, 0, 0, &data, 1);
	return bitrev8(data);
}

void rtc_reset (void)
{
	uchar data;
	S35390A_DEBUG("RESET RTC\n");

	/*
	 * RESET + 24 hour
	 */
	data = 3;

	i2c_write(S35390A_BASE + 0x00, 0, 0, &data, 1);
}

#endif
