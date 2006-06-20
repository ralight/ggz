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
package ggz.ui;

import ggz.client.core.ChatEventData;
import ggz.client.core.ErrorEventData;
import ggz.client.core.Player;
import ggz.client.core.Room;
import ggz.client.core.RoomChangeEventData;
import ggz.client.core.RoomListener;
import ggz.client.core.Table;
import ggz.client.core.TableLeaveEventData;
import ggz.common.ChatType;
import ggz.common.PlayerType;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Graphics;
import java.io.IOException;
import java.util.ResourceBundle;
import java.util.logging.Logger;

import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.TableCellRenderer;

public class RoomChatPanel extends JPanel implements RoomListener {
    private static final Logger log = Logger.getLogger(RoomChatPanel.class
            .getName());

    private static final ResourceBundle messages = ResourceBundle
            .getBundle("ggz.ui.messages");

    protected Room room;

    /** Our nickname on server */
    protected String handle;

    protected ChatPanel chatPanel;

    private JScrollPane playerScrollPane;

    private JTable playerList;

    private PlayersTableModel players;

    public RoomChatPanel(boolean showTableNumber) {
        super(new BorderLayout());
        chatPanel = new ChatPanel(new RoomChatAction());
        add(chatPanel, BorderLayout.CENTER);

        // The list of players.
        players = new PlayersTableModel(showTableNumber);
        playerList = new JTable(players);
        // playerList.setRowHeight(20);
        // playerList.getTableHeader().setBackground(new Color(0xce, 0xfa,
        // 0xdf));
        playerList
                .getColumn("Nickname")
                .setHeaderValue(
                        messages
                                .getString("RoomChatPanel.ColumnHeader.PlayerNickname"));
        playerList.getColumn("Lag").setMaxWidth(20);
        playerList.getColumn("Lag").setHeaderValue("");
        playerList.getColumn("Type").setMaxWidth(40);
        playerList.getColumn("Type").setHeaderValue(
                messages.getString("RoomChatPanel.ColumnHeader.PlayerType"));
        if (showTableNumber) {
            playerList.getColumn("T#").setMaxWidth(20);
            playerList
                    .getColumn("T#")
                    .setHeaderValue(
                            messages
                                    .getString("RoomChatPanel.ColumnHeader.PlayerTableNumber"));
        }
        playerList.setRowSelectionAllowed(false);
        playerList.setGridColor(playerList.getBackground());
        // playerList.getSelectionModel().setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        setOpaque(false);
        playerList.setOpaque(false);
        playerList.setPreferredScrollableViewportSize(new Dimension(220, 100));
        playerList.setDefaultRenderer(PlayerType.class,
                new PlayerTypeCellRenderer());
        playerList.setDefaultRenderer(Player.class, new PlayerCellRenderer());
        playerList.setDefaultRenderer(Integer.class, new LagCellRenderer());
        JLabel rowHeightCalculator = new JLabel("Qwerty");
        rowHeightCalculator.setFont(playerList.getFont());
        playerList.setRowHeight(Math.max(16, rowHeightCalculator
                .getPreferredSize().height));
        playerScrollPane = new JScrollPane();
        playerScrollPane.setOpaque(false);
        playerScrollPane.getViewport().add(playerList);
        playerScrollPane.getViewport().setOpaque(false);
        add(playerScrollPane, BorderLayout.WEST);
    }

    public void setRoom(Room r) throws IOException {
        if (r != room) {
            if (room != null) {
                room.remove_event_hook(this);
            }
            room = r;
            handle = room.get_server().get_handle();
            room.add_event_hook(this);
            chatPanel.clearChat();
        }
        room.list_players();
    }

    public void chat_event(final ChatEventData data) {
        // Ignore normal chat messages from ourselves since we append the text
        // on send without waiting for the server to make the app feel more
        // responsive. We also ignore all table chat since this is the chat
        // panel for rooms and not tables.
        if ((!handle.equals(data.sender) || data.type != ChatType.GGZ_CHAT_NORMAL)
                && data.type != ChatType.GGZ_CHAT_TABLE) {
            // All handlers are called from the socket thread so we need to do
            // this crazy stuff.
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    chatPanel.appendChat(data.type, data.sender, data.message);
                }
            });
        }
    }

    public void player_count(int room_id) {
        // Seems a pretty redundant message.
    }

    public void player_lag(String player_name) {
        // TODO only notify row for player.
        players.fireTableDataChanged();
    }

    public void player_list(int room_id) {
        players.fireTableDataChanged();
    }

    public void player_stats(String player) {
        // Ignore
    }

    public void room_enter(RoomChangeEventData data) {
        player_list(0);
    }

    public void room_leave(RoomChangeEventData data) {
        player_list(0);
    }

    public void table_join_fail(String error) {
        // Ignore
    }

    public void table_joined(int table_index) {
        // Ignore
    }

    public void table_launch_fail(ErrorEventData data) {
        // Ignore
    }

    public void table_launched() {
        // Ignore
    }

    public void table_leave_fail(String error) {
        // Ignore
    }

    public void table_left(TableLeaveEventData data) {
        // Ignore
    }

    public void table_list() {
        // Ignore
    }

    public void table_add(Table table) {
        // Ignore
    }

    public void table_delete(Table table) {
        // Ignore
    }

    public void table_update(Table table) {
        // Need to update the table number of the player(s).
        repaint();
    }

    private class PlayersTableModel extends AbstractTableModel {
        private boolean showTableNumber;

        public PlayersTableModel(boolean showTableNumber) {
            this.showTableNumber = showTableNumber;
        }

        public int getColumnCount() {
            return showTableNumber ? 4 : 3;
        }

        public int getRowCount() {
            return room == null ? 0 : room.get_num_players();
        }

        public String getColumnName(int column) {
            switch (column) {
            case 0:
                return "Type";
            case 1:
                return "Nickname";
            case 2:
                return showTableNumber ? "T#" : "Lag";
            case 3:
                return "Lag";
            default:
                return null;
            }
        }

        public Class getColumnClass(int columnIndex) {
            switch (columnIndex) {
            case 0:
                return PlayerType.class;
            case 1:
                return Player.class;
            case 2:
                return showTableNumber ? String.class : Integer.class;
            case 3:
                return Integer.class;
            default:
                return super.getColumnClass(columnIndex);
            }
        }

        public Object getValueAt(int rowIndex, int columnIndex) {
            if (room == null)
                return null;

            Player player = room.get_nth_player(rowIndex);

            if (player == null)
                return null;

            switch (columnIndex) {
            case 0:
                return player.get_type();
            case 1:
                return player;
            case 2:
                if (showTableNumber) {
                    return player.get_table() == null ? null : new Integer(
                            player.get_table().get_id());
                }
            case 3:
                return new Integer(player.get_lag());
            default:
                return null;
            }
        }

    }

    private class RoomChatAction extends ChatAction {
        protected boolean sendChat(ChatType chatType, String target,
                String message) throws IOException {
            room.chat(chatType, target, message);
            return true;
        }

        protected void chat_display_local(ChatType type, String message) {
            chatPanel.appendChat(type, handle, message);
        }

        protected ChatType getDefaultChatType() {
            return ChatType.GGZ_CHAT_NORMAL;
        }
    }

    private class PlayerTypeCellRenderer extends DefaultTableCellRenderer
            implements TableCellRenderer {
        private PlayerType type;

        private PlayerTypeCellRenderer() {
            setHorizontalAlignment(SwingConstants.CENTER);
        }

        public Component getTableCellRendererComponent(JTable table,
                Object value, boolean isSelected, boolean hasFocus, int row,
                int column) {

            type = (PlayerType) value;
            setIcon(IconFactory.getPlayerTypeIcon(type));
            return this;
        }

        public String getToolTipText() {
            if (type == null) {
                return null;
            } else if (type == PlayerType.GGZ_PLAYER_NORMAL) {
                return "Registered Player";
            } else if (type == PlayerType.GGZ_PLAYER_GUEST) {
                return "Guest";
            } else if (type == PlayerType.GGZ_PLAYER_ADMIN) {
                return "GGZ Administrator";
            } else if (type == PlayerType.GGZ_PLAYER_BOT) {
                return "Artificial Intelligence";
            }
            return type.toString();
        }
    }

    private class PlayerCellRenderer extends DefaultTableCellRenderer implements
            TableCellRenderer {
        private Player player;

        protected PlayerCellRenderer() {
            setOpaque(true);
        }

        public Component getTableCellRendererComponent(JTable table,
                Object value, boolean isSelected, boolean hasFocus, int row,
                int column) {
            this.player = (Player) value;
            this.setText(player == null ? null : player.get_name());
            this.setFont(table.getFont());
            this.setForeground(table.getForeground());
            this.setBackground(table.getBackground());
            return this;
        }

        public String getToolTipText() {
            String toolTipText = player == null ? null
                    : "<HTML>"
                            + (player.get_rating() == Player.NO_RATING ? ""
                                    : (messages
                                            .getString("RoomChatPanel.PlayerInfoRating")
                                            + ": " + player.get_rating()))
                            + (player.get_ranking() == Player.NO_RANKING ? ""
                                    : ("<BR>"
                                            + messages
                                                    .getString("RoomChatPanel.PlayerInfoRanking")
                                            + ": " + player.get_ranking()))
                            + (player.get_highscore() == Player.NO_HIGHSCORE ? ""
                                    : ("<BR>"
                                            + messages
                                                    .getString("RoomChatPanel.PlayerInfoHighscore")
                                            + ": " + player.get_highscore()))
                            + (player.get_wins() == Player.NO_RECORD ? ""
                                    : ("<BR>"
                                            + messages
                                                    .getString("RoomChatPanel.PlayerInfoWins")
                                            + ": " + player.get_wins()))
                            + (player.get_losses() == Player.NO_RECORD ? ""
                                    : ("<BR>"
                                            + messages
                                                    .getString("RoomChatPanel.PlayerInfoLosses")
                                            + ": " + player.get_losses()))
                            + (player.get_forfeits() == Player.NO_RECORD ? ""
                                    : ("<BR>"
                                            + messages
                                                    .getString("RoomChatPanel.PlayerInfoForfeits")
                                            + ": " + player.get_forfeits()))
                            + (player.get_ties() == Player.NO_RECORD ? ""
                                    : ("<BR>"
                                            + messages
                                                    .getString("RoomChatPanel.PlayerInfoTies")
                                            + ": " + player.get_ties()));
            if ("<HTML>".equals(toolTipText.trim())) {
                toolTipText = null;
            }
            return toolTipText;
        }
    }

    private class LagCellRenderer extends JComponent implements
            TableCellRenderer {
        private int lag;

        private LagCellRenderer() {
            // setBackground(Color.WHITE);
            // setPreferredSize(new Dimension(20, 20));
        }

        public Component getTableCellRendererComponent(JTable table,
                Object value, boolean isSelected, boolean hasFocus, int row,
                int column) {
            if (value != null) {
                this.lag = ((Integer) value).intValue();
            } else {
                // Assume great connection
                this.lag = 1;
            }
            return this;
        }

        public void update(Graphics g) {
            paint(g);
        }

        public void paint(Graphics g) {
            g.setColor(getBackground());
            g.fillRect(0, 0, getWidth(), getHeight());
            switch (lag) {
            case 1:
                g.setColor(Color.GREEN);
                break;
            case 2:
                g.setColor(Color.GREEN.darker());
                break;
            case 3:
                g.setColor(Color.ORANGE);
                break;
            case 4:
                g.setColor(Color.ORANGE.darker());
                break;
            default:
                g.setColor(Color.RED);
                break;
            }
            int barHeight = (getHeight() - 4) / 5;
            int y = getHeight() - barHeight;
            int insetx = 2;
            for (int i = lag; i < 6; i++) {
                g.fillRect(insetx, y, getWidth() - (insetx * 2), barHeight);
                y -= barHeight + 1;
            }
        }

        public String getToolTipText() {
            switch (lag) {
            case 1:
                return messages.getString("RoomChatPanel.ToolTip.Lag1");
            case 2:
                return messages.getString("RoomChatPanel.ToolTip.Lag2");
            case 3:
                return messages.getString("RoomChatPanel.ToolTip.Lag3");
            case 4:
                return messages.getString("RoomChatPanel.ToolTip.Lag4");
            case 5:
                return messages.getString("RoomChatPanel.ToolTip.Lag5");
            default:
                log.warning("Unrecognised lag value: " + lag);
                return null;
            }
        }
    }
}
