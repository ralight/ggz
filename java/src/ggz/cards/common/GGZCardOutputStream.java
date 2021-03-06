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

import ggz.common.dio.DIOOutputStream;

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
public class GGZCardOutputStream extends DIOOutputStream {

    public GGZCardOutputStream(OutputStream os) {
        super(os);
    }

    public void write_card(Card card) throws IOException {
        byte face;
        // The face depends on the suit.
        if (card.getFace() == Face.UNKNOWN_FACE) {
            face = -1;
        } else if (card.getFace() == Face.JOKER1) {
            face = 0;
        } else if (card.getFace() == Face.JOKER2) {
            face = 1;
        } else if (card.getFace() == Face.ACE_LOW) {
            face = 1;
        } else if (card.getFace() == Face.DEUCE) {
            face = 2;
        } else if (card.getFace() == Face.THREE) {
            face = 3;
        } else if (card.getFace() == Face.FOUR) {
            face = 4;
        } else if (card.getFace() == Face.FIVE) {
            face = 5;
        } else if (card.getFace() == Face.SIX) {
            face = 6;
        } else if (card.getFace() == Face.SEVEN) {
            face = 7;
        } else if (card.getFace() == Face.EIGHT) {
            face = 8;
        } else if (card.getFace() == Face.NINE) {
            face = 9;
        } else if (card.getFace() == Face.TEN) {
            face = 10;
        } else if (card.getFace() == Face.JACK) {
            face = 11;
        } else if (card.getFace() == Face.QUEEN) {
            face = 12;
        } else if (card.getFace() == Face.KING) {
            face = 13;
        } else if (card.getFace() == Face.ACE_HIGH) {
            face = 14;
        } else {
            throw new IllegalArgumentException(
                    "Attempt to write card with unrecognised card face: "
                            + card.getFace());
        }
        write(face);
        write(encode_suit(card.getSuit()));
        write(card.deck);
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

    private static byte encode_suit(Suit suit) {
        if (suit == Suit.NO_SUIT) {
            return -2;
        } else if (suit == Suit.UNKNOWN_SUIT) {
            return -1;
        } else if (suit == Suit.CLUBS) {
            return 0;
        } else if (suit == Suit.DIAMONDS) {
            return 1;
        } else if (suit == Suit.HEARTS) {
            return 2;
        } else if (suit == Suit.SPADES) {
            return 3;
        } else {
            throw new IllegalStateException(
                    "Attempt to encode unrecognised card suit: " + suit);
        }
    }
}
