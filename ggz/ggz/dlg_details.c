#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "dlg_details.h"


GtkWidget*
create_dlg_details (void)
{
  GtkWidget *dlg_details;
  GtkWidget *dialog_vbox1;
  GtkWidget *scrolled_window;
  GtkWidget *text;
  GtkWidget *dialog_action_area1;
  GtkWidget *hbuttonbox1;
  GtkWidget *cancel_button;

  dlg_details = gtk_dialog_new ();
  gtk_object_set_data (GTK_OBJECT (dlg_details), "dlg_details", dlg_details);
  gtk_window_set_title (GTK_WINDOW (dlg_details), "Connection Messages");
  gtk_window_set_position (GTK_WINDOW (dlg_details), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size (GTK_WINDOW (dlg_details), 400, 300);
  gtk_window_set_policy (GTK_WINDOW (dlg_details), TRUE, TRUE, FALSE);

  dialog_vbox1 = GTK_DIALOG (dlg_details)->vbox;
  gtk_object_set_data (GTK_OBJECT (dlg_details), "dialog_vbox1", dialog_vbox1);
  gtk_widget_show (dialog_vbox1);

  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolled_window);
  gtk_object_set_data_full (GTK_OBJECT (dlg_details), "scrolled_window", scrolled_window,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolled_window);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), scrolled_window, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 10);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  text = gtk_text_new (NULL, NULL);
  gtk_widget_ref (text);
  gtk_object_set_data_full (GTK_OBJECT (dlg_details), "text", text,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (text);
  gtk_container_add (GTK_CONTAINER (scrolled_window), text);

  dialog_action_area1 = GTK_DIALOG (dlg_details)->action_area;
  gtk_object_set_data (GTK_OBJECT (dlg_details), "dialog_action_area1", dialog_action_area1);
  gtk_widget_show (dialog_action_area1);
  gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area1), 10);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_details), "hbuttonbox1", hbuttonbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (dialog_action_area1), hbuttonbox1, TRUE, TRUE, 0);

  cancel_button = gtk_button_new_with_label ("Cancel");
  gtk_widget_ref (cancel_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_details), "cancel_button", cancel_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cancel_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), cancel_button);
  GTK_WIDGET_SET_FLAGS (cancel_button, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (cancel_button), "clicked",
                      GTK_SIGNAL_FUNC (details_cancel_details),
                      NULL);

  return dlg_details;
}

