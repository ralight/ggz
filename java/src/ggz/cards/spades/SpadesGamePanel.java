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
package ggz.cards.spades;

import ggz.cards.CardGamePanel;
import ggz.cards.PlayerLabel;
import ggz.cards.TableConstraints;
import ggz.cards.client.Client;
import ggz.cards.client.Player;
import ggz.cards.common.Bid;
import ggz.cards.common.GGZCardInputStream;
import ggz.client.mod.ModGame;

import java.awt.Color;
import java.awt.GridLayout;
import java.awt.SystemColor;
import java.io.IOException;
import java.util.logging.Logger;

import javax.swing.BorderFactory;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;

public class SpadesGamePanel extends CardGamePanel {
    protected static final Logger log = Logger.getLogger(SpadesGamePanel.class
            .getName());

    protected SpadesBidPanel spadesBidPanel;

    protected JPanel spadesScorePanel;

    protected JLabel usScoreLabel;

    protected JLabel themScoreLabel;

    protected static final Color highlightColor = Color.green.darker();

    public void init(ModGame mod) throws IOException {
        super.init(mod);
        // Create a custom Spades score panel in the NE corner.
        usScoreLabel = new JLabel();
        usScoreLabel.setBackground(SystemColor.text);
        usScoreLabel.setHorizontalAlignment(SwingConstants.CENTER);
        usScoreLabel.setBorder(BorderFactory.createEmptyBorder(0, 2, 0, 2));
        usScoreLabel.setOpaque(true);
        themScoreLabel = new JLabel();
        themScoreLabel.setBackground(SystemColor.text);
        themScoreLabel.setHorizontalAlignment(SwingConstants.CENTER);
        themScoreLabel.setBorder(BorderFactory.createEmptyBorder(0, 2, 0, 2));
        themScoreLabel.setOpaque(true);
        JLabel usHeader = new JLabel("Us");
        usHeader.setHorizontalAlignment(SwingConstants.CENTER);
        usHeader.setOpaque(true);
        JLabel themHeader = new JLabel("Them");
        themHeader.setHorizontalAlignment(SwingConstants.CENTER);
        themHeader.setOpaque(true);
        spadesScorePanel = new JPanel(new GridLayout(2, 2, 1, 0));
        spadesScorePanel.setOpaque(true);
        spadesScorePanel.setBackground(Color.black);
        spadesScorePanel.add(usHeader);
        spadesScorePanel.add(themHeader);
        spadesScorePanel.add(usScoreLabel);
        spadesScorePanel.add(themScoreLabel);
        table.add(spadesScorePanel, new TableConstraints(
                TableConstraints.NORTH_EAST_CORNER));
    }

    public void alert_bid(final int bidder, final Bid bid) {
        // Ignore bids that the server sends that we aren't interested in. e.g.
        // No blind bid and those sent when joining mid game.
        if (!(bid.getVal() == 0 && bid.getSpec() == 2) && (bid.getSuit() == 4)) {
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    if (bidPanel == null) {
                        createBidPanel(bidder);
                    }
                    bidPanel.addBid(bidder, bid);
                    validate();
                }
            });
        }
    }

    protected void createBidPanel(int firstBidder) {
        // Hang onto the SpadesBidPanel we create since we need it
        // to render bids.
        spadesBidPanel = new SpadesBidPanel(firstBidder, cardClient);
        bidPanel = spadesBidPanel;
        bidPanel.addActionListener(this);
        table
                .add(bidPanel, new TableConstraints(
                        TableConstraints.BUTTON_PANEL));
        table.invalidate();
        table.validate();
    }

    public void set_player_message(final int player_num, final String message) {
        // Do nothing. We handle it in handle_game_message().
    }

    public void handle_game_message(GGZCardInputStream in, String game)
            throws IOException {
        // This whole shebang is 40 bytes.
        //int mySize = 10 * 4;
//        if (size < mySize) {
            // Hmm, server has less info to send us than we expect so just do
            // nothing.
//            return 0;
//        }
        final int teamScore1 = in.readInt();
        final int teamBags1 = in.readInt();
        final int teamScore2 = in.readInt();
        final int teamBags2 = in.readInt();
        final int tricks[] = new int[4];
        for (int i = 0; i < 4; i++) {
            tricks[i] = in.readInt();
        }
        final int nextBid = in.readInt();
        final int nextPlay = in.readInt();
        log.fine("Score 1: " + teamScore1 + "(" + teamBags1 + ")");
        log.fine("Score 2: " + teamScore2 + "(" + teamBags2 + ")");
        for (int i = 0; i < 4; i++) {
            log.fine("Seat " + i + " Tricks: " + tricks[i]);
        }
        log.fine("Next bid: " + nextBid);
        log.fine("Next play: " + nextPlay);
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                Player south = cardClient.get_nth_player(0);
                if (south.get_ggz_seat_num() % 2 == 0) {
                    usScoreLabel.setText(teamScore1 + " (" + teamBags1 + ")");
                    themScoreLabel.setText(teamScore2 + " (" + teamBags2 + ")");
                } else {
                    usScoreLabel.setText(teamScore2 + " (" + teamBags2 + ")");
                    themScoreLabel.setText(teamScore1 + " (" + teamBags1 + ")");
                }
                StringBuffer buffer = new StringBuffer();
                for (int i = 0; i < playerLabels.length; i++) {
                    PlayerLabel label = playerLabels[i];
                    Player p = cardClient.get_nth_player(i);
                    Bid bid = p.get_bid();

                    buffer.append("<HTML>");
                    if (cardClient.get_game_state() == Client.STATE_PLAY) {
                        buffer.append("Tricks: ");
                        buffer.append(tricks[p.get_ggz_seat_num()]);
                        buffer.append("<BR>");
                    }
                    if (spadesBidPanel != null && bid != null) {
                        buffer.append("Bid: ");
                        buffer.append(spadesBidPanel.getBidText(bid));
                        buffer.append("<BR>");
                    }
                    boolean isThinking = false;
                    if (cardClient.get_game_state() == Client.STATE_BID) {
                        if (p.get_ggz_seat_num() == nextBid) {
                            buffer.append("Thinking...");
                            isThinking = true;
                        }
                    } else if (cardClient.get_game_state() == Client.STATE_PLAY) {
                        if (p.get_ggz_seat_num() == nextPlay) {
                            buffer.append("Thinking...");
                            isThinking = true;
                        }
                    }
                    buffer.append("</HTML>");
                    label.setText(buffer.toString());
                    // Highlight the label if it's this player's turn.
                    label.setBackground(highlightColor);
                    label.setOpaque(isThinking);
                    buffer.replace(0, buffer.length(), "");
                }
                revalidate();
                repaint();
            }
        });
//        return mySize;
    }

}
