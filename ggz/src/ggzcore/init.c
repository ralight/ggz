/*
 * File: init.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/15/00
 *
 * Initialization code
 *
 * Copyright (C) 2000 Brent Hendricks.
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
#include <ggzcore.h>
#include <event.h>
#include <user.h>
#include <msg.h>
#include <net.h>
#include <state.h>
#include <server.h>



int ggzcore_init(GGZOptions options)
{
	if (options.flags & GGZ_OPT_PARSER) {
		ggzcore_debug(GGZ_DBG_CONF, "Parsing global conf file: %s", 
			      options.global_conf);
		ggzcore_debug(GGZ_DBG_CONF, "Parsing user conf file: %s", 
			      options.user_conf);
		ggzcore_debug(GGZ_DBG_CONF, "Parsing local conf file: %s", 
			      options.local_conf);
	}


	/* Initialize various systems */
	/* FIXME: Get filename and levels from conf file */
	_ggzcore_debug_init((GGZ_DBG_ALL & ~GGZ_DBG_POLL), "/tmp/ggz.debug");
	_ggzcore_event_init();

	/* State should have first callbacks registered(?) */
	_ggzcore_state_init();
	_ggzcore_net_init();

	/* Register internal callbacks for events */
	_ggzcore_user_register();
	_ggzcore_server_register();
	
	return 0;
}


void ggzcore_destroy(void)
{
	/* Clean up event system */
	_ggzcore_event_destroy();

	_ggzcore_debug_cleanup();
}
	


