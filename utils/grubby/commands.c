/* commands.c
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
#include "datatypes.h"
#include "log.h"
#include "messages.h"
#include "rc.h"
#include "support.h"
#include "net.h"

struct Grubby grubby;
struct Memory memory;

void show_owner_help( void );
void show_public_help( char *from );
void show_time( char *from, char *name );
void toggle_lang_checker( char *word );
void show_stats( void );
void add_name( char *from, char *name );
void add_message( char *from, char *message );
void check_messages( char *from );

void owner_commands( char **words, int totalwords )
{
	if( !strcasecmp(words[0], grubby.name) )
	{
		switch( totalwords )
		{
			case 2:
				if( !strcasecmp(words[1], "help") )
					show_owner_help();

				if( !strcasecmp(words[1], "stats") )
					show_stats();
				break;
			case 3:
				if( !strcasecmp( words[1], "goto" ) )
					change_room(atoi(words[2]));

				if( !strcasecmp( words[1], "log" ) && !strcasecmp( words[2], "off" ) )
					end_log();

				if( !strcasecmp( words[1], "save" ) && !strcasecmp( words[2], "memory" ) )
					save_memory();
			case 5:
				if( !strcasecmp( words[1], "turn" ) && !strcasecmp( words[3], "lang") &&
				    !strcasecmp( words[4], "checker" ) )
					toggle_lang_checker( words[2] );
				break;
			case 6:
				if( !strcasecmp( words[1], "log" ) && !strcasecmp( words[2], "to" ) &&
				    !strcasecmp( words[4], "as" ) )
					start_log( words[3], words[5] );
				break;
			default:
				break;
		}
	}
}

void public_commands( char *from, char **words, int totalwords, char *fullmessage )
{
	char out[grubby.chat_length];
	int i;

	if( !strcasecmp(words[0], grubby.name) )
	{
		switch( totalwords )
		{
			case 2:
				if( !strcasecmp(words[1], "help") )
				{
					show_public_help( from );
					return;
				}

				if( !strcasecmp(words[1], "about") )
				{
					for(i=0; i<sizeof(about_strings)/sizeof(char *); i++)
						send_chat_insert_name( from, about_strings[i] );
					return;
				}
				break;
			case 5:
				if( !strcasecmp( words[1], "have" ) && !strcasecmp( words[2], "you") &&
				    !strcasecmp( words[3], "seen" ) )
				{
					show_time( from, words[4] );
					return;
				}
				break;
			case 6:
				if( !strcasecmp( words[1], "do" ) && !strcasecmp( words[2], "i") &&
				    !strcasecmp( words[3], "have" ) && !strcasecmp( words[4], "any" ) &&
				    !strcasecmp( words[5], "messages" ) )
					check_messages( from );

			default:
				break;
		}

		sprintf( out, "%s my name is ", grubby.name );
		if( !strncasecmp( fullmessage, out, strlen( out ) ) )
		{
			add_name( from, fullmessage+strlen( out ) );
			return;
		}

		sprintf( out, "%s tell ", grubby.name );
		if( !strncasecmp( fullmessage, out, strlen( out ) ) )
		{
			add_message( from, fullmessage+strlen( out ) );
			return;
		}

		sprintf( out, "%s say ", grubby.name );
		if( !strncasecmp( fullmessage, out, strlen( out ) ) )
		{
			send_chat( fullmessage+strlen( out ) );
			return;
		}
	}
}

void show_time( char *from, char *name )
{
	int i, loc;
	int days, hours, mins, secs;
	char out[grubby.chat_length];
	char out2[grubby.chat_length];

	/* Check if we know the user */
	i = check_known( name );
	if( i == -1 )
	{
		sprintf( out, "Sorry %s, I've never seen %s before.", get_name( from ), get_name( name ) );
		send_msg( from, out );
		return;
	}

	/* Check if they are asking for themself*/
	if( !strcmp(name, from) )
	{
		sprintf( out, "Well %s, I'm looking right at you!", get_name( name ) );
		send_msg( from, out );
		return;
	}

	/* Print out the time sence last seen */
	secs = difftime( time(NULL), memory.people[i].lastseen );
	days = secs/86400;
	secs -= days*86400;
	hours = secs/3600;
	secs -= hours*3600;
	mins = secs/60;
	secs -= mins*60;

	loc = 0;
	if( days > 0 )
	{
		if( days > 1 )
			sprintf(out, "%d days, ", days);
		else
			sprintf(out, "1 day, ");
	}
	if( hours > 0 )
	{
		if( hours > 1 )
			loc = sprintf(out+loc, "%d hours, ", hours);
		else
			loc = sprintf(out+loc, "1 hour, ");
	}
	if( mins != 1 )
		loc = sprintf(out+loc, "%d minutes and ", mins);
	else
		loc = sprintf(out+loc, "1 minute and ");
	if( secs != 1 )
		loc = sprintf(out+loc, "%d seconds", secs);
	else
		loc = sprintf(out+loc, "1 second");

	sprintf( out2, "I last greeted %s %s ago.", get_name( name ), out);
	send_msg( from, out2 );
}

void show_public_help( char *from )
{
	char out[grubby.chat_length];

	send_msg( from, "          Public Commands I Understand" );
	send_msg( from, "          ----------------------------" );
	send_msg( from, " " );
	send_msg( from, "\00314about" );
	send_msg( from, "I'll tell you a little about my history." );
	send_msg( from, " ");
	send_msg( from, "\00314help");
	send_msg( from, "Displays the commands that I will accept from you." );
	send_msg( from, " ");
	send_msg( from, "\00314have you seen <username>" );
	send_msg( from, "Will return how long ago I saw <username>." );
	send_msg( from, " ");
	send_msg( from, "\00314my name is <real name>" );
	send_msg( from, "I'd love to get to know you a little better." );
	send_msg( from, " ");
	send_msg( from, "\00314tell <username> <message>" );
	send_msg( from, "I'll tell <username> your <message> next time I see them." );
	send_msg( from, " ");
	send_msg( from, "\00314do i have any messages" );
	send_msg( from, "Get any messages people may have left you." );
	send_msg( from, " ");
	send_msg( from, " ");
	send_msg( from, "When you're telling, or asking something of me, please make sure to address me properly:");
	send_msg( from, " ");
	sprintf( out,   "        \00314%s <command>", grubby.name);
	send_msg( from, out );
}

void show_owner_help( void )
{
	send_msg( grubby.owner, "           Owner Commands I Understand" );
	send_msg( grubby.owner, "           ---------------------------" );
	send_msg( grubby.owner, " " );
	send_msg( grubby.owner, "\00314goto <room number>" );
	send_msg( grubby.owner, "goto the new room");
	send_msg( grubby.owner, " " );
	send_msg( grubby.owner, "\00314turn [on|off] lang checker");
	send_msg( grubby.owner, "Turns on or off the foul language checker." );
	send_msg( grubby.owner, " " );
	send_msg( grubby.owner, "\00314stats" );
	send_msg( grubby.owner, "Lists out all of the current settings." );
	send_msg( grubby.owner, " " );
	send_msg( grubby.owner, "\00314log to <file> as [html|text]");
	send_msg( grubby.owner, "Starts loggint the room to a file in the given format." );
	send_msg( grubby.owner, " " );
	send_msg( grubby.owner, "\00314log off");
	send_msg( grubby.owner, "Turn logging to file off." );
	send_msg( grubby.owner, " " );
	send_msg( grubby.owner, "\00314save memory");
	send_msg( grubby.owner, "Force save of memory to file.");
	send_msg( grubby.owner, " " );
	send_msg( grubby.owner, " " );
}

void toggle_lang_checker( char *word )
{
	if( !strcasecmp(word, "on") && grubby.lang_check == 0)
	{
		send_msg( grubby.owner, "It's already on." );
		return;
	}
	if( !strcasecmp(word, "on") && grubby.lang_check == 1)
	{
		grubby.lang_check = 0;
		send_msg( grubby.owner, "Language checker turned on." );
		return;
	}

	if( !strcasecmp(word, "off") && grubby.lang_check == 1)
	{
		send_msg( grubby.owner, "It's already off." );
		return;
	}
	if( !strcasecmp(word, "off") && grubby.lang_check == 0)
	{
		grubby.lang_check = 1;
		send_msg( grubby.owner, "Language checker turned off." );
		return;
	}
}

void show_stats( void )
{
	char out[grubby.chat_length];

	sprintf( out, "Name:  %s", grubby.name );
	send_msg( grubby.owner, out );
	sprintf( out, "Owner: %s", grubby.owner );
	send_msg( grubby.owner, out );
	sprintf( out, "Known People: %d", memory.num_people );
	send_msg( grubby.owner, out );
	send_msg( grubby.owner, "------------------------" );
	if( grubby.lang_check == 0 )
		send_msg( grubby.owner, "Lang Checker: ON" );
	else
		send_msg( grubby.owner, "Lang Checker: OFF" );
	if( grubby.logfile != NULL )
	{
		sprintf( out, "Logging: %s", grubby.logname );
		send_msg( grubby.owner, out );
	} else
		send_msg( grubby.owner, "Logging: OFF" );

}

void add_name( char *from, char *name )
{
	int i;
	char out[grubby.chat_length];

	/* Check if we know the user */
	i = check_known( from );
	if( i == -1 )
	{

	} else {
		memory.people[i].realname = malloc( sizeof( char ) * ( strlen( name ) + 1 ) );
		strcpy( memory.people[i].realname, name);
		sprintf( out, "I'll remember your name as %s from now on %s.", memory.people[i].realname, from );
		send_msg( from, out );
	}
}

void add_message( char *from, char *message )
{
	int i, size;
	char *name = NULL;
	char *text = NULL;
	char out[grubby.chat_length];

	/* Get the name and message */
	size = strlen( message )+1;
	for( i=0; i<size; i++ )
		if( message[i] == ' ' )
		{
			message[i] = '\0';
			name = message;
			text = message+i+1;
			break;
		}

	/* Make sure we have a name and message */
	if( name == NULL || text == NULL )
	{
		send_msg( from, "Usage: tell <username> <message>" );
		return;
	}

	/* Check if we know the user */
	i = check_known( name );
	if( i == -1 )
	{
		sprintf( out, "Sorry %s, I don't know %s.", get_name( from ), name );
		send_msg( from, out );
	} else {
		memory.people[i].msg[memory.people[i].msgcount].text = malloc( sizeof( char ) * ( strlen( text ) + 1 ) );
		strcpy( memory.people[i].msg[memory.people[i].msgcount].text, text);
		strcpy( memory.people[i].msg[memory.people[i].msgcount].from, from);
		memory.people[i].msg[memory.people[i].msgcount].timestamp = time(NULL);
		memory.people[i].msgcount++;
		sprintf( out, "I'll be sure to tell %s for you.", get_name( name ) );
		send_msg( from, out );
		sprintf( out, "Hey, you got a message from %s.", get_name( from ) );
		send_msg( name, out );
	}
}

void check_messages( char *from )
{
	int i,m;
	char dt[100];
	char out[grubby.chat_length];

	/* Check if we know the user */
	i = check_known( from );
	if( i == -1 )
	{
		sprintf( out, "Sorry %s, I don't know you.", from );
		send_msg( from, out );
	} else {
                /* Check for messages */
                if( memory.people[i].msgcount > 0 )
                {       
                        for( m=0; m<memory.people[i].msgcount; m++ )
                        {
                                strftime( dt, 100, "%b %d @ %I:%M%p", localtime( &memory.people[i].msg[m].timestamp ) );
                                sprintf( out, "Message %d from %s was left at %s:",
                                         m+1, get_name( memory.people[i].msg[m].from ), dt );
                                send_msg( from, out ); 
                                sprintf( out, "       %s", memory.people[i].msg[m].text );
                                send_msg( from, out );
                                free( memory.people[i].msg[m].text );
                        }
                        memory.people[i].msgcount = 0;
                } else {
			sprintf( out, "Sorry %s, but I guess your not important enough to get any messages.",
				 get_name( from ) );
			send_msg( from, out );
		}
	}
}
