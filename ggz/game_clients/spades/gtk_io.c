/*
 * File: gtk_io.c
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 1/17/99
 *
 * This file contains functions for displaying cards, the current
 * score, etc.
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


#include <config.h>		/* Site config data */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>		/* For atoi */
#include <stdarg.h>
#include <gtk/gtk.h>

#include <ggz.h>

#include "dlg_exit.h"
#include "ggzintl.h"

#include <card.h>
#include <client.h>
#include <display.h>
#include <gtk_io.h>
#include <gtk_connect.h>
#include <gtk_dlg_options.h>
#include <gtk_dlg_stat.h>
#include <gtk_menu.h>
#include <gtk_taunt.h>
#include <gtk_play.h>

/* Global state of game variable */
extern gameState_t gameState;
playArea_t *playArea;


/* Global status bar variables */
GtkWidget *statusBar;
guint id;


/**
 * Initialize display (pop up main window too)
 */
void DisplayInit(void)
{
	GtkWidget *mainBox, *splitBox, *vLine, *tauntArea, *menuBar/*, *tmp*/;

	/* Allocate playArea */
	playArea = (playArea_t *) malloc(sizeof(playArea_t));

	/* Set all pointers to NULL */
	memset(playArea, 0, sizeof(playArea_t));

	/* Main window and callbacks */
	playArea->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_window_set_title(GTK_WINDOW(playArea->window), "GtkSpades");

	gtk_signal_connect(GTK_OBJECT(playArea->window), "delete_event",
			   GTK_SIGNAL_FUNC(ExitDialog), NULL);



	/* Create menus */
	menuBar = CreateMenus(playArea->window);
	gtk_widget_show(menuBar);

	/* Create Playing area */
	CreatePlayArea();
	gtk_widget_show(playArea->field);

	/* Separator */
	vLine = gtk_vseparator_new();
	/* gtk_widget_show(vLine); */

	/* Create Taunt Area */
	tauntArea = CreateTauntArea(); 
	/* gtk_widget_show(tauntArea); */

	/* Status Bar */
	statusBar = gtk_statusbar_new();
	/*gtk_container_set_border_width( GTK_CONTAINER(statusBar), 10 ); */
	id =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(statusBar), "Main");
	gtk_widget_show(statusBar);

	/* Packing boxes */
	mainBox = gtk_vbox_new(FALSE, 0);
	splitBox = gtk_hbox_new(FALSE, 0);

	gtk_box_pack_start(GTK_BOX(splitBox), playArea->field, FALSE,
			   FALSE, 0);
	gtk_box_pack_start(GTK_BOX(splitBox), vLine, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(splitBox), tauntArea, TRUE, TRUE, 0);
	gtk_widget_show(splitBox);

	gtk_box_pack_start(GTK_BOX(mainBox), menuBar, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(mainBox), splitBox, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(mainBox), statusBar, FALSE, FALSE, 0);
	gtk_widget_show(mainBox);

	gtk_container_add(GTK_CONTAINER(playArea->window), mainBox);
	/*gtk_widget_realize( playArea->window ); */
	gtk_widget_show(playArea->window);

	InitPixmaps(playArea->window);

}


static GdkPixbuf *load_pixmap(GdkWindow *window, GdkBitmap **mask,
	    GdkColor *trans, const char *name)
{
	char *fullpath;
	GdkPixbuf *image;
	GError *error = NULL;

	fullpath = g_strdup_printf("%s/pixmaps/%s", GGZDATADIR, name);
	image = gdk_pixbuf_new_from_file(fullpath, &error);
	if(image == NULL)
		ggz_error_msg_exit("Can't load pixmap %s", fullpath);
	g_free(fullpath);

	return image;
}


void InitPixmaps(GtkWidget * window)
{

	GtkStyle *style;
	GdkBitmap *mask;


	/* now for the pixmap from gdk */
	style = gtk_widget_get_style(window);

	playArea->cards = load_pixmap(window->window, &mask,
				      &style->bg[GTK_STATE_NORMAL],
				      "cards-1.xpm");
#if 0 /* Card backs aren't used */
	playArea->cardbacks = load_pixmap(window->window, &mask,
					  &style->bg[GTK_STATE_NORMAL],
					  "cards-b1.xpm");
#endif
}


/**
 * Connect to server Dialog box 
 */
void InputServerInfo(void)
{

	OptionsDialog(NULL, NULL);

}




/**
 * Display a line of text in the status bar
 */
void DisplayStatusLine(const char *format, ...)
{

	va_list ap;
	char *message;

	if (statusBar) {

		va_start(ap, format);
		message = g_strdup_vprintf(format, ap);
		va_end(ap);

		gtk_statusbar_pop(GTK_STATUSBAR(statusBar), id);
		gtk_statusbar_push(GTK_STATUSBAR(statusBar), id, message);
		g_free(message);
	}

}


/**
 * Clear status 
 */
void ClearStatusLine(void)
{

	gtk_statusbar_pop(GTK_STATUSBAR(statusBar), id);

}


/**
 * Display hand of cards
 */
void DisplayHand(void)
{

	int i;

	/* Clear old hand */
	gdk_draw_rectangle(playArea->handBuf,
			   playArea->hand->style->bg_gc[GTK_STATE_NORMAL],
			   TRUE,
			   0, 0,
			   playArea->hand->allocation.width,
			   playArea->hand->allocation.height);


	for (i = 0; i < 13; i++) {
		if (gameState.hand[i] != BLANK_CARD) {
			DisplayCard(gameState.hand[i], 0, 20 * i);
		}
	}

	gtk_widget_draw(playArea->hand, NULL);
}


/**
 * Clear off table and display names (maybe should be called ClearTable? )
 */
void DisplayTable(void)
{


	/* Clear off table */
	gdk_draw_rectangle(playArea->tableBuf,
			   playArea->table->style->bg_gc[GTK_STATE_NORMAL],
			   TRUE,
			   0, 0,
			   playArea->table->allocation.width,
			   playArea->table->allocation.height);

	gtk_label_set_text(GTK_LABEL(playArea->names[0]),
			   gameState.players[gameState.playerId]);

	gtk_label_set_text(GTK_LABEL(playArea->names[1]),
			   gameState.players[(gameState.playerId + 1) %
					     4]);

	gtk_label_set_text(GTK_LABEL(playArea->names[2]),
			   gameState.players[(gameState.playerId + 2) %
					     4]);

	gtk_label_set_text(GTK_LABEL(playArea->names[3]),
			   gameState.players[(gameState.playerId + 3) %
					     4]);

	gtk_widget_draw(playArea->table, NULL);
}


/**
 * Display a card (in your hand)
 */
void DisplayCard(Card card, int y, int x)
{


	int localX, localY, index = card_suit_num(card);

	if (index < 3)
		index = (index + 2) % 3;

	localX = CARDWIDTH * index;
	localY = CARDHEIGHT * (card_face(card) - 1);

	gdk_pixbuf_render_to_drawable(playArea->cards, playArea->handBuf,
				      playArea->hand->style->
				      fg_gc[GTK_WIDGET_STATE(playArea->hand)],
				      localX, localY, x, y,
				      CARDWIDTH, CARDHEIGHT,
				      GDK_RGB_DITHER_NONE, 0, 0);

#ifdef DEBUG
	g_printerr("%s\n", card_name(card, LONG_NAME));
#endif

}


/**
 * Display a card (on the table)
 */
void DisplayPlayedCard(Card card, int player, int id)
{

	int localX, localY, index = card_suit_num(card);
	int x = 0, y = 0, tablePos = (player - id + 4) % 4;

	if (index < 3)
		index = (index + 2) % 3;

	localX = CARDWIDTH * index;
	localY = CARDHEIGHT * (card_face(card) - 1);

	switch (tablePos) {
	case 0:
		x = (TABLEWIDTH - CARDWIDTH) / 2;
		y = TABLEHEIGHT - CARDHEIGHT - 1;
		break;
	case 1:
		x = 0;
		y = (TABLEHEIGHT - CARDHEIGHT) / 2;
		break;
	case 2:
		x = (TABLEWIDTH - CARDWIDTH) / 2;
		y = 1;
		break;
	case 3:
		x = TABLEWIDTH - CARDWIDTH;
		y = (TABLEHEIGHT - CARDHEIGHT) / 2;
		break;
	}

	gdk_pixbuf_render_to_drawable(playArea->cards, playArea->tableBuf,
				      playArea->table->style->
				      fg_gc[GTK_WIDGET_STATE(playArea->
							     table)],
				      localX, localY, x, y,
				      CARDWIDTH, CARDHEIGHT,
				      GDK_RGB_DITHER_NONE, 0, 0);

	gtk_widget_draw(playArea->table, NULL);

#ifdef DEBUG
	g_printerr("%s\n", card_name(card, LONG_NAME));
#endif

}


/**
 * Display Bids/Tallys
 */
void DisplayTallys(void)
{

	char *buf;
	int i;

	for (i = 0; i < 4; i++) {
		switch (gameState.bids[i]) {
		case BID_BLANK:
			buf =
			    g_strdup_printf("%-8s", gameState.players[i]);
			break;
		case BID_KNEEL:
			buf =
			    g_strdup_printf("%-8s : %2d of  n",
					    gameState.players[i],
					    gameState.tallys[i]);
			break;
		default:
			buf =
			    g_strdup_printf("%-8s : %2d of %2d",
					    gameState.players[i],
					    gameState.tallys[i],
					    gameState.bids[i]);
		}
		gtk_label_set_text(GTK_LABEL(playArea->tallys[i]), buf);
		g_free(buf);
	}
}


/**
 * Display team bids/scores
 */
void DisplayScores(void)
{

	char *buf;
	int i;

	for (i = 0; i < 2; i++) {

		/* Display Team Scores */
		buf =
		    g_strdup_printf(_("%s and %s"), gameState.players[i],
				    gameState.players[i + 2]);
		gtk_label_set_text(GTK_LABEL(playArea->teams[i]), buf);
		g_free(buf);

		buf = g_strdup_printf(_("Score: % 4d"), gameState.scores[i]);
		gtk_label_set_text(GTK_LABEL(playArea->scores[i]), buf);
		g_free(buf);

		/* Display Team Bids */
		if (gameState.bids[i] != BID_BLANK
		    && gameState.bids[i + 2] != BID_BLANK) {
			if (gameState.bids[i] == BID_KNEEL
			    || gameState.bids[i + 2] == BID_KNEEL) {
				buf =
				    g_strdup_printf(_("  Bid: n%3d "),
						    (gameState.bids[i] +
						     gameState.bids[i +
								    2] -
						     BID_KNEEL));
			} else {
				buf =
				    g_strdup_printf(_("  Bid:  %3d "),
						    gameState.bids[i] +
						    gameState.bids[i + 2]);
			}
		} else {
			buf = g_strdup_printf(_("  Bid:  "));
		}

		gtk_label_set_text(GTK_LABEL(playArea->bids[i]), buf);
		g_free(buf);
	}

}


void DisplayPrompt(void)
{

	switch (gameState.gameSegment) {

	case ST_GET_HAND:
	case ST_GET_BIDDER:
	case ST_GET_TALLY:
	case ST_GET_SCORE:
	case ST_GET_LEAD:
		break;

	case ST_GET_BIDS:
		if (gameState.curPlayer == gameState.playerId) {
			gtk_widget_set_sensitive(playArea->bidButton,
						 TRUE);
			DisplayStatusLine(_("Your bid: "));
		}
		break;

	case ST_GET_TRICK:
		if (gameState.curPlayer == gameState.playerId) {
			DisplayStatusLine(_("Please play a card"));
		}
		break;

	}			/*switch( gameState.gameSegment ) */
}


/*
 * Function to display record of games won/lost is a session
 */
void DisplayRecord(void)
{

	StatDialog(NULL, NULL);

}


void HideCard(int i)
{

	gameState.hand[i] = BLANK_CARD;
	DisplayHand();

}


void DisplayCleanup(void)
{

#ifdef DEBUG
	g_printerr("I'm dying\n");
#endif

}


int MainIOLoop(void)
{

	gtk_main();

	return 0;

}
