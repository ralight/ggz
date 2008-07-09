/*
 * File: ggzutils.c
 * Author: GGZ Dev Team
 * Project: GGZ GTK Client
 * Date: 12/28/2006
 * $Id: playerlist.c 8763 2006-12-27 10:02:33Z jdorje $
 * 
 * GGZ-specific utility functions
 * 
 * Copyright (C) 2006 GGZ Development Team
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

#include <assert.h>

#include "ggzutils.h"
#include "support.h"

/* Return a text name for the permission, or NULL. */
const char *perm_name(GGZPerm p)
{
	switch (p) {
	case GGZ_PERM_JOIN_TABLE:
		return _("Can join tables");
	case GGZ_PERM_ROOMS_LOGIN:
		return _("Can join rooms");
	case GGZ_PERM_LAUNCH_TABLE:
		return _("Can launch tables");
	case GGZ_PERM_ROOMS_ADMIN:
		return _("Room administrator");
	case GGZ_PERM_CHAT_ANNOUNCE:
		return _("Can make server announcements");
	case GGZ_PERM_CHAT_BOT:
		return _("Is a bot");
	case GGZ_PERM_NO_STATS:
		return _("No stats for this player");
	case GGZ_PERM_EDIT_TABLES:
		return _("Can admin tables");
	case GGZ_PERM_TABLE_PRIVMSG:
		return _("Can send private messages at a table");
	case GGZ_PERM_COUNT:
		break;
	}

	return NULL;
}

/* Used for forground/background colors 
 * in the chat area
 */
static GdkColor ColorWhite = {0, 0xFFFF, 0xFFFF, 0xFFFF};
static GdkColor ColorBlack = {0, 0x0000, 0x0000, 0x0000};

/* Aray of GdkColors currently used for chat and MOTD
 * They are all non-ditherable and as such should look the same everywhere
 */
static GdkColor colors[] =
{
        {0, 0x0000, 0x0000, 0x0000},          /* 0   Black			*/
        {0, 0xFFFF, 0xFFFF, 0x3333},          /* 1   Dark Goldenrod		*/
        {0, 0xCCCC, 0x0000, 0x0000},          /* 2   Orange Red 3		*/
        {0, 0x6666, 0x9999, 0x0000},          /* 3   Olive Drab			*/
        {0, 0xCCCC, 0x3333, 0xCCCC},          /* 4   Medium Orchid		*/
        {0, 0x9999, 0x3333, 0x3333},          /* 5   Indian Red 4		*/
        {0, 0x0000, 0x6666, 0xFFFF},          /* 6   Royal Blue 2		*/
        {0, 0xFFFF, 0x9999, 0x3333},          /* 7   Tan 1			*/
        {0, 0x6666, 0xCCCC, 0xCCCC},          /* 8   Dark Slate Grey 3		*/
        {0, 0x6666, 0xCCCC, 0xFFFF},          /* 9   Cadet Blue			*/
        {0, 0x9999, 0x3333, 0xFFFF},          /* 10  Purple 2			*/
        {0, 0x9999, 0x0000, 0x6666},          /* 11  Violet Red 4		*/
        {0, 0x3333, 0x0000, 0x6666},          /* 12  Dark Blue			*/
        {0, 0x9999, 0x3333, 0x3333},          /* 13  Indian Red			*/
        {0, 0x3333, 0x6666, 0xFFFF},          /* 14  Blue			*/
        {0, 0x6666, 0xCCCC, 0xFFFF},          /* 15  Pale Violet Red		*/
        {0, 0xCCCC, 0xCCCC, 0x3333},          /* 16  Yellow 3			*/
        {0, 0x6666, 0xFFFF, 0xCCCC},          /* 17  Aquamarine 2		*/
        {0, 0xFFFF, 0xFFFF, 0xFFFF},          /* 18  forebround (White)		*/
        {0, 0x0000, 0x0000, 0x0000}           /* 19  background (Black)		*/
};

/* chat_allocate_colors() - Allocates the colors all at once so they
 *                          can be called without the need to allocate
 *                          each time.
 *
 * Recieves:
 *
 * Returns:   
 */

void init_colors(void)
{
	gint i;

        /* Allocate standared colors (just once)*/
        if (!colors[0].pixel) {
                for (i = 0; i < 20; i++) {
                        colors[i].pixel = (gulong) ((colors[i].red & 0xff00) * 256 +
                                        (colors[i].green & 0xff00) +
                                        (colors[i].blue & 0xff00) / 256);
                        if (!gdk_colormap_alloc_color(gdk_colormap_get_system(),
						      &colors[i],
						      FALSE, TRUE)) {
                                g_error("*** GGZ: Couldn't alloc color\n");
			}
                }
        }
        ColorBlack.pixel = (gulong) ((ColorBlack.red & 0xff00) * 256 +
        			(ColorBlack.green & 0xff00) +
        			(ColorBlack.blue & 0xff00) / 256);
        if (!gdk_colormap_alloc_color(gdk_colormap_get_system(),
				      &ColorBlack, FALSE, TRUE)) {
        	g_error("*** GGZ: Couldn't alloc color\n");
	}
        ColorWhite.pixel = (gulong) ((ColorWhite.red & 0xff00) * 256 +
        			(ColorWhite.green & 0xff00) +
        			(ColorWhite.blue & 0xff00) / 256);
        if (!gdk_colormap_alloc_color(gdk_colormap_get_system(),
				      &ColorWhite, FALSE, TRUE)) {
        	g_error("*** GGZ: Couldn't alloc color\n");
	}
}

GtkWidget *create_xtext(void)
{
	return gtk_xtext_new(colors, TRUE);
}


void activate_colors(GtkXText *xtext)
{
	gtk_xtext_set_palette(xtext, colors);
}

GdkColor color_white(void)
{
	return ColorWhite;
}

GdkColor color_black(void)
{
	return ColorBlack;
}

GdkColor get_color(int id)
{
	assert(id >= 0 && id < sizeof(colors) / sizeof(colors[0]));
	return colors[id];
}

void set_color(int id, GdkColor color)
{
	colors[id] = color;
}
