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
package ggz.client.core;

/**
 * @brief The states a server connection may be in.
 * 
 * On the client side, a simplistic state maching is used to tell what's going
 * on. A game client should usually consult the current state when determining
 * what actions are possible.
 */
public class StateID {
	/** Not connected (at all) */
	public static final StateID GGZ_STATE_OFFLINE = new StateID("GGZ_STATE_OFFLINE");

	/** In the process of connecting. */
	public static final StateID GGZ_STATE_CONNECTING = new StateID("GGZ_STATE_CONNECTING");

	/** Continuous reconnection attempts. */
	public static final StateID GGZ_STATE_RECONNECTING = new StateID("GGZ_STATE_RECONNECTING");

	/** Connected, but not doing anything. */
	public static final StateID GGZ_STATE_ONLINE = new StateID("GGZ_STATE_ONLINE");

	/** In the process of logging in. */
	public static final StateID GGZ_STATE_LOGGING_IN = new StateID("GGZ_STATE_LOGGING_IN");

	/** Online and logged in! */
	public static final StateID GGZ_STATE_LOGGED_IN = new StateID("GGZ_STATE_LOGGED_IN");

	/** Moving into a room. */
	public static final StateID GGZ_STATE_ENTERING_ROOM = new StateID("GGZ_STATE_ENTERING_ROOM");

	/** Online, logged in, and in a room. */
	public static final StateID GGZ_STATE_IN_ROOM = new StateID("GGZ_STATE_IN_ROOM");

	/** Moving between rooms. */
	public static final StateID GGZ_STATE_BETWEEN_ROOMS = new StateID("GGZ_STATE_BETWEEN_ROOMS");

	/** Trying to launch a table. */
	public static final StateID GGZ_STATE_LAUNCHING_TABLE = new StateID("GGZ_STATE_LAUNCHING_TABLE");

	/** Trying to join a table. */
	public static final StateID GGZ_STATE_JOINING_TABLE = new StateID("GGZ_STATE_JOINING_TABLE");

	/** Online, loggied in, in a room, at a table. */
	public static final StateID GGZ_STATE_AT_TABLE = new StateID("GGZ_STATE_AT_TABLE");

	/** Waiting to leave a table. */
	public static final StateID GGZ_STATE_LEAVING_TABLE = new StateID("GGZ_STATE_LEAVING_TABLE");

	/** In the process of logging out. */
	public static final StateID GGZ_STATE_LOGGING_OUT = new StateID("GGZ_STATE_LOGGING_OUT");

	public static int nextOrdinal = 0;
	
	public int ordinal;
    
    private String name;
	
	private StateID(String s) {
        name = s;
		ordinal = nextOrdinal;
		nextOrdinal++;
	}
	
	public int ordinal() {
		return ordinal;
	}
    
    public String toString() {
        return name;
    }
}
