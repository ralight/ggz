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
package ggz.client.mod;

/**
 * Callback events.
 * 
 * Each of these is a possible GGZmod event. For each event, the table may
 * register a handler with GGZmod to handle that event.
 * 
 * @see GGZModHandler
 * @see ggzmod_set_handler
 */
public class ModEvent {
    /**
     * Module status changed This event occurs when the game's status changes.
     * The old state (a GGZModState*) is passed as the event's data.
     * 
     * @see GGZModState
     */
    public static final ModEvent GGZMOD_EVENT_STATE = new ModEvent();

    /**
     * A new server connection has been made This event occurs when a new
     * connection to the game server has been made, either by the core client or
     * by the game client itself. The fd is passed as the event's data.
     * 
     * @see ggzmod_connect
     */
    public static final ModEvent GGZMOD_EVENT_SERVER = new ModEvent();

    /**
     * The player's seat status has changed.
     * 
     * This event occurs when the player's seat status changes; i.e. he changes
     * seats or starts/stops spectating. The event data is a int[2] pair
     * consisting of the old {is_spectator, seat_num}.
     */
    public static final ModEvent GGZMOD_EVENT_PLAYER = new ModEvent();

    /**
     * A seat change.
     * 
     * This event occurs when a seat change occurs. The old seat (a GGZSeat*) is
     * passed as the event's data. The seat information will be updated before
     * the event is invoked.
     */
    public static final ModEvent GGZMOD_EVENT_SEAT = new ModEvent();

    /**
     * A spectator seat change.
     * 
     * This event occurs when a spectator seat change occurs. The old spectator
     * (a GGZSpectator*) is passed as the event's data. The spectator
     * information will be updated before the event is invoked.
     */
    public static final ModEvent GGZMOD_EVENT_SPECTATOR_SEAT = new ModEvent();

    /**
     * A chat message event.
     * 
     * This event occurs when we receive a chat. The chat may have originated in
     * another game client or from the GGZ client; in either case it will be
     * routed to us. The chat information (a GGZChat*) is passed as the event's
     * data. Note that the chat may originate with a player or a spectator, and
     * they may have changed seats or left the table by the time it gets to us.
     */
    public static final ModEvent GGZMOD_EVENT_CHAT = new ModEvent();

    /**
     * A player's stats have been updated.
     * 
     * @see ggzmod_player_get_record
     * @see ggzmod_player_get_rating
     * @see ggzmod_player_get_ranking
     * @see ggzmod_player_get_highscore
     * @param data
     *            The name of the player whose stats have changed.
     */
    public static final ModEvent GGZMOD_EVENT_STATS = new ModEvent();

    /**
     * Player information has arrived.
     * 
     * Information has been requested about one or more players and it has now
     * arrived. The event data is a GGZPlayerInfo* structure or NULL if info
     * about all players was requested.
     */
    public static final ModEvent GGZMOD_EVENT_INFO = new ModEvent();

    /**
     * An error has occurred This event occurs when a GGZMod error has occurred.
     * An error message (a char*) will be passed as the event's data. GGZMod may
     * attempt to recover from the error, but it is not guaranteed that the GGZ
     * connection will continue to work after an error has happened.
     */
    public static final ModEvent GGZMOD_EVENT_ERROR = new ModEvent();

    private ModEvent() {
        // Private constructor to prevent access.
    }
}
