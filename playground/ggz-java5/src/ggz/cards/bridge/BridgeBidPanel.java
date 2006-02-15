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

import ggz.cards.common.Bid;

import java.awt.AWTEventMulticaster;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;

import javax.swing.AbstractAction;
import javax.swing.ButtonGroup;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JToggleButton;
import javax.swing.SwingConstants;
import javax.swing.WindowConstants;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.DefaultTableModel;

public class BridgeBidPanel extends JPanel implements ItemListener {
	private int numPlayers;

	private JTable bidHistoryTable;

	private JScrollPane bidHistoryScrollPane;

	private JPanel layoutPanel;

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

	private List validBids;

	private Bid bid;

	private ActionListener actionListeners;

	private static final Bid BID_PASS = new Bid((byte) 0, 0, (byte) 1, (byte) 0);

	private static final Bid BID_DOUBLE = new Bid((byte) 0, 0, (byte) 2,
			(byte) 0);

	private static final Bid BID_REDOUBLE = new Bid((byte) 0, 0, (byte) 3,
			(byte) 0);

	private static final Bid BID_7_NT = new Bid((byte) 7, 4, (byte) 0, (byte) 0);

	public BridgeBidPanel(int numPlayers) {
		super(new BorderLayout(2, 2));
		this.numPlayers = numPlayers;
		ButtonGroup numTricksButtonGroup = new ButtonGroup();
		numTricksButtons = new JToggleButton[7];
		numTricksButtonPanel = new JPanel(new FlowLayout(FlowLayout.CENTER, 2,
				2));// (new GridLayout(1, 7, 2, 2));

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
				.getResource("../images/club.gif")));

		strainButtonPanel.add(strainButtonSizePanel);
		strainButtons[strain] = button;
		strainButtonGroup.add(button);
		strainButtonSizePanel.add(button);
		strain++;
		button = new JToggleButton(new ImageIcon(getClass().getResource(
				"../images/diamond.gif")));
		strainButtons[strain] = button;
		strainButtonGroup.add(button);
		strainButtonSizePanel.add(button);
		strain++;
		button = new JToggleButton(new ImageIcon(getClass().getResource(
				"../images/heart.gif")));
		strainButtons[strain] = button;
		strainButtonGroup.add(button);
		strainButtonSizePanel.add(button);
		strain++;
		button = new JToggleButton(new ImageIcon(getClass().getResource(
				"../images/spade.gif")));
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

		bidHistoryTable = new JTable(1, numPlayers);
		bidHistoryTable.setDefaultRenderer(Object.class, new BidCellRenderer());
		bidHistoryTable.setPreferredScrollableViewportSize(new Dimension(65,
				bidHistoryTable.getRowHeight() * 5));
		bidHistoryScrollPane = new JScrollPane();
		bidHistoryScrollPane.getViewport().add(bidHistoryTable);

		layoutPanel = new JPanel(new GridLayout(3, 1));
		layoutPanel.add(numTricksButtonPanel);
		layoutPanel.add(strainButtonPanel);
		layoutPanel.add(doubleRedoublePassPanel);
		add(layoutPanel, BorderLayout.SOUTH);
		add(bidHistoryScrollPane, BorderLayout.CENTER);

		setOpaque(false);
		layoutPanel.setOpaque(false);
		numTricksButtonPanel.setOpaque(false);
		strainButtonPanel.setOpaque(false);
		strainButtonSizePanel.setOpaque(false);
		doubleRedoublePassPanel.setOpaque(false);
		bidHistoryScrollPane.getViewport().setBackground(
				bidHistoryTable.getBackground());
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
				int buttonIndex = bid.getSuit();
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

	public void setValidBids(Bid[] bidChoices) {
		this.validBids = Arrays.asList(bidChoices);
		initButtonStates();
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
			if (bid.getVal() == numTricks && bid.getSuit() == strain) {
				return bid;
			}
		}

		return null;
	}

	public void addBid(int bidder, Bid bid) {
		// Check if the bidder has bid this round.
		int currentRowIndex = bidHistoryTable.getRowCount() - 1;
		for (int i = bidder; i < numPlayers; i++) {
			if (bidHistoryTable.getValueAt(currentRowIndex, i) != null) {
				DefaultTableModel model = (DefaultTableModel) bidHistoryTable
						.getModel();
				model.addRow(new Object[numPlayers]);
				break;
			}
		}
		bidHistoryTable.setValueAt(bid, bidHistoryTable.getRowCount() - 1,
				bidder);
	}

	public int getBidIndex() {
		return validBids.indexOf(bid);
	}

	protected void setBid(Bid bid) {
		this.bid = bid;
		if (actionListeners != null) {
			actionListeners.actionPerformed(new ActionEvent(this,
					ActionEvent.ACTION_PERFORMED, bid.toString()));
		}
	}

	public void setEnabled(boolean enabled) {
		super.setEnabled(enabled);
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

	protected static String getText(Bid bid) {
		switch (bid.getSpec()) {
		case 0: // regular bid
			return String.valueOf(bid.getVal())
					+ (bid.getSuit() == 4 ? " NT" : "");
		case 1: // pass
			return "Pass";
		case 2: // double
			return "Double";
		case 3: // Redouble
			return "Redouble";
		}
		throw new IllegalArgumentException("Unrecognised bid: " + bid);
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) throws Exception {
		Bid[] bids = new Bid[] { 
				new Bid((byte) 6, 1, (byte) 0, (byte) 0),
				new Bid((byte) 6, 2, (byte) 0, (byte) 0),
				new Bid((byte) 6, 3, (byte) 0, (byte) 0),
				new Bid((byte) 6, 4, (byte) 0, (byte) 0),
				new Bid((byte) 7, 0, (byte) 0, (byte) 0),
				new Bid((byte) 7, 1, (byte) 0, (byte) 0),
				new Bid((byte) 7, 2, (byte) 0, (byte) 0),
				new Bid((byte) 7, 3, (byte) 0, (byte) 0),
				new Bid((byte) 7, 4, (byte) 0, (byte) 0),
				new Bid((byte) 0, 0, (byte) 2, (byte) 0),
				new Bid((byte) 0, 0, (byte) 1, (byte) 0) };
		JFrame frame = new JFrame("Bid Panel Test");
		BridgeBidPanel bidPanel = new BridgeBidPanel(4);
		frame.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
		frame.getContentPane().setBackground(Color.GREEN.darker());
		frame.getContentPane().add(bidPanel, BorderLayout.CENTER);
		frame.pack();
		bidPanel.setValidBids(bids);
		frame.setVisible(true);
	}

	public void itemStateChanged(ItemEvent e) {
		if (e.getStateChange() == ItemEvent.SELECTED) {
			JToggleButton button = (JToggleButton) e.getSource();
			int numTricks = Integer.parseInt(button.getText());
			setStrainButtonStates(numTricks);
		}
	}

	public synchronized void addActionListener(ActionListener l) {
		actionListeners = AWTEventMulticaster.add(actionListeners, l);
	}

	public synchronized void removeActionListener(ActionListener l) {
		actionListeners = AWTEventMulticaster.remove(actionListeners, l);
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
				return getText(bid);
			}
			return super.getValue(key);
		}

		public void actionPerformed(ActionEvent event) {
			setBid(bid);
		}
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
								"../images/club.gif")));
						break;
					case 1:
						setIcon(new ImageIcon(getClass().getResource(
								"../images/diamond.gif")));
						break;
					case 2:
						setIcon(new ImageIcon(getClass().getResource(
								"../images/heart.gif")));
						break;
					case 3:
						setIcon(new ImageIcon(getClass().getResource(
								"../images/spade.gif")));
						break;
					default:
						setIcon(null);
						break;
					}
				} else {
					setIcon(null);
				}
				setText(BridgeBidPanel.getText(bid));
			}
			return this;
		}
	}
}
