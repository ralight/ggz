/*
 * File: module.h
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 12/01/00
 *
 * This fils contains functions for handling client-side game modules
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

#ifndef __MODULE_H_
#define __MODULE_H_

/* Structure describing particular client-side game module */
struct _GGZModule {

	/* Type of game played by this module */
	char *game;
	
	/* Game module version */
	char *version;

	/* Protocol version implemented */
	char *protocol;

	/* Module author */
	char *author;

	/* Native frontend */
	char *frontend;

	/* Hopepage for this module */
	char *url;

	/* Path to module executable */
	char *path;

	/* Path to icon for this game module */
	char *icon;

	/* Path to help file */
	char *help;

};

int _ggzcore_module_setup(void);
unsigned int _ggzcore_module_get_num(void);
int _ggzcore_module_launch(struct _GGZModule *module);
void _ggzcore_module_cleanup(void);

char* _ggzcore_module_get_game(struct _GGZModule *module);
char* _ggzcore_module_get_version(struct _GGZModule *module);
char* _ggzcore_module_get_protocol(struct _GGZModule *module);
char* _ggzcore_module_get_author(struct _GGZModule *module);
char* _ggzcore_module_get_frontend(struct _GGZModule *module);
char* _ggzcore_module_get_url(struct _GGZModule *module);
char* _ggzcore_module_get_path(struct _GGZModule *module);
char* _ggzcore_module_get_icon_path(struct _GGZModule *module);
char* _ggzcore_module_get_help_path(struct _GGZModule *module);


#endif /* __MODULE_H_ */
