## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 4506 2002-09-11 03:25:20Z jdorje $

SUBDIRS = .

lib_LTLIBRARIES = libggz.la

include_HEADERS = ggz.h ggz_common.h

libggz_la_LIBADD = @LDADD@ @LTLIBOBJS@

libggz_la_LDFLAGS = -version-info 0:0:0

libggz_la_SOURCES =	conf.c \
			easysock.c \
			ggz.h \
		   	list.c \
		   	memory.c \
			misc.c misc.h \
			msg.c \
			numberlist.c \
		   	stack.c \
		   	xmlelement.c \
		   	\
		   	ggz_common.h \
		   	ggz_common.c
