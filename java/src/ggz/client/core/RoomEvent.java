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
 * A GGZRoomEvent is an event associated with the room, that is triggered by a
 * communication from the server. When a room event occurs, the associated event
 * handler will be called, and will be passed the event data (a void*), along
 * with the (optional) user data. All room events apply to the current room
 * unless a room number is given. Room events are almost always triggered by
 * calling ggzcore_server_read_data.
 * 
 * @see ggzcore_room_add_event_hook
 * @see ggzcore_server_read_data
 */
public class RoomEvent {
    /**
     * The list of players in a room has arrived.
     * 
     * @param data
     *            The room id (int *)
     * @note This will only be issued for the current room.
     * @see ggzcore_room_list_players
     */
    public static final RoomEvent GGZ_PLAYER_LIST = new RoomEvent();

    /**
     * Received the list of active tables.
     * 
     * @param data
     *            NULL
     * @see ggzcore_room_list_tables
     */
    public static final RoomEvent GGZ_TABLE_LIST = new RoomEvent();

    /**
     * Received a chat message of any kind. This can happen at any time when
     * you're in a room.
     * 
     * @param data
     *            The GGZChatEventData associated with the chat.
     * @see GGZChatEventData
     */
    public static final RoomEvent GGZ_CHAT_EVENT = new RoomEvent();

    /**
     * A player has entered the room with you.
     * 
     * @param data
     *            A GGZRoomChangeEventData structure.
     */
    public static final RoomEvent GGZ_ROOM_ENTER = new RoomEvent();

    /**
     * A player has left your room.
     * 
     * @param data
     *            A GGZRoomChangeEventData structure.
     */
    public static final RoomEvent GGZ_ROOM_LEAVE = new RoomEvent();

    /**
     * A table has been added (created) to the room.
     * 
     * @param data
     *            the new table
     */
    public static final RoomEvent GGZ_TABLE_ADD = new RoomEvent();

    /**
     * A table has been removed (deleted) from the room.
     * 
     * @param data
     *            the table that was dropped
     */
    public static final RoomEvent GGZ_TABLE_DELETE = new RoomEvent();

    /**
     * One of the tables in the current room has changed.
     * 
     * @param data
     *            the table that was updated
     */
    public static final RoomEvent GGZ_TABLE_UPDATE = new RoomEvent();

    /**
     * The table you tried to launch has launched!
     * 
     * @see ggzcore_room_launch_table
     * @param data
     *            NULL
     */
    public static final RoomEvent GGZ_TABLE_LAUNCHED = new RoomEvent();

    /**
     * The table you tried to launch couldn't be launched
     * 
     * @see GGZ_TABLE_LAUNCHED
     * @param data
     *            A pointer to a GGZErrorEventData
     */
    public static final RoomEvent GGZ_TABLE_LAUNCH_FAIL = new RoomEvent();

    /**
     * Your table join attempt has succeeded.
     * 
     * @see ggzcore_room_join_table
     * @param data
     *            The table index (int*) of the table we joined.
     */
    public static final RoomEvent GGZ_TABLE_JOINED = new RoomEvent();

    /**
     * Joining a table did not succeed.
     * 
     * @see GGZ_TABLE_JOINED
     * @param data
     *            A helpful error string.
     */
    public static final RoomEvent GGZ_TABLE_JOIN_FAIL = new RoomEvent();

    /**
     * You have successfully left the table you were at.
     * 
     * @see ggzcore_room_leave_table
     * @param data
     *            The GGZTableLeaveEventData associated with the leave.
     * @see GGZTableLeaveEventData
     */
    public static final RoomEvent GGZ_TABLE_LEFT = new RoomEvent();

    /**
     * Your attempt to leave the table has failed.
     * 
     * @see GGZ_TABLE_LEFT
     * @param data
     *            A helpful error string.
     */
    public static final RoomEvent GGZ_TABLE_LEAVE_FAIL = new RoomEvent();

    /**
     * A player's lag (measure of connection speed) has been updated
     * 
     * @see ggzcore_player_get_lag
     * @param data
     *            The name of the player whose lag has changed.
     */
    public static final RoomEvent GGZ_PLAYER_LAG = new RoomEvent();

    /**
     * A player's stats have been updated.
     * 
     * @see GGZ_PLAYER_LIST
     * @see ggzcore_player_get_record
     * @see ggzcore_player_get_rating
     * @see ggzcore_player_get_ranking
     * @see ggzcore_player_get_highscore
     * @param data
     *            The name of the player whose stats have changed.
     */
    public static final RoomEvent GGZ_PLAYER_STATS = new RoomEvent();

    /**
     * The number of players in a room has arrived.
     * 
     * @param data
     *            The room id (int *)
     */
    public static final RoomEvent GGZ_PLAYER_COUNT = new RoomEvent();
    
    private RoomEvent() {}
}
