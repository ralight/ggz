if COND_DBPROC
DBPROC_DIR = proc
endif

SUBDIRS = $(DBPROC_DIR)

noinst_LIBRARIES = libggzdb.a
libggzdb_a_CPPFLAGS = -I$(top_srcdir)/ggzd $(LIBGGZ_INCLUDES) -DGGZDEXECMODDIR=\"$(ggzdexecmoddir)\"
libggzdb_a_SOURCES = \
	ggzdb.c \
	ggzdb.h \
	ggzdb_proto.h

ggzdbdir = $(libdir)/ggzd/database
ggzdb_LTLIBRARIES =

AM_CPPFLAGS = $(DATABASE_INCLUDES) -I$(top_srcdir)/ggzd $(LIBGGZ_INCLUDES)

if GGZDB_SQLITE
libggzdb_sqlite_la_SOURCES = ggzdb_sqlite.c
libggzdb_sqlite_la_LDFLAGS = -module -avoid-version
ggzdb_LTLIBRARIES += libggzdb_sqlite.la
endif
if GGZDB_MYSQL
libggzdb_mysql_la_SOURCES = ggzdb_mysql.c rankings.c rankings_mysql.c rankings.h ggzdb_util.c ggzdb_util.h
libggzdb_mysql_la_CPPFLAGS = $(AM_CPPFLAGS) -DGGZDCONFDIR=\"$(ggzdconfdir)\"
libggzdb_mysql_la_LDFLAGS = -module -avoid-version
ggzdb_LTLIBRARIES += libggzdb_mysql.la
endif
if GGZDB_PGSQL
libggzdb_pgsql_la_SOURCES = ggzdb_pgsql.c rankings.c rankings_mysql.c rankings.h ggzdb_util.c ggzdb_util.h
libggzdb_pgsql_la_CPPFLAGS = $(AM_CPPFLAGS) -DGGZDCONFDIR=\"$(ggzdconfdir)\"
libggzdb_pgsql_la_LDFLAGS = -module -avoid-version
ggzdb_LTLIBRARIES += libggzdb_pgsql.la
endif
if GGZDB_DB4
libggzdb_db4_la_SOURCES = ggzdb_db4.c
libggzdb_db4_la_LDFLAGS = -module -avoid-version
ggzdb_LTLIBRARIES += libggzdb_db4.la
endif
if GGZDB_DBI
libggzdb_dbi_la_SOURCES = ggzdb_dbi.c
libggzdb_dbi_la_LDFLAGS = -module -avoid-version
ggzdb_LTLIBRARIES += libggzdb_dbi.la
endif

if GGZDB_PGSQL
noinst_PROGRAMS = calcrankings
calcrankings_CPPFLAGS = $(LIBGGZ_INCLUDES) \
	-DGGZDCONFDIR=\"$(ggzdconfdir)\"
calcrankings_SOURCES = calcrankings.c rankings.c rankings_pgsql.c rankings.h
calcrankings_LDFLAGS = $(LIBGGZ_LDFLAGS)
calcrankings_LDADD = $(LIB_GGZ) $(LIB_DATABASE)
endif

dist_ggzddata_DATA = ggz.sql mysql_schema.sql

