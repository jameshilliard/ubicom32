
###################################################
# Makefile for the Synopsys GMAC driver
###################################################

#KERNELDIR ?= /lib/modules/$(shell uname -r)/build
#PWD := $(shell pwd)

#all:
#	gmake -C $(KERNELDIR) CONFIG_SYNOPGMACHOST_PCI=m  M=$(PWD)
####################################################
# Different CONFIG_XXXXXXXXXXXXXX
# CONFIG_SYNOPGMACHOST_PCI    => for pci driver
####################################################
#clean:
#	rm -rf *.o
#	rm -rf *.ko
#	rm -rf *.mod.*
#	rm -rf .synop*
#	rm -rf .built*
#	rm -rf .tmp*

#obj-$(CONFIG_UBICOM32_GMAC) += ubi32_eth.o
obj-$(CONFIG_UBICOM32_GMAC_DMA) += synop_GMAC_ether.o
synop_GMAC_ether-objs = synopGMAC_Host.o synopGMAC_network_interface.o synopGMAC_Dev.o

####################################################
# Explanation for EXTRA_FLAGS
# -DDEBUG		     => Enable the debug trace for driver
# -DIPC_OFFLOAD		     => Enables the IP and TCP checksum offloading feature in HW (IPV4 only)
# -DENH_DESC		     => Enables Enhanced Descriptors
# -DENH_DESC_8W              => Enable Enhanced Descriptors of 8words or else Descriptor will be of 4words.
# -DENH_DESC_TIMESTAMP       => Enable timestamp (need ENH_DESC_8W to be defined)
####################################################

EXTRA_CFLAGS += -DENH_DESC -DENH_DESC_8W -DIPC_OFFLOAD
