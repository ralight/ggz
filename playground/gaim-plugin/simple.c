#define GAIM_PLUGINS 1
#include <gdk/gdk.h>
#include <gtk/gtkplug.h>

#include <gaim/config.h>
#include <gaim/debug.h>
#include <gaim/gaim.h>
#include <gaim/core.h>
#include <gaim/gtkutils.h>
#include <gaim/gtkplugin.h>
#include <gaim/gtkconv.h>
#include <gaim/prefs.h>
#include <gaim/blist.h>
#include <gaim/gtkblist.h>
#include <gaim/signals.h>
#include <gaim/util.h>
#include <gaim/internal.h>
#include <gaim/cmds.h>
#include <gaim/debug.h>
#include <gaim/plugin.h>
#include <gaim/version.h>

#include <ggz.h>

#include "ggz.xpm"

#define HEADER "*** Command from the gaim-ggz plugin:"
#define FOOTER "***"
#define GGZWRAPPER "ggz-wrapper"
#define GGZMODULECONFIG "/home/ggz/BUILD/etc/ggz.modules"
#define SERVER "live.ggzgamingzone.org"

GaimCmdRet commande(GaimConversation *conv, const gchar *cmd, gchar **args, gchar **error, void *data) {
	int argc,pid;
	char *joueur;
	char *jeu;
	joueur=g_strdup_printf("host%d", (int) (999.0*rand()/(RAND_MAX+1.0)));
	for(argc=0;args[argc];argc++);
	if(argc==0) {
		if(error)
			*error=g_strdup("Il faut spécifier un jeu, les jeux possibles sont(respectez la casse):\n-TicTacToe\n-Reversi\n-Chess");
		return GAIM_CMD_RET_FAILED;
	}
	jeu=args[0];
	pid=fork();
	if(pid==0) {
		char **parms=malloc(4*sizeof(char *));
		char *sys_parm;
		parms[0]=joueur;
		parms[1]=jeu;
		sys_parm=g_strdup_printf("%s -u %s -g %s -s %s", GGZWRAPPER, parms[0], parms[1], SERVER);
		parms[0]="sh";
		parms[1]="-c";
		parms[2]=sys_parm;
		parms[3]=NULL;
		printf("LAUNCH: %s,%s %s %s\n", "/bin/sh", parms[0], parms[1], parms[2]);
		execv("/bin/sh", parms);
		exit(0);
	} else if(pid>0) {
		gaim_conv_im_send(GAIM_CONV_IM(conv), g_strdup_printf("%s%s %s %s", HEADER, jeu,joueur, FOOTER));
		return GAIM_CMD_RET_OK;
	} else {
		if(error)
		*error=strdup("Probleme de fork()");
		return GAIM_CMD_RET_FAILED;
	}
}

gboolean icon_clicked(void **arg, GdkEvent *ev) {
	if(ev->type==GDK_BUTTON_PRESS) {
	GaimConversation *conv=arg[0];
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

static void nouvelle_convers(GaimConversation *conv, void *data) {
//	printf("Une nouvelle conversation:%s!!\n", gaim_conversation_get_name(conv));
	GaimGtkConversation *gtkconv=GAIM_GTK_CONVERSATION(conv);
	GtkWidget *bbox=gtkconv->bbox;
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

	icon = gaim_gtkconv_button_new("ggzicon", "Play Game", "Pfff", gtkconv->tooltips, GTK_SIGNAL_FUNC(PopMenu), NULL);
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

static void message_recu(GaimAccount *acct,char **sender, char **buffer,int flags,  void *data) {
	printf("Recu un message de %s:%s\n", *sender, *buffer);
}


static void message_recu2(GaimAccount *acct,char **sender, char **buffer,int flags,  GaimConversation *conv,void *data) {
//	printf("Recu un message de %s:%s\n", *sender, *buffer);

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
	if(joueur==NULL) {
		printf("Pas de joueur mais un jeu: %s\n", jeu);
		return;
	}
	joueur[0]='\0';
	joueur++;
	if(index(joueur, ' ')) {
		joueur[strlen(joueur) - strlen(index(joueur, ' '))]='\0';
	}
	int pid=0;
	pid=fork();
	if(pid==0) {
		//L'enfant trop fort hein :)
		char **parms=malloc(4*sizeof(char *));
		char *sys_parm;
		parms[1]=g_strdup_printf("guest%d", (int) (999.0*rand()/(RAND_MAX+1.0)));
		parms[2]=joueur;
		sys_parm=g_strdup_printf("%s -u %s -d %s -g %s -s %s", GGZWRAPPER, parms[1], parms[2], jeu, SERVER);
		parms[0]="sh";
		parms[1]="-c";
		parms[2]=sys_parm;
		parms[3]=NULL;
		printf("LAUNCH: %s,%s %s %s\n", "/bin/sh", parms[0], parms[1], parms[2]);
		execv("/bin/sh", parms);
		exit(0);
	} else if(pid>0) {
		//Ici c'est le pere je vous apprends beaucoup de choses non? ;)
	} else {
		//Ici c'est quand y a eu une erreur........
	}
	//Ca sert a rien d'afficher ce somtueux message ;)
	*buffer=NULL;
}

static gboolean
plugin_load(GaimPlugin *plugin)
{
//	gaim_debug(GAIM_DEBUG_INFO, "simple", "simple plugin loaded.\n");

	void *conv_handle = gaim_conversations_get_handle();
	gaim_signal_connect(conv_handle, "conversation-created",
		plugin, GAIM_CALLBACK(nouvelle_convers), NULL);
        gaim_signal_connect(conv_handle, "receiving-chat-msg",
		plugin, GAIM_CALLBACK(message_recu), NULL);
        gaim_signal_connect(conv_handle, "receiving-im-msg",
		plugin, GAIM_CALLBACK(message_recu2), NULL);
	gaim_cmd_register("jeu", "w", GAIM_CMD_P_PLUGIN, GAIM_CMD_FLAG_IM|GAIM_CMD_FLAG_ALLOW_WRONG_ARGS, NULL, GAIM_CMD_FUNC(commande), "jeu <Le nom du jeu>",NULL);
	return TRUE;
}

static gboolean
plugin_unload(GaimPlugin *plugin)
{
//	gaim_debug(GAIM_DEBUG_INFO, "simple", "simple plugin unloaded.\n");

	return TRUE;
}

static GaimPluginInfo info =
{
	//GAIM_PLUGIN_API_VERSION,                          /**< api_version    */
	GAIM_PLUGIN_MAGIC,
	GAIM_MAJOR_VERSION,
	GAIM_MINOR_VERSION,
	GAIM_PLUGIN_STANDARD,                             /**< type           */
	GAIM_GTK_PLUGIN_TYPE,                                             /**< ui_requirement */
	0,                                                /**< flags          */
	NULL,                                             /**< dependencies   */
	GAIM_PRIORITY_DEFAULT,                            /**< priority       */

	NULL,                                             /**< id             */
	"GGZ for gaim",                              /**< name           */
	VERSION,                                          /**< version        */
	                                                  /**  summary        */
	"To have ggz in gaim.",
	                                                  /**  description    */
	"Tests to see that most things are working.",
	"Husson Pierre-Hugues <phh@www.phh.sceen.net>",        /**< author         */
	"http://www.phh.sceen.net",                                          /**< homepage       */

	plugin_load,                                      /**< load           */
	plugin_unload,                                    /**< unload         */
	NULL,                                             /**< destroy        */

	NULL,                                             /**< ui_info        */
	NULL                                             /**< extra_info     */
//	NULL,
//	NULL
};

static void
init_plugin(GaimPlugin *plugin)
{
}

GAIM_INIT_PLUGIN(ggz4gaim, init_plugin, info)
