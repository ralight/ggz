/* popt.c
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
#include "chat.h"
#include "datatypes.h"

extern struct Grubby grubby;


void start_log( char *file, char *type )
{
	char out[grubby.chat_length];

	/* Check if we are loggin already */
	if( grubby.logfile != NULL )
	{
		sprintf( out, "I'm already logging to %s.", grubby.logname );
		send_msg( grubby.owner, out );
		return;
	}

	/* Check for valid file type */
	if( !strcasecmp( type, "html" ) || !strcasecmp( type, "text" ) )
	{
	} else {
		sprintf( out, "Unknown file type: %s", type );
		send_msg( grubby.owner, out );
		sprintf( out, "Usage: %s log to <file> as [html|text].", grubby.name );
		send_msg( grubby.owner, out );
		return;
	}

	/* Open the log file */
	grubby.logfile = fopen( file, "w" );
	if( grubby.logfile == NULL )
	{
		sprintf( out, "Couldn'topen file %s.", file );
		send_msg( grubby.owner, out );
		return;
	}

	grubby.logname = malloc( sizeof( file ) );
	strcpy( grubby.logname, file);
	grubby.logtype = malloc( sizeof( type ) );
	strcpy( grubby.logtype, type );

	/* Initilize the log file */
	if( !strcasecmp( grubby.logtype, "HTML" ) )
	{
		fprintf( grubby.logfile, "<html>\n<title>GNU Gaming Zone Log File</title>\n<body bgcolor=\"white\">\n" );
		fprintf( grubby.logfile, "<center>\n<table width=750>\n" );
		fprintf( grubby.logfile, "<tr><td colspan=2>Server:</td><td>%s</td></tr>\n", grubby.host );
		fprintf( grubby.logfile, "<tr><td colspan=2>Port:</td><td>%d</td></tr>\n", grubby.port );
		fprintf( grubby.logfile, "<tr><td colspan=3><p><hr></p></td></tr>\n" );
	}

	send_chat( "I've been instructed to log this room to file. This" );
	send_chat( "notice is to allow anyone not wishing to be logged" );
	send_chat( "to leave this room." );
}

void end_log( void )
{
	/* check if we are logging */
	if ( grubby.logfile == NULL )
	{
		send_msg( grubby.owner, "I'm not cruuently logging anything." );
		return;
	}

	/* clean up log file */
	if( !strcasecmp( grubby.logtype, "HTML" ) )
	{
		fprintf( grubby.logfile, "<tr><td colspan=3><p><hr></p></td></tr>\n" );
		fprintf( grubby.logfile, "<tr><td colspan=3 align=center>Recorded by %s for %s</td></tr>", grubby.name, grubby.owner );
		fprintf( grubby.logfile, "</table>\n</center>\n</body>\n</html>\n" );
	}

	/* close the log file */
	fclose( grubby.logfile );

	/* reset log information */
	grubby.logfile = NULL;
	free( grubby.logname );
	grubby.logname = NULL;
	free( grubby.logtype );
	grubby.logtype = NULL;

	send_msg( grubby.owner, "I've stopped logging the chat." );
}

void log_write( char *left, char *right )
{
	if( !strcasecmp( grubby.logtype, "HTML" ) )
	{
		fprintf( grubby.logfile, "<tr><td valign=top alighn=right>%s&nbsp</td><td>&nbsp</td>", left );
		fprintf( grubby.logfile, "<td valign=top>&nbsp %s</td></tr>\n", right );
	}
	else if( !strcasecmp( grubby.logtype, "TEXT" ) )
		fprintf( grubby.logfile, "%s | %s\n", left, right );
}
