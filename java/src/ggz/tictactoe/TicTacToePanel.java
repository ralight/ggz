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

import ggz.cards.SmartChatLayout;
import ggz.client.mod.ModState;
import ggz.games.GamePanel;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.io.IOException;
import java.net.Socket;

import javax.swing.BorderFactory;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;

/**
 * Player 0 = 0 Player 1 = X
 * 
 * @author Helg.Bredow
 * 
 */
public class TicTacToePanel extends GamePanel implements TicTacToeListener,
        BoardListener {

    protected Client client;

    private JPanel mainAndStatusPanel;

    private JPanel boardAndNamesPanel;

    protected BoardPanel board;

    protected JLabel statusPanel;

    protected JLabel[] seatLabel = new JLabel[2];

    public TicTacToePanel() {
        this.mainAndStatusPanel = new JPanel(new BorderLayout());
        this.boardAndNamesPanel = new JPanel(new BorderLayout());
        this.boardAndNamesPanel.setBorder(BorderFactory.createEmptyBorder(10,
                10, 10, 10));

        // Create the game board and add it.
        this.board = new BoardPanel();
        this.board.setEnabled(false);
        this.board.addBoardListener(this);
        this.boardAndNamesPanel.add(this.board, BorderLayout.CENTER);

        // Create the name labels. If we are playing we will be seat 0.
        this.seatLabel[0] = new JLabel("Empty Seat", SwingConstants.CENTER);
        this.boardAndNamesPanel.add(this.seatLabel[0], BorderLayout.SOUTH);
        this.seatLabel[1] = new JLabel("Empty Seat", SwingConstants.CENTER);
        this.boardAndNamesPanel.add(this.seatLabel[1], BorderLayout.NORTH);

        this.mainAndStatusPanel.add(this.boardAndNamesPanel,
                BorderLayout.CENTER);

        // Status panel for messages.
        this.statusPanel = new JLabel("Connecting...");
        this.mainAndStatusPanel.add(this.statusPanel, BorderLayout.SOUTH);

        add(this.mainAndStatusPanel, SmartChatLayout.TABLE);
    }
    
    protected Dimension getPreferredWindowSize() {
        return new Dimension(600, 300);
    }

    public void handleServer(Socket fd) throws IOException {
        super.handleServer(fd);
        this.client = new Client(fd);
        this.client.addTicTacToeListener(this);
        this.client.start();
        this.ggzMod.setState(ModState.GGZMOD_STATE_PLAYING);

        // Set the title of the window.
        JOptionPane.getFrameForComponent(this).setTitle("Tic-Tac-Toe");
    }

    public void boardChanged(final char[] boardData) {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                TicTacToePanel.this.board.refresh(boardData);
            }
        });
    }

    public void seatChanged(final int seatNum) {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                seatLabel[seatNum].setText(client.getPlayerName(seatNum));
            }
        });
    }

    public void moveRequested() {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                board.setUserSymbol(client.getPlayerSymbol(client.getMySeat()));
                board.setEnabled(true);
            }
        });
    }

    /**
     * Called when user input is not longer available because a player has left
     * the game and the game has gone into a wait state and won't accept the
     * move.
     */
    public void cancelMove() {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                board.setEnabled(false);
            }
        });
    }

    public void gameStatus(final String message) {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                TicTacToePanel.this.statusPanel.setText(message);
                TicTacToePanel.this.revalidate();
            }
        });
    }

    public void cellClicked(int cellIndex) {
        this.board.setEnabled(false);

        if (this.client == null)
            return;

        try {
            this.client.sendMyMove(cellIndex);
        } catch (IllegalStateException e) {
            handleException(e);
        } catch (IOException e) {
            handleException(e);
        }
    }
}
