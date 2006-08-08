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
package ggz.cards;

import ggz.cards.bridge.BridgeBidPanel;
import ggz.cards.client.CardGameHandler;
import ggz.cards.client.Client;
import ggz.cards.client.Player;
import ggz.cards.common.Bid;
import ggz.cards.common.Card;
import ggz.cards.common.CardSetType;
import ggz.cards.common.GGZCardInputStream;
import ggz.client.mod.ModGame;
import ggz.client.mod.ModState;
import ggz.common.SeatType;
import ggz.common.StringUtil;
import ggz.games.GamePanel;
import ggz.ui.HyperlinkLabel;
import ggz.ui.preferences.GGZPreferences;
import ggz.ui.preferences.PreferencesDialog;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Cursor;
import java.awt.Font;
import java.awt.Frame;
import java.awt.GridLayout;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.net.MalformedURLException;
import java.net.Socket;
import java.util.List;
import java.util.Random;
import java.util.prefs.PreferenceChangeEvent;
import java.util.prefs.PreferenceChangeListener;

import javax.swing.BorderFactory;
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;
import javax.swing.WindowConstants;
import javax.swing.border.Border;
import javax.swing.border.CompoundBorder;

public class CardGamePanel extends GamePanel implements CardGameHandler,
        ActionListener, PreferenceChangeListener {
    protected Client cardClient;

    protected Icon[] playerIcons;

    protected PlayerLabel[] playerLabels;

    protected Sprite[][] sprites;

    protected Sprite[] spriteInTrick;

    protected TableLayout tableLayout;

    protected TablePanel table;

    /** Delay 2 seconds at the end of every trick. */
    private static long TRICK_DELAY = 2000;

    private long timeLastPlay;

    private int lastPlayerInTrick;

    protected int handILastPlayedFrom;

    protected SpriteHighlighter spriteHighlighter = new SpriteHighlighter();

    protected BidPanel bidPanel;

    protected JPanel southEastPanel;

    protected JButton scoresButton;

    protected ScoresDialog scoresDialog;

    protected JButton lastTrickButton;

    protected JButton previousHandButton;

    protected JButton quitButton;

    protected JPanel northWestPanel;

    protected JLabel optionsSummaryLabel;

    protected HyperlinkLabel rulesLabel;

    public void init(ModGame mod) throws IOException {
        super.init(mod);
        cardClient = new Client();
        cardClient.add_listener(this);
        table = new TablePanel();
        // table.setBackground(new Color(0, 128, 0));
        JPanel border = new JPanel(new BorderLayout());
        border.add(table, BorderLayout.CENTER);
        border.setBorder(BorderFactory.createCompoundBorder(BorderFactory
                .createLineBorder(new Color(0, 64, 64), 10), BorderFactory
                .createLineBorder(Color.BLACK, 2)));

        add(border, SmartChatLayout.TABLE);

        // Calculate the size of the cards.
        Sprite sample = new Sprite(Card.UNKNOWN_CARD);
        tableLayout = new TableLayout(sample.getWidth(), sample.getHeight());
        tableLayout.setCardGap(GGZPreferences.getInt("GGZCards.CardGap", 17));
        tableLayout.setPackCardsInHand(GGZPreferences.getBoolean(
                "GGZCards.PackCards", true));
        table.setSpinCards(GGZPreferences
                .getBoolean("GGZCards.SpinCards", true));
        table.setLayout(tableLayout);
        GGZPreferences.addPreferenceChangeListener(this);

        // Add the control to allow the cards in the last trick to be viewed.
        lastTrickButton = new JButton(new ImageIcon(getClass().getResource(
                "/ggz/cards/images/trick.gif")));
        lastTrickButton.setOpaque(false);
        lastTrickButton.setToolTipText("View last trick");
        lastTrickButton.setVisible(false);
        lastTrickButton.addActionListener(this);
        lastTrickButton.setBorder(BorderFactory.createEtchedBorder());
        // Add the control to allow the cards in the previous hand to be viewed.
        previousHandButton = new JButton(new ImageIcon(getClass().getResource(
                "/ggz/cards/images/hand.gif")));
        previousHandButton.setOpaque(false);
        previousHandButton.setToolTipText("View previous hand");
        previousHandButton.setVisible(false);
        previousHandButton.addActionListener(this);
        previousHandButton.setBorder(BorderFactory.createEtchedBorder());
        scoresButton = new JButton(new ImageIcon(getClass().getResource(
                "/ggz/cards/images/scores.gif")));
        scoresButton.setOpaque(true);
        scoresButton.setToolTipText("Scores");
        scoresButton.setVisible(false);
        scoresButton.addActionListener(this);
        // scoresButton.setBorder(null);
        scoresButton.setBorder(BorderFactory.createEtchedBorder());
        southEastPanel = new JPanel(new GridLayout(1, 2, 0, 0));
        southEastPanel.setOpaque(false);
        southEastPanel.add(previousHandButton);
        southEastPanel.add(lastTrickButton);
        southEastPanel.add(scoresButton);
        table.add(southEastPanel, new TableConstraints(
                TableConstraints.SOUTH_EAST_CORNER));

        JPanel southWestPanel = new JPanel(new GridLayout(0, 1, 2, 2));
        JButton standButton = new JButton(new SeatStandAction());
        standButton.setOpaque(false);
        standButton.setForeground(Color.white);
        standButton.setFocusable(false);
        quitButton = new JButton("Quit");
        quitButton.addActionListener(this);
        quitButton.setOpaque(false);
        quitButton.setForeground(Color.white);
        quitButton.setFocusable(false);
        // Get rid of the margin insets on the Metal or Ocean buttons to
        // make the buttons smaller.
        Border old_border = quitButton.getBorder();
        if (old_border instanceof CompoundBorder) {
            quitButton.setBorder(((CompoundBorder) old_border)
                    .getOutsideBorder());
        }
        southWestPanel.setBorder(BorderFactory.createEmptyBorder(2, 2, 2, 2));
        southWestPanel.setOpaque(false);
        // Don't add the stand button for now since users don't like it. We
        // might consider adding it later but only make it available between
        // games.
        // southWestPanel.add(standButton);
        southWestPanel.add(quitButton);
        table.add(southWestPanel, new TableConstraints(
                TableConstraints.SOUTH_WEST_CORNER));

        northWestPanel = new JPanel(new GridLayout(0, 1));
        northWestPanel.setOpaque(false);
        JLabel preferencesLabel = new JLabel(
                "<HTML><A href=''>Preferences...</A></HTML>");
        preferencesLabel.setCursor(Cursor
                .getPredefinedCursor(Cursor.HAND_CURSOR));
        preferencesLabel.addMouseListener(new MouseAdapter() {
            public void mouseClicked(MouseEvent event) {
                PreferencesDialog.showPreferences(
                        (Component) event.getSource(), new String[] {
                                "ggz.cards.GGZCardsPreferencesTab",
                                "ggz.ui.preferences.ChatPreferencesTab" });
            }
        });

        northWestPanel.add(preferencesLabel);
        table.add(northWestPanel, new TableConstraints(
                TableConstraints.NORTH_WEST_CORNER));
    }

    protected void createBidPanel(int firstBidder) {
        if ("Bridge".equals(JOptionPane.getFrameForComponent(this).getTitle())) {
            bidPanel = new BridgeBidPanel(firstBidder, cardClient);
        } else {
            bidPanel = new BidPanel(firstBidder, cardClient);
        }
        bidPanel.addActionListener(this);
        table
                .add(bidPanel, new TableConstraints(
                        TableConstraints.BUTTON_PANEL));
        table.revalidate();
    }

    public void alert_state(Client.GameState oldState, Client.GameState newState) {
        if (oldState == Client.STATE_BID) {
            if (bidPanel != null) {
                SwingUtilities.invokeLater(new Runnable() {
                    public void run() {
                        table.remove(bidPanel);
                        table.revalidate();
                        bidPanel = null;
                    }
                });
            }
        }
    }

    public void alert_badplay(String err_msg) {
        JOptionPane.showMessageDialog(this, err_msg);
    }

    public void alert_bid(final int bidder, final Bid bid) {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                if (bidPanel == null) {
                    createBidPanel(bidder);
                }
                bidPanel.addBid(bidder, bid);
                revalidate();
            }
        });
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
        tableLayout.setMaxHandSize(max_hand_size);
    }

    public void alert_newgame(CardSetType cardset_type) {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                table.setStatus(null);
                handILastPlayedFrom = -1;
            }
        });
    }

    public void alert_newhand() {
        invokeAndWait(new Runnable() {
            public void run() {
                // Remove any existing sprites in case the hand was passed in.
                // The sprites would be removed on display_hand but this looks
                // nicer.
                for (int player_num = 0; player_num < cardClient
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
                revalidate();
                repaint();
            }
        });
    }

    public void alert_num_players(int numplayers, int old_numplayers) {

        if (playerLabels == null) {
            playerLabels = new PlayerLabel[numplayers];
        } else if (playerLabels.length != numplayers) {
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

        if (spriteInTrick == null) {
            spriteInTrick = new Sprite[numplayers];
        } else if (spriteInTrick.length != numplayers) {
            throw new UnsupportedOperationException(
                    "Dynamic number of players not supported yet.");
        }
    }

    /**
     * Invoked when seat information changes or is first received for a given
     * seat.
     */
    public void alert_player(final int seat_num, final SeatType old_type,
            final String old_name) {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                PlayerLabel label = playerLabels[seat_num];
                Player player = cardClient.get_nth_player(seat_num);
                if (label == null) {
                    label = new PlayerLabel(player.get_name());
                    table.add(label, new TableConstraints(
                            TableConstraints.PLAYER_LABEL, seat_num));
                    playerLabels[seat_num] = label;

                    // Listen for click events so as to pop up a menu that
                    // allows users to do things to this seat.
                    label.addMouseListener(new PopupListener());
                    label.setToolTipText("Right click for options.");
                    // }
                } else {
                    label.setPlayerName(player.get_name());
                }

                // Position the labels.
                SeatType seat_type = player.get_seat_type();
                switch (seat_num) {
                case 0: // Me - south
                    label.setIcon(getPlayerIcon(seat_type));
                    // label.setVerticalAlignment(SwingConstants.TOP);
                    initPopupMenu(seat_num, player.get_ggz_seat_num(),
                            seat_type);
                    break;
                case 1: // West
                    label.setIcon(getPlayerIcon(seat_type));
                    // label.setVerticalTextPosition(SwingConstants.BOTTOM);
                    // label.setHorizontalTextPosition(SwingConstants.CENTER);
                    // label.setHorizontalAlignment(SwingConstants.LEFT);
                    initPopupMenu(seat_num, player.get_ggz_seat_num(),
                            seat_type);
                    break;
                case 2: // North
                    label.setIcon(getPlayerIcon(seat_type));
                    // label.setVerticalAlignment(SwingConstants.TOP);
                    initPopupMenu(seat_num, player.get_ggz_seat_num(),
                            seat_type);
                    break;
                case 3: // East
                    label.setIcon(getPlayerIcon(seat_type));
                    // label.setVerticalTextPosition(SwingConstants.BOTTOM);
                    // label.setHorizontalTextPosition(SwingConstants.CENTER);
                    // label.setHorizontalAlignment(SwingConstants.RIGHT);
                    initPopupMenu(seat_num, player.get_ggz_seat_num(),
                            seat_type);
                    break;
                default:
                    throw new UnsupportedOperationException(
                            "More than 4 players not supported yet.");
                }
            }
        });
    }

    protected Icon getPlayerIcon(SeatType type) {
        if (playerIcons == null) {
            playerIcons = new Icon[SeatType.values.size()];
        }
        Icon icon = playerIcons[type.ordinal()];

        if (icon == null) {
            if (type == SeatType.GGZ_SEAT_BOT) {
                /** The seat has a bot (AI) in it. */
                icon = new ImageIcon(getClass().getResource(
                        "/ggz/cards/images/computer.gif"));
            } else if (type == SeatType.GGZ_SEAT_PLAYER) {
                /** The seat has a regular player in it. */
                icon = new ImageIcon(getClass().getResource(
                        "/ggz/cards/images/human.gif"));
            } else {
                icon = new ImageIcon(getClass().getResource(
                        "/ggz/cards/images/empty-seat.gif"));
            }
            playerIcons[type.ordinal()] = icon;
        }
        return icon;
    }

    protected void initPopupMenu(int seat_num, int ggz_seat_num, SeatType type) {
        JPopupMenu menu = null;

        if (type == SeatType.GGZ_SEAT_NONE) {
            // Do nothing, this should never happen.
        } else if (type == SeatType.GGZ_SEAT_OPEN) {
            // The seat is open (unoccupied).
            menu = new JPopupMenu("Player");
            menu.add(new SeatBotAction(ggz_seat_num));
            menu.add(new SeatSitAction(ggz_seat_num));
        } else if (type == SeatType.GGZ_SEAT_BOT) {
            // The seat has a bot (AI) in it.
            menu = new JPopupMenu("Player");
            menu.add(new SeatOpenAction(ggz_seat_num));
        } else if (type == SeatType.GGZ_SEAT_PLAYER) {
            // The seat has a regular player in it.
            menu = new JPopupMenu("Player");
            menu.add(new SeatBootAction(cardClient.get_nth_player(seat_num)
                    .get_name()));
        } else if (type == SeatType.GGZ_SEAT_RESERVED) {
            // The seat is reserved for a player.
            menu = new JPopupMenu("Player");
            menu.add(new SeatBotAction(ggz_seat_num));
            menu.add(new SeatOpenAction(ggz_seat_num));
            menu.add(new SeatSitAction(ggz_seat_num));
        } else if (type == SeatType.GGZ_SEAT_ABANDONED) {
            // The seat is abandoned by a player.
            menu = new JPopupMenu("Player");
            menu.add(new SeatBotAction(ggz_seat_num));
            menu.add(new SeatSitAction(ggz_seat_num));
        }
        playerLabels[seat_num].putClientProperty("ggz.cards.popupMenu", menu);
    }

    protected int getCardOrientation(int player_num) {
        int orientation;
        // Set the orientation
        switch (player_num) {
        case 0:
            orientation = SwingConstants.BOTTOM;
            break;
        case 1:
            orientation = SwingConstants.LEFT;
            break;
        case 2:
            orientation = SwingConstants.TOP;
            break;
        case 3:
            orientation = SwingConstants.RIGHT;
            break;
        default:
            throw new IllegalArgumentException(
                    "More than 4 players not supported yet.");
        }
        return orientation;
    }

    public void alert_play(final int player_num, final Card card) {
        timeLastPlay = System.currentTimeMillis();
        lastPlayerInTrick = player_num;

        // Check if we've already animated this card because we played it
        // ourselves.
        if (handILastPlayedFrom == player_num) {
            handILastPlayedFrom = -1;
            return;
        }

        invokeAndWait(new Runnable() {
            public void run() {
                Point trickPos = tableLayout.getTrickPos(table, player_num);
                boolean found = false;

                // Cards mostly won't match since they are face down.
                for (int card_num = 0; card_num < sprites[player_num].length; card_num++) {
                    Sprite sprite = sprites[player_num][card_num];
                    if (sprite != null && sprite.card().equals(card)) {
                        // We've found the sprite matching the card that was
                        // played so animate it.
                        sprite.setSelected(false);
                        table.animate(sprite, trickPos, .3);
                        spriteInTrick[player_num] = sprite;
                        sprites[player_num][card_num] = null;
                        tableLayout.setConstraints(sprite,
                                new TableConstraints(
                                        TableConstraints.CARD_IN_TRICK,
                                        player_num));
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    try {
                        // Pick a random card, flip it over and play it.
                        Sprite randomCard = null;
                        Random random = new Random();
                        int card_num;
                        do {
                            card_num = random
                                    .nextInt(sprites[player_num].length);
                            randomCard = sprites[player_num][card_num];
                        } while (randomCard == null);
                        Sprite newSprite = new Sprite(card, randomCard
                                .getOrientation());
                        Point startLocation = randomCard.getLocation();
                        int zOrder = table.getComponentZOrderJDK14(randomCard);

                        newSprite.setLocation(startLocation);
                        table.remove(randomCard);
                        table.add(newSprite, zOrder);
                        table.validate();
                        table.animate(newSprite, trickPos, .3);
                        spriteInTrick[player_num] = newSprite;
                        sprites[player_num][card_num] = null;
                        tableLayout.setConstraints(newSprite,
                                new TableConstraints(
                                        TableConstraints.CARD_IN_TRICK,
                                        player_num));
                    } catch (IOException e) {
                        handleException(e);
                    }
                }
                putNextPlayersCardsOnTop(player_num);
            }
        });
    }

    protected void putNextPlayersCardsOnTop(int player_num) {
        int num_players = cardClient.get_num_players();
        int player_on_left = (player_num + 1) % num_players;
        // HB I commented this out because on a fast connection (LAN) the
        // computer was playing a card faster than the animation so the table
        // card was not unknown and the computers card was not played on top of
        // the discard. All this change should mean is that when the last card
        // is played on a trick there is an unnecessary processing for the
        // player that led to the trick. if
        // (Card.UNKNOWN_CARD.equals(card_client.get_nth_player(player_on_left).get_table_card()))
        // {
        // Player has not played a card so put his cards on top
        for (int card_num = sprites[player_on_left].length - 1; card_num >= 0; card_num--) {
            Sprite card = sprites[player_on_left][card_num];
            if (card != null) {
                // table.setComponentZOrder(card, 0);
                table.remove(card);
                table.add(card, 0);
                tableLayout.setConstraints(card,
                        new TableConstraints(TableConstraints.CARD_IN_HAND,
                                player_on_left, card_num));
            }
        }
        // }
    }

    protected void showLastTrick() throws IOException {
        Client.TrickInfo lastTrick = cardClient.get_last_trick();
        if (lastTrick != null) {
            JDialog dialog = new JDialog(
                    JOptionPane.getFrameForComponent(this), "Last Trick");
            dialog.getContentPane().setLayout(
                    new TableLayout(tableLayout.getCardWidth(), tableLayout
                            .getCardHeight()));
            int playerNum = lastTrick.getPlayerWhoLed();
            int numCards = lastTrick.getNumCards();
            for (int i = 0; i < numCards; i++) {
                // Hack to prevent IndexOutOfBoundsException in Suaro.
                // The array has less elements than there are seats so we may
                // end up having to track cards in the trick ourselves.
                if (playerNum >= numCards) {
                    playerNum = numCards - 1;
                }
                Card card = lastTrick.getCardForPlayer(playerNum);
                Sprite sprite = new Sprite(card, getCardOrientation(playerNum));
                dialog.getContentPane().add(
                        sprite,
                        new TableConstraints(TableConstraints.CARD_IN_TRICK,
                                playerNum), 0);
                // Next player.
                playerNum = (playerNum + 1) % numCards;
            }
            dialog.getContentPane().setBackground(table.getBackground());
            dialog.setSize(200, 200);
            dialog.setResizable(false);
            dialog.setLocation(lastTrickButton.getLocationOnScreen());
            dialog.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
            dialog.setModal(true);
            OptionDialog.fixLocation(dialog);
            dialog.setVisible(true);
        }
    }

    protected void showPreviousHand() throws IOException {
        Card[][] cards = cardClient.get_previous_hand();
        if (cards != null) {
            JDialog dialog = new JDialog(
                    JOptionPane.getFrameForComponent(this), "Previous Hand");
            TableLayout layout = new TableLayout(tableLayout.getCardWidth(),
                    tableLayout.getCardHeight());
            layout.setMaxHandSize(cards[0].length);
            // TODO Use this once we've implemented
            // TableLayout.preferredLayoutSize().
            // layout.setCardGap(GGZPreferences.getInt("GGZCards.CardGap", 17));
            dialog.getContentPane().setLayout(layout);
            for (int playerNum = 0; playerNum < cards.length; playerNum++) {
                int cardOrientation = getCardOrientation(playerNum);
                int cardIndex = 0;
                // Loop over cards backwards so that they are displayed
                // from low to high, left to right.
                for (int cardNum = cards[playerNum].length - 1; cardNum >= 0; cardNum--) {
                    Card card = cards[playerNum][cardNum];
                    Sprite sprite = new Sprite(card, cardOrientation);
                    dialog.getContentPane().add(
                            sprite,
                            new TableConstraints(TableConstraints.CARD_IN_HAND,
                                    playerNum, cardIndex));
                    cardIndex++;
                }
            }
            dialog.getContentPane().setBackground(table.getBackground());
            dialog.setSize(380, 380);
            // dialog.setResizable(false);
            dialog.setLocation(previousHandButton.getLocationOnScreen());
            dialog.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
            dialog.setModal(true);
            OptionDialog.fixLocation(dialog);
            dialog.setVisible(true);
        }
    }

    protected void showScores() {
        Frame frame = JOptionPane.getFrameForComponent(this);
        String scores = cardClient.get_scores();
        if (scoresDialog == null) {
            scoresDialog = new ScoresDialog(frame);
            scoresDialog.setResizable(false);
            // scoresDialog.setLocationRelativeTo(frame);
            scoresDialog
                    .setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
            scoresDialog.setModal(false);
        }
        scoresDialog.setScores(scores);
        if (!scoresDialog.isVisible()) {
            scoresDialog.setLocation(scoresButton.getLocationOnScreen());
            OptionDialog.fixLocation(scoresDialog);
            scoresDialog.setVisible(true);
        }
    }

    public void alert_server(Socket fd) {
        // Don't need the socket so ignore.
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                table.setStatus("Waiting for other players to join...");
            }
        });
    }

    public void alert_table() {
        invokeAndWait(new Runnable() {
            public void run() {
                try {
                    for (int playerNum = 0; playerNum < cardClient
                            .get_num_players(); playerNum++) {
                        Card serverTableCard = cardClient.get_nth_player(
                                playerNum).get_table_card();
                        if (!serverTableCard.equals(Card.UNKNOWN_CARD)
                                && spriteInTrick[playerNum] == null) {
                            // We are out of sync with the server, most likely
                            // because we just joined.
                            Sprite tableSprite = new Sprite(serverTableCard,
                                    getCardOrientation(playerNum));
                            spriteInTrick[playerNum] = tableSprite;
                            table.add(tableSprite, new TableConstraints(
                                    TableConstraints.CARD_IN_TRICK, playerNum),
                                    0);
                            table.validate();
                        }
                    }
                } catch (IOException ex) {
                    handleException(ex);
                }
            }
        });
    }

    public void alert_trick(final int winner) {
        // Delay so the user gets to see the cards in the trick.
        if (lastPlayerInTrick != 0) {
            try {
                long timeTrickOnScreen = System.currentTimeMillis()
                        - timeLastPlay;
                long delay = TRICK_DELAY - timeTrickOnScreen;
                if (delay > 0) {
                    Thread.sleep(delay);
                }
            } catch (InterruptedException ex) {
                // Ignore.
                ex.printStackTrace();
            }
        }

        invokeAndWait(new Runnable() {
            public void run() {
                Sprite[] sprites_to_animate = new Sprite[cardClient
                        .get_num_players()];
                Point[] endPos = new Point[cardClient.get_num_players()];
                int num_sprites_to_animate = 0;

                for (int p = 0; p < cardClient.get_num_players(); p++) {
                    Card card_in_trick = cardClient.get_nth_player(p)
                            .get_table_card();
                    if (spriteInTrick[p] != null
                            && spriteInTrick[p].card().equals(card_in_trick)) {
                        sprites_to_animate[num_sprites_to_animate] = spriteInTrick[p];
                        endPos[num_sprites_to_animate] = table
                                .getPlayerCardPos(winner);
                        num_sprites_to_animate++;
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

    public void alert_scores(int hand_num) {
        // TODO handle these.
        System.out.println("hand_num=" + hand_num);
        this.cardClient.get_num_teams();
        int numTeams = this.cardClient.get_num_teams();
        for (int i = 0; i < numTeams; i++) {
            System.out.println("Team " + i + ": "
                    + this.cardClient.get_nth_team(i).getScore());
        }
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
            Player player = cardClient.get_nth_player(player_num);
            List hand = player.get_hand();

            // Remove all existing sprites
            for (int i = 0; i < sprites[player_num].length; i++) {
                if (sprites[player_num][i] != null) {
                    table.remove(sprites[player_num][i]);
                    sprites[player_num][i] = null;
                }
            }

            int card_num = 0;
            Sprite sprite = null;
            int orientation = getCardOrientation(player_num);
            Point[] endPos = new Point[hand.size()];

            // Loop over the cards backwards so that cards are sorted from low
            // to high, left to right.
            for (int i = hand.size() - 1; i >= 0; i--) {
                Card card = (Card) hand.get(i);
                sprite = new Sprite(card, orientation);
                Point dealerPos = new Point((table.getWidth() / 2)
                        - (sprite.getWidth() / 2), (table.getHeight() / 2)
                        - (sprite.getWidth() / 2));
                endPos[card_num] = tableLayout.getCardInHandPos(table,
                        player_num, card_num);

                sprite.setLocation(dealerPos);
                table.add(sprite);

                sprite.setEnabled(false);
                sprites[player_num][card_num] = sprite;
                tableLayout.setConstraints(sprite, new TableConstraints(
                        TableConstraints.CARD_IN_HAND, player_num, card_num));
                card_num++;
            }

            // Don't do deal animation if we are playing, this usually means a
            // player has shown the cards he/she already holds.
            if (cardClient.get_game_state() == Client.STATE_DEAL) {
                table.animate(hand.size(), sprites[player_num], endPos, 0.3);
            } else {
                // Ensure this players cards are on top since in Bridge the
                // player shows his cards and then immediately plays a card.
                int num_players = cardClient.get_num_players();
                putNextPlayersCardsOnTop((player_num - 1) % num_players);
            }
            table.setSpriteDimensions(sprite.getWidth(), sprite.getHeight());
        } catch (IOException e) {
            handleException(e);
        }
        revalidate();
        repaint();
    }

    public void get_bid(final Bid[] bid_choices, final String[] bid_texts,
            final String[] bid_descs) {
        // for (int i = 0; i < bid_choices.length; i++) {
        // System.out.println(bid_choices[i] + ", " + bid_texts[i] + ", "
        // + bid_descs[i]);
        // }
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                if (bidPanel == null) {
                    createBidPanel(0);
                }
                bidPanel.setValidBids(bid_choices, bid_texts, bid_descs);
                // table.showButtons();
            }
        });
    }

    public void get_newgame() {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                table
                        .setStatus("<HTML>Press <EM>Ready</EM> to start the game.<BR>Once all players are ready the game will begin.");
                JButton startButton = new JButton("Ready");
                table.removeAllButtons();
                table.addButton(startButton);
                startButton.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent e) {
                        try {
                            cardClient.send_newgame();
                            table.removeAllButtons();
                            table.hideButtons();
                            table
                                    .setStatus("Waiting for other players to be ready...");
                        } catch (IOException ex) {
                            ex.printStackTrace();
                        }
                    }
                });
                table.showButtons();
            }
        });
    }

    public boolean get_options(final String[] types, final String[] descs,
            final int[] defaults, final String[][] option_choices)
            throws IOException {
        try {
            SwingUtilities.invokeAndWait(new Runnable() {
                public void run() {
                    table.setStatus(null);
                    OptionDialog.show(CardGamePanel.this, types, descs,
                            defaults, option_choices);
                }
            });
            cardClient.send_options(defaults);
            return true;
        } catch (InvocationTargetException e) {
            return false;
        } catch (InterruptedException e) {
            return false;
        }
    }

    public void get_play(final int play_hand, final Card[] valid_cards) {
        // Enable and disable cards as appropriate.
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                Sprite[] player_cards = sprites[play_hand];
                for (int card_num = 0; card_num < player_cards.length; card_num++) {
                    if (player_cards[card_num] != null) {
                        boolean isFound = false;
                        for (int i = 0; i < valid_cards.length; i++) {
                            if (player_cards[card_num].card().equals(
                                    valid_cards[i])) {
                                isFound = true;
                                break;
                            }
                        }
                        player_cards[card_num].setEnabled(isFound);
                        player_cards[card_num].setSelectable(isFound);
                        if (isFound) {
                            // Remove ourselves first to prevent receiving
                            // multiple notifications. This is safe to do even
                            // if we are not registered yet.
                            player_cards[card_num]
                                    .removeActionListener(CardGamePanel.this);
                            player_cards[card_num]
                                    .removeMouseListener(spriteHighlighter);
                            player_cards[card_num]
                                    .addActionListener(CardGamePanel.this);
                            player_cards[card_num]
                                    .addMouseListener(spriteHighlighter);
                        }
                    }
                }
                handILastPlayedFrom = play_hand;
            }
        });
    }

    public void actionPerformed(ActionEvent event) {
        if (event.getSource() instanceof Sprite) {
            // A card was played by us, find the hand we played from.
            Sprite sprite = (Sprite) event.getSource();
            int play_hand;
            int index_of_sprite_in_hand = -1;
            try {
                cardClient.send_play(sprite.card());
                for (play_hand = 0; play_hand < cardClient.get_num_players(); play_hand++) {
                    for (int card_pos = 0; card_pos < sprites[play_hand].length; card_pos++) {
                        if (sprites[play_hand][card_pos] != null) {
                            sprites[play_hand][card_pos].setSelectable(true);
                            sprites[play_hand][card_pos].setEnabled(false);
                            if (sprites[play_hand][card_pos] == sprite) {
                                index_of_sprite_in_hand = card_pos;
                            }
                        }
                    }
                    if (index_of_sprite_in_hand > -1) {
                        // We've found our sprite. Remember the player number
                        // and stop looking.
                        break;
                    }
                }
                Point trickPos = tableLayout.getTrickPos(table, play_hand);
                sprite.setSelected(false);
                // sprite.setLocation(sprite.getLocation().x,
                // sprite.getLocation().y - (sprite.getHeight() / 2));
                table.animate(sprite, trickPos, 0.3);
                spriteInTrick[play_hand] = sprite;
                sprites[play_hand][index_of_sprite_in_hand] = null;
                tableLayout.setConstraints(sprite, new TableConstraints(
                        TableConstraints.CARD_IN_TRICK, play_hand));
                putNextPlayersCardsOnTop(play_hand);
            } catch (IOException e) {
                handleException(e);
            }
        } else if (event.getSource() == bidPanel) {
            try {
                cardClient.send_bid(bidPanel.getBidIndex());
            } catch (IOException ex) {
                handleException(ex);
            }
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    bidPanel.setEnabled(false);
                }
            });
        } else if (event.getSource() == lastTrickButton) {
            try {
                showLastTrick();
            } catch (IOException ex) {
                handleException(ex);
            }
        } else if (event.getSource() == previousHandButton) {
            try {
                showPreviousHand();
            } catch (IOException ex) {
                handleException(ex);
            }
        } else if (event.getSource() == scoresButton) {
            showScores();
        } else if (event.getSource() == quitButton) {
            quit();
        }
    }

    public void preferenceChange(PreferenceChangeEvent event) {
        if ("GGZCards.CardGap".equals(event.getKey())) {
            tableLayout.setCardGap(GGZPreferences.getInt(event.getKey(), 17));
            table.revalidate();
        } else if ("GGZCards.PackCards".equals(event.getKey())) {
            tableLayout.setPackCardsInHand(GGZPreferences.getBoolean(event
                    .getKey(), true));
            table.revalidate();
        } else if ("GGZCards.SpinCards".equals(event.getKey())) {
            table.setSpinCards(GGZPreferences.getBoolean(event.getKey(), true));
        }
    }

    public void handle_game_message(GGZCardInputStream in, String gametype)
            throws IOException {
        // Let subclasses handle this.
    }

    public void handle_gameover(Player[] winners) {
        if (winners == null) {
            JOptionPane.showMessageDialog(this,
                    "GAME OVER\nThere was no winner.");
        } else {
            StringBuffer csv = new StringBuffer(winners[0].get_name());
            for (int i = 1; i < winners.length; i++) {
                csv.append(",");
                csv.append(winners[i].get_name());
            }
            JOptionPane.showMessageDialog(this,
                    "GAME OVER\nThe game was won by " + csv.toString());
        }
    }

    public void handle_disconnect() {
        // Ignore?
    }

    /**
     * Called by server just before trick winner is announced to reinform the
     * client of all the cards in the last trick. It's also sent after a hand is
     * played to inform the client of the cards that all players held at the
     * bigging of the hand.
     * 
     * @param mark
     *            Last Trick or Previous Hand
     * @param cardlist
     *            the list contains as many arrays as there are players in the
     *            game.
     */
    public void set_cardlist_message(final String mark, Card[][] cardlist) {
        if ("Last Trick".equals(mark)) {
            lastTrickButton.setVisible(true);
        } else if ("Previous Hand".equals(mark)) {
            previousHandButton.setVisible(true);
        }
    }

    public void set_player_message(final int player_num, final String message) {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                PlayerLabel label = playerLabels[player_num];
                label.setText("<HTML>"
                        + StringUtil.replace(StringUtil.replace(message, ":",
                                ":<B>"), "\n", "</B><BR>") + "</HTML>");
                // label.setText("<HTML>" + replace(message, "\n", "<BR>")
                // + "</HTML>");
                table.revalidate();
            }
        });
    }

    public void set_text_message(final String mark, final String message) {
        // All handlers are called from the socket thread so we need to do
        // this crazy stuff. This method is usually invoked from a handler.
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                if ("game".equals(mark)) {
                    JOptionPane.getFrameForComponent(CardGamePanel.this)
                            .setTitle(message);
                } else if ("Options".equals(mark)) {
                    setOptionsSummary(StringUtil.replace(message, "\n", "<BR>"));
                } else if ("Scores".equals(mark)) {
                    scoresButton.setVisible(true);
                    if (scoresDialog != null) {
                        scoresDialog.setScores(message);
                    }
                } else if ("Bid History".equals(mark)) {
                    // Ignore for now, player messages already show bids.
                } else if ("Trump".equals(mark)) {
                    // TODO, show an icon to indicate trumps
                    chatPanel.appendInfo(message);
                } else if ("Up-Card".equals(mark)) {
                    chatPanel.appendInfo(message);
                } else if ("Hand Score".equals(mark)) {
                    chatPanel.appendInfo(message);
                } else if ("Rules".equals(mark)) {
                    try {
                        setRulesURL(message);
                    } catch (MalformedURLException ex) {
                        // Ignore but dump the stack trace for posterity.
                        ex.printStackTrace();
                    }
                } else if ("".equals(mark) && !"".equals(message)) {
                    chatPanel.appendInfo(message);
                } else if ("".equals(mark) && "".equals(message)) {
                    // Do nothing, not sure why the server sends this.
                } else {
                    chatPanel.appendChat("set_text_message", "mark=" + mark
                            + " message=" + message, ggzMod.getMyName());
                }
            }
        });
    }

    public void setOptionsSummary(String options) {
        if (optionsSummaryLabel == null) {
            optionsSummaryLabel = new JLabel("Options");
            optionsSummaryLabel.setForeground(Color.WHITE);
            northWestPanel.add(optionsSummaryLabel);
            // Make sure the font is not bold.
            optionsSummaryLabel.setFont(optionsSummaryLabel.getFont()
                    .deriveFont(Font.PLAIN).deriveFont(Font.ITALIC));
        }
        optionsSummaryLabel.setToolTipText("<HTML>" + options);
        // Dimension preferredSize = optionsSummaryLabel.getPreferredSize();
        // Sometimes the label is really, really big for some reason so
        // resize it to a sane size if we have to.
        // preferredSize.width = Math.min(100, preferredSize.width);
        // preferredSize.height = Math.min(20, preferredSize.height);
        // optionsSummaryLabel.setSize(preferredSize);
        // if (rulesLabel == null) {
        // optionsSummaryLabel.setLocation(0, 0);
        // } else {
        // optionsSummaryLabel.setLocation(0, rulesLabel.getHeight());
        // }
        revalidate();
    }

    /**
     * Adds a label in the top left corner that contains a hyperlink to the
     * rules of the game. It strips any leading text and removes any trailing
     * dot since that's the format the message currently comes from the server.
     * 
     * @param url
     * @throws MalformedURLException
     */
    public void setRulesURL(String url) throws MalformedURLException {
        int beginURLSubstring = url.indexOf("http");
        int endURLSubstring = url.lastIndexOf('.');

        if (beginURLSubstring > -1) {
            if (rulesLabel == null) {
                rulesLabel = new HyperlinkLabel();
                northWestPanel.add(rulesLabel);
            }
            String rulesURL;
            if (endURLSubstring > -1) {
                rulesURL = url.substring(beginURLSubstring, endURLSubstring);
            } else {
                rulesURL = url.substring(beginURLSubstring);
            }
            rulesLabel.setText("How to play", rulesURL);
            // Dimension preferredSize = rulesLabel.getPreferredSize();
            // Sometimes the label is really, really big for some reason so
            // resize it to a sane size if we have to.
            // preferredSize.width = Math.min(100, preferredSize.width);
            // preferredSize.height = Math.min(20, preferredSize.height);
            // rulesLabel.setSize(preferredSize);
            // if (optionsSummaryLabel == null) {
            // rulesLabel.setLocation(0, 0);
            // } else {
            // rulesLabel.setLocation(0, optionsSummaryLabel.getHeight());
            // }
            revalidate();
        }
    }

    public void handleState(ModState oldState) {
        if (ggzMod.getState() == ModState.GGZMOD_STATE_CONNECTED)
            table.setStatus("Connecting to game server...");
        else if (ggzMod.getState() == ModState.GGZMOD_STATE_DONE)
            GGZPreferences.removePreferenceChangeListener(this);
        super.handleState(oldState);
    }

    public void handleServer(Socket fd) throws IOException {
        super.handleServer(fd);
        cardClient.handle_server_connect(fd);
        ggzMod.setState(ModState.GGZMOD_STATE_PLAYING);
    }

    protected class PopupListener extends MouseAdapter {
        public void mousePressed(MouseEvent event) {
            maybeShowPopup(event);
        }

        public void mouseReleased(MouseEvent event) {
            maybeShowPopup(event);
        }

        public void maybeShowPopup(MouseEvent event) {
            PlayerLabel component = (PlayerLabel) event.getComponent();
            JPopupMenu popup = (JPopupMenu) component
                    .getClientProperty("ggz.cards.popupMenu");
            if (popup != null && popup.isPopupTrigger(event)) {
                popup.show(component, event.getX(), event.getY());
            }
        }
    }

    /**
     * Listener class used to highlight a sprite to indicate that it has the
     * focus.
     * 
     * @author helg
     * 
     */
    protected class SpriteHighlighter extends MouseAdapter {
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
    }
}
