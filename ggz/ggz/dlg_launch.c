#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "datatypes.h"
#include "dlg_launch.h"
#include "dlg_error.h"
#include "game.h"
#include "err_func.h"


/* Globals neaded by this dialog */
extern struct ConnectInfo connection;
extern struct GameTypes game_types;

/* Global GtkWidget for this dialog */
GtkWidget *dlg_launch;

/* Local callbacks which no other file will call */
void launch_change_type(GtkCombo *type_combo, gpointer user_data);
void launch_start_game(GtkWidget *btn_launch, gpointer user_data);
void launch_fill_defaults(GtkWidget* widget, gpointer data);
void launch_seat_show(int i, char show);
int max_allowed_players(unsigned char mask);
void launch_reserved_toggle(GtkWidget* button, GtkWidget *entry);


GtkWidget*
create_dlgLaunch (void)
{
  GtkWidget *dlg_launch;
  GtkWidget *vbox3;
  GtkWidget *vbox4;
  GtkWidget *hbox3;
  GtkWidget *label32;
  GtkWidget *combo11;
  GtkWidget *entry11;
  GtkWidget *label33;
  GtkWidget *combo12;
  GtkWidget *combo_entry1;
  GtkWidget *label34;
  GtkWidget *label35;
  GtkWidget *label36;
  GtkWidget *vbox5;
  GtkWidget *hseparator1;
  GtkWidget *label37;
  GtkWidget *hbox4;
  GtkWidget *label38;
  GSList *seat1_group = NULL;
  GtkWidget *radiobutton1;
  GtkWidget *radiobutton2;
  GtkWidget *radiobutton3;
  GtkWidget *entry12;
  GtkWidget *hbox5;
  GtkWidget *label39;
  GSList *seat2_group = NULL;
  GtkWidget *radiobutton4;
  GtkWidget *radiobutton5;
  GtkWidget *radiobutton6;
  GtkWidget *entry13;
  GtkWidget *hbox6;
  GtkWidget *label40;
  GSList *seat3_group = NULL;
  GtkWidget *radiobutton7;
  GtkWidget *radiobutton8;
  GtkWidget *radiobutton9;
  GtkWidget *entry14;
  GtkWidget *hbox7;
  GtkWidget *label41;
  GSList *seat4_group = NULL;
  GtkWidget *radiobutton10;
  GtkWidget *radiobutton11;
  GtkWidget *radiobutton12;
  GtkWidget *entry15;
  GtkWidget *hbox8;
  GtkWidget *label42;
  GSList *seat5_group = NULL;
  GtkWidget *radiobutton13;
  GtkWidget *radiobutton14;
  GtkWidget *radiobutton15;
  GtkWidget *entry16;
  GtkWidget *hbox9;
  GtkWidget *label43;
  GSList *seat6_group = NULL;
  GtkWidget *radiobutton16;
  GtkWidget *radiobutton17;
  GtkWidget *radiobutton18;
  GtkWidget *entry17;
  GtkWidget *hbox10;
  GtkWidget *label44;
  GSList *seat7_group = NULL;
  GtkWidget *radiobutton19;
  GtkWidget *radiobutton20;
  GtkWidget *radiobutton21;
  GtkWidget *entry18;
  GtkWidget *hbox11;
  GtkWidget *label45;
  GSList *seat8_group = NULL;
  GtkWidget *radiobutton22;
  GtkWidget *radiobutton23;
  GtkWidget *radiobutton24;
  GtkWidget *entry19;
  GtkWidget *hbox15;
  GtkWidget *label46;
  GtkWidget *entry20;
  GtkWidget *hbox12;
  GtkWidget *hbuttonbox2;
  GtkWidget *button4;
  GtkWidget *button5;

  dlg_launch = gtk_dialog_new ();
  gtk_object_set_data (GTK_OBJECT (dlg_launch), "dlg_launch", dlg_launch);
  gtk_window_set_title (GTK_WINDOW (dlg_launch), "Seat Assignments");
  gtk_window_set_policy (GTK_WINDOW (dlg_launch), FALSE, FALSE, TRUE);

  vbox3 = GTK_DIALOG (dlg_launch)->vbox;
  gtk_object_set_data (GTK_OBJECT (dlg_launch), "vbox3", vbox3);
  gtk_widget_show (vbox3);

  vbox4 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox4);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "vbox4", vbox4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox4);
  gtk_box_pack_start (GTK_BOX (vbox3), vbox4, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (vbox4), 10);

  hbox3 = gtk_hbox_new (FALSE, 10);
  gtk_widget_ref (hbox3);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "hbox3", hbox3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (vbox4), hbox3, TRUE, TRUE, 5);

  label32 = gtk_label_new ("Game Type");
  gtk_widget_ref (label32);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "label32", label32,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label32);
  gtk_box_pack_start (GTK_BOX (hbox3), label32, FALSE, FALSE, 0);

  combo11 = gtk_combo_new ();
  gtk_widget_ref (combo11);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "combo11", combo11,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo11);
  gtk_box_pack_start (GTK_BOX (hbox3), combo11, TRUE, TRUE, 0);

  entry11 = GTK_COMBO (combo11)->entry;
  gtk_widget_ref (entry11);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "entry11", entry11,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry11);
  gtk_entry_set_editable (GTK_ENTRY (entry11), FALSE);

  label33 = gtk_label_new ("Number of Players");
  gtk_widget_ref (label33);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "label33", label33,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label33);
  gtk_box_pack_start (GTK_BOX (hbox3), label33, FALSE, FALSE, 0);

  combo12 = gtk_combo_new ();
  gtk_widget_ref (combo12);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "combo12", combo12,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo12);
  gtk_box_pack_start (GTK_BOX (hbox3), combo12, TRUE, TRUE, 0);
  gtk_widget_set_usize (combo12, 15, -2);

  combo_entry1 = GTK_COMBO (combo12)->entry;
  gtk_widget_ref (combo_entry1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "combo_entry1", combo_entry1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo_entry1);

  label34 = gtk_label_new ("Author:");
  gtk_widget_ref (label34);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "label34", label34,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label34);
  gtk_box_pack_start (GTK_BOX (vbox4), label34, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label34), GTK_JUSTIFY_LEFT);

  label35 = gtk_label_new ("Description:");
  gtk_widget_ref (label35);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "label35", label35,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label35);
  gtk_box_pack_start (GTK_BOX (vbox4), label35, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label35), GTK_JUSTIFY_LEFT);

  label36 = gtk_label_new ("Homepage:");
  gtk_widget_ref (label36);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "label36", label36,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label36);
  gtk_box_pack_start (GTK_BOX (vbox4), label36, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label36), GTK_JUSTIFY_LEFT);

  vbox5 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox5);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "vbox5", vbox5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox5);
  gtk_box_pack_start (GTK_BOX (vbox4), vbox5, TRUE, TRUE, 3);

  hseparator1 = gtk_hseparator_new ();
  gtk_widget_ref (hseparator1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "hseparator1", hseparator1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hseparator1);
  gtk_box_pack_start (GTK_BOX (vbox5), hseparator1, TRUE, TRUE, 0);

  label37 = gtk_label_new ("Seat 0: {name}");
  gtk_widget_ref (label37);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "label37", label37,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label37);
  gtk_box_pack_start (GTK_BOX (vbox4), label37, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label37), GTK_JUSTIFY_LEFT);

  hbox4 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox4);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "hbox4", hbox4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox4);
  gtk_box_pack_start (GTK_BOX (vbox4), hbox4, TRUE, TRUE, 0);

  label38 = gtk_label_new ("Seat 1:");
  gtk_widget_ref (label38);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "label38", label38,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label38);
  gtk_box_pack_start (GTK_BOX (hbox4), label38, FALSE, FALSE, 0);

  radiobutton1 = gtk_radio_button_new_with_label (seat1_group, "Computer");
  seat1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton1));
  gtk_widget_ref (radiobutton1);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton1", radiobutton1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton1);
  gtk_box_pack_start (GTK_BOX (hbox4), radiobutton1, FALSE, FALSE, 3);

  radiobutton2 = gtk_radio_button_new_with_label (seat1_group, "Open");
  seat1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton2));
  gtk_widget_ref (radiobutton2);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton2", radiobutton2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton2);
  gtk_box_pack_start (GTK_BOX (hbox4), radiobutton2, FALSE, FALSE, 0);

  radiobutton3 = gtk_radio_button_new_with_label (seat1_group, "Reserved for");
  seat1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton3));
  gtk_widget_ref (radiobutton3);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton3", radiobutton3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton3);
  gtk_box_pack_start (GTK_BOX (hbox4), radiobutton3, FALSE, FALSE, 0);

  entry12 = gtk_entry_new_with_max_length (8);
  gtk_widget_ref (entry12);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "entry12", entry12,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry12);
  gtk_box_pack_start (GTK_BOX (hbox4), entry12, TRUE, TRUE, 0);
  gtk_entry_set_editable (GTK_ENTRY (entry12), FALSE);

  hbox5 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox5);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "hbox5", hbox5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox5);
  gtk_box_pack_start (GTK_BOX (vbox4), hbox5, TRUE, TRUE, 0);

  label39 = gtk_label_new ("Seat 2:");
  gtk_widget_ref (label39);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "label39", label39,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label39);
  gtk_box_pack_start (GTK_BOX (hbox5), label39, FALSE, FALSE, 0);

  radiobutton4 = gtk_radio_button_new_with_label (seat2_group, "Computer");
  seat2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton4));
  gtk_widget_ref (radiobutton4);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton4", radiobutton4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton4);
  gtk_box_pack_start (GTK_BOX (hbox5), radiobutton4, FALSE, FALSE, 3);

  radiobutton5 = gtk_radio_button_new_with_label (seat2_group, "Open");
  seat2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton5));
  gtk_widget_ref (radiobutton5);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton5", radiobutton5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton5);
  gtk_box_pack_start (GTK_BOX (hbox5), radiobutton5, FALSE, FALSE, 0);

  radiobutton6 = gtk_radio_button_new_with_label (seat2_group, "Reserved for");
  seat2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton6));
  gtk_widget_ref (radiobutton6);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton6", radiobutton6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton6);
  gtk_box_pack_start (GTK_BOX (hbox5), radiobutton6, FALSE, FALSE, 0);

  entry13 = gtk_entry_new_with_max_length (8);
  gtk_widget_ref (entry13);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "entry13", entry13,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry13);
  gtk_box_pack_start (GTK_BOX (hbox5), entry13, TRUE, TRUE, 0);
  gtk_entry_set_editable (GTK_ENTRY (entry13), FALSE);

  hbox6 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox6);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "hbox6", hbox6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox6);
  gtk_box_pack_start (GTK_BOX (vbox4), hbox6, TRUE, TRUE, 0);

  label40 = gtk_label_new ("Seat 3:");
  gtk_widget_ref (label40);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "label40", label40,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label40);
  gtk_box_pack_start (GTK_BOX (hbox6), label40, FALSE, FALSE, 0);

  radiobutton7 = gtk_radio_button_new_with_label (seat3_group, "Computer");
  seat3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton7));
  gtk_widget_ref (radiobutton7);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton7", radiobutton7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton7);
  gtk_box_pack_start (GTK_BOX (hbox6), radiobutton7, FALSE, FALSE, 3);

  radiobutton8 = gtk_radio_button_new_with_label (seat3_group, "Open");
  seat3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton8));
  gtk_widget_ref (radiobutton8);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton8", radiobutton8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton8);
  gtk_box_pack_start (GTK_BOX (hbox6), radiobutton8, FALSE, FALSE, 0);

  radiobutton9 = gtk_radio_button_new_with_label (seat3_group, "Reserved for");
  seat3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton9));
  gtk_widget_ref (radiobutton9);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton9", radiobutton9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton9);
  gtk_box_pack_start (GTK_BOX (hbox6), radiobutton9, FALSE, FALSE, 0);

  entry14 = gtk_entry_new_with_max_length (8);
  gtk_widget_ref (entry14);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "entry14", entry14,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry14);
  gtk_box_pack_start (GTK_BOX (hbox6), entry14, TRUE, TRUE, 0);
  gtk_entry_set_editable (GTK_ENTRY (entry14), FALSE);

  hbox7 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox7);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "hbox7", hbox7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox7);
  gtk_box_pack_start (GTK_BOX (vbox4), hbox7, TRUE, TRUE, 0);

  label41 = gtk_label_new ("Seat 4:");
  gtk_widget_ref (label41);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "label41", label41,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label41);
  gtk_box_pack_start (GTK_BOX (hbox7), label41, FALSE, FALSE, 0);

  radiobutton10 = gtk_radio_button_new_with_label (seat4_group, "Computer");
  seat4_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton10));
  gtk_widget_ref (radiobutton10);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton10", radiobutton10,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton10);
  gtk_box_pack_start (GTK_BOX (hbox7), radiobutton10, FALSE, FALSE, 3);

  radiobutton11 = gtk_radio_button_new_with_label (seat4_group, "Open");
  seat4_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton11));
  gtk_widget_ref (radiobutton11);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton11", radiobutton11,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton11);
  gtk_box_pack_start (GTK_BOX (hbox7), radiobutton11, FALSE, FALSE, 0);

  radiobutton12 = gtk_radio_button_new_with_label (seat4_group, "Reserved for");
  seat4_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton12));
  gtk_widget_ref (radiobutton12);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton12", radiobutton12,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton12);
  gtk_box_pack_start (GTK_BOX (hbox7), radiobutton12, FALSE, FALSE, 0);

  entry15 = gtk_entry_new_with_max_length (8);
  gtk_widget_ref (entry15);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "entry15", entry15,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry15);
  gtk_box_pack_start (GTK_BOX (hbox7), entry15, TRUE, TRUE, 0);
  gtk_entry_set_editable (GTK_ENTRY (entry15), FALSE);

  hbox8 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox8);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "hbox8", hbox8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox8);
  gtk_box_pack_start (GTK_BOX (vbox4), hbox8, TRUE, TRUE, 0);

  label42 = gtk_label_new ("Seat 5:");
  gtk_widget_ref (label42);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "label42", label42,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label42);
  gtk_box_pack_start (GTK_BOX (hbox8), label42, FALSE, FALSE, 0);

  radiobutton13 = gtk_radio_button_new_with_label (seat5_group, "Computer");
  seat5_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton13));
  gtk_widget_ref (radiobutton13);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton13", radiobutton13,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton13);
  gtk_box_pack_start (GTK_BOX (hbox8), radiobutton13, FALSE, FALSE, 3);

  radiobutton14 = gtk_radio_button_new_with_label (seat5_group, "Open");
  seat5_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton14));
  gtk_widget_ref (radiobutton14);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton14", radiobutton14,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton14);
  gtk_box_pack_start (GTK_BOX (hbox8), radiobutton14, FALSE, FALSE, 0);

  radiobutton15 = gtk_radio_button_new_with_label (seat5_group, "Reserved for");
  seat5_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton15));
  gtk_widget_ref (radiobutton15);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton15", radiobutton15,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton15);
  gtk_box_pack_start (GTK_BOX (hbox8), radiobutton15, FALSE, FALSE, 0);

  entry16 = gtk_entry_new_with_max_length (8);
  gtk_widget_ref (entry16);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "entry16", entry16,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry16);
  gtk_box_pack_start (GTK_BOX (hbox8), entry16, TRUE, TRUE, 0);
  gtk_entry_set_editable (GTK_ENTRY (entry16), FALSE);

  hbox9 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox9);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "hbox9", hbox9,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox9);
  gtk_box_pack_start (GTK_BOX (vbox4), hbox9, TRUE, TRUE, 0);

  label43 = gtk_label_new ("Seat 6:");
  gtk_widget_ref (label43);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "label43", label43,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label43);
  gtk_box_pack_start (GTK_BOX (hbox9), label43, FALSE, FALSE, 0);

  radiobutton16 = gtk_radio_button_new_with_label (seat6_group, "Computer");
  seat6_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton16));
  gtk_widget_ref (radiobutton16);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton16", radiobutton16,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton16);
  gtk_box_pack_start (GTK_BOX (hbox9), radiobutton16, FALSE, FALSE, 3);

  radiobutton17 = gtk_radio_button_new_with_label (seat6_group, "Open");
  seat6_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton17));
  gtk_widget_ref (radiobutton17);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton17", radiobutton17,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton17);
  gtk_box_pack_start (GTK_BOX (hbox9), radiobutton17, FALSE, FALSE, 0);

  radiobutton18 = gtk_radio_button_new_with_label (seat6_group, "Reserved for");
  seat6_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton18));
  gtk_widget_ref (radiobutton18);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton18", radiobutton18,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton18);
  gtk_box_pack_start (GTK_BOX (hbox9), radiobutton18, FALSE, FALSE, 0);

  entry17 = gtk_entry_new_with_max_length (8);
  gtk_widget_ref (entry17);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "entry17", entry17,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry17);
  gtk_box_pack_start (GTK_BOX (hbox9), entry17, TRUE, TRUE, 0);
  gtk_entry_set_editable (GTK_ENTRY (entry17), FALSE);

  hbox10 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox10);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "hbox10", hbox10,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox10);
  gtk_box_pack_start (GTK_BOX (vbox4), hbox10, TRUE, TRUE, 0);

  label44 = gtk_label_new ("Seat 7:");
  gtk_widget_ref (label44);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "label44", label44,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label44);
  gtk_box_pack_start (GTK_BOX (hbox10), label44, FALSE, FALSE, 0);

  radiobutton19 = gtk_radio_button_new_with_label (seat7_group, "Computer");
  seat7_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton19));
  gtk_widget_ref (radiobutton19);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton19", radiobutton19,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton19);
  gtk_box_pack_start (GTK_BOX (hbox10), radiobutton19, FALSE, FALSE, 3);

  radiobutton20 = gtk_radio_button_new_with_label (seat7_group, "Open");
  seat7_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton20));
  gtk_widget_ref (radiobutton20);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton20", radiobutton20,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton20);
  gtk_box_pack_start (GTK_BOX (hbox10), radiobutton20, FALSE, FALSE, 0);

  radiobutton21 = gtk_radio_button_new_with_label (seat7_group, "Reserved for");
  seat7_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton21));
  gtk_widget_ref (radiobutton21);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton21", radiobutton21,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton21);
  gtk_box_pack_start (GTK_BOX (hbox10), radiobutton21, FALSE, FALSE, 0);

  entry18 = gtk_entry_new_with_max_length (8);
  gtk_widget_ref (entry18);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "entry18", entry18,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry18);
  gtk_box_pack_start (GTK_BOX (hbox10), entry18, TRUE, TRUE, 0);
  gtk_entry_set_editable (GTK_ENTRY (entry18), FALSE);

  hbox11 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox11);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "hbox11", hbox11,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox11);
  gtk_box_pack_start (GTK_BOX (vbox4), hbox11, TRUE, TRUE, 0);

  label45 = gtk_label_new ("Seat 8:");
  gtk_widget_ref (label45);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "label45", label45,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label45);
  gtk_box_pack_start (GTK_BOX (hbox11), label45, FALSE, FALSE, 0);

  radiobutton22 = gtk_radio_button_new_with_label (seat8_group, "Computer");
  seat8_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton22));
  gtk_widget_ref (radiobutton22);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton22", radiobutton22,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton22);
  gtk_box_pack_start (GTK_BOX (hbox11), radiobutton22, FALSE, FALSE, 3);

  radiobutton23 = gtk_radio_button_new_with_label (seat8_group, "Open");
  seat8_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton23));
  gtk_widget_ref (radiobutton23);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton23", radiobutton23,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton23);
  gtk_box_pack_start (GTK_BOX (hbox11), radiobutton23, FALSE, FALSE, 0);

  radiobutton24 = gtk_radio_button_new_with_label (seat8_group, "Reserved for");
  seat8_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton24));
  gtk_widget_ref (radiobutton24);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "radiobutton24", radiobutton24,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (radiobutton24);
  gtk_box_pack_start (GTK_BOX (hbox11), radiobutton24, FALSE, FALSE, 0);

  entry19 = gtk_entry_new_with_max_length (8);
  gtk_widget_ref (entry19);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "entry19", entry19,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry19);
  gtk_box_pack_start (GTK_BOX (hbox11), entry19, TRUE, TRUE, 0);
  gtk_entry_set_editable (GTK_ENTRY (entry19), FALSE);

  hbox15 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox15);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "hbox15", hbox15,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox15);
  gtk_box_pack_start (GTK_BOX (vbox4), hbox15, TRUE, TRUE, 5);

  label46 = gtk_label_new ("Game Description   ");
  gtk_widget_ref (label46);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "label46", label46,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label46);
  gtk_box_pack_start (GTK_BOX (hbox15), label46, FALSE, FALSE, 0);

  entry20 = gtk_entry_new ();
  gtk_widget_ref (entry20);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "entry20", entry20,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry20);
  gtk_box_pack_start (GTK_BOX (hbox15), entry20, TRUE, TRUE, 0);

  hbox12 = GTK_DIALOG (dlg_launch)->action_area;
  gtk_object_set_data (GTK_OBJECT (dlg_launch), "hbox12", hbox12);
  gtk_widget_show (hbox12);
  gtk_container_set_border_width (GTK_CONTAINER (hbox12), 10);

  hbuttonbox2 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox2);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "hbuttonbox2", hbuttonbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox2);
  gtk_box_pack_start (GTK_BOX (hbox12), hbuttonbox2, TRUE, TRUE, 0);

  button4 = gtk_button_new_with_label ("Launch");
  gtk_widget_ref (button4);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "button4", button4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button4);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), button4);
  GTK_WIDGET_SET_FLAGS (button4, GTK_CAN_DEFAULT);

  button5 = gtk_button_new_with_label ("Cancel");
  gtk_widget_ref (button5);
  gtk_object_set_data_full (GTK_OBJECT (dlg_launch), "button5", button5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button5);
  gtk_container_add (GTK_CONTAINER (hbuttonbox2), button5);
  GTK_WIDGET_SET_FLAGS (button5, GTK_CAN_DEFAULT);


  gtk_signal_connect_object (GTK_OBJECT (button5), "clicked",
                             GTK_SIGNAL_FUNC (gtk_widget_destroy),
                             GTK_OBJECT (dlg_launch));
  gtk_signal_connect(GTK_OBJECT(GTK_COMBO(combo11)->entry), "changed",
                     GTK_SIGNAL_FUNC (launch_change_type), NULL);
  gtk_signal_connect(GTK_OBJECT(GTK_BUTTON(button4)), "clicked",
                     GTK_SIGNAL_FUNC (launch_start_game), NULL);
        
  gtk_signal_connect (GTK_OBJECT (dlg_launch), "realize",
                      GTK_SIGNAL_FUNC (launch_fill_defaults),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (radiobutton3), "toggled",
                      GTK_SIGNAL_FUNC (launch_reserved_toggle),
                      GTK_ENTRY(entry12));
  gtk_signal_connect (GTK_OBJECT (radiobutton6), "toggled",
                      GTK_SIGNAL_FUNC (launch_reserved_toggle),
                      GTK_ENTRY(entry13));
  gtk_signal_connect (GTK_OBJECT (radiobutton9), "toggled",
                      GTK_SIGNAL_FUNC (launch_reserved_toggle),
                      GTK_ENTRY(entry14));
  gtk_signal_connect (GTK_OBJECT (radiobutton12), "toggled",
                      GTK_SIGNAL_FUNC (launch_reserved_toggle),
                      GTK_ENTRY(entry15));
  gtk_signal_connect (GTK_OBJECT (radiobutton15), "toggled",
                      GTK_SIGNAL_FUNC (launch_reserved_toggle),
                      GTK_ENTRY(entry16));
  gtk_signal_connect (GTK_OBJECT (radiobutton18), "toggled",
                      GTK_SIGNAL_FUNC (launch_reserved_toggle),
                      GTK_ENTRY(entry17));
  gtk_signal_connect (GTK_OBJECT (radiobutton21), "toggled",
                      GTK_SIGNAL_FUNC (launch_reserved_toggle),
                      GTK_ENTRY(entry18));
  gtk_signal_connect (GTK_OBJECT (radiobutton24), "toggled",
                      GTK_SIGNAL_FUNC (launch_reserved_toggle),
                      GTK_ENTRY(entry19));

  return dlg_launch;
}


/*                              *
 *           Callbacks          *
 *                              */

void launch_fill_defaults(GtkWidget* widget, gpointer data)
{
        int i;
        GList *items = NULL;
        GList *items2 = NULL;
        GtkWidget *tmp;
        
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), "label37");
        gtk_label_set_text(GTK_LABEL(tmp), connection.username);
        for (i = 0; i < game_types.count; i++)
                items2 = g_list_append(items2, game_types.info[i].name);
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), "combo11");
        gtk_combo_set_popdown_strings(GTK_COMBO(tmp), items2);
        g_list_free(items);
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), "label34");
        gtk_label_set_text(GTK_LABEL(tmp), game_types.info[0].author);
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), "label35");
        gtk_label_set_text(GTK_LABEL(tmp), game_types.info[0].desc);
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), "label36");
        gtk_label_set_text(GTK_LABEL(tmp), game_types.info[0].web);

	 if(game_types.info[0].num_play_allow & PLAY_ALLOW_EIGHT)
                items = g_list_append(items, "8");
	 if(game_types.info[0].num_play_allow & PLAY_ALLOW_SEVEN)
                items = g_list_append(items, "7");
	 if(game_types.info[0].num_play_allow & PLAY_ALLOW_SIX)
                items = g_list_append(items, "6");
	 if(game_types.info[0].num_play_allow & PLAY_ALLOW_FIVE)
                items = g_list_append(items, "5");
	 if(game_types.info[0].num_play_allow & PLAY_ALLOW_FOUR)
                items = g_list_append(items, "4");
	 if(game_types.info[0].num_play_allow & PLAY_ALLOW_THREE)
                items = g_list_append(items, "3");
	 if(game_types.info[0].num_play_allow & PLAY_ALLOW_TWO)
                items = g_list_append(items, "2");
	 if(game_types.info[0].num_play_allow & PLAY_ALLOW_ONE)
                items = g_list_append(items, "1");
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), "combo12");
        gtk_combo_set_popdown_strings(GTK_COMBO(tmp), items);
        g_list_free(items);

}

void launch_change_type(GtkCombo *type_combo, gpointer user_data)
{
        gpointer tmp;
        int i, max;
        int type_index = 0;

        /* Get new game type index */
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), "combo11");
        for(i = 0; i < game_types.count; i++)
                if(!strcmp(game_types.info[i].name, gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(tmp)->entry))))
                        break;

        type_index = i;
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), "label34");
        gtk_label_set_text(GTK_LABEL(tmp), game_types.info[type_index].author);
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), "label35");
        gtk_label_set_text(GTK_LABEL(tmp), game_types.info[type_index].desc);
        tmp = gtk_object_get_data(GTK_OBJECT(dlg_launch), "label36");
        gtk_label_set_text(GTK_LABEL(tmp), game_types.info[type_index].web);

        /* Hide seats with numbers bigger than we allow and show others */
        max = max_allowed_players(game_types.info[type_index].num_play_allow);
        dbg_msg("Max allowed = %d : hiding unused combos", max);

        for (i = 2; i <= max; i++)
                launch_seat_show(i, TRUE);

        for (i = max + 1; i <= 8; i++)
                launch_seat_show(i, FALSE);
}

void launch_start_game(GtkWidget *btn_launch, gpointer user_data)
{
        GtkWidget *temp_widget;
        int i;
        int launch_game_type=0;
  
        /* Hide it at this point, we still nead
           it there to get the information from */
        gtk_widget_hide(GTK_WIDGET(dlg_launch));
        
        /* Get game type to play */
        temp_widget = gtk_object_get_data(GTK_OBJECT(dlg_launch), "combo11");
        for(i=0;i<game_types.count;i++)
        {
                if(!strcmp(game_types.info[i].name,
                           gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(temp_widget)->entry))))
                        launch_game_type = i;
        }

        if (!connection.connected)
                warn_dlg("Not connected!");
        else {
                launch_game(launch_game_type, 1);
        }
          
}

void launch_reserved_toggle(GtkWidget* button, GtkWidget *entry)
{
        if (GTK_TOGGLE_BUTTON(button)->active) {
		gtk_entry_set_editable (GTK_ENTRY (entry), TRUE);
		gtk_widget_grab_focus(GTK_ENTRY(entry));
        }else{
		gtk_entry_set_text (GTK_ENTRY (entry), "");
		gtk_entry_set_editable (GTK_ENTRY (entry), FALSE);
	}
}

/*                              *
 *        Helper Functions      *
 *                              */

void launch_seat_show(int i, char show)
{
        gpointer tmp;
        char* widget;

        if (show)
                dbg_msg("Showing seat %d", i);
        else
                dbg_msg("Hiding seat %d", i);

        /* Show seat's hbox */
        widget = g_strdup_printf("hbox%d", i+3);
        tmp = GTK_OBJECT(gtk_object_get_data(GTK_OBJECT(dlg_launch), widget));
        if (show)
                gtk_widget_show(GTK_WIDGET(tmp));
        else
                gtk_widget_hide(GTK_WIDGET(tmp));

}
                        
         
int max_allowed_players(unsigned char mask)
{
        char i;

        for(i = 7; i >= 0; i--)
                if (mask / (1 << i))
                        return (i);
 
        return 0;
}       

int launch_seat_type(int i)
{
        char* widget;
        GtkWidget *temp_widget;
	int count = 0;
        int ret = -1;

        dbg_msg("Getting player type for seat %d", i);

        /* Show seat's hbox */
        for(count=i*3; count<i*3+3; count++)
        {
                widget = g_strdup_printf("radiobutton%d", count-2);
                temp_widget = gtk_object_get_data(GTK_OBJECT(dlg_launch), widget);
                if (GTK_TOGGLE_BUTTON(temp_widget)->active && count==i*3)
                        ret=1;	/* Computer */
                if (GTK_TOGGLE_BUTTON(temp_widget)->active && count==i*3+1)
                        ret=2;	/* Open */
                if (GTK_TOGGLE_BUTTON(temp_widget)->active && count==i*3+2)
                        ret=3;	/* Reserved */
                g_free(widget);
        }

	dbg_msg("\tSeat type: %d",ret);
        return ret;
}

void launch_get_reserve_name(int seat, char *name)
{
        char* widget;
        GtkWidget *temp_widget;

        widget = g_strdup_printf("entry%d", seat+11);
        temp_widget = gtk_object_get_data(GTK_OBJECT(dlg_launch), widget);
	g_free(widget);

	name = gtk_entry_get_text(GTK_ENTRY(temp_widget));
}
