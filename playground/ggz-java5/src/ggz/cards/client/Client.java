package ggz.cards.client;

import java.io.*;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.logging.Logger;

import ggz.cards.common.*;
import ggz.client.mod.*;
import ggz.common.SeatType;

public class Client implements ModEventHandler {
    private static final Logger log = Logger.getLogger(Client.class.getName());

    protected Socket fd;

    private GGZCardInputStream fd_in;

    private GGZCardOutputStream fd_out;

    private ModGame mod;

    private int max_hand_size;

    /** The game_t structure contains all global game data. */
    /** The number of players in the game. */
    private int num_players;

    /** Data about each player */
    private CardSeat[] players;

    /** The state the game is in */
    private ClientState state;

    /**
     * @brief The hand we're currently playing from.
     * 
     * This is the hand we're currently playing from, or -1 if we're not
     * currently playing. Note when a play response is sent to the server, it's
     * not reset to -1 until we get verification.
     */
    private int play_hand;

    protected CardGameHandler game;

    ModGame get_ggzmod() {
        return this.mod;
    }

    public int get_num_players() {
        return this.num_players;
    }

    public CardSeat get_nth_player(int n) {
        return this.players[n];
    }

    // int initialize()
    public Client(ModGame mod) {
        /*
         * A word on memory management: the client-common code uses ggz for
         * internal memory management; i.e. ggz_malloc+ggz_free. Anything
         * allocated with easysock will be allocated with malloc and must be
         * freed with free. The table code (which is gui-specific) may use
         * whatever memory management routines it wants (currently the GTK
         * client uses g_malloc and g_free). This may be unnecessarily
         * complicated, but remember that the internal client-common variables
         * are always kept separate from the GUI variables, so there should be
         * no confusion there. And all of the easysock-allocated variables are
         * labelled.
         */

        // srand((unsigned)time(null));
        // this.fd = -1;
        // this.max_hand_size = 0;
        this.state = ClientState.STATE_INIT;
        log.fine("Client initialized.");

        this.mod = mod;
        this.mod.add_mod_event_handler(this);
    }

    public void add_listener(CardGameHandler l) {
        if (this.game != null && l != this.game) {
            throw new UnsupportedOperationException(
                    "Multiple listeners not supported yet.");
        }
        this.game = l;
    }

    public void quit() throws IOException {
        /* Disconnect */
        mod.set_state(ModState.GGZMOD_STATE_DONE);
        // mod.disconnect();
        // ggz_error_msg_exit("Couldn't disconnect from ggz.");

        log.fine("Client disconnected.");
    }

    // Socket get_fd()
    // {
    // return this.fd;
    // }

    private void set_game_state(ClientState state) {
        if (state == this.state) {
            log.fine("Staying in state " + this.state + ".");
        } else {
            log
                    .fine("Changing state from " + this.state + " to " + state
                            + ".");
            this.state = state;
        }
    }

    private void handle_text_message() throws IOException {
        String mark = this.fd_in.read_string();
        String message = this.fd_in.read_string();
        game.set_text_message(mark, message);
    }

    private void handle_cardlist_message() throws IOException {
        ArrayList<Card[]> cardlist = new ArrayList<Card[]>(this.num_players);

        String mark = this.fd_in.read_string();

        for (int p = 0; p < this.num_players; p++) {
            int num_cards = this.fd_in.readInt();
            if (num_cards > 0) {
                Card[] cards = new Card[num_cards];
                for (int i = 0; i < num_cards; i++) {
                    cards[i] = this.fd_in.read_card();
                }
                cardlist.add(cards);
            }
        }

        game.set_cardlist_message(mark, cardlist);
    }

    /*
     * A message_player message tells you one "player message", which is
     * displayed by the client.
     */
    private void handle_player_message() throws IOException {
        int player = this.fd_in.read_seat();
        String message = this.fd_in.read_string();

        assert (player >= 0 && player < this.num_players);

        game.set_player_message(player, message);
    }

    /*
     * This handles a game-specific message. We pass the game all the
     * information about the message and let them read it in from the server
     * themselves. If they don't we do it manually just to get it out of the
     * way.
     */
    /*
     * One alternative would be to read the data in ourselves (the server would
     * have to send the data format, etc) and allow the game to query us about
     * what data we read. This would be safer but a lot more work. Or, if we
     * used an XML protocol, things could just sort-of take care of themselves
     * because we'd just skip over the tag automatically if it wasn't handled (I
     * think).
     */
    private void handle_game_message() throws IOException {
        int num_bytes_handled;
        String game_name = this.fd_in.read_string();
        int size = this.fd_in.readInt();

        /*
         * Note: "size" refers to the size of the data block, not including the
         * headers above.
         */
        log.fine("Received game message of size " + size + " for game "
                + game_name);

        num_bytes_handled = this.game.handle_game_message(this.fd_in,
                game_name, size);
        if (num_bytes_handled < 0) {
            throw new IllegalStateException();
        }
        if (num_bytes_handled > size) {
            throw new IllegalStateException(
                    "Too much game sepcific data was read");
        }
        size -= num_bytes_handled; /* this is how much was unread */

        if (size > 0) {
            /* We read the block just to get it out of the way. */
            byte[] block = new byte[size];
            if (fd_in.read(block) < 0) {
                throw new EOFException();
            }
        }
    }

    /*
     * a message_global message tells you one "global message", which is
     * displayed by the client.
     */
    private void handle_message_global() throws IOException {
        GameMessage op = fd_in.read_game_message();

        log.fine("Received " + op + " global message opcode.");

        switch (op) {
        case GAME_MESSAGE_TEXT:
            handle_text_message();
            break;
        case GAME_MESSAGE_CARDLIST:
            handle_cardlist_message();
            break;
        case GAME_MESSAGE_GAME:
            handle_game_message();
            break;
        case GAME_MESSAGE_PLAYER:
            handle_player_message();
            break;
        }
    }

    private void handle_msg_newgame() throws IOException {
        CardSetType cardset_type = fd_in.read_cardset_type();

        game.alert_newgame(cardset_type);
    }

    /* A gameover message tells you the game is over, and who won. */
    private void handle_msg_gameover() throws IOException {
        int num_winners = fd_in.readInt();
        int[] winners = new int[num_winners];

        assert (num_winners >= 0 && num_winners <= this.num_players);

        if (num_winners > 0) {
            winners = new int[num_winners];
        }

        for (int i = 0; i < num_winners; i++) {
            winners[i] = fd_in.read_seat();
        }

        game.handle_gameover(winners);
        set_game_state(ClientState.STATE_DONE);
    }

    /*
     * A players message tells you all the players (well, seats really) at the
     * table.
     */
    private void handle_msg_players() throws IOException {
        boolean different;
        int numplayers;
        int old_numplayers = this.num_players;

        /*
         * It is possible to have 0 players. At the begginning of a "general"
         * game, you don't know how many seats will be used yet so the number of
         * players is 0.
         */
        numplayers = fd_in.readInt();
        assert (numplayers >= 0);

        /* we may need to allocate memory for the players */
        different = (this.num_players != numplayers);

        /* reallocate the players, if necessary */
        if (different) {
            log.fine("get_players: (re)allocating this.players.");
            this.players = new CardSeat[numplayers];

            for (int p = 0; p < numplayers; p++) {
                /*
                 * At least for table_card, this initialization is very
                 * important.
                 */
                this.players[p] = new CardSeat();
                this.players[p].table_card = Card.UNKNOWN_CARD;
                this.players[p].name = null;
                this.players[p].hand = null;
            }

            /* this forces reallocating later */
            this.max_hand_size = 0;

            this.num_players = numplayers;

            /* Redesign the table, if necessary. */
            game.alert_num_players(numplayers, old_numplayers);
        }

        /* read in data about the players */
        for (int i = 0; i < numplayers; i++) {
            int type, ggzseat;
            SeatType old_type, new_type;
            String old_name, new_name;

            type = fd_in.readInt();
            new_name = fd_in.read_string();
            ggzseat = fd_in.readInt();
            new_type = SeatType.valueOf(type);

            old_name = this.players[i].name;
            old_type = this.players[i].status;

            this.players[i].status = new_type;
            this.players[i].name = new_name;
            this.players[i].ggzseat = ggzseat;

            if (old_type != new_type || old_name == null
                    || !old_name.equals(new_name)) {
                game.alert_player(i, old_type, old_name);
            }
        }

        /* TODO: should we need to enter a waiting state if players leave? */
    }

    /* Possibly increase the maximum hand size we can sustain. */
    private void increase_max_hand_size(int new_max_hand_size) {
        if (new_max_hand_size <= this.max_hand_size) /*
                                                         * no problem
                                                         */
            return;

        log.fine("Expanding max_hand_size to allow for " + new_max_hand_size
                + " cards (previously max was " + this.max_hand_size + ").");
        this.max_hand_size = new_max_hand_size;

        /* Let the table know how big a hand might be. */
        game.alert_hand_size(this.max_hand_size);

        for (int p = 0; p < this.num_players; p++) {
            if (this.players[p].hand == null) {
                this.players[p].hand = new ArrayList<Card>();
            }
            // /* This reallocates the hand to be larger, but leaves the
            // unused cards uninitialized. This should be acceptable. */
            // this.players[p].hand.cards =
            // ggz_realloc(this.players[p].hand.cards,
            // this.max_hand_size *
            // sizeof(*this.players[p].hand.cards));
            // this.players[p].u_hand =
            // ggz_realloc(this.players[p].u_hand,
            // this.max_hand_size
            // * sizeof(*this.players[p].u_hand));
        }
    }

    /* A hand message tells you all the cards in one player's hand. */
    private void handle_msg_hand() throws IOException {
        int player;
        int hand_size;
        ArrayList<Card> hand;

        assert (this.players != null);

        /* first read the player whose hand it is */
        player = fd_in.read_seat();
        assert (player >= 0 && player < this.num_players);

        /* Find out how many cards in this hand */
        hand_size = fd_in.readInt();

        /* Reallocate hand structures, if necessary */
        increase_max_hand_size(hand_size);

        hand = this.players[player].hand;
        /*
         * Read in all the card values. It's important that we don't change
         * anything before here so that any functions we call from
         * increase_max_hand_size won't have inconsistent data.
         */
        hand.clear();
        for (int i = 0; i < hand_size; i++) {
            Card card = fd_in.read_card();

            hand.add(card);

            // this.players[player].u_hand[i].card = card;
            // this.players[player].u_hand[i].is_valid = true;
        }

        log.fine("Received hand message for player " + player + "; "
                + hand.size() + " cards.");

        /* Finally, show the hand. */
        game.display_hand(player);
    }

    /* A bid request asks you to pick from a given list of bids. */
    private void handle_req_bid() throws IOException {
        int possible_bids;
        String[] bid_texts;
        String[] bid_descs;
        Bid[] bid_choices;

        if (this.state == ClientState.STATE_BID) {
            /*
             * The new bid request overrides the old one. But this means some
             * messy cleanup is necessary.
             */
            log.fine("WARNING: new bid message overriding old one.");
        }

        /* Determine the number of bidding choices we have */
        possible_bids = fd_in.readInt();
        bid_choices = new Bid[possible_bids];
        bid_texts = new String[possible_bids];
        bid_descs = new String[possible_bids];

        /* Read in all of the bidding choices. */
        for (int i = 0; i < possible_bids; i++) {
            bid_choices[i] = fd_in.read_bid();
            bid_texts[i] = fd_in.read_string();
            bid_descs[i] = fd_in.read_string();
        }

        /* Get the bid */
        set_game_state(ClientState.STATE_BID);
        game.get_bid(bid_choices, bid_texts, bid_descs);
    }

    private void handle_msg_bid() throws IOException {
        int bidder = fd_in.read_seat();
        Bid bid = fd_in.read_bid();

        game.alert_bid(bidder, bid);
    }

    /*
     * A play request asks you to play a card from any hand (most likely your
     * own).
     */
    private void handle_req_play() throws IOException {
        int num_valid_cards;
        Card[] valid_cards;

        /* Determine which hand we're supposed to be playing from. */
        this.play_hand = fd_in.read_seat();
        num_valid_cards = fd_in.readInt();

        valid_cards = new Card[num_valid_cards];
        for (int i = 0; i < num_valid_cards; i++) {
            valid_cards[i] = fd_in.read_card();
        }

        assert (this.play_hand >= 0 && this.play_hand < this.num_players);

        /* Get the play. */
        set_game_state(ClientState.STATE_PLAY);
        game.get_play(this.play_hand, valid_cards);
    }

    /* A badplay message indicates an invalid play, and requests a new one. */
    private void handle_msg_badplay() throws IOException {
        /* Read the error message for the bad play. */
        String err_msg = fd_in.read_string();

        /* Get a new play. */
        set_game_state(ClientState.STATE_PLAY);
        game.alert_badplay(err_msg);
    }

    /* returns an index into the hand's card for a match to the given card */
    private static int match_card(Card card, ArrayList<Card> hand) {
        int matches = -1, match = -1;
        /*
         * Anything "unknown" will match, as will the card itself. However, we
         * look for the greatest possible number of matches for the card.
         */
        /**
         * Consider the following case: hand contains: (1, 2, 3) and (-1, -1,
         * -1) card to match: (1, 2, 3) it matches (-1, -1, -1) so we go with
         * that match. later, card (2, 3, 4) becomes unmatchable (1, 2, 3) was
         * obviously the better match.
         */
        /*
         * TODO: It might be better to return -1 if there's more than one
         * possible match; that way we can get a re-sync and fix any potential
         * problems before they bother the player. OTOH, then my cool "squeeze"
         * comment below would no longer apply!
         */
        for (int tc = hand.size() - 1; tc >= 0; tc--) {
            /* TODO: look for a stronger match */
            Card hcard = hand.get(tc);
            int tc_matches = 0;

            if ((hcard.get_suit() != Suit.UNKNOWN_SUIT && hcard.get_suit() != card
                    .get_suit())
                    || (hcard.get_face() != Face.UNKNOWN_FACE && hcard
                            .get_face() != card.get_face()))
                continue;

            /*
             * we count the number of matches to make the best pick for the
             * match
             */
            if (hcard.get_suit() != Suit.UNKNOWN_SUIT)
                tc_matches++;
            if (hcard.get_face() != Face.UNKNOWN_FACE)
                tc_matches++;

            /*
             * If we have a greater number of matches, we should always
             * substitute. If we have an equal number of matches, in theory we
             * should pick a match at random. This algorithm is an approximation
             * of that; in the case where all matches are equally good it will
             * be a random pick.
             */
            if (tc_matches > matches || (tc_matches == matches
            /* && (rand() % (hand.size() + 1 - tc)) == 0 */)) {
                matches = tc_matches;
                match = tc;
            }
        }
        return match;
    }

    /* A play message tells of a play from a hand to the table. */
    private void handle_msg_play() throws IOException {
        int p, card_pos;
        Card card;
        ArrayList<Card> hand;

        /* Read the card being played. */
        p = fd_in.read_seat();
        card = fd_in.read_card();

        assert (p >= 0 && p < this.num_players);

        /* Reset the play_hand, just to be safe. */
        if (p == this.play_hand) {
            /*
             * We assume that if the hand the card came from is the hand we're
             * playing from, that it's our play. Thus there is an implicit
             * assumption that two players can't simultaneously play from the
             * same hand.
             */
            this.play_hand = -1;
        } else {
            /*
             * Either we're not playing, or someone else played during our play.
             * Both are possible.
             */
        }

        /*
         * Place the card on the table. Note, this contradicts what the table
         * code does, since that runs animation that may assume the card has not
         * yet been placed on the table. Nonetheless, this is the correct thing
         * to do - it is the table code that needs to be fixed (to keep separate
         * track of what cards are being _drawn_ on the table, as opposed to
         * what cards are _supposed to be_ on the table which is what this is).
         */
        this.players[p].table_card = card;

        /* Find the hand the card is to be removed from. */
        assert (this.players != null);
        hand = this.players[p].hand;
        assert (hand != null);

        /* Find a matching card to remove. */
        card_pos = match_card(card, hand);

        /* Handle a bad match. */
        if (card_pos < 0 || card_pos >= hand.size()) {
            /*
             * This is theoretically possible even without errors! In fact, a
             * clever server could _force_ us to pick wrong. Figure out how and
             * you'll be ready for a "squeeze" play! Fortunately, it's easily
             * solved.
             */
            log
                    .fine("Whoa!  We can't find a match for the card.  That's strange.");
            send_sync_request();
        }

        /* Remove the card. */
        hand.remove(card_pos);

        // /* Remove the card just by marking its meta category to FALSE.
        // Note that we don't decrease hand_size in this case! */
        // for (int card_pos_2 = 0, tc = card_pos;; card_pos_2++) {
        // if (!this.players[p].u_hand[card_pos_2].is_valid)
        // continue;
        // if (tc <= 0)
        // break;
        // tc--;
        // }
        // this.players[p].u_hand[card_pos_2].is_valid = FALSE;

        /* Update the graphics */
        game.alert_play(p, card, card_pos);
    }

    /*
     * A table message tells you all the cards on the table. Each player only
     * gets one card.
     */
    private void handle_msg_table() throws IOException {
        log.fine("Handling table message.");

        assert (this.players != null);
        for (int p = 0; p < this.num_players; p++) {
            Card card = fd_in.read_card();
            log.fine(card.toString());
            this.players[p].table_card = card;
        }

        /*
         * TODO: verify that the table cards have been removed from the hands
         */

        game.alert_table();
    }

    /* A trick message tells you about the end of a trick (and who won). */
    private void handle_msg_trick() throws IOException {
        /* Read the trick winner */
        int winner = fd_in.read_seat();

        assert (winner >= 0 && winner < this.num_players);

        /* Update the graphics. */
        game.alert_trick(winner);

        /* Clear all cards off the table. */
        for (int p = 0; p < this.num_players; p++)
            this.players[p].table_card = Card.UNKNOWN_CARD;
    }

    /*
     * An options request asks you to pick a set of options. Each "option" gives
     * a list of choices so that you pick one choice for each option. An option
     * with only one choice is a special case: a boolean option.
     */
    private void handle_req_options() throws IOException {
        int option_cnt; /* the number of options */
        String[] descs; /* Descriptive texts for the options. */
        int[] choice_cnt; /* The number of choices for each option */
        int[] defaults; /*
                         * What option choice is currently chosen for each
                         * option
                         */
        String[][] option_choices; /*
                                     * The texts for each option choice of each
                                     * option
                                     */

        if (this.state == ClientState.STATE_OPTIONS) {
            /*
             * The new options request overrides the old one. But this means
             * some messy cleanup is necessary.
             */
            log.warning("New options request overriding old one.");
        }

        /* Read the number of options. */
        option_cnt = fd_in.readInt();
        assert (option_cnt > 0);

        /* Allocate all data */
        descs = new String[option_cnt];
        choice_cnt = new int[option_cnt];
        defaults = new int[option_cnt];
        option_choices = new String[option_cnt][];

        /* Read all the options, their defaults, and the possible choices. */
        for (int i = 0; i < option_cnt; i++) {
            descs[i] = fd_in.read_string();
            choice_cnt[i] = fd_in.readInt();
            defaults[i] = fd_in.readInt();
            option_choices[i] = new String[choice_cnt[i]];
            for (int j = 0; j < choice_cnt[i]; j++) {
                option_choices[i][j] = fd_in.read_string();
            }
        }

        /* Get the options. */
        set_game_state(ClientState.STATE_OPTIONS);
        if (!game.get_options(descs, defaults, option_choices)) {
            send_options(defaults);
        }
    }

    /* The language lets the server translate messages for us. */
    private void send_language(String lang) throws IOException {
        if (lang == null) {
            lang = "";
        }

        log.fine("Sending language " + lang + " to the server.");

        this.fd_out.write_opcode(ClientOpCode.MSG_LANGUAGE);
        this.fd_out.write_string(lang);
    }

    /* A newgame message tells the server to start a new game. */
    public void send_newgame() throws IOException {
        fd_out.write_opcode(ClientOpCode.RSP_NEWGAME);
    }

    /* A bid message tells the server our choice for a bid. */
    public void send_bid(int bid) throws IOException {
        set_game_state(ClientState.STATE_WAIT);
        fd_out.write_opcode(ClientOpCode.RSP_BID);
        fd_out.writeInt(bid);
    }

    /* An options message tells the server our choices for options. */
    public void send_options(int[] options) throws IOException {
        int option_cnt = options == null ? 0 : options.length;
        fd_out.write_opcode(ClientOpCode.RSP_OPTIONS);
        fd_out.writeInt(option_cnt);
        for (int i = 0; i < option_cnt; i++) {
            fd_out.writeInt(options[i]);
        }

        set_game_state(ClientState.STATE_WAIT);
    }

    /* A play message tells the server our choice for a play. */
    public void send_play(Card card) throws IOException {
        set_game_state(ClientState.STATE_WAIT);
        fd_out.write_opcode(ClientOpCode.RSP_PLAY);
        fd_out.write_card(card);
    }

    /* A sync request asks for a sync from the server. */
    private void send_sync_request() throws IOException {
        log.fine("Sending sync request to server.");
        fd_out.write_opcode(ClientOpCode.REQ_SYNC);
    }

    /* This function handles any input from the server. */
    protected void handle_server_input() throws IOException {
        /* Read the opcode */
        ServerOpCode opcode = this.fd_in.read_opcode();

        log.fine("Received " + opcode + " opcode from the server.");

        switch (opcode) {
        case REQ_NEWGAME:
            game.get_newgame();
            this.play_hand = -1;
            break;
        case MSG_NEWGAME:
            /* TODO: don't make "new game" until here */
            handle_msg_newgame();
            break;
        case MSG_GAMEOVER:
            handle_msg_gameover();
            break;
        case MSG_PLAYERS:
            handle_msg_players();
            break;
        case MSG_NEWHAND:
            game.alert_newhand();
            break;
        case MSG_HAND:
            handle_msg_hand();
            break;
        case REQ_BID:
            handle_req_bid();
            break;
        case MSG_BID:
            handle_msg_bid();
            break;
        case REQ_PLAY:
            handle_req_play();
            break;
        case MSG_BADPLAY:
            handle_msg_badplay();
            break;
        case MSG_PLAY:
            handle_msg_play();
            break;
        case MSG_TABLE:
            handle_msg_table();
            break;
        case MSG_TRICK:
            handle_msg_trick();
            break;
        case MESSAGE_GAME:
            handle_message_global();
            break;
        case REQ_OPTIONS:
            handle_req_options();
            break;
        }
    }

    public void handle_chat(String player, String msg) {
        game.handle_chat(player, msg);
    }

    public void handle_info(int num, List infos) {
        // TODO Auto-generated method stub

    }

    public void handle_launch() throws IOException {
        mod.set_state(ModState.GGZMOD_STATE_CONNECTED);
        game.handle_launch();
    }

    public void handle_player(String name, boolean is_spectator, int seat_num) {
        game.handle_player(name, is_spectator, seat_num);
    }

    public void handle_server_fd(Socket server_fd) throws IOException {

        mod.set_state(ModState.GGZMOD_STATE_PLAYING);
        handle_server_connect(server_fd);
    }

    private void handle_server_connect(Socket server_fd) throws IOException {
        this.fd = server_fd;
        this.fd_in = new GGZCardInputStream(this.fd.getInputStream());
        this.fd_out = new GGZCardOutputStream(this.fd.getOutputStream());
        send_language(Locale.getDefault().getLanguage());
        // TODO consider removing this alert
        if (game != null)
            game.alert_server(this.fd); /* ?? */

        // Start listening for events
        new Thread(new Runnable() {
            public void run() {
                try {
                    while (true) {
                        handle_server_input();
                    }
                } catch (EOFException e) {
                    // Ignore, end of input.
                    try {
                        quit();
                    } catch (IOException e2) {
                        // ignore.
                    }
                } catch (Throwable e) {
                    e.printStackTrace();
                    try {
                        quit();
                    } catch (IOException e2) {
                        // ignore.
                    }
                } finally {
                    try {
                        game.handle_disconnect();
                        fd.close();
                    } catch (IOException e2) {
                        // ignore.
                    }
                }
            }
        }).start();
    }

    public void handle_seat(Seat seat) {
        game.handle_seat(seat);
    }

    public void handle_spectator_seat(SpectatorSeat seat) {
        // TODO Auto-generated method stub

    }

    public void request_chat(String message) throws IOException {
        mod.request_chat(message);
    }

    public void request_boot(String name) throws IOException {
        mod.request_boot(name);
    }

    public void request_bot(int seat_num) throws IOException {
        mod.request_bot(seat_num);
    }

    public void request_open(int seat_num) throws IOException {
        mod.request_open(seat_num);
    }

    /**
     * GGZCards client game states
     * 
     * @note Any additional state data should be stored separately, while
     *       maintaining the state here.
     */
    private enum ClientState {
        /** game hasn't started yet */
        STATE_INIT,
        /** waiting for others */
        STATE_WAIT,
        /** our turn to play */
        STATE_PLAY,
        /** our turn to bid */
        STATE_BID,
        /** game's over */
        STATE_DONE,
        /** determining options */
        STATE_OPTIONS
    }

}
