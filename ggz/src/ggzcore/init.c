/*
 * File: init.c
 * Author: Brent Hendricks
 * Project: GGZ Core Client Lib
 * Date: 9/15/00
 * $Id: init.c 6879 2005-01-24 07:28:38Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>	/* Site-specific config */
#endif

#include <ggz.h>

#include "conf.h"
#include "gametype.h"
#include "ggzcore.h"
#include "memory.h"
#include "module.h"
#include "net.h"
#include "state.h"

int ggzcore_init(GGZOptions options)
{
#if 0
	if (options.flags & GGZ_OPT_PARSER) {
		ggz_debug(GGZCORE_DBG_CONF, "Parsing global conf file: %s",
			  options.global_conf);
		ggz_debug(GGZCORE_DBG_CONF, "Parsing user conf file: %s",
			  options.user_conf);
		ggzcore_conf_initialize(options.global_conf,
					options.user_conf);
	}
#endif


	/* Initialize various systems */
	if (options.flags & GGZ_OPT_MODULES)
		_ggzcore_module_setup();

	if (options.flags & GGZ_OPT_EMBEDDED)
		_ggzcore_module_set_embedded();

	return 0;
}


void ggzcore_reload(void)
{
	_ggzcore_module_cleanup();
	_ggzcore_module_setup();
}


void ggzcore_destroy(void)
{
	_ggzcore_module_cleanup();
	ggz_conf_cleanup();
}
