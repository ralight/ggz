package ggz.cards;

import ggz.cards.client.CardGameHandler;
import ggz.cards.client.CardSeat;
import ggz.cards.client.Client;
import ggz.cards.common.Bid;
import ggz.cards.common.Card;
import ggz.cards.common.CardSetType;
import ggz.cards.common.GGZCardInputStream;
import ggz.client.mod.ModGame;
import ggz.client.mod.Seat;
import ggz.client.mod.SpectatorSeat;
import ggz.common.ChatType;
import ggz.common.SeatType;
import ggz.ui.ChatAction;
import ggz.ui.ChatPanel;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.Socket;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;

import javax.swing.AbstractAction;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;
import javax.swing.border.CompoundBorder;

public class GamePanel extends JPanel implements CardGameHandler,
        ActionListener {
    protected Client card_client;

    protected ChatPanel chat_panel;

    protected JLabel[] player_labels;

    protected Sprite[][] sprites;

    protected TablePanel table;

    /** Delay 2 seconds at the end of every trick. */
    private static long TRICK_DELAY = 2000;

    private long timeLastPlay;

    private int lastPlayerInTrick;

    public GamePanel(ModGame mod) {
        super(new BorderLayout());
        card_client = new Client(mod);
        card_client.add_listener(this);
        chat_panel = new ChatPanel(new TableChatAction());
        add(chat_panel, BorderLayout.CENTER);
        table = new TablePanel();
        table.setBackground(new Color(0, 128, 0));
        add(table, BorderLayout.NORTH);
        setOpaque(true);
    }

    public void alert_badplay(String err_msg) {
        JOptionPane.showMessageDialog(this, err_msg);
    }

    public void alert_bid(int bidder, Bid bid) {
        // chat_panel.appendChat("alert_bid", bidder + ": " + bid);
    }

    public void alert_hand_size(int max_hand_size) {
        // (Re)allocate sprite arrays for each player.
        for (int i = 0; i < sprites.length; i++) {
            if (sprites[i] == null) {
                sprites[i] = new Sprite[max_hand_size];
            } else if (sprites[i].length < max_hand_size) {
                Sprite[] temp = sprites[i];
                sprites[i] = new Sprite[max_hand_size];
                System.arraycopy(temp, 0, sprites[i], 0, temp.length);
            }
        }
    }

    public void alert_newgame(CardSetType cardset_type) {
        // chat_panel.handle_chat("alert_newgame", null);
        table.setStatus(null);
    }

    public void alert_newhand() {
        invokeAndWait(new Runnable() {
            public void run() {
                // Remove any existing sprites in case the hand was passed in.
                // The sprites would be removed on display_hand but this looks
                // nicer.
                for (int player_num = 0; player_num < card_client
                        .get_num_players(); player_num++) {
                    if (sprites[player_num] != null) {
                        for (int i = 0; i < sprites[player_num].length; i++) {
                            if (sprites[player_num][i] != null) {
                                table.remove(sprites[player_num][i]);
                                sprites[player_num][i] = null;
                            }
                        }
                    }
                }
                validate();
                repaint();
            }
        });
    }

    public void alert_num_players(int numplayers, int old_numplayers) {

        if (player_labels == null) {
            player_labels = new JLabel[numplayers];
        } else if (player_labels.length != numplayers) {
            throw new UnsupportedOperationException(
                    "Dynamic number of players not supported yet.");
        }

        // Initialise the array that holds the card sprites for each player.
        if (sprites == null) {
            sprites = new Sprite[numplayers][];
        } else if (sprites.length != numplayers) {
            throw new UnsupportedOperationException(
                    "Dynamic number of players not supported yet.");
        }
    }

    /**
     * Invoked when seat information changes or is first received for a given seat.
     */
    public void alert_player(int seat_num, SeatType old_type, String old_name) {
        JLabel label = player_labels[seat_num];
        CardSeat player = card_client.get_nth_player(seat_num);
        if (label == null) {
            label = new JLabel(player.get_name());
            label.setForeground(Color.WHITE);
            table.add(label);
            player_labels[seat_num] = label;

            if (seat_num != 0) {
                // Listen for click events so as to pop up a menu that allows
                // users to do things to this seat.
                label.addMouseListener(new PopupListener());
            }
        } else {
            label.setText(player.get_name());
        }

        // Position the labels.
        SeatType seat_type = player.get_seat_type();
        switch (seat_num) {
        case 0: // Me - south
            label.setLocation(table.getWidth() - 200, table.getHeight() - 70);
            label.setVerticalAlignment(SwingConstants.BOTTOM);
            break;
        case 1: // West
            label.setIcon(getPlayerIcon(seat_type));
            label.setVerticalTextPosition(SwingConstants.BOTTOM);
            label.setHorizontalTextPosition(SwingConstants.CENTER);
            label.setHorizontalAlignment(SwingConstants.LEFT);
            label.setLocation(0, table.getHeight() / 3);
            initPopupMenu(seat_num, seat_type);
            break;
        case 2: // North
            label.setIcon(getPlayerIcon(seat_type));
            label.setVerticalAlignment(SwingConstants.TOP);
            label.setLocation(table.getWidth() / 2, 0);
            initPopupMenu(seat_num, seat_type);
            break;
        case 3: // East
            label.setIcon(getPlayerIcon(seat_type));
            label.setVerticalTextPosition(SwingConstants.BOTTOM);
            label.setHorizontalTextPosition(SwingConstants.CENTER);
            label.setHorizontalAlignment(SwingConstants.RIGHT);
            label.setLocation(table.getWidth() - 70,
                    (table.getHeight() / 2) - 20);
            initPopupMenu(seat_num, seat_type);
            break;
        default:
            throw new UnsupportedOperationException(
                    "More than 4 players not supported yet.");
        }
        label.setSize(label.getPreferredSize());
    }

    private ImageIcon getPlayerIcon(SeatType type) {
        switch (type) {
        /** This seat does not exist. */
        case GGZ_SEAT_NONE:
            return null;
        /** The seat is open (unoccupied). */
        case GGZ_SEAT_OPEN:
            return null;
        /** The seat has a bot (AI) in it. */
        case GGZ_SEAT_BOT:
            return new ImageIcon(getClass().getResource("images/p19.gif"));
        /** The seat has a regular player in it. */
        case GGZ_SEAT_PLAYER:
            return new ImageIcon(getClass().getResource("images/p31.gif"));
        /** The seat is reserved for a player. */
        case GGZ_SEAT_RESERVED:
            return null;
        /** The seat is abandoned by a player. */
        case GGZ_SEAT_ABANDONED:
            return null;
        default:
            return null;
        }
    }

    private void initPopupMenu(int seat_num, SeatType type) {
        JPopupMenu menu = null;

        switch (type) {
        case GGZ_SEAT_NONE:
            break;
        /** The seat is open (unoccupied). */
        case GGZ_SEAT_OPEN:
            menu = new JPopupMenu("Player");
            menu.add(new SeatBotAction(seat_num));
            break;
        /** The seat has a bot (AI) in it. */
        case GGZ_SEAT_BOT:
            menu = new JPopupMenu("Player");
            menu.add(new SeatOpenAction(seat_num));
            break;
        /** The seat has a regular player in it. */
        case GGZ_SEAT_PLAYER:
        /** The seat is reserved for a player. */
        case GGZ_SEAT_RESERVED:
            menu = new JPopupMenu("Player");
            menu.add(new SeatBotAction(seat_num));
            menu.add(new SeatBootAction(seat_num));
            menu.add(new SeatOpenAction(seat_num));
            break;
        /** The seat is abandoned by a player. */
        case GGZ_SEAT_ABANDONED:
            menu = new JPopupMenu("Player");
            menu.add(new SeatBotAction(seat_num));
        }
        player_labels[seat_num].putClientProperty("ggz.cards.popupMenu", menu);
    }

    public void alert_play(final int player_num, final Card card,
            final int card_pos) {
        timeLastPlay = System.currentTimeMillis();
        lastPlayerInTrick = player_num;

        invokeAndWait(new Runnable() {
            public void run() {
                Point center = new Point(table.getWidth() / 2, table
                        .getHeight() / 2);
                Point trickPos;

                // We only know the cards in our hand.
                // TODO handle bridge where cards are played from partners hand
                if (player_num == 0) {
                    // Find the sprite at card_pos that is still in the players
                    // hand.
                    // in the array so we need to search forward to find the new
                    // card
                    // at that position. We could also resize the array every
                    // time but
                    // this is just as easy.
                    // CardSeat player = card_client.get_nth_player(player_num);
                    // Collection<Card> hand = player.get_hand();
                    // for (; card_pos < sprites[player_num].length; card_pos++)
                    // {
                    // if (sprites[player_num][card_pos].card().equals(card)) {
                    // break;
                    // }
                    // }
                    // trickPos = new Point(center.x
                    // - (sprites[player_num][card_pos].getWidth() / 2),
                    // center.y);
                    //
                    // table.animate(sprites[player_num][card_pos], trickPos,
                    // .2);
                    // table.setComponentZOrder(sprites[player_num][card_pos],
                    // 0);
                } else {
                    try {
                        // // Cards won't match since they are face down.
                        // for (int i = 0; i < sprites[player].length; i++) {
                        // if
                        // (sprites[player][i].card().equals(Card.UNKNOWN_CARD))
                        // {
                        // Sprite newSprite = new Sprite(card);
                        // newSprite.setLocation(sprites[player][i].getLocation());
                        // table.remove(sprites[player][i]);
                        // sprites[player][i] = newSprite;
                        // table.add(newSprite);
                        // table.animate(newSprite, new Point(
                        // table.getWidth() / 2, table.getHeight() / 2),
                        // .5);
                        // break;
                        // }
                        // }
                        Sprite newSprite = new Sprite(card);
                        Point startLocation = table
                                .getPlayerCardPos(player_num);
                        int zOrder;

                        if (Card.UNKNOWN_CARD.equals(card_client
                                .get_nth_player(0).get_table_card())) {
                            // We have not played a card yet so play the card
                            // underneath ours.
                            int num_players = card_client.get_num_players();
                            int player_on_right = (player_num - 1)
                                    % num_players;
                            if (Card.UNKNOWN_CARD.equals(card_client
                                    .get_nth_player(player_on_right)
                                    .get_table_card())) {
                                // The first card in the trick not played by us
                                // goes on the bottom.
                                zOrder = -1;
                            } else {
                                // Put the card on top of the one played by the
                                // previous
                                // player.
                                zOrder = table
                                        .getComponentZOrder(sprites[player_on_right][0]);
                            }
                        } else {
                            // We have played a card so put it on top of ours.
                            zOrder = 0;
                        }

                        switch (player_num) {
                        case 1: // West
                            trickPos = new Point(center.x
                                    - newSprite.getWidth(), center.y
                                    - (newSprite.getHeight() * 3) / 4);
                            break;
                        case 2: // North
                            trickPos = new Point(center.x
                                    - (newSprite.getWidth() / 2), center.y
                                    - newSprite.getHeight());
                            break;
                        case 3: // East
                            trickPos = new Point(center.x, center.y
                                    - ((newSprite.getHeight() * 2) / 4));
                            break;
                        default:
                            throw new UnsupportedOperationException(
                                    "More than 4 players not supported yet.");
                        }
                        newSprite.setLocation(startLocation);
                        sprites[player_num][0] = newSprite;
                        table.add(newSprite, zOrder);
                        table.validate();
                        table.animate(newSprite, trickPos, .3);
                    } catch (IOException e) {
                        handleException(e);
                    }
                }
            }
        });
    }

    public void alert_server(Socket fd) {
        // Don't need the socket so ignore.
        table.setStatus("Waiting for other players to join...");
    }

    public void alert_table() {
        // chat_panel.handle_chat("alert_table", null);
    }

    public void alert_trick(final int winner) {
        // Delay so the user gets to see the cards in the trick.
        if (lastPlayerInTrick != 0) {
            try {
                long timeTrickOnScreen = System.currentTimeMillis()
                        - timeLastPlay;
                Thread.sleep(TRICK_DELAY - timeTrickOnScreen);
            } catch (InterruptedException ex) {
                // Ignore.
                ex.printStackTrace();
            }
        }

        invokeAndWait(new Runnable() {
            public void run() {
                Sprite[] sprites_to_animate = new Sprite[card_client
                        .get_num_players()];
                Point[] endPos = new Point[card_client.get_num_players()];
                int num_sprites_to_animate = 0;

                for (int p = 0; p < card_client.get_num_players(); p++) {
                    Card card_in_trick = card_client.get_nth_player(p)
                            .get_table_card();
                    if (!Card.UNKNOWN_CARD.equals(card_in_trick)) {
                        Sprite[] players_card_sprites = sprites[p];
                        for (int s = 0; s < players_card_sprites.length; s++) {
                            if (players_card_sprites[s] != null
                                    && players_card_sprites[s].card().equals(
                                            card_in_trick)) {
                                sprites_to_animate[num_sprites_to_animate] = players_card_sprites[s];
                                endPos[num_sprites_to_animate] = table
                                        .getPlayerCardPos(winner);
                                num_sprites_to_animate++;
                                break;
                            }
                        }
                    }
                }
                table.animate(num_sprites_to_animate, sprites_to_animate,
                        endPos, 0.3);
                for (int s = 0; s < num_sprites_to_animate; s++) {
                    table.remove(sprites_to_animate[s]);
                }
                repaint();
            }
        });
    }

    public void display_hand(final int player_num) {
        invokeAndWait(new Runnable() {
            public void run() {
                display_hand_impl(player_num);
            }
        });
    }

    /**
     * This method is run on a separate thread in display_hand() above.
     * 
     * @param player_num
     */
    protected void display_hand_impl(int player_num) {
        try {
            CardSeat player = card_client.get_nth_player(player_num);
            Collection<Card> hand = player.get_hand();
            Iterator<Card> iter = hand.iterator();

            // Remove all existing sprites
            for (int i = 0; i < sprites[player_num].length; i++) {
                if (sprites[player_num][i] != null) {
                    table.remove(sprites[player_num][i]);
                    sprites[player_num][i] = null;
                }
            }

            if (player_num == 0) {
                int i = 0;
                Sprite card = null;

                while (iter.hasNext()) {
                    card = new Sprite(iter.next());
                    Point dealerPos = new Point((table.getWidth() / 2)
                            - (card.getWidth() / 2), (table.getHeight() / 2)
                            - (card.getWidth() / 2));
                    int y = table.getHeight() - (card.getHeight() - 30);
                    Point endPos = new Point(120 + (i * 15), y);

                    card.setLocation(dealerPos);
                    table.add(card, 0);
                    table.animate(card, endPos, 0.1);
                    card.setEnabled(false);
                    card.addActionListener(GamePanel.this);
                    card.addMouseListener(new MouseAdapter() {
                        public void mouseEntered(MouseEvent e) {
                            Sprite sprite = ((Sprite) e.getSource());
                            if (sprite.isSelectable() && sprite.isEnabled()) {
                                sprite.setSelected(true);
                            }
                        }

                        public void mouseExited(MouseEvent e) {
                            Sprite sprite = ((Sprite) e.getSource());
                            sprite.setSelected(false);
                        }
                    });
                    sprites[player_num][i] = card;
                    i++;
                }
                table.setSpriteDimensions(card.getWidth(), card.getHeight());
            } else if (player_num == 1) {
                // West
                int i = 0;

                while (iter.hasNext()) {
                    Card c = iter.next();
                    if (!c.equals(Card.UNKNOWN_CARD)) {
                        Sprite card = new Sprite(c);
                        int x = player_labels[player_num].getWidth();
                        Point endPos = new Point(x, 80 + (i * 17));

                        card.setLocation(endPos);
                        table.add(card, 0);
                        card.setEnabled(false);
                        sprites[player_num][i] = card;
                        i++;
                    }
                }
            } else if (player_num == 2) {
                // North
                int i = 0;

                while (iter.hasNext()) {
                    Card c = iter.next();
                    if (!c.equals(Card.UNKNOWN_CARD)) {
                        Sprite card = new Sprite(c);
                        int y = player_labels[player_num].getHeight();
                        Point endPos = new Point((getWidth() - 250) - (i * 15),
                                y);

                        card.setLocation(endPos);
                        table.add(card, 0);
                        card.setEnabled(false);
                        sprites[player_num][i] = card;
                        i++;
                    }
                }
            } else if (player_num == 3) {
                // East
                int i = 0;

                while (iter.hasNext()) {
                    Card c = iter.next();
                    if (!c.equals(Card.UNKNOWN_CARD)) {
                        Sprite card = new Sprite(c);
                        int x = getWidth()
                                - player_labels[player_num].getWidth()
                                - card.getWidth();
                        Point endPos = new Point(x, getHeight() - 300
                                - (i * 15));

                        card.setLocation(endPos);
                        table.add(card, 0);
                        card.setEnabled(false);
                        sprites[player_num][i] = card;
                        i++;
                    }
                }
            }
        } catch (IOException e) {
            handleException(e);
        }
        invalidate();
        validate();
        repaint();
    }

    public void get_bid(final Bid[] bid_choices, final String[] bid_texts,
            final String[] bid_descs) {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                for (int i = 0; i < bid_choices.length; i++) {
                    JButton bid_button = new JButton(bid_texts[i]);
                    if (!bid_texts[i].equals(bid_descs[i])) {
                        // No point showing a tooltip if it's exactly the
                        // same as the text on the button.
                        bid_button.setToolTipText(bid_descs[i]);
                    }
                    // Get rid of the margin insets on the Ocean buttons to make
                    // the buttons smaller.
                    CompoundBorder old_border = (CompoundBorder) bid_button
                            .getBorder();
                    bid_button.setBorder(old_border.getOutsideBorder());
                    bid_button.addActionListener(new BidAction(i));
                    table.addButton(bid_button);
                }
                table.showButtons();
                invalidate();
                validate();
            }
        });
    }

    private class BidAction implements ActionListener {
        private int bid;

        private BidAction(int bidIndex) {
            this.bid = bidIndex;
        }

        public void actionPerformed(ActionEvent event) {
            try {
                card_client.send_bid(bid);
                table.removeAllButtons();
                table.hideButtons();
            } catch (IOException e) {
                handleException(e);
            }
        }
    }

    public void get_newgame() {
        table
                .setStatus("<HTML>Press <EM>Ready</EM> to start the game.<BR>Once all players are ready the game will begin.</HTML>");
        JButton startButton = new JButton("Ready");
        table.removeAllButtons();
        table.addButton(startButton);
        startButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                try {
                    card_client.send_newgame();
                    table.removeAllButtons();
                    table.hideButtons();
                    table.setStatus("Waiting for other players to be ready...");
                } catch (IOException ex) {
                    ex.printStackTrace();
                }
            }
        });
        table.showButtons();
    }

    public boolean get_options(String[] descs, int[] defaults,
            String[][] option_choices) throws IOException {
        table.setStatus(null);
        boolean okClicked = OptionDialog.show(this, descs, defaults,
                option_choices);
        if (okClicked) {
            card_client.send_options(defaults);
        }
        return okClicked;
    }

    public void get_play(int play_hand, Card[] valid_cards) {
        // chat_panel.handle_chat("get_play", "play_hand=" + play_hand);
        // for (int i = 0; i < valid_cards.length; i++) {
        // chat_panel.handle_chat("get_play", valid_cards[i].toString());
        // }

        // if (play_hand != 0) {
        // throw new UnsupportedOperationException(
        // "Playing from other cards not supported yet.");
        // }

        // Enable and disable cards as appropriate.
        Sprite[] player_cards = sprites[play_hand];
        for (int c = 0; c < player_cards.length; c++) {
            if (player_cards[c] != null) {
                boolean isFound = false;
                for (int i = 0; i < valid_cards.length; i++) {
                    if (player_cards[c].card().equals(valid_cards[i])) {
                        isFound = true;
                        break;
                    }
                }
                player_cards[c].setEnabled(isFound);
                player_cards[c].setSelectable(isFound);
            }
        }
    }

    public void actionPerformed(ActionEvent event) {
        if (event.getSource() instanceof Sprite) {
            // A card was played.
            Sprite sprite = (Sprite) event.getSource();
            try {
                card_client.send_play(sprite.card());
                int card_pos;
                for (card_pos = 0; card_pos < sprites[0].length; card_pos++) {
                    if (sprites[0][card_pos] != null) {
                        sprites[0][card_pos].setSelectable(true);
                        sprites[0][card_pos].setEnabled(false);
                    }
                }
                Point center = new Point(table.getWidth() / 2, table
                        .getHeight() / 2);
                Point trickPos = new Point(center.x
                        - (sprite.getWidth() * 3 / 4), center.y
                        - (sprite.getHeight() / 4));

                sprite.setSelected(false);
                sprite.setLocation(sprite.getLocation().x,
                        sprite.getLocation().y - (sprite.getHeight() / 2));
                table.animate(sprite, trickPos, 0.3);
                // TODO remove sprite from array as well so it can be garbage
                // collected.
                // table.remove(sprite);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public int handle_game_message(GGZCardInputStream in, String game, int size) {
        chat_panel.appendChat("handle_game_message", game + " bytes to read="
                + size);
        return 0;
    }

    public void handle_gameover(int[] winners) {
        JOptionPane.showMessageDialog(this, "GAME OVER");
    }

    /**
     * Called by Hearts server just before trick winner is announced to reinform
     * the client of all the cards in the last trick. It's also sent by Hearts
     * after a hand is played to inform the client of the cards that all players
     * held at the bigging of the hand.
     */
    public void set_cardlist_message(String mark, List<Card[]> cardlist) {
        // chat_panel.appendChat("set_cardlist_message", "mark=" + mark);
        // for (int i = 0; i < cardlist.size(); i++) {
        // chat_panel.appendChat("set_cardlist_message", "Player " + i
        // + " cards: ");
        // for (int j = 0; j < cardlist.get(i).length; j++) {
        // chat_panel.appendChat("set_cardlist_message",
        // cardlist.get(i)[j].toString());
        // }
        // }
    }

    public void set_player_message(int player_num, String message) {
        JLabel label = player_labels[player_num];
        message = message.replace("\n", "<BR>");
        label.setText("<HTML><B>"
                + card_client.get_nth_player(player_num).get_name()
                + "</B><BR><EM><SPAN style='font-weight:normal'>" + message
                + "</span></EM></HTML>");
        label.setSize(label.getPreferredSize());
        // Position the label.
        Point loc = label.getLocation();
        switch (player_num) {
        case 0: // Me - south
            loc.y = Math.min(loc.y, table.getHeight() - label.getHeight());
            break;
        case 1: // West
            break;
        case 2: // North
            break;
        case 3: // East
            loc.x = Math.min(loc.x, table.getWidth() - label.getWidth());
            break;
        default:
            throw new UnsupportedOperationException(
                    "More than 4 players not supported yet.");
        }
        label.setLocation(loc);
    }

    public void set_text_message(String mark, String message) {
        if ("game".equals(mark)) {
            JOptionPane.getFrameForComponent(this).setTitle(message);
        } else if ("Options".equals(mark)) {
            table.setOptionsSummary(message);
        } else if ("Scores".equals(mark)) {
            // Ignore for now, player messages already show scores.
        } else if ("Bid History".equals(mark)) {
            // Ignore for now, player messages already show bids.
        } else if ("Trump".equals(mark)) {
            // TODO, show an icon to indicate trumps
            chat_panel.appendInfo(message);
        } else if ("Rules".equals(mark)) {
            try {
                table.setRulesURL(message);
            } catch (MalformedURLException ex) {
                // Ignore but dump the stack trace for posterity.
                ex.printStackTrace();
            }
        } else if ("".equals(mark) && !"".equals(message)) {
            chat_panel.appendInfo(message);
        } else if ("".equals(mark) && "".equals(message)) {
            // Do nothing, not sure why the server sends this.
        } else {
            chat_panel.appendChat("set_text_message", "mark=" + mark
                    + " message=" + message);
        }
    }

    public void handle_chat(String player, String msg) {
        // Can't ignore messages because we need to handle the /table command,
        // which can be sent from the table or room.
        // String handle = card_client.get_nth_player(0).get_name();
        // if (!handle.equals(player)) {
        chat_panel.appendChat(player, msg);
        // }
    }

    public void handle_info(int num, List infos) {
        // TODO Auto-generated method stub
        chat_panel.appendChat("handle_info", null);
    }

    public void handle_launch() {
        JFrame frame = new JFrame();
        frame.getContentPane().add(this, BorderLayout.CENTER);
        frame.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {
                try {
                    card_client.quit();
                } catch (IOException ex) {
                    ex.printStackTrace();
                }
                e.getWindow().dispose();
            }
        });
        frame.setSize(640, 600);
        frame.setResizable(false);
        frame.setVisible(true);
        table.setStatus("Connecting to game server...");
    }

    public void handle_player(String name, boolean is_spectator, int seat_num) {
        // chat_panel.handle_chat("handle_player", "name=" + name
        // + " is_spectator=" + is_spectator + " seat_num=" + seat_num);
    }

    /**
     * This is invoked when a message arrives from the core client. We
     * handle player messages in alert_player() above, which is invoked
     * when a message arrives from the game client.
     */
    public void handle_seat(Seat seat) {
        // int seat_num = seat.get_num();
        // if (seat_num > 0 && player_labels != null) {
        // if (player_labels[seat_num] != null) {
        // player_labels[seat_num].setIcon(getPlayerIcon(seat.get_type()));
        // player_labels[seat_num].setSize(player_labels[seat_num]
        // .getPreferredSize());
        // initPopupMenu(seat_num);
        // }
        // }
    }

    public void handle_server_fd(Socket fd) {
        // Don't need the socket so ignore
    }

    /**
     * Called when the core client disconnects.
     */
    public void handle_disconnect() {
        JOptionPane.getFrameForComponent(this).dispose();
    }

    public void handle_spectator_seat(SpectatorSeat seat) {
        chat_panel.appendChat("handle_spectator_seat", "num="+ seat.get_num() + " name="+seat.get_name());
    }

    public void handleException(Throwable e) {
        e.printStackTrace();
        JOptionPane.showMessageDialog(this, e);
    }

    public void invokeAndWait(Runnable doRun) {
        try {
            SwingUtilities.invokeAndWait(doRun);
        } catch (Exception e) {
            handleException(e);
        }
    }

    private class TableChatAction extends ChatAction {

        public void sendChat(ChatType type, String target, String message)
                throws IOException {
            if (type != ChatType.GGZ_CHAT_TABLE) {
                chatPanel.appendInfo("This is not supported while at a table.");
            }
            card_client.request_chat(message);
        }

        protected void chat_display_local(ChatType type, String message) {
            // Never display local text because of the /table command. See
            // handle_chat above.
            // String handle = card_client.get_nth_player(0).get_name();
            // chatPanel.appendChat(type, handle, message);
        }

        protected ChatType getDefaultChatType() {
            return ChatType.GGZ_CHAT_TABLE;
        }
    }

    private class SeatBotAction extends AbstractAction {
        private int seat_num;

        public SeatBotAction(int seat_num) {
            this.seat_num = seat_num;
        }

        public Object getValue(String key) {
            if (NAME.equals(key)) {
                return "Put computer player here";
            }
            return super.getValue(key);
        }

        public void actionPerformed(ActionEvent event) {
            try {
                card_client.request_bot(seat_num);
            } catch (IOException e) {
                handleException(e);
            }
        }
    }

    private class SeatBootAction extends AbstractAction {
        private int seat_num;

        public SeatBootAction(int seat_num) {
            this.seat_num = seat_num;
        }

        public Object getValue(String key) {
            if (NAME.equals(key)) {
                return "Boot this player from the game";
            }
            return super.getValue(key);
        }

        public void actionPerformed(ActionEvent event) {
            try {
                String player_name = card_client.get_nth_player(seat_num)
                        .get_name();
                card_client.request_boot(player_name);
            } catch (IOException e) {
                handleException(e);
            }
        }
    }

    private class SeatOpenAction extends AbstractAction {
        private int seat_num;

        public SeatOpenAction(int seat_num) {
            this.seat_num = seat_num;
        }

        public Object getValue(String key) {
            if (NAME.equals(key)) {
                return "Make this seat available for a human to play";
            }
            return super.getValue(key);
        }

        public void actionPerformed(ActionEvent event) {
            try {
                card_client.request_open(seat_num);
            } catch (IOException e) {
                handleException(e);
            }
        }
    }

    private class PopupListener extends MouseAdapter {
        public void mouseClicked(MouseEvent event) {
            JLabel component = (JLabel) event.getComponent();
            JPopupMenu popup = (JPopupMenu) component
                    .getClientProperty("ggz.cards.popupMenu");
            if (popup != null) {
                popup.show(component, event.getX(), event.getY());
            }
        }
    }
}
