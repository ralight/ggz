/*
 * Copyright (C) 2007  Helg Bredow
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

/** A permission set (bitfield) contains all perms for one player. */
public class PermSet {

    private int bitfield;

    public PermSet() {
        this(0);
    }

    public PermSet(int bitfield) {
        this.bitfield = bitfield;
    }

    /** @return true if the perm set qualifies the player as a 'host' */
    public boolean isHost() {
        return (isSet(Perm.GGZ_PERM_EDIT_TABLES) || isSet(Perm.GGZ_PERM_TABLE_PRIVMSG));
    }

    /** @return true if the perm set qualifies the player as a 'bot' */
    public boolean isBot() {
        return isSet(Perm.GGZ_PERM_CHAT_BOT);
    }

    /** @return true if the perm set qualifies the player as an 'admin' */
    public boolean isAdmin() {
        return (isSet(Perm.GGZ_PERM_ROOMS_ADMIN)
                && isSet(Perm.GGZ_PERM_CHAT_ANNOUNCE)
                && isSet(Perm.GGZ_PERM_EDIT_TABLES) && isSet(Perm.GGZ_PERM_TABLE_PRIVMSG));
    }

    /**
     * Sets/clears the given permission.
     * 
     * @param perm
     *            The permission in the set to change.
     * @param value
     *            Whether to set or clear the permission from the set.
     */
    protected void set(Perm perm, boolean val) {
        if (val) {
            /* Set bit */
            (bitfield) |= (1 << perm.ordinal());
        } else {
            /* Clear bit. */
            (bitfield) &= ~(1 << perm.ordinal());
        }
    }

    /** @return true if the permissions is in the set. */
    public boolean isSet(Perm perm) {
        return ((bitfield & (1 << perm.ordinal())) != 0);
    }

    /** @return a set of permissions that represents the given player type. */
    public static PermSet forPlayerType(PlayerType type) {
        PermSet perms = new PermSet();
        if (type == PlayerType.GGZ_PLAYER_NORMAL) {
            perms.set(Perm.GGZ_PERM_JOIN_TABLE, true);
            perms.set(Perm.GGZ_PERM_LAUNCH_TABLE, true);
            perms.set(Perm.GGZ_PERM_ROOMS_LOGIN, true);
            perms.set(Perm.GGZ_PERM_ROOMS_ADMIN, false);
            perms.set(Perm.GGZ_PERM_CHAT_ANNOUNCE, false);
            perms.set(Perm.GGZ_PERM_CHAT_BOT, false);
            perms.set(Perm.GGZ_PERM_NO_STATS, false);
            perms.set(Perm.GGZ_PERM_EDIT_TABLES, false);
            perms.set(Perm.GGZ_PERM_TABLE_PRIVMSG, false);
        } else if (type == PlayerType.GGZ_PLAYER_ADMIN) {
            perms.set(Perm.GGZ_PERM_JOIN_TABLE, true);
            perms.set(Perm.GGZ_PERM_LAUNCH_TABLE, true);
            perms.set(Perm.GGZ_PERM_ROOMS_LOGIN, true);
            perms.set(Perm.GGZ_PERM_ROOMS_ADMIN, true);
            perms.set(Perm.GGZ_PERM_CHAT_ANNOUNCE, true);
            perms.set(Perm.GGZ_PERM_CHAT_BOT, false);
            perms.set(Perm.GGZ_PERM_NO_STATS, false);
            perms.set(Perm.GGZ_PERM_EDIT_TABLES, true);
            perms.set(Perm.GGZ_PERM_TABLE_PRIVMSG, true);
        } else if (type == PlayerType.GGZ_PLAYER_HOST) {
            perms.set(Perm.GGZ_PERM_JOIN_TABLE, true);
            perms.set(Perm.GGZ_PERM_LAUNCH_TABLE, true);
            perms.set(Perm.GGZ_PERM_ROOMS_LOGIN, true);
            perms.set(Perm.GGZ_PERM_ROOMS_ADMIN, false);
            perms.set(Perm.GGZ_PERM_CHAT_ANNOUNCE, false);
            perms.set(Perm.GGZ_PERM_CHAT_BOT, false);
            perms.set(Perm.GGZ_PERM_NO_STATS, false);
            perms.set(Perm.GGZ_PERM_EDIT_TABLES, true);
            perms.set(Perm.GGZ_PERM_TABLE_PRIVMSG, true);
        } else if (type == PlayerType.GGZ_PLAYER_BOT) {
            perms.set(Perm.GGZ_PERM_JOIN_TABLE, true);
            perms.set(Perm.GGZ_PERM_LAUNCH_TABLE, true);
            perms.set(Perm.GGZ_PERM_ROOMS_LOGIN, true);
            perms.set(Perm.GGZ_PERM_ROOMS_ADMIN, false);
            perms.set(Perm.GGZ_PERM_CHAT_ANNOUNCE, false);
            perms.set(Perm.GGZ_PERM_CHAT_BOT, true);
            perms.set(Perm.GGZ_PERM_NO_STATS, false);
            perms.set(Perm.GGZ_PERM_EDIT_TABLES, false);
            perms.set(Perm.GGZ_PERM_TABLE_PRIVMSG, false);
        }
        return perms;
    }
}
