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

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;

import javax.swing.JFrame;
import javax.swing.WindowConstants;

public class AnimationTest {

    /**
     * @param args
     */
    public static void main(String[] args) throws IOException {
        JFrame frame = new JFrame("Animation Test");
        final TablePanel table = new TablePanel();
        frame.add(table, BorderLayout.CENTER);
        frame.setSize(800, 600);
        table.setSize(800, 600);
        table.setBackground(Color.BLUE.darker());

        Sprite sprite = null;
        int x = table.getWidth();

        for (int suit = Suit.CLUBS.ordinal(); suit <= Suit.SPADES.ordinal(); suit++) {
            for (int face = Face.ACE_LOW.ordinal(); face <= Face.ACE_HIGH
                    .ordinal(); face++) {
                sprite = new Sprite(new Card(Face.values()[face],
                        Suit.values()[suit]));
                sprite.setEnabled(true);
                sprite.setLocation(x, table.getHeight() - sprite.getHeight()
                        - 30);
                table.add(sprite);
                sprite.addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent event) {
                        final Sprite source = (Sprite) event.getSource();
                        source.setSelectable(!source.isSelectable());
                        // source.setSelected(!source.isSelected());
                        // new Thread(new Runnable() {
                        // public void run() {
                        table.animate(source, new Point(table.getWidth() / 2,
                                -source.getHeight()), 0.3);
                        // }
                        // }).start();
                    }
                });
                x -= 15;
            }
        }
        frame.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
        frame.setVisible(true);
    }

}
