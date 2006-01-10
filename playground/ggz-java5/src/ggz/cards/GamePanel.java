package ggz.cards;

import ggz.cards.client.CardGameHandler;
import ggz.cards.client.CardSeat;
import ggz.cards.client.Client;
import ggz.cards.common.Bid;
import ggz.cards.common.Card;
import ggz.cards.common.GGZCardInputStream;
import ggz.client.mod.ModGame;
import ggz.client.mod.Seat;
import ggz.client.mod.SpectatorSeat;
import ggz.common.SeatType;
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
import java.net.Socket;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;

import javax.swing.AbstractAction;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;

public class GamePanel extends JPanel implements CardGameHandler,
        ActionListener {
    protected Client card_client;

    protected ChatPanel chat_panel;

    protected JLabel[] player_labels;

    protected Sprite[][] sprites;

    protected TablePanel table;

    protected String options_summary;

    public GamePanel(ModGame mod) {
        super(new BorderLayout());
        card_client = new Client(mod);
        card_client.add_listener(this);
        chat_panel = new ChatPanel(new ChatAction());
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
        chat_panel.handle_chat("alert_bid", bidder + ": " + bid);
    }

    public void alert_hand_size(int max_hand_size) {
        // (Re)allocate sprite arrays for each player.
        for (int i = 0; i < sprites.length; i++) {
            if (sprites[i] == null) {
                sprites[i] = new Sprite[max_hand_size];
            } else if (sprites[i].length < max_hand_size) {
                throw new UnsupportedOperationException(
                        "Changing hand size not supported yet.");
            }
        }
    }

    public void alert_newgame() {
        chat_panel.handle_chat("alert_newgame", null);
    }

    public void alert_newhand() {
        chat_panel.handle_chat("alert_newhand", null);
    }

    public void alert_num_players(int numplayers, int old_numplayers) {
        System.out.println("alert_num_players: numplayers=" + numplayers
                + " old_numplayers=" + old_numplayers);

        if (player_labels == null) {
            player_labels = new JLabel[numplayers];
        } else if (player_labels.length != numplayers) {
            throw new UnsupportedOperationException(
                    "Dynamic number of players not supported yet.");
        }

        for (int i = 0; i < numplayers; i++) {
            JLabel label = player_labels[i];
            if (label == null) {
                label = new JLabel("<HTML><B>"
                        + card_client.get_nth_player(i).get_name()
                        + "</B></HTML>");
                label.setForeground(Color.WHITE);
                table.add(label);
                player_labels[i] = label;
            } else {
                label.setText(card_client.get_nth_player(i).get_name());
            }

            // Position the labels.
            switch (i) {
            case 0: // Me - south
                label.setLocation(table.getWidth() - 200,
                        table.getHeight() - 70);
                label.setVerticalAlignment(SwingConstants.BOTTOM);
                break;
            case 1: // West
                label.setLocation(0, table.getHeight() / 2);
                break;
            case 2: // North
                label.setLocation(table.getWidth() / 2, 0);
                break;
            case 3: // East
                label.setLocation(table.getWidth() - 70, table.getHeight() / 2);
                label.setHorizontalAlignment(SwingConstants.RIGHT);
                break;
            default:
                throw new UnsupportedOperationException(
                        "More than 4 players not supported yet.");
            }
            label.setSize(label.getPreferredSize());
        }

        if (sprites == null) {
            sprites = new Sprite[numplayers][];
        } else if (sprites.length != numplayers) {
            throw new UnsupportedOperationException(
                    "Dynamic number of players not supported yet.");
        }
    }

    public void alert_play(int player_num, Card card, int card_pos) {
        chat_panel.handle_chat("alert_play", "player=" + player_num + " card="
                + card + " card_pos=" + card_pos);

        Point center = new Point(table.getWidth() / 2, table.getHeight() / 2);
        Point trickPos;

        // We only know the cards in our hand.
        // TODO handle bridge where cards are played from partners hand
        if (player_num == 0) {
            // Find the sprite at card_pos that is still in the players hand.
            // in the array so we need to search forward to find the new card
            // at that position. We could also resize the array every time but
            // this is just as easy.
            // CardSeat player = card_client.get_nth_player(player_num);
            // Collection<Card> hand = player.get_hand();
            // for (; card_pos < sprites[player_num].length; card_pos++) {
            // if (sprites[player_num][card_pos].card().equals(card)) {
            // break;
            // }
            // }
            // trickPos = new Point(center.x
            // - (sprites[player_num][card_pos].getWidth() / 2), center.y);
            //
            // table.animate(sprites[player_num][card_pos], trickPos, .2);
            // table.setComponentZOrder(sprites[player_num][card_pos], 0);
        } else {
            try {
                // // Cards won't match since they are face down.
                // for (int i = 0; i < sprites[player].length; i++) {
                // if (sprites[player][i].card().equals(Card.UNKNOWN_CARD)) {
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
                Point startLocation = table.getPlayerCardPos(player_num);
                int zOrder;

                if (card_client.get_nth_player(0).get_table_card() == Card.UNKNOWN_CARD) {
                    // We have not played a card yet so play the card underneath
                    // ours.
                    int num_players = card_client.get_num_players();
                    int player_on_right = (player_num - 1) % num_players;
                    if (card_client.get_nth_player(player_on_right)
                            .get_table_card() == Card.UNKNOWN_CARD) {
                        // The first card in the trick not played by us goes on
                        // the bottom.
                        zOrder = -1;
                    } else {
                        // Put the card on top of the one played by the previous
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
                            - ((newSprite.getWidth() * 3) / 4), center.y
                            - (newSprite.getHeight() * 3) / 4);
                    break;
                case 2: // North
                    trickPos = new Point(center.x - (newSprite.getWidth() / 2),
                            center.y - newSprite.getHeight());
                    break;
                case 3: // East
                    trickPos = new Point(center.x - (newSprite.getWidth() / 4),
                            center.y - ((newSprite.getHeight() * 2) / 4));
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

    public void alert_player(int i, SeatType old_type, String old_name) {
        chat_panel.handle_chat("alert_player", "alert_player: i=" + i
                + " old_type=" + old_type + " old_name=" + old_name);
    }

    public void alert_server(Socket fd) {
        chat_panel.handle_chat("alert_server", null);
    }

    public void alert_table() {
        chat_panel.handle_chat("alert_table", null);
    }

    public void alert_trick(int winner) {
        chat_panel.handle_chat("alert_trick", null);

        // Remove all the sprites that are on the table.
        for (int p = 0; p < card_client.get_num_players(); p++) {
            Card card_in_trick = card_client.get_nth_player(p).get_table_card();
            Sprite[] players_card_sprites = sprites[p];
            for (int s = 0; s < players_card_sprites.length; s++) {
                if (players_card_sprites[s] != null
                        && players_card_sprites[s].card().equals(card_in_trick)) {
                    table.animate(players_card_sprites[s], table
                            .getPlayerCardPos(winner), 0.3);
                    table.remove(players_card_sprites[s]);
                    break;
                }
            }
        }
        repaint();
    }

    public void display_hand(final int player_num) {
        // System.out.println("display_hand: " + player_num);
        try {
            SwingUtilities.invokeAndWait(new Runnable() {
                public void run() {
                    try {
                        CardSeat player = card_client
                                .get_nth_player(player_num);
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
                            double increment = 0.15;
                            double radians = (hand.size() / 2) * increment;
                            int i = 0;

                            while (iter.hasNext()) {
                                Sprite card = new Sprite(iter.next());
                                Point dealerPos = new Point(
                                        (table.getWidth() / 2)
                                                - (card.getWidth() / 2), (table
                                                .getHeight() / 2)
                                                - (card.getWidth() / 2));
                                int y = table.getHeight()
                                        - (card.getHeight() - 30);// (int)
                                Point endPos = new Point(100 + (i * 15), y /*
                                                                             * +
                                                                             * (i*5)
                                                                             */);
                                // Math.round(.5
                                // *
                                // card.getHeight());
                                card.setLocation(dealerPos);
                                table.add(card, 0);
                                table.animate(card, endPos, 0.1);
                                // card.setLocation(endPos);
                                card.setEnabled(false);
                                // card.setSelectable(true);
                                // card.fanAtAngle(radians);
                                card.addActionListener(GamePanel.this);
                                card.addMouseListener(new MouseAdapter() {
                                    public void mouseEntered(MouseEvent e) {
                                        Sprite sprite = ((Sprite) e.getSource());
                                        if (sprite.isSelectable()
                                                && sprite.isEnabled()) {
                                            sprite.setSelected(true);
                                        }
                                    }

                                    public void mouseExited(MouseEvent e) {
                                        Sprite sprite = ((Sprite) e.getSource());
                                        sprite.setSelected(false);
                                    }
                                });
                                sprites[player_num][i] = card;
                                radians -= increment;
                                i++;
                            }
                            // } else {
                            // // int i = 0;
                            // //
                            // Sprite card = new Sprite(Card.UNKNOWN_CARD);
                            // Point dealerPos = new Point((table.getWidth() /
                            // 2)
                            // - (card.getWidth() / 2), (table.getHeight() / 2)
                            // - (card.getWidth() / 2));
                            // Point endPos =
                            // table.getPlayerCardPos(player_num);
                            // table.setSpriteDimensions(card.getWidth(),
                            // card.getHeight());
                            // for (;iter.hasNext(); iter.next()) {
                            // card.setLocation(dealerPos);
                            // table.add(card);
                            // table.animate(card, endPos, 0.1);
                            // table.remove(card);
                            // // sprites[player_num][i] = card;
                            // // i++;
                            // }
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    invalidate();
                    validate();
                    repaint();

                }
            });
        } catch (Exception e) {
            handleException(e);
        }
    }

    public void get_bid(final Bid[] bid_choices, final String[] bid_texts,
            final String[] bid_descs) {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                for (int i = 0; i < bid_choices.length; i++) {
                    JButton bid_button = new JButton(bid_texts[i]);
                    bid_button.addActionListener(new BidAction(i));
                    table.addButton(bid_button);
                    chat_panel.handle_chat("get_bid", bid_descs[i]);
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
        JButton startButton = new JButton("Start");
        table.addButton(startButton);
        startButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                try {
                    card_client.send_newgame();
                    table.removeAllButtons();
                    table.hideButtons();
                } catch (IOException ex) {
                    ex.printStackTrace();
                }
            }
        });
        table.showButtons();
    }

    public boolean get_options(String[] descs, int[] defaults,
            String[][] option_choices) throws IOException {
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

        // Enable and disable cards as appropriate.
        Sprite[] player_cards = sprites[play_hand];
        for (int c = 0; c < player_cards.length; c++) {
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

    public void actionPerformed(ActionEvent event) {
        if (event.getSource() instanceof Sprite) {
            // A card was played.
            Sprite sprite = (Sprite) event.getSource();
            try {
                card_client.send_play(sprite.card());
                int card_pos;
                for (card_pos = 0; card_pos < sprites[0].length; card_pos++) {
                    sprites[0][card_pos].setSelectable(true);
                    sprites[0][card_pos].setEnabled(false);
                }
                Point center = new Point(table.getWidth() / 2, table
                        .getHeight() / 2);
                Point trickPos = new Point(center.x - (sprite.getWidth() / 2),
                        center.y);

                sprite.setSelected(false);
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
        System.out.println("handle_game_message");
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
        chat_panel.handle_chat("set_cardlist_message", "mark=" + mark);
        for (int i = 0; i < cardlist.size(); i++) {
            chat_panel.handle_chat("set_cardlist_message", "Player " + i
                    + " cards: ");
            for (int j = 0; j < cardlist.get(i).length; j++) {
                chat_panel.handle_chat("set_cardlist_message",
                        cardlist.get(i)[j].toString());
            }
        }
    }

    public void set_player_message(int player_num, String message) {
        // chat_panel.handle_chat("set_player_message", "player=" + player_num
        // + " message=" + message);
        JLabel label = player_labels[player_num];
        // Label label = message_labels[player_num];
        // if (label == null) {
        // label = new Label(message);
        // label.setForeground(Color.WHITE);
        //
        // // Position the label.
        // Point loc = player_labels[player_num].getLocation();
        // switch (player_num) {
        // case 0: // Me - south
        // loc.x += player_labels[player_num].getWidth();
        // break;
        // case 1: // West
        // case 2: // North
        // case 3: // East
        // loc.y += player_labels[player_num].getHeight();
        // break;
        // default:
        // throw new UnsupportedOperationException(
        // "More than 4 players not supported yet.");
        // }
        // label.setLocation(loc);
        // table.add(label);
        // label.setSize(label.getPreferredSize());
        // message_labels[player_num] = label;
        // } else {
        message = message.replace("\n", "<BR>");
        label.setText("<HTML><B>"
                + card_client.get_nth_player(player_num).get_name()
                + "</B><BR><EM><SPAN style='font-weight:normal'>" + message
                + "</span></EM></HTML>");
        label.setSize(label.getPreferredSize());
        // }
    }

    public void set_text_message(String mark, String message) {
        if ("game".equals(mark)) {
            JOptionPane.getFrameForComponent(this).setTitle(message);
        } else if ("Options".equals(mark)) {
            options_summary = message;
        } else if ("Rules".equals(mark)) {
            chat_panel.handle_chat(null, message);
        } else if ("".equals(mark) && "".equals(message)) {
            // Do nothing, not sure why the server sends this.
        } else {
            chat_panel.handle_chat("set_text_message", "mark=" + mark
                    + " message=" + message);
        }
    }

    public void handle_chat(String player, String msg) {
        String handle = card_client.get_nth_player(0).get_name();
        if (!handle.equals(player)) {
            chat_panel.handle_chat(player, msg);
        }
    }

    public void handle_info(int num, List infos) {
        // TODO Auto-generated method stub
        System.out.println("handle_info");
    }

    public void handle_launch() {
        JFrame frame = new JFrame();
        frame.getContentPane().add(this, BorderLayout.CENTER);
        // frame.add(this, BorderLayout.CENTER);
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
        // frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
        frame.setSize(640, 600);
        frame.setResizable(false);
        frame.setVisible(true);
    }

    public void handle_player(String name, boolean is_spectator, int seat_num) {
        // TODO Auto-generated method stub
        System.out.println("handle_player: name=" + name + " is_spectator="
                + is_spectator + " seat_num=" + seat_num);
    }

    public void handle_seat(Seat seat) {
        // TODO Auto-generated method stub
        System.out.println("handle_seat: " + seat);
    }

    public void handle_server_fd(Socket fd) {
        System.out.println("handle_server_fd");
    }

    public void handle_spectator_seat(SpectatorSeat seat) {
        // TODO Auto-generated method stub
        System.out.println("handle_spectator_seat");
    }

    public void handleException(Throwable e) {
        e.printStackTrace();
        JOptionPane.showMessageDialog(this, e);
    }

    private class ChatAction extends AbstractAction {
        public Object getValue(String key) {
            if (NAME.equals(key)) {
                return "Send";
            }
            return super.getValue(key);
        }

        public void actionPerformed(ActionEvent e) {
            try {
                String message = chat_panel.getMessage();
                String handle = card_client.get_nth_player(0).get_name();
                card_client.request_chat(message);
                chat_panel.clearMessage();
                chat_panel.handle_chat(handle, message);
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        }
    }
}
