#include <gnome.h>


gboolean
on_msgbox_destroy_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_btnOK_clicked                       (GtkButton       *button,
                                        gpointer         user_data);
