## Process this with automake to create Makefile.in

AUTOMAKE_OPTIONS = foreign

LIBIBERTY = -L../../../libiberty/pic -L../../../libiberty -liberty

pkglib_LTLIBRARIES = libloader.la libVerilogLoader.la

pkgdata_DATA = sw-load-elf.txt

INCLUDES = -I. -I../../include -I$(srcdir)/../../include -I$(srcdir)/../../../include

libloader_la_SOURCES = compLoader.cxx elfload.c
libloader_la_LDFLAGS = -module -no-undefined
libloader_la_LIBADD = $(LIBIBERTY)

libVerilogLoader_la_SOURCES = VerilogLoader.cxx
libVerilogLoader_la_LDFLAGS = -module -no-undefined
libVerilogLoader_la_LIBADD = $(LIBIBERTY)

html_stylesheet=$(srcdir)/../component_html.xsl
include $(srcdir)/../../config/Makefile.docs
