ifeq ($(CONFIG_EXTERNAL_TOOLCHAIN),)

else

  define Package/libgcc/install
	for file in $(call qstrip,$(CONFIG_LIBGCC_FILE_SPEC)); do \
		dir=/lib ; \
		$(INSTALL_DIR) $(1)/$$$$dir ; \
		$(CP) $(call qstrip,$(CONFIG_LIBGCC_ROOT_DIR))/$$$$file $(1)/$$$$dir/ ; \
	done
  endef

  define Package/libstdcpp/install
	for file in $(call qstrip,$(CONFIG_LIBSTDCPP_FILE_SPEC)); do \
		dir=/lib ; \
		$(INSTALL_DIR) $(1)/$$$$dir ; \
		$(CP) $(call qstrip,$(CONFIG_LIBSTDCPP_ROOT_DIR))/$$$$file $(1)/$$$$dir/ ; \
	done
  endef

endif

