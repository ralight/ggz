#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "dlg_motd.h"
#include "support.h"

/* Global GtkWidget for this dialog */
GtkWidget *dlg_motd;

extern GtkWidget *main_win;

/* Local callbacks which no other file will call */
void motd_ok(GtkButton * button, gpointer user_data);
void motd_realize(GtkButton * button, gpointer user_data);

void motd_ok(GtkButton * button, gpointer user_data)
{
	GtkWidget *tmp;

        gtk_widget_destroy(dlg_motd);
        dlg_motd = NULL;
        tmp = gtk_object_get_data(GTK_OBJECT(main_win), "motd");
        gtk_widget_set_sensitive(GTK_WIDGET(tmp),TRUE);
}
 
void motd_realize(GtkButton * button, gpointer user_data)
{
        GtkWidget *tmp;
        
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_motd), "text1");
        gtk_text_set_word_wrap(GTK_TEXT(tmp), TRUE);
}


GtkWidget*
create_dlgMOTD (void)
{
  GtkWidget *dlgMOTD;
  GtkWidget *vbox2;
  GtkWidget *scrolledwindow1;
  GtkWidget *text1;
  GtkWidget *button3;

  dlgMOTD = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_object_set_data (GTK_OBJECT (dlgMOTD), "dlgMOTD", dlgMOTD);
  gtk_widget_set_usize (dlgMOTD, 300, 350);
  gtk_window_set_title (GTK_WINDOW (dlgMOTD), _("MOTD"));
  gtk_window_set_policy (GTK_WINDOW (dlgMOTD), FALSE, FALSE, FALSE);

  vbox2 = gtk_vbox_new (FALSE, 5);
  gtk_widget_ref (vbox2);
  gtk_object_set_data_full (GTK_OBJECT (dlgMOTD), "vbox2", vbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (dlgMOTD), vbox2);
  gtk_container_set_border_width (GTK_CONTAINER (vbox2), 5);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow1);
  gtk_object_set_data_full (GTK_OBJECT (dlgMOTD), "scrolledwindow1", scrolledwindow1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (vbox2), scrolledwindow1, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  text1 = gtk_text_new (NULL, NULL);
  gtk_widget_ref (text1);
  gtk_object_set_data_full (GTK_OBJECT (dlgMOTD), "text1", text1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (text1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), text1);

  button3 = gtk_button_new_with_label (_("OK"));
  gtk_widget_ref (button3);
  gtk_object_set_data_full (GTK_OBJECT (dlgMOTD), "button3", button3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button3);
  gtk_box_pack_start (GTK_BOX (vbox2), button3, FALSE, FALSE, 0);
  GTK_WIDGET_SET_FLAGS (button3, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (dlgMOTD), "realize",
                      GTK_SIGNAL_FUNC (motd_realize),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (button3), "clicked",
                      GTK_SIGNAL_FUNC (motd_ok),
                      NULL);

  gtk_widget_grab_default (button3);
  return dlgMOTD;
}

