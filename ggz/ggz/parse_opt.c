/*
 * File: parse_opt.c
 * Author: Brent Hendricks
 * Project: GGZ Client
 * Date: 10/15/99
 * Desc: Parse command-line arguments and conf file
 *
 * Copyright (C) 1999 Brent Hendricks.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <config.h>

#include <popt.h>
#include <stdio.h>
#include <gtk/gtk.h>

#include "datatypes.h"

extern struct ConnectInfo connection;
extern char *local_conf_fname;

static const struct poptOption args[] = {
	
	{"server", 's', POPT_ARG_STRING, &connection.server, 0,
	 "GGZ server name", "SERVER"},

	{"port", 'p', POPT_ARG_INT, &connection.port, 0, "GGZ server port",
	 "PORT"},
	
	{"name", 'n', POPT_ARG_STRING, &connection.username, 0,
	 "Player name", "PORT"},

	{"file", 'f', POPT_ARG_STRING, &local_conf_fname, 0,
	 "Test configuration file", "FILENAME" },
	
	{"version", 'V', POPT_ARG_NONE, NULL, 1},
	
	POPT_AUTOHELP {NULL, '\0', 0, NULL, 0}	/* end the list */
};

/* Parse command-line options */
void parse_args(gint argc, gchar *argv[])
{
	gint rc;
	poptContext context = poptGetContext(NULL, argc, argv, args, 0);

	while ((rc = poptGetNextOpt(context)) != -1) {
		switch (rc) {
		case 1:	/* Version command */
			g_print("Gnu Gaming Zone client: version %s\n",
				VERSION);
			poptFreeContext(context);
			exit(0);
		case POPT_ERROR_NOARG:
		case POPT_ERROR_BADOPT:
		case POPT_ERROR_BADNUMBER:
		case POPT_ERROR_OVERFLOW:
			g_printerr("%s: %s", poptBadOption(context, 0),
				   poptStrerror(rc));
			poptFreeContext(context);
			exit(-1);
			break;
		}
	}
	
	poptFreeContext(context);
}


/* Parse options from conf file, but don't overwrite existing options*/
void parse_conf_file(void)
{
/*	
	if( opt.local_conf ) {
	dbg_msg("Reading local conf file : %s", opt.local_conf);
	}
	else {
	dbg_msg("Reading global conf file : %s/netgamed.conf", SYSCONFDIR );
	}
*/
}
