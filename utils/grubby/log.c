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
#include "net.h"

extern struct Grubby grubby;


void start_log( char *file, char *type )
{
	char out[1024];

	/* Check if we are loggin already */
	if( grubby.logfile != NULL )
	{
		sprintf( out, "I'm already logging to %s.", grubby.logname );
		net_send_prvmsg( grubby.owner, out );
		return;
	}

	/* Check for valid file type */
	if( !strcasecmp( type, "html" ) || !strcasecmp( type, "text" ) )
	{
	} else {
		sprintf( out, "Unknown file type: %s", type );
		net_send_prvmsg( grubby.owner, out );
		sprintf( out, "Usage: %s log to <file> as [html|text].", grubby.name );
		net_send_prvmsg( grubby.owner, out );
		return;
	}

	/* Open the log file */
	grubby.logfile = fopen( file, "w" );
	if( grubby.logfile == NULL )
	{
		sprintf( out, "Couldn'topen file %s.", file );
		net_send_prvmsg( grubby.owner, out );
		return;
	}

	grubby.logname = malloc( sizeof( char ) * ( strlen( file ) + 1 ) );
	strcpy( grubby.logname, file);
	grubby.logtype = malloc( sizeof( char ) * ( strlen( type ) + 1 ) );
	strcpy( grubby.logtype, type );

	/* Initilize the log file */
	if( !strcasecmp( grubby.logtype, "HTML" ) )
	{
		fprintf( grubby.logfile, "<html>\n<title>GGZ Gaming Zone Log File</title>\n<body bgcolor=\"white\">\n" );
		fprintf( grubby.logfile, "<center>\n<table width=750>\n" );
		fprintf( grubby.logfile, "<tr><td colspan=2>Server:</td><td>%s</td></tr>\n", grubby.host );
		fprintf( grubby.logfile, "<tr><td colspan=2>Port:</td><td>%d</td></tr>\n", grubby.port );
		fprintf( grubby.logfile, "<tr><td colspan=3><p><hr></p></td></tr>\n" );
	}

	net_send_msg( "I've been instructed to log this room to file. This notice is to allow anyone not wishing to be logged to leave this room.");
}

void end_log( void )
{
	/* check if we are logging */
	if ( grubby.logfile == NULL )
	{
		net_send_prvmsg( grubby.owner, "I'm not cruuently logging anything." );
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

	net_send_prvmsg( grubby.owner, "I've stopped logging the chat." );
}

void log_write( char *left, char *right )
{
	if(grubby.logfile != NULL)
	{
		if( !strcasecmp( grubby.logtype, "HTML" ) )
		{
			fprintf( grubby.logfile, "<tr><td valign=top alighn=right>%s&nbsp</td><td>&nbsp</td>", left );
			fprintf( grubby.logfile, "<td valign=top>&nbsp %s</td></tr>\n", right );
		}
		else if( !strcasecmp( grubby.logtype, "TEXT" ) )
			fprintf( grubby.logfile, "%s | %s\n", left, right );
		fflush( grubby.logfile );
	}
}
