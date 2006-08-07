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
package ggz.games;

import ggz.cards.SmartChatLayout;
import ggz.client.mod.ModGame;
import ggz.client.mod.ModEventHandler;
import ggz.client.mod.ModState;
import ggz.client.mod.Seat;
import ggz.client.mod.SpectatorSeat;
import ggz.common.AdminType;
import ggz.common.ChatType;
import ggz.common.PlayerInfo;
import ggz.ui.ChatAction;
import ggz.ui.ChatPanel;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.net.Socket;

import javax.swing.AbstractAction;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;
import javax.swing.WindowConstants;

/**
 * Base class that the main UI panel for all games should inherit from. This
 * class handles the communication with the game client.
 * 
 * @author Helg.Bredow
 * 
 */
public class GamePanel extends JPanel implements ModEventHandler {
    // private static final Logger log = Logger.getLogger(GamePanel.class
    // .getName());

    protected ModGame ggzMod;

    protected ChatPanel chatPanel;

    protected SpectatorListPanel playerListPanel;

    protected GamePanel() {
        super(new SmartChatLayout());
        chatPanel = new ChatPanel(new TableChatAction());
        chatPanel.setPreferredSize(new Dimension(200, 150));
        playerListPanel = new SpectatorListPanel();
        add(chatPanel, SmartChatLayout.CHAT);
        add(playerListPanel, SmartChatLayout.SPECTATOR_LIST);
        setOpaque(true);
    }

    public void init(ModGame mod) throws IOException {
        this.ggzMod = mod;
        playerListPanel.setMod(mod);
        mod.setHandler(this);
    }

    protected void quit() {
        try {
            ggzMod.disconnect();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void handleChat(final String player, final String msg) {
        // Ignore messages that we type since they have already been echoed
        // locally.
        if (player.equals(ggzMod.getMyName())) {
            return;
        }

        // All handlers are called from the socket thread so we need to do
        // this crazy stuff. This method is usually invoked from a handler.
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                chatPanel.appendChat(player, msg, ggzMod.getMyName());
            }
        });
    }

    public void handleError(final String msg) {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                JOptionPane.showMessageDialog(GamePanel.this, msg);
            }
        });
    }

    public void handleState(ModState oldState) {
        if (ggzMod.getState() == ModState.GGZMOD_STATE_CONNECTED) {
            JFrame frame = new JFrame();
            frame.getContentPane().add(this, BorderLayout.CENTER);
            frame.addWindowListener(new WindowAdapter() {
                public void windowClosing(WindowEvent e) {
                    quit();
                }
            });
            frame.setSize(800, 600);
            // frame.setLocationByPlatform(true);
            frame.setLocationRelativeTo(null);
            frame.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
            frame.show();
        } else if (ggzMod.getState() == ModState.GGZMOD_STATE_DONE) {
            // Prevent memory leaks.
            chatPanel.dispose();
            JOptionPane.getFrameForComponent(this).dispose();
        }
    }

    public void handlePlayer(String name, boolean is_spectator, int seat_num) {
        // chat_panel.handle_chat("handle_player", "name=" + name
        // + " is_spectator=" + is_spectator + " seat_num=" + seat_num);
        // System.out.println("handle_player(" + name + ", " + is_spectator + ",
        // "
        // + seat_num + ")");
    }

    /**
     * This is invoked when a message arrives from the core client. We handle
     * player messages in alert_player() above, which is invoked when a message
     * arrives from the game client.
     */
    public void handleSeat(Seat oldSeat) {
        // Do nothing here.
    }

    public void handleServer(Socket fd) throws IOException {
        // ggzMod.setState(ModState.GGZMOD_STATE_PLAYING);
    }

    public void handleStats() {
        // Let subclasses deal with displaying player stats.
    }

    public void handleInfo(PlayerInfo info) {
        // Let subclasses deal with displaying player info.
    }

    public void handleSpectatorSeat(final SpectatorSeat oldSeat,
            final SpectatorSeat newSeat) {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                if (newSeat.getName() == null) {
                    playerListPanel.removeSpectator(oldSeat);
                } else {
                    playerListPanel.addSpectator(newSeat);
                }
            }
        });
    }

    public void handleException(Throwable e) {
        e.printStackTrace();
        JOptionPane.showMessageDialog(this, e);
    }

    public void invokeAndWait(Runnable doRun) {
        try {
            SwingUtilities.invokeAndWait(doRun);
        } catch (InvocationTargetException e) {
            // TODO work out a way to avoid this exception, it happens when the
            // dialog is closed while cards are being animated.
        } catch (InterruptedException e) {
            handleException(e);
        }
    }

    protected class TableChatAction extends ChatAction {

        public boolean sendChat(ChatType type, String target, String message)
                throws IOException {
            ggzMod.requestChat(type, target, message);
            return true;
        }

        protected boolean sendAdmin(AdminType type, String player, String reason)
                throws IOException {
            chatPanel
                    .appendCommandText("You can't issue admin commands from a table.");
            return false;
        }

        protected void chat_display_local(ChatType type, String message) {
            chatPanel.appendChat(type, ggzMod.getMyName(), message, ggzMod
                    .getMyName());
        }

        protected ChatType getDefaultChatType() {
            return ChatType.GGZ_CHAT_TABLE;
        }
    }

    protected class SeatBotAction extends AbstractAction {
        private int seat_num;

        public SeatBotAction(int seat_num) {
            super("Put computer player here");
            this.seat_num = seat_num;
        }

        public void actionPerformed(ActionEvent event) {
            try {
                ggzMod.requestBot(seat_num);
            } catch (IOException e) {
                handleException(e);
            }
        }
    }

    protected class SeatBootAction extends AbstractAction {
        private String playerName;

        public SeatBootAction(String playerName) {
            super("Boot this player from the game");
            this.playerName = playerName;
        }

        public void actionPerformed(ActionEvent event) {
            if (JOptionPane.showConfirmDialog(GamePanel.this,
                    "Are you sure you want to boot " + playerName
                            + " from the game?", "Boot",
                    JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION) {
                try {
                    ggzMod.requestBoot(playerName);
                } catch (IOException e) {
                    handleException(e);
                }
            }
        }
    }

    protected class SeatOpenAction extends AbstractAction {
        private int seat_num;

        public SeatOpenAction(int seat_num) {
            super("Make this seat available for a human to play");
            this.seat_num = seat_num;
        }

        public void actionPerformed(ActionEvent event) {
            try {
                ggzMod.requestOpen(seat_num);
            } catch (IOException e) {
                handleException(e);
            }
        }
    }

    protected class SeatSitAction extends AbstractAction {
        private int seat_num;

        public SeatSitAction(int seat_num) {
            super("Move here");
            this.seat_num = seat_num;
        }

        public void actionPerformed(ActionEvent event) {
            try {
                ggzMod.requestSit(seat_num);
            } catch (IOException e) {
                handleException(e);
            }
        }
    }

    protected class SeatStandAction extends AbstractAction {
        public SeatStandAction() {
            super("Spectate");
        }

        public void actionPerformed(ActionEvent event) {
            try {
                ggzMod.requestStand();
            } catch (IOException e) {
                handleException(e);
            }
        }
    }
}
