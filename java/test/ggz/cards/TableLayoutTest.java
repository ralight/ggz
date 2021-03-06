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

import ggz.cards.common.Card;
import ggz.cards.common.Face;
import ggz.cards.common.Suit;

import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.SwingConstants;
import javax.swing.WindowConstants;

public class TableLayoutTest {

    /**
     * @param args
     */
    public static void main(String[] args) throws Exception {
        final TablePanel table = new TablePanel();
        Rectangle cardDimension = new Sprite(new Card(Face.ACE_HIGH,
                Suit.SPADES)).getBounds();
        final TableLayout layout = new TableLayout(cardDimension.width,
                cardDimension.height);
        final MouseListener spriteHighlighter = new MouseAdapter() {
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
        };

        table.setLayout(layout);
        table.add(new JLabel("South"), new TableConstraints(
                TableConstraints.PLAYER_LABEL, 0));
        table.add(new JLabel("West"), new TableConstraints(
                TableConstraints.PLAYER_LABEL, 1));
        table.add(new JLabel("North"), new TableConstraints(
                TableConstraints.PLAYER_LABEL, 2));
        table.add(new JLabel("East"), new TableConstraints(
                TableConstraints.PLAYER_LABEL, 3));
        for (int cardIndex = 0; cardIndex < 13; cardIndex++) {
            Sprite sprite = new Sprite(new Card(Face.values()[cardIndex + 1],
                    Suit.SPADES), SwingConstants.BOTTOM);
            table.add(sprite, new TableConstraints(
                    TableConstraints.CARD_IN_HAND, 0, cardIndex));
            sprite.setEnabled(true);
            sprite.setSelectable(true);
            sprite.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent event) {
                    Sprite clickedSprite = ((Sprite) event.getSource());
                    clickedSprite.removeActionListener(this);
                    clickedSprite.removeMouseListener(spriteHighlighter);
                    table.animate(clickedSprite, layout.getTrickPos(table, 0),
                            0.5);
                    layout.setConstraints(clickedSprite, new TableConstraints(
                            TableConstraints.CARD_IN_TRICK, 0));
                }
            });
            sprite.addMouseListener(spriteHighlighter);
        }
        for (int cardIndex = 0; cardIndex < 13; cardIndex++) {
            Sprite sprite = new Sprite(new Card(Face.values()[cardIndex + 1],
                    Suit.HEARTS), SwingConstants.LEFT);
            table.add(sprite, new TableConstraints(
                    TableConstraints.CARD_IN_HAND, 1, cardIndex));
            sprite.setEnabled(true);
            sprite.setSelectable(true);
            sprite.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent event) {
                    Sprite clickedSprite = ((Sprite) event.getSource());
                    clickedSprite.removeActionListener(this);
                    clickedSprite.removeMouseListener(spriteHighlighter);
                    table.animate(clickedSprite, layout.getTrickPos(table, 1),
                            0.5);
                    layout.setConstraints(clickedSprite, new TableConstraints(
                            TableConstraints.CARD_IN_TRICK, 1));
                }
            });
            sprite.addMouseListener(spriteHighlighter);
        }
        for (int cardIndex = 0; cardIndex < 13; cardIndex++) {
            Sprite sprite = new Sprite(new Card(Face.values()[cardIndex + 1],
                    Suit.CLUBS), SwingConstants.TOP);
            table.add(sprite, new TableConstraints(
                    TableConstraints.CARD_IN_HAND, 2, cardIndex));
            sprite.setEnabled(true);
            sprite.setSelectable(true);
            sprite.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent event) {
                    Sprite clickedSprite = ((Sprite) event.getSource());
                    clickedSprite.removeActionListener(this);
                    clickedSprite.removeMouseListener(spriteHighlighter);
                    table.animate(clickedSprite, layout.getTrickPos(table, 2),
                            0.5);
                    layout.setConstraints(clickedSprite, new TableConstraints(
                            TableConstraints.CARD_IN_TRICK, 2));
                }
            });
            sprite.addMouseListener(spriteHighlighter);
        }
        for (int cardIndex = 0; cardIndex < 13; cardIndex++) {
            Sprite sprite = new Sprite(new Card(Face.values()[cardIndex + 1],
                    Suit.DIAMONDS), SwingConstants.RIGHT);
            table.add(sprite, new TableConstraints(
                    TableConstraints.CARD_IN_HAND, 3, cardIndex));
            sprite.setEnabled(true);
            sprite.setSelectable(true);
            sprite.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent event) {
                    Sprite clickedSprite = ((Sprite) event.getSource());
                    clickedSprite.removeActionListener(this);
                    clickedSprite.removeMouseListener(spriteHighlighter);
                    table.animate(clickedSprite, layout.getTrickPos(table, 3),
                            0.5);
                    layout.setConstraints(clickedSprite, new TableConstraints(
                            TableConstraints.CARD_IN_TRICK, 3));
                }
            });
            sprite.addMouseListener(spriteHighlighter);
        }
        table.validate();

        // Put it in a JFrame and show it.
        JFrame frame = new JFrame("TableLayout Test");
        frame.getContentPane().add(table);
        frame.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
        frame.pack();
        frame.setVisible(true);
    }

}
