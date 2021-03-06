#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=kissdx
PKG_VERSION:=0.14.0.b1a
PKG_RELEASE:=7

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).zip
PKG_SOURCE_URL:=http://kissdx.vidartysse.net
PKG_MD5SUM:=ef213d2748f5a63b1c97ac7bfa942b2f

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/kissdx
  SECTION:=multimedia
  CATEGORY:=Multimedia
  DEPENDS:=+libjpeg +libiconv +libdvdread
  TITLE:=PC-Link clone for KiSS media players
  URL:=http://kissdx.vidartysse.net
endef

define Package/kissdx/description
	kissdx is a PC-Link clone for KiSS media players, based for
	the most part on kissd (which it now replaces), with added
	features for media playback, management, flexibility and more.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CFLAGS="$(TARGET_CFLAGS) -DLinux -DUSE_INTERNAL_SENDFILE -std=gnu99 -Wall -Wstrict-prototypes -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -DFILE_OFFSET_BITS=64 -D_GNU_SOURCE -I$(STAGING_DIR)/usr/lib/libiconv/include -I$(STAGING_DIR)/usr/include/dvdread $(TARGET_CPPFLAGS) $(TARGET_LDFLAGS) -L$(STAGING_DIR)/usr/lib/libiconv/lib" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib/libiconv/lib $(TARGET_LDFLAGS)" \
		STRIP=$(STRIP) \
		CC="$(TARGET_CROSS)gcc" all
endef

define Package/kissdx/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/kissdx $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/kissdx.config $(1)/etc/config/kissdx
	$(INSTALL_BIN) ./files/kissdx.init $(1)/etc/init.d/kissdx
endef

$(eval $(call BuildPackage,kissdx))
