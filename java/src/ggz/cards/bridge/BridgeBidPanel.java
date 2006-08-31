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
package ggz.cards.bridge;

import ggz.cards.BidPanel;
import ggz.cards.client.Client;
import ggz.cards.common.Bid;
import ggz.cards.common.Suit;

import java.awt.FlowLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;

import javax.swing.AbstractAction;
import javax.swing.ButtonGroup;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JToggleButton;

public class BridgeBidPanel extends BidPanel implements ItemListener {

    private JPanel numTricksButtonPanel;

    private JToggleButton[] numTricksButtons;

    private JPanel strainButtonPanel;

    private JPanel strainButtonSizePanel;

    private JToggleButton[] strainButtons;

    private JButton passButton;

    private JButton doubleButton;

    private JButton redoubleButton;

    private JButton bidButton;

    private JPanel doubleRedoublePassPanel;

    protected List validBids;

    private Bid selectedBid;

    private static final Bid BID_PASS = new Bid((byte) 0, Suit.NO_SUIT,
            (byte) 1, (byte) 0);

    private static final Bid BID_DOUBLE = new Bid((byte) 0, Suit.NO_SUIT,
            (byte) 2, (byte) 0);

    private static final Bid BID_REDOUBLE = new Bid((byte) 0, Suit.NO_SUIT,
            (byte) 3, (byte) 0);

    private static final Bid BID_7_NT = new Bid((byte) 7, Suit.NO_SUIT,
            (byte) 0, (byte) 0);

    public BridgeBidPanel(int firstBidder, Client cardClient) {
        super(firstBidder, cardClient);
        ButtonGroup numTricksButtonGroup = new ButtonGroup();
        numTricksButtons = new JToggleButton[7];
        numTricksButtonPanel = new JPanel(new FlowLayout(FlowLayout.CENTER, 2,
                2));

        for (int tricks = 0; tricks < numTricksButtons.length; tricks++) {
            JToggleButton button = new JToggleButton(String.valueOf(tricks + 1));
            numTricksButtons[tricks] = button;
            numTricksButtonGroup.add(button);
            numTricksButtonPanel.add(button);
            button.addItemListener(this);
        }

        ButtonGroup strainButtonGroup = new ButtonGroup();
        strainButtons = new JToggleButton[5];
        strainButtonPanel = new JPanel(new FlowLayout(FlowLayout.CENTER, 0, 0));
        strainButtonSizePanel = new JPanel(new GridLayout(1, 5, 2, 0));
        int strain = 0;
        JToggleButton button = new JToggleButton(new ImageIcon(getClass()
                .getResource("/ggz/cards/images/club.gif")));

        strainButtonPanel.add(strainButtonSizePanel);
        strainButtons[strain] = button;
        strainButtonGroup.add(button);
        strainButtonSizePanel.add(button);
        strain++;
        button = new JToggleButton(new ImageIcon(getClass().getResource(
                "/ggz/cards/images/diamond.gif")));
        strainButtons[strain] = button;
        strainButtonGroup.add(button);
        strainButtonSizePanel.add(button);
        strain++;
        button = new JToggleButton(new ImageIcon(getClass().getResource(
                "/ggz/cards/images/heart.gif")));
        strainButtons[strain] = button;
        strainButtonGroup.add(button);
        strainButtonSizePanel.add(button);
        strain++;
        button = new JToggleButton(new ImageIcon(getClass().getResource(
                "/ggz/cards/images/spade.gif")));
        strainButtons[strain] = button;
        strainButtonGroup.add(button);
        strainButtonSizePanel.add(button);
        strain++;
        button = new JToggleButton("NT");
        strainButtons[strain] = button;
        strainButtonGroup.add(button);
        strainButtonSizePanel.add(button);
        strain++;

        doubleRedoublePassPanel = new JPanel(new FlowLayout(FlowLayout.CENTER,
                2, 2));
        bidButton = new JButton(new BidAction());
        doubleRedoublePassPanel.add(bidButton);
        doubleButton = new JButton(new NonBidAction(BID_DOUBLE));
        doubleRedoublePassPanel.add(doubleButton);
        redoubleButton = new JButton(new NonBidAction(BID_REDOUBLE));
        doubleRedoublePassPanel.add(redoubleButton);
        passButton = new JButton(new NonBidAction(BID_PASS));
        doubleRedoublePassPanel.add(passButton);

        buttonPanel.setVisible(false);
        buttonPanel.setLayout(new GridLayout(3, 1));
        buttonPanel.add(numTricksButtonPanel);
        buttonPanel.add(strainButtonPanel);
        buttonPanel.add(doubleRedoublePassPanel);

        numTricksButtonPanel.setOpaque(false);
        strainButtonPanel.setOpaque(false);
        strainButtonSizePanel.setOpaque(false);
        doubleRedoublePassPanel.setOpaque(false);
    }

    private void initButtonStates() {
        // Disable all the buttons.
        for (int i = 0; i < strainButtons.length; i++) {
            strainButtons[i].setEnabled(false);
        }

        for (int i = 0; i < numTricksButtons.length; i++) {
            numTricksButtons[i].setEnabled(false);
        }

        // Enable all the buttons for which we have bids.
        for (int bidIndex = 0; bidIndex < validBids.size(); bidIndex++) {
            int numTricks = ((Bid) validBids.get(bidIndex)).getVal();
            if (numTricks > 0) {
                JToggleButton button = numTricksButtons[numTricks - 1];
                button.setEnabled(true);
                // Select the first valid one.
                button.setSelected(bidIndex == 0);
            }
        }

        bidButton.setEnabled(validBids.contains(BID_7_NT));
        doubleButton.setEnabled(validBids.contains(BID_DOUBLE));
        redoubleButton.setEnabled(validBids.contains(BID_REDOUBLE));
        passButton.setEnabled(validBids.contains(BID_PASS));
    }

    private void setStrainButtonStates(int numTricks) {
        int indexOfSelectedButton = 0;

        // Disable all the buttons.
        for (int strain = 0; strain < strainButtons.length; strain++) {
            JToggleButton button = strainButtons[strain];
            button.setEnabled(false);
            if (button.isSelected()) {
                indexOfSelectedButton = strain;
            }
        }

        // Enable all the buttons for which we have a bid.
        boolean isSelectionValid = false;
        for (int bidIndex = 0; bidIndex < validBids.size(); bidIndex++) {
            Bid bid = (Bid) validBids.get(bidIndex);
            // If the bid has a spec or spec 2 then it's not a suit bid.
            if (bid.getSpec() == 0 && bid.getSpec2() == 0
                    && bid.getVal() == numTricks) {
                int buttonIndex = getButtonIndex(bid.getSuit());
                JToggleButton button = strainButtons[buttonIndex];
                button.setEnabled(true);
                if (indexOfSelectedButton == buttonIndex) {
                    // The previously selected button is still valid.
                    isSelectionValid = true;
                } else if (!isSelectionValid
                        && indexOfSelectedButton < buttonIndex) {
                    // Select the first valid one.
                    button.setSelected(true);
                    isSelectionValid = true;
                }
            }
        }
    }

    public void setValidBids(Bid[] bid_choices, String[] bid_texts,
            String[] bid_descs) {
        this.validBids = Arrays.asList(bid_choices);
        initButtonStates();
        // We don't show the buttons until it is our turn to bid. This is to
        // prevent the buttons from being shown for spectators.
        buttonPanel.setVisible(true);
    }

    protected Bid getSelectedBid() {
        int strain = -1; // Assumes -1 is not a valid strain.
        int numTricks = -1; // Assumes -1 is not a valid number of tricks.

        // What strain was selected?
        for (int i = 0; i < strainButtons.length; i++) {
            if (strainButtons[i].isSelected()) {
                strain = i;
                break;
            }
        }

        // How many tricks are bid?
        for (int i = 0; i < numTricksButtons.length; i++) {
            if (numTricksButtons[i].isSelected()) {
                numTricks = i + 1;
                break;
            }
        }

        // Find the corresponding bid object.
        for (Iterator iter = validBids.iterator(); iter.hasNext();) {
            Bid bid = (Bid) iter.next();
            if (bid.getVal() == numTricks
                    && getButtonIndex(bid.getSuit()) == strain) {
                return bid;
            }
        }

        return null;
    }

    protected int getButtonIndex(Suit suit) {
        if (Suit.CLUBS.equals(suit)) {
            return 0;
        } else if (Suit.DIAMONDS.equals(suit)) {
            return 1;
        } else if (Suit.HEARTS.equals(suit)) {
            return 2;
        } else if (Suit.SPADES.equals(suit)) {
            return 3;
        } else {
            return 4;
        }
    }

    public int getBidIndex() {
        return validBids.indexOf(selectedBid);
    }

    protected void setBid(Bid bid) {
        this.selectedBid = bid;
        if (actionListeners != null) {
            actionListeners.actionPerformed(new ActionEvent(this,
                    ActionEvent.ACTION_PERFORMED, bid.toString()));
        }
    }

    public void setEnabled(boolean enabled) {
        // super.setEnabled(enabled);
        if (enabled) {
            initButtonStates();
        } else {
            for (int i = 0; i < strainButtons.length; i++) {
                strainButtons[i].setEnabled(false);
            }

            for (int i = 0; i < numTricksButtons.length; i++) {
                numTricksButtons[i].setEnabled(false);
            }
        }
    }

    protected String getBidText(Bid bid) {
        switch (bid.getSpec()) {
        case 0: // regular bid
            return String.valueOf(bid.getVal())
                    + (bid.getSuit() == Suit.NO_SUIT ? " NT" : "");
        case 1: // pass
            return "Pass";
        case 2: // double
            return "Double";
        case 3: // Redouble
            return "Redouble";
        }
        throw new IllegalArgumentException("Unrecognised bid: " + bid);
    }

    public void itemStateChanged(ItemEvent e) {
        if (e.getStateChange() == ItemEvent.SELECTED) {
            JToggleButton button = (JToggleButton) e.getSource();
            int numTricks = Integer.parseInt(button.getText());
            setStrainButtonStates(numTricks);
        }
    }

    protected class BidAction extends AbstractAction {

        public Object getValue(String key) {
            if (NAME.equals(key)) {
                return "Bid";
            }
            return super.getValue(key);
        }

        public void actionPerformed(ActionEvent event) {
            setBid(getSelectedBid());
        }
    }

    protected class NonBidAction extends AbstractAction {
        private Bid bid;

        protected NonBidAction(Bid bid) {
            this.bid = bid;
        }

        public Object getValue(String key) {
            if (NAME.equals(key)) {
                return getBidText(bid);
            }
            return super.getValue(key);
        }

        public void actionPerformed(ActionEvent event) {
            setBid(bid);
        }
    }
}
