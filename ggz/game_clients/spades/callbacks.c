#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <client.h>
#include <gtk_connect.h>
#include <gtk_dlg_options.h>
#include <gtk_dlg_error.h>
#include <options.h>

#include "callbacks.h"


GtkWidget *detail_window = NULL;
extern gameState_t gameState;
extern option_t options;


void launch_game(GtkButton * button, gpointer user_data)
{
	char ai;
	input_t *in = (input_t *) user_data;

	options.endGame =
		gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(in->endGame));
	options.minBid =
		gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(in->minBid));

	ai = (options.bitOpt & MSK_AI) >> 1;

	CheckWriteInt(gameState.spadesSock, sizeof(options));
	write(gameState.spadesSock, &options, sizeof(options));
	write(gameState.spadesSock, &ai, 1);	/* AI players */	
	g_printerr("Sent options\n");
}



