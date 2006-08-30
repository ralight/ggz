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
package ggz.cards.client;

import ggz.cards.common.Bid;
import ggz.cards.common.Card;
import ggz.common.SeatType;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * A "player" as seen by the client is really just a seat at the table, complete
 * with a hand and everything. It's not necessary that a player actually be
 * sitting at that seat - there may just be a pile of cards.
 * 
 * @note We are always player 0. The server will deal with all conversions.
 *       Contains all information about a seat at the table.
 */
public class Player {

    /** ggz seating assignment info */
    SeatType status;

    /** player's name */
    String name;

    /** GGZ seat number (or -1) */
    int ggzseat;

    /** For games that support teams (or partnerships). */
    int team;

    /**
     * @brief Player's card on the table.
     * 
     * This variable is maintained by the core client-common code, and should
     * not be modified by the GUI table code. It will always hold the card known
     * to be on the table for the player, i.e. basically the same information
     * that the server knows (although with a bit of lag). Note that this need
     * not correspond with what is actually drawn by the GUI.
     */
    Card tableCard;

    /** player's hand */
    ArrayList hand;

    /** Bid player made for the current hand. */
    Bid bid;

    /** true if the player is currently bidding. */
    boolean bidding;

    /** true if the player is currently playing a card. */
    boolean playing;

    /** The number of tricks this player has won this hand. */
    int tricks;

    public String getName() {
        return this.name;
    }

    public List getHand() {
        return Collections.unmodifiableList(this.hand);
    }

    public Card getTableCard() {
        return this.tableCard;
    }

    public SeatType getSeatType() {
        return this.status;
    }

    public Bid getBid() {
        return this.bid;
    }

    public int getTeam() {
        return this.team;
    }

    public boolean isBidding() {
        return this.bidding;
    }

    public boolean isPlaying() {
        return this.playing;
    }
    
    public int getTricks() {
        return this.tricks;
    }

    /**
     * The seat number as recorded by GGZ. Game seat numbers are from the
     * perspective of each client, this give the "absolute" number and is the
     * number that should be used for reseat requests.
     * 
     * @return
     */
    public int getGGZSeatNum() {
        return this.ggzseat;
    }
}
