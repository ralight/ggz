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

import ggz.cards.CardGamePanel;
import ggz.cards.TableConstraints;
import ggz.cards.common.Bid;

import java.util.logging.Logger;

public class SpadesGamePanel extends CardGamePanel {
    protected static final Logger log = Logger.getLogger(SpadesGamePanel.class
            .getName());

    protected SpadesBidPanel spadesBidPanel;

    public void alert_bid(final int bidder, final Bid bid) {
        // Ignore bids that the server sends that we aren't interested in. e.g.
        // No blind bid and those sent when joining mid game.
        if (!(bid.getVal() == 0 && bid.getSpec() == 2) && (bid.getSuit() == 4)) {
            super.alert_bid(bidder, bid);
        }
    }

    protected void createBidPanel(int firstBidder) {
        // Hang onto the SpadesBidPanel we create since we need it
        // to render bids.
        spadesBidPanel = new SpadesBidPanel(firstBidder, getCardClient());
        bidPanel = spadesBidPanel;
        bidPanel.addActionListener(this);
        table
                .add(bidPanel, new TableConstraints(
                        TableConstraints.BUTTON_PANEL));
        table.invalidate();
        table.validate();
    }
}
