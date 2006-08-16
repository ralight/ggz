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

import javax.swing.JComponent;

public class TableLayout implements LayoutManager2 {
    private static final int FACE_DOWN_CARD_GAP = 10;

    private boolean packCardsInHand = true;

    private int cardFanGap = 17;

    private int cardWidth;

    private int cardHeight;

    private int maxHandSize = 0;

    private Dimension minimumSize = new Dimension(640, 480);

    private Component statusLabel;

    private Component buttonPanel;

    private Component[] playerLabels = new Component[4];

    private Component[][] cardsInHand = new Component[4][maxHandSize];

    private Component[] cardsInTrick = new Component[4];

    private Component southEastCorner;

    private Component southWestCorner;

    private Component northEastCorner;

    private Component northWestCorner;

    public TableLayout(int cardWidth, int cardHeight) {
        this.cardWidth = cardWidth;
        this.cardHeight = cardHeight;
    }

    public void setMaxHandSize(int maxHandSize) {
        this.maxHandSize = maxHandSize;
        for (int playerNum = 0; playerNum < cardsInHand.length; playerNum++) {
            cardsInHand[playerNum] = arrayEnsureSize(cardsInHand[playerNum],
                    maxHandSize);
        }
    }

    public void setCardGap(int gap) {
        cardFanGap = gap;
    }

    public void setPackCardsInHand(boolean b) {
        packCardsInHand = b;
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
            case TableConstraints.STATUS_LABEL:
                statusLabel = comp;
                break;
            case TableConstraints.BUTTON_PANEL:
                buttonPanel = comp;
                break;
            case TableConstraints.SOUTH_EAST_CORNER:
                southEastCorner = comp;
                break;
            case TableConstraints.SOUTH_WEST_CORNER:
                southWestCorner = comp;
                break;
            case TableConstraints.NORTH_EAST_CORNER:
                northEastCorner = comp;
                break;
            case TableConstraints.NORTH_WEST_CORNER:
                northWestCorner = comp;
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

        layoutStatusLabel(parent);
        layoutBidPanel(parent);
        layoutSouthEastCorner(parent);
        layoutSouthWestCorner(parent);
        layoutNorthEastCorner(parent);
        layoutNorthWestCorner(parent);
    }

    protected void layoutStatusLabel(Container parent) {
        if (statusLabel != null) {
            statusLabel.setSize(statusLabel.getPreferredSize());
            statusLabel.setLocation(parent.getWidth() / 2
                    - statusLabel.getWidth() / 2, parent.getHeight() / 2
                    - statusLabel.getHeight() / 2);
        }
    }

    protected void layoutBidPanel(Container parent) {
        if (buttonPanel != null) {
            buttonPanel.setSize(buttonPanel.getPreferredSize());
            buttonPanel.setLocation((parent.getWidth() / 2)
                    - (buttonPanel.getWidth() / 2), parent.getHeight()
                    - (buttonPanel.getHeight() + 170));
            // bidPanel.setLocation(parent.getWidth() / 2
            // - bidPanel.getWidth() / 2, parent.getHeight() / 2
            // - bidPanel.getHeight() / 2);
        }
    }

    /**
     * If there is a last trick button, it is put in the bottom right hand
     * corner.
     * 
     * @param parent
     */
    protected void layoutSouthEastCorner(Container parent) {
        if (southEastCorner != null) {
            southEastCorner.setSize(southEastCorner.getPreferredSize());
            southEastCorner.setLocation(parent.getWidth()
                    - southEastCorner.getWidth(), parent.getHeight()
                    - southEastCorner.getHeight());
        }
    }

    protected void layoutSouthWestCorner(Container parent) {
        if (southWestCorner != null) {
            southWestCorner.setSize(southWestCorner.getPreferredSize());
            southWestCorner.setLocation(0, parent.getHeight()
                    - southWestCorner.getHeight());
        }
    }

    protected void layoutNorthEastCorner(Container parent) {
        if (northEastCorner != null) {
            northEastCorner.setSize(northEastCorner.getPreferredSize());
            northEastCorner.setLocation(parent.getWidth()
                    - northEastCorner.getWidth(), 0);
        }
    }

    protected void layoutNorthWestCorner(Container parent) {
        if (northWestCorner != null) {
            northWestCorner.setSize(northWestCorner.getPreferredSize());
            northWestCorner.setLocation(0, 0);
        }
    }

    protected void layoutPlayerLabel(Container parent, int playerIndex,
            Component comp) {
        // Labels need to be able to grow and to stop them ping-ponging
        // we never shrink them once they have grown.
        JComponent c = (JComponent) comp;
        c.setPreferredSize(null);
        Dimension preferredSize = comp.getPreferredSize();
        Dimension currentSize = comp.getSize();
        comp.setSize(Math.max(preferredSize.width, currentSize.width), Math
                .max(preferredSize.height, currentSize.height));
        c.setPreferredSize(comp.getSize());

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
        Rectangle handRect = getHandRect(parent, playerIndex);
        int x = handRect.x;
        int y = handRect.y;
        int gap = getCardFanGap(playerIndex);
        if (cards != null) {
            switch (playerIndex) {
            case 0: // South
                for (int cardIndex = maxHandSize - 1; cardIndex >= 0; cardIndex--) {
                    Component card = cards[cardIndex];
                    if (card != null) {
                        card.setLocation(x, y);
                    }
                    if (!packCardsInHand || card != null) {
                        x += gap;
                    }
                }
                break;
            case 1: // West
                for (int cardIndex = maxHandSize - 1; cardIndex >= 0; cardIndex--) {
                    Component card = cards[cardIndex];
                    if (card != null) {
                        card.setLocation(x, y);
                    }
                    if (!packCardsInHand || card != null) {
                        y += gap;
                    }
                }
                break;
            case 2: // North
                for (int cardIndex = 0; cardIndex < maxHandSize; cardIndex++) {
                    Component card = cards[cardIndex];
                    if (card != null) {
                        card.setLocation(x, y);
                    }
                    if (!packCardsInHand || card != null) {
                        x += gap;
                    }
                }
                break;
            case 3: // East
                for (int cardIndex = 0; cardIndex < maxHandSize; cardIndex++) {
                    Component card = cards[cardIndex];
                    if (card != null) {
                        card.setLocation(x, y);
                    }
                    if (!packCardsInHand || card != null) {
                        y += gap;
                    }
                }
                break;
            }
        }
    }

    /**
     * Returns true if there is at least one card in the player's hand that is
     * face up.
     * 
     * @param playerIndex
     * @return
     */
    protected boolean isHandFaceUp(int playerIndex) {
        boolean isHandFaceUp = false;
        Component[] cards = this.cardsInHand[playerIndex];
        for (int cardIndex = 0; cardIndex < maxHandSize; cardIndex++) {
            Component card = cards[cardIndex];
            if (card != null) {
                isHandFaceUp |= ((Sprite) card).card().isFaceUp();
            }
        }
        return isHandFaceUp;
    }
    
    protected int getCardFanGap(int playerIndex) {
        return isHandFaceUp(playerIndex) ? cardFanGap : FACE_DOWN_CARD_GAP;
    }

    protected void layoutCardInTrick(Container parent, int playerIndex,
            Component card) {
        card.setLocation(getTrickPos(parent, playerIndex));
    }

    protected Rectangle getMaxHandRect(Container parent, int playerIndex) {
        return getHandRect(parent, playerIndex, maxHandSize);
    }

    protected Rectangle getHandRect(Container parent, int playerIndex) {
        return getHandRect(parent, playerIndex, getHandSize(playerIndex));
    }

    protected Rectangle getHandRect(Container parent, int playerIndex,
            int handSize) {
        Rectangle rect = new Rectangle(0, 0, 0, 0);
        switch (playerIndex) {
        case 0: // South
        case 2: // North
            rect.width = ((handSize - 1) * getCardFanGap(playerIndex)) + cardWidth;
            rect.height = cardHeight;
            int labelWidth = playerLabels[playerIndex] == null ? 0
                    : playerLabels[playerIndex].getWidth();
            int padding = (parent.getWidth() - (labelWidth + rect.width)) / 2;

            if (playerIndex == 0) {
                rect.x = padding;
                // Adjust for SE corner.
                if (southEastCorner != null) {
                    int diff = southEastCorner.getWidth() - padding;
                    if (diff > 0) {
                        rect.x -= diff;
                    }
                }
                rect.y = parent.getHeight() - (rect.height * 80 / 100);
            } else if (playerIndex == 2) {
                rect.x = padding + labelWidth;
                rect.y = -rect.height * 60 / 100;
            }
            break;
        case 1: // WEST
        case 3: // EAST
            rect.width = cardHeight;
            rect.height = ((handSize - 1) * getCardFanGap(playerIndex)) + cardWidth;
            int labelHeight = playerLabels[playerIndex] == null ? 0
                    : playerLabels[playerIndex].getHeight();
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

    /**
     * Gets the number of cards that we need to reserve space for.
     * 
     * @param player_pos
     * @return
     */
    protected int getHandSize(int player_pos) {
        if (packCardsInHand) {
            // Need to count the non-null cards.
            int count = 0;
            Component[] hand = cardsInHand[player_pos];
            for (int cardIndex = 0; cardIndex < hand.length; cardIndex++) {
                if (hand[cardIndex] != null) {
                    count++;
                }
            }
            return count;
        }
        return maxHandSize;
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
            x += (getCardFanGap(playerIndex) * (maxHandSize - cardIndex));
            break;
        case 1: // West
            y += (getCardFanGap(playerIndex) * (maxHandSize - cardIndex));
            break;
        case 2: // North
            x += (getCardFanGap(playerIndex) * cardIndex);
            break;
        case 3: // East
            y += (getCardFanGap(playerIndex) * cardIndex);
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
        if (!removed && statusLabel != null) {
            if (statusLabel == comp) {
                statusLabel = null;
            }
        }
        if (!removed && buttonPanel != null) {
            if (buttonPanel == comp) {
                buttonPanel = null;
            }
        }
        if (!removed && southEastCorner != null) {
            if (southEastCorner == comp) {
                southEastCorner = null;
            }
        }
        if (!removed && southWestCorner != null) {
            if (southWestCorner == comp) {
                southWestCorner = null;
            }
        }
        if (!removed && northEastCorner != null) {
            if (northEastCorner == comp) {
                northEastCorner = null;
            }
        }
        if (!removed && northWestCorner != null) {
            if (northWestCorner == comp) {
                northWestCorner = null;
            }
        }
    }

    public int getCardWidth() {
        return cardWidth;
    }

    public int getCardHeight() {
        return cardHeight;
    }
}