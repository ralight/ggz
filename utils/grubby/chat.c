/* chat.c
 *
 * A GRUB ia A (G)GZ (R)eally (U)seful (B)ot
 *
 * Original: Rich Gade
 * ReWrite:  Justin Zaun
 *
 * May 23rd, 2000
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "chat.h"
#include "commands.h"
#include "datatypes.h"
#include "log.h"
#include "net.h"
#include "protocols.h"
#include "support.h"

extern struct Grubby grubby;

void chat_show_bad_lang( char *from );

void chat_parse( char *from, char *message )
{
	int i, x, word_c, old, size;
	char **words;
	char out[1024];
	char item[3];
	int langcheck, totalbadwords;

	old = 0;
	size = strlen(message);
	for( i=0; i<size; i++)
	{
		if( message[i] == ':' || message[i] == ',' )
		{
			for( x=i; x < size; x++)
			{
				message[x] = message[x+1];
			}
		}
	}

	strncpy(out, message, 1024);

	word_c=0;
	
	/* Prescan for total number of words */
	for( i=0; i<strlen( message )+1; i++)
	{
		if( message[i] == ' ' || message[i] == '\0' )
			word_c++;
	}

	/* calloc the array */
	if( ( words = calloc(word_c, sizeof(char *)) ) == NULL )
	{
		printf( "  \\- calloc failed in handle_chat()\n");
		exit(1);
	}
	
	word_c=0;

	langcheck = ggzcore_conf_read_int("GRUBBYSETTINGS", "LANGCHECK", -1);
	totalbadwords = ggzcore_conf_read_int("BADWORDS", "TOTAL", 0);
	
	size = strlen(message)+1;
	for( i=0; i<size; i++)
	{
		if( message[i] == ' ' || message[i] == '\0' )
		{
			message[i] = '\0';
			words[word_c] = message + old;
			if(langcheck == 0)
			{
				for(x=1; x<totalbadwords+1; x++)
				{
					sprintf(item,"%d",x);
					if(!strcasecmp(message + old, ggzcore_conf_read_string("BADWORDS", item, "")))
					{
						chat_show_bad_lang( from );
						return;
					}
				}
			}
			old = i+1;
			word_c++;
		}
	}

	i = support_check_known( from );
	/* Unknown person */
	if( i == -1 )
	{
	}

	/* Known person */
//	else

	if( !strcmp(grubby.owner, from) )
		owner_commands( words, word_c );

	public_commands( from, words, word_c, out );

	/* check if we are logging */
	if( grubby.logfile != NULL )
	{

	}
}

void handle_part( char *from )
{
	char out[grubby.chat_length];

	int i;

	/* Check wether we already know the person */
	i = support_check_known( from );

	/* Unknown person */
	if( i == -1 )
	{
		sprintf( out, "Yo %s, catch ya later! Have fun playin all these games.", from);
		net_send_prvmsg( from, out );
	} else {
		sprintf( out, "Yo %s, catch ya later! Have fun playin all these games.", support_get_name( from ) );
		net_send_prvmsg( from, out );
	}

	/* check if we are logging */
	if( grubby.logfile != NULL )
		log_write( "<--", from );
}

void chat_show_bad_lang( char *from )
{
	char out[1024];

	sprintf( out, "%s, please watch your language.", from);
	net_send_msg( out );
}

void chat_chatter()
{
	int x, y, num_chatters;
	char item[4];

	num_chatters = ggzcore_conf_read_int("CHATTERS", "TOTAL", 0);
	if(ggzcore_conf_read_int("GRUBBYSETTINGS", "DOCHATTER", 1) == 0)
	{
		x = 1+(int) (500.0*rand()/(RAND_MAX+1.0));
		if(x >= 100 && x <= 100)
		{
			if(num_chatters>0)
			{
				y = 1+(int) ((float)num_chatters*rand()/(RAND_MAX+1.0));
				sprintf(item,"%d", y);
				net_send_msg(ggzcore_conf_read_string("CHATTERS", item, ""));
			}
		}
	}
}
