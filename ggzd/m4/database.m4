# =================================
# GGZ Gaming Zone - Database Macros
# =================================
#
# Checks for a suitable ggzd backend database
# and sets several variables appropriately.
#   $mysql => yes or no
#   $pgsql => yes or no
#   $sqlite => yes or no
#   $dbi => yes or no
#   $(LIB_MYSQL), $(LIB_PGSQL), $(LIB_SQLITE), $(LIB_DBI) => library linkages
#   $(DATABASE_INCLUDES) => headers for all database modules
# All configured database modules will be compiled
# and made available as dynamically-loadable backends (plugins).
# The list is also exported as DATABASE_TYPES definition (deprecated).
#
# ------------------------------------------------------------------------
# Content of this file:
# ------------------------------------------------------------------------
# AC_GGZ_DATABASE - find a suitable database library
#
# ------------------------------------------------------------------------
# Internal functions:
# ------------------------------------------------------------------------
# AC_GGZ_DATABASE_MYSQL - MySQL 3.x/4.x
# AC_GGZ_DATABASE_PGSQL - PostgreSQL 7.x/8.x
# AC_GGZ_DATABASE_SQLITE - SQLite embedded database
# AC_GGZ_DATABASE_DBI - DB-independent abstraction library

AC_DEFUN([AC_GGZ_DATABASE_PGSQL],
[
	AC_ARG_ENABLE([pgsql],
	              [AC_HELP_STRING([--enable-pgsql=yes/no/auto],[Force or disable pgsql support])],
		      [enable_pgsql=$enableval],[enable_pgsql=auto])
	if test "$enable_pgsql" = no; then
		AC_MSG_NOTICE([skipping pgsql check])
		pgsql=no
	else
		AC_CHECK_LIB(pq, PQconnectdb,
		[
			AC_CHECK_HEADER([postgresql/libpq-fe.h], [pgsql=yes],
			[
				AC_CHECK_HEADER([pgsql/libpq-fe.h],
				[
					pgsql=yes
					AC_DEFINE([PGSQL_IN_PGSQLDIR], 1, [Define if the pgsql headers are under pgsql/])
				],
				[
					if test "$enable_pgsql" = yes; then
						AC_MSG_ERROR([cannot configure pgsql (pgsql-dev headers needed)])
					fi
					pgsql=no
				])
			])
		],
		[
			if test "$enable_pgsql" = yes; then
				AC_MSG_ERROR([cannot configure pgsql (pgsql library needed)])
			fi
			pgsql=no
		])
	fi

	if test "$pgsql" = yes; then
		LIB_PGSQL="-lpq"
		AC_SUBST(LIB_PGSQL)
		AC_MSG_NOTICE([Configuring pgsql database module.])
		default_db=pgsql
		AC_DEFINE([WITH_PGSQL], 1, [PostgreSQL is used, needed for statistics])
	fi
	AM_CONDITIONAL([GGZDB_PGSQL], [test "$pgsql" = yes])
])

AC_DEFUN([AC_GGZ_DATABASE_MYSQL],
[
	AC_ARG_ENABLE([mysql],
	              [AC_HELP_STRING([--enable-mysql=yes/no/auto],[Force or disable mysql support])],
		      [enable_mysql=$enableval],[enable_mysql=auto])
	if test "$enable_mysql" = no; then
		AC_MSG_NOTICE([skipping mysql test])
		mysql=no
	else
		save_libs=$LIBS
		LIBS="$LIBS -L/usr/lib/mysql -lpthread"
		AC_CHECK_LIB(mysqlclient_r, mysql_real_connect,
		[
			AC_CHECK_HEADER(mysql/mysql.h, [mysql=yes],
			[
				if test "$enable_mysql" = yes; then
					AC_MSG_ERROR([cannot configure mysql (mysql-dev headers needed)])
				fi
				mysql=no
			])
		],
		[
			if test "$enable_mysql" = yes; then
				AC_MSG_ERROR([cannot configure mysql (mysql library needed)])
			fi
			mysql=no
		])
		LIBS=$save_libs
	fi

	if test "$mysql" = yes; then
		LIB_MYSQL="-L/usr/lib/mysql -lmysqlclient_r"
		AC_SUBST(LIB_MYSQL)
		AC_MSG_NOTICE([Configuring mysql database module.])
		default_db=mysql
		AC_DEFINE([WITH_MYSQL], 1, [MySQL is used, needed for statistics])
	fi
	AM_CONDITIONAL([GGZDB_MYSQL], [test "$mysql" = yes])
])

AC_DEFUN([AC_GGZ_DATABASE_SQLITE],
[
	AC_ARG_ENABLE([sqlite],
	              [AC_HELP_STRING([--enable-sqlite=yes/no/auto],[Force or disable sqlite support])],
		      [enable_sqlite=$enableval],[enable_sqlite=auto])
	if test "$enable_sqlite" = no; then
		AC_MSG_NOTICE([skipping sqlite test])
		sqlite=no
	else
		AC_CHECK_LIB(sqlite3, sqlite3_open,
		[
			AC_CHECK_HEADER(sqlite3.h, [sqlite=yes],
			[
				if test "$enable_sqlite" = yes; then
					AC_MSG_ERROR([cannot configure sqlite (sqlite3-dev headers needed)])
				fi
				sqlite=no
			])
		],
		[
			if test "$enable_sqlite" = yes; then
				AC_MSG_ERROR([cannot configure sqlite (sqlite3 library needed)])
			fi
			sqlite=no
		])
	fi

	if test "$sqlite" = yes; then
		LIB_SQLITE="-lsqlite3"
		AC_SUBST(LIB_SQLITE)
		AC_MSG_NOTICE([Configuring sqlite database module.])
		default_db=sqlite
	fi
	AM_CONDITIONAL([GGZDB_SQLITE], [test "$sqlite" = yes])

])

AC_DEFUN([AC_GGZ_DATABASE_DBI],
[
	AC_ARG_ENABLE([dbi],
	              [AC_HELP_STRING([--enable-dbi=yes/no/auto],[Force or disable dbi support])],
		      [enable_dbi=$enableval],[enable_dbi=auto])
	if test "$enable_dbi" = no; then
		AC_MSG_NOTICE([skipping dbi test])
		dbi=no
	else
		AC_CHECK_LIB(dbi, dbi_conn_new,
		[
			AC_CHECK_HEADER(dbi/dbi.h, [dbi=yes],
			[
				if test "$enable_dbi" = yes; then
					AC_MSG_ERROR([cannot configure dbi (dbi-dev headers needed)])
				fi
				dbi=no
			])
		],
		[
			if test "$enable_dbi" = yes; then
				AC_MSG_ERROR([cannot configure dbi (dbi library needed)])
			fi
			dbi=no
		])
	fi

	if test "$dbi" = yes; then
		LIB_DBI="-ldbi"
		AC_SUBST(LIB_DBI)
		AC_MSG_NOTICE([Configuring dbi database module.])
		default_db=dbi
	fi
	AM_CONDITIONAL([GGZDB_DBI], [test "$dbi" = yes])
])

AC_DEFUN([AC_GGZ_DATABASE],
[
	# Initialisation
	DATABASE_INCLUDES=""

	# Check if each module type is supported.  The *last* available
	# module in this list will be made the default.
	AC_GGZ_DATABASE_DBI
	AC_GGZ_DATABASE_MYSQL
	AC_GGZ_DATABASE_PGSQL
	AC_GGZ_DATABASE_SQLITE

	AC_SUBST(DATABASE_INCLUDES)

	# Make sure a database was configured
	if test "$default_db" = ""; then
		AC_MSG_ERROR([no usable database library found.  See above messages for more.])
	fi
	AC_DEFINE_UNQUOTED(DEFAULT_DB, "$default_db", [Default database module to use])
	AC_SUBST(default_db)
])
