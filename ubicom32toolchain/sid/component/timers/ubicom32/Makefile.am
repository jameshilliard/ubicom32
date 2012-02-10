## Process this with automake to create Makefile.in

AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I $(srcdir)/../../../config

noinst_LTLIBRARIES = libubicom32timers.la

INCLUDES = -I. -I../../../include -I$(srcdir)/../../../include
libubicom32timers_la_SOURCES = ubicom32.cxx
libubicom32timers_la_LDFLAGS =

pkgdata_DATA = hw-timer-ubicom32_ref.txt


html_stylesheet=$(srcdir)/../../component_html.xsl
include $(srcdir)/../../../config/Makefile.docs
