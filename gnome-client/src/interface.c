#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gnome.h>
#include <gconf/gconf-client.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "xtext.h"

extern GConfClient *config;

void login_set_login (GtkWidget *login)
{
	GtkWidget *tmp;
	GtkTreeModel *stoProfiles;
	GtkTreeIter iter;
	gint intProfiles;
	GSList *slProfiles = NULL;
	gint x;
	
	/*Hide the "new" widgets*/
	tmp = lookup_widget(login, "lblNewProfileName");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "entNewProfileName");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "lblNewUsername");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "entNewUsername");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "lblNewServer");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "entNewServer");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "btnNewCreate");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "btnNewCancel");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "btnNewAdvanced");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "lblNewInfo");
	gtk_widget_hide(tmp);
	
	/*hide the "chat" widgets*/
	tmp = lookup_widget(login, "trePlayers");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "vbChatButtons");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "vbChatDisplay");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "swRooms");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "vbTables");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "btnChatChange");
	gtk_widget_set_sensitive(tmp, FALSE);
	tmp = lookup_widget(login, "btnChatGame");
	gtk_widget_set_sensitive(tmp, FALSE);
	
	/*Change the background*/
	tmp = lookup_widget(login, "pmBackground");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "pmNewBackground");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "pmChatBackground");
	gtk_widget_hide(tmp);
		
	/*Show the "login" widgets*/
	tmp = lookup_widget(login, "lblUsername");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "entUsername");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "lblPassword");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "entPassword");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "lblServer");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "entServer");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "btnGuest");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "btnRegistered");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "btnLogin");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "btnNew");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "btnQuit");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "treProfiles");
	gtk_widget_show(tmp);

	/*Clear and re-fill the profile list*/
	tmp = lookup_widget(login, "treProfiles");
	stoProfiles = gtk_tree_view_get_model (GTK_TREE_VIEW (tmp));
	gtk_list_store_clear (GTK_LIST_STORE (stoProfiles));
	
	intProfiles = gconf_client_get_int (config, "/schemas/apps/ggz-gnome/profiles/total", NULL);
	slProfiles = gconf_client_get_list (config, "/schemas/apps/ggz-gnome/profiles/profiles",  GCONF_VALUE_STRING, NULL);
	if (g_slist_length (slProfiles) != intProfiles)
	{
		/* Something is wrong */
		g_print(_("There has been a problem loading your profiles.\n"));
		gconf_client_set_int (config, "/schemas/apps/ggz-gnome/profiles/total", 0, NULL);
		gconf_client_set_list (config, "/schemas/apps/ggz-gnome/profiles/profiles",  GCONF_VALUE_STRING, NULL, NULL);
		
	} else {
		for (x = 0; x < intProfiles; x++)
		{
			gtk_list_store_append (GTK_LIST_STORE(stoProfiles),
					       &iter);
			gtk_list_store_set (GTK_LIST_STORE(stoProfiles),
					    &iter, 0,
					    g_slist_nth_data (slProfiles, x), 
					    -1);
		}
	}
	
	/*Clear xtext */
	tmp = lookup_widget(login, "custChatXText");
	gtk_xtext_clear(GTK_XTEXT(tmp)->buffer);
	
	/*Focus the unername entry*/
	tmp = lookup_widget(login, "entUsername");
	gtk_widget_grab_focus (tmp);

}


void login_set_new (GtkWidget *login)
{
	GtkWidget *tmp;
	
	/*hide the "login" widgets*/
	tmp = lookup_widget(login, "lblUsername");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "entUsername");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "lblPassword");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "entPassword");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "lblServer");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "entServer");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "btnGuest");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "btnRegistered");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "btnLogin");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "btnNew");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "btnQuit");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "treProfiles");
	gtk_widget_hide(tmp);

	/*hide the "chat" widgets*/
	tmp = lookup_widget(login, "trePlayers");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "vbChatButtons");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "vbChatDisplay");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "swRooms");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "vbTables");
	gtk_widget_hide(tmp);
	
	/*Hide the star widget*/
	tmp = lookup_widget(login, "pmStar1");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "pmStar2");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "pmStar3");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "pmStar4");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "pmStar5");
	gtk_widget_hide(tmp);

	/*Change the background*/
	tmp = lookup_widget(login, "pmNewBackground");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "pmBackground");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "pmChatBackground");
	gtk_widget_hide(tmp);


	/*Show the "new" widgets*/
	tmp = lookup_widget(login, "lblNewProfileName");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "entNewProfileName");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "lblNewUsername");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "entNewUsername");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "lblNewServer");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "entNewServer");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "btnNewCreate");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "btnNewCancel");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "btnNewAdvanced");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "lblNewInfo");
	gtk_widget_show(tmp);

}

void login_set_chat (GtkWidget *login)
{
	GtkWidget *tmp;
	
	/*hide the "login" widgets*/
	tmp = lookup_widget(login, "lblUsername");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "entUsername");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "lblPassword");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "entPassword");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "lblServer");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "entServer");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "btnGuest");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "btnRegistered");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "btnLogin");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "btnNew");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "btnQuit");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "treProfiles");
	gtk_widget_hide(tmp);

	/*Hide the "new" widgets*/
        tmp = lookup_widget(login, "lblNewProfileName");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "entNewProfileName");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "lblNewUsername");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "entNewUsername");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "lblNewServer");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "entNewServer");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "btnNewCreate");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "btnNewCancel");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "btnNewAdvanced");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "lblNewInfo");
	gtk_widget_hide(tmp);

	/*Hide the star widget*/
	tmp = lookup_widget(login, "pmStar1");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "pmStar2");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "pmStar3");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "pmStar4");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "pmStar5");
	gtk_widget_hide(tmp);

	/*Change the background*/
	tmp = lookup_widget(login, "pmChatBackground");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "pmBackground");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "pmNewBackground");
	gtk_widget_hide(tmp);
	
        /*Setup the "chat" widgets*/
	tmp = lookup_widget(login, "btnChatChange");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "btnChatChat1");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "btnChatGame");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "btnChatChat2");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "trePlayers");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "vbChatButtons");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "vbChatDisplay");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "swRooms");
	gtk_widget_hide(tmp);							
	tmp = lookup_widget(login, "vbTables");
	gtk_widget_hide(tmp);							

	/*Focus the chat text entry*/
	tmp = lookup_widget(login, "entChatNew");
	gtk_widget_grab_focus(tmp);
}

void login_set_chat_rooms(GtkWidget *login)
{
	GtkWidget *tmp;

	
	/*Setup the "chat" widgets*/
	tmp = lookup_widget(login, "btnChatChange");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "btnChatChat1");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "btnChatGame");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "btnChatChat2");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "vbChatDisplay");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "vbTables");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "swRooms");
	gtk_widget_show(tmp);
											
}

void login_set_chat_games(GtkWidget *login)
{
	GtkWidget *tmp;

	
	/*Setup the "chat" widgets*/
	tmp = lookup_widget(login, "btnChatChange");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "btnChatChat1");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "btnChatGame");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "btnChatChat2");
	gtk_widget_show(tmp);
	tmp = lookup_widget(login, "vbChatDisplay");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "swRooms");
	gtk_widget_hide(tmp);
	tmp = lookup_widget(login, "vbTables");
	gtk_widget_show(tmp);
}

GtkWidget*
create_login (void)
{
  GtkWidget *window;
  GtkWidget *fixMain;
  GtkWidget *btnLogin;
  GtkWidget *btnNew;
  GtkWidget *btnQuit;
  GtkListStore *stoProfiles;
  GtkWidget *treProfiles;
  GtkCellRenderer *rndProfiles;
  GtkTreeViewColumn *colProfiles;
  GtkTreeSelection *selProfiles;
  GtkWidget *vbLogin;
  GtkWidget *lblUsername;
  GtkWidget *entUsername;
  GtkWidget *lblPassword;
  GtkWidget *entPassword;
  GtkWidget *lblServer;
  GtkWidget *entServer;
  GtkWidget *hbLogin;
  GtkWidget *vbNewLogin;
  GtkWidget *lblNewProfileName;
  GtkWidget *entNewProfileName;
  GtkWidget *lblNewUsername;
  GtkWidget *entNewUsername;
  GtkWidget *lblNewServer;
  GtkWidget *entNewServer;
  GtkWidget *hbNewLogin;
  GtkWidget *btnNewCreate;
  GtkWidget *btnNewCancel;
  GtkWidget *btnNewAdvanced;
  GtkWidget *lblNewInfo;
  GSList *hbLogin_group = NULL;
  GtkWidget *btnGuest;
  GtkWidget *btnRegistered;
  GtkWidget *pmStar1;
  GtkWidget *pmStar2;
  GtkWidget *pmStar3;
  GtkWidget *pmStar4;
  GtkWidget *pmStar5;
  GtkListStore *stoPlayers;
  GtkWidget *trePlayers;
  GtkCellRenderer *rndPlayers;
  GtkTreeViewColumn *colPlayers;
  GtkWidget *vbChatButtons;
  GtkWidget *btnChatChange;
  GtkWidget *btnChatChat1;
  GtkWidget *btnChatGame;
  GtkWidget *btnChatChat2;
  GtkWidget *btnChatLogoff;
  GtkWidget *vbChatDisplay;
  GtkWidget *nhChatXText;
  GtkWidget *fraChatXText;
  GtkWidget *custChatXText;
  GtkWidget *vsrChatScroll;
  GtkWidget *hbChatEnter;
  GtkWidget *entChatNew;
  GtkWidget *btnChatSend;
  GtkWidget *pmBackground;
  GtkWidget *pmNewBackground;
  GtkWidget *pmChatBackground;
  GtkWidget *swRooms;
  GtkWidget *vpRooms;
  GtkWidget *vbRooms;
  GtkWidget *vbTables;
  GtkListStore *stoTables;
  GtkWidget *treTables;
  GtkCellRenderer *rndTables;
  GtkTreeViewColumn *colTables;
  GtkWidget *hbuttonbox1;
  GtkWidget *btnLaunch;
  GtkWidget *btnJoin;
  GtkWidget *btnWatch;
  static FakeTips  tipsPlayer;

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_name (window, "window");
  gtk_object_set_data (GTK_OBJECT (window), "window", window);
  gtk_widget_set_usize (window, 640, 480);
  gtk_window_set_title (GTK_WINDOW (window), _("GGZ Gaming Zone"));
  gtk_window_set_resizable (GTK_WINDOW (window), FALSE);

  fixMain = gtk_fixed_new ();
  gtk_widget_set_name (fixMain, "fixMain");
  gtk_widget_ref (fixMain);
  gtk_object_set_data_full (GTK_OBJECT (window), "fixMain", fixMain,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (fixMain);
  gtk_container_add (GTK_CONTAINER (window), fixMain);

  pmBackground = create_pixmap (window, "Login.xpm");
  gtk_widget_set_name (pmBackground, "pmBackground");
  gtk_widget_ref (pmBackground);
  gtk_object_set_data_full (GTK_OBJECT (window), "pmBackground", pmBackground,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pmBackground);
  gtk_fixed_put (GTK_FIXED (fixMain), pmBackground, 0, 0);
  gtk_widget_set_uposition (pmBackground, 0, 0);
  gtk_widget_set_usize (pmBackground, 640, 480);

  pmNewBackground = create_pixmap (window, "New.xpm");
  gtk_widget_set_name (pmNewBackground, "pmNewBackground");
  gtk_widget_ref (pmNewBackground);
  gtk_object_set_data_full (GTK_OBJECT (window), "pmNewBackground", pmNewBackground,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pmNewBackground);
  gtk_fixed_put (GTK_FIXED (fixMain), pmNewBackground, 0, 0);
  gtk_widget_set_uposition (pmNewBackground, 0, 0);
  gtk_widget_set_usize (pmNewBackground, 640, 480);

  pmChatBackground = create_pixmap (window, "chat.xpm");
  gtk_widget_set_name (pmChatBackground, "pmChatBackground");
  gtk_widget_ref (pmChatBackground);
  gtk_object_set_data_full (GTK_OBJECT (window), "pmChatBackground", pmChatBackground,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pmBackground);
  gtk_fixed_put (GTK_FIXED (fixMain), pmChatBackground, 0, 0);
  gtk_widget_set_uposition (pmChatBackground, 0, 0);
  gtk_widget_set_usize (pmChatBackground, 640, 480);

  btnLogin = gtk_button_new_with_mnemonic (_("_LOGIN"));
  gtk_widget_set_name (btnLogin, "btnLogin");
  gtk_widget_ref (btnLogin);
  gtk_object_set_data_full (GTK_OBJECT (window), "btnLogin", btnLogin,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnLogin);
  gtk_fixed_put (GTK_FIXED (fixMain), btnLogin, 280, 208);
  gtk_widget_set_uposition (btnLogin, 272, 208);
  gtk_widget_set_usize (btnLogin, 95, 25);
  gtk_widget_set_sensitive (btnLogin, FALSE);

  btnNew = gtk_button_new_with_mnemonic (_("_PROFILES"));
  gtk_widget_set_name (btnNew, "btnNew");
  gtk_widget_ref (btnNew);
  gtk_object_set_data_full (GTK_OBJECT (window), "btnNew", btnNew,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnNew);
  gtk_fixed_put (GTK_FIXED (fixMain), btnNew, 280, 288);
  gtk_widget_set_uposition (btnNew, 272, 288);
  gtk_widget_set_usize (btnNew, 95, 25);

  btnQuit = gtk_button_new_with_mnemonic (_("_QUIT"));
  gtk_widget_set_name (btnQuit, "btnQuit");
  gtk_widget_ref (btnQuit);
  gtk_object_set_data_full (GTK_OBJECT (window), "btnQuit", btnQuit,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnQuit);
  gtk_fixed_put (GTK_FIXED (fixMain), btnQuit, 280, 368);
  gtk_widget_set_uposition (btnQuit, 272, 368);
  gtk_widget_set_usize (btnQuit, 95, 25);

  stoProfiles = gtk_list_store_new (1, G_TYPE_STRING);
  treProfiles = gtk_tree_view_new_with_model (GTK_TREE_MODEL (stoProfiles));
  g_object_unref (G_OBJECT (stoProfiles));
  rndProfiles = gtk_cell_renderer_text_new ();
  g_object_set (G_OBJECT (rndProfiles),
		  "foreground", "black",
		  NULL);
  colProfiles = gtk_tree_view_column_new_with_attributes (_("Profiles"), rndProfiles,
		  "text", 0, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treProfiles), colProfiles);
  gtk_widget_set_name (treProfiles, "treProfiles");
  gtk_widget_ref (treProfiles);
  gtk_object_set_data_full (GTK_OBJECT (window), "treProfiles", treProfiles,
                            (GtkDestroyNotify) gtk_widget_unref);
  selProfiles = gtk_tree_view_get_selection(GTK_TREE_VIEW (treProfiles));
  gtk_tree_selection_set_mode (selProfiles, GTK_SELECTION_SINGLE);
  gtk_widget_show (treProfiles);
  gtk_fixed_put (GTK_FIXED (fixMain), treProfiles, 379, 200);
  gtk_widget_set_uposition (treProfiles, 379, 200);
  gtk_widget_set_usize (treProfiles, 222, 201);

  lblNewInfo = gtk_label_new(NULL);
  gtk_widget_set_name (lblNewInfo, "lblNewInfo");
  gtk_widget_ref (lblNewInfo);
  gtk_label_set_line_wrap (GTK_LABEL (lblNewInfo), TRUE);
  gtk_label_set_markup (GTK_LABEL (lblNewInfo), 
		"<b>User Profiles</b>\n   Creating a user profile allows GGZ to always know that you are you.\n<b>Username</b>\n   The username you " \
		"choose will be what everyone knows you by. This username can not be changed once the account is created, so choose wisely.");
  gtk_object_set_data_full (GTK_OBJECT (window), "lblNewInfo", lblNewInfo,
		  	    (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (lblNewInfo);
  gtk_fixed_put (GTK_FIXED (fixMain), lblNewInfo, 290, 200);
  gtk_widget_set_uposition (lblNewInfo, 280, 200);
  gtk_widget_set_usize (lblNewInfo, 325, 201);
  gtk_misc_set_alignment (GTK_MISC (lblNewInfo), 0, 0);
  
  vbLogin = gtk_vbox_new (TRUE, 0);
  gtk_widget_set_name (vbLogin, "vbLogin");
  gtk_widget_ref (vbLogin);
  gtk_object_set_data_full (GTK_OBJECT (window), "vbLogin", vbLogin,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbLogin);
  gtk_fixed_put (GTK_FIXED (fixMain), vbLogin, 40, 200);
  gtk_widget_set_uposition (vbLogin, 40, 200);
  gtk_widget_set_usize (vbLogin, 222, 201);

  lblUsername = gtk_label_new (_("USERNAME"));
  gtk_widget_set_name (lblUsername, "lblUsername");
  gtk_widget_ref (lblUsername);
  gtk_object_set_data_full (GTK_OBJECT (window), "lblUsername", lblUsername,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (lblUsername);
  gtk_box_pack_start (GTK_BOX (vbLogin), lblUsername, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (lblUsername), 0, 0.5);

  entUsername = gtk_entry_new ();
  gtk_widget_set_name (entUsername, "entUsername");
  gtk_widget_ref (entUsername);
  gtk_object_set_data_full (GTK_OBJECT (window), "entUsername", entUsername,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entUsername);
  gtk_box_pack_start (GTK_BOX (vbLogin), entUsername, FALSE, FALSE, 0);

  lblPassword = gtk_label_new (_("PASSWORD"));
  gtk_widget_set_name (lblPassword, "lblPassword");
  gtk_widget_ref (lblPassword);
  gtk_object_set_data_full (GTK_OBJECT (window), "lblPassword", lblPassword,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (lblPassword);
  gtk_box_pack_start (GTK_BOX (vbLogin), lblPassword, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (lblPassword), 0, 0.5);

  entPassword = gtk_entry_new ();
  gtk_widget_set_name (entPassword, "entPassword");
  gtk_widget_ref (entPassword);
  gtk_object_set_data_full (GTK_OBJECT (window), "entPassword", entPassword,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_set_sensitive (entPassword, FALSE);
  gtk_widget_show (entPassword);
  gtk_box_pack_start (GTK_BOX (vbLogin), entPassword, FALSE, FALSE, 0);

  lblServer = gtk_label_new (_("SERVER"));
  gtk_widget_set_name (lblServer, "lblServer");
  gtk_widget_ref (lblServer);
  gtk_object_set_data_full (GTK_OBJECT (window), "lblServer", lblServer,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (lblServer);
  gtk_box_pack_start (GTK_BOX (vbLogin), lblServer, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (lblServer), 0, 0.5);

  entServer = gtk_entry_new ();
  gtk_widget_set_name (entServer, "entServer");
  gtk_widget_ref (entServer);
  gtk_object_set_data_full (GTK_OBJECT (window), "entServer", entServer,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entServer);
  gtk_box_pack_start (GTK_BOX (vbLogin), entServer, FALSE, FALSE, 0);

  hbLogin = gtk_hbox_new (FALSE, 0);
  gtk_widget_set_name (hbLogin, "hbLogin");
  gtk_widget_ref (hbLogin);
  gtk_object_set_data_full (GTK_OBJECT (window), "hbLogin", hbLogin,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbLogin);
  gtk_box_pack_start (GTK_BOX (vbLogin), hbLogin, TRUE, TRUE, 0);

  btnGuest = gtk_radio_button_new_with_mnemonic (hbLogin_group, _("_GUEST"));
  hbLogin_group = gtk_radio_button_group (GTK_RADIO_BUTTON (btnGuest));
  gtk_widget_set_name (btnGuest, "btnGuest");
  gtk_widget_ref (btnGuest);
  gtk_object_set_data_full (GTK_OBJECT (window), "btnGuest", btnGuest,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnGuest);
  gtk_box_pack_start (GTK_BOX (hbLogin), btnGuest, TRUE, TRUE, 0);

  btnRegistered = gtk_radio_button_new_with_mnemonic (hbLogin_group, _("_REGISTERED"));
  hbLogin_group = gtk_radio_button_group (GTK_RADIO_BUTTON (btnRegistered));
  gtk_widget_set_name (btnRegistered, "btnRegistered");
  gtk_widget_ref (btnRegistered);
  gtk_object_set_data_full (GTK_OBJECT (window), "btnRegistered", btnRegistered,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnRegistered);
  gtk_box_pack_start (GTK_BOX (hbLogin), btnRegistered, TRUE, TRUE, 0);

  vbNewLogin = gtk_vbox_new (TRUE, 0);
  gtk_widget_set_name (vbNewLogin, "vbNewLogin");
  gtk_widget_ref (vbNewLogin);
  gtk_object_set_data_full (GTK_OBJECT (window), "vbNewLogin", vbNewLogin,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbNewLogin);
  gtk_fixed_put (GTK_FIXED (fixMain), vbNewLogin, 50, 200);
  gtk_widget_set_uposition (vbNewLogin, 50, 200);
  gtk_widget_set_usize (vbNewLogin, 222, 201);

  lblNewProfileName = gtk_label_new (_("PROFILE NAME"));
  gtk_widget_set_name (lblNewProfileName, "lblNewProfielName");
  gtk_widget_ref (lblNewProfileName);
  gtk_object_set_data_full (GTK_OBJECT (window), "lblNewProfileName", lblNewProfileName,
			    (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (lblNewProfileName);
  gtk_box_pack_start (GTK_BOX (vbNewLogin), lblNewProfileName, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (lblNewProfileName), 0, 0.5);
  
  entNewProfileName = gtk_entry_new();
  gtk_widget_set_name (entNewProfileName, "entNewProfileName");
  gtk_object_set_data_full (GTK_OBJECT (window), "entNewProfileName", entNewProfileName,
		  	    (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entNewProfileName);
  gtk_box_pack_start (GTK_BOX (vbNewLogin), entNewProfileName, FALSE, FALSE, 0);
  
  lblNewUsername = gtk_label_new (_("USERNAME"));
  gtk_widget_set_name (lblNewUsername, "lblNewUsername");
  gtk_widget_ref (lblNewUsername);
  gtk_object_set_data_full (GTK_OBJECT (window), "lblNewUsername", lblNewUsername,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (lblNewUsername);
  gtk_box_pack_start (GTK_BOX (vbNewLogin), lblNewUsername, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (lblNewUsername), 0, 0.5);

  entNewUsername = gtk_entry_new ();
  gtk_widget_set_name (entNewUsername, "entNewUsername");
  gtk_widget_ref (entNewUsername);
  gtk_object_set_data_full (GTK_OBJECT (window), "entNewUsername", entNewUsername,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entNewUsername);
  gtk_box_pack_start (GTK_BOX (vbNewLogin), entNewUsername, FALSE, FALSE, 0);

  lblNewServer = gtk_label_new (_("SERVER"));
  gtk_widget_set_name (lblNewServer, "lblNewServer");
  gtk_widget_ref (lblNewServer);
  gtk_object_set_data_full (GTK_OBJECT (window), "lblNewServer", lblNewServer,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (lblNewServer);
  gtk_box_pack_start (GTK_BOX (vbNewLogin), lblNewServer, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (lblNewServer), 0, 0.5);

  entNewServer = gtk_entry_new ();
  gtk_widget_set_name (entNewServer, "entNewServer");
  gtk_widget_ref (entNewServer);
  gtk_object_set_data_full (GTK_OBJECT (window), "entNewServer", entNewServer,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entNewServer);
  gtk_box_pack_start (GTK_BOX (vbNewLogin), entNewServer, FALSE, FALSE, 0);

  hbNewLogin = gtk_hbox_new (TRUE, 0);
  gtk_widget_set_name (hbNewLogin, "hbNewLogin");
  gtk_widget_ref (hbNewLogin);
  gtk_object_set_data_full (GTK_OBJECT (window), "hbNewLogin", hbNewLogin,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbNewLogin);
  gtk_box_pack_end (GTK_BOX (vbNewLogin), hbNewLogin, FALSE, FALSE, 0);

  btnNewCreate = gtk_button_new_with_mnemonic (_("_CREATE"));
  gtk_widget_set_name (btnNewCreate, "btnNewCreate");
  gtk_widget_ref (btnNewCreate);
  gtk_object_set_data_full (GTK_OBJECT (window), "btnNewCreate", btnNewCreate,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnNewCreate);
  gtk_box_pack_start (GTK_BOX (hbNewLogin), btnNewCreate, FALSE, FALSE, 0);
  gtk_widget_set_usize (btnNewCreate, 80, -2);
  gtk_widget_set_sensitive (btnNewCreate, FALSE);

  btnNewCancel = gtk_button_new_with_mnemonic (_("C_ANCEL"));
  gtk_widget_set_name (btnNewCancel, "btnNewCancel");
  gtk_widget_ref (btnNewCancel);
  gtk_object_set_data_full (GTK_OBJECT (window), "btnNewCancel", btnNewCancel,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnNewCancel);
  gtk_box_pack_start (GTK_BOX (hbNewLogin), btnNewCancel, FALSE, FALSE, 0);
  gtk_widget_set_usize (btnNewCancel, 80, -2);

  btnNewAdvanced = gtk_button_new_with_mnemonic (_("A_DVANCED"));
  gtk_widget_set_name (btnNewAdvanced, "btnNewAdvanced");
  gtk_widget_ref (btnNewAdvanced);
  gtk_object_set_data_full (GTK_OBJECT (window), "btnNewAdvanced", btnNewAdvanced,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnNewAdvanced);
  gtk_fixed_put (GTK_FIXED (fixMain), btnNewAdvanced, 490, 369);
  gtk_widget_set_usize (btnNewCancel, 90, -2);

  stoPlayers = gtk_list_store_new (3, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_STRING);
  trePlayers = gtk_tree_view_new_with_model (GTK_TREE_MODEL (stoPlayers));
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (trePlayers), TRUE);
  g_object_unref (G_OBJECT (stoPlayers));
  rndPlayers = gtk_cell_renderer_text_new ();
  g_object_set (G_OBJECT (rndPlayers),
		"foreground", "black",
		NULL);
  colPlayers = gtk_tree_view_column_new_with_attributes (_("L"), rndPlayers,
		"text", 0, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (trePlayers), colPlayers);
  colPlayers = gtk_tree_view_column_new_with_attributes (_("T"), rndPlayers,
		  "text", 1, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (trePlayers), colPlayers);
  colPlayers = gtk_tree_view_column_new_with_attributes (_("Players"), rndPlayers,
		  "text", 2, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (trePlayers), colPlayers);
  gtk_widget_set_name (trePlayers, "trePlayers");
  gtk_widget_ref (trePlayers);
  gtk_object_set_data_full (GTK_OBJECT (window), "trePlayers", trePlayers,
		(GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (trePlayers);
  gtk_fixed_put (GTK_FIXED (fixMain), trePlayers, 21, 110);
  gtk_widget_set_uposition (trePlayers, 21, 110);
  gtk_widget_set_usize (trePlayers, 138, 230);

  vbChatButtons = gtk_vbox_new (TRUE, 5);
  gtk_widget_set_name (vbChatButtons, "vbChatButtons");
  gtk_widget_ref (vbChatButtons);
  gtk_object_set_data_full (GTK_OBJECT (window), "vbChatButtons", vbChatButtons,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbChatButtons);
  gtk_fixed_put (GTK_FIXED (fixMain), vbChatButtons, 21, 371);
  gtk_widget_set_uposition (vbChatButtons, 21, 371);
  gtk_widget_set_usize (vbChatButtons, 138, 87);

  btnChatChange = gtk_button_new_with_mnemonic (_("CHANGE _ROOM"));
  gtk_widget_set_name (btnChatChange, "btnChatChange");
  gtk_widget_ref (btnChatChange);
  gtk_object_set_data_full (GTK_OBJECT (window), "btnChatChange", btnChatChange,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnChatChange);
  gtk_box_pack_start (GTK_BOX (vbChatButtons), btnChatChange, TRUE, TRUE, 0);

  btnChatChat1 = gtk_button_new_with_mnemonic (_("_CHAT"));
  gtk_widget_set_name (btnChatChat1, "btnChatChat1");
  gtk_widget_ref (btnChatChat1);
  gtk_object_set_data_full (GTK_OBJECT (window), "btnChatChat1", btnChatChat1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnChatChat1);
  gtk_box_pack_start (GTK_BOX (vbChatButtons), btnChatChat1, TRUE, TRUE, 0);
  
  btnChatGame = gtk_button_new_with_mnemonic (_("_TABLES"));
  gtk_widget_set_name (btnChatGame, "btnChatGame");
  gtk_widget_ref (btnChatGame);
  gtk_object_set_data_full (GTK_OBJECT (window), "btnChatGame", btnChatGame,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnChatGame);
  gtk_box_pack_start (GTK_BOX (vbChatButtons), btnChatGame, TRUE, TRUE, 0);

  btnChatChat2 = gtk_button_new_with_mnemonic (_("_CHAT"));
  gtk_widget_set_name (btnChatChat2, "btnChatChat2");
  gtk_widget_ref (btnChatChat2);
  gtk_object_set_data_full (GTK_OBJECT (window), "btnChatChat2", btnChatChat2,
                          (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnChatChat2);
  gtk_box_pack_start (GTK_BOX (vbChatButtons), btnChatChat2, TRUE, TRUE, 0);

  btnChatLogoff = gtk_button_new_with_mnemonic (_("_LOGOFF"));
  gtk_widget_set_name (btnChatLogoff, "btnChatLogoff");
  gtk_widget_ref (btnChatLogoff);
  gtk_object_set_data_full (GTK_OBJECT (window), "btnChatLogoff", btnChatLogoff,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnChatLogoff);
  gtk_box_pack_start (GTK_BOX (vbChatButtons), btnChatLogoff, TRUE, TRUE, 0);

  vbChatDisplay = gtk_vbox_new (FALSE, 5);
  gtk_widget_set_name (vbChatDisplay, "vbChatDisplay");
  gtk_widget_ref (vbChatDisplay);
  gtk_object_set_data_full (GTK_OBJECT (window), "vbChatDisplay", vbChatDisplay,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbChatDisplay);
  gtk_fixed_put (GTK_FIXED (fixMain), vbChatDisplay, 191, 110);
  gtk_widget_set_uposition (vbChatDisplay, 191, 110);
  gtk_widget_set_usize (vbChatDisplay, 428, 348);

  nhChatXText = gtk_hbox_new (FALSE, 0);
  gtk_widget_set_name (nhChatXText, "nhChatXText");
  gtk_widget_ref (nhChatXText);
  gtk_object_set_data_full (GTK_OBJECT (window), "nhChatXText", nhChatXText,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (nhChatXText);
  gtk_box_pack_start (GTK_BOX (vbChatDisplay), nhChatXText, TRUE, TRUE, 0);

  fraChatXText = gtk_frame_new (NULL);
  gtk_widget_set_name (fraChatXText, "fraChatXText");
  gtk_widget_ref (fraChatXText);
  gtk_object_set_data_full (GTK_OBJECT (window), "fraChatXText", fraChatXText,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (fraChatXText);
  gtk_box_pack_start (GTK_BOX (nhChatXText), fraChatXText, TRUE, TRUE, 0);

  custChatXText = create_custChatXText ("custChatXText", NULL, NULL, 0, 0);
  gtk_widget_set_name (custChatXText, "custChatXText");
  gtk_widget_ref (custChatXText);
  gtk_object_set_data_full (GTK_OBJECT (window), "custChatXText", custChatXText,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (custChatXText);
  gtk_container_add (GTK_CONTAINER (fraChatXText), custChatXText);
  GTK_WIDGET_UNSET_FLAGS (custChatXText, GTK_CAN_FOCUS);
  GTK_WIDGET_UNSET_FLAGS (custChatXText, GTK_CAN_DEFAULT);

  vsrChatScroll = gtk_vscrollbar_new (GTK_XTEXT (custChatXText)->adj);
  gtk_widget_set_name (vsrChatScroll, "vsrChatScroll");
  gtk_widget_ref (vsrChatScroll);
  gtk_object_set_data_full (GTK_OBJECT (window), "vsrChatScroll", vsrChatScroll,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vsrChatScroll);
  gtk_box_pack_start (GTK_BOX (nhChatXText), vsrChatScroll, FALSE, TRUE, 0);

  hbChatEnter = gtk_hbox_new (FALSE, 5);
  gtk_widget_set_name (hbChatEnter, "hbChatEnter");
  gtk_widget_ref (hbChatEnter);
  gtk_object_set_data_full (GTK_OBJECT (window), "hbChatEnter", hbChatEnter,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbChatEnter);
  gtk_box_pack_start (GTK_BOX (vbChatDisplay), hbChatEnter, FALSE, FALSE, 0);

  entChatNew = gtk_entry_new ();
  gtk_widget_set_name (entChatNew, "entChatNew");
  gtk_widget_ref (entChatNew);
  gtk_object_set_data_full (GTK_OBJECT (window), "entChatNew", entChatNew,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entChatNew);
  gtk_box_pack_start (GTK_BOX (hbChatEnter), entChatNew, TRUE, TRUE, 0);

  btnChatSend = gtk_button_new_with_mnemonic (_("_SEND"));
  gtk_widget_set_name (btnChatSend, "btnChatSend");
  gtk_widget_ref (btnChatSend);
  gtk_object_set_data_full (GTK_OBJECT (window), "btnChatSend", btnChatSend,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnChatSend);
  gtk_box_pack_start (GTK_BOX (hbChatEnter), btnChatSend, FALSE, TRUE, 0);

  swRooms = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (swRooms);
  gtk_object_set_data_full (GTK_OBJECT (window), "swRooms", swRooms,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (swRooms);
  gtk_fixed_put (GTK_FIXED (fixMain), swRooms, 191, 110);
  gtk_widget_set_uposition (swRooms, 191, 112);
  gtk_widget_set_usize (swRooms, 428, 348);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swRooms), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  vpRooms = gtk_viewport_new (NULL, NULL);
  gtk_widget_ref (vpRooms);
  gtk_object_set_data_full (GTK_OBJECT (window), "vpRooms", vpRooms,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vpRooms);
  gtk_container_add (GTK_CONTAINER (swRooms), vpRooms);

  vbRooms = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbRooms);
  gtk_object_set_data_full (GTK_OBJECT (window), "vbRooms", vbRooms,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbRooms);
  gtk_container_add (GTK_CONTAINER (vpRooms), vbRooms);

  vbTables = gtk_vbox_new (FALSE, 0);
  gtk_object_set_data_full (GTK_OBJECT (window), "vbTables", vbTables,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbTables);
  gtk_fixed_put (GTK_FIXED (fixMain), vbTables, 191, 110);
  gtk_widget_set_usize (vbTables, 428, 348);

  stoTables = gtk_list_store_new (4, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_UINT, G_TYPE_STRING);
  treTables = gtk_tree_view_new_with_model (GTK_TREE_MODEL (stoTables));
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treTables), TRUE);
  g_object_unref (G_OBJECT (stoTables));
  rndTables = gtk_cell_renderer_text_new ();
  g_object_set (G_OBJECT (rndTables),
		"foreground", "black",
		NULL);
  colTables = gtk_tree_view_column_new_with_attributes (_("ID"), rndTables,
		"text", 0, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treTables), colTables);
  colTables = gtk_tree_view_column_new_with_attributes (_("Free"), rndTables,
		"text", 1, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treTables), colTables);
  colTables = gtk_tree_view_column_new_with_attributes (_("Total"), rndTables,
		  "text", 2, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treTables), colTables);
  colTables = gtk_tree_view_column_new_with_attributes (_("Description"), rndTables,
		  "text", 3, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treTables), colTables);
  gtk_widget_set_name (treTables, "treTables");
  gtk_widget_ref (treTables);
  gtk_object_set_data_full (GTK_OBJECT (window), "treTables", treTables,
		(GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (treTables);
  gtk_box_pack_start (GTK_BOX (vbTables), treTables, TRUE, TRUE, 0);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_set_name (hbuttonbox1, "hbuttonbox1");
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (vbTables), hbuttonbox1, FALSE, FALSE, 5);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (hbuttonbox1), 0);

  btnLaunch = gtk_button_new_with_mnemonic (_("Launch Game"));
  gtk_widget_set_name (btnLaunch, "btnLaunch");
  gtk_widget_show (btnLaunch);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), btnLaunch);
  GTK_WIDGET_SET_FLAGS (btnLaunch, GTK_CAN_DEFAULT);

  btnJoin = gtk_button_new_with_mnemonic (_("Join Game"));
  gtk_widget_set_name (btnJoin, "btnJoin");
  gtk_object_set_data_full (GTK_OBJECT (window), "btnJoin", btnJoin,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnJoin);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), btnJoin);
  GTK_WIDGET_SET_FLAGS (btnLaunch, GTK_CAN_DEFAULT);

  btnWatch = gtk_button_new_with_mnemonic (_("Watch Game"));
  gtk_widget_set_name (btnWatch, "btnWatch");
  gtk_object_set_data_full (GTK_OBJECT (window), "btnWatch", btnWatch,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnWatch);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), btnWatch);
  GTK_WIDGET_SET_FLAGS (btnLaunch, GTK_CAN_DEFAULT);

  pmStar1 = create_pixmap (window, "star1.xpm");
  gtk_widget_set_name (pmStar1, "pmStar1");
  gtk_widget_ref (pmStar1);
  gtk_object_set_data_full (GTK_OBJECT (window), "pmStar1", pmStar1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pmStar1);
  gtk_fixed_put (GTK_FIXED (fixMain), pmStar1, 200, 432);
  gtk_widget_set_uposition (pmStar1, 230, 450);
  gtk_widget_set_usize (pmStar1, 21, 21);

  pmStar2 = create_pixmap (window, "star2.xpm");
  gtk_widget_set_name (pmStar2, "pmStar2");
  gtk_widget_ref (pmStar2);
  gtk_object_set_data_full (GTK_OBJECT (window), "pmStar2", pmStar2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pmStar2);
  gtk_fixed_put (GTK_FIXED (fixMain), pmStar2, 200, 432);
  gtk_widget_set_uposition (pmStar2, 230, 450);
  gtk_widget_set_usize (pmStar2, 21, 21);

  pmStar3 = create_pixmap (window, "star3.xpm");
  gtk_widget_set_name (pmStar3, "pmStar3");
  gtk_widget_ref (pmStar3);
  gtk_object_set_data_full (GTK_OBJECT (window), "pmStar3", pmStar3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pmStar3);
  gtk_fixed_put (GTK_FIXED (fixMain), pmStar3, 200, 432);
  gtk_widget_set_uposition (pmStar3, 230, 450);
  gtk_widget_set_usize (pmStar3, 21, 21);

  pmStar4 = create_pixmap (window, "star4.xpm");
  gtk_widget_set_name (pmStar4, "pmStar4");
  gtk_widget_ref (pmStar4);
  gtk_object_set_data_full (GTK_OBJECT (window), "pmStar4", pmStar4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pmStar4);
  gtk_fixed_put (GTK_FIXED (fixMain), pmStar4, 200, 432);
  gtk_widget_set_uposition (pmStar4, 230, 450);
  gtk_widget_set_usize (pmStar4, 21, 21);

  pmStar5 = create_pixmap (window, "star5.xpm");
  gtk_widget_set_name (pmStar5, "pmStar5");
  gtk_widget_ref (pmStar5);
  gtk_object_set_data_full (GTK_OBJECT (window), "pmStar5", pmStar5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pmStar5);
  gtk_fixed_put (GTK_FIXED (fixMain), pmStar5, 200, 432);
  gtk_widget_set_uposition (pmStar5, 230, 450);
  gtk_widget_set_usize (pmStar5, 21, 21);

  login_set_login(window);

  gtk_signal_connect (GTK_OBJECT (window), "delete_event",
		      GTK_SIGNAL_FUNC (on_window_delete_event),
		      NULL);
  gtk_signal_connect (GTK_OBJECT (window), "destroy_event",
                      GTK_SIGNAL_FUNC (on_window_destroy_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnLogin), "clicked",
                      GTK_SIGNAL_FUNC (on_btnLogin_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnNew), "clicked",
                      GTK_SIGNAL_FUNC (on_btnProfiles_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnQuit), "clicked",
                      GTK_SIGNAL_FUNC (gtk_main_quit),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnGuest), "clicked",
                      GTK_SIGNAL_FUNC (on_entLogin_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnRegistered), "clicked",
                      GTK_SIGNAL_FUNC (on_entLogin_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (entUsername), "changed",
		      GTK_SIGNAL_FUNC (on_entLogin_changed),
		      NULL);
  gtk_signal_connect (GTK_OBJECT (entPassword), "changed",
		      GTK_SIGNAL_FUNC (on_entLogin_changed),
		      NULL);
  gtk_signal_connect (GTK_OBJECT (entServer), "changed",
		      GTK_SIGNAL_FUNC (on_entLogin_changed),
		      NULL);
  gtk_signal_connect (GTK_OBJECT (pmBackground), "draw",
                      GTK_SIGNAL_FUNC (on_pmBackground_draw),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (pmBackground), "expose_event",
                      GTK_SIGNAL_FUNC (on_pmBackground_expose_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (pmNewBackground), "draw",
                      GTK_SIGNAL_FUNC (on_pmBackground_draw),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (pmNewBackground), "expose_event",
                      GTK_SIGNAL_FUNC (on_pmBackground_expose_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (pmChatBackground), "draw",
		      GTK_SIGNAL_FUNC (on_pmBackground_draw),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (pmChatBackground), "expose_event",
                      GTK_SIGNAL_FUNC (on_pmBackground_expose_event),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnNewCreate), "clicked",
                      GTK_SIGNAL_FUNC (on_btnNewCreate_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnNewCancel), "clicked",
                      GTK_SIGNAL_FUNC (on_btnNewCancel_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnNewAdvanced), "clicked",
                      GTK_SIGNAL_FUNC (on_btnNewAdvanced_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (entNewProfileName), "changed",
		      GTK_SIGNAL_FUNC (on_entLogin_changed),
		      NULL);
  gtk_signal_connect (GTK_OBJECT (entNewUsername), "changed",
                      GTK_SIGNAL_FUNC (on_entLogin_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (entNewServer), "changed",
                      GTK_SIGNAL_FUNC (on_entLogin_changed),
                      NULL); 
  g_signal_connect (G_OBJECT (selProfiles), "changed",
		      G_CALLBACK (on_tree_selection_changed),
		      NULL);
  gtk_signal_connect (GTK_OBJECT (trePlayers), "motion_notify_event",
		      GTK_SIGNAL_FUNC (on_player_tree_motion_notify_event),
		      &tipsPlayer);
  gtk_signal_connect (GTK_OBJECT (trePlayers), "enter_notify_event",
		      GTK_SIGNAL_FUNC (on_player_tree_enter_notify_event),
		      &tipsPlayer);
  gtk_signal_connect (GTK_OBJECT (trePlayers), "leave_notify_event",
		      GTK_SIGNAL_FUNC (on_player_tree_leave_notify_event),
		      &tipsPlayer);
  gtk_signal_connect (GTK_OBJECT (btnChatChange), "clicked",
                      GTK_SIGNAL_FUNC (on_btnChatChange_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnChatChat1), "clicked",
                      GTK_SIGNAL_FUNC (on_btnChatChat_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnChatGame), "clicked",
                      GTK_SIGNAL_FUNC (on_btnChatGame_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnChatChat2), "clicked",
		      GTK_SIGNAL_FUNC (on_btnChatChat_clicked),
		      NULL);
  gtk_signal_connect (GTK_OBJECT (btnChatLogoff), "clicked",
                      GTK_SIGNAL_FUNC (on_btnChatLogoff_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnLaunch), "clicked",
                      GTK_SIGNAL_FUNC (on_btnLaunch_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnJoin), "clicked",
                      GTK_SIGNAL_FUNC (on_btnJoin_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnWatch), "clicked",
                      GTK_SIGNAL_FUNC (on_btnWatch_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnChatSend), "clicked",
                      GTK_SIGNAL_FUNC (on_btnChatSend_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (entChatNew), "activate",
		      GTK_SIGNAL_FUNC (on_entCharNew_activate),
		      NULL);

  gtk_timeout_add(40, on_player_tree_timeout, &tipsPlayer);

  return window;
}
