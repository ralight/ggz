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
    public SpadesBidPanel(int firstBidder, Client client) {
        super(firstBidder, client);
    }

    protected String getBidText(Bid bid) {
        if (bid == null) {
            return null;
        }
        if (bid.getVal() == 0) {
            switch (bid.getSpec()) {
            case 0:
                return "0";
            case 1:
                return "Nil";
            case 2:
                return "Show cards";
            case 3:
                return "Dnil";
            default:
                throw new UnsupportedOperationException("Unrecognised bid: "
                        + bid);
            }
        }
        return String.valueOf(bid.getVal());
    }
}
