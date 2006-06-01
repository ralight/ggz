EXTRA_DIST = README.minidom metasrv.xml example.xml

noinst_PROGRAMS = minidom

minidom_SOURCES = main.c
minidom_LDADD = $(top_builddir)/metaserv/libmeta/libggzmeta.a
minidom_CPPFLAGS = -I $(top_srcdir)/metaserv/libmeta

