#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include <ggz.h>

#include "protocols.h"

#define PORT 5688
#define HOSTNAME "localhost"
#define PROTOCOL_VERSION 3

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
void req_room_list(void);
void change_room(void);
void yack_away(void);

int my_socket, rooms=0, cur_room=-1, logged_in=0, max_chatlen;

int main()
{
	fd_set select_mux;
	struct timeval timeout;

	srandom((unsigned)time(NULL));
	my_socket = ggz_make_socket(GGZ_SOCK_CLIENT, PORT, HOSTNAME);

	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	while(1)
	{
		FD_ZERO(&select_mux);
		FD_SET(my_socket, &select_mux);
		if(select(my_socket+1, &select_mux, NULL, NULL, &timeout)) {
			if(FD_ISSET(my_socket, &select_mux))
				handle_read();
		} else {
			if(!logged_in)
				req_login();
			else if(rooms != 0
			   && (cur_room == -1 || (random()%12) == 0))
				change_room();
			else if(cur_room != -1)
				yack_away();
		}

		if(timeout.tv_sec <= 0 && timeout.tv_usec <= 0) {
			timeout.tv_sec = 5;
			timeout.tv_usec = 0;
		}
	}
}


void handle_read(void)
{
	int opcode, i, num, num2, j, num3;
	char *string;
	char chr;

	ggz_read_int(my_socket, &opcode);
	/*printf("OP: %s\n", opstring[opcode]);*/
	switch(opcode) {
		case MSG_SERVER_ID:
			ggz_read_string_alloc(my_socket, &string);
			free(string);
			ggz_read_int(my_socket, &num);
			ggz_read_int(my_socket, &max_chatlen);
			if(num != PROTOCOL_VERSION) {
				printf("Server is protocol version %d\n", num);
				printf("We are version %d\n", PROTOCOL_VERSION);
				printf("Exiting...\n");
				exit(1);
			}
			break;
		case RSP_LOGIN_ANON:
			ggz_read_char(my_socket, &chr);
			ggz_read_int(my_socket, &num);
			if(chr != 0)
				logged_in = 0;
			else
				logged_in = 1;
			req_room_list();
			break;
		case MSG_MOTD:
			ggz_read_int(my_socket, &num);
			for(i=0; i<num; i++) {
				ggz_read_string_alloc(my_socket, &string);
				free(string);
			}
			break;
		case RSP_LIST_ROOMS:
			ggz_read_int(my_socket, &num);
			rooms = num;
			for(i=0; i<num; i++) {
				ggz_read_int(my_socket, &num2);
				ggz_read_string_alloc(my_socket, &string);
				free(string);
				ggz_read_int(my_socket, &num2);
			}
			break;
		case RSP_ROOM_JOIN:
			ggz_read_char(my_socket, &chr);
			break;
		case MSG_CHAT:
			ggz_read_char(my_socket, &chr);
			ggz_read_string_alloc(my_socket, &string);
			free(string);
			if(chr & GGZ_CHAT_M_MESSAGE) {
				ggz_read_string_alloc(my_socket, &string);
				free(string);
			}
			break;
		case RSP_CHAT:
			ggz_read_char(my_socket, &chr);
			break;
		case MSG_UPDATE_PLAYERS:
			ggz_read_char(my_socket, &chr);
			ggz_read_string_alloc(my_socket, &string);
			free(string);
			break;
		case MSG_UPDATE_TYPES:
			ggz_write_int(my_socket, REQ_LIST_TYPES);
			ggz_write_char(my_socket, 1);
			break;
		case MSG_UPDATE_TABLES:
			ggz_write_int(my_socket, REQ_LIST_TABLES);
			ggz_write_int(my_socket, -1);
			ggz_write_char(my_socket, 0);
			break;
		case RSP_LIST_PLAYERS:
			ggz_read_int(my_socket, &num);
			for(i=0; i<num; i++) {
				ggz_read_string_alloc(my_socket, &string);
				free(string);
				ggz_read_int(my_socket, &num2);
			}
			break;
		case RSP_LIST_TYPES:
			ggz_read_int(my_socket, &num);
			for(i=0; i<num; i++) {
				ggz_read_int(my_socket, &num2);
				ggz_read_string_alloc(my_socket, &string);
				free(string);
				ggz_read_string_alloc(my_socket, &string);
				free(string);
				ggz_read_char(my_socket, &chr);
				ggz_read_string_alloc(my_socket, &string);
				free(string);
				ggz_read_string_alloc(my_socket, &string);
				free(string);
				ggz_read_string_alloc(my_socket, &string);
				free(string);
			}
			break;
		case RSP_LIST_TABLES:
			ggz_read_int(my_socket, &num);
			for(i=0; i<num; i++) {
				ggz_read_int(my_socket, &num2);
				ggz_read_int(my_socket, &num2);
				ggz_read_int(my_socket, &num2);
				ggz_read_string_alloc(my_socket, &string);
				free(string);
				ggz_read_char(my_socket, &chr);
				ggz_read_int(my_socket, &num2);
				for(j=0; j<num2; j++) {
					ggz_read_int(my_socket, &num3);
					ggz_read_string_alloc(my_socket,&string);
					free(string);
				}
			}
			break;
		default:
			printf("Unimplemented opcode received\n");
			exit(1);
	}
}


void req_login(void)
{
	char my_name[9];
	int i;

	ggz_write_int(my_socket, REQ_LOGIN_ANON);
	for(i=0; i<8; i++) {
		my_name[i] = (random() % 26) + 'a';
		if(random() % 12 == 0) {
			i++;
			break;
		}
	}
	my_name[i] = '\0';

	ggz_write_string(my_socket, my_name);
}


void req_room_list(void)
{
	ggz_write_int(my_socket, REQ_LIST_ROOMS);
	ggz_write_int(my_socket, -1);
	ggz_write_char(my_socket, (char)0);
}


void change_room(void)
{
	int new_room=cur_room;

	if(rooms == 1)
		return;
	while(new_room == cur_room)
		new_room = random() % rooms;
	ggz_write_int(my_socket, REQ_ROOM_JOIN);
	ggz_write_int(my_socket, new_room);
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

	ggz_write_int(my_socket, REQ_CHAT);
	ggz_write_char(my_socket, GGZ_CHAT_NORMAL);
	ggz_write_string(my_socket, msg);
}
