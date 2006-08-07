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
import ggz.common.SeatType;

/**
 * A seat at a GGZ game table.
 * 
 * Each seat at the table is tracked like this.
 */
public class Seat {
    /** Seat index; 0..(num_seats-1). */
    private int num;

    /** Type of seat. */
    private SeatType type;

    /** Name of player occupying seat. */
    private String name;

    /** Statistics about this player. */
    private Stat stat;

    /** Extra information about the player. */
    private PlayerInfo info;

    public Seat(int num, SeatType type, String name) {
        this.num = num;
        this.type = type;
        this.name = name;
    }

    void setStat(Stat stat) {
        this.stat = stat;
    }

    void setInfo(PlayerInfo info) {
        this.info = info;
    }

    public int getNum() {
        return this.num;
    }

    public String getName() {
        return this.name;
    }

    public SeatType getType() {
        return this.type;
    }

    public Stat getStat() {
        return this.stat;
    }

    /**
     * Gets extra information about this player. This will be null unless
     * {@link ModGame#requestPlayerInfo(int)} has been called.
     * 
     * @return the PlayerInfo object.
     */
    public PlayerInfo getInfo() {
        return this.info;
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

    public String toString() {
        return "Seat: num=" + this.num + ", name=" + this.name + ", type="
                + this.type;
    }
}
