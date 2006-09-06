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

/** Messages from server */
public class ServerOpCode {

    private static final ArrayList values = new ArrayList();

    private final String mnemonic;

    /**
     * Requests a newgame response (RSP_NEWGAME) from the client. No data.
     */
    public static final ServerOpCode REQ_NEWGAME = new ServerOpCode(
            "REQ_NEWGAME");

    /**
     * Tells the client of a start of a new game. Followed by a string
     * containing the game type, and an integer containing the cardset type.
     */
    public static final ServerOpCode MSG_NEWGAME = new ServerOpCode(
            "MSG_NEWGAME");

    /**
     * Tells the client of a gameover situation. It'll be followed by an integer
     * n followed by a list of n players who are the winners of the game.
     */
    public static final ServerOpCode MSG_GAMEOVER = new ServerOpCode(
            "MSG_GAMEOVER");

    /**
     * Tells the client the list of seats. It'll be followed by an integer
     * containing the number of seats, one containing the number of players, and
     * one containing the number of teams, followed by data for all seats. For
     * each seat, an integer giving the seat status (GGZSeat), a string for the
     * seats's name, and integer for the player sitting at the seat, and and
     * integer for the team of that player will be sent. Note that the names may
     * be invalid or empty for OPEN seats.
     */
    public static final ServerOpCode MSG_PLAYERS = new ServerOpCode(
            "MSG_PLAYERS");

    /**
     * Tells the client scores. Consists of the hand number (between 0 and the
     * current hand), an integer containing the number of "extra" scores (0 or
     * higher), and one score per team. Each score consists of an integer for
     * the main score plus one integer for each extra score.
     */
    public static final ServerOpCode MSG_SCORES = new ServerOpCode("MSG_SCORES");

    /**
     * Tells the client tricks. Consists of one integer per player that gives
     * the number of tricks won by that player (only applicable in some games).
     */
    public static final ServerOpCode MSG_TRICKS_COUNT = new ServerOpCode(
            "MSG_TRICKS_COUNT");

    /**
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
    public static final ServerOpCode REQ_OPTIONS = new ServerOpCode(
            "REQ_OPTIONS");

    /**
     * Tells the client a new hand is starting. It is followed by an single
     * integer containing the hand number (hand counting starts at 0), then by
     * the 'dealer' seat.
     */
    public static final ServerOpCode MSG_NEWHAND = new ServerOpCode(
            "MSG_NEWHAND");

    /** Contains a single byte that tells the trump suit. */
    public static final ServerOpCode MSG_TRUMP = new ServerOpCode("MSG_TRUMP");

    /**
     * Tells the client of a player's hand. It'll be followed by a seat # for
     * whom the hand belongs to, followed by an integer n for the hand size,
     * followed by n cards.
     */
    public static final ServerOpCode MSG_HAND = new ServerOpCode("MSG_HAND");

    /**
     * Tells the client the status of each player. This consists of two
     * integers, each of which is a bit vector containing one bit per player.
     * The first integer has bits set if the player is bidding, the second if
     * the player is playing.
     */
    public static final ServerOpCode MSG_PLAYERS_STATUS = new ServerOpCode(
            "MSG_PLAYERS_STATUS");

    /**
     * Requests a bid from the client. It'll be followed by an integer n, then n
     * bid choices. Each bid choice consists of the bid itself (see
     * read_bid/write_bid) and two strings: a short bid text, and a longer bid
     * description. The client must choose one of these bids and send a RSP_BID
     * in response.
     */
    public static final ServerOpCode REQ_BID = new ServerOpCode("REQ_BID");

    /**
     * Tells the client of a player's bid. It is followed by a seat # for the
     * seat from which the bid comes, then the bid itself.
     */
    public static final ServerOpCode MSG_BID = new ServerOpCode("MSG_BID");

    /**
     * Requests a play (of a card) from the client. It'll be followd by the seat #
     * of the hand from which the client is supposed to play (only a few games,
     * like bridge, ever require a player to play from a hand that is not their
     * own), then a number n, then a list of n valid cards. The client should
     * send a RSP_PLAY in response.
     */
    public static final ServerOpCode REQ_PLAY = new ServerOpCode("REQ_PLAY");

    /**
     * Tells the player that they have made a bad play. It is followed by a
     * string that is the error message. It needs a RSP_PLAY in response.
     */
    public static final ServerOpCode MSG_BADPLAY = new ServerOpCode(
            "MSG_BADPLAY");

    /**
     * Tells the client of a player playing a card onto the table. It is
     * followed by a seat # for the seat from which the card has been played,
     * then the card that was played from their hand.
     */
    public static final ServerOpCode MSG_PLAY = new ServerOpCode("MSG_PLAY");

    /**
     * Tells the client of the end of a trick. It is followed by a seat # giving
     * the player who won the trick.
     */
    public static final ServerOpCode MSG_TRICK = new ServerOpCode("MSG_TRICK");

    /**
     * Sends out the current table to the client. It is followed by a card for
     * each player, in order. Note that only one card per player/seat may be on
     * the table at a time.
     */
    public static final ServerOpCode MSG_TABLE = new ServerOpCode("MSG_TABLE");

    /**
     * A simple text message, containing two strings: a "marker" and the
     * "message".
     */
    public static final ServerOpCode MSG_GAME_MESSAGE_TEXT = new ServerOpCode(
            "MSG_GAME_MESSAGE_TEXT");

    /**
     * A text message for the player, consisting of a seat # followed by a
     * message string.
     */
    public static final ServerOpCode MSG_GAME_MESSAGE_PLAYER = new ServerOpCode(
            "MSG_GAME_MESSAGE_PLAYER");

    /**
     * A list of cards for each player. First comes a "marker" string, then (for
     * each player) an integer n plus n cards for that player.
     */
    public static final ServerOpCode MSG_GAME_MESSAGE_CARDLIST = new ServerOpCode(
            "MSG_GAME_MESSAGE_CARDLIST");

    /**
     * Block data that may be game-specific. It is up to the client frontend to
     * determine what (if anything) to do with this data; it'll be built at the
     * server end by the game module.
     */
    public static final ServerOpCode MSG_GAME_SPECIFIC = new ServerOpCode(
            "MSG_GAME_SPECIFIC");

    private ServerOpCode(String s) {
        values.add(this);
        this.mnemonic = s;
    }

    public static ServerOpCode valueOf(int ordinal) {
        return (ServerOpCode) values.get(ordinal);
    }

    public String toString() {
        return this.mnemonic;
    }
}
