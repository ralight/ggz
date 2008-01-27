bin_PROGRAMS = ggzmetaserv

ggzmetaserv_SOURCES = metaserv.c helper.c
ggzmetaserv_LDADD = \
	$(top_builddir)/metaserv/libmeta/libggzmeta.a \
	$(LIB_GGZ) \
	-lpthread
ggzmetaserv_CPPFLAGS = \
	-I $(top_srcdir)/metaserv/libmeta \
	$(LIBGGZ_INCLUDES) \
	-DPREFIX=\"$(prefix)\"
ggzmetaserv_LDFLAGS = $(LIBGGZ_LDFLAGS)

noinst_HEADERS = helper.h

confdir = $(prefix)/share/ggzmetaserv
dist_conf_DATA = metaservconf.xml

EXTRA_DIST = README.metaserv

