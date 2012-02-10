#
# makefile to biuld ubicom toolchain's
#
# this file is not part of gcc
#

DIR=$(shell pwd)
TOOLS_INSTALL_DIR ?= $(DIR)/install
BUILD := $(DIR)/build

all:
	mkdir -p $(BUILD); \
	cd $(BUILD); \
	$(DIR)/build_all $(TOOLS_INSTALL_DIR)

clean:
	echo Removing  $(BUILD)
	-rm -rf $(BUILD)

realclean: clean
	echo Removing  $(TOOLS_INSTALL_DIR)
	-rm -rf $(TOOLS_INSTALL_DIR)
