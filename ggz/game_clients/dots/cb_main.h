#include <gtk/gtk.h>

void
on_mnu_exit_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_mnu_preferences_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_mnu_about_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_window_delete_event                 (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_board_expose_event                  (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

gboolean
on_board_button_press_event            (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_p1b_expose_event                    (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

gboolean
on_p2b_expose_event                    (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);
