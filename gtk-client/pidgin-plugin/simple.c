#include "config.h"

#include <gdk/gdk.h>
#include <gtk/gtkplug.h>

#define PURPLE_PLUGINS 1

#include <libpurple/version.h>

#include <libpurple/debug.h>
#include <libpurple/core.h>
#include <libpurple/prefs.h>
#include <libpurple/blist.h>
#include <libpurple/signals.h>
#include <libpurple/util.h>
#include <libpurple/cmds.h>
#include <libpurple/debug.h>
#include <libpurple/plugin.h>
#include <libpurple/conversation.h>

#include <pidgin/gtkutils.h>
#include <pidgin/gtkplugin.h>
#include <pidgin/gtkconv.h>
#include <pidgin/gtkblist.h>

#include <ggz.h>

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "ggz.xpm"

#define HEADER "*** Command from the purple-ggz plugin:"
#define FOOTER "***"
#define GGZWRAPPER "ggz-wrapper"
#define GGZMODULECONFIG PREFIX "/etc/ggz.modules"
#define SERVER "live.ggzgamingzone.org"

GtkWidget *pTable, *pLogin, *pPassword, *pCheckBox, *pServer;
int LastGamePID;

PurpleCmdRet commande(PurpleConversation *conv, const gchar *cmd, gchar **args, gchar **error, void *data) {
	int argc,pid;
	char *joueur;
	char *jeu;
	joueur=g_strdup_printf("host%d", (int) (999.0*rand()/(RAND_MAX+1.0)));
	for(argc=0;args[argc];argc++);
	if(argc==0) {
		if(error)
			*error=g_strdup("Il faut spécifier un jeu, les jeux possibles sont(respectez la casse):\n-TicTacToe\n-Reversi\n-Chess");
		return PURPLE_CMD_RET_FAILED;
	}
	jeu=args[0];
	pid=fork();
	if(pid==0) {
		char **parms=malloc(4*sizeof(char *));
		char *sys_parm;
		if(purple_prefs_get_bool("/plugins/gtk/ggzpurple/guest"))
		 {
			parms[0]=joueur;
			parms[1]=jeu;
			sys_parm=g_strdup_printf("%s -u %s -g %s -s %s", GGZWRAPPER, parms[0], parms[1], purple_prefs_get_string("/plugins/gtk/ggzpurple/server"));
		 }
		else
		 {
			parms[0]=(char*)purple_prefs_get_string("/plugins/gtk/ggzpurple/login");
			parms[1]=jeu;
			sys_parm=g_strdup_printf("%s -u %s -g %s -p %s -s %s", GGZWRAPPER, parms[0], parms[1], purple_prefs_get_string("/plugins/gtk/ggzpurple/password"), purple_prefs_get_string("/plugins/gtk/ggzpurple/server"));
		 }
		parms[0]="sh";
		parms[1]="-c";
		parms[2]=sys_parm;
		parms[3]=NULL;
		printf("LAUNCH: %s,%s %s %s\n", "/bin/sh", parms[0], parms[1], parms[2]);
		execv("/bin/sh", parms);
		exit(0);
	} else if(pid>0) {
		if(purple_prefs_get_bool("/plugins/gtk/ggzpurple/guest")) purple_conv_im_send(PURPLE_CONV_IM(conv), g_strdup_printf("%s%s %s %s", HEADER, jeu,joueur, FOOTER));
		else purple_conv_im_send(PURPLE_CONV_IM(conv), g_strdup_printf("%s%s %s %s", HEADER, jeu,purple_prefs_get_string("/plugins/gtk/ggzpurple/login"), FOOTER));

		LastGamePID = pid;

		return PURPLE_CMD_RET_OK;
	} else {
		if(error)
		*error=strdup("Probleme de fork()");
		return PURPLE_CMD_RET_FAILED;
	}
}

gboolean icon_clicked(void **arg, GdkEvent *ev) {
	if(ev->type==GDK_BUTTON_PRESS) {
	PurpleConversation *conv=arg[0];
	char **args=malloc(2*sizeof(char*));
	args[0]=arg[1];
	args[1]=NULL;
	commande(conv, g_strdup("jeu"), args, NULL, NULL);
	return TRUE;
	}
	return FALSE;
}

gboolean PopMenu(GtkMenu *menu, GdkEvent *ev) {
	if(ev->type==GDK_BUTTON_PRESS) {
		GdkEventButton *bev=(GdkEventButton *)ev;
		gtk_menu_popup(menu, NULL, NULL, NULL, NULL, bev->button, bev->time);
		return TRUE;
	}
	return FALSE;
}

static void nouvelle_convers(PurpleConversation *conv, void *data) {
//	printf("Une nouvelle conversation:%s!!\n", purple_conversation_get_name(conv));
	PidginConversation *gtkconv = PIDGIN_CONVERSATION(conv);
	GtkWidget *bbox=gtkconv->lower_hbox;
	GtkWidget *icon;
	GtkWidget *menu;
	GtkWidget *menuitem;
	void **arg;
	GtkSizeGroup *sg;

	int i, ret, handle;
	int argcp;
	char **argvp;
	char *gamename;

	GdkPixbuf *pixbuf;
	GtkIconSet *iconset;
	GtkIconFactory *factory;

	factory = gtk_icon_factory_new();
	gtk_icon_factory_add_default(factory);

	pixbuf = gdk_pixbuf_new_from_xpm_data(ggz_xpm);
	iconset = gtk_icon_set_new_from_pixbuf(pixbuf);
	g_object_unref(G_OBJECT(pixbuf));
	gtk_icon_factory_add (factory, "ggzicon", iconset);

	sg = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);

#if PURPLE_MAJOR_VERSION < 2
	icon = purple_gtkconv_button_new("ggzicon", "Jouer", "Pfff", gtkconv->tooltips, GTK_SIGNAL_FUNC(PopMenu), NULL);
#else
	icon = NULL; /* FIXME! */
#endif
	if(icon == NULL) {
		printf("Arf :/ \n");
		return;
	} else {
		printf("OK :) \n");
	}

	/* Menu */
	menu = gtk_menu_new();

	handle = ggz_conf_parse(GGZMODULECONFIG, GGZ_CONF_RDONLY);
	ret = ggz_conf_read_list(handle, "Games", "*Engines*", &argcp, &argvp);
	for(i = 0; i < argcp; i++)
	{
		gamename = argvp[i];
		arg = malloc(sizeof(void*)*2);
		arg[0] = (gpointer)conv;
		arg[1] = strdup(gamename);
		menuitem = gtk_menu_item_new_with_label(gamename);
		gtk_signal_connect_object(GTK_OBJECT(menuitem), "event", GTK_SIGNAL_FUNC(icon_clicked), (gpointer) arg);
		gtk_menu_append(GTK_MENU(menu), menuitem);
		gtk_widget_show(menuitem);
	}
	ggz_conf_close(handle);

	//Menu général
	gtk_signal_connect_object(GTK_OBJECT(icon), "event", GTK_SIGNAL_FUNC(PopMenu), GTK_OBJECT(menu));

	//Fin menu
	gtk_box_pack_start(GTK_BOX(bbox), icon, FALSE, FALSE, 0);
	gtk_button_set_relief(GTK_BUTTON(icon), GTK_RELIEF_NONE);
	gtk_tooltips_set_tip(gtkconv->tooltips, icon, g_strdup("Clickez sur ce bouton pour jouer avec votre correspondant"), 0);
	g_hash_table_insert(conv->data, g_strdup("icon"), icon);
	gtk_size_group_add_widget(sg, icon);
	gtk_box_reorder_child(GTK_BOX(bbox), icon, 7);

	gtk_widget_show(icon);
}


static void message_recu2(PurpleAccount *acct,char **sender, char **buffer,int flags,  PurpleConversation *conv,void *data) {

	GtkWidget *pDialog;

	if(*buffer==NULL)
		return;

	if(strstr(*buffer, HEADER)==NULL)
		return;

	char *joueur=NULL;
	char *jeu=NULL;
	jeu=strstr(*buffer, HEADER);
	jeu+=strlen(HEADER);
	jeu[strlen(jeu)-strlen(strstr(jeu, FOOTER))]='\0';
	joueur=index(jeu, ' ');

	if(strstr(jeu,"refuse_game") != NULL)
	{
    		pDialog = gtk_message_dialog_new (NULL,
        		GTK_DIALOG_MODAL,
        		GTK_MESSAGE_QUESTION,
        		GTK_BUTTONS_OK,
        		g_strdup_printf("Votre partie avec %s a été refusée/annulée !",*sender));
		gtk_dialog_run(GTK_DIALOG(pDialog));
		gtk_widget_destroy(pDialog);

		//printf("Test : %d\n",LastGamePID);
		//kill(LastGamePID, SIGKILL);

		*buffer = NULL;

		return;
	}

	if(joueur==NULL) {
		printf("Pas de joueur mais un jeu: %s\n", jeu);
		return;
	}
	joueur[0]='\0';
	joueur++;
	if(index(joueur, ' ')) {
		joueur[strlen(joueur) - strlen(index(joueur, ' '))]='\0';
	}

    	pDialog = gtk_message_dialog_new (NULL,
        	GTK_DIALOG_MODAL,
        	GTK_MESSAGE_QUESTION,
        	GTK_BUTTONS_YES_NO,
        	"%s/%s vous propose une partie de %s, voulez vous accepter ?",*sender,joueur,jeu);

	if(gtk_dialog_run(GTK_DIALOG(pDialog)) == GTK_RESPONSE_NO)
	{
		gtk_widget_destroy(pDialog);

		purple_conv_im_send(PURPLE_CONV_IM(purple_conversation_new(PURPLE_CONV_TYPE_IM,acct,*sender)), g_strdup_printf("%srefuse_game %s", HEADER, FOOTER));
		*buffer=NULL;
		return;
	}

	gtk_widget_destroy(pDialog);

	int pid=0;
	pid=fork();
	if(pid==0) {
		//L'enfant trop fort hein :)
		char **parms=malloc(4*sizeof(char *));
		char *sys_parm;
		if(purple_prefs_get_bool("/plugins/gtk/ggzpurple/guest"))
		 {
			parms[1]=g_strdup_printf("guest%d", (int) (999.0*rand()/(RAND_MAX+1.0)));
			parms[2]=joueur;
			sys_parm=g_strdup_printf("%s -u %s -d %s -g %s -s %s", GGZWRAPPER, parms[1], parms[2], jeu, purple_prefs_get_string("/plugins/gtk/ggzpurple/server"));
		 }
		else
		 {
			parms[1]=(char*)purple_prefs_get_string("/plugins/gtk/ggzpurple/login");
			parms[2]=joueur;
			sys_parm=g_strdup_printf("%s -u %s -d %s -g %s -p %s -s %s", GGZWRAPPER, parms[1], parms[2], jeu, purple_prefs_get_string("/plugins/gtk/ggzpurple/password"), purple_prefs_get_string("/plugins/gtk/ggzpurple/server"));
		 }
		parms[0]="sh";
		parms[1]="-c";
		parms[2]=sys_parm;
		parms[3]=NULL;
		printf("LAUNCH: %s,%s %s %s\n", "/bin/sh", parms[0], parms[1], parms[2]);
		execv("/bin/sh", parms);
		exit(0);
	} else if(pid>0) {
		//Ici c'est le pere je vous apprends beaucoup de choses non? ;)
		//LastGamePID = pid;
	} else {
		//Ici c'est quand y a eu une erreur........
	}
	//Ca sert a rien d'afficher ce somtueux message ;)
	*buffer=NULL;
}

static gboolean
plugin_load(PurplePlugin *plugin)
{
//	purple_debug(PURPLE_DEBUG_INFO, "simple", "simple plugin loaded.\n");

	void *conv_handle = purple_conversations_get_handle();
	purple_signal_connect(conv_handle, "conversation-created",
		plugin, PURPLE_CALLBACK(nouvelle_convers), NULL);
        /*purple_signal_connect(conv_handle, "receiving-chat-msg",
		plugin, PURPLE_CALLBACK(message_recu), NULL);*/
        purple_signal_connect(conv_handle, "receiving-im-msg",
		plugin, PURPLE_CALLBACK(message_recu2), NULL);
	purple_cmd_register("jeu", "w", PURPLE_CMD_P_PLUGIN, PURPLE_CMD_FLAG_IM|PURPLE_CMD_FLAG_ALLOW_WRONG_ARGS, NULL, PURPLE_CMD_FUNC(commande), "jeu nom_du_jeu",NULL);
	return TRUE;
}

static gboolean
plugin_unload(PurplePlugin *plugin)
{
//	purple_debug(PURPLE_DEBUG_INFO, "simple", "simple plugin unloaded.\n");

	return TRUE;
}

static void
ggzpurple_entry_change_cb(GtkObject *obj, gchar *pref)
{
	purple_prefs_set_string(pref,gtk_entry_get_text(GTK_ENTRY(obj)));
}

static void
ggzpurple_checkbox_change_cb(GtkObject *obj, gchar *pref)
{
	purple_prefs_set_bool(pref,gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(obj)));
}

GtkWidget *
ggzpurple_preferences_get_frame(PurplePlugin *plugin) {

	pTable = gtk_table_new(5,5,TRUE);

	pLogin = gtk_entry_new();
	pPassword = gtk_entry_new();
	pCheckBox = gtk_check_button_new();
	pServer = gtk_entry_new();

	gtk_entry_set_visibility(GTK_ENTRY(pPassword), FALSE);


	gtk_entry_set_text(GTK_ENTRY(pLogin),purple_prefs_get_string("/plugins/gtk/ggzpurple/login"));
	gtk_entry_set_text(GTK_ENTRY(pPassword),purple_prefs_get_string("/plugins/gtk/ggzpurple/password"));
	gtk_entry_set_text(GTK_ENTRY(pServer),purple_prefs_get_string("/plugins/gtk/ggzpurple/server"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pCheckBox), purple_prefs_get_bool("/plugins/gtk/ggzpurple/guest"));


	gtk_table_attach_defaults(GTK_TABLE(pTable),gtk_label_new("Identification en 'guest' :"),0,2,1,2);
	gtk_table_attach_defaults(GTK_TABLE(pTable),gtk_label_new("Pseudo :"),0,2,2,3);
	gtk_table_attach_defaults(GTK_TABLE(pTable),gtk_label_new("Pass :"),0,2,3,4);
	gtk_table_attach_defaults(GTK_TABLE(pTable),gtk_label_new("Serveur :"),0,2,4,5);

	gtk_table_attach_defaults(GTK_TABLE(pTable),pCheckBox,3,4,1,2);
	gtk_table_attach_defaults(GTK_TABLE(pTable),pLogin,3,5,2,3);
	gtk_table_attach_defaults(GTK_TABLE(pTable),pPassword,3,5,3,4);
	gtk_table_attach_defaults(GTK_TABLE(pTable),pServer,3,5,4,5);

	//purple_prefs_set_int(pref_url, int);

	//purple_prefs_add_string(const char *name, const char *value);
	//purple_prefs_add_bool(const char *name, gboolean value);
	//purple_prefs_add_int(const char *name, int value);


	//gboolean purple_prefs_get_bool(const char *name);
	//int purple_prefs_get_int(const char *name);

	//const char *purple_prefs_get_string(const char *name);


	g_signal_connect(GTK_OBJECT(pLogin), "changed", G_CALLBACK(ggzpurple_entry_change_cb), "/plugins/gtk/ggzpurple/login");

	g_signal_connect(GTK_OBJECT(pPassword), "changed", G_CALLBACK(ggzpurple_entry_change_cb), "/plugins/gtk/ggzpurple/password");

	g_signal_connect(GTK_OBJECT(pServer), "changed", G_CALLBACK(ggzpurple_entry_change_cb), "/plugins/gtk/ggzpurple/server");

	g_signal_connect(GTK_OBJECT(pCheckBox), "clicked", G_CALLBACK(ggzpurple_checkbox_change_cb), "/plugins/gtk/ggzpurple/guest");

	gtk_widget_show_all(pTable);
	return pTable;
}

void ggzpurple_preferences_add()
{
 purple_prefs_add_none("/plugins/gtk/ggzpurple");
 purple_prefs_add_string("/plugins/gtk/ggzpurple/login", "");
 purple_prefs_add_string("/plugins/gtk/ggzpurple/password", "");
 purple_prefs_add_string("/plugins/gtk/ggzpurple/server", SERVER);
 purple_prefs_add_bool("/plugins/gtk/ggzpurple/guest", TRUE);
}

static PidginPluginUiInfo ui_info = { ggzpurple_preferences_get_frame };

static PurplePluginInfo info =
{
	PURPLE_PLUGIN_MAGIC,
	PURPLE_MAJOR_VERSION,
	PURPLE_MINOR_VERSION,
	PURPLE_PLUGIN_STANDARD,                           /**< type           */
	PIDGIN_PLUGIN_TYPE,                               /**< ui_requirement */
	0,                                                /**< flags          */
	NULL,                                             /**< dependencies   */
	PURPLE_PRIORITY_DEFAULT,                          /**< priority       */

	NULL,                                             /**< id             */
	"GGZ for purple",                                 /**< name           */
	VERSION,                                          /**< version        */
	                                                  /**  summary        */
	"To have ggz in purple.",
	                                                  /**  description    */
	"Tests to see that most things are working.",
	"Husson Pierre-Hugues <phh@www.phh.sceen.net>",   /**< author         */
	"http://www.phh.sceen.net",                       /**< homepage       */

	plugin_load,                                      /**< load           */
	plugin_unload,                                    /**< unload         */
	NULL,                                             /**< destroy        */

	&ui_info,                                         /**< ui_info        */
	NULL                                              /**< extra_info     */
};

static void
init_plugin(PurplePlugin *plugin)
{
	ggzpurple_preferences_add();
}

PURPLE_INIT_PLUGIN(ggz4purple, init_plugin, info)
