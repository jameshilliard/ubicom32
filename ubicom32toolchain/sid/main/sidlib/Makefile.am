## Process this with automake to create Makefile.in

## FIXME: Work around apparent bug in automake.
INTLLIBS = @INTLLIBS@
INTLLIBS_INSTALL = @INTLLIBS_INSTALL@
EXTRA_LTLIBRARIES = 

AUTOMAKE_OPTIONS = foreign
SUBDIRS = @cpu_subdirs@ testsuite .
ACLOCAL_AMFLAGS = -I $(srcdir)/../../config

INCLUDESBFD = -I../../../bfd -I$(srcdir)/../../../include
INCLUDES = -I. -I.. @cpu_incl@ -I../../include -I$(srcdir)/../../include $(INCLUDESBFD)

CXXFLAGS=
TOP_CXXFLAGS=$(CXXFLAGS)
AM_CXXFLAGS=$(TOP_CXXFLAGS)
AM_MAKEFLAGS= "TOP_CXXFLAGS=$(TOP_CXXFLAGS)"
MAKEOVERRIDES=

LIBIBERTY = -L../../../libiberty -liberty

## Must declare component dir variable then include list of component libs
component = ../../component
#include $(srcdir)/../../config/Makefile.complibs

# libsidcpu.la MUST be installed, otherwise the -rpath argument will not be passed to libtool which generates a broken libsidcpu.la
#noinst_LTLIBRARIES = libsidcpu.la
#pkglib_LTLIBRARIES = libsid.la $(INTLLIBS_INSTALL)
pkglib_LTLIBRARIES = libsidcpu.la libsid.la $(INTLLIBS_INSTALL)

libsidcpu_la_SOURCES = simwcpu.cxx cfgroot_interface.cxx
libsidcpu_la_LDFLAGS = -module -no-undefined
libsidcpu_la_LIBADD = @cpu_libs@ $(INTLLIBS) $(LIBIBERTY)
libsidcpu_la_DEPENDENCIES = @cpu_libs@

libsid_la_SOURCES = sidlib.cxx
libsid_la_DEPENDENCIES = $(COMP_LIBS) libsidcpu.la sidlib.cxx
libsid_la_LIBADD = $(top_builddir)/../../component/audio/libaudio.la \
                   $(top_builddir)/../../component/cache/libcache.la \
                   $(top_builddir)/../../component/cfgroot/libconfig.la \
                   $(top_builddir)/../../component/cgen-cpu/libcgencpu.la \
                   $(top_builddir)/../../component/consoles/libconsoles.la \
                   $(top_builddir)/../../component/gdb/libgdb.la \
                   $(top_builddir)/../../component/glue/libglue.la \
                   $(top_builddir)/../../component/gloss/libgloss.la \
                   $(top_builddir)/../../component/glue/libglue.la \
                   $(top_builddir)/../../component/ide/libide.la \
                   $(top_builddir)/../../component/interrupt/libinterrupt.la \
                   $(top_builddir)/../../component/lcd/libhd44780u.la \
                   $(top_builddir)/../../component/lcd/liblcd-char-display.la \
                   $(top_builddir)/../../component/lcd/libt6963c.la \
                   $(top_builddir)/../../component/loader/libloader.la \
                   $(top_builddir)/../../component/loader/libVerilogLoader.la \
                   $(top_builddir)/../../component/mailbox/libmailbox.la \
                   $(top_builddir)/../../component/mapper/libmapper.la \
                   $(top_builddir)/../../component/memory/libmemory.la \
                   $(top_builddir)/../../component/mmu/libmmu.la \
                   $(top_builddir)/../../component/mmu/libjupiterMMU.la \
                   $(top_builddir)/../../component/parport/libparport.la \
                   $(top_builddir)/../../component/profiling/libprof.la \
                   $(top_builddir)/../../component/rtc/librtc.la \
                   $(top_builddir)/../../component/sched/libsched.la \
                   $(top_builddir)/../../component/tcl/libtclapi.la \
                   $(top_builddir)/../../component/timers/libtimers.la \
                   $(top_builddir)/../../component/uart/libuart.la
#libsid_la_LDFLAGS = -export-dynamic -static -export-symbols-regex '[a-zA-Z0-9]*_library$$' $(DLPREOPEN_COMP_LIBS) -dlpreopen libsidcpu.la
libsid_la_LDFLAGS = -export-dynamic -static -export-symbols-regex '[a-zA-Z0-9]*_library$$' $(DLPREOPEN_COMP_LIBS) .libs/libsidcpu.a

if HAVE_EXTERNAL_INTLLIB
$(INTLLIBS_INSTALL): @INTLLIBS_EXTERNAL@
	cp @INTLLIBS_EXTERNAL@ $(INTLLIBS_INSTALL)

endif

# sidlibtool is no longer required since libsid.la and libsidcpu.la can build with the stock libtool
#install-exec-local: libtool
#	$(mkinstalldirs) $(DESTDIR)$(bindir)
#	$(INSTALL_PROGRAM) libtool $(DESTDIR)$(bindir)/sidlibtool
          
