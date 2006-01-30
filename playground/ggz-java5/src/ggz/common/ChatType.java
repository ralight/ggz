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
 * @brief Chat types.
 * 
 * Each time we send a chat to the server, it will have one of these types
 * associated with it.
 * @see ggzcore_room_chat
 */
public enum ChatType {
    /**
     * An unknown/unrecognized chat type - likely either a future chat op or a
     * communications error. It can either be ignored or handled generically.
     */
    GGZ_CHAT_UNKNOWN,

    /** A normal chat, just a message sent to the current room. */
    GGZ_CHAT_NORMAL,

    /**
     * An announcement, usually triggered with /wall. Only admins can do this,
     * and it is announced in all rooms.
     */
    GGZ_CHAT_ANNOUNCE,

    /**
     * A beep. We beep a player, and their client will run the beep.
     */
    GGZ_CHAT_BEEP,

    /**
     * A personal message to another player. It consists of both a message and a
     * target player.
     */
    GGZ_CHAT_PERSONAL,

    /**
     * A table-wide chat. It goes only to players at the current table.
     */
    GGZ_CHAT_TABLE;

    private static final String NORMAL_CHAT_NAME = "normal";

    private static final String ANNOUNCE_CHAT_NAME = "announce";

    private static final String BEEP_CHAT_NAME = "beep";

    private static final String PRIVATE_CHAT_NAME = "private";

    private static final String TABLE_CHAT_NAME = "table";

    public String toString() {
        switch (this) {
        case GGZ_CHAT_NORMAL:
            return NORMAL_CHAT_NAME;
        case GGZ_CHAT_ANNOUNCE:
            return ANNOUNCE_CHAT_NAME;
        case GGZ_CHAT_BEEP:
            return BEEP_CHAT_NAME;
        case GGZ_CHAT_PERSONAL:
            return PRIVATE_CHAT_NAME;
        case GGZ_CHAT_TABLE:
            return TABLE_CHAT_NAME;
        case GGZ_CHAT_UNKNOWN:
            break;
        }

        // ggz_error_msg("ggz_chattype_to_string: invalid chattype "+type+"
        // given.");
        return ""; /* ? */
    }

    public static ChatType string_to_chattype(String type_str) {
        /*
         * If it doesn't match _exactly_ we return GGZ_CHAT_NONE. This is bad
         * for, say, user input, but perfectly acceptable as an inverse to
         * ggz_chattype_to_string().
         */
        if (type_str == null)
            return GGZ_CHAT_UNKNOWN;

        if (NORMAL_CHAT_NAME.equals(type_str))
            return GGZ_CHAT_NORMAL;
        else if (ANNOUNCE_CHAT_NAME.equals(type_str))
            return GGZ_CHAT_ANNOUNCE;
        else if (BEEP_CHAT_NAME.equals(type_str))
            return GGZ_CHAT_BEEP;
        else if (PRIVATE_CHAT_NAME.equals(type_str))
            return GGZ_CHAT_PERSONAL;
        else if (TABLE_CHAT_NAME.equals(type_str))
            return GGZ_CHAT_TABLE;

        return GGZ_CHAT_UNKNOWN;
    }
}