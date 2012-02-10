BOARDNAME:=IP8100RGW
CFLAGS:=-Os -pipe -march=ubicom32v5 -DUBICOM32_ARCH_VERSION=5 -DIP8000
LDFLAGS:=-march=ubicom32v5

define Target/Description
	Build firmware image for Ubicom IP8000RGW board
endef

