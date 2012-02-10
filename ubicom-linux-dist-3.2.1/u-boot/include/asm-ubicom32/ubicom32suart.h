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
 */

#ifndef _ASM_UBICOM32_UBICOM32_SUART_H
#define _ASM_UBICOM32_UBICOM32_SUART_H

/*
 * Platform resource id for serdes uart clock parameter
 */
#define UBICOM32_SUART_IORESOURCE_CLOCK		(1)

void ubi32_mailbox_putc(char);
int ubi32_mailbox_getc(void);
int ubi32_mailbox_tstc(void);

#endif /* _ASM_UBICOM32_UBICOM32_SUART_H */
