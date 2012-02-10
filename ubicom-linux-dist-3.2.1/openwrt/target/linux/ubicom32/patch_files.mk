define PatchExternalFiles
	$(INSTALL_DIR) $(1)/$(2)/
	if [ -f $(PLATFORM_SUBDIR)/base-files/$(2)/$(3) ]; then \
		patch $(PLATFORM_SUBDIR)/base-files/$(2)/$(3) -o $(TOPDIR)/tmp/$(3) < ./files/$(3).patch ; \
	else \
		patch $(TOPDIR)/package/base-files/files/$(2)/$(3) -o $(TOPDIR)/tmp/$(3) < ./files/$(3).patch ; \
	fi
	$(INSTALL_BIN) $(TOPDIR)/tmp/$(3) $(1)/$(2)/$(3)
endef

