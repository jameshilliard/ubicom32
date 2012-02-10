#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=ubicom32
BOARD:=ubicom32
BOARDNAME:=Ubicom IPxx
FEATURES:=squashfs pci usb audio
SUBTARGETS=IP8100RGW

KERNEL_CROSS=$(ARCH)-elf-
KERNEL_CC=$(KERNEL_CROSS)gcc

LINUX_VERSION:=2.6.36
LINUX_CONFIG=$(PLATFORM_SUBDIR)/config-$(LINUX_VERSION).$(PROFILE)

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += \
	luci \
	kmod-ath_sag \
	kmod-synopsys-otg \
	block-hotplug \
	e2fsprogs \
	portmap \
	ntfs-3g \
	mount.ntfs-3g \
	profilerd \
	pptp \
	rp-l2tpd \
	kmod-hdlc \
	kmod-ppp-synctty \
	rp-pppoe-relay \
	ntpclient \
	luci-app-ntpc \
	ip \
	tc \
	genl \
	igmpproxy \
	lld2d \
	ddns-scripts \
	luci-app-ddns \
	miniupnpd \
	luci-app-upnp \
	wireless-tools \
	hostapd-ubicom32 \
	hostapd-utils-ubicom32 \
	kmod-ipt-nathelper-extra \
	btnd \
	quagga \
	quagga-ripd \
	ethtool

$(eval $(call BuildTarget))
