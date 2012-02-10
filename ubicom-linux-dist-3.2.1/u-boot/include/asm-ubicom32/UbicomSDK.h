/*
 * (C) Copyright 2009
 * Ubicom, Inc. www.ubicom.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __UBICOMSDK_H__
#define __UBICOMSDK_H__

/*
 * Boolean states.
 */
#define FALSE 0
#define TRUE 1

#define DEBUG_PRINTF(fmt, ...)	
#define DEBUG_PRINTF_LEVEL(level, file, line, fmt, ...)
#define DEBUG_VPRINTF_LEVEL(level, file, line, fmt, ap)
#define DEBUG_TRACE(fmt, ...)
#define DEBUG_INFO(fmt, ...)
#define DEBUG_WARN(fmt, ...)
#define DEBUG_ERROR(fmt, ...)
#define THIS_FILE(file)
#define DEBUG_ASSERT(cond, fmt, ...)
#define DEBUG_PRINT_MAC(mac)
#define DEBUG_PRINT_MAC_LE(mac)
#define DEBUG_PRINT_HEX_ARRAY(buf, len)
#define DEBUG_PRINT_VERSION(str, vers)
#define DEBUG_LOCK_ORDER (LOCK_ORDER(PKG_IPHAL, HAL_LOCK_ORDER_DEBUG))
#define	DEBUG_PRINT_CHIP_ID(str)




#endif//__UBICOMSDK_H__
