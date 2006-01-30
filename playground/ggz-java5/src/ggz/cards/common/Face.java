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

/**
 * Regular values for card faces.
 * 
 * @note If a player does not know the card face, UNKNOWN will be sent.
 * @note Values 2-10 have their face value.
 * @see card_t::face
 * @note This only applies for French (standard) decks.
 */
public enum Face {
    /** An unknown face on a card */
    UNKNOWN_FACE, // = -1,
    /** An "ace" (low) card face */
    ACE_LOW, // = 1,
    DEUCE, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN,
    /** A "jack" card face */
    JACK, // = 11,
    /** A "queen" card face */
    QUEEN, // = 12,
    /** A "king" card face */
    KING, // = 13,
    /** An "ace" (high) card face */
    ACE_HIGH, // = 14,
    /** A "joker 1" card face */
    JOKER1, // = 0,
    /** A "joker 2" card face */
    JOKER2; // = 1
}
