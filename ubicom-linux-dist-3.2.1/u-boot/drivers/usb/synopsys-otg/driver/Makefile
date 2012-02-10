#
# (C) Copyright 2000-2007
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#
include $(TOPDIR)/config.mk

#
# Reuse LM_INTERFACE to add Ubicom platform driver
#
BUS_INTERFACE	:=
PCI_INTERFACE	:= 0

#  
# Select the mode of the driver to optimize out unnecessary code
#
ifdef CONFIG_DWC_MODE_HOST_ONLY
CPPFLAGS	+= -DDWC_HOST_ONLY
endif

ifdef CONFIG_DWC_MODE_DEVICE_ONLY
CPPFLAGS        += -DDWC_DEVICE_ONLY
endif

#
# Debug mode
#
ifdef CONFIG_DWC_DEBUG
CPPFLAGS	+= -DDEBUG
endif

# Use one of the following flags to compile the software in host-only or
# device-only mode.
CPPFLAGS 	+= -Dlinux -DDWC_HS_ELECT_TST
CPPFLAGS 	+= -DDWC_EN_ISOC
CPPFLAGS        += -I$(TOPDIR)/drivers/usb/synopsys-otg/dwc_common_port 
CPPFLAGS   	+= -DDWC_LINUX
CPPFLAGS	+= $(BUS_INTERFACE)

CFLAGS		+= $(CPPFLAGS)

LIB	:= $(obj)libusb_synopsis_otg.a

COBJS-y += usb_dwc.o dwc_otg_cil.o dwc_otg_hcd_queue.o dwc_otg_hcd.o dwc_otg_cil_intr.o dwc_otg_hcd_intr.o dwc_otg_driver.o dwc_otg_hcd_uboot.o

COBJS	:= $(COBJS-y)
SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

all:	$(LIB)

$(LIB):	$(obj).depend $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
