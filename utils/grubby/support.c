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

extern struct Memory memory;

int check_known( char *name )
{
	int i;

	/* get the users number */
	for(i=0; i<memory.num_people; i++)
		if( !strcmp(memory.people[i].username, name))
			break;

	/* Check if we know the user */
	if( i >= memory.num_people )
		return(-1);
	else
		return(i);
}

char *get_name( char *name )
{
	int i;

	i = check_known( name );
	if( i == -1 )
		return( name );

	if( memory.people[i].realname )
		return( memory.people[i].realname );
	else
		return( name );
}
