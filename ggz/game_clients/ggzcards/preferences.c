/* 
 * File: preferences.c
 * Author: Jason Short
 * Project: GGZCards Client
 * Date: 02/17/2002
 * Desc: Provides automated preferences handling
 * $Id: preferences.c 3685 2002-03-25 22:40:22Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdlib.h>		/* for getenv */

#include <ggz.h>		/* libggz */

#include "ggzintl.h"

#include "game.h"
#include "preferences.h"

PrefType pref_types[] = {
	{"animation", N_("Use animation"),
	 &preferences.animation, TRUE},
	
	{"faster_animation", N_("Faster animation"),
	 &preferences.faster_animation, FALSE},
	
	{"smoother_animation", N_("Smoother animation"),
	 &preferences.smoother_animation, FALSE},
	
	{"multiple_animation", N_("Multiple (simultaneous) animations"),
	 &preferences.multiple_animation, FALSE},
	
	{"longer_clearing_delay",
	 N_("Have a longer delay before clearing the table"),
	 &preferences.longer_clearing_delay, FALSE},
	
	{"cardlists",
	 N_("Show graphical cardlists (may not take effect immediately)"),
	 &preferences.cardlists, TRUE},
	
	{"autostart", N_("Show bid choices right on the table"),
	 &preferences.bid_on_table, FALSE},
	
	{"use_default_options", N_("Automatically start game"),
	 &preferences.autostart, FALSE},
	
	{"bid_on_table", N_("Always use default options"),
	 &preferences.use_default_options, FALSE},
	
	{NULL, NULL, NULL, 0}
};


static void access_preferences(int save);


/* This function can be called at any time to save _or_ load the global
   settings (preferences). If "save" is TRUE, we'll save the settings.
   Otherwise we'll load them. File name, etc., are all taken care of
   internally by the function. Note, this function need not have anything to 
   do with the code for the user to edit their preferences.  This code just
   read/writes the data to/from the file. */
static void access_preferences(int save)
{
	static int file = -1;
	PrefType *pref;

	if (file < 0) {
		/* Preferences go in ~/.ggz/ggzcards-gtk.rc */
		char *name = g_strdup_printf("%s/.ggz/ggzcards-gtk.rc",
					     getenv("HOME"));
		file = ggz_conf_parse(name, GGZ_CONF_RDWR | GGZ_CONF_CREATE);

		if (file < 0)
			ggz_error_msg("Couldn't open conf file '%s'.",
			              name);

		g_free(name);
	}

	if (save) {
		/* Save. */
		for (pref = pref_types; pref->name; pref++) {
			ggz_conf_write_int(file, "BOOLEAN", pref->name,
					   *pref->value);
		}

		ggz_conf_commit(file);
	} else {
		/* Load. */
		for (pref = pref_types; pref->name; pref++) {
			*pref->value =
				ggz_conf_read_int(file, "BOOLEAN", pref->name,
						  pref->dflt);
		}
	}
}

void load_preferences(void)
{
	access_preferences(FALSE);
}

void save_preferences(void)
{
	access_preferences(TRUE);
}
