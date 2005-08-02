/* 
 * File: preferences.h
 * Author: Jason Short
 * Project: GGZCards Client
 * Date: 02/17/2002
 * Desc: Provides automated preferences handling
 * $Id: preferences.h 4057 2002-04-23 05:38:53Z jdorje $
 *
 * Copyright (C) 2002 GGZ Development Team
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

#ifndef __PREFERENCES_H__
#define __PREFERENCES_H__

/* List of preference types.  Only booleans are acceptable. */
typedef struct {
	char *name;
	char *desc;
	char *fulldesc;
	bool *value;
	bool dflt;
} PrefType;

/* Information about preferences. */
extern PrefType pref_types[];

void load_preferences(void);
void save_preferences(void);

#endif /* __PREFERENCES_H__ */
