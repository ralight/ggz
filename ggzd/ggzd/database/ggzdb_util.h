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

/* Escape a string for SQL */
char *_ggz_sql_escape(const char *str);

/* Unescape a string from SQL */
char *_ggz_sql_unescape(const char *str);

/* Free a ggzdbSavegamePlayers structure */
void _ggzdb_sgpstr_free(void *str);

/* Replace all patterns in a string */
char *_ggzdb_strreplace(const char *str, const char *pattern, const char *subst);

#endif

