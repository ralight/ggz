package ggz.cards;

import ggz.cards.common.Card;
import ggz.cards.common.Face;
import ggz.cards.common.Suit;

import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;

import javax.swing.JFrame;
import javax.swing.WindowConstants;

public class SpriteTest {

    /**
     * @param args
     */
    public static void main(String[] args) throws IOException {
        JFrame frame = new JFrame("Sprite Test");
        frame.setLayout(null);
        Sprite sprite = null;

        for (int suit = Suit.CLUBS.ordinal(); suit <= Suit.SPADES.ordinal(); suit++) {
            for (int face = Face.ACE_LOW.ordinal(); face <= Face.ACE_HIGH.ordinal(); face++) {
                sprite = new Sprite(new Card(Face.values()[face],
                        Suit.values()[suit]));
                sprite.setEnabled(true);
                sprite.setLocation(sprite.getWidth() * (face - 1), sprite
                        .getHeight()
                        * (suit - 1));
                frame.add(sprite);
                sprite.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent event) {
                        Sprite source = (Sprite)event.getSource();
                        source.setSelectable(!source.isSelectable());
                        System.out.println(source.card());
                    }
                });
            }
        }

        frame.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
        frame.getContentPane().setPreferredSize(
                new Dimension(sprite.getWidth() * 14, sprite.getHeight() * 4));
        frame.pack();
        frame.setVisible(true);
    }

}
