SUBDIRS = .

bin_PROGRAMS = metaserv

metaserv_SOURCES = metaserv.c helper.c
metaserv_LDADD = $(top_builddir)/metaserv/minidom/libminidom.a -lpthread

noinst_HEADERS = helper.h

INCLUDES = -I $(top_srcdir)/metaserv/minidom

confdir = $(prefix)/share/metaserv
dist_conf_DATA = metaservconf.xml

EXTRA_DIST = README.metaserv

