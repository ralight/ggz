package ggz.cards.common;

/* in different games, bids may have different meanings.  we'll just use this
 arbitrary data structure for it */
public class Bid {
    /*
     * the value of the bid this can be used for many different games that have
     * unusual but similar bidding. Different games may use it differently.
     */
    byte val;

    /* the suit of the bid (generally trump) */
    int suit;

    /* specialty bids (defined per-game) */
    byte spec;

    /* More specialty bids (just to round things out) */
    byte spec2;

    Bid(byte value, int suit, byte spec, byte spec2) {
        this.val = value;
        this.suit = suit;
        this.spec = spec;
        this.spec2 = spec2;
    }
}
