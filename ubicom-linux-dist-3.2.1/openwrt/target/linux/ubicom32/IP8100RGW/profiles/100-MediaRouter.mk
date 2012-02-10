#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/MediaRouter
  NAME:=MediaRouter
  PACKAGES:= samba3 luci-app-samba minidlna_binary kmod-Ubicom_WISH wish radvd luci-app-radvd wide-dhcpv6-client wide-dhcpv6-server kmod-streamengine3 \
		ipv6tunnel luci-app-ipv6tunnel
endef

define Profile/MediaRouter/Description
	Ubicom MediaRouter Profile with Media Support
endef
$(eval $(call Profile,MediaRouter))

