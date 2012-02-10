## Process this with automake to create Makefile.in

AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I $(srcdir)/../../../config

noinst_LTLIBRARIES = libubicom32.la

BFDINCLUDES =  -I../../../../bfd -I$(srcdir)/../../../../include
INCLUDES = -I$(srcdir) -I$(srcdir)/.. -I../../../include -I$(srcdir)/../../../include $(BFDINCLUDES)
CXXFLAGS = -g

pkgdata_DATA = hw-cpu-ubicom32.txt

libubicom32_la_SOURCES = ubicom32bf.cxx ubicom32bf-decode.cxx ubicom32bf-sem.cxx
libubicom32_la_LDFLAGS =
libubicom32_la_LIBADD = ubicom32-asm.lo ubicom32-dis.lo ubicom32-opc.lo ubicom32-ibld.lo ubicom32-desc.lo

OPCODES_COMPILE_FLAGS = -DHAVE_CONFIG_H \
	-I../../../../opcodes \
	-I../../../../bfd \
	-I../../../../intl \
	-I$(srcdir)/../../../../opcodes \
	-I$(srcdir)/../../../../include \
	-I$(srcdir)/../../../../bfd     \
	-I$(srcdir)/../../../../intl    \
	-D_GNU_SOURCE 

ubicom32-asm.lo: ../../../../opcodes/ubicom32-asm.c
	/bin/sh ./libtool --mode=compile gcc $(OPCODES_COMPILE_FLAGS) $(CFLAGS) -c $<

ubicom32-dis.lo: ../../../../opcodes/ubicom32-dis.c
	/bin/sh ./libtool --mode=compile gcc $(OPCODES_COMPILE_FLAGS) $(CFLAGS) -c $<

ubicom32-opc.lo: ../../../../opcodes/ubicom32-opc.c
	/bin/sh ./libtool --mode=compile gcc $(OPCODES_COMPILE_FLAGS) $(CFLAGS) -c $<

ubicom32-ibld.lo: ../../../../opcodes/ubicom32-ibld.c
	/bin/sh ./libtool --mode=compile gcc $(OPCODES_COMPILE_FLAGS) $(CFLAGS) -c $<

ubicom32-desc.lo: ../../../../opcodes/ubicom32-desc.c
	/bin/sh ./libtool --mode=compile gcc $(OPCODES_COMPILE_FLAGS) $(CFLAGS) -c $<
 
cgendir = $(srcdir)/../../../../cgen
CPUDIR = $(cgendir)/cpu
CGENFLAGS = -v

CGEN_ALL = cgen-ubicom32
.PHONY: cgen-all $(CGEN_ALL)
cgen-all: $(CGEN_ALL)

cgen-ubicom32:
	$(SHELL) ../CGEN.sh cpu-decode $(srcdir) $(cgendir) "$(CGENFLAGS)" \
	ubicom32 "with-scache" ubicom32bf ubicom32 "ip3035,ubicom32dsp,ubicom32_ver4" ubicom32 "ubicom32bf-" \
	$(cgendir)/cpu/ubicom32.cpu \
	"desc.h cpu.h defs.h decode.h decode.cxx semantics.cxx"

cgen-ubicom32mars:
	$(SHELL) ../CGEN.sh cpu-decode $(srcdir) $(cgendir) "$(CGENFLAGS)" \
	ubicom32 "with-scache" ubicom32mars ubicom32 ubicom32dsp ubicom32 "ubicom32mars-" \
	ubicom32 \
	"desc.h cpu.h defs.h decode.h decode.cxx semantics.cxx"

CGENDEPS = \
	$(cgendir)/sim-arch.scm  $(cgendir)/sim-decode.scm \
	$(cgendir)/sim-test.scm  $(cgendir)/sim-cpu.scm \
	$(cgendir)/sim-model.scm $(cgendir)/sim.scm

if CGEN_MAINT
UBICOM32_DEPS = stamp-ubicom32
else
UBICOM32_DEPS =
endif

$(srcdir)/ubicom32bf-desc.h $(srcdir)/ubicom32bf-cpu.h $(srcdir)/ubicom32bf-defs.h $(srcdir)/ubicom32bf-decode.h $(srcdir)/ubicom32bf-decode.cxx $(srcdir)/ubicom32bf-semantics.cxx: $(UBICOM32_DEPS)
	@true

stamp-ubicom32: $(CGENDEPS) $(CPUDIR)/ubicom32.cpu $(CPUDIR)/ubicom32.opc
	$(MAKE) cgen-ubicom32
	touch stamp-ubicom32



html_stylesheet=$(srcdir)/../../component_html.xsl
include $(srcdir)/../../../config/Makefile.docs
