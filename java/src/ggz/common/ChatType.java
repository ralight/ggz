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

/**
 * Each time we send a chat to the server, it will have one of these types
 * associated with it.
 */
public class ChatType {
    /**
     * An unknown/unrecognized chat type - likely either a future chat op or a
     * communications error. It can either be ignored or handled generically.
     */
    public static final ChatType GGZ_CHAT_UNKNOWN = new ChatType();

    /** A normal chat, just a message sent to the current room. */
    public static final ChatType GGZ_CHAT_NORMAL = new ChatType();

    /**
     * An announcement, usually triggered with /wall. Only admins can do this,
     * and it is announced in all rooms.
     */
    public static final ChatType GGZ_CHAT_ANNOUNCE = new ChatType();

    /**
     * A beep. We beep a player, and their client will run the beep.
     */
    public static final ChatType GGZ_CHAT_BEEP = new ChatType();

    /**
     * A personal message to another player. It consists of both a message and a
     * target player.
     */
    public static final ChatType GGZ_CHAT_PERSONAL = new ChatType();

    /**
     * A table-wide chat. It goes only to players at the current table.
     */
    public static final ChatType GGZ_CHAT_TABLE = new ChatType();

    private static final String NORMAL_CHAT_NAME = "normal";

    private static final String ANNOUNCE_CHAT_NAME = "announce";

    private static final String BEEP_CHAT_NAME = "beep";

    private static final String PRIVATE_CHAT_NAME = "private";

    private static final String TABLE_CHAT_NAME = "table";

    private ChatType() {
        // Private constructor to prevent access.
    }

    public String toString() {
        if (this == GGZ_CHAT_NORMAL) {
            return NORMAL_CHAT_NAME;
        } else if (this == GGZ_CHAT_ANNOUNCE) {
            return ANNOUNCE_CHAT_NAME;
        } else if (this == GGZ_CHAT_BEEP) {
            return BEEP_CHAT_NAME;
        } else if (this == GGZ_CHAT_PERSONAL) {
            return PRIVATE_CHAT_NAME;
        } else if (this == GGZ_CHAT_TABLE) {
            return TABLE_CHAT_NAME;
        }
        return super.toString();
    }

    /**
     * Converts a string to a a ChatType. If the string doesn't match exactly
     * then GGZ_CHAT_UNKOWN is returned. The recognised strings are normal,
     * announce, beep, private and table.
     * 
     * @param type_str
     *            The string to convert.
     * @return The chat type that corresponds to the given string.
     */
    public static ChatType valueOf(String type_str) {
        if (NORMAL_CHAT_NAME.equals(type_str)) {
            return GGZ_CHAT_NORMAL;
        } else if (ANNOUNCE_CHAT_NAME.equals(type_str)) {
            return GGZ_CHAT_ANNOUNCE;
        } else if (BEEP_CHAT_NAME.equals(type_str)) {
            return GGZ_CHAT_BEEP;
        } else if (PRIVATE_CHAT_NAME.equals(type_str)) {
            return GGZ_CHAT_PERSONAL;
        } else if (TABLE_CHAT_NAME.equals(type_str)) {
            return GGZ_CHAT_TABLE;
        }
        return GGZ_CHAT_UNKNOWN;
    }
}