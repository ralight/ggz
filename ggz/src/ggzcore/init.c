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

int ggzcore_init(GGZOptionFlags options, const char* global, const char* local)
{
	if (options & GGZ_OPT_PARSER) {
		ggzcore_debug("Parsing system conf file: %s", global);
		ggzcore_debug("Parsing local conf file: %s", local);
	}
	
	/* Initialize event system */
	ggzcore_event_init();
	
	/* Register callbacks for UI functions */
	ggzcore_user_register();
	
	return 0;
}


void ggzcore_destroy(void)
{
	/* Clean up event system */
	ggzcore_event_destroy();
}
	


