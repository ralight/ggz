#include <gtk/gtk.h>


gboolean
on_main_window_configure_event         (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

void
on_main_window_draw                    (GtkWidget       *widget,
                                        GdkRectangle    *area,
                                        gpointer         user_data);

gboolean
on_main_window_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_mainarea_expose_event               (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);
void
on_exit_menu_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);


gboolean
main_window_exit                       (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);


gboolean
on_mainarea_configure_event            (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

gboolean
on_mainarea_button_press_event         (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
callback_widget_set_enabled (char *name, gboolean mode);

void
on_send_setup_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_request_sync_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_hide_enemy_units1_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save_map_menu_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_show_game_options_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
