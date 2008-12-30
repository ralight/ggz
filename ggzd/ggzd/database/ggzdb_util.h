/*
 * Helper functions for ggzd database modules
 * Copyright (C) 2000 Brent Hendricks.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifndef GGZDB_UTIL_H

/* Function declaration for a callback to execute a query on a DB.
 * Should return 1 on success, 0 on failure.
 * dbconn is a pointer to the DB specific connection handle.
 * query is the SQL string to execute.
 * error is a pre-allocated string of length errorlen that any error message
 * should be copied into.
 */
typedef int (*GGZDBExecSQL) (void *dbconn, const char *query, char *error, size_t errorlen);

/* Backend independent schema setup function using the GGZDBExecSQL callback
 * function to get the DB specific query execution.
 */
int _ggz_setupschema(const char *filename, GGZDBExecSQL sql_exec, void *sql_conn);

/* Escape a string for SQL */
char *_ggz_sql_escape(const char *str);

/* Unescape a string from SQL */
char *_ggz_sql_unescape(const char *str);

/* Free a ggzdbSavegamePlayers structure */
void _ggzdb_sgpstr_free(void *str);

/*
 * Function to return the normalized version of a string (e.g. username)
 * according to a "stringprep" profile.
 */
char *username_canonical(const char *username);

#endif

