
#include <config.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>
#include <easysock.h>

#include "game.h"
#include "dlg_error.h"
#include "options.h"
#include "client.h"
#include "protocols.h"
#include "err_func.h"


extern Options opt;

static guint game_handle;

static void run_game(int type, char flag, int fd);
static void handle_game(gpointer data, gint source, GdkInputCondition cond);
static void handle_options(gpointer data, gint source, GdkInputCondition cond);

void launch_game(int type, char launch)
{
	pid_t pid;
	int fd[2];
	GdkInputFunction callback;

	/* Create socketpair for communication */
	socketpair(PF_UNIX, SOCK_STREAM, 0, fd);
	
	if (launch) 
		dbg_msg("Launching game");
	else
		dbg_msg("Joining game");
	
	/* Fork table process */
	if ( (pid = fork()) <0) {
		DisplayError("fork failed");
		return;
	}
	else if (pid == 0) {
		run_game(type, launch, fd[0]);
		DisplayError("exec failed");
		return;
	}
	else {
		/* Close the remote ends of the socket pairs*/
		close(fd[0]);
		opt.game_pid = pid;
		opt.game_fd = fd[1];
		if (launch)
			callback = handle_options;
		else 
			callback = handle_game;
		
		game_handle = gdk_input_add(fd[1], GDK_INPUT_READ, *callback, 
					    NULL);
	}
}


static void run_game(int type, char flag, int fd) 
{
	dbg_msg("Process forked.  Game running");

	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	close(fd);

	/* FIXME: Don't hardcode client game */
	/* FIXME: Maybe pass through pipe, rather than cmd-line? */
	if (flag)
		execl("../game_clients/spades/gspades", "gspades", "-o", NULL);
	else 
		execl("../game_clients/spades/gspades", "gspades", NULL);
}


static void handle_options(gpointer data, gint source, GdkInputCondition cond) 
{
	int size;
	char ai;
	void* options;

	dbg_msg("Getting options from game client");

	CheckReadInt(source, &size);
	options = malloc(size);
	/*FIXME: check for failed malloc */
	es_readn(source, options, size);
	es_read_char(source, &ai);
	
	/* Send launch game request to server */
	CheckWriteInt(opt.sock, REQ_LAUNCH_GAME);
	CheckWriteInt(opt.sock, 0); /* Game type index */
	CheckWriteInt(opt.sock, 4); /* Number of seats */
	es_write_char(opt.sock, ai);    /* AI players */
	CheckWriteInt(opt.sock, 0); /* Number of reservations */
	CheckWriteInt(opt.sock, size);
	es_writen(opt.sock, options, size);
	free(options);

	gdk_input_remove(game_handle);
	game_handle = gdk_input_add(source, GDK_INPUT_READ, handle_game, 
				    NULL);
}


static void handle_game(gpointer data, gint source, GdkInputCondition cond) 
{
	int size, status;
	char buf[4096];

	dbg_msg("Got game msg from game client");

	size = read(source, buf, 4096);
	dbg_msg("Client sent %d bytes", size);
	CheckWriteInt(opt.sock, REQ_GAME);
	CheckWriteInt(opt.sock, size);
	status = es_writen(opt.sock, buf, size);
	
	if (status <= 0) { /* Game over */
		dbg_msg("Game is over (msg from client)");
		opt.playing = 0;
		close(source);
	}
}

