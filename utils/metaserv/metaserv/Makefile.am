SUBDIRS = .

bin_PROGRAMS = metaserv

metaserv_SOURCES = metaserv.c helper.c
metaserv_LDADD = $(top_builddir)/metaserv/minidom/libminidom.a

noinst_HEADERS = helper.h

INCLUDES = -I $(top_srcdir)/metaserv/minidom

confdir = $(prefix)/share/metaserv
conf_DATA = metaservconf.xml

EXTRA_DIST = README.metaserv

