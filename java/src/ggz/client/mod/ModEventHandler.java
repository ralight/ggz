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

import ggz.common.PlayerInfo;

import java.io.IOException;
import java.net.Socket;

public interface ModEventHandler {
    public void init(ModGame mod) throws IOException;

    /**
     * Module status changed. This event occurs when the game's status changes.
     * The old state (a ModState) is passed as the event's data.
     * 
     * @see ModState
     */
    public void handleState(ModState oldState);

    /**
     * A new server connection has been made. This event occurs when a new
     * connection to the game server has been made, either by the core client or
     * by the game client itself. The socket is passed as the event's data.
     * 
     * @see ModGame#connect()
     */
    public void handleServer(Socket fd) throws IOException;

    /**
     * The player's seat status has changed. Invoked when information about the
     * current player changes. e.g. When standing the player changes to a
     * spectator or when changing seats the seat number changes. The old values
     * are passed as parameters and to obtain the new values, call the accessor
     * methods on the GGZMod.
     * 
     * @param oldName
     * @param oldIsSpectator
     * @param oldSeatNum
     */
    public void handlePlayer(String oldName, boolean oldIsSpectator,
            int oldSeatNum);

    /**
     * A seat change.
     * 
     * This event occurs when a seat change occurs. The old seat (a Seat) is
     * passed as the event's data. The seat information will be updated before
     * the event is invoked.
     * 
     * @param oldSeat
     *            Seat information at that seat number.
     */
    public void handleSeat(Seat oldSeat);

    /**
     * Invoked when a spectator leaves or joins. The seat name will be null when
     * a spectator leaves and will contain the name of the spectator on join.
     * 
     * @param oldSeat
     *            the old seat.
     * @param newSeat
     *            the new seat.
     */
    public void handleSpectatorSeat(SpectatorSeat old_seat,
            SpectatorSeat newSeat);

    /**
     * A chat message event.
     * 
     * This event occurs when we receive a chat. The chat may have originated in
     * another game client or from the GGZ client; in either case it will be
     * routed to us. The chat information is passed as the event's data. Note
     * that the chat may originate with a player or a spectator, and they may
     * have changed seats or left the table by the time it gets to us.
     */
    public void handleChat(String player, String msg);

    /** Players' stats have been updated. */
    public void handleStats();

    /**
     * Player information has arrived.
     * 
     * Information has been requested about one or more players and it has now
     * arrived. The event data is a PlayerInfo object or null if info about all
     * players was requested.
     */
    public void handleInfo(PlayerInfo info);

    /**
     * An error has occurred This event occurs when a GGZMod error has occurred.
     * An error message will be passed as the event's data. GGZMod may attempt
     * to recover from the error, but it is not guaranteed that the GGZ
     * connection will continue to work after an error has happened.
     */
    public void handleError(String msg);
}
