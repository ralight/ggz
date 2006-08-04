SUBDIRS = security

lib_LTLIBRARIES = libggz.la

include_HEADERS = ggz.h ggz_common.h ggz_dio.h

libggz_la_LIBADD = $(LDADD) $(top_builddir)/src/security/libggzsecurity.la $(LIB_ANL)

libggz_la_LDFLAGS = -version-info 4:0:2

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
	xmlelement.c \
	\
	ggz_common.h \
	ggz_common.c

