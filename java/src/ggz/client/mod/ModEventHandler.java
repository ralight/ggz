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

import java.io.IOException;
import java.net.Socket;
import java.util.List;

public interface ModEventHandler {
    public void init(ModGame mod) throws IOException;

    public void handle_launch() throws IOException;

    public void handle_server_fd(Socket fd) throws IOException;

    public void handle_chat(String player, String msg);

    /**
     * Invoked to inform about the result of an action such as Boot, Reseat etc.
     * 
     * @param msg
     */
    public void handle_result(String msg);

    public void handle_info(int num, List infos);

    /**
     * Invoked when information about the current player changes. e.g. When
     * standing the player changes to a spectator or when changing seats the
     * seat number changes.
     * 
     * @param name
     * @param is_spectator
     * @param seat_num
     */
    public void handle_player(String name, boolean is_spectator, int seat_num);

    /**
     * Invoked when in formation about the seat changes.
     * 
     * @param seat
     */
    public void handle_seat(Seat seat);

    /**
     * Invoked when a spectator leaves or joins. The seat name will be null when
     * a spectator leaves and will contain the name of the spectator on join.
     * 
     * @param seat
     */
    public void handle_spectator_seat(SpectatorSeat seat);
}
