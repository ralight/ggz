package ggz.cards.bridge;

import ggz.cards.BidPanelTest;
import ggz.cards.common.Bid;

import java.awt.BorderLayout;
import java.awt.Color;

import javax.swing.JFrame;
import javax.swing.WindowConstants;

public class BridgeBidPanelTest extends BidPanelTest {
    /**
     * TODO: Test with all bids possible except double and redouble. Found bug
     * where initial state had no num tricks button selected, causes exception
     * if bid button is clicked in this state.
     * 
     * @param args
     */
    public static void main(String[] args) throws Exception {
        Bid[] bids = new Bid[] { new Bid((byte) 6, 1, (byte) 0, (byte) 0),
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
        BridgeBidPanel bidPanel = new BridgeBidPanel(0, new TestClient());
        frame.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
        frame.getContentPane().setBackground(Color.GREEN.darker());
        frame.getContentPane().add(bidPanel, BorderLayout.CENTER);
        bidPanel.setValidBids(bids, null, null);
        frame.pack();
        frame.setVisible(true);
    }

}
