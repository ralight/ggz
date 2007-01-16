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
package ggz.cards.spades;

import ggz.cards.BidPanel;
import ggz.cards.client.Client;
import ggz.cards.common.Bid;

public class SpadesBidPanel extends BidPanel {
    /* Special bid values. */
    private static final int NORMAL_BID = 0;

    private static final int NIL = 1;

    private static final int SHOW_CARDS = 2;

    private static final int DOUBLE_NIL = 3;

    public SpadesBidPanel(int firstBidder, Client client) {
        super(firstBidder, client);
    }

    protected String getBidText(Bid bid) {
        if (bid == null) {
            return null;
        }

        switch (bid.getSpec()) {
        case NORMAL_BID:
            return String.valueOf(bid.getVal());
        case NIL:
            return "Nil";
        case SHOW_CARDS:
            return "Show cards";
        case DOUBLE_NIL:
            return "Dnil";
        default:
            throw new UnsupportedOperationException("Unrecognised bid: " + bid);
        }
    }
}
