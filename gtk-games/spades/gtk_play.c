/*
 * File: gtk_play.c
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 1/23/99
 * $Id: gtk_play.c 6340 2004-11-12 17:31:36Z jdorje $
 *
 * This fils contains functions for creating and handling the playing area
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


#include <config.h>

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#include <gtk_io.h>
#include <gtk_play.h>
#include <client.h>
#include <display.h>

#include "ggzintl.h"

/* Global state of game variable */
extern gameState_t gameState;
extern playArea_t *playArea;


static gint exposeHand(GtkWidget *, GdkEventExpose *, gpointer);
static gint configHand(GtkWidget *, GdkEventConfigure *, gpointer);
static gint exposeTable(GtkWidget *, GdkEventExpose *, gpointer);
static gint configTable(GtkWidget *, GdkEventConfigure *, gpointer);
static gint ReadCard(GtkWidget *, GdkEventButton *);
static void ReadBid(GtkWidget *);
static gint SpinInput(GtkSpinButton *, gfloat *);
static gint SpinOutput(GtkSpinButton *);

void CreatePlayArea(void)
{

	GtkWidget *leftTally,
	    *rightTally,
	    *mid,
	    *vBar1,
	    *vBar2,
	    *hBar1,
	    *hBar2,
	    *hBar3,
	    *hBar4,
	    *hBar5, /**hand,*/ *hBox, *ioBox, *bid, *bidLabel, *bidTable;
	GtkObject *bidAdj;


	/* Create the tally region widgets ... */
	playArea->teams[0] = gtk_label_new("");
	playArea->teams[1] = gtk_label_new("");
	playArea->scores[0] = gtk_label_new("");
	playArea->scores[1] = gtk_label_new("");
	playArea->bids[0] = gtk_label_new("");
	playArea->bids[1] = gtk_label_new("");
	/*playArea->tallys[0] = gtk_label_new("                   ");
	   playArea->tallys[1] = gtk_label_new("                   ");
	   playArea->tallys[2] = gtk_label_new("                   ");
	   playArea->tallys[3] = gtk_label_new("                   "); */
	playArea->tallys[0] = gtk_label_new("");
	playArea->tallys[1] = gtk_label_new("");
	playArea->tallys[2] = gtk_label_new("");
	playArea->tallys[3] = gtk_label_new("");
	hBar1 = gtk_hseparator_new();
	hBar2 = gtk_hseparator_new();
	hBar3 = gtk_hseparator_new();
	hBar4 = gtk_hseparator_new();

	/* ...and line them up */
	gtk_misc_set_alignment(GTK_MISC(playArea->scores[0]), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(playArea->scores[1]), 0, 0.5);
	gtk_misc_set_alignment(GTK_MISC(playArea->bids[0]), 0, 0.5);
	gtk_misc_set_alignment(GTK_MISC(playArea->bids[1]), 0, 0.5);
	gtk_misc_set_alignment(GTK_MISC(playArea->tallys[0]), 0, 0.5);
	gtk_misc_set_alignment(GTK_MISC(playArea->tallys[1]), 0, 0.5);
	gtk_misc_set_alignment(GTK_MISC(playArea->tallys[2]), 0, 0.5);
	gtk_misc_set_alignment(GTK_MISC(playArea->tallys[3]), 0, 0.5);


	/* ... and show them */
	gtk_widget_show(playArea->teams[0]);
	gtk_widget_show(playArea->teams[1]);
	gtk_widget_show(playArea->scores[0]);
	gtk_widget_show(playArea->scores[1]);
	gtk_widget_show(playArea->bids[0]);
	gtk_widget_show(playArea->bids[1]);
	gtk_widget_show(playArea->tallys[0]);
	gtk_widget_show(playArea->tallys[1]);
	gtk_widget_show(playArea->tallys[2]);
	gtk_widget_show(playArea->tallys[3]);
	gtk_widget_show(hBar1);
	gtk_widget_show(hBar2);
	gtk_widget_show(hBar3);
	gtk_widget_show(hBar4);


	/* Vertical bars between tally regions and table */
	vBar1 = gtk_vseparator_new();
	vBar2 = gtk_vseparator_new();
	gtk_widget_show(vBar1);
	gtk_widget_show(vBar2);

	/* Create the table name labels */
	playArea->names[0] = gtk_label_new("");
	playArea->names[1] = gtk_label_new("");
	playArea->names[2] = gtk_label_new("");
	playArea->names[3] = gtk_label_new("");
	gtk_widget_show(playArea->names[0]);
	gtk_widget_show(playArea->names[1]);
	gtk_widget_show(playArea->names[2]);
	gtk_widget_show(playArea->names[3]);


	/* Create the card table */
	playArea->table = gtk_drawing_area_new();
	gtk_drawing_area_size(GTK_DRAWING_AREA(playArea->table),
			      TABLEWIDTH, TABLEHEIGHT);
	gtk_widget_set_events(playArea->table, GDK_EXPOSURE_MASK);
	gtk_widget_show(playArea->table);

	/* Signals used to handle backing pixmap */
	g_signal_connect(GTK_OBJECT(playArea->table), "expose_event",
			 GTK_SIGNAL_FUNC(exposeTable), NULL);
	g_signal_connect(GTK_OBJECT(playArea->table), "configure_event",
			 GTK_SIGNAL_FUNC(configTable), NULL);


	/* Line between table and hand */
	hBar5 = gtk_hseparator_new();
	gtk_widget_show(hBar5);

	/* Create drawing area for hand */
	playArea->hand = gtk_drawing_area_new();
	gtk_drawing_area_size(GTK_DRAWING_AREA(playArea->hand),
			      CARDWIDTH + 240, CARDHEIGHT);
	gtk_widget_set_events(playArea->hand,
			      GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);
	gtk_widget_show(playArea->hand);

	/* Signals used to handle backing pixmap */
	g_signal_connect(GTK_OBJECT(playArea->hand), "expose_event",
			 GTK_SIGNAL_FUNC(exposeHand), NULL);
	g_signal_connect(GTK_OBJECT(playArea->hand), "configure_event",
			 GTK_SIGNAL_FUNC(configHand), NULL);
	g_signal_connect(GTK_OBJECT(playArea->hand),
			 "button_press_event", GTK_SIGNAL_FUNC(ReadCard),
			 NULL);


	/* Create bid input dial */
	bidLabel = gtk_label_new(_("Bid"));
	gtk_widget_show(bidLabel);
	bidAdj = gtk_adjustment_new(3, -1, 13, 1, 10, 10);
	bid = gtk_spin_button_new(GTK_ADJUSTMENT(bidAdj), 1, 0);

	/* This makes things *slightly* better under GTK2. */
	gtk_spin_button_set_update_policy(GTK_SPIN_BUTTON(bid),
					  GTK_UPDATE_IF_VALID);

	gtk_widget_show(bid);
	g_signal_connect(GTK_OBJECT(bid), "input",
			 GTK_SIGNAL_FUNC(SpinInput), NULL);

	g_signal_connect(GTK_OBJECT(bid), "output",
			 GTK_SIGNAL_FUNC(SpinOutput), NULL);


	/* Make bid button */
	playArea->bidButton = gtk_button_new_with_label(_("Send Bid"));
	gtk_widget_set_sensitive(playArea->bidButton, FALSE);
	g_signal_connect_swapped(GTK_OBJECT(playArea->bidButton),
				 "clicked", GTK_SIGNAL_FUNC(ReadBid),
				 GTK_OBJECT(bid));
	gtk_widget_show(playArea->bidButton);


	/* Now the layout... */

	/* Create left score region */
	leftTally = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(leftTally), playArea->teams[0], FALSE,
			   TRUE, 0);
	gtk_box_pack_start(GTK_BOX(leftTally), hBar1, FALSE, TRUE, 2);
	gtk_box_pack_start(GTK_BOX(leftTally), playArea->scores[0], FALSE,
			   TRUE, 0);
	gtk_box_pack_start(GTK_BOX(leftTally), playArea->bids[0], FALSE,
			   TRUE, 0);
	gtk_box_pack_start(GTK_BOX(leftTally), hBar2, FALSE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(leftTally), playArea->tallys[0], FALSE,
			   TRUE, 0);
	gtk_box_pack_start(GTK_BOX(leftTally), playArea->tallys[2], FALSE,
			   TRUE, 0);
	gtk_widget_show(leftTally);


	/*Create right score region */
	rightTally = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(rightTally), playArea->teams[1], FALSE,
			   TRUE, 0);
	gtk_box_pack_start(GTK_BOX(rightTally), hBar3, FALSE, TRUE, 2);
	gtk_box_pack_start(GTK_BOX(rightTally), playArea->scores[1], FALSE,
			   TRUE, 0);
	gtk_box_pack_start(GTK_BOX(rightTally), playArea->bids[1], FALSE,
			   TRUE, 0);
	gtk_box_pack_start(GTK_BOX(rightTally), hBar4, FALSE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(rightTally), playArea->tallys[1], FALSE,
			   TRUE, 0);
	gtk_box_pack_start(GTK_BOX(rightTally), playArea->tallys[3], FALSE,
			   TRUE, 0);
	gtk_widget_show(rightTally);

	/* Create middle table region */
	mid = gtk_table_new(3, 3, FALSE);
	gtk_table_set_col_spacings(GTK_TABLE(mid), 5);


	gtk_table_attach(GTK_TABLE(mid), playArea->table, 1, 2, 1, 2,
			 GTK_SHRINK, GTK_SHRINK, 0, 0);
	gtk_table_attach(GTK_TABLE(mid), playArea->names[0], 0, 3, 2, 3,
			 GTK_SHRINK, GTK_SHRINK, 0, 0);
	gtk_table_attach(GTK_TABLE(mid), playArea->names[1], 0, 1, 1, 2,
			 GTK_SHRINK, GTK_SHRINK, 0, 0);
	gtk_table_attach(GTK_TABLE(mid), playArea->names[2], 0, 3, 0, 1,
			 GTK_SHRINK, GTK_SHRINK, 0, 0);
	gtk_table_attach(GTK_TABLE(mid), playArea->names[3], 2, 3, 1, 2,
			 GTK_SHRINK, GTK_SHRINK, 0, 0);
	gtk_widget_show(mid);


	/* Create bidbox */
	bidTable = gtk_table_new(2, 2, TRUE);
	gtk_table_set_row_spacings(GTK_TABLE(bidTable), 5);

	gtk_table_attach(GTK_TABLE(bidTable), bid, 1, 2, 0, 1,
			 (GtkAttachOptions) GTK_FILL,
			 (GtkAttachOptions) GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(bidTable), bidLabel, 0, 1, 0, 1,
			 (GtkAttachOptions) GTK_FILL,
			 (GtkAttachOptions) GTK_FILL, 0, 0);
	gtk_table_attach(GTK_TABLE(bidTable), playArea->bidButton, 0, 2, 1,
			 2, (GtkAttachOptions) GTK_FILL,
			 (GtkAttachOptions) GTK_FILL, 0, 5);
	gtk_widget_show(bidTable);


	/* Packing boxes */
	playArea->field = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(playArea->field), 10);

	hBox = gtk_hbox_new(FALSE, 0);
	ioBox = gtk_hbox_new(FALSE, 10);

	gtk_box_pack_start(GTK_BOX(hBox), leftTally, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hBox), vBar1, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX(hBox), mid, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hBox), vBar2, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hBox), rightTally, TRUE, TRUE, 0);
	gtk_widget_show(hBox);

	gtk_box_pack_start(GTK_BOX(ioBox), playArea->hand, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(ioBox), bidTable, FALSE, FALSE, 0);
	gtk_widget_show(ioBox);

	gtk_box_pack_start(GTK_BOX(playArea->field), hBox, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX(playArea->field), hBar5, FALSE, TRUE,
			   10);
	gtk_box_pack_start(GTK_BOX(playArea->field), ioBox, FALSE, TRUE,
			   0);

}


/* 
 * Create a new backing pixmap of the appropriate size 
 */
static gint configHand(GtkWidget * widget, GdkEventConfigure * event,
		       gpointer data)
{


	if (playArea->handBuf == NULL) {
		playArea->handBuf = gdk_pixmap_new(widget->window,
						   widget->allocation.
						   width,
						   widget->allocation.
						   height, -1);

		gdk_draw_rectangle(playArea->handBuf,
				   widget->style->bg_gc[GTK_STATE_NORMAL],
				   TRUE,
				   0, 0,
				   widget->allocation.width,
				   widget->allocation.height);

	}

	return TRUE;

}


/*
 * Redraw the screen from the backing pixmap 
 */
static gint exposeHand(GtkWidget * widget, GdkEventExpose * event,
		       gpointer data)
{

	gdk_draw_drawable(widget->window,
			  widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
			  playArea->handBuf,
			  event->area.x, event->area.y,
			  event->area.x, event->area.y,
			  event->area.width, event->area.height);

	/* draw a test black rectangle */
	/*gdk_draw_rectangle( widget->window,
	   widget->style->black_gc,
	   TRUE,
	   0,0,
	   widget->allocation.width,
	   widget->allocation.height); */


	return FALSE;
}


/* 
 * Create a new backing pixmap of the appropriate size 
 */
static gint configTable(GtkWidget * widget, GdkEventConfigure * event,
			gpointer data)
{


	if (playArea->tableBuf == NULL) {
		playArea->tableBuf = gdk_pixmap_new(widget->window,
						    widget->allocation.
						    width,
						    widget->allocation.
						    height, -1);

		gdk_draw_rectangle(playArea->tableBuf,
				   widget->style->bg_gc[GTK_STATE_NORMAL],
				   TRUE,
				   0, 0,
				   widget->allocation.width,
				   widget->allocation.height);

	}

	return TRUE;

}


/*
 * Redraw the screen from the backing pixmap 
 */
static gint exposeTable(GtkWidget * widget, GdkEventExpose * event,
			gpointer data)
{

	gdk_draw_drawable(widget->window,
			  widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
			  playArea->tableBuf,
			  event->area.x, event->area.y,
			  event->area.x, event->area.y,
			  event->area.width, event->area.height);

	return FALSE;
}


/*
 * Determine played card from hand
 */
static gint ReadCard(GtkWidget * widget, GdkEventButton * event)
{

	int status;
	int x = (int)(event->x);
	int handIndex;

	if (event->button == 1 && playArea->handBuf != NULL) {

		/* If it's our turn to play a card... */
		if (gameState.gameSegment == ST_GET_TRICK &&
		    gameState.curPlayer == gameState.playerId) {

			/* Put handIndex in the range of 0-12 */
			handIndex = (x - (x % 20)) / 20;
			if (handIndex > 12) {
				handIndex = 12;
			}

			/* Back up one if we get an "already played" */
			while (handIndex > 0
			       && gameState.hand[handIndex] ==
			       BLANK_CARD) {
				handIndex--;
			}

			/* Don't allow negative numbers */
			if (handIndex < 0) {
				handIndex = 0;
			}

			/* If we're not in the right card, go back to an invalid one */
			if (x - (handIndex * 20) > CARDWIDTH) {
				handIndex++;
			}

			status = SendCard(handIndex);

			switch (status) {

			case -1:	/* Card already played */
				DisplayStatusLine
				    (_("You already played that card"));
				break;
			case -2:	/* Renege */
				DisplayStatusLine
				    (_
				     ("Hey, no cheating, you're not out yet"));
				break;
			case 0:	/* Card ok */
				DisplayPlayedCard(gameState.
						  hand[handIndex], 0, 0);
				HideCard(handIndex);
				UpdateGame();
				ClearStatusLine();
				break;
			}
		}	/* if it's our turn to play */
	}
	/* if left-click and drawing area != NULL */
	return TRUE;

}


/*
 * Determine played card from hand
 */
void ReadBid(GtkWidget * widget)
{

	int status;
	int bid =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));

#ifdef DEBUG
	g_printerr("%d is my bid\n", bid);
#endif

	if (gameState.gameSegment == ST_GET_BIDS &&
	    gameState.curPlayer == gameState.playerId) {

		gameState.bids[gameState.playerId] = bid;
		status = SendMyBid();

		switch (status) {

		case -1:	/* Invalid Bid */
			DisplayStatusLine(_("Invalid Bid"));
			break;
		case -2:	/* Bid less than minimum */
			DisplayStatusLine
			    (_("Your team must bid at least the minimum"));
			break;
		default:	/* Bid accepted */
			DisplayTallys();
			UpdateGame();
			if (gameState.gameSegment == ST_GET_LEAD) {
				DisplayScores();
			}
			gtk_widget_set_sensitive(playArea->bidButton,
						 FALSE);
			ClearStatusLine();
			break;
		}
	}
}


/*
 * Callback function to handle input events to bid spinbutton
 */
static gint SpinInput(GtkSpinButton * spin, gfloat * new_val)
{

	if (strcasecmp("nil", gtk_entry_get_text(GTK_ENTRY(spin))) == 0) {
		*new_val = (gfloat) (-1);
		/* This makes things *slightly* better under GTK2. */
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin),
					  (gfloat) (-1));
		return TRUE;
	} else {
		return FALSE;
	}
}


/*
 * Callback function to handle output events to bid spinbutton
 */
static gint SpinOutput(GtkSpinButton * spin)
{

	gint val = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin));

	if (val == -1) {
		if (strcasecmp(gtk_entry_get_text(GTK_ENTRY(spin)), "nil")) {
			gtk_entry_set_text(GTK_ENTRY(spin), "nil");
		}
		return TRUE;
	} else {
		return FALSE;
	}
}
