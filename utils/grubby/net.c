/* net.c
 *
 * A GRUB ia A (G)GZ (R)eally (U)seful (B)ot
 *
 * Original: Rich Gade
 * ReWrite:  Justin Zaun
 *
 * May 23rd, 2000
 */

#include <easysock.h>
#include <stdlib.h>
#include <stdio.h>
#include "chat.h"
#include "datatypes.h"
#include "protocols.h"

extern struct Grubby grubby;
extern int sig;

void get_op_code( void )
{
	int	opcode, num, num2, num3;
	char	*string, *string2;
	char	chr;

	if(es_read_int(grubby.socket, &opcode))
		exit(1);

	switch(opcode)
	{
		case MSG_SERVER_ID:
			es_read_string_alloc(grubby.socket, &string);
			free(string);
			es_read_int(grubby.socket, &num);
			es_read_int(grubby.socket, &num2);
			grubby.chat_length = num2;
			sig = SIG_LOGIN;
			break;
		case MSG_SERVER_FULL:
			exit(1);
			break;
		case RSP_LOGIN_ANON:
			es_read_char(grubby.socket, &chr);
			if (chr == 0)
			{
			} else {
				exit(1);
			}
			es_read_int(grubby.socket, &num2);
			sig = SIG_CHANGEROOM;
			break;
		case MSG_MOTD:
			es_read_int(grubby.socket, &num);
			for (num2=0; num2<num; num2++)
			{
				es_read_string_alloc(grubby.socket, &string);
				free(string);
			}
			break;
		case RSP_ROOM_JOIN:
			es_read_char(grubby.socket, &chr);
			switch (chr)
			{
				case E_ROOM_FULL:
					break;
				case E_BAD_OPTIONS:
					break;
				case 0:
					send_chat( "All must kneel before thy almighty AGRUB!" );
					sig = SIG_IDLE;
					es_write_int( grubby.socket, REQ_LIST_PLAYERS );
					break;
				default:
					exit(1);
					break;
			}
			break;
		case RSP_CHAT:
			es_read_char (grubby.socket, &chr);
			break;
		case MSG_CHAT:
			es_read_char (grubby.socket, &chr);
			es_read_string_alloc (grubby.socket, &string);
			if (chr & GGZ_CHAT_M_MESSAGE)
				es_read_string_alloc (grubby.socket, &string2);
			else
				string2 = malloc(1);
			handle_chat( chr, string, string2 );

			free(string);
			free(string2);
			break;
		case MSG_UPDATE_ROOMS:
			break;
		case MSG_UPDATE_PLAYERS:
			es_read_char (grubby.socket, &chr);
			es_read_string_alloc (grubby.socket, &string);
			switch( chr )
			{
				case GGZ_UPDATE_ADD:
					handle_join( string );
					break;
				case GGZ_UPDATE_DELETE:
					handle_part( string );
					break;
			}
			free (string);
			break;
		case MSG_UPDATE_TABLES:
			break;
		case RSP_LOGOUT:
			sig = SIG_CLEANUP;
			break;
		case RSP_LIST_PLAYERS:
			es_read_int( grubby.socket, &num );
			for( num2=0; num2<num; num2++ )
			{
				es_read_string_alloc( grubby.socket, &string );
				es_read_int( grubby.socket, &num3 );
				free(string);
			}
			break;
		default:
			exit(1);
			break;
	}
}


void login( void )
{
	es_write_int(grubby.socket, REQ_LOGIN_ANON);
	es_write_string(grubby.socket, grubby.name);
}


void change_room( int room_num )
{
	es_write_int(grubby.socket, REQ_ROOM_JOIN);
	es_write_int(grubby.socket, room_num);
}
