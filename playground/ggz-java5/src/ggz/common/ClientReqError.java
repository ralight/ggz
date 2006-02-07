/*
 * Copyright (C) 2006  Helg Bredow
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
package ggz.common;

public enum ClientReqError {
    E_NO_STATUS, /* internal placeholder; a statusless event */
    E_OK, /* No error */
    E_USR_LOOKUP, E_BAD_OPTIONS, E_ROOM_FULL, E_TABLE_FULL, E_TABLE_EMPTY, E_LAUNCH_FAIL, E_JOIN_FAIL, E_NO_TABLE, E_LEAVE_FAIL, E_LEAVE_FORBIDDEN, E_ALREADY_LOGGED_IN, E_NOT_LOGGED_IN, E_NOT_IN_ROOM, E_AT_TABLE, E_IN_TRANSIT, E_NO_PERMISSION, E_BAD_XML, E_SEAT_ASSIGN_FAIL, E_NO_CHANNEL, E_TOO_LONG, E_UNKNOWN, E_BAD_USERNAME;

    public String toString() {
        switch (this) {
        case E_OK:
            return "ok";
        case E_USR_LOOKUP:
            return "usr lookup";
        case E_BAD_USERNAME:
        	return "bad username";
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
            return "already logged in";
        case E_NOT_LOGGED_IN:
            return "not logged in";
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
        case E_NO_CHANNEL:
            return "no channel";
        case E_TOO_LONG:
            return "too long";
        case E_NO_STATUS:
        case E_UNKNOWN:
            break;
        }

        return "[unknown]";
    }

    /**
     * Converts an error string to a ClientReqError object. The strings
     * recognised by this function are those returned by the server as the CODE
     * attribute of the &lt;RESULT&gt; tag.
     * 
     * @param str
     * @return The equivalent ClientReqError or E_UNKNOWN if the code is not
     *         recognised.
     */
    public static ClientReqError string_to_error(String str) {
        if (str == null)
            return ClientReqError.E_OK;

        if ("ok".equals(str))
            return ClientReqError.E_OK;
        if ("0".equals(str)) {
            /*
             * This provides a tiny bit of backwards compatability. It should go
             * away eventually.
             */
            return ClientReqError.E_OK;
        }
        if ("usr lookup".equals(str))
            return ClientReqError.E_USR_LOOKUP;
        if ("bad username".equals(str))
            return ClientReqError.E_BAD_USERNAME;
        if ("bad options".equals(str))
            return ClientReqError.E_BAD_OPTIONS;
        if ("room full".equals(str))
            return ClientReqError.E_ROOM_FULL;
        if ("table full".equals(str))
            return ClientReqError.E_TABLE_FULL;
        if ("table empty".equals(str))
            return ClientReqError.E_TABLE_EMPTY;
        if ("launch fail".equals(str))
            return ClientReqError.E_LAUNCH_FAIL;
        if ("join fail".equals(str))
            return ClientReqError.E_JOIN_FAIL;
        if ("no table".equals(str))
            return ClientReqError.E_NO_TABLE;
        if ("leave fail".equals(str))
            return ClientReqError.E_LEAVE_FAIL;
        if ("leave forbidden".equals(str))
            return ClientReqError.E_LEAVE_FORBIDDEN;
        if ("already logged in".equals(str))
            return ClientReqError.E_ALREADY_LOGGED_IN;
        if ("not logged in".equals(str))
            return ClientReqError.E_NOT_LOGGED_IN;
        if ("not in room".equals(str))
            return ClientReqError.E_NOT_IN_ROOM;
        if ("at table".equals(str))
            return ClientReqError.E_AT_TABLE;
        if ("in transit".equals(str))
            return ClientReqError.E_IN_TRANSIT;
        if ("no permission".equals(str))
            return ClientReqError.E_NO_PERMISSION;
        if ("bad xml".equals(str))
            return ClientReqError.E_BAD_XML;
        if ("seat assign fail".equals(str))
            return ClientReqError.E_SEAT_ASSIGN_FAIL;
        if ("no channel".equals(str))
            return ClientReqError.E_NO_CHANNEL;
        if ("too long".equals(str))
            return ClientReqError.E_TOO_LONG;

        return ClientReqError.E_UNKNOWN;
    }
}