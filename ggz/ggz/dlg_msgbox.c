#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "dlg_msgbox.h"

/* Local varibles */
GtkWidget *dlg_msgbox;

/* Local callbacks which no other file will call */
GtkWidget* create_msgbox (void);
void msgbox_ok(GtkButton * button, gpointer user_data);
void msgbox_close(GtkButton * button, gpointer user_data);


GtkWidget*
create_msgbox (void)
{
  GtkWidget *msgbox;
  GtkWidget *vbox6;
  GtkWidget *scrolledwindow2;
  GtkWidget *text2;
  GtkWidget *hbox13;
  GtkWidget *hbox14;
  GtkWidget *button6;
  GtkWidget *button7;

  msgbox = gtk_dialog_new ();
  gtk_object_set_data (GTK_OBJECT (msgbox), "msgbox", msgbox);
  gtk_widget_set_usize (msgbox, 260, 150);
  gtk_window_set_title (GTK_WINDOW (msgbox), "Message Box");
  gtk_window_set_policy (GTK_WINDOW (msgbox), FALSE, FALSE, FALSE);

  vbox6 = GTK_DIALOG (msgbox)->vbox;
  gtk_object_set_data (GTK_OBJECT (msgbox), "vbox6", vbox6);
  gtk_widget_show (vbox6);

  scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow2);
  gtk_object_set_data_full (GTK_OBJECT (msgbox), "scrolledwindow2", scrolledwindow2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow2);
  gtk_box_pack_start (GTK_BOX (vbox6), scrolledwindow2, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow2), 5);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow2), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  text2 = gtk_text_new (NULL, NULL);
  gtk_widget_ref (text2);
  gtk_object_set_data_full (GTK_OBJECT (msgbox), "text2", text2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (text2);
  gtk_container_add (GTK_CONTAINER (scrolledwindow2), text2);

  hbox13 = GTK_DIALOG (msgbox)->action_area;
  gtk_object_set_data (GTK_OBJECT (msgbox), "hbox13", hbox13);
  gtk_widget_show (hbox13);
  gtk_container_set_border_width (GTK_CONTAINER (hbox13), 10);

  hbox14 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox14);
  gtk_object_set_data_full (GTK_OBJECT (msgbox), "hbox14", hbox14,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox14);
  gtk_box_pack_start (GTK_BOX (hbox13), hbox14, FALSE, FALSE, 0);

  button6 = gtk_button_new_with_label ("OK");
  gtk_widget_ref (button6);
  gtk_object_set_data_full (GTK_OBJECT (msgbox), "button6", button6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button6);
  gtk_box_pack_start (GTK_BOX (hbox14), button6, FALSE, FALSE, 0);
  gtk_widget_set_usize (button6, 75, -2);

  button7 = gtk_button_new_with_label ("Close");
  gtk_widget_ref (button7);
  gtk_object_set_data_full (GTK_OBJECT (msgbox), "button7", button7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button7);
  gtk_box_pack_start (GTK_BOX (hbox14), button7, FALSE, FALSE, 0);
  gtk_widget_set_usize (button7, 75, -2);

  gtk_signal_connect (GTK_OBJECT (button6), "clicked",
                      GTK_SIGNAL_FUNC (msgbox_ok),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (button7), "clicked",
                      GTK_SIGNAL_FUNC (msgbox_close),
                      NULL);

  return msgbox;
}

  
/*                              *
 *           Callbacks          *
 *                              */
  
void msgbox_ok(GtkButton * button, gpointer user_data)
{ 
        gtk_widget_destroy(dlg_msgbox);
        dlg_msgbox = NULL;
}

void msgbox_close(GtkButton * button, gpointer user_data)
{ 
	/* FIXME: Add dissconnect code here */

        gtk_widget_destroy(dlg_msgbox);
        dlg_msgbox = NULL;
}

void msg(int mode, char *title, char *message)
{
	GtkWidget *temp_widget;

	dlg_msgbox = create_msgbox();

	if (mode==0){
		temp_widget = gtk_object_get_data(GTK_OBJECT(dlg_msgbox), "button7"); 
		gtk_widget_hide(temp_widget);
	}else{
		temp_widget = gtk_object_get_data(GTK_OBJECT(dlg_msgbox), "button6"); 
		gtk_widget_hide(temp_widget);
	}		

	temp_widget = gtk_object_get_data(GTK_OBJECT(dlg_msgbox), "msgbox"); 
	gtk_window_set_title (GTK_WINDOW (temp_widget), title);
	temp_widget = gtk_object_get_data(GTK_OBJECT(dlg_msgbox), "text2"); 
	gtk_text_insert (GTK_TEXT (temp_widget), NULL, NULL, NULL, message, -1);
	gtk_widget_show(dlg_msgbox);
}
