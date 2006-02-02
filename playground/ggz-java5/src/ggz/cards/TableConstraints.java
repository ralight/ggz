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

public class TableConstraints {
    /**
     * The player index for which this component belongs.
     */
    protected int playerIndex;

    /**
     * The purpose of this component. E.g. player lable, card in hand, card in
     * trick.
     */
    protected int type;

    protected int cardIndex;

    public static final int PLAYER_LABEL = 0;

    public static final int CARD_IN_HAND = 1;

    public static final int CARD_IN_TRICK = 2;
    
    public static final int STATUS_LABEL = 3;

    public TableConstraints(int type) {
        this.type = type;
    }
    
    public TableConstraints(int type, int playerIndex) {
        this(type);
        this.playerIndex = playerIndex;
    }

    public TableConstraints(int type, int playerIndex, int cardIndex) {
        this(type, playerIndex);
        this.cardIndex = cardIndex;
    }
}
