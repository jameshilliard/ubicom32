#
#
# (C) Copyright 2009
# Ubicom, Inc. www.ubicom.com
#
# (C) Copyright 2002-2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

LIB	= $(obj)lib$(ARCH).a

COBJS-y	+= flash.o
COBJS-y	+= mem_flash.o
COBJS-y	+= board.o
COBJS-y	+= devtree.o
COBJS-y	+= bootm.o
COBJS-y	+= cache.o
COBJS-y	+= gpio.o
COBJS-$(CONFIG_UBICOM32_VIDEO) += ubicom32video.o
COBJS-$(CONFIG_CMD_USB) += usb.o
COBJS-$(CONFIG_CMD_USB_UPGRADE) += cmd_usb_upgrade.o
COBJS-$(CONFIG_EUPGRADE_VIDEO) += eupgrade_video.o
COBJS-$(CONFIG_EUPGRADE_LED) += eupgrade_led.o
COBJS-$(CONFIG_EUPGRADE_CHARLCD) += eupgrade_charlcd.o
COBJS-$(CONFIG_EUPGRADE) += eupgrade.o

COBJS-$(CONFIG_PNG) += pngreader.o
COBJS-$(CONFIG_PNG) += png/png.o png/pngset.o png/pngget.o png/pngrutil.o \
		       png/pngtrans.o png/pngread.o png/pngpread.o \
		       png/pngrio.o png/pngrtran.o png/pngmem.o png/pngerror.o

SRCS	:= $(SOBJS-y:.o=.S) $(COBJS-y:.o=.c)
OBJS	:= $(addprefix $(obj),$(SOBJS-y) $(COBJS-y))

$(LIB):	$(obj).depend $(OBJS) $(PNG_LOGOS-y)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
