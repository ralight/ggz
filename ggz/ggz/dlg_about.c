#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "dlg_about.h"
#include "support.h"

/* Globals for this dialog */
extern GtkWidget *main_win;


/* Local Functions */
static void about_close(GtkWidget* widget, gpointer data);


void about_close(GtkWidget* widget, gpointer data)
{
	GtkWidget *tmp;

	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "about");
        gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);

	gtk_widget_destroy(widget);
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
  GtkWidget *button_box;
  GtkWidget *ok_button;

  dlg_about = gtk_dialog_new ();
  gtk_object_set_data (GTK_OBJECT (dlg_about), "dlg_about", dlg_about);
  gtk_window_set_title (GTK_WINDOW (dlg_about), _("About"));
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

  label1 = gtk_label_new (_("GNU Gaming Zone"));
  gtk_widget_ref (label1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_about), "label1", label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (vbox1), label1, FALSE, FALSE, 8);

  label2 = gtk_label_new (_("Authors:\n\t\tGTK+ Version\n\t\t\t\t\t\t\t\tRich Gade\t\t\t\t\t\t\t\t\t(rgade@users.sourceforge.net)\n\t\t\t\t\t\t\t\tBrent Hendricks\t\t(bmh@users.sourceforge.net)\n\t\t\t\t\t\t\t\tJustin Zaun\t\t\t\t\t\t\t(jzaun@users.sourceforge.net)\n\n\t\tWindows Version\n\t\t\t\t\t\t\t\tDoug Hudson\t\t\t(djh@users.sourceforge.net)\n\nWebsite:\n\t\t\t\t\t\t\t\thttp://ggz.sourceforge.net\n"));
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

  button_box = gtk_hbutton_box_new ();
  gtk_widget_ref (button_box);
  gtk_object_set_data_full (GTK_OBJECT (dlg_about), "button_box", button_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button_box);
  gtk_box_pack_start (GTK_BOX (dialog_action_area1), button_box, TRUE, TRUE, 0);

  ok_button = gtk_button_new_with_label (_("OK"));
  gtk_widget_ref (ok_button);
  gtk_object_set_data_full (GTK_OBJECT (dlg_about), "ok_button", ok_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ok_button);
  gtk_container_add (GTK_CONTAINER (button_box), ok_button);
  GTK_WIDGET_SET_FLAGS (ok_button, GTK_CAN_DEFAULT);

  gtk_signal_connect_object (GTK_OBJECT (dlg_about), "delete_event",
                             GTK_SIGNAL_FUNC (about_close),
                             GTK_OBJECT (dlg_about));
  gtk_signal_connect_object (GTK_OBJECT (ok_button), "clicked",
                             GTK_SIGNAL_FUNC (about_close),
                             GTK_OBJECT (dlg_about));

  return dlg_about;
}


