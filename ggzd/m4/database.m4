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
dnl AC_GGZ_DATABASE_DB2 - Sleepycat DB2.x
dnl AC_GGZ_DATABASE_DB3 - Sleepycat DB3.x
dnl AC_GGZ_DATABASE_DB4 - Sleepycat DB4.x
dnl AC_GGZ_DATABASE_MYSQL - MySQL 3.x/4.x
dnl AC_GGZ_DATABASE_PGSQL - PostgreSQL 7.x/8.x
dnl

AC_DEFUN([AC_GGZ_DATABASE_DB3],
[
	db3lib=""
	db3inc=""

	dnl Check for db3 libraries
	dnl Version priority: db3.2, db3

	if test "$db3lib" = ""; then
		AC_CHECK_LIB(db-3.2, db_env_create,
		[
			db3lib="-ldb-3.2"
			database=db3
		],
		[])
	fi

	if test "$db3lib" = ""; then
		AC_CHECK_LIB(db3, db_env_create,
		[
			db3lib="-ldb3"
			database=db3
		],
		[])
	fi

	if test "$database" = "db3" && test "$db3lib" = ""; then
		AC_MSG_ERROR([cannot configure db3 (libdb3 needed)])
	fi

	dnl Check for include files

	AC_CHECK_HEADER(db.h,
	[
		db3inc="db.h"
		database=db3
	],
	[
		AC_CHECK_HEADER(db3/db.h,
		[
			db3inc="db3/db.h"
			database=db3
			AC_DEFINE([DB3_IN_DIR], 1, [Define if the db3 libs and headers are under db3/])
		],
		[])
	])

	if test "$database" = "db3" && test "$db3inc" = ""; then
		AC_MSG_ERROR([cannot configure db3 (db3-dev needed)])
	fi

	dnl Setup variables

	if test "$database" = "db3"; then
		LIB_DATABASE="$db3lib"
	fi
])

AC_DEFUN([AC_GGZ_DATABASE_DB4],
[
	db4lib=""
	db4inc=""

	dnl Check for db4 libraries
	dnl Version priority: db4.3, db4.2, db4.1, db4.0, db (unversioned)

	if test "$db4lib" = ""; then
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

	if test "$db4lib" = ""; then
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

	if test "$db4lib" = ""; then
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

	if test "$db4lib" = ""; then
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
			AC_DEFINE([DB4_IN_DIR], 1, [Define if the db4 libs and headers are under db4/])
		],
		[])
	])

	if test "$database" = "db4" && test "$db4inc" = ""; then
		AC_MSG_ERROR([cannot configure db4 (db4-dev needed)])
	fi

	dnl Setup variables

	if test "$database" = "db4"; then
		LIB_DATABASE="$db4lib"
	fi
])

AC_DEFUN([AC_GGZ_DATABASE_DB2],
[
	db2lib=""
	db2inc=""

	dnl Check for db2 libraries
	dnl Version priority: db2, db

	if test "$db2lib" = ""; then
		AC_CHECK_LIB(db2, db_appinit,
		[
			db2lib="-ldb2"
			database=db2
		],
		[])
	fi

	if test "$db2lib" = ""; then
		AC_CHECK_LIB(db, db_appinit,
		[
			db2lib="-ldb"
			database=db2
		],
		[])
	fi

	if test "$database" = "db2" && test "$db2lib" = ""; then
		AC_MSG_ERROR([cannot configure db2 (libdb2 needed)])
	fi

	dnl Check for include files

	AC_CHECK_HEADER(db.h,
	[
		db4inc="db.h"
		database=db2
	],
	[
		AC_CHECK_HEADER(db2/db.h,
		[
			db2inc="db2/db.h"
			database=db2
			AC_DEFINE([DB2_IN_DIR], 1, [Define if the db2 libs and headers are under db2/])
		],
		[])
	])

	if test "$database" = "db2" && test "$db2inc" = ""; then
		AC_MSG_ERROR([cannot configure db2 (db2-dev needed)])
	fi

	dnl Setup variables

	if test "$database" = "db2"; then
		LIB_DATABASE="$db2lib"
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
	LIBS="$LIBS -L/usr/lib/mysql"
	AC_CHECK_LIB(mysqlclient_r, mysql_connect,
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

AC_DEFUN([AC_GGZ_DATABASE],
[
case "$database" in
	db4)    database=db4 ;;
	db3)    database=db3 ;;
	db2)    database=db2 ;;
	pgsql)  database=pgsql ;;
	mysql)  database=mysql ;;
	sqlite) database=sqlite ;;
	*)      database=yes ;;
esac

dnl Order of preference: db4, db3, db2, PgSQL, MySQL.  This is determined
dnl solely by the order of the checks below.  Since a correct db will be
dnl auto-detected, we should order these solely based on how good they are
dnl (in terms of stability and performance).

dnl Check for db4 database
if test "$database" = db4 || test "$database" = yes; then
	AC_GGZ_DATABASE_DB4
fi

dnl Check for db3 database
if test "$database" = db3 || test "$database" = yes; then
	AC_GGZ_DATABASE_DB3
fi

dnl Check for db2 database
if test "$database" = db2 || test "$database" = yes; then
	AC_GGZ_DATABASE_DB2
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

dnl Make sure a database was configured
if test "$database" = yes; then
	AC_MSG_ERROR([no usable database library found.  See above messages for more.])
else
	AC_DEFINE_UNQUOTED([DATABASE_TYPE], "${database}", [Database backend type])
fi

AC_SUBST(LIB_DATABASE)
AM_CONDITIONAL([GGZDB_DB2], [test "$database" = "db2"])
AM_CONDITIONAL([GGZDB_DB3], [test "$database" = "db3"])
AM_CONDITIONAL([GGZDB_DB4], [test "$database" = "db4"])
AM_CONDITIONAL([GGZDB_MYSQL], [test "$database" = "mysql"])
AM_CONDITIONAL([GGZDB_PGSQL], [test "$database" = "pgsql"])
AM_CONDITIONAL([GGZDB_SQLITE], [test "$database" = "sqlite"])
])

