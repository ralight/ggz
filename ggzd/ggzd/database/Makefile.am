if COND_DBPROC
DBPROC_DIR = proc
endif

SUBDIRS = $(DBPROC_DIR)

AM_CPPFLAGS = $(DATABASE_INCLUDES) $(LIBGGZ_INCLUDES) \
	-I$(top_srcdir)/ggzd -I$(top_srcdir)/ggzdmod \
	-DGGZDEXECMODDIR=\"$(ggzdexecmoddir)\" \
	-DGGZDCONFDIR=\"$(ggzdconfdir)\"

noinst_LIBRARIES = libggzdb.a
libggzdb_a_SOURCES = \
	ggzdb.c \
	ggzdb.h \
	ggzdb_functions.h \
	ggzdb_proto.h

EXTRA_DIST = proc/Makefile proc/canonicalstr_proc.c proc/README.proc

ggzdbdir = $(libdir)/ggzd/database
ggzdb_LTLIBRARIES =

if GGZDB_SQLITE
libggzdb_sqlite_la_SOURCES = ggzdb_sqlite.c ggzdb_proto.h
libggzdb_sqlite_la_LDFLAGS = -module -avoid-version
ggzdb_LTLIBRARIES += libggzdb_sqlite.la
endif
if GGZDB_MYSQL
libggzdb_mysql_la_SOURCES = ggzdb_mysql.c rankings.c rankings_mysql.c rankings.h ggzdb_util.c ggzdb_util.h ggzdb_proto.h
libggzdb_mysql_la_LDFLAGS = -module -avoid-version
ggzdb_LTLIBRARIES += libggzdb_mysql.la
endif
if GGZDB_PGSQL
libggzdb_pgsql_la_SOURCES = ggzdb_pgsql.c rankings.c rankings_pgsql.c rankings.h ggzdb_util.c ggzdb_util.h ggzdb_proto.h
libggzdb_pgsql_la_LDFLAGS = -module -avoid-version
ggzdb_LTLIBRARIES += libggzdb_pgsql.la
endif
if GGZDB_DB4
libggzdb_db4_la_SOURCES = ggzdb_db4.c ggzdb_proto.h
libggzdb_db4_la_LDFLAGS = -module -avoid-version
ggzdb_LTLIBRARIES += libggzdb_db4.la
endif
if GGZDB_DBI
libggzdb_dbi_la_SOURCES = ggzdb_dbi.c ggzdb_proto.h
libggzdb_dbi_la_LDFLAGS = -module -avoid-version
ggzdb_LTLIBRARIES += libggzdb_dbi.la
endif

if GGZDB_PGSQL
noinst_PROGRAMS = calcrankings
calcrankings_SOURCES = calcrankings.c
calcrankings_LDFLAGS = $(LIBGGZ_LDFLAGS)
calcrankings_LDADD = $(LIB_GGZ) $(LIB_DATABASE) libggzdb_pgsql.la
endif

dist_ggzddata_DATA = mysql_schema.sql pgsql_schema.sql upgrade_0.7_0.8.sql

