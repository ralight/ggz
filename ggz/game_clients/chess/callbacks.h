#include <gtk/gtk.h>


void
on_exit_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_board_configure_event               (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

gboolean
on_board_expose_event                  (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

gboolean
on_board_button_press_event            (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_board_drag_begin                    (GtkWidget       *widget,
                                        GdkDragContext  *drag_context,
                                        gpointer         user_data);

gboolean
on_board_drag_motion                   (GtkWidget       *widget,
                                        GdkDragContext  *drag_context,
                                        gint             x,
                                        gint             y,
                                        guint            time,
                                        gpointer         user_data);

gboolean
on_board_drag_drop                     (GtkWidget       *widget,
                                        GdkDragContext  *drag_context,
                                        gint             x,
                                        gint             y,
                                        guint            time,
                                        gpointer         user_data);
