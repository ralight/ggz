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

import ggz.common.dio.DIOInputStream;

import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;

public class GGZCardInputStream extends DIOInputStream {
    public GGZCardInputStream(InputStream in) {
        super(in);
    }

    public CardSetType read_cardset_type() throws IOException {
        int cardset = readInt();
        CardSetType cardset_type = CardSetType.valueOf(cardset);

        if (cardset_type != CardSetType.CARDSET_FRENCH) {
            throw new UnsupportedOperationException(
                    "Unsupported card set type: " + cardset_type);
        }

        return cardset_type;
    }

    public Card read_card() throws IOException {
        byte[] b = new byte[3];
        readFully(b);
        Face face;
        Suit suit;

        // Determine the suit first.
        suit = decode_suit(b[1]);

        // The face depends on the suit.
        switch (b[0]) {
        case -1:
            face = Face.UNKNOWN_FACE;
            break;
        case 0:
            face = Face.JOKER1;
            break;
        case 1:
            face = (suit == Suit.NO_SUIT) ? Face.JOKER2 : Face.ACE_LOW;
            break;
        case 2:
            face = Face.DEUCE;
            break;
        case 3:
            face = Face.THREE;
            break;
        case 4:
            face = Face.FOUR;
            break;
        case 5:
            face = Face.FIVE;
            break;
        case 6:
            face = Face.SIX;
            break;
        case 7:
            face = Face.SEVEN;
            break;
        case 8:
            face = Face.EIGHT;
            break;
        case 9:
            face = Face.NINE;
            break;
        case 10:
            face = Face.TEN;
            break;
        case 11:
            face = Face.JACK;
            break;
        case 12:
            face = Face.QUEEN;
            break;
        case 13:
            face = Face.KING;
            break;
        case 14:
            face = Face.ACE_HIGH;
            break;
        default:
            throw new IllegalStateException(
                    "Read invalid value for card face: " + b[0]);
        }

        return new Card(face, suit, b[2]);
    }

    public Suit read_suit() throws IOException {
        return decode_suit(readByte());
    }

    public Bid read_bid() throws IOException {
        byte[] b = new byte[4];
        readFully(b);
        return new Bid(b[0], decode_suit(b[1]), b[2], b[3]);
    }

    public ServerOpCode read_opcode() throws IOException {
        int index = read();
        if (index < 0) {
            throw new EOFException();
        }
        return ServerOpCode.valueOf(index);
    }

    public GameMessage read_game_message() throws IOException {
        int index = read();
        return GameMessage.valueOf(index);
    }

    public int read_seat() throws IOException {
        return read();
    }

    /**
     * This only supports suit in a French deck at the moment.
     * 
     * @param b
     * @return
     */
    private static Suit decode_suit(byte b) {
        Suit suit;
        switch (b) {
        case -2:
            suit = Suit.NO_SUIT;
            break;
        case -1:
            suit = Suit.UNKNOWN_SUIT;
            break;
        case 0:
            suit = Suit.CLUBS;
            break;
        case 1:
            suit = Suit.DIAMONDS;
            break;
        case 2:
            suit = Suit.HEARTS;
            break;
        case 3:
            suit = Suit.SPADES;
            break;
        case 4:
            suit = Suit.NO_TRUMP;
            break;
        default:
            throw new UnsupportedOperationException(
                    "Read unknown value for card suit: " + b);
        }
        return suit;
    }
}
