/*
 * Initial main.c file generated by Glade. Edit as required.
 * Glade will not overwrite this file.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>

#include <easysock.h>

#include "support.h"
#include "main.h"
#include "dlg_main.h"
#include "table.h"
#include "game.h"
#include "hand.h"

GtkWidget *dlg_main = NULL;

/* Private functions */
static void ggz_connect(void);
static void game_handle_io(gpointer data, gint source, GdkInputCondition cond);
static void game_init(void);
static int get_seat(void);
static int get_players(void);
static int get_play_status(void);
static int get_gameover_status(void);
static int get_sync_info(void);


int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);

	ggz_connect();
	gdk_input_add(game.fd, GDK_INPUT_READ, game_handle_io, NULL);

	dlg_main = create_dlg_main();

	gtk_widget_show(dlg_main);

	table_initialize();
	game_init();

	gtk_main();
	return 0;
}


static void ggz_connect(void)
{
	char fd_name[30];
	struct sockaddr_un addr;

	/* Connect to Unix domain socket */
	sprintf(fd_name, "/tmp/LaPocha.%d", getpid());

	if((game.fd = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0)
		exit(-1);

	bzero(&addr, sizeof(addr));
	addr.sun_family = AF_LOCAL;
	strcpy(addr.sun_path, fd_name);

	if(connect(game.fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		exit(-1);
}


#ifdef DEBUG
char *opstr[] = { "LP_MSG_SEAT",    "LP_MSG_PLAYERS",    "LP_MSG_GAMEOVER",
		  "LP_MSG_HAND",    "LP_REQ_BID",        "LP_RSP_BID",
                  "LP_MSG_BID",     "LP_REQ_PLAY",       "LP_RSP_PLAY",
                  "LP_MSG_PLAY",    "LP_SND_SYNC" };
#endif

static void game_handle_io(gpointer data, gint source, GdkInputCondition cond)
{
	int op, status;

	if(es_read_int(game.fd, &op) < 0) {
		/* FIXME: do something here... */
		return;
	}

	status = 0;
#ifdef DEBUG
	fprintf(stderr, "%s\n", opstr[op]);
#endif
	switch(op) {
		case LP_MSG_SEAT:
			status = get_seat();
			break;
		case LP_MSG_PLAYERS:
			status = get_players();
			if(game.state != LP_STATE_CHOOSE)
				game.state = LP_STATE_WAIT;
			break;
		case LP_MSG_GAMEOVER:
			status = get_gameover_status();
			break;
		case LP_MSG_HAND:
			/* We ignore this unless we've seen an opponents move */
			status = hand_read_hand();
			break;
		case LP_REQ_PLAY:
			game.state = LP_STATE_PLAY;
			status = 0;
			break;
		default:
			fprintf(stderr, "Unknown opcode received %d\n", op);
			status = -1;
			break;
	}

	if(status < 0) {
		fprintf(stderr, "Ouch!\n");
		close(game.fd);
		exit(-1);
	}
}


static void game_init(void)
{
	statusbar_message("Waiting for server");
	game.state = LP_STATE_INIT;
}


static int get_seat(void)
{
	if(es_read_int(game.fd, &game.me) < 0)
		return -1;
	return 0;
}


static int get_players(void)
{
	int i;
	char *temp;

	for(i=0; i<4; i++) {
		if(es_read_int(game.fd, &game.seats[i]) < 0)
			return -1;
		if(game.seats[i] != GGZ_SEAT_OPEN) {
			if(es_read_string(game.fd, (char*)&game.names[i], 17)<0)
				return -1;
			table_set_name(i, game.names[i]);
			if(i != game.me) {
				temp = g_strdup_printf("%s joined the table",
							game.names[i]);
				statusbar_message(temp);
				g_free(temp);
			}
		} else {
			table_set_name(i, "Empty Seat");
			if(game.got_players) {
				temp = g_strdup_printf("%s left the table",
							game.names[i]);
				statusbar_message(temp);
				g_free(temp);
			}
		}
	}

	game.got_players++;

	return 0;
}


static int get_play_status(void)
{
	char status;

	if(es_read_char(game.fd, &status) < 0)
		return -1;

	return (int)status;
}


static int get_sync_info(void)
{
	return 0;
}


static int get_gameover_status(void)
{
	return 0;
}


static void handle_req_newgame(void)
{
	/* Reinitialize the game data and board */
	game_init();
	game.got_players = 1;

	/* Send a game request to the server */
	es_write_int(game.fd, LP_REQ_NEWGAME);
}


void statusbar_message(char *msg)
{
	static GtkWidget *sb=NULL;
	static guint sb_context;

	if(sb == NULL) {
		sb = gtk_object_get_data(GTK_OBJECT(dlg_main), "statusbar1");
		sb_context = gtk_statusbar_get_context_id(GTK_STATUSBAR(sb),
							  "Game Messages");
	}

	gtk_statusbar_push(GTK_STATUSBAR(sb), sb_context, msg);
}
