#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdio.h>

#include "cb_opt.h"
#include "dlg_opt.h"
#include "support.h"
#include "main.h"
#include "game.h"


static gboolean allow_dlg_opt_delete = FALSE;

void
on_opt_btn_ok_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *sb_x, *sb_y;

	sb_x = gtk_object_get_data(GTK_OBJECT(opt_dialog), "opt_spin_x");
	sb_y = gtk_object_get_data(GTK_OBJECT(opt_dialog), "opt_spin_y");

	/* Get the number of squares wide and high */
	board_width = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(sb_x));
	board_height = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(sb_y));

	/* Convert from squares to dots */
	board_width++;
	board_height++;

	allow_dlg_opt_delete = TRUE;

	if(send_options() < 0)
		gtk_main_quit();

	game_init();
}


gboolean
on_dlg_opt_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	/* Do not allow this dialog to close before we've sent the options */
	if(allow_dlg_opt_delete)
		return FALSE;
	else
		return TRUE;
}

