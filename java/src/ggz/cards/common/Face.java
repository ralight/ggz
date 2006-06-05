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

import java.util.ArrayList;

/**
 * Regular values for card faces.
 * 
 * @note If a player does not know the card face, UNKNOWN will be sent.
 * @note Values 2-10 have their face value.
 * @see card_t::face
 * @note This only applies for French (standard) decks.
 */
public class Face {
    private String name;

    public static final ArrayList values = new ArrayList();

    /** An unknown face on a card */
    public static final Face UNKNOWN_FACE = new Face("Unknown"); // = -1,

    /** An "ace" (low) card face */
    public static final Face ACE_LOW = new Face("Ace"); // = 1,

    public static final Face DEUCE = new Face("Deuce");

    public static final Face THREE = new Face("Three");

    public static final Face FOUR = new Face("Four");

    public static final Face FIVE = new Face("Five");

    public static final Face SIX = new Face("Six");

    public static final Face SEVEN = new Face("Seven");

    public static final Face EIGHT = new Face("Eight");

    public static final Face NINE = new Face("Nine");

    public static final Face TEN = new Face("Ten");

    /** A "jack" card face */
    public static final Face JACK = new Face("Jack"); // = 11,

    /** A "queen" card face */
    public static final Face QUEEN = new Face("Queen"); // = 12,

    /** A "king" card face */
    public static final Face KING = new Face("King"); // = 13,

    /** An "ace" (high) card face */
    public static final Face ACE_HIGH = new Face("Ace"); // = 14,

    /** A "joker 1" card face */
    public static final Face JOKER1 = new Face("Joker"); // = 0,

    /** A "joker 2" card face */
    public static final Face JOKER2 = new Face("Joker"); // = 1

    private Face(String name) {
        this.name = name;
        values.add(this);
    }

    public int ordinal() {
        return values.indexOf(this);
    }

    public static Face[] values() {
        return (Face[]) values.toArray(new Face[values.size()]);
    }

    public static Face valueOf(int ordinal) {
        return (Face) values.get(ordinal);
    }

    public String toString() {
        return name;
    }
}
