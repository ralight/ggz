SUBDIRS = .

DBALL = ggzdb_db2.c ggzdb_db3.c ggzdb_db4.c ggzdb_pgsql.c ggzdb_mysql.c
DBSOURCES = ggzdb_@database@.c

EXTRA_DIST = $(DBALL)

lib_LIBRARIES = libggzdb.a
libggzdb_a_SOURCES = \
	$(DBSOURCES) \
	ggzdb.c \
	ggzdb.h \
	ggzdb_proto.h

INCLUDES = -I$(top_srcdir)/ggzd $(LIBGGZ_INCLUDES)

libggzdb_a_LDFLAGS = $(LIBGGZ_LDFLAGS)
#libggzdb_a_LIBADD = $(LDADD) $(ggzd_LIBS) $(LIB_GGZ)

