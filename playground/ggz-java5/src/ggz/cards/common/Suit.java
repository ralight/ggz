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
 * Regular values for card suits.
 * 
 * @note If a player does not know the card suit, UNKNOWN will be sent.
 * @see Card.get_suit()
 */
public class Suit {

	public static final ArrayList values = new ArrayList();
	
	/** An unknown suit of a card */
	public static final Suit UNKNOWN_SUIT = new Suit(); // = -1,

	/** The clubs (lowest) suit */
	public static final Suit CLUBS = new Suit(); // = 0,

	/** The diamonds (second) suit */
	public static final Suit DIAMONDS = new Suit(); // = 1,

	/** The hearts (third) suit */
	public static final Suit HEARTS = new Suit(); // = 2,

	/** The spades (highest) suit */
	public static final Suit SPADES = new Suit(); // = 3,

	/** A no-suit used for jokers etc. */
	public static final Suit NO_SUIT = new Suit(); // = 4;

	private Suit() {
		values.add(this);
	}

	public int ordinal() {
		return values.indexOf(this);
	}
	
	public static Suit[] values() {
		return (Suit[]) values.toArray(new Suit[values.size()]);
	}

	public static Suit valueOf(int ordinal) {
		return (Suit) values.get(ordinal);
	}
}
