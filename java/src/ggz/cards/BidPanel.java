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

import ggz.cards.client.Client;
import ggz.cards.client.Player;
import ggz.cards.common.Bid;

import java.awt.AWTEventMulticaster;
import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Arrays;
import java.util.List;

import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JTable;
import javax.swing.SwingConstants;
import javax.swing.border.Border;
import javax.swing.border.CompoundBorder;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.DefaultTableModel;
import javax.swing.table.TableColumn;

public class BidPanel extends JPanel implements ActionListener {
    protected ActionListener actionListeners;

    protected int numPlayers;

    protected int lastBidder;

    protected List validBids;

    protected JPanel bidHistoryPanel;

    protected JPanel buttonLayoutPanel;

    protected JPanel buttonPanel;

    private JTable bidHistoryTable;

    private int currentRow = 0;

    private int currentColumn = 0;

    private JPanel bidHistoryTableContainer;

    private int bidIndex;

    public BidPanel(int firstBidder, Client cardClient) {
        super(new BorderLayout(2, 2));
        numPlayers = cardClient.get_num_players();
        lastBidder = firstBidder - 1;
        bidHistoryTable = new JTable(1, numPlayers);
        // Set up the column names, the first bidder is in the first column.
        for (int i = 0; i < numPlayers; i++) {
            Player player = cardClient.get_nth_player(firstBidder);
            TableColumn column = bidHistoryTable.getColumnModel().getColumn(i);
            column.setHeaderValue(player.get_name());
            column.setPreferredWidth(80);
            firstBidder = (firstBidder + 1) % numPlayers;
        }
        bidHistoryTable.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);
        bidHistoryTable.setDefaultRenderer(Object.class, new BidCellRenderer());
        bidHistoryTable.setPreferredScrollableViewportSize(new Dimension(200,
                bidHistoryTable.getRowHeight() * 5));
        bidHistoryTableContainer = new JPanel(new BorderLayout());
        bidHistoryTableContainer.setOpaque(false);
        bidHistoryTableContainer.add(bidHistoryTable, BorderLayout.CENTER);
        bidHistoryTableContainer.add(bidHistoryTable.getTableHeader(),
                BorderLayout.NORTH);
        bidHistoryPanel = new JPanel();
        bidHistoryPanel.add(bidHistoryTableContainer, BorderLayout.CENTER);
        bidHistoryPanel.setOpaque(false);
        buttonPanel = new JPanel(new GridLayout(1, 0, 2, 2));
        buttonPanel.setOpaque(false);
        buttonLayoutPanel = new JPanel();
        buttonLayoutPanel.setOpaque(false);
        buttonLayoutPanel.add(buttonPanel);
        setOpaque(false);
        add(bidHistoryPanel, BorderLayout.NORTH);
        add(buttonLayoutPanel, BorderLayout.SOUTH);
    }

    public void setEnabled(boolean enabled) {
        // super.setEnabled(enabled);
        // Assume only one round of bidding. Removing all buttons makes more
        // sense in this case.
        buttonPanel.removeAll();
    }

    public void setValidBids(Bid[] bid_choices, String[] bid_texts,
            String[] bid_descs) {
        this.validBids = Arrays.asList(bid_choices);
        for (int bidIndex = 0; bidIndex < bid_choices.length; bidIndex++) {
            JButton bid_button = new JButton(bid_texts[bidIndex]);
            if (!bid_texts[bidIndex].equals(bid_descs[bidIndex])) {
                // No point showing a tooltip if it's exactly the
                // same as the text on the button.
                bid_button.setToolTipText(bid_descs[bidIndex]);
            }

            // Get rid of the margin insets on the Metal or Ocean buttons to
            // make the buttons smaller.
            Border old_border = bid_button.getBorder();
            if (old_border instanceof CompoundBorder) {
                bid_button.setBorder(((CompoundBorder) old_border)
                        .getOutsideBorder());
            }

            // Increase the font size for old people.
            Font oldFont = bid_button.getFont();
            bid_button.setFont(oldFont.deriveFont(oldFont.getStyle(), 14));
            bid_button.addActionListener(this);

            // Store the bid index so that we know which bid was selected when a
            // button is clicked.
            bid_button.putClientProperty("ggz.cards.BidPanel.bidIndex", Integer
                    .valueOf(bidIndex));

            // Users complained that in Spades, chatting when
            // the bid button appears can sometimes cause you to bid
            // 0. I'm not sure why this is happening but it might be
            // because the buttons are receiving the focus when they
            // are displayed.
            bid_button.setFocusable(false);
            buttonPanel.add(bid_button);
        }
        validate();
    }

    public void addBid(int bidder, Bid bid) {
        // Add blank bids for players that were skipped in the bidding.
        int nextSlot = (lastBidder + 1) % numPlayers;
        while (nextSlot != bidder) {
            moveToNextBidSlot();
            nextSlot = (nextSlot + 1) % numPlayers;
        }
        bidHistoryTable.setValueAt(bid, currentRow, currentColumn);
        moveToNextBidSlot();
        lastBidder = bidder;
        invalidate();
        repaint();
    }

    protected void moveToNextBidSlot() {
        currentColumn++;
        if (currentColumn >= numPlayers) {
            currentColumn = 0;
            ((DefaultTableModel) bidHistoryTable.getModel())
                    .addRow(new Object[numPlayers]);
            currentRow++;
        }
    }

    public int getBidIndex() {
        return bidIndex;
    }

    /**
     * Subclasses override this to get the text.
     * 
     * @param bid
     * @return
     */
    protected String getBidText(Bid bid) {
        // This works for Spades, we might need to create a SpadesBidPanel that
        // overrides this method.
        if (bid.getVal() == 0) {
            switch (bid.getSpec()) {
            case 1:
                return "Nil";
            case 2:
                return "No blind bid";
            case 3:
                return "Dnil";
            }
        }
        return String.valueOf(bid.getVal());
    }

    public void actionPerformed(ActionEvent event) {
        JButton button = (JButton) event.getSource();
        Integer bidIndex = (Integer) button
                .getClientProperty("ggz.cards.BidPanel.bidIndex");
        this.bidIndex = bidIndex.intValue();
        if (actionListeners != null) {
            actionListeners.actionPerformed(new ActionEvent(this,
                    ActionEvent.ACTION_PERFORMED, bidIndex.toString()));
        }
    }

    public synchronized void addActionListener(ActionListener l) {
        actionListeners = AWTEventMulticaster.add(actionListeners, l);
    }

    public synchronized void removeActionListener(ActionListener l) {
        actionListeners = AWTEventMulticaster.remove(actionListeners, l);
    }

    protected class BidCellRenderer extends DefaultTableCellRenderer {
        public BidCellRenderer() {
            setHorizontalAlignment(SwingConstants.CENTER);
            setHorizontalTextPosition(SwingConstants.LEFT);
        }

        public Component getTableCellRendererComponent(JTable table,
                Object value, boolean isSelected, boolean hasFocus, int row,
                int column) {
            // Component renderer = super.getTableCellRendererComponent(table,
            // value, isSelected, hasFocus, row, column);
            if (value == null) {
                setText(null);
                setIcon(null);
            } else {
                Bid bid = (Bid) value;
                if (bid.getSpec() == 0) {
                    switch (bid.getSuit()) {
                    case 0:
                        setIcon(new ImageIcon(getClass().getResource(
                                "/ggz/cards/images/club.gif")));
                        break;
                    case 1:
                        setIcon(new ImageIcon(getClass().getResource(
                                "/ggz/cards/images/diamond.gif")));
                        break;
                    case 2:
                        setIcon(new ImageIcon(getClass().getResource(
                                "/ggz/cards/images/heart.gif")));
                        break;
                    case 3:
                        setIcon(new ImageIcon(getClass().getResource(
                                "/ggz/cards/images/spade.gif")));
                        break;
                    default:
                        setIcon(null);
                        break;
                    }
                } else {
                    setIcon(null);
                }
                setText(getBidText(bid));
            }
            return this;
        }
    }
}
