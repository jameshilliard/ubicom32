## Process this with automake to create Makefile.in

AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I $(srcdir)/../../../config

noinst_LTLIBRARIES = libsidubicom32.la

BFDINCLUDES =  -I../../../../bfd -I$(srcdir)/../../../../include
INCLUDES = -I$(srcdir) -I$(srcdir)/.. -I$(srcdir)/../../../include -I../../../include $(BFDINCLUDES)
CXXFLAGS = -g

include_HEADERS = ubicom32sid.h

libsidubicom32_la_SOURCES = ubicom32cpu.cxx ubicom32mainplustimer.cxx ubicom32sid.h
libsidubicom32_la_LDFLAGS =

install-data-local: ubicom32sidlib.txt
	$(mkinstalldirs) $(pkgdatadir)
	$(INSTALL_DATA) $(srcdir)/ubicom32sidlib.txt $(DESTDIR)$(pkgdatadir)/ubicom32sidlib.txt
