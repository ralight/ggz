/* popt.c
 *
 * A GRUB ia A (G)GZ (R)eally (U)seful (B)ot
 *
 * Original: Rich Gade
 * ReWrite:  Justin Zaun
 *
 * May 23rd, 2000
 */

#include <popt.h>
#include <stdio.h>
#include <ggzcore.h>
#include "datatypes.h"
#include "agrub_popt.h"

extern struct Grubby grubby;



struct poptOption args[] = {
	{"port",  'p', POPT_ARG_INT,    &grubby.port,	0, "Port to connect to", "PORT"},
	{"host",  'h', POPT_ARG_STRING, &grubby.host,	0, "Host to connect to", "HOST"},
	{"name",  'n', POPT_ARG_STRING, &grubby.name, 	0, "Username for GGZ",   "NAME"},        
	{"owner", 'o', POPT_ARG_STRING, &grubby.owner,	0, "Username for owner", "OWNER"},
	POPT_AUTOHELP {NULL, '\0', 0, NULL, 0}
};


void popt_get(const int argc, const char *argv[])
{
	poptContext	context;
	int		rc;

	context = poptGetContext(NULL, argc, argv, args, 0);
	while((rc = poptGetNextOpt(context)) != -1) {
		switch(rc) {
			case POPT_ERROR_NOARG:
			case POPT_ERROR_BADOPT:
			case POPT_ERROR_BADNUMBER:
			case POPT_ERROR_OVERFLOW:
				printf("%s: %s\n",
					poptBadOption(context, 0),
					poptStrerror(rc));
					poptFreeContext(context);   
				exit(1);
				break;
		}
	}
	poptFreeContext(context);

	if(grubby.port == -1)
		grubby.port = ggzcore_conf_read_int("GRUBBYSETTINGS","PORT", 5688);
	if(grubby.host == NULL)
                grubby.host = ggzcore_conf_read_string("GRUBBYSETTINGS","HOST","localhost");
	if(grubby.name == NULL)
		grubby.name = ggzcore_conf_read_string("GRUBBYSETTINGS","NAME","Grubby");
	if(grubby.owner == NULL)
		grubby.owner = ggzcore_conf_read_string("GRUBBYSETTINGS","OWNER","(none)");
	
}
