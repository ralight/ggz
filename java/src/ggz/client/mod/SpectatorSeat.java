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

/**
 * @brief A game spectator entry.
 * 
 * Spectators are kept in their own table. A spectator seat is occupied if it
 * has a name, empty if the name is NULL.
 */
public class SpectatorSeat {
    /** Spectator seat index */
    private int num;

    /** The spectator's name (NULL => empty) */
    private String name;

    /** Statistics about this player. */
    private Stat stat;

    public SpectatorSeat(int num, String name) {
        this.num = num;
        this.name = name;
    }

    void setStat(Stat stat) {
        this.stat = stat;
    }

    public int getNum() {
        return this.num;
    }

    public String getName() {
        return this.name;
    }

    public Stat getStat() {
        return this.stat;
    }

    public boolean equals(Object o) {
        return (o != null) && (o instanceof SpectatorSeat)
                && this.num == ((SpectatorSeat) o).num;
    }

    /**
     * This is here only because the Java API says we should implement
     * hashCode() if we implement equals().
     */
    public int hashCode() {
        return this.num;
    }

    public String toString() {
        return this.name;
    }
}
