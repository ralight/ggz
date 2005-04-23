#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <ggzcore.h>
#include <ggz.h>

#include "login.h"
#include "ggzclient.h"

GGZServer *server = NULL;

void login_connect_failed(void)
{
	/* Destroy server object */
	ggzcore_server_free(server);
	server = NULL;
}


void login_failed(void)
{
	/* First, disconnect from the server. */
	ggzcore_server_disconnect(server);
	server = NULL;
}


void login_start_session(gchar *host, gint port,
			 gchar *username, gchar *password,
			 GGZLoginType type)
{
	/* Create new server object and set connection/login info */
	if(server != NULL)
		ggzcore_server_free(server);
	server = ggzcore_server_new();
	ggzcore_server_set_hostinfo(server, host, port, FALSE);
	ggzcore_server_set_logininfo(server, type, username, password, NULL);

	/* Setup callbacks on server */
	ggz_event_init(server);

	/* Start connection */
	ggzcore_server_connect(server);
}

void login_disconnect(void)
{
	if (ggzcore_server_logout(server) < 0)
		ggz_error_msg("Error logging out in "
		              "client_disconnect_activate");
}
