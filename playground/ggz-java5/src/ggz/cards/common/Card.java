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

    public Suit get_suit() {
        return this.suit;
    }

    public Face get_face() {
        return this.face;
    }

    public boolean equals(Object o) {
        Card card2 = (Card) o;
        return this.suit == card2.suit && this.face == card2.face;
    }

    public String toString() {
        return this.face + ":" + this.suit;
    }
}
