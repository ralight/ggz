/* init.c
 *
 * A GRUB ia A (G)GZ (R)eally (U)seful (B)ot
 *
 * Original: Rich Gade
 * ReWrite:  Justin Zaun
 *
 * May 23rd, 2000
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ggzcore.h>
#include "datatypes.h"
#include "protocols.h"
#include "net.h"

struct Grubby grubby;

void init_chatter( void );
void shutdown( int sig );

void grubby_init( void )
{
	/* Trap ^C and family */
	signal( SIGINT, shutdown );
	signal( SIGQUIT, shutdown );
	signal( SIGTERM, shutdown );

	/* Display nice little message		*/
	printf ("AGRUB Version 0.1.0\n-----\n");

	/* Initilize grubbies information	*/
	printf ("  Getting Options\n");
	grubby.port   = -1;  
	grubby.host   = NULL;
	grubby.name   = NULL;
	grubby.owner  = NULL;
}


void ggz_init( void )
{
	GGZOptions opt;
	char *global_conf, *user_conf;

	/* Initilize GGZCORE */
	opt.flags = GGZ_OPT_PARSER;
	global_conf = NULL;
	user_conf = malloc((strlen(getenv("HOME"))+15)*sizeof(char*));
	sprintf(user_conf, "%s/.ggz/grubby.rc", getenv("HOME"));
	ggzcore_conf_initialize(global_conf, user_conf);
	free(user_conf);
        opt.debug_file = ggzcore_conf_read_string("Debug", "File", "/tmp/ggz-grubby.debug");
        opt.debug_levels = (GGZ_DBG_ALL & ~GGZ_DBG_POLL);
	ggzcore_init(opt);

	/* Connect Callbacks */
	net_register_callbacks();
}


void shutdown( int sig )
{
	/* Shitty and fast exit */
	ggzcore_conf_commit();
	ggzcore_event_process_all();
	ggzcore_destroy();
	exit(1);
}
