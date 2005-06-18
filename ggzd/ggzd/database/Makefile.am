SUBDIRS = .

lib_LIBRARIES = libggzdb.a
libggzdb_a_SOURCES = \
	ggzdb.c \
	ggzdb.h \
	ggzdb_proto.h

if GGZDB_SQLITE
libggzdb_a_SOURCES += ggzdb_sqlite.c
endif
if GGZDB_MYSQL
libggzdb_a_SOURCES += ggzdb_mysql.c
endif
if GGZDB_PGSQL
libggzdb_a_SOURCES += ggzdb_pgsql.c
endif
if GGZDB_DB2
libggzdb_a_SOURCES += ggzdb_db2.c
endif
if GGZDB_DB3
libggzdb_a_SOURCES += ggzdb_db3.c
endif
if GGZDB_DB4
libggzdb_a_SOURCES += ggzdb_db4.c
endif

AM_CPPFLAGS = -I$(top_srcdir)/ggzd $(LIBGGZ_INCLUDES)

