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
#include "support.h"
#include "net.h"

struct Grubby grubby;

void commands_show_owner_help( void );
void commands_show_public_help( char *from );
void commands_show_time( char *from, char *name );
void commands_toggle_lang_checker( char *word );
void commands_show_stats( void );
void commands_add_name( char *from, char *name );
void commands_add_message( char *from, char *message );
void commands_check_messages( char *from );
void commands_alert_add( char *from, char *name );

void owner_commands( char **words, int totalwords )
{
	if( !strcasecmp(words[0], grubby.name) )
	{
		switch( totalwords )
		{
			case 2:
				if( !strcasecmp(words[1], "help") )
					commands_show_owner_help();

				if( !strcasecmp(words[1], "stats") )
					commands_show_stats();
				break;
			case 3:
				if( !strcasecmp( words[1], "goto" ) )
					net_change_room(atoi(words[2]));

				if( !strcasecmp( words[1], "log" ) && !strcasecmp( words[2], "off" ) )
					end_log();

				if( !strcasecmp( words[1], "save" ) && !strcasecmp( words[2], "memory" ) )
					ggzcore_conf_commit();
			case 5:
				if( !strcasecmp( words[1], "turn" ) && !strcasecmp( words[3], "lang") &&
				    !strcasecmp( words[4], "checker" ) )
					commands_toggle_lang_checker( words[2] );
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
	char out[1024];
	int i;

	if( !strcasecmp(words[0], grubby.name) )
	{
		switch( totalwords )
		{
			case 2:
				if( !strcasecmp(words[1], "help") )
				{
					commands_show_public_help( from );
					return;
				}

				if( !strcasecmp(words[1], "about") )
				{
					for(i=0; i<sizeof(about_strings)/sizeof(char *); i++)
						net_send_chat_insert_name( from, about_strings[i] );
					return;
				}
				break;
			case 4:
				if( !strcasecmp( words[1], "alert" ) && !strcasecmp( words[2], "add") )
				{
					commands_alert_add( from, words[3] );
					return;
				}
				break;
			case 5:
				if( !strcasecmp( words[1], "have" ) && !strcasecmp( words[2], "you") &&
				    !strcasecmp( words[3], "seen" ) )
				{
					commands_show_time( from, words[4] );
					return;
				}
				break;
			case 6:
				if( !strcasecmp( words[1], "do" ) && !strcasecmp( words[2], "i") &&
				    !strcasecmp( words[3], "have" ) && !strcasecmp( words[4], "any" ) &&
				    !strcasecmp( words[5], "messages" ) )
					commands_check_messages( from );

			default:
				break;
		}

		sprintf( out, "%s my name is ", grubby.name );
		if( !strncasecmp( fullmessage, out, strlen( out ) ) )
		{
			commands_add_name( from, fullmessage+strlen( out ) );
			return;
		}

		sprintf( out, "%s tell ", grubby.name );
		if( !strncasecmp( fullmessage, out, strlen( out ) ) )
		{
			commands_add_message( from, fullmessage+strlen( out ) );
			return;
		}

		sprintf( out, "%s say ", grubby.name );
		if( !strncasecmp( fullmessage, out, strlen( out ) ) )
		{
			net_send_msg( fullmessage+strlen( out ) );
			return;
		}
	}
}

void commands_show_time(char *from, char *name)
{
	int i;
	int days, hours, mins, secs;
	char out[1024];
	char out2[1024];

	memset(out, '\0', strlen(out));
	memset(out2, '\0', strlen(out2));

	/* Check if we know the user */
	i = support_check_known(name);
	if(i == -1)
	{
		sprintf(out, "Sorry %s, I've never seen %s before.", support_get_name(from), support_get_name(name));
		net_send_prvmsg(from, out);
		return;
	}

	/* Check if they are asking for themself*/
	if(!strcmp(name, from))
	{
		sprintf(out, "Well %s, I'm looking right at you!", support_get_name(name));
		net_send_prvmsg(from, out);
		return;
	}

	/* Print out the time sence last seen */
	secs = difftime( time(NULL), ggzcore_conf_read_int(name, "LASTSEEN", -1));
	days = secs/86400;
	secs -= days*86400;
	hours = secs/3600;
	secs -= hours*3600;
	mins = secs/60;
	secs -= mins*60;

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
			sprintf(out2, "%s %d hours, ", out, hours);
		else
			sprintf(out2, "%s 1 hour, ", out);
	}
	if( mins != 1 )
		sprintf(out, "%s %d minutes and ", out2, mins);
	else
		sprintf(out, "%s 1 minute and ", out2);
	if( secs != 1 )
		 sprintf(out2, "%s %d seconds", out, secs);
	else
		sprintf(out2, "%s 1 second", out);

	sprintf(out, "I last greeted %s %s ago.", support_get_name(name), out2);
	net_send_prvmsg(from, out);
}

void commands_show_public_help( char *from )
{
	char out[1024];

	net_send_prvmsg( from, "          Public Commands I Understand" );
	net_send_prvmsg( from, "          ----------------------------" );
	net_send_prvmsg( from, " " );
	net_send_prvmsg( from, "\00314about" );
	net_send_prvmsg( from, "I'll tell you a little about my history." );
	net_send_prvmsg( from, " ");
	net_send_prvmsg( from, "\00314help");
	net_send_prvmsg( from, "Displays the commands that I will accept from you." );
	net_send_prvmsg( from, " ");
	net_send_prvmsg( from, "\00314have you seen <username>" );
	net_send_prvmsg( from, "Will return how long ago I saw <username>." );
	net_send_prvmsg( from, " ");
	net_send_prvmsg( from, "\00314my name is <real name>" );
	net_send_prvmsg( from, "I'd love to get to know you a little better." );
	net_send_prvmsg( from, " ");
	net_send_prvmsg( from, "\00314tell <username> <message>" );
	net_send_prvmsg( from, "I'll tell <username> your <message> next time I see them." );
	net_send_prvmsg( from, " ");
	net_send_prvmsg( from, "\00314do i have any messages" );
	net_send_prvmsg( from, "Get any messages people may have left you." );
	net_send_prvmsg( from, " ");
	net_send_prvmsg( from, " ");
	net_send_prvmsg( from, "When you're telling, or asking something of me, please make sure to address me properly:");
	net_send_prvmsg( from, " ");
	sprintf( out,   "        \00314%s <command>", grubby.name);
	net_send_prvmsg( from, out );
}

void commands_show_owner_help( void )
{
	net_send_prvmsg( grubby.owner, "           Owner Commands I Understand" );
	net_send_prvmsg( grubby.owner, "           ---------------------------" );
	net_send_prvmsg( grubby.owner, " " );
	net_send_prvmsg( grubby.owner, "\00314goto <room number>" );
	net_send_prvmsg( grubby.owner, "goto the new room");
	net_send_prvmsg( grubby.owner, " " );
	net_send_prvmsg( grubby.owner, "\00314turn [on|off] lang checker");
	net_send_prvmsg( grubby.owner, "Turns on or off the foul language checker." );
	net_send_prvmsg( grubby.owner, " " );
	net_send_prvmsg( grubby.owner, "\00314stats" );
	net_send_prvmsg( grubby.owner, "Lists out all of the current settings." );
	net_send_prvmsg( grubby.owner, " " );
	net_send_prvmsg( grubby.owner, "\00314log to <file> as [html|text]");
	net_send_prvmsg( grubby.owner, "Starts loggint the room to a file in the given format." );
	net_send_prvmsg( grubby.owner, " " );
	net_send_prvmsg( grubby.owner, "\00314log off");
	net_send_prvmsg( grubby.owner, "Turn logging to file off." );
	net_send_prvmsg( grubby.owner, " " );
	net_send_prvmsg( grubby.owner, "\00314save memory");
	net_send_prvmsg( grubby.owner, "Force save of memory to file.");
	net_send_prvmsg( grubby.owner, " " );
	net_send_prvmsg( grubby.owner, " " );
}

void commands_toggle_lang_checker( char *word )
{
	if( !strcasecmp(word, "on") && ggzcore_conf_read_int("GRUBBYSETTINGS", "LANGCHECK", 1) == 0)
	{
		net_send_prvmsg( grubby.owner, "It's already on." );
		return;
	}
	if( !strcasecmp(word, "on") && ggzcore_conf_read_int("GRUBBYSETTINGS", "LANGCHECK", 1) == 1)
	{
		ggzcore_conf_write_int("GRUBBYSETTINGS", "LANGCHECK", 0);
		net_send_prvmsg( grubby.owner, "Language checker turned on." );
		return;
	}

	if( !strcasecmp(word, "off") && ggzcore_conf_read_int("GRUBBYSETTINGS", "LANGCHECK", 1) == 1)
	{
		net_send_prvmsg( grubby.owner, "It's already off." );
		return;
	}
	if( !strcasecmp(word, "off") && ggzcore_conf_read_int("GRUBBYSETTINGS", "LANGCHECK", 1) == 0)
	{
		ggzcore_conf_write_int("GRUBBYSETTINGS", "LANGCHECK", 1);
		net_send_prvmsg( grubby.owner, "Language checker turned off." );
		return;
	}
}

void commands_show_stats( void )
{
	char out[1024];

	sprintf( out, "Name:  %s", grubby.name );
	net_send_prvmsg( grubby.owner, out );
	sprintf( out, "Owner: %s", grubby.owner );
	net_send_prvmsg( grubby.owner, out );
	sprintf( out, "Known People: %d", -1 );
	net_send_prvmsg( grubby.owner, out );
	net_send_prvmsg( grubby.owner, "------------------------" );
}

void commands_add_name( char *from, char *name )
{
	char out[1024];

	/* Check if we know the user */
	ggzcore_conf_write_string(from, "REALNAME", name);
	sprintf(out, "I'll remember your name as %s from now on %s.", name, from);
	net_send_prvmsg(from, out);
}

void commands_add_message( char *from, char *message )
{
	int i, x, size;
	char *name = NULL;
	char *text = NULL;
	char out[1024];
	int num_msg;
	char **oldmessages;
	char **messages;

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
		net_send_prvmsg( from, "Usage: tell <username> <message>" );
		return;
	}

	/* Check if we know the user */
	i = support_check_known( name );
	if( i == -1 )
	{
		sprintf(out, "Sorry %s, I don't know %s.", support_get_name(from), name);
		net_send_prvmsg(from, out );
	} else {
		ggzcore_conf_read_list(name, "MESSAGES", &num_msg, &oldmessages);
		messages = calloc( num_msg+2, sizeof(char*));
		for (x = 0; x < num_msg; x++)
			messages[x] = strdup(oldmessages[x]);
		messages[num_msg] = strdup(from);
		messages[num_msg+1] = strdup(text);
		ggzcore_conf_write_list(name, "MESSAGES", num_msg+2, messages);

		sprintf( out, "I'll be sure to tell %s for you.", support_get_name( name ) );
		net_send_prvmsg( from, out );
		sprintf( out, "Hey, you got a message from %s.", support_get_name( from ) );
		net_send_prvmsg( name, out );
		free(messages);
		free(oldmessages);
	}
}

void commands_check_messages( char *from )
{
	int i,m;
	char out[1024];
	int num_msg;
	char **messages;

	/* Check if we know the user */
	i = support_check_known( from );
	if( i == -1 )
	{
		sprintf( out, "Sorry %s, I don't know you.", from );
		net_send_prvmsg( from, out );
	} else {
                /* Check for messages */
		ggzcore_conf_read_list(from, "MESSAGES", &num_msg, &messages);
                if( num_msg > 0 )
                {       
                        for( m=0; m<num_msg; m=m+2 )
                        {
				sprintf(out, "Message %d from %s:", m/2+1, messages[m]);
                                net_send_prvmsg( from, out ); 
                                sprintf( out, "       %s", messages[m+1] );
                                net_send_prvmsg( from, out );
                        }
			ggzcore_conf_remove_key(from, "MESSAGES");
                } else {
			sprintf( out, "Sorry %s, but I guess your not important enough to get any messages.",
				 support_get_name( from ) );
			net_send_prvmsg( from, out );
		}
	}
}

void commands_alert_add( char *from, char *name )
{
	int i, x;
	char out[1024];
	int num_alerts;
	char **oldalerts;
	char **alerts;

	/* Make sure we have a name */
	if( name == NULL )
	{
		net_send_prvmsg( from, "Usage: alert add <username>" );
		return;
	}

	/* Check if we know the user */
	i = support_check_known( name );
	if( i == -1 )
	{
		sprintf(out, "Sorry %s, I don't know %s.", support_get_name(from), name);
		net_send_prvmsg(from, out );
	} else {
		ggzcore_conf_read_list(name, "ALERTS", &num_alerts, &oldalerts);
		alerts = calloc( num_alerts+1, sizeof(char*));
		for (x = 0; x < num_alerts; x++)
		{
			alerts[x] = strdup(oldalerts[x]);
			if(!strcmp(alerts[x], from))
			{
				sprintf(out, "I'm already alerting you when I see %s.", support_get_name(name));
				net_send_prvmsg(from, out);
				free(alerts);
				free(oldalerts);
				return;
			}
		}
		alerts[num_alerts] = strdup(from);
		ggzcore_conf_write_list(name, "ALERTS", num_alerts+1, alerts);

		sprintf( out, "I'll be sure to alert you when %s is online.", support_get_name(name) );
		net_send_prvmsg( from, out );
		free(alerts);
		free(oldalerts);
	}
}

