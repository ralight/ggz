SUBDIRS = security

if INSTALL_LIBGGZ
lib_LTLIBRARIES = libggz.la
include_HEADERS = ggz.h ggz_common.h ggz_dio.h
else
noinst_LTLIBRARIES = libggz.la
endif

libggz_la_LIBADD = security/libggzsecurity.la $(LIB_ASYNC) $(LIB_PTHREADS) $(LIB_GC)
libggz_la_CPPFLAGS = $(GC_INCLUDES)

libggz_la_LDFLAGS = -version-info 5:0:3
if SYMBOL_VISIBILITY
libggz_la_LDFLAGS += -Wl,--version-script=$(srcdir)/linker.map
libggz_la_DEPENDENCIES = $(srcdir)/linker.map
endif

libggz_la_SOURCES = \
	conf.c \
	ggz_dio.c	\
	easysock.c \
	ggz.h \
	list.c \
	memory.c \
	misc.c misc.h \
	msg.c \
	numberlist.c \
	stack.c \
	support.h \
	uri.c \
	xmlelement.c \
	\
	ggz_common.h \
	ggz_common.c

EXTRA_DIST = Doxyfile linker.map
