## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3769 2002-04-06 08:02:03Z jdorje $

SUBDIRS = .

bin_PROGRAMS = telggz

telggz_SOURCES = chat.c chat.h \
		 main.c \
		 meta.c meta.h \
		 net.c net.h

telggz_LDADD = $(LDADD) \
               $(LIB_GGZCORE) \
               $(top_builddir)/metaserv/minidom/libminidom.a

INCLUDES = -I/usr/local/include -I$(top_srcdir)/metaserv/minidom
