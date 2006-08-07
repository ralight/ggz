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

import ggz.common.ChatType;

import java.io.IOException;
import java.util.EventListener;

public interface ModTransactionHandler extends EventListener {
    /** Sit down (stop spectatin; join a seat) */
    public void handle_sit(int seat_num) throws IOException;

    /** Stand up (leave your seat; become a spectator) */
    public void handle_stand() throws IOException;

    /** Boot a player */
    public void handle_boot(String name) throws IOException;

    /** Replace a bot/reserved seat with an open one. */
    public void handle_open(int seat_num) throws IOException;

    /** Put a bot into an open seat. */
    public void handle_bot(int seat_num) throws IOException;

    /** A chat originating from the game client. */
    public void handle_chat(ChatType type, String recipient, String chat)
            throws IOException;

    /** Information about one or more players. */
    public void handle_info(int seat_num) throws IOException;

    /** The game client has notified us of a state change. */
    public void handle_state(ModState prev) throws IOException;

    /** There was an error communicating with the game client. */
    public void handle_error(Throwable exception);
}
