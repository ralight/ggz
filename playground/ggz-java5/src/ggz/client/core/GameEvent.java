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
 * A GGZGameEvent is an event associated with the game, that is triggered by a
 * communication from the server or from the game. When a game event occurs, the
 * associated event handle will be called, and will be passed the event data (a
 * void*) along with the (optional) user data. All game events apply to the
 * current game. Game events are usually triggered by calling
 * ggzcore_server_read_data or ggzcore_game_read_data.
 * 
 * @see ggzcore_game_add_event_hook
 * @see ggzcore_server_read_data
 */
public enum GameEvent {
    /**
     * A game was launched by the player (you). After this the core client
     * should call ggzcore_game_get_control_fd, monitor the socket that function
     * returns, and call ggzcore_game_read_data when there is data pending. This
     * event is triggered inside of ggzcore_game_launch.
     * 
     * @param data
     *            NULL
     * @see ggzcore_game_launch
     */
    GGZ_GAME_LAUNCHED,

    /**
     * Your game launch has failed. Triggered instead of GGZ_GAME_LAUNCHED when
     * there's a failure somewhere.
     * 
     * @param data
     *            NULL
     * @see GGZ_GAME_LAUNCHED
     */
    GGZ_GAME_LAUNCH_FAIL,

    /**
     * Negotiation with server was successful. This should happen some time
     * after the launch succeeds. The core client need do nothing at this point.
     * 
     * @param data
     *            NULL
     */
    GGZ_GAME_NEGOTIATED,

    /**
     * Negotiation was not successful, game launch failed.
     * 
     * @todo Currently this can't actually happen...
     */
    GGZ_GAME_NEGOTIATE_FAIL,

    /**
     * Game reached the 'playing' state. When this happens the core client
     * should call ggzcore_room_launch_table or ggzcore_room_join_table to
     * finalize the game join.
     * 
     * @param data
     *            NULL
     */
    GGZ_GAME_PLAYING
}
