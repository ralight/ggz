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

import java.util.ArrayList;

/*
 * Player permissions
 * 
 * These definitions provide a set of permissions that GGZ players may have.
 */
public class Perm {

    private static final ArrayList values = new ArrayList();

    /* Boolean permissions for GGZ players. */
    /** Can join a table */
    public static final Perm GGZ_PERM_JOIN_TABLE = new Perm();

    /** Can launch a new table */
    public static final Perm GGZ_PERM_LAUNCH_TABLE = new Perm();

    /** Can enter login-only rooms */
    public static final Perm GGZ_PERM_ROOMS_LOGIN = new Perm();

    /** Can enter admin-only rooms */
    public static final Perm GGZ_PERM_ROOMS_ADMIN = new Perm();

    /** Can make announcements */
    public static final Perm GGZ_PERM_CHAT_ANNOUNCE = new Perm();

    /** Player is a known bot */
    public static final Perm GGZ_PERM_CHAT_BOT = new Perm();

    /** No stats for this player. */
    public static final Perm GGZ_PERM_NO_STATS = new Perm();

    /** Can edit tables. */
    public static final Perm GGZ_PERM_EDIT_TABLES = new Perm();

    /** Can send public messages at a table */
    public static final Perm GGZ_PERM_TABLE_PRIVMSG = new Perm();

    /** These are defined by the network protocol */
    private static String[] PERM_NAMES = { "join_table", "launch_table",
            "rooms_login", "rooms_admin", "chat_announce", "chat_bot",
            "no_stats", "edit_tables", "edit_privmsg" };

    private Perm() {
        values.add(this);
    }

    public int ordinal() {
        return values.indexOf(this);
    }

    public String toString() {
        return PERM_NAMES[ordinal()];
    }

    public static Perm[] values() {
        return (Perm[]) values.toArray(new Perm[values.size()]);
    }
}
