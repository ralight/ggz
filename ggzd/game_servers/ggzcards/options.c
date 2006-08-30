/* 
 * File: options.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/06/2001
 * Desc: Functions and data for game options system
 * $Id: options.c 8552 2006-08-30 06:03:56Z jdorje $
 *
 * GGZCards has a rather nifty option system.  Each option has a name as
 * its "key".  Each option has a certain number of possible values, in
 * the integer range 0..(n-1).  Each value is linked to a text description.
 * All of these are created by the individual games using the interface
 * functions in this file.
 *
 * When the game starts, the server automatically asks the client (host
 * player) what options (s)he wants.  Options can also be passed in
 * through the command line to the server, and are handled at game start
 * time.
 *
 * Copyright (C) 2001-2002 Brent Hendricks.
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-130
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ggz.h>

#include "net_common.h"

#include "common.h"
#include "message.h"
#include "net.h"
#include "options.h"
static struct option_t {
	char *key;
	int value;
	struct option_t *next;	/* linked list */
} *optionlist = NULL;

static struct pending_option_t {
	char *type;
	char *key;
	char *desc;
	int num;
	int dflt;
	char **choices;
	struct pending_option_t *next;	/* linked list */
} *pending_options = NULL;

static int option_count = 0;
static int pending_option_count = 0;


/** Handles a correct options response from the host client.
 *  Should be called when an option message is received in response
 *  to a get_options() option request.
 *  @note Only called by common code.
 *  @todo Perhaps should be called handle_options_event? */
static void handle_options(int *options);

bool are_options_set(void)
{
	return pending_option_count == 0;
}

void set_option(char *key, int value)
{
	struct option_t *option = ggz_malloc(sizeof(*option));
	option->key = key;
	option->value = value;
	option->next = optionlist;
	optionlist = option;
}

void add_option(char *type, char *key, char *desc, int num, int dflt, ...)
{
	va_list ap;
	int i;
	struct pending_option_t *po;
	struct option_t *op;

	/* ignore any option that's already been set */
	for (op = optionlist; op != NULL; op = op->next)
		if (!strcmp(op->key, key))
			return;
	for (po = pending_options; po != NULL; po = po->next)
		if (!strcmp(po->key, key))
			return;

	po = ggz_malloc(sizeof(*po));
	po->type = type;
	po->key = key;
	po->desc = desc;
	po->num = num;
	po->dflt = dflt;
	po->choices = ggz_malloc(num * sizeof(char *));

	va_start(ap, dflt);
	for (i = 0; i < num; i++) {
		po->choices[i] = va_arg(ap, char *);
		if (po->choices[i] == NULL)
			ggz_error_msg("add_option: NULL option choice.");
	}
	va_end(ap);

	po->next = pending_options;
	pending_option_count++;
	pending_options = po;
}

void request_client_options(void)
{
	ggz_debug(DBG_MISC, "Entering get_options.");

	if (pending_options == NULL) {
		assert(FALSE);
		try_to_start_game();
	} else {
		struct pending_option_t *po = pending_options;
		char *option_types[pending_option_count];
		char *option_descs[pending_option_count];
		int num_choices[pending_option_count];
		int option_defaults[pending_option_count];
		char** option_choices[pending_option_count];
		int op;
		
		for (op = 0; op < pending_option_count; op++) {
			option_types[op] = po->type;
			option_descs[op] = po->desc;
			num_choices[op] = po->num;
			option_defaults[op] = po->dflt;
			option_choices[op] = po->choices;
			
			po = po->next;
		}
		
		net_send_options_request(game.host,
		                         pending_option_count,
					 option_types,
		                         option_descs,
		                         num_choices,
		                         option_defaults,
		                         option_choices);
	}
}


int get_num_pending_options(void)
{
	return pending_option_count;
}


void handle_client_options(player_t p, int num_options, int *options)
{
	assert(num_options > 0); /* Should be assured by caller */

	if (p != game.host) {
		/* how could this happen? */
		ggz_debug(DBG_CLIENT, "received options from non-host player.");
		return;
	}

	if (game.data == NULL) {
		games_handle_gametype(options[0]);

		init_game();
		broadcast_sync();

		next_move();
	} else {
		if (num_options != pending_option_count)
			return;
		handle_options(options);
	}
}

static void handle_options(int *options)
{
	int op;
	struct pending_option_t *po = pending_options;

	ggz_debug(DBG_MISC, "Entering handle_options.");

	for (op = 0; op < pending_option_count; op++) {
		set_option(po->key, options[op]);

		pending_options = po->next;
		ggz_free(po);
		po = pending_options;

		option_count++;
	}
	pending_option_count = 0;
	
	(void) try_to_start_game();
}

void finalize_options(void)
{
	struct option_t *op;
	char *optext;
	char opbuf[128];
	char buf[4096] = "Options:\n";
	int len = strlen(buf), opcount = 0;

	for (op = optionlist; op != NULL; op = op->next) {
		game.data->handle_option(op->key, op->value);
		optext = game.data->get_option_text(opbuf, sizeof(opbuf),
						     op->key, op->value);
		if (optext == NULL) {
			ggz_error_msg("finalize_options: NULL optext "
			              "returned for option (%s, %d).",
				      op->key, op->value);
			len += snprintf(buf + len, sizeof(buf) - len,
					"  %s : %d\n", op->key, op->value);
			opcount++;
			continue;
		}
		if (!*optext)
			continue;
		len += snprintf(buf + len, sizeof(buf) - len, "  %s\n",
				optext);
		opcount++;
	}
	if (!opcount)
		/* there's absolutely no reason for this to be a server
		   string! */
		snprintf(buf + len, sizeof(buf) - len,
			 "  No special options have been selected.\n");
	set_global_message("Options", "%s", buf);
}
