## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 4939 2002-10-17 21:14:59Z jdorje $

SUBDIRS = .

lib_LTLIBRARIES = libggz.la

include_HEADERS = ggz.h ggz_common.h

libggz_la_LIBADD = @LDADD@

libggz_la_LDFLAGS = -version-info 1:0:0

libggz_la_SOURCES =	conf.c \
			easysock.c \
			ggz.h \
		   	list.c \
		   	memory.c \
			misc.c misc.h \
			msg.c \
			numberlist.c \
		   	stack.c \
			support.c \
		   	xmlelement.c \
		   	\
		   	ggz_common.h \
		   	ggz_common.c
