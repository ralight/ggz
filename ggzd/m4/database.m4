dnl =================================
dnl GGZ Gaming Zone - Database Macros
dnl =================================
dnl
dnl Checks for a suitable ggzd backend database
dnl and sets $database as well as LIB_DATABASE appropriately
dnl
dnl ------------------------------------------------------------------------
dnl Content of this file:
dnl ------------------------------------------------------------------------
dnl AC_GGZ_DATABASE - find a suitable database library
dnl
dnl ------------------------------------------------------------------------
dnl Internal functions:
dnl ------------------------------------------------------------------------
dnl AC_GGZ_DATABASE_DB4 - Sleepycat/Oracle DB4.x
dnl AC_GGZ_DATABASE_MYSQL - MySQL 3.x/4.x
dnl AC_GGZ_DATABASE_PGSQL - PostgreSQL 7.x/8.x
dnl AC_GGZ_DATABASE_SQLITE - SQLite embedded database
dnl AC_GGZ_DATABASE_DBI - DB-independent abstraction library
dnl

AC_DEFUN([AC_GGZ_DATABASE_DB4],
[
	db4lib=""
	db4inc=""

	dnl Check for include files

	AC_CHECK_HEADER(db.h,
	[
		db4inc="db.h"
		database=db4
	],
	[
		AC_CHECK_HEADER(db4/db.h,
		[
			db4inc="db4/db.h"
			database=db4
			DATABASE_INCLUDES="-I /usr/include/db4"
		],
		[
			AC_CHECK_HEADER(db42/db.h,
			[
				db4inc="db42/db.h"
				database=db4
				DATABASE_INCLUDES="-I /usr/include/db42"
			],
			[])
		])
	])

	if test "$database" = "db4" && test "$db4inc" = ""; then
		AC_MSG_ERROR([cannot configure db4 (db4-dev needed)])
	fi

	dnl Get the minor version of db4 so we get a matching library

	minor=$(grep DB_VERSION_MINOR /usr/include/$db4inc 2>/dev/null | cut -f 3)

	dnl Check for db4 libraries
	dnl Version priority: db4.4, db4.3, db4.2, db4.1, db4.0, db (unversioned)

	if test "$db4lib" = "" || test "$minor" = "4"; then
		AC_CHECK_LIB(db-4.4, db_env_create_4004,
		[
			db4lib="-ldb-4.4"
			database=db4
		],
		[
			AC_CHECK_LIB(db-4.4, db_env_create,
			[
				db4lib="-ldb-4.4"
				database=db4
			],
			[
				AC_CHECK_LIB(db, db_env_create_4004,
				[
					db4lib="-ldb"
					database=db4
				],
				[])
			])
		])
	fi

	if test "$db4lib" = "" || test "$minor" = "3"; then
		AC_CHECK_LIB(db-4.3, db_env_create_4003,
		[
			db4lib="-ldb-4.3"
			database=db4
		],
		[
			AC_CHECK_LIB(db-4.3, db_env_create,
			[
				db4lib="-ldb-4.3"
				database=db4
			],
			[
				AC_CHECK_LIB(db, db_env_create_4003,
				[
					db4lib="-ldb"
					database=db4
				],
				[])
			])
		])
	fi

	if test "$db4lib" = "" || test "$minor" = "2"; then
		AC_CHECK_LIB(db-4.2, db_env_create_4002,
		[
			db4lib="-ldb-4.2"
			database=db4
		],
		[
			AC_CHECK_LIB(db-4.2, db_env_create,
			[
				db4lib="-ldb-4.2"
				database=db4
			],
			[
				AC_CHECK_LIB(db, db_env_create_4002,
				[
					db4lib="-ldb"
					database=db4
				],
				[])
			])
		])
	fi

	if test "$db4lib" = "" || test "$minor" = "1"; then
		AC_CHECK_LIB(db-4.1, db_env_create_4001,
		[
			db4lib="-ldb-4.1"
			database=db4
		],
		[
			AC_CHECK_LIB(db-4.1, db_env_create,
			[
				db4lib="-ldb-4.1"
				database=db4
			],
			[
				AC_CHECK_LIB(db, db_env_create_4001,
				[
					db4lib="-ldb"
					database=db4
				],
				[])
			])
		])
	fi

	if test "$db4lib" = "" || test "$minor" = "0"; then
		AC_CHECK_LIB(db-4.0, db_env_create_4000,
		[
			db4lib="-ldb-4.0"
			database=db4
		],
		[
			AC_CHECK_LIB(db-4.0, db_env_create,
			[
				db4lib="-ldb-4.0"
				database=db4
			],
			[
				AC_CHECK_LIB(db, db_env_create_4000,
				[
					db4lib="-ldb"
					database=db4
				],
				[])
			])
		])
	fi

	if test "$db4lib" = ""; then
		AC_CHECK_LIB(db, db_env_create,
		[
			db4lib="-ldb"
			database=db4
		],
		[])
	fi

	if test "$database" = "db4" && test "$db4lib" = ""; then
		AC_MSG_ERROR([cannot configure db4 (libdb4 needed)])
	fi

	dnl Setup variables

	if test "$database" = "db4"; then
		LIB_DATABASE="$db4lib"
	fi
])

AC_DEFUN([AC_GGZ_DATABASE_PGSQL],
[
	AC_CHECK_LIB(pq, PQconnectdb,
	[
		AC_CHECK_HEADER([postgresql/libpq-fe.h],
		[
			database=pgsql
			LIB_DATABASE="-lpq"
		],
		[
			AC_CHECK_HEADER([pgsql/libpq-fe.h],
			[
				database=pgsql
				LIB_DATABASE="-lpq"
				AC_DEFINE([PGSQL_IN_PGSQLDIR], 1, [Define if the pgsql headers are under pgsql/])
			],
			[
				if test "$database" = pgsql; then
					AC_MSG_ERROR([cannot configure pgsql (pgsql-dev headers needed)])
				fi
			])
		])
	],
	[
		if test "$database" = pgsql; then
			AC_MSG_ERROR([cannot configure pgsql (pgsql library needed)])
		fi
	])
])

AC_DEFUN([AC_GGZ_DATABASE_MYSQL],
[
	save_libs=$LIBS
	LIBS="$LIBS -L/usr/lib/mysql -lpthread"
	AC_CHECK_LIB(mysqlclient_r, mysql_real_connect,
	[
		AC_CHECK_HEADER(mysql/mysql.h,
		[
			database=mysql
			LIB_DATABASE="-L/usr/lib/mysql -lmysqlclient_r"
		],
		[
			if test "$database" = mysql; then
				AC_MSG_ERROR([cannot configure mysql (mysql-dev headers needed)])
			fi
		])
	],
	[
		if test "$database" = mysql; then
			AC_MSG_ERROR([cannot configure mysql (mysql library needed)])
		fi
	])
	LIBS=$save_libs
])

AC_DEFUN([AC_GGZ_DATABASE_SQLITE],
[
	AC_CHECK_LIB(sqlite3, sqlite3_open,
	[
		AC_CHECK_HEADER(sqlite3.h,
		[
			database=sqlite
			LIB_DATABASE="-lsqlite3"
		],
		[
			if test "$database" = sqlite; then
				AC_MSG_ERROR([cannot configure sqlite (sqlite3-dev headers needed)])
			fi
		])
	],
	[
		if test "$database" = sqlite; then
			AC_MSG_ERROR([cannot configure sqlite (sqlite3 library needed)])
		fi
	])
])

AC_DEFUN([AC_GGZ_DATABASE_DBI],
[
	AC_CHECK_LIB(dbi, dbi_conn_new,
	[
		AC_CHECK_HEADER(dbi/dbi.h,
		[
			database=dbi
			LIB_DATABASE="-ldbi"
		],
		[
			if test "$database" = dbi; then
				AC_MSG_ERROR([cannot configure dbi (dbi-dev headers needed)])
			fi
		])
	],
	[
		if test "$database" = dbi; then
			AC_MSG_ERROR([cannot configure dbi (dbi library needed)])
		fi
	])
])

AC_DEFUN([AC_GGZ_DATABASE],
[
case "$database" in
	db4)    database=db4 ;;
	pgsql)  database=pgsql ;;
	mysql)  database=mysql ;;
	sqlite) database=sqlite ;;
	dbi)    database=dbi ;;
	yes)    database=yes ;;
	no)     database=no ;;
	*)      database=invalid ;;
esac

dnl Some sanity checks first
if test "$database" = "invalid"; then
	AC_MSG_ERROR([invalid database type $database specified, see --help for a list])
fi

if test "$database" = "no"; then
	AC_MSG_ERROR([ggzd doesn't work without a database backend, see --help for a list])
fi

dnl Order of preference: db4, PgSQL, MySQL, SQlite, DBI.  This is determined
dnl solely by the order of the checks below.  Since a correct db will be
dnl auto-detected, we should order these solely based on how good they are
dnl (in terms of stability and performance).

dnl Check for db4 database
if test "$database" = db4 || test "$database" = yes; then
	AC_GGZ_DATABASE_DB4
fi

dnl Check for PgSQL database
if test "$database" = pgsql || test "$database" = yes; then
	AC_GGZ_DATABASE_PGSQL
fi

dnl Check for MySQL database
if test "$database" = mysql || test "$database" = yes; then
	AC_GGZ_DATABASE_MYSQL
fi

dnl Check for SQLite database
if test "$database" = sqlite || test "$database" = yes; then
	AC_GGZ_DATABASE_SQLITE
fi

dnl Check for DBI database abstraction layer
if test "$database" = dbi || test "$database" = yes; then
	AC_GGZ_DATABASE_DBI
fi

dnl Make sure a database was configured
if test "$database" = yes; then
	AC_MSG_ERROR([no usable database library found.  See above messages for more.])
else
	AC_DEFINE_UNQUOTED([DATABASE_TYPE], "${database}", [Database backend type])

	if test "$database" = "mysql"; then
		AC_DEFINE([WITH_MYSQL], 1, [MySQL is used, needed for statistics])
	elif test "$database" = "pgsql"; then
		AC_DEFINE([WITH_PGSQL], 1, [PostgreSQL is used, needed for statistics])
	fi
fi

AC_SUBST(LIB_DATABASE)
AC_SUBST(DATABASE_INCLUDES)
AM_CONDITIONAL([GGZDB_DB4], [test "$database" = "db4"])
AM_CONDITIONAL([GGZDB_MYSQL], [test "$database" = "mysql"])
AM_CONDITIONAL([GGZDB_PGSQL], [test "$database" = "pgsql"])
AM_CONDITIONAL([GGZDB_SQLITE], [test "$database" = "sqlite"])
AM_CONDITIONAL([GGZDB_DBI], [test "$database" = "dbi"])
])

