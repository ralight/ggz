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
