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

#include <ggzcore.h>
#include "datatypes.h"
#include "support.h"


int support_check_known( char *name )
{
	if(ggzcore_conf_read_int(name, "LASTSEEN", -1) != -1)
		return 0;
	return -1;	
}

char *support_get_name( char *name )
{
	int i;

	i = support_check_known( name );
	if( i == -1 )
		return( name );

	if(!strcmp(ggzcore_conf_read_string(name, "REALNAME", "(null)"),"(null)"))
		return(name);
	return(ggzcore_conf_read_string(name, "REALNAME", "(null)"));
}
