/* support.c
 *
 * A GRUB ia A (G)GZ (R)eally (U)seful (B)ot
 *
 * Original: Rich Gade
 * ReWrite:  Justin Zaun
 *
 * May 23rd, 2000
 */

#include <stdio.h>
#include "datatypes.h"
#include "support.h"

extern struct Memmory memmory;

int check_known( char *name )
{
	int i;

	/* get the users numer */
	for(i=0; i<memmory.num_people; i++)
		if( !strcmp(memmory.people[i].username, name))
			break;

	/* Check if we know the user */
	if( i >= memmory.num_people )
		return(-1);
	else
		return(i);
}

char *get_name( char *name )
{
	int i;

	i = check_known( name );
	printf("%d\n",i);
	if( i == -1 )
		return( name );
	printf("%s\n",memmory.people[i].realname);
	if( memmory.people[i].realname )
		return( memmory.people[i].realname );
	else
		return( name );
}
