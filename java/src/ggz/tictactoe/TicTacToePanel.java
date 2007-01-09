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

import java.awt.BorderLayout;
import java.io.IOException;
import java.net.Socket;

import ggz.cards.SmartChatLayout;
import ggz.client.mod.ModState;
import ggz.games.GamePanel;

import javax.swing.BorderFactory;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;

/**
 * Player 0 = 0 Player 1 = X
 * 
 * @author Helg.Bredow
 * 
 */
public class TicTacToePanel extends GamePanel implements TicTacToeListener,
        BoardListener {

    private Client client;

    private JPanel boardAndStatusPanel;

    protected BoardPanel board;

    protected JLabel statusPanel;

    public TicTacToePanel() {
        this.boardAndStatusPanel = new JPanel(new BorderLayout());
        this.boardAndStatusPanel.setBorder(BorderFactory.createEmptyBorder(10,
                10, 10, 10));

        // Create the game board and add it.
        this.board = new BoardPanel();
        this.board.addBoardListener(this);
        this.boardAndStatusPanel.add(this.board, BorderLayout.CENTER);

        // Status panel for messages.
        this.statusPanel = new JLabel("Connecting...");
        this.boardAndStatusPanel.add(this.statusPanel, BorderLayout.SOUTH);

        add(this.boardAndStatusPanel, SmartChatLayout.TABLE);
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

    public void boardUpdated(final char[] boardData) {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                TicTacToePanel.this.board.refresh(boardData);
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
