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

import ggz.cards.client.CardSeat;
import ggz.cards.common.Card;
import ggz.client.mod.ModGame;

import java.awt.Point;
import java.io.IOException;
import java.util.Collection;
import java.util.Iterator;

import javax.swing.JLabel;

/**
 * Main class used to handle the card game Suaro. It needs special logic to
 * handle the Kitty and up-card.
 * 
 * @author Helg.Bredow
 * 
 */
public class SuaroPanel extends GamePanel {

    public SuaroPanel(ModGame mod) throws IOException {
        super(mod);
    }

    public void display_hand_impl(int player_num) {
        // Display hands for North and South as usual.
        if (player_num == 0 || player_num == 2) {
            super.display_hand_impl(player_num);
            return;
        }
        
        try {
            CardSeat player = card_client.get_nth_player(player_num);
            Collection<Card> hand = player.get_hand();
            Iterator<Card> iter = hand.iterator();

            // Remove all existing sprites
            for (int i = 0; i < sprites[player_num].length; i++) {
                if (sprites[player_num][i] != null) {
                    table.remove(sprites[player_num][i]);
                    sprites[player_num][i] = null;
                }
            }

            if (player_num == 1) {
                // Kitty
                int i = 0;

                while (iter.hasNext()) {
                    Card c = iter.next();
                    //if (!c.equals(Card.UNKNOWN_CARD)) {
                        Sprite card = new Sprite(c);
                        int x = player_labels[player_num].getWidth();
                        Point endPos = new Point(x, 100 + (i * 17));

                        card.setLocation(endPos);
                        table.add(card, 0);
                        card.setEnabled(false);
                        sprites[player_num][i] = card;
                        i++;
                    //}
                }
            } else if (player_num == 3) {
                // Up-card
                Card c = iter.next();
                if (!c.equals(Card.UNKNOWN_CARD)) {
                    JLabel label = player_labels[player_num];
                    Sprite card = new Sprite(c);
                    int x = getWidth() - card.getWidth() - 10;
                    Point endPos = new Point(x, label.getLocation().y
                            + label.getHeight() + 10);

                    card.setLocation(endPos);
                    table.add(card, 0);
                    card.setEnabled(false);
                    sprites[player_num][0] = card;
                }
            }
        } catch (IOException e) {
            handleException(e);
        }
        invalidate();
        validate();
        repaint();
    }
}
