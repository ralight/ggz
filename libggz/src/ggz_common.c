/*
 * File: ggz_common.c
 * Author: GGZ Dev Team
 * Project: GGZ Common Library
 * Date: 01/13/2002
 * $Id: ggz_common.c 4855 2002-10-10 20:54:03Z jdorje $
 *
 * This provides GGZ-specific functionality that is common to
 * some or all of the ggz-server, game-server, ggz-client, and
 * game-client.
 *
 * Copyright (C) 2002 Brent Hendricks.
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

#include <string.h>

#include "ggz.h"
#include "ggz_common.h"

const char *ggz_seattype_to_string(GGZSeatType type)
{
	switch (type) {
	case GGZ_SEAT_OPEN:
		return "open";
	case GGZ_SEAT_BOT:
		return "bot";
	case GGZ_SEAT_RESERVED:
		return "reserved";
	case GGZ_SEAT_PLAYER:
		return "player";
	case GGZ_SEAT_NONE:
		return "none";
	}

	ggz_error_msg("ggz_seattype_to_string: "
		      "invalid seattype %d given.", type);
	return "none";
}

GGZSeatType ggz_string_to_seattype(const char *type_str)
{
	/* If it doesn't match _exactly_ we return GGZ_SEAT_NONE.  This
	   is bad for, say, user input, but perfectly acceptable as an
	   inverse to ggz_seattype_to_string(). */
	if (!type_str)
		return GGZ_SEAT_NONE;

	if (!strcasecmp(type_str, "open"))
		return GGZ_SEAT_OPEN;
	else if (!strcasecmp(type_str, "bot"))
		return GGZ_SEAT_BOT;
	else if (!strcasecmp(type_str, "reserved"))
		return GGZ_SEAT_RESERVED;
	else if (!strcasecmp(type_str, "player"))
		return GGZ_SEAT_PLAYER;
	else
		return GGZ_SEAT_NONE;
}

const char *ggz_chattype_to_string(GGZChatType type)
{
	switch (type) {
	case GGZ_CHAT_NORMAL:
		return "normal";
	case GGZ_CHAT_ANNOUNCE:
		return "announce";
	case GGZ_CHAT_BEEP:
		return "beep";
	case GGZ_CHAT_PERSONAL:
		return "private";
	case GGZ_CHAT_TABLE:
		return "table";
	case GGZ_CHAT_NONE:
		break;
	}

	ggz_error_msg("ggz_chattype_to_string: "
		      "invalid chattype %d given.", type);
	return ""; /* ? */
}

GGZChatType ggz_string_to_chattype(const char *type_str)
{
	/* If it doesn't match _exactly_ we return GGZ_CHAT_NONE.  This
	   is bad for, say, user input, but perfectly acceptable as an
	   inverse to ggz_chattype_to_string(). */
	if (!type_str)
		return GGZ_CHAT_NONE;

	if (!strcasecmp(type_str, "normal"))
		return GGZ_CHAT_NORMAL;
	else if (!strcasecmp(type_str, "announce"))
		return GGZ_CHAT_ANNOUNCE;
	else if (!strcasecmp(type_str, "beep"))
		return GGZ_CHAT_BEEP;
	else if (!strcasecmp(type_str, "private"))
		return GGZ_CHAT_PERSONAL;
	else if (!strcasecmp(type_str, "table"))
		return GGZ_CHAT_TABLE;
	else
		return GGZ_CHAT_NONE;

}

char *bool_to_str(int bool_val)
{
	if (bool_val)
		return "true";
	else
		return "false";
}

/* Convert a possibly-null string that should contain "true" or "false"
   to a boolean (int) value.  The default value is returned if an invalid
   or empty value is sent. */
int str_to_bool(const char *str, int dflt)
{
	if (!str)
		return dflt;
  
	if (strcasecmp(str, "true") == 0)
		return 1;

	if (strcasecmp(str, "false") == 0)
		return 0;

	return dflt;
}
