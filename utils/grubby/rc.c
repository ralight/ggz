/* rc.c
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
#include "datatypes.h"

extern struct Grubby grubby;
extern struct Memory memory;

void save_memory( void )
{
	int i, x;
	char *home, file[1024];
	FILE *rc;

	/* Open the rc file */
	home = getenv( "HOME" );
	strcpy( file, home );
	strcat( file, "/.agrub" );
	rc = fopen( file, "w" );
	if( rc == NULL )
		return;

	/* Write memory out to file */
	fprintf( rc, "AGRUB\n" );

	/* Save People */
	fprintf( rc, "%d\n", memory.num_people );
	for( i=0; i<memory.num_people; i++ )
	{
		fprintf( rc, "%s\n", memory.people[i].username );
		fprintf( rc, "%s\n", memory.people[i].realname );
		fprintf( rc, "%s\n", memory.people[i].email );
		fprintf( rc, "%s\n", memory.people[i].url );
		fprintf( rc, "%s\n", memory.people[i].info );
		fprintf( rc, "%d\n", memory.people[i].seen );
		fprintf( rc, "%d\n", memory.people[i].lastseen );
		fprintf( rc, "%s\n", memory.people[i].lastroom );

		/* Save messages to person */
		fprintf( rc, "%d\n", memory.people[i].msgcount );
		for( x=0; x<memory.people[i].msgcount; x++ )
		{
			fprintf( rc, "%s\n", memory.people[i].msg[x].from );
			fprintf( rc, "%s\n", memory.people[i].msg[x].text );
			fprintf( rc, "%d\n", memory.people[i].msg[x].timestamp );
		}
	}

	fclose( rc );
}

void read_memory( void )
{
	int i, x;
	char *home, file[1024];
	char temp[1024];
	FILE *rc;

	/* Open the rc file */
	home = getenv( "HOME" );
	strcpy( file, home );
	strcat( file, "/.agrub" );
	rc = fopen( file, "r" );
	if( rc == NULL )
		return;

	/* Read memory in from file */
	fscanf( rc, "AGRUB\n" );

	/* Read People */
	fscanf( rc, "%d\n", &memory.num_people );
	for( i=0; i<memory.num_people; i++ )
	{
		fgets(temp, 1024, rc);
		*(strrchr( temp, '\n' ) ) = 0;
		memory.people[i].username = malloc( sizeof( char ) * strlen( temp ) );
		strcpy( memory.people[i].username, temp );

		fgets(temp, 1024, rc);
		*(strrchr( temp, '\n' ) ) = 0;
		memory.people[i].realname = malloc( sizeof( char ) * strlen( temp ) );
		strcpy( memory.people[i].realname, temp );

		fgets(temp, 1024, rc);
		*(strrchr( temp, '\n' ) ) = 0;
		memory.people[i].email = malloc( sizeof( char ) * strlen( temp ) );
		strcpy( memory.people[i].email, temp );

		fgets(temp, 1024, rc);
		*(strrchr( temp, '\n' ) ) = 0;
		memory.people[i].url = malloc( sizeof( char ) * strlen( temp ) );
		strcpy( memory.people[i].url, temp );

		fgets(temp, 1024, rc);
		*(strrchr( temp, '\n' ) ) = 0;
		memory.people[i].info = malloc( sizeof( char ) * strlen( temp ) );
		strcpy( memory.people[i].info, temp );

		fscanf( rc, "%d\n", &memory.people[i].seen );
		fscanf( rc, "%d\n", &memory.people[i].lastseen );

		fgets(temp, 1024, rc);
		*(strrchr( temp, '\n' ) ) = 0;
		memory.people[i].lastroom = malloc( sizeof( char ) * strlen( temp ) );
		strcpy( memory.people[i].lastroom, temp );

		/* Save messages to person */
		fscanf( rc, "%d\n", &memory.people[i].msgcount );
		for( x=0; x<memory.people[i].msgcount; x++ )
		{
			fscanf( rc, "%s\n", memory.people[i].msg[x].from );

			fgets(temp, 1024, rc);
			*(strrchr( temp, '\n' ) ) = 0;
			memory.people[i].msg[x].text = malloc( sizeof( char ) * strlen( temp ) );
			strcpy( memory.people[i].msg[x].text, temp );

			fscanf( rc, "%d\n", &memory.people[i].msg[x].timestamp );
		}
	}

	fclose( rc );
}

