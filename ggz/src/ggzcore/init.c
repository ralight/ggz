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

#include "config.h"
#include "conf.h"
#include "confio.h"
#include "ggzcore.h"
#include "msg.h"
#include "net.h"
#include "state.h"
#include "gametype.h"
#include "memory.h"


int ggzcore_init(GGZOptions options)
{
	/*
	if (options.flags & GGZ_OPT_PARSER) {
		ggzcore_debug(GGZ_DBG_CONF, "Parsing global conf file: %s", 
			      options.global_conf);
		ggzcore_debug(GGZ_DBG_CONF, "Parsing user conf file: %s", 
			      options.user_conf);
		ggzcore_conf_initialize(options.global_conf, options.user_conf);
		}*/
	


	/* Initialize various systems */
	_ggzcore_debug_init(options.debug_levels, options.debug_file);
	_ggzcore_net_init();
/*     	_ggzcore_module_init();*/

	/* Setup the gametype linked list */
	return 0;
}


void ggzcore_destroy(void)
{
/*	_ggzcore_module_cleanup();*/
	_ggzcore_confio_cleanup();
	_ggzcore_memory_check();
	_ggzcore_debug_cleanup();
}
	


