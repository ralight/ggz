/*
 * File: ggzrc.h
 * Author: Brent Hendricks
 * Project: GGZ Client
 * Date: 03/02/2000
 *
 * This fils contains functions for connecting with the server
 *
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

extern int ggzrc_initialize(char *);
extern int ggzrc_commit_changes(void);
extern void ggzrc_cleanup(void);
extern void ggzrc_write_string(const char *, const char *, const char *);
extern char *ggzrc_read_string(const char *, const char *, const char *);
extern void ggzrc_write_int(const char *, const char *, const int);
extern int ggzrc_read_int(const char *, const char *, const int);
