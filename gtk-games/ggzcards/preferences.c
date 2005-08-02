/* 
 * File: preferences.c
 * Author: Jason Short
 * Project: GGZCards Client
 * Date: 02/17/2002
 * Desc: Provides automated preferences handling
 * $Id: preferences.c 6330 2004-11-11 16:30:21Z jdorje $
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
#include <stdlib.h>	/* for getenv */

#include <ggz.h>	/* libggz */

#include "ggzintl.h"

#include "game.h"
#include "preferences.h"

PrefType pref_types[] = {
	{"animation",
	 N_("Use animation"),
	 N_
	 ("If this is disabled, there will be no animation at all.  Without "
	  "animation, none of the other animation preferences will have "
	  "any effect."),
	 &preferences.animation,
	 TRUE}
	,

	/* These options are no longer used.  Now all animations have 250ms
	   duration and maximum possible smoothness. */
#if 0
	{"faster_animation",
	 N_("Faster animation"),
	 N_("If this is selected, the animation will happen faster.  The "
	    "faster animation generally looks smoother, but may not work "
	    "well on slower computers."),
	 &preferences.faster_animation,
	 TRUE}
	,

	{"smoother_animation",
	 N_("Smoother animation"),
	 N_("If this is selected, there will be more frames in each "
	    "animation sequence.  This will generally make the animation "
	    "look smoother, but may not work well on slower computers."),
	 &preferences.smoother_animation,
	 TRUE}
	,
#endif

	{"multiple_animation",
	 N_("Multiple (simultaneous) animations"),
	 N_
	 ("If this preference is selected, then any time a new animation "
	  "is ready to start while an old one is still going they will "
	  "both proceed simultaneously.  This is prettier, but may not "
	  "work well on slower computers and is (possibly) harder to "
	  "follow.  Disabling it will \"zip\" the current animation to "
	  "a finish when the new one is started."),
	 &preferences.multiple_animation,
	 FALSE}
	,

	{"longer_clearing_delay",
	 N_("Have a longer delay before clearing the table"),
	 N_("Normally, after the end of a trick the cards will not be "
	    "cleared off of the table for one second.  Selecting this "
	    "option will instead delay for two seconds.  Note that "
	    "after the cards are cleared, the previous trick will "
	    "(depending on the game) still be visible in the "
	    "\"Last Trick\" selection of the \"Messages\" menu."),
	 &preferences.longer_clearing_delay,
	 FALSE}
	,
#ifdef DEBUG
	{"no_clearing_delay",
	 N_("Clear the table immediately after each trick"),
	 N_("Setting this option causes the cards to be cleared from the "
	    "table after just one-tenth of a second."),
	 &preferences.no_clearing_delay,
	 FALSE}
	,
#endif

	{"single_click_play",
	 N_("Single-click cards to play them"),
	 N_("Normally, you must click a card twice to play it (once "
	    "to select it and again to commit to the play).  With "
	    "this option, the card is played immediately when you "
	    "click it."),
	 &preferences.single_click_play,
	 FALSE}
	,

	{"collapse_hand",
	 N_("Collapse hand as it is played"),
	 N_("If this is unset, then when a card is played none of "
	    "the other cards change position, but an empty spot is "
	    "left in the hand.  If set, then the hand will be collapsed "
	    "so that the empty spot is filled.  Changing this will take "
	    "effect immediately, but the transition may not be smooth."),
	 &preferences.collapse_hand,
	 TRUE}
	,

	{"cardlists",
	 N_("Show graphical cardlists"),
	 N_
	 ("If this is selected, then card lists (like the \"Last Trick\" "
	  "and \"Previous Hand\" messages) will be drawn graphically.  "
	  "Disable it to get simple text-drawn card lists.  The choice "
	  "will not take effect until you restart GGZCards."),
	 &preferences.cardlists,
	 TRUE}
	,

#ifdef CAN_BID_ON_TABLE
	{"bid_on_table",
	 N_("Show bid choices right on the table"),
	 N_("If this is selected, then (for games that use bidding) the "
	    "choices for the bid will be drawn directly onto the table.  "
	    "If it's deselected you will instead get a popup window showing "
	    "the choices.  Having them drawn on the table is better, but "
	    "may not work for all games."),
	 &preferences.bid_on_table,
	 TRUE}
	,
#endif

	{"bidding_tooltips",
	 N_("Show tooltips for bidding"),
	 N_("Select this option to see descriptive tooltips for every "
	    "choice when you bid.  The choice will take effect the next "
	    "time you bid."),
	 &preferences.bidding_tooltips,
	 TRUE}
	,

	{"large_dominoes",
	 N_("Draw dominoes much larger"),
	 N_("Some games use dominoes as the card set.  You may choose "
	    "to have the dominoes drawn normal size or in a much "
	    "larger size.  The change may not happen correctly until "
	    "you restart the client."),
	 &preferences.large_dominoes,
	 FALSE}
	,

	{"autostart",
	 N_("Automatically start game"),
	 N_("If this is selected, then instead of waiting for you to "
	    "choose \"Start Game\" from the \"Game\" menu, the game "
	    "will instead start immediately (once everyone else is "
	    "ready, of course)."),
	 &preferences.autostart,
	 FALSE}
	,

	{"use_default_options",
	 N_("Always use default options"),
	 N_("If this is selected, then any time you are prompted for "
	    "game options (which only happens at the start of a game "
	    "when you are the table host) the default choices will "
	    "automatically be chosen."),
	 &preferences.use_default_options,
	 FALSE}
	,

	{"confirm_exit",
	 N_("Confirmation before exiting"),
	 N_("If selected, you will get a confirmation dialog when you "
	    "try to exit during a game."),
	 &preferences.confirm_exit,
	 TRUE}
	,

#ifdef DEBUG
	{"use_ai",
	 N_("Have the AI play for you"),
	 N_("If selected, the AI player will play the game for you.  "
	    "This is useful as a debugging tool."),
	 &preferences.use_ai,
	 FALSE}
	,
#endif /* DEBUG */

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
		file =
		    ggz_conf_parse(name, GGZ_CONF_RDWR | GGZ_CONF_CREATE);

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
