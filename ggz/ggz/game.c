
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

static void run_game(int type, int fd, char* path);
static void handle_game_fd(gpointer data, gint source, GdkInputCondition cond);

void launch_game(int type)
{
	pid_t pid;
	int fd[2];

	/* Create socketpair for communication */
	socketpair(PF_UNIX, SOCK_STREAM, 0, fd);
	
	dbg_msg("Launching game");
	/* Fork table process */
	if ( (pid = fork()) <0) {
		DisplayError("fork failed");
		return;
	}
	/* FIXME: Don't hardcode client game */
	else if (pid == 0) {
		run_game(type, fd[0], "../game_clients/spades/gspades");
		DisplayError("exec failed");
		return;
	}
	else {
		/* Close the remote ends of the socket pairs*/
		close(fd[0]);
		opt.game_pid = pid;
		opt.game_fd = fd[1];
		
		game_handle = gdk_input_add_full(fd[1], GDK_INPUT_READ, 
						 handle_game_fd, NULL, NULL);

	}
}


static void run_game(int type, int fd, char* path) 
{
	dbg_msg("Process forked.  Game running");

	/* FIXME: Close all other fd's and kill threads? */
	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	close(fd);
	
	execl(path, "gspades", NULL);
}


static void handle_game_fd(gpointer data, gint source, GdkInputCondition cond) 
{
	int size, status;
	char ai;
	char buf[4096];
	void* options;

	if (!opt.playing) {		
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
	} else {
		size = read(source, buf, 4096);
		dbg_msg("Client sent %d bytes", size);
		status = es_writen(opt.sock, buf, size);
		
		if (status <= 0) { /* Game over */
			dbg_msg("Game is over (msg from client)");
			opt.playing = 0;
			close(source);
		}
	}
}

