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

public class ClientReqError {
    /*
     * internal placeholder; a statusless event
     */
    public static final ClientReqError E_NO_STATUS = new ClientReqError();

    /* No error */
    public static final ClientReqError E_OK = new ClientReqError();

    public static final ClientReqError E_USR_LOOKUP = new ClientReqError();

    public static final ClientReqError E_USR_FOUND = new ClientReqError();

    public static final ClientReqError E_USR_TYPE = new ClientReqError();

    public static final ClientReqError E_BAD_OPTIONS = new ClientReqError();

    public static final ClientReqError E_ROOM_FULL = new ClientReqError();

    public static final ClientReqError E_TABLE_FULL = new ClientReqError();

    public static final ClientReqError E_TABLE_EMPTY = new ClientReqError();

    public static final ClientReqError E_LAUNCH_FAIL = new ClientReqError();

    public static final ClientReqError E_JOIN_FAIL = new ClientReqError();

    public static final ClientReqError E_NO_TABLE = new ClientReqError();

    public static final ClientReqError E_LEAVE_FAIL = new ClientReqError();

    public static final ClientReqError E_LEAVE_FORBIDDEN = new ClientReqError();

    public static final ClientReqError E_ALREADY_LOGGED_IN = new ClientReqError();

    public static final ClientReqError E_NOT_LOGGED_IN = new ClientReqError();

    public static final ClientReqError E_NOT_IN_ROOM = new ClientReqError();

    public static final ClientReqError E_AT_TABLE = new ClientReqError();

    public static final ClientReqError E_IN_TRANSIT = new ClientReqError();

    public static final ClientReqError E_NO_PERMISSION = new ClientReqError();

    public static final ClientReqError E_BAD_XML = new ClientReqError();

    public static final ClientReqError E_SEAT_ASSIGN_FAIL = new ClientReqError();

    public static final ClientReqError E_NO_CHANNEL = new ClientReqError();

    public static final ClientReqError E_TOO_LONG = new ClientReqError();

    public static final ClientReqError E_UNKNOWN = new ClientReqError();

    public static final ClientReqError E_BAD_USERNAME = new ClientReqError();

    private ClientReqError() {
    }

    public String toString() {
        if (this == E_OK) {
            return "ok";
        } else if (this == E_USR_LOOKUP) {
            return "usr lookup";
        } else if (this == E_USR_FOUND) {
            return "user not found";
        } else if (this == E_USR_TYPE) {
            return "wrong login type";
        } else if (this == E_BAD_USERNAME) {
            return "bad username";
        } else if (this == E_BAD_OPTIONS) {
            return "bad options";
        } else if (this == E_ROOM_FULL) {
            return "room full";
        } else if (this == E_TABLE_FULL) {
            return "table full";
        } else if (this == E_TABLE_EMPTY) {
            return "table empty";
        } else if (this == E_LAUNCH_FAIL) {
            return "launch fail";
        } else if (this == E_JOIN_FAIL) {
            return "join fail";
        } else if (this == E_NO_TABLE) {
            return "no table";
        } else if (this == E_LEAVE_FAIL) {
            return "leave fail";
        } else if (this == E_LEAVE_FORBIDDEN) {
            return "leave forbidden";
        } else if (this == E_ALREADY_LOGGED_IN) {
            return "already logged in";
        } else if (this == E_NOT_LOGGED_IN) {
            return "not logged in";
        } else if (this == E_NOT_IN_ROOM) {
            return "not in room";
        } else if (this == E_AT_TABLE) {
            return "at table";
        } else if (this == E_IN_TRANSIT) {
            return "in transit";
        } else if (this == E_NO_PERMISSION) {
            return "no permission";
        } else if (this == E_BAD_XML) {
            return "bad xml";
        } else if (this == E_SEAT_ASSIGN_FAIL) {
            return "seat assign fail";
        } else if (this == E_NO_CHANNEL) {
            return "no channel";
        } else if (this == E_TOO_LONG) {
            return "too long";
        } else if (this == E_NO_STATUS) {
        } else if (this == E_UNKNOWN) {
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
    public static ClientReqError valueOf(String str) {
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
        if ("user not found".equals(str))
            return E_USR_FOUND;
        if ("wrong login type".equals(str))
            return E_USR_TYPE;
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