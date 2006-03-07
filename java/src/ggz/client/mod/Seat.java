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

import ggz.common.SeatType;

/**
 * @brief A seat at a GGZ game table.
 * 
 * Each seat at the table is tracked like this.
 */
public class Seat {
    /** Seat index; 0..(num_seats-1). */
    int num;

    /** Type of seat. */
    SeatType type;

    /** Name of player occupying seat. */
    String name;

    public Seat(int num, SeatType type, String name) {
        this.num = num;
        this.type = type;
        this.name = name;
    }
    
    public int get_num() {
        return num;
    }
    
    public String get_name() {
        return name;
    }
    
    public SeatType get_type() {
        return type;
    }

    public boolean equals(Object o) {
        return (o != null)
                && (o instanceof Seat)
                && this.num == ((Seat) o).num
                && this.type == ((Seat) o).type
                && (this.name == ((Seat) o).name || (this.name != null && this.name
                        .equals(((Seat) o).name)));
    }

    /**
     * This is here only because the Java API says we should implement
     * hashCode() if we implement equals().
     */
    public int hashCode() {
        return this.num;
    }
}
