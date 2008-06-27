lib_LTLIBRARIES = 

AM_CPPFLAGS = $(DATABASE_INCLUDES)

if GGZDB_MYSQL
libcanonicalstr_mysql_la_SOURCES = canonicalstr_proc_mysql.c canonicalstr.c canonicalstr.h
libcanonicalstr_mysql_la_CPPFLAGS = $(AM_CPPFLAGS)
libcanonicalstr_mysql_la_LDFLAGS = -module -avoid-version
lib_LTLIBRARIES += libcanonicalstr_mysql.la
endif
if GGZDB_PGSQL
libcanonicalstr_pgsql_la_SOURCES = canonicalstr_proc_pgsql.c canonicalstr.c canonicalstr.h
libcanonicalstr_pgsql_la_CPPFLAGS = $(AM_CPPFLAGS)
libcanonicalstr_pgsql_la_LDFLAGS = -module -avoid-version
lib_LTLIBRARIES += libcanonicalstr_pgsql.la
endif
