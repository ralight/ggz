## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 5817 2004-02-07 09:48:18Z josef $

SUBDIRS = security

lib_LTLIBRARIES = libggz.la

include_HEADERS = ggz.h ggz_common.h

libggz_la_LIBADD = @LDADD@ $(top_builddir)/src/security/libggzsecurity.la

libggz_la_LDFLAGS = -version-info 2:0:1

libggz_la_SOURCES =	conf.c \
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
