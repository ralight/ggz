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
    ArrayList hand;

    public String get_name() {
        return this.name;
    }

    public List get_hand() {
        return Collections.unmodifiableList(this.hand);
    }

    public Card get_table_card() {
        return table_card;
    }

    public SeatType get_seat_type() {
        return status;
    }
}
