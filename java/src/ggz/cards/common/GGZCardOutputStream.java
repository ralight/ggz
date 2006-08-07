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

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;

/**
 * This class does not write anything to the output stream until the flush()
 * method is called. All writes first go into an internal buffer so that the
 * size of a packet can be determined. When the flush() method is called, a
 * "packet header" is written that contains the size of the packet (including
 * the header) in bytes.
 * 
 * @author Helg.Bredow
 * 
 */
public class GGZCardOutputStream extends DataOutputStream {
    private ByteArrayOutputStream buffer;

    private DataOutputStream output;

    public GGZCardOutputStream(OutputStream os) {
        super(new ByteArrayOutputStream());
        // The actual stream that will eventually be writtin to when the flush()
        // method is called.
        output = new DataOutputStream(os);
        // Cast our decorated stream back to what we know it is for convenience.
        // We have to do it this way because we can't create the buffer before
        // calling the super constructor.
        buffer = (ByteArrayOutputStream) out;
    }

    public void write_card(Card card) throws IOException {
        byte face;
        // The face depends on the suit.
        if (card.face() == Face.UNKNOWN_FACE) {
            face = -1;
        } else if (card.face() == Face.JOKER1) {
            face = 0;
        } else if (card.face() == Face.JOKER2) {
            face = 1;
        } else if (card.face() == Face.ACE_LOW) {
            face = 1;
        } else if (card.face() == Face.DEUCE) {
            face = 2;
        } else if (card.face() == Face.THREE) {
            face = 3;
        } else if (card.face() == Face.FOUR) {
            face = 4;
        } else if (card.face() == Face.FIVE) {
            face = 5;
        } else if (card.face() == Face.SIX) {
            face = 6;
        } else if (card.face() == Face.SEVEN) {
            face = 7;
        } else if (card.face() == Face.EIGHT) {
            face = 8;
        } else if (card.face() == Face.NINE) {
            face = 9;
        } else if (card.face() == Face.TEN) {
            face = 10;
        } else if (card.face() == Face.JACK) {
            face = 11;
        } else if (card.face() == Face.QUEEN) {
            face = 12;
        } else if (card.face() == Face.KING) {
            face = 13;
        } else if (card.face() == Face.ACE_HIGH) {
            face = 14;
        } else {
            throw new IllegalArgumentException(
                    "Attempt to write card with unrecognised card face: "
                            + card.face());
        }
        write(face);
        write(encode_suit(card.suit()));
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
        write(bid.getVal());
        write(bid.getSuit());
        write(bid.getSpec());
        write(bid.getSpec2());
    }

    public void write_opcode(ClientOpCode opcode) throws IOException {
        write(opcode.ordinal());
    }

    public void write_seat(int seat) throws IOException {
        if (seat < 0 || seat >= 127) {
            throw new IllegalArgumentException("seat must be >= 0 and < 127");
        }
        write(seat);
    }

    /**
     * Writes a char string to the given fd preceeded by its size. The string is
     * encoded using UTF-8.
     */
    public void write_string(String message) throws IOException {
        byte[] bytes = message.getBytes("UTF-8");
        int size = bytes.length + 1;

        writeInt(size);
        write(bytes);
        // I think we need to write the null terminator, if not then adjust size
        // above.
        write(0);
    }

    /**
     * Writes out the bytes that we have accumulated in previous write
     * operations, preceded by a packet header that specifies the size of the
     * packet.
     */
    public void end_packet() throws IOException {
        // Write the size of the packet that is being sent, this size must
        // include the two bytes for this header.
        output.writeShort(buffer.size() + 2);
        buffer.writeTo(output);
        output.flush();
        // Get ready for the next packet.
        buffer.reset();
    }

    private static byte encode_suit(Suit suit) {
        if (suit == Suit.UNKNOWN_SUIT) {
            return -1;
        } else if (suit == Suit.CLUBS) {
            return 0;
        } else if (suit == Suit.DIAMONDS) {
            return 1;
        } else if (suit == Suit.HEARTS) {
            return 2;
        } else if (suit == Suit.SPADES) {
            return 3;
        } else if (suit == Suit.NO_SUIT) {
            return 4;
        } else {
            throw new IllegalStateException(
                    "Attempt to encode unrecognised card suit: " + suit);
        }
    }
}
