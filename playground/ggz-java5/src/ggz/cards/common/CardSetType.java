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
 * The type of deck.
 * 
 * In theory, many different types of decks are possible. Currently only the
 * standard French deck is used.
 */
public class CardSetType {

	public static final ArrayList values = new ArrayList();

	public static final CardSetType UNKNOWN_CARDSET = new CardSetType(); // = -1,

    /** A standard (French) card deck (A/K/Q/J/10..2). */
    public static final CardSetType CARDSET_FRENCH = new CardSetType();

    /**
     * A set of dominoes. Here each domino has two sides, each of which has a
     * number. For storage purposes, the higher side will be considered the
     * "suit" and the lower side the "face".
     */
    public static final CardSetType CARDSET_DOMINOES = new CardSetType();

	private CardSetType() {
		values.add(this);
	}

    /**
     * Decodes the value sent from the server. C enum cardset_type.
     * @param i
     * @return
     */
	public static CardSetType valueOf(int i) {
		return (CardSetType) values.get(i + 1);
	}
}
