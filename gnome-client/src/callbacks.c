#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <ggz.h>
#include <ggzcore.h>
#include <gconf/gconf-client.h>

#include "callbacks.h"
#include "interface.h"
#include "login.h"
#include "support.h"
#include "xtext.h"
#include "chat.h"
#include "profilesi.h"
#include "game.h"

extern GtkWidget* interface;
extern GdkColor colors[];
extern GGZServer *server;
extern GConfClient *config;

gboolean
on_player_tree_popup_leave_notify_event	(GtkWidget	*widget,
					 GdkEventCrossing *event,
					 gpointer	*user_data);


gboolean
on_window_destroy_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  return FALSE;
}


gboolean
on_window_delete_event                (GtkWidget       *widget,
                                       GdkEvent        *event,
                                       gpointer         user_data)
{

  return TRUE;
}


void
on_entLogin_changed			(GtkEntry       *widget,
					 gpointer       *user_data)
{

	static gint form[6] = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};
	gint val = 0;
	const gchar *text;
	GtkWidget *tmp;
	
	/*What widget we dealin with?*/
	if (!strcmp (gtk_widget_get_name (GTK_WIDGET (widget)), "entUsername")) {
		val = 0;
	} else if (!strcmp (gtk_widget_get_name (GTK_WIDGET (widget)), "entPassword")) {
		val = 1;
	} else if (!strcmp (gtk_widget_get_name (GTK_WIDGET (widget)), "entServer")) {
		val = 2;
	} else if (!strcmp (gtk_widget_get_name (GTK_WIDGET (widget)), "entNewProfileName")) {
		val = 3;
	} else if (!strcmp (gtk_widget_get_name (GTK_WIDGET (widget)), "entNewUsername")) {
		val = 4;
	} else if (!strcmp (gtk_widget_get_name (GTK_WIDGET (widget)), "entNewServer")) {
		val = 5;
	} else if (!strcmp (gtk_widget_get_name (GTK_WIDGET (widget)), "btnGuest")) {
		val = 6;
		tmp = lookup_widget (interface, "entPassword");
		gtk_widget_set_sensitive (GTK_WIDGET(tmp), FALSE);
	} else if (!strcmp (gtk_widget_get_name (GTK_WIDGET (widget)), "btnRegistered")) {
		val = 6;
		tmp = lookup_widget (interface, "entPassword");
		gtk_widget_set_sensitive (GTK_WIDGET(tmp), TRUE);
	}
	
	/*Set the state of the entry*/
	if (val !=6)
	{
		text = gtk_entry_get_text (GTK_ENTRY (widget));
		form[val] = (strlen(text) > 0);
	}

	/*Set the buttons sensitivity*/
	if (form[0] && form[1] && form[2])
	{
		tmp = lookup_widget (interface, "btnLogin");
		gtk_widget_set_sensitive (GTK_WIDGET(tmp), TRUE);
		return;
	} else {
		tmp = lookup_widget (interface, "btnLogin");
		gtk_widget_set_sensitive (GTK_WIDGET(tmp), FALSE);
	}
	tmp = lookup_widget (interface, "btnGuest");
	if (form[0] && form[2] &&
	    gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(tmp)))
	{
		tmp = lookup_widget (interface, "btnLogin");
		gtk_widget_set_sensitive (GTK_WIDGET(tmp), TRUE);
		return;
	} else {
		tmp = lookup_widget (interface, "btnLogin");
		gtk_widget_set_sensitive (GTK_WIDGET(tmp), FALSE);
	}
	tmp = lookup_widget (interface, "btnNewCreate");
	if (form[3] && form[4] && form[5])
	{
		gtk_widget_set_sensitive (GTK_WIDGET (tmp), TRUE);
		return;
	} else {
		gtk_widget_set_sensitive (GTK_WIDGET(tmp), FALSE);
	}
	
}


void
on_btnLogin_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	gchar **host = NULL, *username = NULL, *password = NULL;
	gint port = 5688;
	GtkWidget *tmp;
	GGZLoginType type = GGZ_LOGIN_GUEST;

	/* Get connection and login data */
	tmp = lookup_widget(interface, "entServer");
	host = g_strsplit(gtk_entry_get_text(GTK_ENTRY(tmp)), ":", 2);
	
	if(host[1] != NULL)
		port = atoi(host[1]);
	
	tmp = lookup_widget(interface, "entUsername");
	username = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp)));

	tmp = lookup_widget(interface, "btnRegistered");
	if (GTK_TOGGLE_BUTTON (tmp)->active)
	{
		type = GGZ_LOGIN;
		tmp = lookup_widget(interface, "entPassword");
		password = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp)));
	}
	tmp = lookup_widget(interface, "btnGuest");
	if (GTK_TOGGLE_BUTTON (tmp)->active)
		type = GGZ_LOGIN_GUEST;

	login_start_session(host[0], port, username, password, type);
	g_strfreev(host);
	g_free(username);
	g_free(password);
}


void
on_btnProfiles_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
	login_set_new(interface);
}


void
on_btnNewCreate_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
        gchar **host = NULL, *username = NULL, *password = NULL;
        gint port = 5688;
        GtkWidget *tmp;
        GGZLoginType type = GGZ_LOGIN_GUEST;

	/* Get connection and login data */
        tmp = lookup_widget(interface, "entNewServer");
        host = g_strsplit(gtk_entry_get_text(GTK_ENTRY(tmp)), ":", 2);

	if(host[1] != NULL)
	        port = atoi(host[1]);

	tmp = lookup_widget(interface, "entNewUsername");
        username = g_strdup(gtk_entry_get_text(GTK_ENTRY(tmp)));

        type = GGZ_LOGIN_NEW;

        login_start_session(host[0], port, username, password, type);
        g_strfreev(host);
        g_free(username);
        g_free(password);
																	
}


void
on_btnNewCancel_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
	login_set_login(interface);
}


void
on_btnNewAdvanced_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *winProfile;
	GtkWidget *tmp;
	GtkTreeModel *stoProfiles;
	GtkTreeIter iter;
	gint intProfiles;
	GSList *slProfiles = NULL;
	gint x;

	winProfile = create_winProfiles ();
	gtk_widget_show(winProfile);

	/*Clear and re-fill the profile list*/
	tmp = lookup_widget(winProfile, "treProfiles");
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
}


void
on_tree_selection_changed 		(GtkTreeSelection *selection, 
					 gpointer data)
{
	GtkWidget *tmp;
	GtkTreeIter iter;
	GtkTreeModel *model;
	gchar *strTmp, *strProfile;
	GSList *slProfileNames, *slProfileUsernames, *slProfileServers, *slProfilePWDs;
	gint intProfiles, count;
	
	if (gtk_tree_selection_get_selected (selection, &model, &iter))
	{
		gtk_tree_model_get (model, &iter, 0, &strProfile, -1);
               
		slProfileNames = gconf_client_get_list (config, "/schemas/apps/ggz-gnome/profiles/profiles", GCONF_VALUE_STRING, NULL);
                slProfileUsernames = gconf_client_get_list (config, "/schemas/apps/ggz-gnome/profiles/usernames", GCONF_VALUE_STRING, NULL);
                slProfileServers = gconf_client_get_list (config, "/schemas/apps/ggz-gnome/profiles/servers", GCONF_VALUE_STRING, NULL);
                slProfilePWDs = gconf_client_get_list (config, "/schemas/apps/ggz-gnome/profiles/passwords", GCONF_VALUE_STRING, NULL);
                intProfiles = gconf_client_get_int (config, "/schemas/apps/ggz-gnome/profiles/total", NULL);
		
		for (count = 0; count < intProfiles; count++)
		{
			strTmp = g_strdup(g_slist_nth_data (slProfileNames, count));
			if (!strcmp(strTmp, strProfile))
			{
				tmp = lookup_widget (interface, "entUsername");
				gtk_entry_set_text (GTK_ENTRY (tmp), g_slist_nth_data (slProfileUsernames, count));
				tmp = lookup_widget (interface, "entPassword");
				gtk_entry_set_text (GTK_ENTRY (tmp), g_slist_nth_data (slProfilePWDs, count));
				tmp = lookup_widget (interface, "entServer");
				gtk_entry_set_text (GTK_ENTRY (tmp), g_slist_nth_data (slProfileServers, count));

				tmp = lookup_widget (interface, "btnRegistered");
				gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (tmp), TRUE);
				
			}
			g_free(strTmp);
		}
		
		g_free (strProfile);
	}
}


gint
on_player_tree_timeout                  (gpointer         user_data)
{
	FakeTips *tips;

	tips = (FakeTips*)user_data;
	if (tips->mouse && tips->timeout < 27)
		tips->timeout++;

	if (tips->timeout == 1)
	{
		if (tips->menu)
		{
			gtk_widget_destroy(tips->menu);
			tips->item = NULL;
			tips->menu = NULL;
		}
	}

	if (tips->timeout == 25)
	{
		tips->menu = gtk_menu_new();
		tips->item = gtk_menu_item_new_with_label("test");
		gtk_widget_show(tips->item);
		gtk_container_add (GTK_CONTAINER (tips->menu), tips->item);
		gtk_signal_connect (GTK_OBJECT (tips->menu), "leave_notify_event",
				    GTK_SIGNAL_FUNC (on_player_tree_popup_leave_notify_event),
				    user_data);
		gtk_menu_popup (GTK_MENU (tips->menu), NULL, NULL, NULL, NULL, 0, gtk_get_current_event_time());
	}

	return TRUE;
}


gboolean
on_player_tree_popup_leave_notify_event	(GtkWidget	*widget,
					 GdkEventCrossing *event,
					 gpointer	*user_data)
{
	FakeTips *tips;
	static gint count = 0;

	tips = (FakeTips*)user_data;
	if (count < 6)
	{
		count++;
	} else {
		count = 0;
		gtk_menu_popdown(GTK_MENU(tips->menu));
	}


	return TRUE;
}


gboolean
on_player_tree_motion_notify_event	(GtkWidget	*widget,
					 GdkEventMotion  *event,
					 gpointer	*user_data)
{
	FakeTips *tips;

	tips = (FakeTips*)user_data;
	tips->timeout=0;

	return TRUE;
}


gboolean
on_player_tree_enter_notify_event	(GtkWidget	*widget,
					 GdkEventCrossing *event,
					 gpointer	*user_data)
{
	FakeTips *tips;

	tips = (FakeTips*)user_data;
	tips->timeout=0;
	tips->mouse = TRUE;

	return TRUE;
}


gboolean
on_player_tree_leave_notify_event	(GtkWidget	*widget,
					 GdkEventCrossing *event,
					 gpointer	*user_data)
{
	FakeTips *tips;

	tips = (FakeTips*)user_data;
	tips->timeout=0;
	tips->mouse = FALSE;

	return TRUE;
}


void
on_btnChatChange_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
	login_set_chat_rooms(interface);
}


void
on_btnChatGame_clicked               (GtkButton       *button,
                                      gpointer         user_data)
{
	GGZRoom *room;
	GGZGameType *type;
	GtkWidget *tmp;

	room = ggzcore_server_get_cur_room(server);
	if (room)
	{
		type = ggzcore_room_get_gametype(room);
		tmp = lookup_widget(interface, "btnWatch");
		if(ggzcore_gametype_get_spectators_allowed(type))
			gtk_widget_set_sensitive(GTK_WIDGET(tmp), TRUE);
		else
			gtk_widget_set_sensitive(GTK_WIDGET(tmp), FALSE);
		login_set_chat_games(interface);
	}
}


void
on_btnChatLogoff_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
	login_disconnect();
}


void
on_btnChatChat_clicked               (GtkButton       *button,
                                      gpointer         user_data)
{
	login_set_chat(interface);
}


GtkWidget*
create_custChatXText (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2)
{
	GtkXText *tmp;
	GdkPixmap *background;
	GdkColormap *system;
	gchar *bgfile;
	
	tmp = GTK_XTEXT(gtk_xtext_new (colors, TRUE));
	gtk_xtext_set_font(tmp, "-*-fixed-medium-r-semicondensed--*-120-*-*-c-*-iso8859-1");
	system = gdk_colormap_get_system();
	bgfile = g_strdup_printf("%s/ggz-gnome/Default/chatbackground.xpm", GGZDATADIR);
	background = gdk_pixmap_colormap_create_from_xpm(NULL, system, NULL, NULL, bgfile);
	g_free (bgfile);
	gtk_xtext_set_background(tmp, background, 0, 0);
	tmp->auto_indent = TRUE;
	tmp->wordwrap = TRUE;
	tmp->max_auto_indent = 200;
	gtk_xtext_set_palette (tmp, colors);

	return GTK_WIDGET(tmp);
}


void
on_btnChatSend_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *tmp;

	tmp = lookup_widget(interface, "entChatNew");
	chat_send((gchar*)gtk_entry_get_text(GTK_ENTRY(tmp)));
	gtk_entry_set_text(GTK_ENTRY(tmp), "");

}


void
on_btnLaunch_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_btnJoin_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *tmp;
	GtkTreeSelection *selection;
	GtkTreeIter iter;
	GtkTreeModel *model;
	gint id, open, total;
	gchar *desc;
	char *name;
	char *engine;
	char *version;
	GGZRoom *room;
	GGZGameType *type;
	GGZModule *module;

	/* Get current row */
	tmp = lookup_widget (interface, "treTables");
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(tmp));

	if (gtk_tree_selection_get_selected (selection, &model, &iter))
	{
		room = ggzcore_server_get_cur_room(server);
		if (!room) 
		{
			g_print ("You must be in a room to launch a game\n");
			return;
		}

		type = ggzcore_room_get_gametype(room);
		if (!type) 
		{
			g_print ("No game types defined for this room\n");
			return;
		}
	
		name = ggzcore_gametype_get_name(type);
		engine = ggzcore_gametype_get_prot_engine(type);
		version = ggzcore_gametype_get_prot_version(type);
		module = ggzcore_module_get_nth_by_type(name, engine, version, 0);
		if (!module) 
		{
			g_print ("No game modules defined for that game\n");
			g_print ("Download one from %s\n", ggzcore_gametype_get_url(type));
			return;
		}

		gtk_tree_model_get (model, &iter, 0, &id, 1, &open, 1, &total, 0, &desc, -1);
		game_init(module, type, id, FALSE);
	}
	login_set_chat(interface);
}


void
on_btnWatch_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *tmp;
	GtkTreeSelection *selection;
	GtkTreeIter iter;
	GtkTreeModel *model;
	gint id, open, total;
	gchar *desc;
	char *name;
	char *engine;
	char *version;
	GGZRoom *room;
	GGZGameType *type;
	GGZModule *module;

	/* Get current row */
	tmp = lookup_widget (interface, "treTables");
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(tmp));

	if (gtk_tree_selection_get_selected (selection, &model, &iter))
	{
		room = ggzcore_server_get_cur_room(server);
		if (!room) 
		{
			g_print ("You must be in a room to launch a game\n");
			return;
		}

		type = ggzcore_room_get_gametype(room);
		if (!type) 
		{
			g_print ("No game types defined for this room\n");
			return;
		}
	
		name = ggzcore_gametype_get_name(type);
		engine = ggzcore_gametype_get_prot_engine(type);
		version = ggzcore_gametype_get_prot_version(type);
		module = ggzcore_module_get_nth_by_type(name, engine, version, 0);
		if (!module) 
		{
			g_print ("No game modules defined for that game\n");
			g_print ("Download one from %s\n", ggzcore_gametype_get_url(type));
			return;
		}

		gtk_tree_model_get (model, &iter, 0, &id, 1, &open, 1, &total, 0, &desc, -1);
		game_init(module, type, id, TRUE);
	}
	login_set_chat(interface);
}


void
on_entCharNew_activate                 (GtkEntry        *entry,
                                        gpointer         user_data)
{
	GtkWidget *tmp;
	
	tmp = lookup_widget(interface, "entChatNew");
	if(strcmp(gtk_entry_get_text(GTK_ENTRY(tmp)), ""))
	{
		chat_send((gchar*)gtk_entry_get_text(GTK_ENTRY(tmp)));
		gtk_entry_set_text(GTK_ENTRY(tmp), "");
	}
}


gboolean
on_room_click                             (GtkWidget       *widget,
                                           GdkEventButton  *event,
                                           gpointer         user_data)
{
	gchar *strRoomNum;
	gint intRoomNum;
	
	if (event->type == GDK_BUTTON_PRESS)
	{
		strRoomNum = (gchar*)gtk_widget_get_name (widget);
		intRoomNum = atoi (strRoomNum);
		ggzcore_server_join_room (server, intRoomNum);
		login_set_chat(interface);
	}

	return FALSE;
}


gint
on_pmStar_timeout                 (gpointer         user_data)
{
	GtkWidget *tmp;
	static gint counter;
	static gint countup = TRUE;
	
	if(countup)
		counter++;
	else
		counter--;
	
	if(counter == 100)
		countup = FALSE;
	if(counter == 1)
		countup = TRUE;

	tmp = lookup_widget(interface, "pmBackground");
	if(GTK_WIDGET_VISIBLE(tmp) && GDK_IS_DRAWABLE(tmp))
	switch (counter)
	{
		case 40:
			tmp = lookup_widget(interface, "pmStar1");
			gtk_widget_show(tmp);
			tmp = lookup_widget(interface, "pmStar2");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar3");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar4");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar5");
			gtk_widget_hide(tmp);
			break;
		case 31:
			tmp = lookup_widget(interface, "pmStar1");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar2");
			gtk_widget_show(tmp);
			tmp = lookup_widget(interface, "pmStar3");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar4");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar5");
			gtk_widget_hide(tmp);
			break;
		case 32:
			tmp = lookup_widget(interface, "pmStar1");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar2");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar3");
			gtk_widget_show(tmp);
			tmp = lookup_widget(interface, "pmStar4");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar5");
			gtk_widget_hide(tmp);
			break;
		case 33:
			tmp = lookup_widget(interface, "pmStar1");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar2");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar3");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar4");
			gtk_widget_show(tmp);
			tmp = lookup_widget(interface, "pmStar5");
			gtk_widget_hide(tmp);
			break;
		case 34:
			tmp = lookup_widget(interface, "pmStar1");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar2");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar3");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar4");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar5");
			gtk_widget_show(tmp);
			break;
		case 35:
			tmp = lookup_widget(interface, "pmStar1");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar2");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar3");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar4");
			gtk_widget_show(tmp);
			tmp = lookup_widget(interface, "pmStar5");
			gtk_widget_hide(tmp);
			break;
		case 36:
			tmp = lookup_widget(interface, "pmStar1");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar2");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar3");
			gtk_widget_show(tmp);
			tmp = lookup_widget(interface, "pmStar4");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar5");
			gtk_widget_hide(tmp);
			break;
		case 37:
			tmp = lookup_widget(interface, "pmStar1");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar2");
			gtk_widget_show(tmp);
			tmp = lookup_widget(interface, "pmStar3");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar4");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar5");
			gtk_widget_hide(tmp);
			break;
		case 38:
			tmp = lookup_widget(interface, "pmStar1");
			gtk_widget_show(tmp);
			tmp = lookup_widget(interface, "pmStar2");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar3");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar4");
			gtk_widget_hide(tmp);
			tmp = lookup_widget(interface, "pmStar5");
			gtk_widget_hide(tmp);
			break;
	}
		
	return TRUE;
}


void
on_pmBackground_draw                   (GtkWidget       *widget,
                                        GdkRectangle    *area,
                                        gpointer         user_data)
{

}


gboolean
on_pmBackground_expose_event           (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{

	return FALSE;
}

