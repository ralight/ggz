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
#include "dlg_launch.h"


GtkWidget*
create_dlgLaunch (void)
{
  GtkWidget *dlgLaunch;
  GtkWidget *vbox1;
  GtkWidget *hbox1;
  GtkWidget *label1;
  GtkWidget *combo1;
  GtkWidget *combo_entry1;
  GtkWidget *table1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *label5;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkWidget *label8;
  GtkWidget *label9;
  GtkWidget *label10;
  GtkWidget *label11;
  GtkWidget *label12;
  GtkWidget *label13;
  GtkWidget *combo2;
  GList *combo2_items = NULL;
  GtkWidget *combo_entry2;
  GtkWidget *combo3;
  GList *combo3_items = NULL;
  GtkWidget *combo_entry3;
  GtkWidget *combo4;
  GList *combo4_items = NULL;
  GtkWidget *combo_entry4;
  GtkWidget *combo5;
  GList *combo5_items = NULL;
  GtkWidget *combo_entry5;
  GtkWidget *combo6;
  GList *combo6_items = NULL;
  GtkWidget *combo_entry6;
  GtkWidget *combo7;
  GList *combo7_items = NULL;
  GtkWidget *combo_entry7;
  GtkWidget *combo8;
  GList *combo8_items = NULL;
  GtkWidget *combo_entry8;
  GtkWidget *combo9;
  GList *combo9_items = NULL;
  GtkWidget *combo_entry9;
  GtkWidget *combo10;
  GList *combo10_items = NULL;
  GtkWidget *combo_entry10;
  GtkWidget *lblUser;
  GtkWidget *togglebutton1;
  GtkWidget *togglebutton2;
  GtkWidget *togglebutton3;
  GtkWidget *togglebutton4;
  GtkWidget *togglebutton5;
  GtkWidget *togglebutton6;
  GtkWidget *togglebutton7;
  GtkWidget *togglebutton8;
  GtkWidget *togglebutton9;
  GtkWidget *hbox3;
  GtkWidget *btnLaunch;
  GtkWidget *btnCancel;

  dlgLaunch = gtk_window_new (GTK_WINDOW_DIALOG); 
  gtk_object_set_data (GTK_OBJECT (dlgLaunch), "dlgLaunch", dlgLaunch);
  gtk_window_set_title (GTK_WINDOW (dlgLaunch), "Launch Game");
  gtk_window_set_policy (GTK_WINDOW (dlgLaunch), FALSE, FALSE, FALSE);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1); 
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1); 
  gtk_container_add (GTK_CONTAINER (dlgLaunch), vbox1);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox1);  
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "hbox1", hbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox1); 
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, FALSE, TRUE, 5);

  label1 = gtk_label_new ("Game Type: ");
  gtk_widget_ref (label1);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "label1", label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (hbox1), label1, FALSE, TRUE, 5);
  combo1 = gtk_combo_new ();
  gtk_widget_ref (combo1);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "combo1", combo1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo1);
  gtk_box_pack_start (GTK_BOX (hbox1), combo1, TRUE, TRUE, 5);
                            
  combo_entry1 = GTK_COMBO (combo1)->entry;
  gtk_widget_ref (combo_entry1);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "combo_entry1", combo_entry1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo_entry1);

  table1 = gtk_table_new (11, 3, FALSE);
  gtk_widget_ref (table1); 
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "table1", table1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox1), table1, FALSE, TRUE, 0);

  label2 = gtk_label_new ("1");
  gtk_widget_ref (label2); 
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "label2", label2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label2); 
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  label3 = gtk_label_new ("2");
  gtk_widget_ref (label3);  
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "label3", label3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 2, 3,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  label4 = gtk_label_new ("3");
  gtk_widget_ref (label4);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "label4", label4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table1), label4, 0, 1, 3, 4,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  label5 = gtk_label_new ("4");
  gtk_widget_ref (label5);  
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "label5", label5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label5);
  gtk_table_attach (GTK_TABLE (table1), label5, 0, 1, 4, 5,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  label6 = gtk_label_new ("5");
  gtk_widget_ref (label6); 
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "label6", label6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label6);
  gtk_table_attach (GTK_TABLE (table1), label6, 0, 1, 5, 6,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  label7 = gtk_label_new ("6");
  gtk_widget_ref (label7); 
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "label7", label7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label7);
  gtk_table_attach (GTK_TABLE (table1), label7, 0, 1, 6, 7,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  label8 = gtk_label_new ("7");
  gtk_widget_ref (label8); 
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "label8", label8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label8);
  gtk_table_attach (GTK_TABLE (table1), label8, 0, 1, 7, 8,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  label9 = gtk_label_new ("8");
  gtk_widget_ref (label9); 
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "label9", label9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label9);
  gtk_table_attach (GTK_TABLE (table1), label9, 0, 1, 8, 9,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  label10 = gtk_label_new ("9");
  gtk_widget_ref (label10);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "label10", label10,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label10);
  gtk_table_attach (GTK_TABLE (table1), label10, 0, 1, 9, 10,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  label11 = gtk_label_new ("10");
  gtk_widget_ref (label11);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "label11", label11,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label11);
  gtk_table_attach (GTK_TABLE (table1), label11, 0, 1, 10, 11,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  label12 = gtk_label_new ("Players");
  gtk_widget_ref (label12);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "label12", label12,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label12);
  gtk_table_attach (GTK_TABLE (table1), label12, 1, 2, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  label13 = gtk_label_new ("  R  ");
  gtk_widget_ref (label13); 
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "label13", label13,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label13);
  gtk_table_attach (GTK_TABLE (table1), label13, 2, 3, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  combo2 = gtk_combo_new ();
  gtk_widget_ref (combo2);  
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "combo2", combo2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo2);
  gtk_table_attach (GTK_TABLE (table1), combo2, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  combo2_items = g_list_append (combo2_items, "Closed");
  combo2_items = g_list_append (combo2_items, "Human");
  combo2_items = g_list_append (combo2_items, "Computer");
  gtk_combo_set_popdown_strings (GTK_COMBO (combo2), combo2_items);
  g_list_free (combo2_items);
                    
  combo_entry2 = GTK_COMBO (combo2)->entry;
  gtk_widget_ref (combo_entry2);   
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "combo_entry2", combo_entry2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo_entry2);
  gtk_entry_set_text (GTK_ENTRY (combo_entry2), "Closed");

  combo3 = gtk_combo_new ();
  gtk_widget_ref (combo3);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "combo3", combo3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo3); 
  gtk_table_attach (GTK_TABLE (table1), combo3, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),  
                    (GtkAttachOptions) (0), 0, 0);
  combo3_items = g_list_append (combo3_items, "Closed");
  combo3_items = g_list_append (combo3_items, "Human");
  combo3_items = g_list_append (combo3_items, "Computer");
  gtk_combo_set_popdown_strings (GTK_COMBO (combo3), combo3_items);
  g_list_free (combo3_items);

  combo_entry3 = GTK_COMBO (combo3)->entry;
  gtk_widget_ref (combo_entry3);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "combo_entry3", combo_entry3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo_entry3);  
  gtk_entry_set_text (GTK_ENTRY (combo_entry3), "Closed");
                            
  combo4 = gtk_combo_new ();
  gtk_widget_ref (combo4);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "combo4", combo4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo4);
  gtk_table_attach (GTK_TABLE (table1), combo4, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),  
                    (GtkAttachOptions) (0), 0, 0);
  combo4_items = g_list_append (combo4_items, "Closed");
  combo4_items = g_list_append (combo4_items, "Human");
  combo4_items = g_list_append (combo4_items, "Computer");
  gtk_combo_set_popdown_strings (GTK_COMBO (combo4), combo4_items);
  g_list_free (combo4_items);

  combo_entry4 = GTK_COMBO (combo4)->entry;
  gtk_widget_ref (combo_entry4);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "combo_entry4", combo_entry4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo_entry4);
  gtk_entry_set_text (GTK_ENTRY (combo_entry4), "Closed");
                            
  combo5 = gtk_combo_new ();
  gtk_widget_ref (combo5);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "combo5", combo5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo5);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "combo5", combo5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo5);
  gtk_table_attach (GTK_TABLE (table1), combo5, 1, 2, 5, 6,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),  
                    (GtkAttachOptions) (0), 0, 0);
  combo5_items = g_list_append (combo5_items, "Closed");
  combo5_items = g_list_append (combo5_items, "Human");
  combo5_items = g_list_append (combo5_items, "Computer");
  gtk_combo_set_popdown_strings (GTK_COMBO (combo5), combo5_items);
  g_list_free (combo5_items);

  combo_entry5 = GTK_COMBO (combo5)->entry;
  gtk_widget_ref (combo_entry5);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "combo_entry5", combo_entry5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo_entry5);
  gtk_entry_set_text (GTK_ENTRY (combo_entry5), "Closed");
                            
  combo6 = gtk_combo_new ();
  gtk_widget_ref (combo6);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "combo6", combo6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo6);
  gtk_table_attach (GTK_TABLE (table1), combo6, 1, 2, 6, 7,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),  
                    (GtkAttachOptions) (0), 0, 0);
  combo6_items = g_list_append (combo6_items, "Closed");
  combo6_items = g_list_append (combo6_items, "Human");
  combo6_items = g_list_append (combo6_items, "Computer");
  gtk_combo_set_popdown_strings (GTK_COMBO (combo6), combo6_items);
  g_list_free (combo6_items);

  combo_entry6 = GTK_COMBO (combo6)->entry;
  gtk_widget_ref (combo_entry6);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "combo_entry6", combo_entry6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo_entry6);
  gtk_entry_set_text (GTK_ENTRY (combo_entry6), "Closed");
                            
  combo7 = gtk_combo_new ();
  gtk_widget_ref (combo7);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "combo7", combo7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo7);
  gtk_table_attach (GTK_TABLE (table1), combo7, 1, 2, 7, 8,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),  
                    (GtkAttachOptions) (0), 0, 0);
  combo7_items = g_list_append (combo7_items, "Closed");
  combo7_items = g_list_append (combo7_items, "Human");
  combo7_items = g_list_append (combo7_items, "Computer");
  gtk_combo_set_popdown_strings (GTK_COMBO (combo7), combo7_items);
  g_list_free (combo7_items);

  combo_entry7 = GTK_COMBO (combo7)->entry;
  gtk_widget_ref (combo_entry7);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "combo_entry7", combo_entry7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo_entry7);
  gtk_entry_set_text (GTK_ENTRY (combo_entry7), "Closed");
                            
  combo8 = gtk_combo_new ();
  gtk_widget_ref (combo8);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "combo8", combo8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo8);
  gtk_table_attach (GTK_TABLE (table1), combo8, 1, 2, 8, 9,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),  
                    (GtkAttachOptions) (0), 0, 0);
  combo8_items = g_list_append (combo8_items, "Closed");
  combo8_items = g_list_append (combo8_items, "Human");
  combo8_items = g_list_append (combo8_items, "Computer");
  gtk_combo_set_popdown_strings (GTK_COMBO (combo8), combo8_items);
  g_list_free (combo8_items);

  combo_entry8 = GTK_COMBO (combo8)->entry;
  gtk_widget_ref (combo_entry8);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "combo_entry8", combo_entry8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo_entry8);
  gtk_entry_set_text (GTK_ENTRY (combo_entry8), "Closed");
                            
  combo9 = gtk_combo_new ();
  gtk_widget_ref (combo9);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "combo9", combo9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo9);
  gtk_table_attach (GTK_TABLE (table1), combo9, 1, 2, 9, 10,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),  
                    (GtkAttachOptions) (0), 0, 0);
  combo9_items = g_list_append (combo9_items, "Closed");
  combo9_items = g_list_append (combo9_items, "Human");
  combo9_items = g_list_append (combo9_items, "Computer");
  gtk_combo_set_popdown_strings (GTK_COMBO (combo9), combo9_items);
  g_list_free (combo9_items);

  combo_entry9 = GTK_COMBO (combo9)->entry;
  gtk_widget_ref (combo_entry9);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "combo_entry9", combo_entry9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo_entry9);
  gtk_entry_set_text (GTK_ENTRY (combo_entry9), "Closed");
                            
  combo10 = gtk_combo_new ();
  gtk_widget_ref (combo10);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "combo10", combo10,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo10);
  gtk_table_attach (GTK_TABLE (table1), combo10, 1, 2, 10, 11,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),  
                    (GtkAttachOptions) (0), 0, 0);
  combo10_items = g_list_append (combo10_items, "Closed");
  combo10_items = g_list_append (combo10_items, "Human");     
  combo10_items = g_list_append (combo10_items, "Computer");
  gtk_combo_set_popdown_strings (GTK_COMBO (combo10), combo10_items);
  g_list_free (combo10_items);

  combo_entry10 = GTK_COMBO (combo10)->entry;
  gtk_widget_ref (combo_entry10);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "combo_entry10", combo_entry10,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo_entry10);
  gtk_entry_set_text (GTK_ENTRY (combo_entry10), "Closed");
                            
  lblUser = gtk_label_new ("User Name");
  gtk_widget_ref (lblUser);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "lblUser", lblUser,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (lblUser);
  gtk_table_attach (GTK_TABLE (table1), lblUser, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  togglebutton1 = gtk_toggle_button_new_with_label ("");
  gtk_widget_ref (togglebutton1);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "togglebutton1", togglebutton1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (togglebutton1);
  gtk_table_attach (GTK_TABLE (table1), togglebutton1, 2, 3, 2, 3,   
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  togglebutton2 = gtk_toggle_button_new_with_label ("");
  gtk_widget_ref (togglebutton2);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "togglebutton2", togglebutton2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (togglebutton2);
  gtk_table_attach (GTK_TABLE (table1), togglebutton2, 2, 3, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  togglebutton3 = gtk_toggle_button_new_with_label ("");
  gtk_widget_ref (togglebutton3);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "togglebutton3", togglebutton3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (togglebutton3);
  gtk_table_attach (GTK_TABLE (table1), togglebutton3, 2, 3, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  togglebutton4 = gtk_toggle_button_new_with_label ("");
  gtk_widget_ref (togglebutton4); 
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "togglebutton4", togglebutton4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (togglebutton4);
  gtk_table_attach (GTK_TABLE (table1), togglebutton4, 2, 3, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  togglebutton5 = gtk_toggle_button_new_with_label ("");
  gtk_widget_ref (togglebutton5); 
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "togglebutton5", togglebutton5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (togglebutton5);
  gtk_table_attach (GTK_TABLE (table1), togglebutton5, 2, 3, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  togglebutton6 = gtk_toggle_button_new_with_label ("");
  gtk_widget_ref (togglebutton6); 
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "togglebutton6", togglebutton6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (togglebutton6);
  gtk_table_attach (GTK_TABLE (table1), togglebutton6, 2, 3, 7, 8,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  togglebutton7 = gtk_toggle_button_new_with_label ("");
  gtk_widget_ref (togglebutton7); 
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "togglebutton7", togglebutton7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (togglebutton7);
  gtk_table_attach (GTK_TABLE (table1), togglebutton7, 2, 3, 8, 9,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  togglebutton8 = gtk_toggle_button_new_with_label ("");
  gtk_widget_ref (togglebutton8); 
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "togglebutton8", togglebutton8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (togglebutton8);
  gtk_table_attach (GTK_TABLE (table1), togglebutton8, 2, 3, 9, 10,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  togglebutton9 = gtk_toggle_button_new_with_label ("");
  gtk_widget_ref (togglebutton9); 
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "togglebutton9", togglebutton9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (togglebutton9);
  gtk_table_attach (GTK_TABLE (table1), togglebutton9, 2, 3, 10, 11,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  hbox3 = gtk_hbox_new (TRUE, 0);
  gtk_widget_ref (hbox3);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "hbox3", hbox3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox3, FALSE, FALSE, 5);

  btnLaunch = gtk_button_new_with_label ("Launch Game");
  gtk_widget_ref (btnLaunch);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "btnLaunch", btnLaunch,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnLaunch);
  gtk_box_pack_start (GTK_BOX (hbox3), btnLaunch, FALSE, TRUE, 10);
                    
  btnCancel = gtk_button_new_with_label ("Cancel Game");
  gtk_widget_ref (btnCancel);
  gtk_object_set_data_full (GTK_OBJECT (dlgLaunch), "btnCancel", btnCancel,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnCancel);
  gtk_box_pack_start (GTK_BOX (hbox3), btnCancel, FALSE, TRUE, 10);

  gtk_signal_connect_object (GTK_OBJECT (btnCancel), "clicked",
                             GTK_SIGNAL_FUNC (gtk_widget_destroy),
                             GTK_OBJECT (dlgLaunch));
  gtk_signal_connect(GTK_OBJECT(GTK_COMBO(combo1)->entry), "changed",
                     GTK_SIGNAL_FUNC (launch_change_type), NULL);
  gtk_signal_connect(GTK_OBJECT(GTK_BUTTON(btnLaunch)), "clicked",
                     GTK_SIGNAL_FUNC (launch_start_game), NULL);
  return dlgLaunch;
}


