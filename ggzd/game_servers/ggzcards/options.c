/* 
 * File: options.c
 * Author: Jason Short
 * Project: GGZCards Server
 * Date: 07/06/2001
 * Desc: Functions and data for game options system
 * $Id: options.c 2726 2001-11-13 00:05:44Z jdorje $
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
 * Copyright (C) 2001 Brent Hendricks.
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

#include <config.h>		/* Site-specific config */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <easysock.h>

#include "common.h"

static int options_initted = 0;

static struct option_t {
	char *key;
	int value;
	struct option_t *next;	/* linked list */
} *optionlist = NULL;

static struct pending_option_t {
	char *key;
	/* char* text; */
	int num;
	int dflt;
	char **choices;
	struct pending_option_t *next;	/* linked list */
} *pending_options = NULL;

static int option_count = 0;
static int pending_option_count = 0;

int options_set()
{
	return options_initted;
}

void set_option(char *key, int value)
{
	struct option_t *option = ggz_malloc(sizeof(*option));
	option->key = key;
	option->value = value;
	option->next = optionlist;
	optionlist = option;
}

void add_option(char *key, int num, int dflt, ...)
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
	po->key = key;
	po->num = num;
	po->dflt = dflt;
	po->choices = ggz_malloc(num * sizeof(char *));

	va_start(ap, dflt);
	for (i = 0; i < num; i++) {
		po->choices[i] = va_arg(ap, char *);
		if (po->choices[i] == NULL)
			ggzd_debug("ERROR: SERVER BUG: "
				   "add_option: NULL option choice.");
	}
	va_end(ap);

	po->next = pending_options;
	pending_option_count++;
	pending_options = po;
}

void get_options()
{
	int fd = game.host >= 0 ? ggzd_get_player_socket(game.host) : -1;
	int op, choice;

	ggzd_debug("Entering get_options.");

	game.funcs->get_options();

	if (pending_options == NULL) {
		options_initted = 1;
		ggzd_debug("get_options: no options to get.");
	} else if (fd == -1) {
		ggzd_debug("ERROR: SERVER BUG: " "no connection to host.");
	} else {
		struct pending_option_t *po = pending_options;
		write_opcode(fd, WH_REQ_OPTIONS);
		es_write_int(fd, pending_option_count);
		for (op = 0; op < pending_option_count; op++) {
			es_write_int(fd, po->num);
			es_write_int(fd, po->dflt);
			for (choice = 0; choice < po->num; choice++)
				es_write_string(fd, po->choices[choice]);
			po = po->next;
		}
	}
}

int rec_options(int num_options, int *options)
{
	int fd = game.host >= 0 ? ggzd_get_player_socket(game.host) : -1,
		status = 0, i;
	if (fd == -1) {
		ggzd_debug("SERVER bug: unknown host in rec_options.");
		exit(-1);
	}

	for (i = 0; i < num_options; i++)
		if (es_read_int(fd, &options[i]) < 0)
			status = options[i] = -1;

	if (status != 0)
		ggzd_debug("ERROR: rec_options: status is %d.", status);
	return status;
}

void handle_options()
{
	int options[pending_option_count], op;
	struct pending_option_t *po = pending_options;

	ggzd_debug("Entering handle_options.");
	rec_options(pending_option_count, options);

	for (op = 0; op < pending_option_count; op++) {
		set_option(po->key, options[op]);

		pending_options = po->next;
		ggz_free(po);
		po = pending_options;

		option_count++;
	}

	options_initted = 1;
}

void finalize_options()
{
	struct option_t *op;
	char *optext;
	char opbuf[128];
	char buf[4096] = "Options:\n";
	int len = strlen(buf), opcount = 0;

	for (op = optionlist; op != NULL; op = op->next) {
		game.funcs->handle_option(op->key, op->value);
		optext = game.funcs->get_option_text(opbuf, sizeof(opbuf),
						     op->key, op->value);
		if (optext == NULL) {
			ggzd_debug("ERROR: SERVER BUG: "
				   "finalize_options: NULL optext returned for option (%s, %d).",
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
