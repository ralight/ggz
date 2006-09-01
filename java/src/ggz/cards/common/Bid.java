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

    public static final Bid BRIDGE_PASS = new Bid(Bid.NO_VALUE, Suit.NO_SUIT, 1, 0);

    public static final Bid BRIDGE_DOUBLE = new Bid(Bid.NO_VALUE, Suit.NO_SUIT, 2, 0);

    public static final Bid BRIDGE_REDOUBLE = new Bid(Bid.NO_VALUE, Suit.NO_SUIT, 3, 0);

    public static final Bid BRIDGE_7_NT = new Bid(7, Suit.NO_TRUMP, 0, 0);

    public static final Bid SPADES_SHOW_CARDS = new Bid(Bid.NO_VALUE,
            Suit.NO_SUIT, 2, 0);

    /*
     * the value of the bid this can be used for many different games that have
     * unusual but similar bidding. Different games may use it differently.
     */
    private int val;

    /* the suit of the bid (generally trump) */
    private Suit suit;

    /* specialty bids (defined per-game) */
    private int spec;

    /* More specialty bids (just to round things out) */
    private int spec2;

    /** Specifies that the bid has no value (e.g. Nil Bid). */
    public static final int NO_VALUE = -1;

    public Bid(int value) {
        this(value, Suit.NO_SUIT, 0, 0);
    }

    public Bid(int value, Suit suit) {
        this(value, suit, 0, 0);
    }

    public Bid(int value, Suit suit, int spec, int spec2) {
        this.val = value;
        this.suit = suit;
        this.spec = spec;
        this.spec2 = spec2;
    }

    public int getVal() {
        return val;
    }

    public Suit getSuit() {
        return suit;
    }

    public int getSpec() {
        return spec;
    }

    public int getSpec2() {
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
