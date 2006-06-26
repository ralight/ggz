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

public interface ModGame {
    public String get_player();

    public boolean is_spectator();

    public int get_seat_num();

    public Seat get_seat(int num);

    public int get_num_seats();

    public int get_num_spectator_seats();

    public SpectatorSeat get_spectator_seat(int num);

    public ModState get_state();

    public void set_state(ModState state) throws IOException;

    public void request_stand() throws IOException;

    public void request_sit(int seat_num) throws IOException;

    public void request_boot(String name) throws IOException;

    public void request_bot(int seat_num) throws IOException;

    public void request_open(int seat_num) throws IOException;

    public void request_chat(ChatType type, String recipient, String chat_msg) throws IOException;

    public void player_request_info(int seat_num) throws IOException;

    // public void player_get_info(int seat_num);
    public void add_mod_event_handler(ModEventHandler handler);
}