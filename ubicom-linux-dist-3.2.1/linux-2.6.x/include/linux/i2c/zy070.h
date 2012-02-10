/* Header file for:
 * Cypress ZY070 Prototype touchscreen driver.
 * include/linux/zy070.h
 *
 * Copyright (C) 2009, 2010 Cypress Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2, and only version 2, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Cypress reserves the right to make changes without further notice
 * to the materials described herein. Cypress does not assume any
 * liability arising out of the application described herein.
 *
 * Contact Cypress Semiconductor at www.cypress.com
 *
 * History:
 *			(C) 2010 Cypress - Update for GPL distribution
 *			(C) 2009 Cypress - Assume maintenance ownership
 *			(C) 2009 Enea - Original prototype
 *
 */


#ifndef __ZY070_H__
#define __ZY070_H__

#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/timer.h>
#include <linux/workqueue.h>

/* Register addresses */
/* Only registers used will defined at this time */
#define REG_STATUS (0x05)
#define REG_POS_XH (0x01)
#define REG_POS_XL (0x02)
#define REG_POS_YH (0x03)
#define REG_POS_YL (0x04)
#define REG_BTN    (0x05)
#define REG_ZOOM   (0x06)
#define REG_GEST   (0x09)
#define REG_SLEEP  (0x0A)
#define REG_SENS   (0x0B)
#define REG_FW_VER (0x0C)

/* Various defines (to be moved into zy070 header) */
#define REG_STAT_NO_FINGER (0x00)
#define REG_STAT_ONE_FINGER (0x01)
#define REG_STAT_TWO_FINGER (0x02)

#define ZY070_TEMPLATE 1
#define CONFIG_TOUCHSCREEN_ZY070_I2C 1
#define CONFIG_TOUCHSCREEN_ZY070_SPI 0
#define CONFIG_TOUCHSCREEN_ZY070_UART 0
#define	MAX_12BIT			((1<<12)-1)
#define	TOUCHSCREEN_TIMEOUT		(msecs_to_jiffies(50))
/* Various flags needed */
#define ZY070_SWAP_XY (0x01)
#define ZY070_FLIP_Y (0x02)
#define ZY070_FLIP_X (0x04)

#define GET_NUM_FINGERS(X) ((X) &0x0F)
#define IS_LARGE_AREA(X) (((X) & 0x10) >> 4)
#define FLIP_DATA(X) ((X) && 0x01)

/* ZY070 private data
 *
 * Still to be added:
 * Registers for Power management.
 */

struct zy070_platform_data {
  u32 maxx;
  u32 maxy;
  u32 flags;

  int     (*init_platform_hw)(void);
  void    (*exit_platform_hw)(void);
};

#endif 
