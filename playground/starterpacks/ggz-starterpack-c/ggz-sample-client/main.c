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

void game_init(void)
{
	game.ggzmod = init_ggz_gtk(GTK_WINDOW(main_win), game_handle_io);
}

