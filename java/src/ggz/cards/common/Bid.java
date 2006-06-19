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
package ggz.cards.common;

/*
 * In different games, bids may have different meanings. We'll just use this
 * arbitrary data structure for it
 */
public class Bid {
    /*
     * the value of the bid this can be used for many different games that have
     * unusual but similar bidding. Different games may use it differently.
     */
    private byte val;

    /* the suit of the bid (generally trump) */
    private int suit;

    /* specialty bids (defined per-game) */
    private byte spec;

    /* More specialty bids (just to round things out) */
    private byte spec2;

    public Bid(byte value) {
        this(value, -1, (byte) 0, (byte) 0);
    }

    public Bid(byte value, int suit) {
        this(value, suit, (byte) 0, (byte) 0);
    }

    public Bid(byte value, int suit, byte spec, byte spec2) {
        this.val = value;
        this.suit = suit;
        this.spec = spec;
        this.spec2 = spec2;
    }

    public byte getVal() {
        return val;
    }

    public int getSuit() {
        return suit;
    }

    public byte getSpec() {
        return spec;
    }

    public byte getSpec2() {
        return spec2;
    }

    public String toString() {
        return "val=" + val + ", suit=" + suit + ", spec=" + spec + ", spec2="
                + spec2;
    }

    public boolean equals(Object o) {
        return (o != null) && (o instanceof Bid) && this.val == ((Bid) o).val
                && this.suit == ((Bid) o).suit && this.spec == ((Bid) o).spec
                && this.spec2 == ((Bid) o).spec2;
    }

    /**
     * This is here only because the Java API says we should implement
     * hashCode() if we implement equals().
     */
    public int hashCode() {
        return this.val;
    }
}
