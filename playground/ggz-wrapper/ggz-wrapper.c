#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ggz.h>
#include <ggzcore.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <glib.h>
#include <fcntl.h>
#include "server.h"
#include "loop.h"
#include "game.h"

void usage(char *base) {
	printf("%s [-g gametype] [-u your_nickname] [-p password] [-s server] [-P port(of the server)] [-d nick_of_the_other_player]\n", base);
	exit(0);
}

int main(int argc, char **argv, char **envp) {
	/*if(argc!=3 && argc !=4)
		usage(argv[0]);
	nick=argv[1];
	if(strcmp(argv[2], "host")==0)
		dst_nick=NULL;
	else 
		dst_nick=argv[2];
	if(argc==4)
		password=argv[4];
	else
		password=NULL;
	*/
	char *server="127.0.0.1";
	password=NULL;
	nick=g_strdup_printf("guest%d", rand()%999);
	game_name="TicTacToe";
	dst_nick=NULL;
	extern char *optarg;
	static char optstring[]="g:u:p:s:d:P:h";
	int port;
	port=5688;
	int optch;
	while((optch=getopt(argc, argv, optstring))!=-1) {
		switch(optch) {
			case 'g':
				game_name=optarg;
				break;
			case 'u':
				nick=optarg;
				break;
			case 'p':
				password=optarg;
				break;
			case 's':
				server=optarg;
				break;
			case 'd':
				dst_nick=optarg;
				break;
			case 'P':
				port=atoi(optarg);
				break;
			case 'h':
				usage(argv[0]);
				break;
			default:
				printf("Option %c inconnue\n", optch);
				break;
		}
	}
				


	if(password)
		server_init(server, port, GGZ_LOGIN, nick, password);
	else
		server_init(server, port, GGZ_LOGIN_GUEST, nick, password);
	loop();
	return 0;
}
