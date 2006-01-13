package ggz.cards.common;

/**
 * @brief The type of deck.
 * 
 * In theory, many different types of decks are possible. Currently only the
 * standard French deck is used.
 */
public enum CardSetType {
    UNKNOWN_CARDSET, // = -1,

    /** A standard (French) card deck (A/K/Q/J/10..2). */
    CARDSET_FRENCH,

    /**
     * A set of dominoes. Here each domino has two sides, each of which has a
     * number. For storage purposes, the higher side will be considered the
     * "suit" and the lower side the "face".
     */
    CARDSET_DOMINOES;

    /**
     * Decodes the value sent from the server. C enum cardset_type.
     * @param i
     * @return
     */
    public static CardSetType valueOf(int i) {
        return values()[i + 1];
    }
}
