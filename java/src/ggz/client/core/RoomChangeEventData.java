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

/** @brief The data associated with GGZ_ROOM_ENTER/GGZ_ROOM_LEAVE events. */
public class RoomChangeEventData {

    /** The player entering/leaving. */
    public Player player;

    /**
     * The room we are entering.
     * 
     * This may be -1 if the player is leaving the server, or -2 if the info is
     * unknown.
     */
    public int to_room;

    /**
     * The room the player is leaving.
     * 
     * This may be -1 if the player is just entering the server, or -2 if the
     * info is unknown.
     */
    public int from_room;
}
