#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>

#include "profilesc.h"
#include "profilesi.h"
#include "support.h"


void
on_entries_changed                     (GtkEditable     *editable,
                                        gpointer         user_data)
{

}


void
on_btnNew_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_btnApply_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_btnDelete_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_btnDone_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy (GTK_WIDGET (user_data));
}

