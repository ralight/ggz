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

import ggz.common.SeatType;

public class TableSeat {
    /* Seat index */
    public int index;

    /*
     * Type of player in seat. Used for regular seats only; spectator seats just
     * ignore it.
     */
    public SeatType type;

    /*
     * Player's name; or NULL if none. An empty spectator seat will have no
     * name.
     */
    public String name;

    public TableSeat() {
        super();
    }
    
    public TableSeat(int index, SeatType type, String name) {
        this.index = index;
        this.type = type;
        this.name = name;
    }
}
