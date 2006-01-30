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

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;

public class GGZCardOutputStream extends DataOutputStream {

    public GGZCardOutputStream(OutputStream out) {
        super(out);
    }

    public void write_card(Card card) throws IOException {
        byte face;
        // The face depends on the suit.
        switch (card.get_face()) {
        case UNKNOWN_FACE:
            face = -1;
            break;
        case JOKER1:
            face = 0;
            break;
        case JOKER2:
            face = 1;
            break;
        case ACE_LOW:
            face = 1;
            break;
        case DEUCE:
            face = 2;
            break;
        case THREE:
            face = 3;
            break;
        case FOUR:
            face = 4;
            break;
        case FIVE:
            face = 5;
            break;
        case SIX:
            face = 6;
            break;
        case SEVEN:
            face = 7;
            break;
        case EIGHT:
            face = 8;
            break;
        case NINE:
            face = 9;
            break;
        case TEN:
            face = 10;
            break;
        case JACK:
            face = 11;
            break;
        case QUEEN:
            face = 12;
            break;
        case KING:
            face = 13;
            break;
        case ACE_HIGH:
            face = 14;
            break;
        default:
            throw new IllegalArgumentException(
                    "Attempt to write card with unrecognised card face: "
                            + card.get_face());
        }
        write(face);
        write(encode_suit(card.get_suit()));
        write(card.deck);
    }

    /**
     * @brief Writes a bid to the socket.
     * @param fd
     *            The file descriptor to which to read.
     * @param bid
     *            A pointer to the bid data.
     * @return 0 on success, -1 on failure.
     */
    public void write_bid(Bid bid) throws IOException {
        write(bid.val);
        write(bid.suit);
        write(bid.spec);
        write(bid.spec2);
    }

    public void write_opcode(ClientOpCode opcode) throws IOException {
        write(opcode.ordinal());
    }

    public void write_seat(int seat) throws IOException {
        assert (seat >= 0 && seat < 127);
        write(seat);
    }

    /*
     * Writes a char string to the given fd preceeded by its size. The string is
     * encoded using ISO-8859-1.
     */
    public void write_string(String message) throws IOException {
        byte[] bytes = message.getBytes("ISO-8859-1");
        int size = bytes.length + 1;

        writeInt(size);
        write(bytes);
        // I think we need to write the null terminator, if not then adjust size
        // above.
        write(0);
    }

    private static byte encode_suit(Suit suit) {
        switch (suit) {
        case UNKNOWN_SUIT:
            return -1;
        case CLUBS:
            return 0;
        case DIAMONDS:
            return 1;
        case HEARTS:
            return 2;
        case SPADES:
            return 3;
        case NO_SUIT:
            return 4;
        default:
            throw new IllegalStateException(
                    "Attempt to encode unrecognised card suit: " + suit);
        }
    }
}
