#include <gtk/gtk.h>


void
on_opt_btn_ok_clicked                  (GtkButton       *button,
                                        gpointer         user_data);


gboolean
on_dlg_opt_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);
