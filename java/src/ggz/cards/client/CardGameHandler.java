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
import ggz.cards.common.CardSetType;
import ggz.cards.common.GGZCardInputStream;
import ggz.common.SeatType;

import java.io.IOException;
import java.net.Socket;

public interface CardGameHandler {
    public void alert_server(Socket fd);

    public void set_text_message(String mark, String message);

    /**
     * Called when either "Last Trick" or "Previous Hand" message is received
     * from the server.
     * 
     * @param mark
     *            One of either "Last Trick" or "Previous Hand".
     * @param cardlist
     *            Cards in the last trick or hand. The first index is the player
     *            index, the second the card.
     */
    public void set_cardlist_message(String mark, Card[][] cardlist);

    public void set_player_message(int player, String message);

    /**
     * Called when game-specific data is received by the client. The client will
     * skip over any remaining bytes that were not read by the handler.
     * 
     * @param in
     * @param gametype
     * @throws IOException
     */
    public void handle_game_message(GGZCardInputStream in, String gametype)
            throws IOException;

    public void alert_newgame(CardSetType cardset_type);

    public void handle_gameover(Player[] winners);

    public void alert_player(int i, SeatType old_type, String old_name);

    public void alert_num_players(int numplayers, int old_numplayers);

    public void alert_hand_size(int max_hand_size);

    public void display_hand(int player);

    public void get_bid(Bid[] bid_choices, String[] bid_texts,
            String[] bid_descs);

    public void alert_bid(int bidder, Bid bid);

    public void get_play(int play_hand, Card[] valid_cards);

    public void alert_badplay(String err_msg);

    public void alert_play(int player, Card card);

    /**
     * A table message tells you all the cards on the table. Each player only
     * gets one card.
     */
    public void alert_table();

    /** A trick message tells you about the end of a trick (and who won). */
    public void alert_trick(int winner);

    /**
     * An options request asks you to pick a set of options. Each "option" gives
     * a list of choices so that you pick one choice for each option. An option
     * with only one choice is a special case: a boolean option.
     */
    public boolean get_options(String[] types, String[] descs, int[] defaults,
            String[][] option_choices) throws IOException;

    /**
     * Server has requested the client to OK a new game. This is sent at the
     * start of a new game before the game begins.
     */
    public void get_newgame();

    public void alert_newhand();

    /** Called when each team's score is received from the server. */
    public void alert_scores(int hand_num);

    /**
     * Called when the number of tricks that a player has won is received from
     * the server.
     */
    public void alert_tricks_count();

    /** Called when the socket disconnects. */
    public void handle_disconnect();

    public void alert_state(Client.GameState oldState, Client.GameState newState);

    /**
     * Called when the status of one or more players has changed. The Players
     * bidding and playing property will be true for each player that is either
     * playing or bidding respectively.
     */
    public void alert_players_status();
}
