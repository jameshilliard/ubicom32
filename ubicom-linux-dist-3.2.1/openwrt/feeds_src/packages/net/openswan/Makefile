#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=openswan
PKG_VERSION:=2.6.29
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.openswan.org/download
PKG_MD5SUM:=cd6c127585ec6db63780e9a40d5387fb

PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/openswan/Default
  TITLE:=Openswan
  URL:=http://www.openswan.org/
  DEPENDS:=@!LINUX_2_4
endef

define Package/openswan/Default/description
 Openswan is an IPsec implementation for Linux.
endef

define Package/openswan
$(call Package/openswan/Default)
  SUBMENU:=VPN
  SECTION:=net
  CATEGORY:=Network
  TITLE+= (daemon)
  DEPENDS+= +libgmp +ip
endef

define Package/openswan/description
$(call Package/openswan/Default/description)
 This package contains the Openswan user-land daemon.
endef

define KernelPackage/openswan
$(call Package/openswan/Default)
  SUBMENU:=Network Support
  TITLE+= (kernel module)
  FILES:=$(PKG_BUILD_DIR)/modobj*/ipsec.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/openswan/description
$(call Package/openswan/Default/description)
 This package contains the Openswan kernel module.
endef

TARGET_CPPFLAGS += -I$(LINUX_DIR)/include -D__EXPORTED_HEADERS__

OPENSWAN_MAKEFLAGS:= \
	-C $(PKG_BUILD_DIR) \
	$(TARGET_CONFIGURE_OPTS) \
	LINUX_RELEASE="$(LINUX_RELEASE)" \
	KERNELSRC="$(LINUX_DIR)" \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	USERCOMPILE="$(TARGET_CFLAGS) -I$(PKG_BUILD_DIR)/ports/linux/include -I$(PKG_BUILD_DIR)/linux/include -I$(PKG_BUILD_DIR)/include $(TARGET_CPPFLAGS) $(TARGET_LDFLAGS)" \
	IPSECDIR="/usr/lib/ipsec" \
	INC_USRLOCAL="/usr" \
	INC_RCDEFAULT="/etc/init.d" \
	MODPROBE="/sbin/insmod" \
	USE_XAUTH="true" \
	LDFLAGS="$(TARGET_LDFLAGS)" \
	DESTDIR="$(PKG_INSTALL_DIR)"

KMOD_OPENSWAN_MAKEFLAGS := \
	-C $(PKG_BUILD_DIR) \
	LINUX_RELEASE="$(LINUX_RELEASE)" \
	KERNELSRC="$(LINUX_DIR)" \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(KERNEL_CROSS)"

define Build/Compile
	$(MAKE) $(PKG_JOBS) $(OPENSWAN_MAKEFLAGS) programs
	$(MAKE) $(PKG_JOBS) $(KMOD_OPENSWAN_MAKEFLAGS) module
endef

define Build/Install
	$(MAKE) $(OPENSWAN_MAKEFLAGS) install
endef

define Package/openswan/install
	$(CP) $(PKG_INSTALL_DIR)/* $(1)
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/etc/config
	$(CP) ./files/ipsec.init $(1)/etc/init.d/ipsec
	$(CP) ./files/ipsec.conf $(1)/etc/ipsec.conf
	$(CP) ./files/ipsec.secrets $(1)/etc/ipsec.secrets
	$(CP) ./files/passwd $(1)/etc/ipsec.d/passwd
	$(CP) ./files/vpn_users $(1)/etc/config/vpn_users
	$(CP) ./files/ipsec.nf_add_rules $(1)/etc/ipsec.nf_add_rules
	$(CP) ./files/ipsec.nf_rm_rules $(1)/etc/ipsec.nf_rm_rules
	rm -rf $(1)/usr/share
	rm -rf $(1)/usr/man
	rm -rf $(1)/var
	rm -rf $(1)/etc/rc.d
	find $(1) -name \*.old | xargs rm -rf
endef

$(eval $(call BuildPackage,openswan))
$(eval $(call KernelPackage,openswan))
