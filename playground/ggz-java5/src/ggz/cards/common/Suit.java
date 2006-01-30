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
 * Regular values for card suits.
 * 
 * @note If a player does not know the card suit, UNKNOWN will be sent.
 * @see Card.get_suit()
 */
public enum Suit {
    /** An unknown suit of a card */
    UNKNOWN_SUIT, // = -1,
    /** The clubs (lowest) suit */
    CLUBS, // = 0,
    /** The diamonds (second) suit */
    DIAMONDS, // = 1,
    /** The hearts (third) suit */
    HEARTS, // = 2,
    /** The spades (highest) suit */
    SPADES, // = 3,
    /** A no-suit used for jokers, etc. */
    NO_SUIT, // = 4;
}
