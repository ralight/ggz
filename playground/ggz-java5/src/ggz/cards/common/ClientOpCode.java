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
package ggz.cards.common;

/** Messages from client */
public enum ClientOpCode {
    /*
     * Notifies the server of the client's language. Followed by a string
     * including the language.
     */
    MSG_LANGUAGE,

    /* A newgame response, sent in response to a REQ_NEWGAME. No data. */
    RSP_NEWGAME,

    /*
     * An options response, sent in response to a REQ_OPTIONS. It consists of a
     * number n, followed by a list of n integers, each representing an option
     * selection. The number of options n must be what REQ_OPTIONS has
     * requested.
     */
    RSP_OPTIONS,

    /*
     * A play response, sent in response to a REQ_PLAY. It is followed by a card
     * that the client/user wishes to play. Note that although the REQ_PLAY
     * gives a list of valid cards, the client need not conform to this list. In
     * particular, if the client tries to play an invalid card a MSG_BADPLAY
     * with an user-ready error message will be sent.
     */
    RSP_PLAY,

    /*
     * A bid response, sent in response to a REQ_BID. It consists of only an
     * integer giving the chosen bid (see REQ_BID).
     */
    RSP_BID,

    /*
     * A sync request. The server will send out all data again to sync the
     * client.
     */
    REQ_SYNC;
}
