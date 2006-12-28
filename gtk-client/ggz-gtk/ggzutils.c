/*
 * File: ggzutils.c
 * Author: GGZ Dev Team
 * Project: GGZ GTK Client
 * Date: 12/28/2006
 * $Id: playerlist.c 8763 2006-12-27 10:02:33Z jdorje $
 * 
 * GGZ-specific utility functions
 * 
 * Copyright (C) 2006 GGZ Development Team
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
#  include <config.h>
#endif

#include "ggzutils.h"
#include "support.h"

/* Return a text name for the permission, or NULL. */
char *perm_name(GGZPerm p)
{
	switch (p) {
	case GGZ_PERM_JOIN_TABLE:
		return _("Can join tables");
	case GGZ_PERM_ROOMS_LOGIN:
		return _("Can join rooms");
	case GGZ_PERM_LAUNCH_TABLE:
		return _("Can launch tables");
	case GGZ_PERM_ROOMS_ADMIN:
		return _("Room administrator");
	case GGZ_PERM_CHAT_ANNOUNCE:
		return _("Can make server announcements");
	case GGZ_PERM_CHAT_BOT:
		return _("Is a bot");
	case GGZ_PERM_NO_STATS:
		return _("No stats for this player");
	case GGZ_PERM_EDIT_TABLES:
		return _("Can admin tables");
	case GGZ_PERM_TABLE_PRIVMSG:
		return _("Can send private messages at a table");
	case GGZ_PERM_COUNT:
		break;
	}

	return NULL;
}
