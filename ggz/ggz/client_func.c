/*
 * File: client_func.c
 * Author: Brent Hendricks
 * Project: GGZ Client
 * Date: 7/31/97
 *
 * This file contains the support functions which do the dirty work of
 * playing spades.  This file is an attempt to remain modular so that
 * it can be modified without affecting play, or having to greatly
 * modify the client_main.cc file which contains the general playing
 * algorithm
 *
 * Copyright (C) 1998 Brent Hendricks.
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


#include <config.h>		/* Site config data */

#include <unistd.h>		/* For getopt et. al */
#include <stdio.h>		/* For fprintf */
#include <stdlib.h>		/* For exit, atoi */
#include <signal.h>		/* For signal */
#include <sys/wait.h>

#ifdef DEBUG_MEM
# include <dmalloc.h>
#endif

#include <string.h>		/* For strcpy */

#include "client.h"
#include "easysock.h"
#include "dlg_error.h"
#include "datatypes.h"

#define basename(path) (strrchr(path,'/')==NULL) ? path : strrchr(path, '/')+1

/* Global state of game variable */
extern struct ConnectInfo connection;


int CheckReadInt(int msgsock, int *message)
{
	int status, success = NET_OK;

	status = es_read_int(msgsock, message);

	if (status < 0) {
		DisplayError("Network error receiving int");
		NetClose();
		success = NET_FAIL;
	} else if (status == 0) {
		DisplayError("Socket closed while reading int");
		NetClose();
		success = NET_FAIL;
	}

	return success;
}


int CheckReadString(int msgsock, char **message)
{
	int status, success = NET_OK;

	status = es_read_string_alloc(msgsock, message);

	if (status < 0) {
		DisplayError("Network error receiving string");
		NetClose();
		success = NET_FAIL;
	} else if (status == 0) {
		DisplayError("Socket closed while reading string");
		NetClose();
		success = NET_FAIL;
	}

	return success;
}


int CheckWriteInt(int msgsock, int message)
{
	int status, success = NET_OK;

	status = es_write_int(msgsock, message);

	if (status <= 0) {
		DisplayError("Network error sending int");
		NetClose();
		success = NET_FAIL;
	}

	return success;
}


int CheckWriteString(int msgsock, char *message)
{
	int status, success = NET_OK;

	status = es_write_string(msgsock, message);

	if (status <= 0) {
		DisplayError("Network error sending string");
		NetClose();
		success = NET_FAIL;
	}

	return success;
}


void NetClose(void)
{
	close(connection.sock);
}


RETSIGTYPE die(int sig)
{
	/* We let our error checker handle broken pipes */
	if (sig != SIGPIPE) {
		NetClose();
		signal(sig, SIG_DFL);	/* Reset old signal handler */
		raise(sig);	/* Re-send signal so other cleanup gets done */
	}
}
