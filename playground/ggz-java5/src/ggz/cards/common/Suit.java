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
