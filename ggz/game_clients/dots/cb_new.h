#include <gtk/gtk.h>


gboolean
on_dlg_new_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_new_btn_yes_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_new_btn_no_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void handle_newgame(gboolean);
