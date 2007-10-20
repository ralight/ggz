if COND_DBPROC
DBPROC_DIR = proc
endif

SUBDIRS = $(DBPROC_DIR)

noinst_LIBRARIES = libggzdb.a
libggzdb_a_SOURCES = \
	ggzdb.c \
	ggzdb.h \
	ggzdb_proto.h

libggzdb_a_CPPFLAGS = -I$(top_srcdir)/ggzd $(LIBGGZ_INCLUDES) $(DATABASE_INCLUDES)

if GGZDB_SQLITE
libggzdb_a_SOURCES += ggzdb_sqlite.c
endif
if GGZDB_MYSQL
libggzdb_a_SOURCES += ggzdb_mysql.c rankings.c rankings.h
libggzdb_a_CPPFLAGS += -DGGZDCONFDIR=\"$(ggzdconfdir)\"
endif
if GGZDB_PGSQL
libggzdb_a_SOURCES += ggzdb_pgsql.c rankings.c rankings.h
libggzdb_a_CPPFLAGS += -DGGZDCONFDIR=\"$(ggzdconfdir)\"
endif
if GGZDB_DB4
libggzdb_a_SOURCES += ggzdb_db4.c
endif
if GGZDB_DBI
libggzdb_a_SOURCES += ggzdb_dbi.c
endif

if GGZDB_PGSQL
noinst_PROGRAMS = calcrankings
calcrankings_CPPFLAGS = $(LIBGGZ_INCLUDES) \
	-DGGZDCONFDIR=\"$(ggzdconfdir)\"
calcrankings_SOURCES = calcrankings.c rankings.c rankings.h
calcrankings_LDFLAGS = $(LIBGGZ_LDFLAGS)
calcrankings_LDADD = $(LIB_GGZ) $(LIB_DATABASE)
endif

dist_ggzddata_DATA = ggz.sql

