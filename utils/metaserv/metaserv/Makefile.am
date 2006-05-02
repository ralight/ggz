bin_PROGRAMS = ggzmetaserv

ggzmetaserv_SOURCES = metaserv.c helper.c
ggzmetaserv_LDADD = \
	$(top_builddir)/metaserv/minidom/libminidom.a \
	$(top_builddir)/metaserv/libmeta/libggzmeta.a \
	-lpthread
ggzmetaserv_CPPFLAGS = \
	-I $(top_srcdir)/metaserv/minidom \
	-I $(top_srcdir)/metaserv/libmeta

noinst_HEADERS = helper.h

confdir = $(prefix)/share/metaserv
dist_conf_DATA = metaservconf.xml

EXTRA_DIST = README.metaserv

