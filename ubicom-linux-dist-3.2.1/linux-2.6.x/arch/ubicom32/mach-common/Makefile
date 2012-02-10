#
# arch/ubicom32/mach-common/Makefile
#	Makefile for Ubicom32 generic drivers/code.
#
# (C) Copyright 2009, Ubicom, Inc.
#
# This file is part of the Ubicom32 Linux Kernel Port.
#
# The Ubicom32 Linux Kernel Port is free software: you can redistribute
# it and/or modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation, either version 2 of the
# License, or (at your option) any later version.
#
# The Ubicom32 Linux Kernel Port is distributed in the hope that it
# will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
# the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with the Ubicom32 Linux Kernel Port.  If not,
# see <http://www.gnu.org/licenses/>.
#
# Ubicom32 implementation derived from (with many thanks):
#   arch/m68knommu
#   arch/blackfin
#   arch/parisc
#

obj-y += cachectl.o common.o board.o bootargs.o profile.o profile_vma.o

ifeq ($(CONFIG_USB_SUPPORT), y)
  obj-$(CONFIG_UBICOM32_V4) += usb_tio.o usb.o
  obj-$(CONFIG_UBI32_DWC_OTG) += usb_dwc.o lm.o
  obj-$(CONFIG_UBI32_XHCI) += usb_xhci.o usb_xhci_phy.o
endif

ifdef CONFIG_UBICOM32_V3
obj-$(CONFIG_PCI) += pci.o
endif

ifdef CONFIG_UBICOM32_V4
obj-$(CONFIG_PCI) += pci.o
endif

ifdef CONFIG_UBICOM32_V5
obj-$(CONFIG_PCI) += pcie.o pcie_io.o
obj-$(CONFIG_PCI_MSI) += pcie_msi.o
ifdef CONFIG_PCI_CLOCK_MASTER
obj-$(CONFIG_PCI) += pcie_phy.o
else
obj-$(CONFIG_PCI) += pcie_phy_extref.o
endif
endif

obj-$(CONFIG_PCI) += io.o pcibios.o
obj-$(CONFIG_UBICOM32_STOPWATCH_PCI) += pci_measurement.o
obj-$(CONFIG_GPIOLIB) += ubi32-gpio.o

ifdef CONFIG_MTD_UBI32_FC
FLASH_HELPER = y
endif
ifdef CONFIG_MTD_UBI32_NAND_SPI_ER
FLASH_HELPER = y
endif
obj-$(FLASH_HELPER) += flash.o

obj-$(CONFIG_FB_UBICOM32) += vdc_tio.o
obj-$(CONFIG_UBICOM_HID) += ubicom32hid.o
obj-$(CONFIG_UBICOM_INPUT) += ubicom32input.o
obj-$(CONFIG_UBICOM_INPUT_I2C) += ubicom32input_i2c.o
obj-$(CONFIG_UBICOM_SWITCH) += switch-core.o
obj-$(CONFIG_UBICOM_SWITCH_BCM539X) += switch-bcm539x.o
obj-$(CONFIG_UIO_UBICOM32RING) += ring_tio.o
obj-$(CONFIG_SND_UBI32) += audio.o
obj-$(CONFIG_UBICOM32_PLIO) += plio.o
obj-$(CONFIG_UBICOM32_LCD_ADAPTER) += lcd_adapter.o adv7393.o tfp410.o
obj-$(CONFIG_IP7500AV) += adv7393.o
obj-$(CONFIG_SRS_WOWHD_PROC) += srs_wowhd.o
obj-$(CONFIG_UBICOM32_ADV7180) += adv7180.o
obj-$(CONFIG_UBICOM32_VIDEOPASSTHROUGH) += videopassthrough.o
obj-$(CONFIG_UBICOM32_AUDIOPASSTHROUGH) += audiopassthrough.o
