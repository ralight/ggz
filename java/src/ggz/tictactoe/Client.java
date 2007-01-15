/*
 * Copyright (C) 2007  Helg Bredow
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
package ggz.tictactoe;

import ggz.common.SeatType;
import ggz.common.dio.DIOInputStream;
import ggz.common.dio.DIOOutputStream;

import java.io.EOFException;
import java.io.IOException;
import java.net.Socket;
import java.util.logging.Logger;

import javax.swing.SwingUtilities;

public class Client {

    /* Tic-Tac-Toe protocol */
    /* Messages from server */
    private static final int TTT_MSG_SEAT = 0;

    private static final int TTT_MSG_PLAYERS = 1;

    private static final int TTT_MSG_MOVE = 2;

    private static final int TTT_MSG_GAMEOVER = 3;

    private static final int TTT_REQ_MOVE = 4;

    private static final int TTT_RSP_MOVE = 5;

    private static final int TTT_SND_SYNC = 6;

    /* Move errors */
    private static final int TTT_ERR_STATE = -1;

    private static final int TTT_ERR_TURN = -2;

    private static final int TTT_ERR_BOUND = -3;

    private static final int TTT_ERR_FULL = -4;

    /* Messages from client */
    private static final int TTT_SND_MOVE = 0;

    // private static final int TTT_REQ_SYNC = 1;

    /* ttt client game states */
    private static final int STATE_INIT = 0;

    private static final int STATE_WAIT = 1;

    private static final int STATE_MOVE = 2;

    private static final int STATE_DONE = 3;

    /* Events that we can fire */
    private static final int EVENT_BOARD_CHANGED = 0;

    private static final int EVENT_CANCEL_MOVE = 1;

    private static final int EVENT_GAME_STATUS = 2;

    private static final int EVENT_MOVE_REQUESTED = 3;

    private static final int EVENT_SEAT_CHANGED = 4;

    /* Basic info about connection */

    protected Socket socket;

    private DIOInputStream dio;

    private DIOOutputStream out;

    private int num;

    private SeatType[] seats = new SeatType[2];

    private String[] names = new String[2];

    /* TTT game specifics */
    private char[] board = new char[9];

    private byte state;

    private int mostRecentMove;

    private TicTacToeListener listener;

    private static final Logger log = Logger.getLogger(Client.class.getName());

    public Client(Socket s) throws IOException {
        this.socket = s;
        this.dio = new DIOInputStream(s.getInputStream());
        this.out = new DIOOutputStream(s.getOutputStream());
    }

    public String getPlayerName(int seatNum) {
        return this.names[seatNum];
    }

    public char getPlayerSymbol(int seatNum) {
        if (!(seatNum >= -1 && seatNum <= 1))
            throw new IndexOutOfBoundsException("seatNum must be 0 or 1");

        if (seatNum == 0)
            return 'O';
        else if (seatNum == 1)
            return 'X';
        else
            return '-';
    }

    public int getMySeat() {
        return this.num;
    }

    /** Starts listening for messages from the server. */
    public void start() {
        gameInit();
        new Thread(new Runnable() {
            public void run() {
                try {
                    while (true) {
                        readPacket();
                    }
                } catch (EOFException e) {
                    // Ignore, end of input.
                } catch (IOException e) {
                    e.printStackTrace();
                } finally {
                    try {
                        Client.this.socket.close();
                    } catch (IOException e2) {
                        // ignore.
                    }
                }
            }
        }).start();
    }

    protected void readPacket() throws IOException {
        this.dio.startPacket();

        int op = this.dio.readInt();

        switch (op) {

        case TTT_MSG_SEAT:
            receiveSeat();
            break;

        case TTT_MSG_PLAYERS:
            receivePlayers();
            this.state = STATE_WAIT;
            break;

        case TTT_REQ_MOVE:
            this.state = STATE_MOVE;
            fireEvent(EVENT_MOVE_REQUESTED, null);
            fireEvent(EVENT_GAME_STATUS, "It's your move.");
            break;

        case TTT_RSP_MOVE:
            receiveMoveStatus();
            break;

        case TTT_MSG_MOVE:
            receiveMove();
            break;

        case TTT_SND_SYNC:
            receiveSync();
            break;

        case TTT_MSG_GAMEOVER:
            receiveGameOver();
            this.state = STATE_DONE;
            break;
        }
    }

    private void receiveSeat() throws IOException {
        log.fine("Receiving my seat number...");
        this.num = this.dio.readInt();
    }

    private void receivePlayers() throws IOException {
        log.fine("Getting player names...");
        int numPlayersInGame = 0;
        for (int seatNum = 0; seatNum < 2; seatNum++) {
            int seat = this.dio.readInt();
            String seatName = null;

            this.seats[seatNum] = SeatType.valueOf(seat);

            if (this.seats[seatNum] == SeatType.GGZ_SEAT_NONE) {
                throw new UnsupportedOperationException(
                        "Seat type GGZ_SEAT_NONE not supported.");
            } else if (this.seats[seatNum] == SeatType.GGZ_SEAT_OPEN) {
                seatName = "Empty Seat";
            } else if (this.seats[seatNum] == SeatType.GGZ_SEAT_RESERVED) {
                seatName = "Reserved for " + this.dio.readString();
            } else if (this.seats[seatNum] == SeatType.GGZ_SEAT_ABANDONED) {
                seatName = this.dio.readString() + " has left the game";
                if (this.state == STATE_MOVE) {
                    fireEvent(EVENT_CANCEL_MOVE, null);
                }
            } else {
                seatName = this.dio.readString();
                numPlayersInGame++;
            }
            this.names[seatNum] = seatName + " (" + getPlayerSymbol(seatNum)
                    + ")";
            fireEvent(EVENT_SEAT_CHANGED, new Integer(seatNum));
        }

        if (numPlayersInGame != 2) {
            fireEvent(EVENT_GAME_STATUS, "Waiting for a player to join...");
        } else {
            fireEvent(EVENT_GAME_STATUS, "Opponent is thinking...");
        }
    }

    /*
     * The server doesn't usually inform us of our move. But we get told about
     * the opponent's move, and if we're a spectator we get to hear both.
     */
    private void receiveMove() throws IOException {
        // seat is the player who made the move (0 or 1).
        int seat = this.dio.readInt();

        // move is the move (0..8).
        int move = this.dio.readInt();
        char symbol = getPlayerSymbol(seat);

        if (this.num < 0)
            log.fine(this.names[seat] + " has moved.");
        else
            log.fine("Your opponent has moved.");

        this.board[move] = symbol;
        fireEvent(EVENT_BOARD_CHANGED, null);
    }

    private void receiveSync() throws IOException {
        log.fine("Syncing with server...");
        byte turn = this.dio.readByte();

        if (this.num == -1) {
            // We are a spectator.
            switch (turn) {
            case 0:
            case 1:
                fireEvent(EVENT_GAME_STATUS, getPlayerName(turn)
                        + " is thinking...");
                break;
            default:
                fireEvent(EVENT_GAME_STATUS, "Waiting for a player to join...");
                break;
            }
        } else if (turn == this.num) {
            fireEvent(EVENT_GAME_STATUS, "It's your move.");
        } else if (turn == 1 % this.num) {
            fireEvent(EVENT_GAME_STATUS, "Opponent is thinking...");
        } else {
            fireEvent(EVENT_GAME_STATUS, "Waiting for a player to join...");
        }

        for (int i = 0; i < 9; i++) {
            byte space = this.dio.readByte();
            if (space >= 0)
                this.board[i] = getPlayerSymbol(space);
        }

        fireEvent(EVENT_BOARD_CHANGED, null);
        log.fine("Sync completed.");
    }

    private void receiveGameOver() throws IOException {
        byte winner = this.dio.readByte();

        log.fine("Game over.");

        switch (winner) {
        case 0:
        case 1:
            fireEvent(EVENT_GAME_STATUS, this.names[winner] + " won!");
            break;
        case 2:
            fireEvent(EVENT_GAME_STATUS, "Tie game!");
            break;
        default:
            throw new IllegalStateException(
                    "Received invalid winner from server: " + winner);
        }
    }

    private void receiveMoveStatus() throws IOException {
        byte status = this.dio.readByte();

        switch (status) {
        case TTT_ERR_STATE:
            fireEvent(EVENT_GAME_STATUS, "Server not ready!!");
            break;
        case TTT_ERR_TURN:
            fireEvent(EVENT_GAME_STATUS, "Not your turn!");
            break;
        case TTT_ERR_BOUND:
            fireEvent(EVENT_GAME_STATUS, "Move out of bounds!");
            break;
        case TTT_ERR_FULL:
            fireEvent(EVENT_GAME_STATUS, "Space already occupied!");
            break;
        case 0:
            fireEvent(EVENT_GAME_STATUS, "Opponent is thinking...");
            this.board[this.mostRecentMove] = getPlayerSymbol(this.num);
            fireEvent(EVENT_BOARD_CHANGED, null);
            break;
        }
    }

    public void sendMyMove(int cellIndex) throws IOException {
        if (this.num < 0)
            throw new IllegalStateException();

        if (this.state != STATE_MOVE) {
            if (this.num >= 0)
                fireEvent(EVENT_GAME_STATUS, "It's not your move yet.");
            else
                fireEvent(EVENT_GAME_STATUS, "You're just watching.");
            return;
        }

        fireEvent(EVENT_GAME_STATUS, "Sending move.");
        this.mostRecentMove = cellIndex;

        out.writeInt(TTT_SND_MOVE);
        out.writeInt(cellIndex);
        out.endPacket();

        this.state = STATE_WAIT;
    }

    private void gameInit() {
        this.state = STATE_INIT;
        this.num = -1;

        for (int i = 0; i < 9; i++)
            this.board[i] = ' ';

        fireEvent(EVENT_BOARD_CHANGED, null);
    }

    /**
     * Fires the event on the AWT event queue. This makes the UI a lot easier to
     * program since event handlers are guaranteed to be invoked on the AWT
     * event queue so no deadlocks can occur.
     * 
     * @param eventId
     * @param eventData
     */
    private void fireEvent(final int eventId, final Object eventData) {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                switch (eventId) {
                case EVENT_GAME_STATUS:
                    fireGameStatus((String) eventData);
                    break;
                case EVENT_BOARD_CHANGED:
                    fireBoardChanged();
                    break;
                case EVENT_CANCEL_MOVE:
                    fireCancelMove();
                    break;
                case EVENT_MOVE_REQUESTED:
                    fireMoveRequested();
                    break;
                case EVENT_SEAT_CHANGED:
                    fireSeatChanged(((Integer) eventData).intValue());
                    break;
                default:
                    throw new UnsupportedOperationException(
                            "Unsupported event type: " + eventId);
                }
            }
        });
    }

    protected void fireGameStatus(String message) {
        if (this.listener != null)
            this.listener.gameStatus(message);
    }

    protected void fireBoardChanged() {
        if (this.listener != null)
            this.listener.boardChanged(this.board);
    }

    protected void fireSeatChanged(int seatNum) {
        if (this.listener != null)
            this.listener.seatChanged(seatNum);
    }

    protected void fireMoveRequested() {
        if (this.listener != null)
            this.listener.moveRequested();
    }

    protected void fireCancelMove() {
        if (this.listener != null)
            this.listener.cancelMove();
    }

    public void addTicTacToeListener(TicTacToeListener l) {
        if (this.listener != null && l != this.listener) {
            throw new UnsupportedOperationException(
                    "Multiple listeners not supported yet.");
        }
        this.listener = l;
    }
}
