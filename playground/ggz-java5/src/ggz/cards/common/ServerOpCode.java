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

/** Messages from server */
public enum ServerOpCode {

    /*
     * Requests a newgame response (RSP_NEWGAME) from the client. No data.
     */
    REQ_NEWGAME,

    /*
     * Tells the client of a start of a new game. Followed by an integer
     * containing the cardset type.
     */
    MSG_NEWGAME,

    /*
     * Tells the client of a gameover situation. It'll be followed by an integer
     * n followed by a list of n players who are the winners of the game.
     */
    MSG_GAMEOVER,

    /*
     * Tells the client the list of players. It'll be followd by an integer n
     * followed by data for all n players. For each player, an integer giving
     * the seat status (GGZSeat) and a string for the player's name will be
     * sent. Note that the names may be invalid (but never null) for OPEN seats.
     */
    MSG_PLAYERS,

    /*
     * Requests options from the client. It is followed by an integer n followed
     * by n option requests. Each option request consists of a descriptive text
     * for the option, an integer m telling how many choices there are for this
     * option, and integer in the range 0..(m-1) giving the default option
     * choice, then m strings representing the option choices themselves. For
     * each of the n options, the client must choose one of the m choices and
     * send this back to the server with a RSP_OPTIONS. If m==1 then the option
     * is considered "boolean" and either a 0 or 1 may be sent. It needs a
     * RSP_OPTIONS in response.
     */
    REQ_OPTIONS,

    /* Tells the client a new hand is starting. No data. */
    MSG_NEWHAND,

    /*
     * Tells the client of a player's hand. It'll be followed by a seat # for
     * whom the hand belongs to, followed by an integer n for the hand size,
     * followed by n cards.
     */
    MSG_HAND,

    /*
     * Requests a play (of a card) from the client. It'll be followd by the seat #
     * of the hand from which the client is supposed to play (only a few games,
     * like bridge, ever require a player to play from a hand that is not their
     * own), then a number n, then a list of n valid cards. The client should
     * send a RSP_PLAY in response.
     */
    REQ_PLAY,

    /*
     * Tells the player that they have made a bad play. It is followed by a
     * string that is the error message. It needs a RSP_PLAY in response.
     */
    MSG_BADPLAY,

    /*
     * Tells the client of a player playing a card onto the table. It is
     * followed by a seat # for the seat from which the card has been played,
     * then the card that was played from their hand.
     */
    MSG_PLAY,

    /*
     * Tells the client of the end of a trick. It is followed by a seat # giving
     * the player who won the trick.
     */
    MSG_TRICK,

    /*
     * Requests a bid from the client. It'll be followed by an integer n, then n
     * bid choices. Each bid choice consists of the bid itself (see
     * read_bid/write_bid) and two strings: a short bid text, and a longer bid
     * description. The client must choose one of these bids and send a RSP_BID
     * in response.
     */
    REQ_BID,

    /*
     * Tells the client of a player's bid. It is followed by a seat # for the
     * seat from which the bid comes, then the bid itself.
     */
    MSG_BID,

    /*
     * Sends out the current table to the client. It is followed by a card for
     * each player, in order. Note that only one card per player/seat may be on
     * the table at a time.
     */
    MSG_TABLE,

    /* A game message. See game_message_t below. */
    MESSAGE_GAME;
}
