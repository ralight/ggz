## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3767 2002-04-06 07:50:08Z jdorje $

SUBDIRS = .

bin_PROGRAMS = telggz

telggz_SOURCES = chat.c chat.h \
		 main.c \
		 meta.c meta.h \
		 net.c net.h

telggz_LDADD = -L/usr/local/lib \
	       -lggzcore $(top_srcdir)/metaserv/minidom/libminidom.a

INCLUDES = -I/usr/local/include -I$(top_srcdir)/metaserv/minidom
