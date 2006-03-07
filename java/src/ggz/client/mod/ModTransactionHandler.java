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
import java.util.EventListener;

public interface ModTransactionHandler extends EventListener {
    public void handle_state(ModState prev) throws IOException;

    public void handle_sit(int seat_num) throws IOException;

    public void handle_stand() throws IOException;

    public void handle_boot(String name) throws IOException;

    public void handle_seatchange(ModTransaction t, int seat_num) throws IOException;

    public void handle_chat(String chat) throws IOException;

    public void handle_info(int seat_num) throws IOException;

}
