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

import java.util.ArrayList;

/**
 * A player type.
 * 
 * The server will tell the client the type of each player.
 */
public class PlayerType {

	public static final ArrayList values = new ArrayList();

	/** A normal player is registered but has no special permission. */
    public static final PlayerType GGZ_PLAYER_NORMAL = new PlayerType();

    /** A guest player is not registered. */
    public static final PlayerType GGZ_PLAYER_GUEST = new PlayerType();

    /** An admin player is registered and has some special permissions. */
    public static final PlayerType GGZ_PLAYER_ADMIN = new PlayerType();

    /** A bot is a special type of player. */
    public static final PlayerType GGZ_PLAYER_BOT = new PlayerType();

    /** This is an unknown type of player. */
    public static final PlayerType GGZ_PLAYER_UNKNOWN = new PlayerType();

    private static final String GUEST_PLAYER_NAME = "guest";

    private static final String NORMAL_PLAYER_NAME = "normal";

    private static final String ADMIN_PLAYER_NAME = "admin";

    private static final String BOT_PLAYER_NAME = "bot";

    private static final String UNKNOWN_PLAYER_NAME = "unknown";

    private PlayerType() {
    	values.add(this);
    }
    
    public int ordinal() {
    	return values.indexOf(this);
    }
    
    public static PlayerType[] values() {
    	return (PlayerType[]) values.toArray(new PlayerType[values.size()]);
    }
    
    public String toString() {
        if (this == GGZ_PLAYER_GUEST) {
            return GUEST_PLAYER_NAME;
        } else if (this == GGZ_PLAYER_NORMAL) {
            return NORMAL_PLAYER_NAME;
        } else if (this == GGZ_PLAYER_ADMIN) {
            return ADMIN_PLAYER_NAME;
        } else if (this == GGZ_PLAYER_BOT) {
            return BOT_PLAYER_NAME;
        } else if (this == GGZ_PLAYER_UNKNOWN) {
            return UNKNOWN_PLAYER_NAME;
        }

        throw new RuntimeException("Unrecognised PlayerType: " + this);
    }

    public static PlayerType valueOf(String type_str) {
        if (type_str == null) {
            return GGZ_PLAYER_UNKNOWN;
        } else if (NORMAL_PLAYER_NAME.equalsIgnoreCase(type_str)) {
            return GGZ_PLAYER_NORMAL;
        } else if (GUEST_PLAYER_NAME.equalsIgnoreCase(type_str)) {
            return GGZ_PLAYER_GUEST;
        } else if (ADMIN_PLAYER_NAME.equalsIgnoreCase(type_str)) {
            return GGZ_PLAYER_ADMIN;
        } else if (BOT_PLAYER_NAME.equalsIgnoreCase(type_str)) {
            return GGZ_PLAYER_BOT;
        }

        return GGZ_PLAYER_UNKNOWN; /* ? */
    }
}
