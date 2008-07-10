/*
 * File: client.h
 * Author: Justin Zaun
 * Project: GGZ GTK Client
 * $Id: client.h 10274 2008-07-10 21:38:34Z jdorje $
 *
 * This is the main program body for the GGZ client
 *
 * Copyright (C) 2000 Justin Zaun.
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

#include <ggzcore.h>

#include "support.h"

#ifndef __CLIENT_H__
#define __CLIENT_H__

void INTERNAL client_join_table(void);

void INTERNAL client_start_table_join(void);

GtkWidget *create_win_main(void);

/* Default font */
#define DEFAULT_FONT "-*-fixed-medium-r-semicondensed--*-120-*-*-c-*-iso8859-1"

enum ggz_page {
	GGZ_PAGE_FIRSTLOGIN,
	GGZ_PAGE_LOGIN,
	GGZ_PAGE_MAIN,
	GGZ_PAGE_PROPS
};

void INTERNAL main_activate(void);

struct ggz_gtk {
	GtkWidget *main_window;
	GtkWidget *win_main; /* FIXME: not actually a window anymore. */
	GtkWidget *notebook;

	/* Callback functions registered by the
	   user of the ggz-gtk library */
	void (*connected_cb)(GGZServer *server);
	void (*launched_cb)(void);
	void (*ggz_closed_cb)(void);

	const char *embedded_protocol_engine;
	const char *embedded_protocol_version;
	const char *embedded_default_profile;

	GGZServer *server;
	gboolean spectating;

	/* glib socket monitoring tags */
	guint server_tag, channel_tag, game_tag;

	struct ggz_chat *chat;

	/* launch code */
	GtkWidget *launch_dialog;
	gboolean launching;
};

extern struct ggz_gtk ggz_gtk;

#endif /* __CLIENT_H__ */
