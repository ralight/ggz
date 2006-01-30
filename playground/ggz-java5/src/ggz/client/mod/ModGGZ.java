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
import java.lang.reflect.InvocationTargetException;
import java.net.Socket;
import java.util.List;

public interface ModGGZ {
    public void set_module(String pwd, String class_name);

    public ModState get_state();

    public void add_mod_listener(ModTransactionHandler handler);

    public void set_player(String name, boolean is_spectator, int seat_num);

    public void connect() throws InstantiationException,
            IllegalAccessException, InvocationTargetException,
            NoSuchMethodException, ClassNotFoundException, IOException;

    public void disconnect() throws IOException;

    public void inform_chat(String player, String msg);

    public void set_seat(Seat seat);

    public void set_spectator_seat(SpectatorSeat seat);

    public void set_info(int num, List infos);

    public void set_server_fd(Socket fd) throws IOException;
}