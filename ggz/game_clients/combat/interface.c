/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

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
#include "interface.h"
#include "support.h"

GtkWidget*
create_main_window (void)
{
  GtkWidget *main_window;
  GtkWidget *vbox;
  GtkWidget *menubar;
  guint tmp_key;
  GtkWidget *file_menu;
  GtkWidget *file_menu_menu;
  GtkAccelGroup *file_menu_menu_accels;
  GtkWidget *exit_menu;
  GtkWidget *hbox;
  GtkWidget *mainarea;
  GtkWidget *vseparator1;
  GtkWidget *player_box;
  GtkWidget *statusbar1;
  GtkAccelGroup *accel_group;

  accel_group = gtk_accel_group_new ();

  main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_name (main_window, "main_window");
  gtk_object_set_data (GTK_OBJECT (main_window), "main_window", main_window);
  gtk_window_set_title (GTK_WINDOW (main_window), _("Combat"));

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_set_name (vbox, "vbox");
  gtk_widget_ref (vbox);
  gtk_object_set_data_full (GTK_OBJECT (main_window), "vbox", vbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox);
  gtk_container_add (GTK_CONTAINER (main_window), vbox);

  menubar = gtk_menu_bar_new ();
  gtk_widget_set_name (menubar, "menubar");
  gtk_widget_ref (menubar);
  gtk_object_set_data_full (GTK_OBJECT (main_window), "menubar", menubar,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (menubar);
  gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, FALSE, 0);

  file_menu = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (file_menu)->child),
                                   _("_File"));
  gtk_widget_add_accelerator (file_menu, "activate_item", accel_group,
                              tmp_key, GDK_MOD1_MASK, 0);
  gtk_widget_set_name (file_menu, "file_menu");
  gtk_widget_ref (file_menu);
  gtk_object_set_data_full (GTK_OBJECT (main_window), "file_menu", file_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (file_menu);
  gtk_container_add (GTK_CONTAINER (menubar), file_menu);

  file_menu_menu = gtk_menu_new ();
  gtk_widget_set_name (file_menu_menu, "file_menu_menu");
  gtk_widget_ref (file_menu_menu);
  gtk_object_set_data_full (GTK_OBJECT (main_window), "file_menu_menu", file_menu_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (file_menu), file_menu_menu);
  file_menu_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (file_menu_menu));

  exit_menu = gtk_menu_item_new_with_label ("");
  tmp_key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (exit_menu)->child),
                                   _("E_xit"));
  gtk_widget_add_accelerator (exit_menu, "activate_item", file_menu_menu_accels,
                              tmp_key, 0, 0);
  gtk_widget_set_name (exit_menu, "exit_menu");
  gtk_widget_ref (exit_menu);
  gtk_object_set_data_full (GTK_OBJECT (main_window), "exit_menu", exit_menu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (exit_menu);
  gtk_container_add (GTK_CONTAINER (file_menu_menu), exit_menu);

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_set_name (hbox, "hbox");
  gtk_widget_ref (hbox);
  gtk_object_set_data_full (GTK_OBJECT (main_window), "hbox", hbox,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);

  mainarea = gtk_drawing_area_new ();
  gtk_widget_set_name (mainarea, "mainarea");
  gtk_widget_ref (mainarea);
  gtk_object_set_data_full (GTK_OBJECT (main_window), "mainarea", mainarea,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mainarea);
  gtk_box_pack_start (GTK_BOX (hbox), mainarea, FALSE, FALSE, 0);
  gtk_widget_set_usize (mainarea, 490, 490);

  vseparator1 = gtk_vseparator_new ();
  gtk_widget_set_name (vseparator1, "vseparator1");
  gtk_widget_ref (vseparator1);
  gtk_object_set_data_full (GTK_OBJECT (main_window), "vseparator1", vseparator1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vseparator1);
  gtk_box_pack_start (GTK_BOX (hbox), vseparator1, FALSE, FALSE, 7);
  gtk_widget_set_usize (vseparator1, 2, -2);

  player_box = gtk_vbox_new (FALSE, 0);
  gtk_widget_set_name (player_box, "player_box");
  gtk_widget_ref (player_box);
  gtk_object_set_data_full (GTK_OBJECT (main_window), "player_box", player_box,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (player_box);
  gtk_box_pack_start (GTK_BOX (hbox), player_box, TRUE, TRUE, 0);
  gtk_widget_set_usize (player_box, 180, -2);

  statusbar1 = gtk_statusbar_new ();
  gtk_widget_set_name (statusbar1, "statusbar1");
  gtk_widget_ref (statusbar1);
  gtk_object_set_data_full (GTK_OBJECT (main_window), "statusbar1", statusbar1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (statusbar1);
  gtk_box_pack_start (GTK_BOX (vbox), statusbar1, FALSE, FALSE, 0);

  gtk_signal_connect (GTK_OBJECT (main_window), "configure_event",
                      GTK_SIGNAL_FUNC (on_main_window_configure_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (main_window), "delete_event",
                      GTK_SIGNAL_FUNC (main_window_exit),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (main_window), "destroy_event",
                      GTK_SIGNAL_FUNC (main_window_exit),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (exit_menu), "activate",
                      GTK_SIGNAL_FUNC (on_exit_menu_activate),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (mainarea), "expose_event",
                      GTK_SIGNAL_FUNC (on_mainarea_expose_event),
                      NULL);

  gtk_window_add_accel_group (GTK_WINDOW (main_window), accel_group);

  return main_window;
}

