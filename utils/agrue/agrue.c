#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include <easysock.h>
#include <protocols.h>

#define PORT 5687
#define HOSTNAME "localhost"

char *opstring[] = {
	"MSG_SERVER_ID",	"MSG_SERVER_FULL",	"MSG_MOTD",
	"MSG_CHAT",		"MSG_UPDATE_PLAYERS",	"MSG_UPDATE_TYPES",
	"MSG_UPDATE_TABLES",	"MSG_UPDATE_ROOMS",	"MSG_ERROR",
	"RSP_LOGIN_NEW",	"RSP_LOGIN",		"RSP_LOGIN_ANON",
	"RSP_LOGOUT",		"RSP_PREF_CHANGE",	"RSP_REMOVE_USER",
	"RSP_LIST_PLAYERS",	"RSP_LIST_TYPES",	"RSP_LIST_TABLES",
	"RSP_LIST_ROOMS",	"RSP_TABLE_OPTIONS",	"RSP_USER_STAT",
	"RSP_TABLE_LAUNCH",	"RSP_TABLE_JOIN",	"RSP_TABLE_LEAVE",
	"RSP_GAME",		"RSP_CHAT",		"RSP_MOTD",
	"RSP_ROOM_JOIN"
};

void handle_read(void);
void req_login(void);
void change_room(void);
void yack_away(void);

int my_socket, rooms=0, cur_room=-1, logged_in=0;

int main()
{
	int opcode;
	fd_set select_mux;
	struct timeval timeout;

	srandom((unsigned)time(NULL));
	my_socket = es_make_socket(ES_CLIENT, PORT, HOSTNAME);

	while(1)
	{
		FD_ZERO(&select_mux);
		FD_SET(my_socket, &select_mux);
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		if(select(my_socket+1, &select_mux, NULL, NULL, &timeout)) {
			if(FD_ISSET(my_socket, &select_mux))
				handle_read();
		} else {
			if(!logged_in)
				req_login();
			else if(rooms != 0 && cur_room == -1)
				change_room();
			else if(rooms != 0 && (random()%15) == 0)
				change_room();
			else if(cur_room != -1 && (random()%3) == 0)
				yack_away();
		}
	}
}


void handle_read(void)
{
	int opcode, i, num, num2, j, num3;
	char *string;
	char chr;

	es_read_int(my_socket, &opcode);
	switch(opcode) {
		case MSG_SERVER_ID:
			es_read_string_alloc(my_socket, &string);
			free(string);
			break;
		case RSP_LOGIN_ANON:
			es_read_char(my_socket, &chr);
			es_read_int(my_socket, &num);
			if(chr != 0)
				logged_in = 0;
			else
				logged_in = 1;
			break;
		case MSG_MOTD:
			es_read_int(my_socket, &num);
			for(i=0; i<num; i++) {
				es_read_string_alloc(my_socket, &string);
				free(string);
			}
			break;
		case MSG_UPDATE_ROOMS:
			es_write_int(my_socket, REQ_LIST_ROOMS);
			es_write_int(my_socket, -1);
			es_write_char(my_socket, (char)0);
			break;
		case RSP_LIST_ROOMS:
			es_read_int(my_socket, &num);
			rooms = num;
			for(i=0; i<num; i++) {
				es_read_int(my_socket, &num2);
				es_read_string_alloc(my_socket, &string);
				free(string);
				es_read_int(my_socket, &num2);
			}
			break;
		case RSP_ROOM_JOIN:
			es_read_char(my_socket, &chr);
			break;
		case MSG_CHAT:
			es_read_string_alloc(my_socket, &string);
			free(string);
			es_read_string_alloc(my_socket, &string);
			free(string);
			break;
		case RSP_CHAT:
			es_read_char(my_socket, &chr);
			break;
		case MSG_UPDATE_PLAYERS:
			es_write_int(my_socket, REQ_LIST_PLAYERS);
			break;
		case MSG_UPDATE_TYPES:
			es_write_int(my_socket, REQ_LIST_TYPES);
			es_write_char(my_socket, 1);
			break;
		case MSG_UPDATE_TABLES:
			es_write_int(my_socket, REQ_LIST_TABLES);
			es_write_int(my_socket, -1);
			es_write_char(my_socket, 0);
			break;
		case RSP_LIST_PLAYERS:
			es_read_int(my_socket, &num);
			for(i=0; i<num; i++) {
				es_read_string_alloc(my_socket, &string);
				free(string);
				es_read_int(my_socket, &num2);
			}
			break;
		case RSP_LIST_TYPES:
			es_read_int(my_socket, &num);
			for(i=0; i<num; i++) {
				es_read_int(my_socket, &num2);
				es_read_string_alloc(my_socket, &string);
				free(string);
				es_read_string_alloc(my_socket, &string);
				free(string);
				es_read_char(my_socket, &chr);
				es_read_string_alloc(my_socket, &string);
				free(string);
				es_read_string_alloc(my_socket, &string);
				free(string);
				es_read_string_alloc(my_socket, &string);
				free(string);
			}
			break;
		case RSP_LIST_TABLES:
			es_read_int(my_socket, &num);
			for(i=0; i<num; i++) {
				es_read_int(my_socket, &num2);
				es_read_int(my_socket, &num2);
				es_read_int(my_socket, &num2);
				es_read_string_alloc(my_socket, &string);
				free(string);
				es_read_char(my_socket, &chr);
				es_read_int(my_socket, &num2);
				for(j=0; j<num2; j++) {
					es_read_int(my_socket, &num3);
					es_read_string_alloc(my_socket,&string);
					free(string);
				}
			}
			break;
	}
}


void req_login(void)
{
	char my_name[9];
	int i;

	es_write_int(my_socket, REQ_LOGIN_ANON);
	for(i=0; i<8; i++) {
		my_name[i] = (random() % 26) + 'a';
		if(random() % 12 == 0) {
			i++;
			break;
		}
	}
	my_name[i] = '\0';

	es_write_string(my_socket, my_name);
}


/* This function will really give your CPU a tummy ache if there's only 1 room*/
void change_room(void)
{
	int new_room=cur_room;
	while(new_room == cur_room)
		new_room = random() % rooms;
	es_write_int(my_socket, REQ_ROOM_JOIN);
	es_write_int(my_socket, new_room);
	cur_room = new_room;
}


void yack_away(void)
{
	char msg[128];
	int i;

	for(i=0; i<128; i++) {
		if(random() % 8)
			msg[i] = (random() % 26) + 'a';
		else
			msg[i] = ' ';
		if(random() % 25 == 0) {
			i++;
			break;
		}
	}
	msg[i] = '\0';

	es_write_int(my_socket, REQ_CHAT);
	es_write_string(my_socket, msg);
}
