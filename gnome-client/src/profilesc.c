#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <gconf/gconf-client.h>

#include "profilesc.h"
#include "profilesi.h"
#include "support.h"

extern GConfClient *config;

void
on_entries_changed                     (GtkWidget     *widget,
                                        gpointer         user_data)
{
	static gint form[4] = {FALSE, FALSE, FALSE, FALSE};
	gint val;
	const gchar *text;
	GtkWidget *tmp;
	GtkWidget *winProfile = GTK_WIDGET(user_data);

	/*What widget we dealin with?*/
	if (!strcmp (gtk_widget_get_name (GTK_WIDGET (widget)), "entUsername")) {
		val = 0;
	} else if (!strcmp (gtk_widget_get_name (GTK_WIDGET (widget)), "entPassword")) {
		val = 1;
	} else if (!strcmp (gtk_widget_get_name (GTK_WIDGET (widget)), "entServer")) {
		val = 2;
	} else if (!strcmp (gtk_widget_get_name (GTK_WIDGET (widget)), "entProfileName")) {
		val = 3;
	}
	
	/*Set the state of the entry*/
	text = gtk_entry_get_text (GTK_ENTRY (widget));
	form[val] = (strlen(text) > 0);

	/*Set the buttons sensitivity*/
	if (form[0] && form[1] && form[2] && form[3])
	{
		tmp = lookup_widget (winProfile, "btnNew");
		gtk_widget_set_sensitive (GTK_WIDGET (tmp), TRUE);
		tmp = lookup_widget (winProfile, "btnApply");
		gtk_widget_set_sensitive (GTK_WIDGET (tmp), TRUE);
		tmp = lookup_widget (winProfile, "btnDelete");
		gtk_widget_set_sensitive (GTK_WIDGET (tmp), TRUE);
		return;
	} else {
		tmp = lookup_widget (winProfile, "btnNew");
		gtk_widget_set_sensitive (GTK_WIDGET (tmp), FALSE);
		tmp = lookup_widget (winProfile, "btnApply");
		gtk_widget_set_sensitive (GTK_WIDGET (tmp), FALSE);
		tmp = lookup_widget (winProfile, "btnDelete");
		gtk_widget_set_sensitive (GTK_WIDGET (tmp), FALSE);
	}
}


void
on_btnNew_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
	GSList *slProfileNames, *slProfileUsernames, *slProfileServers, *slProfilePWDs;
	gint intProfiles, x;
	GtkTreeModel *stoProfiles;
	GtkTreeIter iter;
	GSList *slProfiles = NULL;
	gint curItem;
	GtkWidget *tmp;
	GtkWidget *winProfile = GTK_WIDGET(user_data);

	/*Get current information*/
	slProfileNames = gconf_client_get_list (config, "/schemas/apps/ggz-gnome/profiles/profiles", GCONF_VALUE_STRING, NULL);
	slProfileUsernames = gconf_client_get_list (config, "/schemas/apps/ggz-gnome/profiles/usernames", GCONF_VALUE_STRING, NULL);
	slProfileServers = gconf_client_get_list (config, "/schemas/apps/ggz-gnome/profiles/servers", GCONF_VALUE_STRING, NULL);
	slProfilePWDs = gconf_client_get_list (config, "/schemas/apps/ggz-gnome/profiles/passwords", GCONF_VALUE_STRING, NULL);
	intProfiles = gconf_client_get_int (config, "/schemas/apps/ggz-gnome/profiles/total", NULL);

	/*Update the lists*/
	tmp = lookup_widget (winProfile, "entProfileName");
	slProfileNames = g_slist_append (slProfileNames,
		(char*)gtk_entry_get_text (GTK_ENTRY(tmp)));
	tmp = lookup_widget (winProfile, "entUsername");
	slProfileUsernames = g_slist_append (slProfileUsernames,
		(char*)gtk_entry_get_text (GTK_ENTRY(tmp)));
	tmp = lookup_widget (winProfile, "entServer");
	slProfileServers = g_slist_append (slProfileServers,
		(char*)gtk_entry_get_text (GTK_ENTRY(tmp)));
	tmp = lookup_widget (winProfile, "entPassword");
	slProfilePWDs = g_slist_append (slProfilePWDs,
		(char*)gtk_entry_get_text (GTK_ENTRY(tmp)));
	intProfiles++;

	/*Store the information back*/
	gconf_client_set_list (config, "/schemas/apps/ggz-gnome/profiles/profiles", GCONF_VALUE_STRING, slProfileNames, NULL);
	gconf_client_set_list (config, "/schemas/apps/ggz-gnome/profiles/usernames", GCONF_VALUE_STRING, slProfileUsernames, NULL);
	gconf_client_set_list (config, "/schemas/apps/ggz-gnome/profiles/servers", GCONF_VALUE_STRING, slProfileServers, NULL);
	gconf_client_set_list (config, "/schemas/apps/ggz-gnome/profiles/passwords", GCONF_VALUE_STRING, slProfilePWDs, NULL);
	gconf_client_set_int (config, "/schemas/apps/ggz-gnome/profiles/total", intProfiles, NULL);


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
on_btnApply_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_btnDelete_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{

}

void
on_btnDone_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy (GTK_WIDGET (user_data));
}



void
on_profiles_changed 		(GtkTreeSelection *selection, 
					 gpointer data)
{
	GtkWidget *tmp;
	GtkTreeIter iter;
	GtkTreeModel *model;
	gchar *strTmp, *strProfile;
	GSList *slProfileNames, *slProfileUsernames, *slProfileServers, *slProfilePWDs;
	gint intProfiles, count;

	GtkWidget *winProfile = GTK_WIDGET(data);

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
				tmp = lookup_widget (winProfile, "entProfileName");
				gtk_entry_set_text (GTK_ENTRY (tmp), g_slist_nth_data (slProfileNames, count));
				tmp = lookup_widget (winProfile, "entUsername");
				gtk_entry_set_text (GTK_ENTRY (tmp), g_slist_nth_data (slProfileUsernames, count));
				tmp = lookup_widget (winProfile, "entPassword");
				gtk_entry_set_text (GTK_ENTRY (tmp), g_slist_nth_data (slProfilePWDs, count));
				tmp = lookup_widget (winProfile, "entServer");
				gtk_entry_set_text (GTK_ENTRY (tmp), g_slist_nth_data (slProfileServers, count));
				
			}
			g_free(strTmp);
		}
		
		g_free (strProfile);
	}
}
