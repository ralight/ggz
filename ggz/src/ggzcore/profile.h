/*
 * File: profile.h
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/22/00
 *
 * This file contains functions for handling server connection profiles
 *
 * Copyright (C) 1998 Brent Hendricks.
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


#ifndef __PROFILE_H__
#define __PROFILE_H__

/* Read (or re-read) server profiles from configuration files */
void profile_load(void);


/* Save server profiles to configuration file */
void profile_save(void);


/* 
 * Add a server to the profile list.  server should be be a pointer to
 * dynamically allocated Server structure.  All string members should
 * also be dynmcially allocated 
 */
void profile_list_add(Profile* server);


/* 
 * Return list of profile names as a GList.  The list should be
 * free'd, but not the data elememts.  
 */
GList* profile_get_name_list(void);


/*
 * Return the list of profile names as an argv-style array of strings
 * The array should be free'd, but not the array elements.
 */
const char** profile_get_names(void);


/*
 * Return a pointer to the server profile whose name matches 'name'
 * Do *not* free()!
 */
Profile* profile_get(char* name);


/*
 * Remove the server profile whose name matches 'name'
 */
void profile_list_remove(char* name);

#endif
