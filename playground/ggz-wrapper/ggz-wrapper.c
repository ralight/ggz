#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "server.h"
#include "loop.h"
#include "game.h"

#define DEFAULT_SERVER   "127.0.0.1"
#define DEFAULT_PORT     5688
#define DEFAULT_GAMENAME "TicTacToe"
#define DEFAULT_NICKNAME NULL
#define DEFAULT_PASSWORD NULL
#define DEFAULT_DESTNICK NULL

#define _(x) x

char *game_name;
char *dst_nick;

static void usage(void) {
	printf(_("ggz-wrapper: Dummy core client for the GGZ Gaming Zone\n"));
	printf(_("\n"));
	printf(_("Options:\n"));
	printf(_("[-g <gametype>] Type of game to play\n"));
	printf(_("[-u <user>    ] Your nickname\n"));
	printf(_("[-p <password>] Password (empty for guest logins)\n"));
	printf(_("[-s <server>  ] Server hostname\n"));
	printf(_("[-P <port>    ] Server port\n"));
	printf(_("[-d <destnick>] Nickname of opponent\n"));
	exit(0);
}

int main(int argc, char **argv) {
	struct option options[] =
	{
		{"help", no_argument, 0, 'h'},
		{"gametype", required_argument, 0, 'g'},
		{"user", required_argument, 0, 'u'},
		{"password", required_argument, 0, 'p'},
		{"server", required_argument, 0, 's'},
		{"port", required_argument, 0, 'P'},
		{"destnick", required_argument, 0, 'd'},
		{0, 0, 0, 0}
	};
	char optstring[] = "g:u:p:s:d:P:h";
	char randomuser[64];

	int optch;
	int optindex;

	char *server = DEFAULT_SERVER;
	char *password = DEFAULT_PASSWORD;
	char *nick = DEFAULT_NICKNAME;
	int port = DEFAULT_PORT;

	game_name = DEFAULT_GAMENAME;
	dst_nick = DEFAULT_DESTNICK;

	while((optch = getopt_long(argc, argv, optstring, options, &optindex)) != -1) {
		switch(optch) {
			case 'g':
				game_name = optarg;
				break;
			case 'u':
				nick = optarg;
				break;
			case 'p':
				password = optarg;
				break;
			case 's':
				server = optarg;
				break;
			case 'd':
				dst_nick = optarg;
				break;
			case 'P':
				port = atoi(optarg);
				break;
			case 'h':
				usage();
				break;
			default:
				printf(_("Option %c unknown, try --help.\n"), optch);
				usage();
				break;
		}
	}

	if(!nick) {
		snprintf(randomuser, sizeof(randomuser), _("gaimguest%i"), rand() % 10000);
		nick = strdup(randomuser);
	}

	if(password)
		server_init(server, port, GGZ_LOGIN, nick, password);
	else
		server_init(server, port, GGZ_LOGIN_GUEST, nick, password);
	loop();
	return 0;
}
