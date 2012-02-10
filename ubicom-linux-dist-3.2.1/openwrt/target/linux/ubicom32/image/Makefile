# 
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

# use default targets for everything

ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
  ifeq ($(CONFIG_TARGET_ROOTFS_SQUASHFS),y)

    define Image/mkfs/squashfs

		@echo "Symlinking /init to /etc/preinit"
		@(cd $(TARGET_DIR); \
		  ln -sf etc/preinit init; \
		 )

		@mkdir -p $(TARGET_DIR)/overlay
		$(STAGING_DIR_HOST)/bin/mksquashfs4 $(TARGET_DIR) $(KDIR)/root.squashfs -nopad -noappend -root-owned -comp lzma -processors 1
		$(call Image/Build,squashfs)
    endef

  endif
endif

# The next line will be necessary in Image/Build/squashfs if we support splitting flash
# $(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
define Image/Build/squashfs
	$(CP) $(KDIR)/root.squashfs $(BIN_DIR)/root.squashfs
endef

define Image/Build
	$(call Image/Build/$(1))
endef

$(eval $(call BuildImage))
