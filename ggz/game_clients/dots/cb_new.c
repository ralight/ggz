#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "main.h"
#include "game.h"
#include "cb_new.h"
#include "dlg_new.h"
#include "support.h"

static gboolean allow_dlg_new_delete = FALSE;

gboolean
on_dlg_new_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	/* Do not allow this dialog to close until they choose */
	if(allow_dlg_new_delete)
		return FALSE;
	else
		return TRUE;
}


void
on_new_btn_yes_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
	handle_newgame(TRUE);
}


void
on_new_btn_no_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	handle_newgame(FALSE);
	gtk_main_quit();
}

void handle_newgame(gboolean opt)
{
	if(opt == TRUE)
		handle_req_newgame();

	allow_dlg_new_delete = TRUE;
}
