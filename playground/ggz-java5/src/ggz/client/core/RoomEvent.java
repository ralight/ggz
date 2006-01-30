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
public enum RoomEvent {
    /**
     * The list of players in a room has arrived.
     * 
     * @param data
     *            The room id (int *)
     * @note This will only be issued for the current room.
     * @see ggzcore_room_list_players
     */
    GGZ_PLAYER_LIST,

    /**
     * Received the list of active tables.
     * 
     * @param data
     *            NULL
     * @see ggzcore_room_list_tables
     */
    GGZ_TABLE_LIST,

    /**
     * Received a chat message of any kind. This can happen at any time when
     * you're in a room.
     * 
     * @param data
     *            The GGZChatEventData associated with the chat.
     * @see GGZChatEventData
     */
    GGZ_CHAT_EVENT,

    /**
     * A player has entered the room with you.
     * 
     * @param data
     *            A GGZRoomChangeEventData structure.
     */
    GGZ_ROOM_ENTER,

    /**
     * A player has left your room.
     * 
     * @param data
     *            A GGZRoomChangeEventData structure.
     */
    GGZ_ROOM_LEAVE,

    /**
     * One of the tables in the current room has changed.
     * 
     * @todo How are you supposed to know which table has changed?
     * @param data
     *            NULL
     */
    GGZ_TABLE_UPDATE,

    /**
     * The table you tried to launch has launched!
     * 
     * @see ggzcore_room_launch_table
     * @param data
     *            NULL
     */
    GGZ_TABLE_LAUNCHED,

    /**
     * The table you tried to launch couldn't be launched
     * 
     * @see GGZ_TABLE_LAUNCHED
     * @param data
     *            A pointer to a GGZErrorEventData
     */
    GGZ_TABLE_LAUNCH_FAIL,

    /**
     * Your table join attempt has succeeded.
     * 
     * @see ggzcore_room_join_table
     * @param data
     *            The table index (int*) of the table we joined.
     */
    GGZ_TABLE_JOINED,

    /**
     * Joining a table did not succeed.
     * 
     * @see GGZ_TABLE_JOINED
     * @param data
     *            A helpful error string.
     */
    GGZ_TABLE_JOIN_FAIL,

    /**
     * You have successfully left the table you were at.
     * 
     * @see ggzcore_room_leave_table
     * @param data
     *            The GGZTableLeaveEventData associated with the leave.
     * @see GGZTableLeaveEventData
     */
    GGZ_TABLE_LEFT,

    /**
     * Your attempt to leave the table has failed.
     * 
     * @see GGZ_TABLE_LEFT
     * @param data
     *            A helpful error string.
     */
    GGZ_TABLE_LEAVE_FAIL,

    /**
     * A player's lag (measure of connection speed) has been updated
     * 
     * @see ggzcore_player_get_lag
     * @param data
     *            The name of the player whose lag has changed.
     */
    GGZ_PLAYER_LAG,

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
    GGZ_PLAYER_STATS,

    /**
     * The number of players in a room has arrived.
     * 
     * @param data
     *            The room id (int *)
     */
    GGZ_PLAYER_COUNT;
}
