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

    public Client(Socket s) throws IOException {
        this.socket = s;
        this.dio = new DIOInputStream(s.getInputStream());
        this.out = new DIOOutputStream(s.getOutputStream());
    }

    public char getPlayerSymbol(int player) {
        if (!(player >= -1 && player <= 1))
            throw new IndexOutOfBoundsException("player must be 0 or 1");

        if (player == 0)
            return 'O';
        else if (player == 1)
            return 'X';
        else
            return '-';
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
            // TODO enable the board to allow player input.
            gameStatus("It's your move.");
            break;

        case TTT_RSP_MOVE:
            receiveMoveStatus();
            displayBoard();
            break;

        case TTT_MSG_MOVE:
            receiveMove();
            displayBoard();
            break;

        case TTT_SND_SYNC:
            receiveSync();
            displayBoard();
            break;

        case TTT_MSG_GAMEOVER:
            receiveGameOver();
            this.state = STATE_DONE;
            break;
        }
    }

    private void receiveSeat() throws IOException {
        gameStatus("Receiving your seat number...");

        this.num = this.dio.readInt();

        gameStatus("Waiting for other player...");
    }

    private void receivePlayers() throws IOException {
        gameStatus("Getting player names...");
        for (int seatNum = 0; seatNum < 2; seatNum++) {
            int seat = this.dio.readInt();
            this.seats[seatNum] = SeatType.valueOf(seat);

            if (this.seats[seatNum] != SeatType.GGZ_SEAT_OPEN) {
                this.names[seatNum] = this.dio.readString();
                gameStatus(this.names[seatNum] + " is "
                        + getPlayerSymbol(seatNum) + ".");
            }
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
            gameStatus(this.names[seat] + " (" + symbol + ") has moved.");
        else
            gameStatus("Your opponent has moved.");

        this.board[move] = symbol;
    }

    private void receiveSync() throws IOException {
        byte turn = this.dio.readByte();

        gameStatus("Syncing with server...");
        gameStatus("It's " + this.names[turn] + " (" + getPlayerSymbol(turn)
                + ")'s turn.");

        for (int i = 0; i < 9; i++) {
            byte space = this.dio.readByte();
            if (space >= 0)
                this.board[i] = getPlayerSymbol(turn);
        }

        gameStatus("Sync completed.");
    }

    private void receiveGameOver() throws IOException {
        byte winner = this.dio.readByte();

        gameStatus("Game over.");

        switch (winner) {
        case 0:
        case 1:
            gameStatus(this.names[winner] + " (" + getPlayerSymbol(winner)
                    + ") won.");
            break;
        case 2:
            gameStatus("Tie game!");
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
            gameStatus("Server not ready!!");
            break;
        case TTT_ERR_TURN:
            gameStatus("Not your turn!");
            break;
        case TTT_ERR_BOUND:
            gameStatus("Move out of bounds!");
            break;
        case TTT_ERR_FULL:
            gameStatus("Space already occupied!");
            break;
        case 0:
            gameStatus("Move accepted.");
            this.board[this.mostRecentMove] = getPlayerSymbol(this.num);
            break;
        }
    }

    public void sendMyMove(int cellIndex) throws IOException {
        if (this.num < 0)
            throw new IllegalStateException();

        if (this.state != STATE_MOVE) {
            if (this.num >= 0)
                gameStatus("It's not your move yet.");
            else
                gameStatus("You're just watching.");
            return;
        }

        gameStatus("Sending move.");
        this.mostRecentMove = cellIndex;

        out.writeInt(TTT_SND_MOVE);
        out.writeInt(cellIndex);
        out.endPacket();

        this.state = STATE_WAIT;
    }

    private void gameInit() {
        for (int i = 0; i < 9; i++)
            this.board[i] = ' ';

        this.state = STATE_INIT;

        this.num = -1;
        displayBoard();
    }

    private void gameStatus(String message) {
        if (this.listener != null)
            this.listener.gameStatus(message);
    }

    private void displayBoard() {
        if (this.listener != null)
            this.listener.boardUpdated(this.board);
    }

    public void addTicTacToeListener(TicTacToeListener l) {
        if (this.listener != null && l != this.listener) {
            throw new UnsupportedOperationException(
                    "Multiple listeners not supported yet.");
        }
        this.listener = l;
    }
}
