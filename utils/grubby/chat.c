/* chat.c
 *
 * A GRUB ia A (G)GZ (R)eally (U)seful (B)ot
 *
 * Original: Rich Gade
 * ReWrite:  Justin Zaun
 *
 * May 23rd, 2000
 */

#include <easysock.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "chat.h"
#include "commands.h"
#include "datatypes.h"
#include "log.h"
#include "protocols.h"
#include "support.h"

extern struct Grubby grubby;
extern struct Memmory memmory;

void show_bad_lang( char *from );

void handle_chat( unsigned char OpCode, char *from, char *message )
{
	int i, word_c, old, size;
	char **words;
	char out[grubby.chat_length + strlen(grubby.name) + 1];

	old = 0;

	/* Personal Message */
	if( OpCode == GGZ_CHAT_PERSONAL )
		sprintf( out, "%s %s", grubby.name, message );
	else
		strcpy( out, message );

	/* Personal Message */
	if( OpCode == GGZ_CHAT_PERSONAL )
		word_c=1;
	else
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
	
	/* Personal Message */
	if( OpCode == GGZ_CHAT_PERSONAL )
		word_c=1;
	else
		word_c=0;

	size = strlen(message)+1;
	for( i=0; i<size; i++)
	{
		if( message[i] == ' ' || message[i] == '\0' )
		{
			message[i] = '\0';
			words[word_c] = message + old;
			if( grubby.lang_check == 0 && (
			    !strcasecmp(message + old, "fuck") || !strcasecmp(message + old, "bitch") ||
			    !strcasecmp(message + old, "fucker") || !strcasecmp(message + old, "shit") ||
			    !strcasecmp(message + old, "cock") || !strcasecmp(message + old, "dick") ||
			    !strcasecmp(message + old, "cunt") ||
			    !strcasecmp(message + old, "pussy") ))
				show_bad_lang( from );
			old = i+1;
			word_c++;
		}
	}

	/* Personal Message */
	if( OpCode == GGZ_CHAT_PERSONAL )
		words[0] = grubby.name;

	i = check_known( from );
	/* Unknown person */
	if( i == -1 )
	{
		if( i < MAX_PEOPLE )
		{
			memmory.people[memmory.num_people].username = malloc( sizeof(char) * ( strlen(from) + 1 ) );
			strcpy( memmory.people[memmory.num_people].username, from );
			memmory.people[memmory.num_people].lastseen = time(NULL);
			memmory.num_people++;
		}
	}

	/* Known person */
	else
		memmory.people[i].lastseen = time(NULL);

	if( !strcmp(grubby.owner, from) )
		owner_commands( words, word_c );
	public_commands( from, words, word_c, out );

	/* check if we are logging */
	if( grubby.logfile != NULL )
	{
		switch( OpCode )
		{
			case GGZ_CHAT_NORMAL:
				log_write( from, out );
				break;
		}
	}
}

void handle_join( char *from )
{
	char out[grubby.chat_length];

	int i, m;
	char dt[30];

	i = check_known( from );

	/* Unknown person */
	if( i == -1 )
	{
		sprintf( out, "Hi %s, I'm %s.", from, grubby.name );
		send_msg( from, out );
		sprintf( out, "I haven't seen you before, please take a second to see what I can do. Type: %s, help", grubby.name );
		send_msg( from, out );
		if( i < MAX_PEOPLE )
		{
			memmory.people[memmory.num_people].username = malloc( sizeof(char) * ( strlen(from) + 1 ) );
			strcpy( memmory.people[memmory.num_people].username, from );
			memmory.people[memmory.num_people].lastseen = time(NULL);
			memmory.num_people++;
		} else {
			sprintf( out, "Sorry %s, but my memmory is all filled up, ask %s to make it bigger!",
				 from, grubby.owner );
			send_msg( from, out );
		}
	}

	/* Known person */
	else
	{
		sprintf( out, "Welcome back, %s.", get_name( from ) );
		send_msg( from, out );
		memmory.people[i].lastseen = time(NULL);

		/* Check for messages */
		if( memmory.people[i].msgcount > 0 )
		{
			for( m=0; m<memmory.people[i].msgcount; m++ )
			{
				strftime( dt, 100, "%b %d @ %I:%M%p", localtime( &memmory.people[i].msg[m].timestamp ) );
				sprintf( out, "Message %d from %s was left at %s:", 
					 m+1, get_name( memmory.people[i].msg[m].from ), dt );
				send_msg( from, out );
				sprintf( out, "       %s", memmory.people[i].msg[m].text );
				send_msg( from, out );
				free( memmory.people[i].msg[m].text );
			}
			memmory.people[i].msgcount = 0;
		}
	}

	/* check if we are logging */
	if( grubby.logfile != NULL )
	{
		log_write( "-->", from );
		send_msg( from, "This channel is being logged to file." );
	}
}

void handle_part( char *from )
{
	char out[grubby.chat_length];

	int i;

	/* Check wether we already know the person */
	i = check_known( from );

	/* Unknown person */
	if( i == -1 )
	{
		sprintf( out, "Yo %s, catch ya later! Have fun playin all these games.", from);
		send_msg( from, out );
	}

	/* Known person */
	if ( i < memmory.num_people )
	{
		sprintf( out, "Yo %s, catch ya later! Have fun playin all these games.", get_name( from ) );
		send_msg( from, out );
	}

	/* check if we are logging */
	if( grubby.logfile != NULL )
		log_write( "<--", from );
}

/*
 * Send a private message to a user
 */
void send_msg( char *to, char *message )
{
	es_write_int( grubby.socket, REQ_CHAT );
	es_write_char( grubby.socket, GGZ_CHAT_PERSONAL );
	es_write_string( grubby.socket, to);
	es_write_string( grubby.socket, message);
}

/*
 * Send a public message to the current room
 */
void send_chat( char *message )
{
	es_write_int( grubby.socket, REQ_CHAT );
	es_write_char( grubby.socket, GGZ_CHAT_NORMAL );
	es_write_string( grubby.socket, message );
}

/*
 * Tell a user to watch there language
 */
void show_bad_lang( char *from )
{
	char out[grubby.chat_length];

	sprintf( out, "%s, please watch your language.", from);
	send_chat( out );
}

void send_chat_insert_name( char *from, char *msg )
{
	char *p;
	char out[grubby.chat_length];
	int percent_s=0;

	for( p=msg; *p!='\0'; p++ )
		if( *p == '%' && *(p+1) == 's' )
			percent_s++;

	if( percent_s )
		sprintf( out, msg, grubby.name ); 
	else
		strcpy( out, msg );

	send_msg( from, out );
}

