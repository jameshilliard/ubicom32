## Process this with automake to create Makefile.in

AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I $(srcdir)/../../../config

noinst_LTLIBRARIES = libsidip4k.la

BFDINCLUDES =  -I../../../../bfd -I$(srcdir)/../../../../include
INCLUDES = -I$(srcdir) -I$(srcdir)/.. -I$(srcdir)/../../../include -I../../../include $(BFDINCLUDES)
CXXFLAGS = -g

include_HEADERS = ip4ksid.h

libsidip4k_la_SOURCES = ip4kcpu.cxx ip4kcopro.cxx ip4kmainpluscopro.cxx ip4ksid.h
libsidip4k_la_LDFLAGS =

install-data-local: ip4ksidlib.txt
	$(mkinstalldirs) $(pkgdatadir)
	$(INSTALL_DATA) $(srcdir)/ip4ksidlib.txt $(DESTDIR)$(pkgdatadir)/ip4ksidlib.txt
