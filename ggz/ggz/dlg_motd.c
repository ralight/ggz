#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "dlg_motd.h"

/* Global GtkWidget for this dialog */
GtkWidget *dlg_motd;

extern GtkWidget *main_win;

/* Local callbacks which no other file will call */
void motd_ok(GtkButton * button, gpointer user_data);


GtkWidget*
create_dlgMOTD (void)
{
  GtkWidget *dlgMOTD;
  GtkWidget *vbox1;
  GtkWidget *scrolledwindow1;
  GtkWidget *txtMOTD;
  GtkWidget *btnOK;

  dlgMOTD = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_object_set_data (GTK_OBJECT (dlgMOTD), "dlgMOTD", dlgMOTD);
  gtk_widget_set_usize (dlgMOTD, 300, 350);
  gtk_window_set_title (GTK_WINDOW (dlgMOTD), "MOTD");
  gtk_window_set_policy (GTK_WINDOW (dlgMOTD), FALSE, FALSE, FALSE);

  vbox1 = gtk_vbox_new (FALSE, 5);
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (dlgMOTD), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (dlgMOTD), vbox1);
  gtk_container_set_border_width (GTK_CONTAINER (vbox1), 5);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow1);
  gtk_object_set_data_full (GTK_OBJECT (dlgMOTD), "scrolledwindow1", scrolledwindow1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  txtMOTD = gtk_text_new (NULL, NULL);
  gtk_widget_ref (txtMOTD);
  gtk_object_set_data_full (GTK_OBJECT (dlgMOTD), "txtMOTD", txtMOTD,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (txtMOTD);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), txtMOTD);

  btnOK = gtk_button_new_with_label ("OK");
  gtk_widget_ref (btnOK);
  gtk_object_set_data_full (GTK_OBJECT (dlgMOTD), "btnOK", btnOK,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnOK);
  gtk_box_pack_start (GTK_BOX (vbox1), btnOK, FALSE, FALSE, 0);

  gtk_widget_grab_focus (btnOK);

  gtk_signal_connect (GTK_OBJECT (btnOK), "clicked",
                      GTK_SIGNAL_FUNC (motd_ok),
                      NULL);

  return dlgMOTD;
}
 
  
/*                              *
 *           Callbacks          *
 *                              */
 
void motd_ok(GtkButton * button, gpointer user_data)
{
	GtkWidget *tmp;

        gtk_widget_destroy(dlg_motd);
        dlg_motd = NULL;
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "motd");
        gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
}
 

