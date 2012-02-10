## Process this with automake to create Makefile.in

AUTOMAKE_OPTIONS = foreign
#SUBDIRS = . testsuite
SUBDIRS = .

pkglib_LTLIBRARIES = libmailbox.la

pkgdata_DATA = hw-mailbox.txt

INCLUDES = -I. -I../../include -I$(srcdir)/../../include
libmailbox_la_SOURCES = mbox.cxx
libmailbox_la_LDFLAGS = -module -no-undefined



html_stylesheet=$(srcdir)/../component_html.xsl
include $(srcdir)/../../config/Makefile.docs
