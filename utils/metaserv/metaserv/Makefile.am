bin_PROGRAMS = ggzmetaserv

ggzmetaserv_SOURCES = metaserv.c helper.c
ggzmetaserv_LDADD = \
	$(top_builddir)/metaserv/libmeta/libggzmeta.a \
	-lpthread
ggzmetaserv_CPPFLAGS = -I $(top_srcdir)/metaserv/libmeta \
	-DPREFIX=\"$(prefix)\"

noinst_HEADERS = helper.h

confdir = $(prefix)/share/ggzmetaserv
dist_conf_DATA = metaservconf.xml

EXTRA_DIST = README.metaserv

