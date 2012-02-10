#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/MediaRouterUSB30
  NAME:=MediaRouterUSB30
  PACKAGES:= samba3 luci-app-samba minidlna_binary kmod-Ubicom_WISH wish
endef

define Profile/MediaRouterUSB30/Description
	Ubicom MediaRouter Profile with Media Support and USB 3.0
endef
$(eval $(call Profile,MediaRouterUSB30))

