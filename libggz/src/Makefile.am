## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3113 2002-01-14 07:04:06Z jdorje $

SUBDIRS = .

lib_LTLIBRARIES = libggz.la

include_HEADERS = ggz.h ggz_common.h

libggz_la_LIBADD = @LDADD@

libggz_la_LDFLAGS = -version-info 0:0:0

libggz_la_SOURCES =	conf.c \
			ggz.h \
		   	list.c \
		   	memory.c \
			misc.c \
			msg.c msg.h \
		   	stack.c \
		   	xmlelement.c \
		   	\
		   	ggz_common.h \
		   	ggz_common.c
