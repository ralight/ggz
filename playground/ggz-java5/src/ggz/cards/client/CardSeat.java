package ggz.cards.client;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;

import ggz.cards.common.Card;
import ggz.common.SeatType;

/**
 * A "player" as seen by the client is really just a seat at the table, complete
 * with a hand and everything. It's not necessary that a player actually be
 * sitting at that seat - there may just be a pile of cards.
 * 
 * @note We are always player 0. The server will deal with all conversions.
 *       Contains all information about a seat at the table.
 */
public class CardSeat {

    /** ggz seating assignment info */
    SeatType status;

    /** player's name */
    String name;

    /** GGZ seat number (or -1) */
    int ggzseat;

    /**
     * @brief Player's card on the table.
     * 
     * This variable is maintained by the core client-common code, and should
     * not be modified by the GUI table code. It will always hold the card known
     * to be on the table for the player, i.e. basically the same information
     * that the server knows (although with a bit of lag). Note that this need
     * not correspond with what is actually drawn by the GUI.
     */
    Card table_card;

    /** player's hand */
    ArrayList<Card> hand;

    // /** @brief The size of the uncompressed hand.
    // *
    // * The player's "hand" is stored in the hand structure, above.
    // * But for the convenience of certain GUI clients, separate
    // * hand data is also tracked. Here the u_hand_size is the
    // * total number of entries in the u_hand array. Each entry
    // * in the u_hand array is marked as either valid or invalid.
    // * Valid ones correspond to cards in the hand, invalid ones
    // * are cards that have already been played. (The "u" stands
    // * for "uncollapsed", I suppose.) */
    // int u_hand_size;
    //
    // /** @see u_hand_size */
    // struct {
    // bool is_valid;
    // card_t card;
    // } *u_hand;

    public String get_name() {
        return this.name;
    }

    public Collection<Card> get_hand() {
        return Collections.unmodifiableCollection(this.hand);
    }
    
    public Card get_table_card() {
        return table_card;
    }
}
