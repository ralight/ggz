## Process this file with automake to produce Makefile.in
# $Id: Makefile.am 3882 2002-04-10 14:01:02Z dr_maux $

SUBDIRS = .

bin_PROGRAMS = telggz

telggz_SOURCES = chat.c chat.h \
		 main.c \
		 meta.c meta.h \
		 net.c net.h

telggz_LDADD = $(LDADD) \
               $(LIB_GGZCORE) \
               $(top_builddir)/metaserv/minidom/libminidom.a

INCLUDES = -I$(top_srcdir)/metaserv/minidom $(GGZCORE_INCLUDES)
