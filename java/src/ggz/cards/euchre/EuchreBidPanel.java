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
package ggz.cards.euchre;

import ggz.cards.BidPanel;
import ggz.cards.client.Client;
import ggz.cards.common.Bid;

public class EuchreBidPanel extends BidPanel {
    /* Special bid values. */
    private static final int PASS = 0;

    private static final int TAKE = 1;

    private static final int GO_ALONE = 3;

    private static final int GO_TEAM = 4;

    public EuchreBidPanel(int firstBidder, Client cardClient) {
        super(firstBidder, cardClient);
    }

    protected String getBidText(Bid bid) {
        if (bid == null) {
            return null;
        }

        switch (bid.getSpec()) {
//        case NORMAL_BID:
//            return String.valueOf(bid.getVal());
        case PASS:
            return "Pass";
        case TAKE:
            return "Take";
        case GO_ALONE:
            return "Go alone";
        case GO_TEAM:
            return "Go team";
        default:
            return String.valueOf(bid.getVal());
//            throw new UnsupportedOperationException("Unrecognised bid: " + bid);
        }
    }

}
