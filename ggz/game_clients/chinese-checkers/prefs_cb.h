#include <gtk/gtk.h>

void
on_ok_button_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_apply_button_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_cancel_button_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_theme_list_select_row               (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);
