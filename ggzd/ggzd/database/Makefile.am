SUBDIRS = schema

AM_CPPFLAGS = $(DATABASE_INCLUDES) $(LIBGGZ_INCLUDES) \
	-I$(top_srcdir)/ggzd -I$(top_srcdir)/ggzdmod \
	-DGGZDEXECMODDIR=\"$(ggzdexecmoddir)\" \
	-DGGZDCONFDIR=\"$(ggzdconfdir)\" \
	$(ICU_INCLUDES)

noinst_LIBRARIES = libggzdb.a
libggzdb_a_SOURCES = \
	ggzdb.c \
	ggzdb.h \
	ggzdb_functions.h \
	ggzdb_proto.h

ggzdbdir = $(libdir)/ggzd/database
ggzdb_LTLIBRARIES =

if GGZDB_SQLITE
libggzdb_sqlite_la_SOURCES = ggzdb_sqlite.c ggzdb_util.c ggzdb_util.h ggzdb_proto.h
libggzdb_sqlite_la_LDFLAGS = -module -avoid-version
libggzdb_sqlite_la_LIBADD = $(LIB_SQLITE)
ggzdb_LTLIBRARIES += libggzdb_sqlite.la
endif
if GGZDB_MYSQL
libggzdb_mysql_la_SOURCES = ggzdb_mysql.c rankings.c rankings_mysql.c rankings.h ggzdb_util.c ggzdb_util.h ggzdb_proto.h
libggzdb_mysql_la_LDFLAGS = -module -avoid-version
libggzdb_mysql_la_LIBADD = $(LIB_MYSQL)
ggzdb_LTLIBRARIES += libggzdb_mysql.la
endif
if GGZDB_PGSQL
libggzdb_pgsql_la_SOURCES = ggzdb_pgsql.c rankings.c rankings_pgsql.c rankings.h ggzdb_util.c ggzdb_util.h ggzdb_proto.h
libggzdb_pgsql_la_LDFLAGS = -module -avoid-version
libggzdb_pgsql_la_LIBADD = $(LIB_PGSQL)
ggzdb_LTLIBRARIES += libggzdb_pgsql.la
endif

if GGZDB_PGSQL
noinst_PROGRAMS = calcrankings
calcrankings_SOURCES = calcrankings.c
calcrankings_LDFLAGS = $(LIBGGZ_LDFLAGS)
calcrankings_LDADD = $(LIB_GGZ) $(LIB_ICU) libggzdb_pgsql.la
endif

