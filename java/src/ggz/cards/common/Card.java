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

/** A card. */
public class Card {

    public static final Card UNKNOWN_CARD = new Card(Face.UNKNOWN_FACE,
            Suit.UNKNOWN_SUIT, (byte) -1);

    /**
     * The face of the card.
     * 
     * @see Face
     */
    private Face face;

    /**
     * The suit of the card.
     * 
     * @see Suit
     */
    private Suit suit;

    /**
     * The deck number of the card.
     * 
     * @see card_deck_enum
     */
    byte deck;

    public Card(Face face, Suit suit) {
        this(face, suit, (byte) -1);
    }

    Card(Face face, Suit suit, byte deck) {
        this.face = face;
        this.suit = suit;
        this.deck = deck;
    }

    public Suit getSuit() {
        return this.suit;
    }

    public Face getFace() {
        return this.face;
    }

    /**
     * Determines whether it's known what suit and face this card is. The server
     * only notifies us the suit and face if we can see it, hence it will be
     * "face up".
     * 
     * @return true iff face != UNKNOWN_FACE and suit != UNKNOWN_SUIT.
     */
    public boolean isFaceUp() {
        return this.face != Face.UNKNOWN_FACE && this.suit != Suit.UNKNOWN_SUIT;
    }

    public boolean equals(Object o) {
        if (o instanceof Card) {
            Card card2 = (Card) o;
            return this.suit == card2.suit && this.face == card2.face;
        }
        return false;
    }

    /**
     * Taken from the example on p.39 of Effective Java by Joshua Bloch. A
     * better solution would be not to implement equals() and hashCode() and to
     * instead make the constructors private and provide a static getCard()
     * method that returns the appropriate card from a static set of cards since
     * cards are a finite set. However, we also have to think about what happens
     * when there are two decks in the game and what that means for equality.
     */
    public int hashCode() {
        int result = 17;
        result = 37 * result + this.suit.ordinal();
        result = 37 * result + this.face.ordinal();
        result = 37 * result + this.deck;
        return result;
    }

    public String toString() {
        return this.face + " of " + this.suit;
    }
}
