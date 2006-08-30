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
import ggz.ui.CustomMetalTheme;

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
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.DefaultTableModel;
import javax.swing.table.TableColumn;

public class BidPanel extends JPanel implements ActionListener {
    protected ActionListener actionListeners;

    protected int numPlayers;

    protected int lastBidder;

    protected JPanel bidHistoryPanel;

    protected JPanel buttonLayoutPanel;

    protected JPanel buttonPanel;

    private JTable bidHistoryTable;

    private int currentRow = 0;

    private int currentColumn = -1;

    private JPanel bidHistoryTableContainer;

    protected List validBids;

    private int selectedBid;

    public BidPanel(int firstBidder, Client cardClient) {
        super(new BorderLayout(2, 2));
        this.numPlayers = cardClient.get_num_players();
        this.lastBidder = firstBidder - 1;
        this.bidHistoryTable = new JTable(1, this.numPlayers);
        // Set up the column names, the first bidder is in the first column.
        for (int i = 0; i < this.numPlayers; i++) {
            Player player = cardClient.get_nth_player(firstBidder);
            TableColumn column = this.bidHistoryTable.getColumnModel()
                    .getColumn(i);
            column.setHeaderValue(player.getName());
            column.setPreferredWidth(80);
            firstBidder = (firstBidder + 1) % this.numPlayers;
        }
        this.bidHistoryTable.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);
        this.bidHistoryTable.setDefaultRenderer(Object.class,
                new BidCellRenderer());
        this.bidHistoryTable.setPreferredScrollableViewportSize(new Dimension(
                200, this.bidHistoryTable.getRowHeight() * 5));
        this.bidHistoryTableContainer = new JPanel(new BorderLayout());
        this.bidHistoryTableContainer.setOpaque(false);
        this.bidHistoryTableContainer.add(this.bidHistoryTable,
                BorderLayout.CENTER);
        this.bidHistoryTableContainer.add(
                this.bidHistoryTable.getTableHeader(), BorderLayout.NORTH);
        this.bidHistoryPanel = new JPanel();
        this.bidHistoryPanel.add(this.bidHistoryTableContainer,
                BorderLayout.CENTER);
        this.bidHistoryPanel.setOpaque(false);
        this.buttonPanel = new JPanel(new GridLayout(1, 0, 2, 2));
        this.buttonPanel.setOpaque(false);
        this.buttonLayoutPanel = new JPanel();
        this.buttonLayoutPanel.setOpaque(false);
        this.buttonLayoutPanel.add(this.buttonPanel);
        setOpaque(false);
        add(this.bidHistoryPanel, BorderLayout.NORTH);
        add(this.buttonLayoutPanel, BorderLayout.SOUTH);
    }

    public void setEnabled(boolean enabled) {
        // super.setEnabled(enabled);
        // Assume only one round of bidding. Removing all buttons makes more
        // sense in this case.
        this.buttonPanel.removeAll();
    }

    public void setValidBids(Bid[] bid_choices, String[] bid_texts,
            String[] bid_descs) {
        // Store the list of valid bids for subclasses.
        this.validBids = Arrays.asList(bid_choices);

        for (int bidIndex = 0; bidIndex < bid_choices.length; bidIndex++) {
            JButton bid_button = new JButton(bid_texts[bidIndex]);
            if (!bid_texts[bidIndex].equals(bid_descs[bidIndex])) {
                // No point showing a tooltip if it's exactly the
                // same as the text on the button.
                bid_button.setToolTipText(bid_descs[bidIndex]);
            }

            CustomMetalTheme.removeInsideBorder(bid_button);

            Font oldFont = bid_button.getFont();
            bid_button.setFont(oldFont.deriveFont(oldFont.getStyle(), 14));
            bid_button.addActionListener(this);

            // Store the bid index so that we know which bid was selected when a
            // button is clicked.
            bid_button.putClientProperty(
                    "ggz.cards.BidPanel.bidIndex", new Integer(bidIndex)); //$NON-NLS-1$

            // Users complained that in Spades, chatting when
            // the bid button appears can sometimes cause you to bid
            // 0. I'm not sure why this is happening but it might be
            // because the buttons are receiving the focus when they
            // are displayed.
            bid_button.setFocusable(false);
            buttonPanel.add(bid_button);
        }
        revalidate();
    }

    public void addBid(int bidder, Bid bid) {
        // Add blank bids for players that were skipped in the bidding.
        int nextSlot = lastBidder;
        do {
            moveToNextBidSlot();
            nextSlot = (nextSlot + 1) % numPlayers;
        } while (nextSlot != bidder);

        bidHistoryTable.setValueAt(bid, currentRow, currentColumn);
        lastBidder = bidder;
        revalidate();
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
        return selectedBid;
    }

    /**
     * Subclasses override this to get the text.
     * 
     * @param bid
     * @return
     */
    protected String getBidText(Bid bid) {
        return String.valueOf(bid.getVal());
    }

    public void actionPerformed(ActionEvent event) {
        JButton button = (JButton) event.getSource();
        Integer bidIndex = (Integer) button
                .getClientProperty("ggz.cards.BidPanel.bidIndex"); //$NON-NLS-1$
        this.selectedBid = bidIndex.intValue();
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
                                "/ggz/cards/images/club.gif"))); //$NON-NLS-1$
                        break;
                    case 1:
                        setIcon(new ImageIcon(getClass().getResource(
                                "/ggz/cards/images/diamond.gif"))); //$NON-NLS-1$
                        break;
                    case 2:
                        setIcon(new ImageIcon(getClass().getResource(
                                "/ggz/cards/images/heart.gif"))); //$NON-NLS-1$
                        break;
                    case 3:
                        setIcon(new ImageIcon(getClass().getResource(
                                "/ggz/cards/images/spade.gif"))); //$NON-NLS-1$
                        break;
                    default:
                        setIcon(null);
                        break;
                    }
                } else {
                    setIcon(null);
                }
                setFont(table.getFont());
                setText(getBidText(bid));
            }
            return this;
        }
    }
}
