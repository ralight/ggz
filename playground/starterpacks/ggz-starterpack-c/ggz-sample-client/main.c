/*
 * GGZ Starterpack for C - Sample Client
 * Copyright (C) 2008 GGZ Development Team
 *
 * This code is made available as public domain; you can use it as a base
 * for your own game, as long as its licence is compatible with the libraries
 * you use.
 */

#include <gtk/gtk.h>
#include <ggz.h>
#include <ggz_common.h>
#include <ggzmod.h>
#include <stdlib.h>

#include "game.h"
#include "main_win.h"
#include "modversion.h"
#include "net.h"

/* main window widget */
extern GtkWidget *main_win;

/* Global game variables */
struct game_state_t game;

int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);

	main_win = create_main_win();
	gtk_widget_show(main_win);

	game_init();
	game_status("Nothing received yet.");

	gtk_main();

	if (ggzmod_disconnect(game.ggzmod) < 0)
		return -1;

	ggzmod_free(game.ggzmod);
	return 0;
}

static void game_handle_message(int opcode)
{
	switch (opcode) {

	case hello:
		game_status("Received HELLO from server!");
		break;
	}
}

static void game_handle_error()
{
	/* Do something here! */
}

static gboolean game_handle_io(GGZMod * mod)
{
	ggzcomm_set_notifier_callback(game_handle_message);
	ggzcomm_set_error_callback(game_handle_error);
	ggzcomm_network_main(ggzmod_get_server_fd(mod));

	return TRUE;
}

static gboolean handle_game_server(GIOChannel * channel, GIOCondition cond,
				   gpointer data)
{
	GGZMod *mod = data;

	return game_handle_io(mod);
}

static void handle_ggzmod_server(GGZMod * mod, GGZModEvent e,
				 const void *data)
{
	const int *fd = data;
	GIOChannel *channel;

	ggzmod_set_state(mod, GGZMOD_STATE_PLAYING);
	channel = g_io_channel_unix_new(*fd);
	g_io_add_watch(channel, G_IO_IN, handle_game_server, mod);
}

static gboolean handle_ggz(GIOChannel * channel, GIOCondition cond,
			   gpointer data)
{
	GGZMod *mod = data;

	return (ggzmod_dispatch(mod) >= 0);
}

void game_init(void)
{
	GIOChannel *channel;

	if (!ggzmod_is_ggz_mode()) {
		printf("This program should only be run from within GGZ.\n");
		exit(1);
	}

	/* Connect to GGZ; usually there are a lot more handlers */
	game.ggzmod = ggzmod_new(GGZMOD_GAME);
	ggzmod_set_handler(game.ggzmod, GGZMOD_EVENT_SERVER, handle_ggzmod_server);
	ggzmod_connect(game.ggzmod);

	channel = g_io_channel_unix_new(ggzmod_get_fd(game.ggzmod));
	g_io_add_watch(channel, G_IO_IN, handle_ggz, game.ggzmod);
}

