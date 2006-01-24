package ggz.cards.client;

import ggz.cards.common.Bid;
import ggz.cards.common.Card;
import ggz.cards.common.CardSetType;
import ggz.cards.common.GGZCardInputStream;
import ggz.client.mod.ModEventHandler;
import ggz.common.SeatType;

import java.io.IOException;
import java.net.Socket;
import java.util.List;

public interface CardGameHandler extends ModEventHandler {
    public void alert_server(Socket fd);

    public void set_text_message(String mark, String message);

    public void set_cardlist_message(String mark, List<Card[]> cardlist);

    public void set_player_message(int player, String message);

    public int handle_game_message(GGZCardInputStream in, String game, int size);

    public void alert_newgame(CardSetType cardset_type);

    public void handle_gameover(int[] winners);

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
    public boolean get_options(String[] descs, int[] defaults,
            String[][] option_choices) throws IOException;

    public void get_newgame();

    public void alert_newhand();

    /** Called when the socket disconnects. */
    public void handle_disconnect();

}
