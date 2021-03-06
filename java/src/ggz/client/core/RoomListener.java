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

import java.util.EventListener;
import java.util.List;

public interface RoomListener extends EventListener {
    /**
     * The list of players in a room has arrived.
     * 
     * @param players
     *            The list of players.
     * @note This will only be issued for the current room.
     * @see ggzcore_room_list_players
     */
    public void player_list(List players);

    /**
     * Received the list of active tables.
     * 
     * @param data
     *            NULL
     * @see ggzcore_room_list_tables
     */
    public void table_list();

    /**
     * Received a chat message of any kind. This can happen at any time when
     * you're in a room.
     * 
     * @param data
     *            The GGZChatEventData associated with the chat.
     * @see GGZChatEventData
     */
    public void chat_event(ChatEventData data);

    /**
     * A player has entered the room with you.
     * 
     * @param data
     *            A GGZRoomChangeEventData structure.
     */
    public void room_enter(RoomChangeEventData data);

    /**
     * A player has left your room.
     * 
     * @param data
     *            A GGZRoomChangeEventData structure.
     */
    public void room_leave(RoomChangeEventData data);

    /**
     * A table has been created.
     * 
     * @param data
     *            the table that was added
     */
    public void table_add(Table table);

    /**
     * A table has been removed from the room.
     * 
     * @param data
     *            the table that was removed
     */
    public void table_delete(Table table);

    /**
     * One of the tables in the current room has changed.
     * 
     * @param data
     *            the table that was updated
     */
    public void table_update(Table table);

    /**
     * The table you tried to launch has launched!
     * 
     * @see ggzcore_room_launch_table
     * @param data
     *            NULL
     */
    public void table_launched();

    /**
     * The table you tried to launch couldn't be launched
     * 
     * @see public void TABLE_LAUNCHED
     * @param data
     *            A pointer to a GGZErrorEventData
     */
    public void table_launch_fail(ErrorEventData data);

    /**
     * Your table join attempt has succeeded.
     * 
     * @see ggzcore_room_join_table
     * @param data
     *            The table index (int*) of the table we joined.
     */
    public void table_joined(int table_index);

    /**
     * Joining a table did not succeed.
     * 
     * @see public void TABLE_JOINED
     * @param data
     *            A helpful error string.
     */
    public void table_join_fail(String error);

    /**
     * You have successfully left the table you were at.
     * 
     * @see ggzcore_room_leave_table
     * @param data
     *            The GGZTableLeaveEventData associated with the leave.
     * @see GGZTableLeaveEventData
     */
    public void table_left(TableLeaveEventData data);

    /**
     * Your attempt to leave the table has failed.
     * 
     * @see public void TABLE_LEFT
     * @param data
     *            A helpful error string.
     */
    public void table_leave_fail(String error);

    /**
     * A player's lag (measure of connection speed) has been updated
     * 
     * @see ggzcore_player_get_lag
     * @param player
     *            The player whose lag has changed.
     */
    public void player_lag(Player player);

    /**
     * A player's stats have been updated.
     * 
     * @see public void PLAYER_LIST
     * @see ggzcore_player_get_record
     * @see ggzcore_player_get_rating
     * @see ggzcore_player_get_ranking
     * @see ggzcore_player_get_highscore
     * @param player
     *            The player whose stats have changed.
     */
    public void player_stats(Player player);

    /**
     * The number of players in a room has arrived.
     * 
     * @param count
     *            The number of players in the room.
     */
    public void player_count(int count);

    /**
     * A player's permissions have been updated.
     * 
     * @see Player#get_perms()
     * @see Player#has_perm(ggz.common.Perm)
     * @param data
     *            The name of the player whose perms have changed.
     */
    public void player_perms(Player player);

}
