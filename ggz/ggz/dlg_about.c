#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "dlg_about.h"


/* Globals for this dialog */
GtkWidget *dlg_about;
extern GtkWidget *main_win;


/* Local Functions */
static void about_close(GtkWidget* widget, gpointer data);

void about_close(GtkWidget* widget, gpointer data)
{
	GtkWidget *tmp;

	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "about");
        gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);

	gtk_widget_destroy(dlg_about);
}



GtkWidget*
create_dlg_about (void)
{
  GtkWidget *dlg_about;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *dialog_action_area1;
  GtkWidget *hbox1;
  GtkWidget *ok_button;

  dlg_about = gtk_dialog_new ();
  gtk_object_set_data (GTK_OBJECT (dlg_about), "dlg_about", dlg_about);
  gtk_window_set_title (GTK_WINDOW (dlg_about), "About");
  gtk_window_set_policy (GTK_WINDOW (dlg_about), FALSE, FALSE, FALSE);

  dialog_vbox1 = GTK_DIALOG (dlg_about)->vbox;
  gtk_object_set_data (GTK_OBJECT (dlg_about), "dialog_vbox1", dialog_vbox1);
  gtk_widget_show (dialog_vbox1);
  gtk_widget_set_usize (dialog_vbox1, 310, 277);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_about), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, FALSE, FALSE, 0);

  label1 = gtk_label_new ("GNU-Gaming Zone");
  gtk_widget_ref (label1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_about), "label1", label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (vbox1), label1, FALSE, FALSE, 8);

  label2 = gtk_label_new ("Authors:\n\t\tGTK+ Version\n\t\t\t\t\t\t\t\tRich Gade\t\t\t\t\t\t\t\t\t(rgade@crown.net)\n\t\t\t\t\t\t\t\tBrent Henricks\t\t(brentmh@rice.edu)\n\t\t\t\t\t\t\t\tJustin Zaun\t\t\t\t\t\t\t(jzaun@telerama.com)\n\n\t\tWindows Version\n\t\t\t\t\t\t\t\tDoug Hudson\t\t\t(hudson2@pilot.msu.edu)\n\nWebsite:\n\t\t\t\t\t\t\t\tggz.sourceforge.net\n");
  gtk_widget_ref (label2);
  gtk_object_set_data_full (GTK_OBJECT (dlg_about), "label2", label2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (vbox1), label2, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label2), 0.25, 5.21541e-08);

  dialog_action_area1 = GTK_DIALOG (dlg_about)->action_area;
  gtk_object_set_data (GTK_OBJECT (dlg_about), "dialog_action_area1", dialog_action_area1);
  gtk_widget_show (dialog_action_area1);
  gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area1), 10);

  hbox1 = gtk_hbox_new (TRUE, 0);
  gtk_widget_ref (hbox1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_about), "hbox1", hbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (dialog_action_area1), hbox1, TRUE, TRUE, 0);

  ok_button = gtk_button_new_with_label ("OK");
  gtk_widget_ref (ok_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_about), "ok_button", ok_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ok_button);
  gtk_box_pack_start (GTK_BOX (hbox1), ok_button, FALSE, FALSE, 0);
  gtk_widget_set_usize (ok_button, 47, -2);
  GTK_WIDGET_SET_FLAGS (ok_button, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (ok_button), "clicked",
                      GTK_SIGNAL_FUNC (about_close),
                      NULL);

  gtk_widget_grab_focus (ok_button);
  gtk_widget_grab_default (ok_button);
  return dlg_about;
}

