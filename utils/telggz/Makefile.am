## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 6258 2004-11-04 23:00:59Z jdorje $

SUBDIRS = .

bin_PROGRAMS = telggz

telggz_SOURCES = chat.c chat.h \
		 main.c \
		 meta.c meta.h \
		 net.c net.h

telggz_LDFLAGS = $(GGZCORE_LDFLAGS)
telggz_LDADD = $(LDADD) \
               $(LIB_GGZCORE) \
               $(top_builddir)/metaserv/minidom/libminidom.a

AM_CPPFLAGS = -I$(top_srcdir)/metaserv/minidom $(GGZCORE_INCLUDES)
