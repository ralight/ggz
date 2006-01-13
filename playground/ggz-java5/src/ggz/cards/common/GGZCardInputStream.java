package ggz.cards.common;

import java.io.DataInputStream;
import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;

public class GGZCardInputStream extends DataInputStream {

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
        read(b);
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

    public Bid read_bid() throws IOException {
        byte[] b = new byte[4];
        read(b);
        return new Bid(b[0], b[1], b[2], b[3]);
    }

    public ServerOpCode read_opcode() throws IOException {
        int index = read();
        if (index < 0) {
            throw new EOFException();
        }
        return ServerOpCode.values()[index];
    }

    public GameMessage read_game_message() throws IOException {
        int index = read();
        return GameMessage.values()[index];
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
            suit = Suit.NO_SUIT;
            break;
        default:
            throw new UnsupportedOperationException(
                    "Read unknown value for card suit: " + b);
        }
        return suit;
    }

    /*
     * Read a char string from the given fd and allocate space for it.
     */
    public String read_string() throws IOException {
        int size = readInt();
        byte[] chars = new byte[size];
        int status;
        String message;

        if ((status = read(chars)) < 0) {
            throw new EOFException();
        }

        if (status < size) {
            throw new IOException("Read too few chars expected " + size
                    + " but got " + status + ": "
                    + new String(chars, 0, status, "ISO-8859-1"));
        }
        // TODO check if ISO-8859-1 encoding is appropriate.
        message = new String(chars, 0, status - 1, "ISO-8859-1");
        return message;
    }
}
