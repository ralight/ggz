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

import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.LayoutManager2;
import java.awt.Point;
import java.awt.Rectangle;

public class TableLayout implements LayoutManager2 {
    private int cardFanGap = 15;

    private int cardWidth;

    private int cardHeight;

    private int maxHandSize = 0;

    private Dimension minimumSize = new Dimension(640, 480);

    private Component[] playerLabels = new Component[4];

    private Component[][] cardsInHand = new Component[4][maxHandSize];

    private Component[] cardsInTrick = new Component[4];

    public TableLayout(int cardWidth, int cardHeight) {
        this.cardWidth = cardWidth;
        this.cardHeight = cardHeight;
    }

    public void setMaxHandSize(int maxHandSize) {
        this.maxHandSize = maxHandSize;
        for (int playerNum = 0; playerNum < cardsInHand.length; playerNum++) {
            cardsInHand[playerNum] = arrayEnsureSize(cardsInHand[playerNum], maxHandSize);
        }
    }

    /**
     * If the constraints are null then the component will not be layed out by
     * this layout manager. This is so that components can be added to a
     * container at an absolute position, which is useful for animation.
     */
    public void addLayoutComponent(Component comp, Object constraints) {
        if (constraints == null) {
            return;
        }
        setConstraints(comp, (TableConstraints) constraints);
    }

    public void setConstraints(Component comp, TableConstraints constraints) {
        removeLayoutComponent(comp);
        if (constraints != null) {
            switch (constraints.type) {
            case TableConstraints.PLAYER_LABEL:
                playerLabels = arrayPut(playerLabels, constraints.playerIndex,
                        comp);
                break;
            case TableConstraints.CARD_IN_HAND:
                cardsInHand[constraints.playerIndex] = arrayPut(
                        cardsInHand[constraints.playerIndex],
                        constraints.cardIndex, comp);
                break;
            case TableConstraints.CARD_IN_TRICK:
                cardsInTrick = arrayPut(cardsInTrick, constraints.playerIndex,
                        comp);
                break;
            default:
                throw new IllegalArgumentException(
                        "invalid table constraint type");
            }
        }
    }

    protected static Component[] arrayEnsureSize(Component[] array, int size) {
        if (array == null) {
            array = new Component[size];
        } else if (size > array.length) {
            // Need to grow the array.
            Component[] temp = array;
            array = new Component[size];
            System.arraycopy(temp, 0, array, 0, temp.length);
        }
        return array;
    }
    
    protected static Component[] arrayPut(Component[] array, int index,
            Component comp) {
        array = arrayEnsureSize(array, index + 1);
        array[index] = comp;
        return array;
    }

    public float getLayoutAlignmentX(Container target) {
        return 0;
    }

    public float getLayoutAlignmentY(Container target) {
        return 0;
    }

    public void invalidateLayout(Container target) {
        // Nothing to do.
    }

    public Dimension maximumLayoutSize(Container target) {
        return null;
    }

    /**
     * Has no effect, since this layout manager does not use a per-component
     * string.
     */
    public void addLayoutComponent(String name, Component comp) {
        // Do nothing.
    }

    public void layoutContainer(Container parent) {
        if (playerLabels != null) {
            for (int playerIndex = 0; playerIndex < playerLabels.length; playerIndex++) {
                Component comp = playerLabels[playerIndex];
                if (comp != null) {
                    layoutPlayerLabel(parent, playerIndex, comp);
                }
            }
        }

        if (cardsInHand != null) {
            for (int playerIndex = 0; playerIndex < cardsInHand.length; playerIndex++) {
                layoutCardsInHand(parent, playerIndex, cardsInHand[playerIndex]);
            }
        }

        if (cardsInTrick != null) {
            for (int playerIndex = 0; playerIndex < cardsInTrick.length; playerIndex++) {
                Component comp = cardsInTrick[playerIndex];
                if (comp != null) {
                    layoutCardInTrick(parent, playerIndex, comp);
                }
            }
        }
    }

    protected void layoutPlayerLabel(Container parent, int playerIndex,
            Component comp) {
        // Labels need to be able to grow and to stop them ping-ponging
        // we never shrink them once they have grown.
        int saneWidth = 120;
        int saneHeight = 120;
        Dimension preferredSize = comp.getPreferredSize();
        Dimension currentSize = comp.getSize();
        comp.setSize(Math.min(saneWidth, Math.max(preferredSize.width, currentSize.width)), 
                Math.min(saneHeight, Math.max(preferredSize.height, currentSize.height)));

        Rectangle handRect = getMaxHandRect(parent, playerIndex);
        switch (playerIndex) {
        case 0: // South
            comp.setLocation(handRect.x + handRect.width, parent.getHeight()
                    - comp.getHeight());
            break;
        case 1: // West
            comp.setLocation(0, handRect.y + handRect.height);
            break;
        case 2: // North
            comp.setLocation(handRect.x - comp.getWidth(), 0);
            break;
        case 3: // East
            comp.setLocation(parent.getWidth() - comp.getWidth(), handRect.y
                    - comp.getHeight());
            break;
        }
    }

    protected void layoutCardsInHand(Container parent, int playerIndex,
            Component[] cards) {
        Rectangle handRect = getMaxHandRect(parent, playerIndex);
        int x = handRect.x;
        int y = handRect.y;
        if (cards != null) {
            switch (playerIndex) {
            case 0: // South
                for (int cardIndex = maxHandSize - 1; cardIndex >= 0; cardIndex--) {
                    Component card = cards[cardIndex];
                    if (card != null) {
                        card.setLocation(x, y);
                    }
                    x += cardFanGap;
                }
                break;
            case 1: // West
                for (int cardIndex = maxHandSize - 1; cardIndex >= 0; cardIndex--) {
                    Component card = cards[cardIndex];
                    if (card != null) {
                        card.setLocation(x, y);
                    }
                    y += cardFanGap;
                }
                break;
            case 2: // North
                for (int cardIndex = 0; cardIndex < maxHandSize; cardIndex++) {
                    Component card = cards[cardIndex];
                    if (card != null) {
                        card.setLocation(x, y);
                    }
                    x += cardFanGap;
                }
                break;
            case 3: // East
                for (int cardIndex = 0; cardIndex < maxHandSize; cardIndex++) {
                    Component card = cards[cardIndex];
                    if (card != null) {
                        card.setLocation(x, y);
                    }
                    y += cardFanGap;
                }
                break;
            }
        }
    }

    protected void layoutCardInTrick(Container parent, int playerIndex,
            Component card) {
        card.setLocation(getTrickPos(parent, playerIndex));
    }

    protected Rectangle getMaxHandRect(Container parent, int playerIndex) {
        Rectangle rect = new Rectangle(0, 0, 0, 0);
        switch (playerIndex) {
        case 0: // South
        case 2: // North
            rect.width = ((maxHandSize - 1) * cardFanGap) + cardWidth;
            rect.height = cardHeight;
            int labelWidth = playerLabels[playerIndex] == null ? 0 : playerLabels[playerIndex].getWidth();
            int padding = (parent.getWidth() - (labelWidth + rect.width)) / 2;

            if (playerIndex == 0) {
                rect.x = padding;
                rect.y = parent.getHeight() - (rect.height * 80 / 100);
            } else if (playerIndex == 2) {
                rect.x = padding + labelWidth;
                rect.y = -rect.height * 60 / 100;
            }
            break;
        case 1: // WEST
        case 3: // EAST
            rect.width = cardHeight;
            rect.height = ((maxHandSize - 1) * cardFanGap) + cardWidth;
            int labelHeight = playerLabels[playerIndex] == null ? 0 : playerLabels[playerIndex].getHeight();
            padding = (parent.getHeight() - (labelHeight + rect.height)) / 2;

            if (playerIndex == 1) {
                rect.y = padding;
                rect.x = -rect.width * 60 / 100;
            } else if (playerIndex == 3) {
                rect.y = padding + labelHeight;
                rect.x = parent.getWidth() - (rect.width * 40 / 100);
            }
            break;
        }
        return rect;
    }

    public Point getTrickPos(Container parent, int player) {
        Point center = new Point(parent.getWidth() / 2, parent.getHeight() / 2);
        switch (player) {
        case 0: // South
            return new Point(center.x - (cardWidth * 5 / 8), center.y
                    - (cardHeight * 2 / 8));
        case 1: // West
            return new Point(center.x - (cardHeight * 6 / 8), center.y
                    - (cardWidth * 5 / 8));
        case 2: // North
            return new Point(center.x - (cardWidth * 3 / 8), center.y
                    - (cardHeight * 6 / 8));
        case 3: // East
            return new Point(center.x - (cardHeight * 2 / 8), center.y
                    - (cardWidth * 3 / 8));
        default:
            throw new UnsupportedOperationException(
                    "More than 4 players not supported yet.");
        }
    }

    public Point getCardInHandPos(Container parent, int playerIndex,
            int cardIndex) {
        Rectangle handRect = getMaxHandRect(parent, playerIndex);
        int x = handRect.x;
        int y = handRect.y;
        switch (playerIndex) {
        case 0: // South
            x += (cardFanGap * (maxHandSize - cardIndex));
            break;
        case 1: // West
            y += (cardFanGap * (maxHandSize - cardIndex));
            break;
        case 2: // North
            x += (cardFanGap * cardIndex);
            break;
        case 3: // East
            y += (cardFanGap * cardIndex);
            break;
        }
        return new Point(x, y);
    }

    public Dimension minimumLayoutSize(Container parent) {
        return minimumSize;
    }

    public Dimension preferredLayoutSize(Container parent) {
        return minimumLayoutSize(parent);
    }

    public void removeLayoutComponent(Component comp) {
        // Find the component and remove it.
        boolean removed = false;
        if (cardsInHand != null) {
            for (int playerIndex = 0; !removed
                    && playerIndex < cardsInHand.length; playerIndex++) {
                for (int cardIndex = 0; !removed
                        && cardIndex < cardsInHand[playerIndex].length; cardIndex++) {
                    if (cardsInHand[playerIndex][cardIndex] == comp) {
                        cardsInHand[playerIndex][cardIndex] = null;
                        removed = true;
                    }
                }
            }
        }
        if (!removed && cardsInTrick != null) {
            for (int playerIndex = 0; !removed
                    && playerIndex < cardsInTrick.length; playerIndex++) {
                if (cardsInTrick[playerIndex] == comp) {
                    cardsInTrick[playerIndex] = null;
                    removed = true;
                }
            }
        }
        if (!removed && playerLabels != null) {
            for (int playerIndex = 0; !removed
                    && playerIndex < playerLabels.length; playerIndex++) {
                if (playerLabels[playerIndex] == comp) {
                    playerLabels[playerIndex] = null;
                    removed = true;
                }
            }
        }
    }

}