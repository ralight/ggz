/*
 * File: ggz_common.c
 * Author: GGZ Dev Team
 * Project: GGZ Common Library
 * Date: 01/13/2002
 * $Id: ggz_common.c 5104 2002-10-29 11:29:45Z jdorje $
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
	case GGZ_CHAT_UNKNOWN:
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
		return GGZ_CHAT_UNKNOWN;

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

	return GGZ_CHAT_UNKNOWN;
}

const char *ggz_leavetype_to_string(GGZLeaveType type)
{
	switch (type) {
	case GGZ_LEAVE_NORMAL:
		return "normal";
	case GGZ_LEAVE_BOOT:
		return "boot";
	case GGZ_LEAVE_GAMEOVER:
		return "gameover";
	case GGZ_LEAVE_GAMEERROR:
		return "gameerror";
	}

	ggz_error_msg("ggz_leavetype_to_string: "
		      "invalid leavetype %d given.", type);
	return ""; /* ? */
}

GGZLeaveType ggz_string_to_leavetype(const char *type_str)
{
	if (!type_str)
		return GGZ_LEAVE_GAMEERROR;

	if (!strcasecmp(type_str, "normal"))
		return GGZ_LEAVE_NORMAL;
	else if (!strcasecmp(type_str, "boot"))
		return GGZ_LEAVE_BOOT;
	else if (!strcasecmp(type_str, "gameover"))
		return GGZ_LEAVE_GAMEOVER;
	else if (!strcasecmp(type_str, "gameerror"))
		return GGZ_LEAVE_GAMEERROR;

	return GGZ_LEAVE_GAMEERROR;
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

const char *ggz_error_to_string(GGZClientReqError err)
{
	switch (err) {
	case E_OK:
		return "ok";
	case E_USR_LOOKUP:
		return "usr lookup";
	case E_BAD_OPTIONS:
		return "bad options";
	case E_ROOM_FULL:
		return "room full";
	case E_TABLE_FULL:
		return "table full";
	case E_TABLE_EMPTY:
		return "table empty";
	case E_LAUNCH_FAIL:
		return "launch fail";
	case E_JOIN_FAIL:
		return "join fail";
	case E_NO_TABLE:
		return "no table";
	case E_LEAVE_FAIL:
		return "leave fail";
	case E_LEAVE_FORBIDDEN:
		return "leave forbidden";
	case E_ALREADY_LOGGED_IN:
		return "already logged_in";
	case E_NOT_LOGGED_IN
		: return "not logged in";
	case E_NOT_IN_ROOM:
		return "not in room";
	case E_AT_TABLE:
		return "at table";
	case E_IN_TRANSIT:
		return "in transit";
	case E_NO_PERMISSION:
		return "no permission";
	case E_BAD_XML:
		return "bad xml";
	case E_SEAT_ASSIGN_FAIL:
		return "seat assign fail";
	case E_NO_STATUS:
	case E_UNKNOWN:
		break;
	}

	ggz_error_msg("ggz_error_to_string: invalid error %d given.", err);
	return "[unknown]";
}

GGZClientReqError ggz_string_to_error(const char *str)
{
	if (!str)
		return E_OK;

	if (!strcasecmp(str, "ok"))
		return E_OK;
	if (!strcasecmp(str, "0")) {
		/* This provides a tiny bit of backwards compatability.
		   It should go away eventually. */
		return E_OK;
	}
	if (!strcasecmp(str, "usr lookup"))
		return E_USR_LOOKUP;
	if (!strcasecmp(str, "bad options"))
		return E_BAD_OPTIONS;
	if (!strcasecmp(str, "room full"))
		return E_ROOM_FULL;
	if (!strcasecmp(str, "table full"))
		return E_TABLE_FULL;
	if (!strcasecmp(str, "table empty"))
		return E_TABLE_EMPTY;
	if (!strcasecmp(str, "launch fail"))
		return E_LAUNCH_FAIL;
	if (!strcasecmp(str, "join fail"))
		return E_JOIN_FAIL;
	if (!strcasecmp(str, "no table"))
		return E_NO_TABLE;
	if (!strcasecmp(str, "leave fail"))
		return E_LEAVE_FAIL;
	if (!strcasecmp(str, "leave forbidden"))
		return E_LEAVE_FORBIDDEN;
	if (!strcasecmp(str, "already logged in"))
		return E_ALREADY_LOGGED_IN;
	if (!strcasecmp(str, "not logged in"))
		return E_NOT_LOGGED_IN;
	if (!strcasecmp(str, "not in room"))
		return E_NOT_IN_ROOM;
	if (!strcasecmp(str, "at table"))
		return E_AT_TABLE;
	if (!strcasecmp(str, "in transit"))
		return E_IN_TRANSIT;
	if (!strcasecmp(str, "no permission"))
		return E_NO_PERMISSION;
	if (!strcasecmp(str, "bad xml"))
		return E_BAD_XML;
	if (!strcasecmp(str, "seat assign fail"))
		return E_SEAT_ASSIGN_FAIL;

	return E_UNKNOWN;
}
