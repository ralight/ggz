/*
 * File: gtk_taunt.c
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 1/23/99
 *
 * This fils contains functions for creating and handling the taunt area
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

#ifdef HAVE_CONFIG_H
#  include <config.h>	/* Site-specific config */
#endif

#include <gtk/gtk.h>
#include <string.h>
#include <unistd.h>

#include <gtk_taunt.h>
#include <client.h>


/* Global state of game variable */
extern gameState_t gameState;


/* Global taunt variables */
GtkWidget *tauntBox;
static GtkWidget *tauntDisplay;
static GdkColor tauntColor[4];


/*
 * Create and initialize the tauntDisplay area
 */
GtkWidget *CreateTauntArea(void)
{

	GtkWidget *box1, *box2, *box3, *button, *separator;
	GdkColormap *cMap;

	/* Create the GtkText widget */
	tauntDisplay =
	    gtk_text_view_new_with_buffer(gtk_text_buffer_new(NULL));
	gtk_text_view_set_editable(GTK_TEXT_VIEW(tauntDisplay), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tauntDisplay),
				    GTK_WRAP_WORD);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(tauntDisplay),
					 FALSE);
	gtk_widget_show(tauntDisplay);


	/* Separator between entry and display */
	separator = gtk_hseparator_new();
	gtk_widget_show(separator);


	/* Taunt entry */
	tauntBox = gtk_entry_new_with_max_length(256);
	g_signal_connect_swapped(GTK_OBJECT(tauntBox), "activate",
				 GTK_SIGNAL_FUNC(ReadTaunt),
				 GTK_OBJECT(tauntBox));
	gtk_widget_set_sensitive(tauntBox, FALSE);
	gtk_widget_show(tauntBox);


	/* Taunt submit button */
	button = gtk_button_new_with_label("Send");
	g_signal_connect_swapped(GTK_OBJECT(button), "clicked",
				 GTK_SIGNAL_FUNC(ReadTaunt),
				 GTK_OBJECT(tauntBox));
	gtk_widget_show(button);


	/* Get the system colour map and allocate the colour red */
	cMap = gdk_colormap_get_system();
	tauntColor[0].red = 0;
	tauntColor[0].green = 0;
	tauntColor[0].blue = 0;
	if (!gdk_color_alloc(cMap, &tauntColor[0])) {
		g_error("couldn't allocate colour");
	}
	tauntColor[1].red = 0xffff;
	tauntColor[1].green = 0;
	tauntColor[1].blue = 0;
	if (!gdk_color_alloc(cMap, &tauntColor[1])) {
		g_error("couldn't allocate colour");
	}
	tauntColor[2].red = 0;
	tauntColor[2].green = 0x8888;
	tauntColor[2].blue = 0xa0a0;
	if (!gdk_color_alloc(cMap, &tauntColor[2])) {
		g_error("couldn't allocate colour");
	}
	tauntColor[3].red = 0;
	tauntColor[3].green = 0;
	tauntColor[3].blue = 0xffff;
	if (!gdk_color_alloc(cMap, &tauntColor[3])) {
		g_error("couldn't allocate colour");
	}

	/* Packing boxes */
	box1 = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(box1), 10);
	box2 = gtk_hbox_new(FALSE, 10);
	box3 = gtk_hbox_new(FALSE, 10);

	gtk_box_pack_start(GTK_BOX(box2), tauntDisplay, TRUE, TRUE, 0);
	gtk_widget_show(box2);

	gtk_box_pack_start(GTK_BOX(box3), tauntBox, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(box3), button, FALSE, FALSE, 0);
	gtk_widget_show(box3);

	gtk_box_pack_start(GTK_BOX(box1), box2, TRUE, TRUE, 0);
	gtk_box_pack_end(GTK_BOX(box1), box3, FALSE, FALSE, 0);
	gtk_box_pack_end(GTK_BOX(box1), separator, FALSE, FALSE, 10);

	return box1;

}


void ReadTaunt(GtkWidget * widget)
{

	if (gameState.gameSegment > ST_REG_TAUNT
	    && strcmp(gtk_entry_get_text(GTK_ENTRY(tauntBox)), "") != 0
	    && CheckWriteInt(gameState.tauntSock,
			     gameState.gameNum) == NET_OK
	    && CheckWriteInt(gameState.tauntSock,
			     gameState.gamePid) == NET_OK
	    && CheckWriteInt(gameState.tauntSock,
			     gameState.playerId) == NET_OK
	    && CheckWriteInt(gameState.tauntSock, getpid()) == NET_OK) {

		CheckWriteString(gameState.tauntSock,
				 gtk_entry_get_text(GTK_ENTRY(tauntBox)));
	}

	gtk_entry_set_text(GTK_ENTRY(tauntBox), "");

}

void DisplayTaunt(char *taunt, int playerNum)
{

	char *buf;

	if (playerNum == -1) {
		buf = g_strdup_printf("[ %s ] %s ", "TauntServer", taunt);
		playerNum = 0;
	} else {
		buf =
		    g_strdup_printf("[ %s ] %s ",
				    gameState.players[playerNum], taunt);
	}

	/* FIXME: color */
	gtk_text_buffer_insert_at_cursor(gtk_text_view_get_buffer
					 (GTK_TEXT_VIEW(tauntDisplay)),
					 buf, -1);
	gtk_text_buffer_insert_at_cursor(gtk_text_view_get_buffer
					 (GTK_TEXT_VIEW(tauntDisplay)),
					 "\n", -1);

	g_free(buf);

}
