#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "cb_bid.h"
#include "dlg_bid.h"
#include "support.h"
#include "game.h"


void
on_dlg_bid_submit_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	int bid;

	bid = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(dlg_bid_spin));
	game_send_bid(bid);

	gtk_widget_hide(dlg_bid_fixed);
}

