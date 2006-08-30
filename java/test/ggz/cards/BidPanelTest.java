package ggz.cards;

import java.awt.BorderLayout;
import java.awt.Color;

import javax.swing.JFrame;
import javax.swing.WindowConstants;

import ggz.cards.client.Client;
import ggz.cards.client.Player;
import ggz.cards.common.Bid;

public class BidPanelTest {

    public static void main(String[] args) throws Exception {
        int firstBidder = 1;
        Client cardClient = new TestClient();
        BidPanel bidPanel = new BidPanel(firstBidder, cardClient);
        JFrame frame = new JFrame("BidPanel Test");
        frame.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
        frame.getContentPane().add(bidPanel, BorderLayout.CENTER);
        bidPanel.addBid(1, new Bid((byte) 1));
        bidPanel.addBid(2, new Bid((byte) 2));
        bidPanel.addBid(3, new Bid((byte) 3));
        bidPanel.addBid(0, new Bid((byte) 4));
        bidPanel.addBid(1, new Bid((byte) 5));
        bidPanel.setValidBids(new Bid[] { new Bid((byte) 0), new Bid((byte) 1),
                new Bid((byte) 2), new Bid((byte) 3), new Bid((byte) 4),
                new Bid((byte) 5), new Bid((byte) 6) }, new String[] { "0",
                "1", "2", "3", "4", "5", "6" }, new String[] { "0", "1", "2",
                "3", "4", "5", "6" });
        frame.getContentPane().setBackground(Color.green);
        frame.pack();
        frame.setVisible(true);
    }

    public static class TestClient extends Client {
        public int get_num_players() {
            return 4;
        }

        public Player get_nth_player(final int n) {
            return new Player() {
                public String getName() {
                    return "Player " + n;
                }
            };
        }
    }
}
