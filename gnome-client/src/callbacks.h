#include <gnome.h>


gboolean
on_window_destroy_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);
gboolean
on_window_delete_event                (GtkWidget       *widget,
                                       GdkEvent        *event,
                                       gpointer         user_data);

void
on_entLogin_changed			(GtkEntry	*widget,
					 gpointer	*user_data);

void
on_btnLogin_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnProfiles_clicked                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnNewCreate_clicked                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnNewCancel_clicked                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnNewAdvanced_clicked                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_tree_selection_changed		(GtkTreeSelection *selection,
					 gpointer data);

gint
on_player_tree_timeout                  (gpointer         user_data);

gboolean
on_player_tree_motion_notify_event	(GtkWidget	*widget,
					 GdkEventMotion  *event,
					 gpointer	*user_data);

gboolean
on_player_tree_enter_notify_event	(GtkWidget	*widget,
					 GdkEventCrossing *event,
					 gpointer	*user_data);

gboolean
on_player_tree_leave_notify_event	(GtkWidget	*widget,
					 GdkEventCrossing *event,
					 gpointer	*user_data);

void
on_btnChatChange_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnChatGame_clicked               (GtkButton       *button,
                                      gpointer         user_data);

void
on_btnChatLogoff_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnChatChat_clicked               (GtkButton       *button,
                                      gpointer         user_data);

GtkWidget*
create_custChatXText (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2);

void
on_btnChatSend_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnLaunch_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnJoin_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_btnWatch_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_entCharNew_activate                 (GtkEntry	*entry,
		                        gpointer	 user_data);

gboolean
on_room_click                             (GtkWidget       *widget,
					GdkEventButton  *event,
					gpointer         user_data);

gint
on_pmStar_timeout                  (gpointer         user_data);

gint
on_general_timeout                  (gpointer         user_data);

void
on_pmBackground_draw                   (GtkWidget       *widget,
                                        GdkRectangle    *area,
                                        gpointer         user_data);

gboolean
on_pmBackground_expose_event           (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);
