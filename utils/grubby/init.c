/* init.c
 *
 * A GRUB ia A (G)GZ (R)eally (U)seful (B)ot
 *
 * Original: Rich Gade
 * ReWrite:  Justin Zaun
 *
 * May 23rd, 2000
 */

#include <easysock.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "datatypes.h"
#include "protocols.h"

struct Grubby grubby;
struct Memory memory;
int sig;

void init_chatter( void );

void grubby_init( void )
{
	int	i;

	/* Display nice little message		*/
	printf ("AGRUB Version 0.1.0\n-----\n");

	/* Initilize grubbies information	*/
	printf ("  |- Getting Options\n");
	grubby.port   = -1;  
	grubby.host   = NULL;
	grubby.name   = NULL;
	grubby.owner  = NULL;
	grubby.lang_check = 0;

	printf ("  |- Init Memory\n");
	/* Initilize grubbies memory		*/
	memory.num_people = 0;
	for(i=0; i<MAX_PEOPLE; i++)
		memory.people[i].username
		  = memory.people[i].realname
		  = memory.people[i].email
		  = memory.people[i].url
		  = memory.people[i].info
		  = memory.people[i].lastroom
		  = NULL;

	init_chatter();

	printf ("  |- Set state to NORMAL\n");
	sig = SIG_NORMAL;
}


void ggz_init( void )
{
	/* Connect up to the GGZ server	*/

	printf ("  |- Connecting to Server\n");
	grubby.socket = es_make_socket(ES_CLIENT, grubby.port, grubby.host);
	if (grubby.socket < 0)
	{
		exit(1);
	} else {
		printf ("  |- Connected to %s:%d\n", grubby.host, grubby.port);		
	}
}

void init_chatter( void )
{
	memory.num_chatter = MAX_CHATTER;
	memory.chatter[0].text = "\"The further I get from the things that I care about, the less I care about how much further away I get.\" -Robert Smith, \"Fear of Ghosts\"";
	memory.chatter[0].type = GGZ_CHAT_NORMAL;
	memory.chatter[1].text = "It's all fun and games until someone gets hurt...then it's just fun.";
	memory.chatter[1].type = GGZ_CHAT_NORMAL;
	memory.chatter[2].text = "Patience is a virtue, afforded those with nothing better to do.";
	memory.chatter[2].type = GGZ_CHAT_NORMAL;
	memory.chatter[3].text = "A computer lets you make more mistakes faster than any invention in human history - with the possible exceptions of handguns and tequila.";
	memory.chatter[3].type = GGZ_CHAT_NORMAL;
	memory.chatter[4].text = "is having one heck of a time looking at all theis games at once!";
	memory.chatter[4].type = GGZ_CHAT_PERSONAL;
}
