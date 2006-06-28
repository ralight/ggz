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
import ggz.common.SortedList;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Graphics;
import java.io.IOException;
import java.util.List;
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
import javax.swing.table.JTableHeader;
import javax.swing.table.TableCellRenderer;
import javax.swing.table.TableColumn;
import javax.swing.table.TableColumnModel;

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

    private JPanel westPanel;

    private JLabel playerCountLabel;

    public RoomChatPanel(boolean showTableNumber) {
        super(new BorderLayout());
        chatPanel = new ChatPanel(new RoomChatAction());
        add(chatPanel, BorderLayout.CENTER);

        // The list of players.
        players = new PlayersTableModel(showTableNumber);
        playerList = new JTable(players);
        playerList.getColumn("Type").setHeaderValue(
                messages.getString("RoomChatPanel.ColumnHeader.PlayerType"));
        playerList
                .getColumn("Nickname")
                .setHeaderValue(
                        messages
                                .getString("RoomChatPanel.ColumnHeader.PlayerNickname"));
        playerList.getColumn("Lag").setHeaderValue("");

        if (showTableNumber) {
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
        playerList.setDefaultRenderer(PlayerType.class,
                new PlayerTypeCellRenderer());
        playerList.setDefaultRenderer(Player.class, new PlayerCellRenderer());
        playerList.setDefaultRenderer(Integer.class, new LagCellRenderer());
        initCellSizes(playerList);
        playerScrollPane = new JScrollPane(playerList);
        playerScrollPane.setOpaque(false);
        playerScrollPane.getViewport().setOpaque(false);
        playerCountLabel = new JLabel();
        westPanel = new JPanel(new BorderLayout());
        westPanel.setOpaque(false);
        westPanel.add(playerScrollPane, BorderLayout.CENTER);
        westPanel.add(playerCountLabel, BorderLayout.SOUTH);
        add(westPanel, BorderLayout.WEST);
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
        // panel for rooms and not tables. Also ignore private chat since this
        // is displayed in a private chat dialog (see PrivateChatDialog).
        if ((handle.equals(data.sender) && data.type == ChatType.GGZ_CHAT_NORMAL)
                || data.type == ChatType.GGZ_CHAT_TABLE
                || data.type == ChatType.GGZ_CHAT_PERSONAL) {
            return;
        }

        // All handlers are called from the socket thread so we need to do
        // this crazy stuff.
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                chatPanel.appendChat(data.type, data.sender, data.message);
            }
        });
    }

    public void player_count(final int n) {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                playerCountLabel.setText(n + (n == 1 ? " player" : " players") + " in room");
                revalidate();
                repaint();
            }
        });
    }

    public void player_lag(Player player) {
        players.fireLagCellUpdated(player);
    }

    public void player_list(List new_players) {
        players.replaceAll(new_players);
        player_count(new_players.size());
    }

    public void player_stats(Player player) {
        players.fireStatsUpdated(player);
    }

    public void room_enter(RoomChangeEventData data) {
        players.add(data.player);
        player_count(room.get_num_players());
    }

    public void room_leave(RoomChangeEventData data) {
        players.remove(data.player);
        player_count(room.get_num_players());
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

    public static void initCellSizes(JTable table) {
        JTableHeader header = table.getTableHeader();
        TableCellRenderer defaultHeaderRenderer = null;

        if (header != null) {
            defaultHeaderRenderer = header.getDefaultRenderer();
        }

        TableColumnModel columns = table.getColumnModel();
        int margin = columns.getColumnMargin(); // only JDK1.3
        // int rowCount = data.getRowCount();
        int totalWidth = 0;
        int rowHeight = 0;

        for (int i = columns.getColumnCount() - 1; i >= 0; --i) {
            TableColumn column = columns.getColumn(i);
            int width = -1;

            TableCellRenderer h = column.getHeaderRenderer();

            if (h == null) {
                h = defaultHeaderRenderer;
            }

            if (h != null) // Not explicitly impossible
            {
                Component c = h.getTableCellRendererComponent(table, column
                        .getHeaderValue(), false, false, -1, i);

                width = c.getPreferredSize().width;
            }

            TableCellRenderer r = table.getCellRenderer(0, i);
            Component c;
            switch (i) {
            case 0:
                width = 15;
                break;
            case 2:
                c = r.getTableCellRendererComponent(table, new Player(
                        "TheQuickBrownFox"), false, false, 0, i);

                width = Math.max(width, c.getPreferredSize().width);
                rowHeight = c.getPreferredSize().height;
                break;
            }

            if (width >= 0) {
                column.setPreferredWidth(width + margin); // <1.3: without
                // margin
            } else {
                ; // ???
            }

            totalWidth += column.getPreferredWidth();
        }

        totalWidth += columns.getColumnCount() * columns.getColumnMargin();
        Dimension size = table.getPreferredScrollableViewportSize();
        size.width = totalWidth;
        table.setPreferredScrollableViewportSize(size);

        table.setRowHeight(Math.max(table.getRowHeight(), rowHeight));

        // table.sizeColumnsToFit(-1); <1.3; possibly even table.revalidate()

        // if (header != null)
        // header.repaint(); only makes sense when the header is visible (only
        // <1.3)
    }

    private class PlayersTableModel extends AbstractTableModel {
        private static final int LAG_COLUMN = 0;

        private boolean showTableNumber;

        private SortedList data;

        public PlayersTableModel(boolean showTableNumber) {
            this.showTableNumber = showTableNumber;
            this.data = new SortedList(Player.SORT_BY_NAME);
        }

        public void replaceAll(List players) {
            this.data.clear();
            this.data.addAll(players);
            fireTableDataChanged();
        }

        public void add(Player p) {
            if (this.data.add(p)) {
                int rowIndex = this.data.indexOf(p);
                fireTableRowsInserted(rowIndex, rowIndex);
            } else {
                log.warning("Could not add player to list: " + p);
            }
        }

        public void remove(Player p) {
            int rowIndex = this.data.indexOf(p);
            if (rowIndex < 0) {
                log.warning("Player not found in list, cannot remove: " + p);
            }else {
                if (this.data.remove(rowIndex) == null) {
                    log
                            .warning("Found player object but couldn't remove it from SortedList: " + p);
                } else {
                    fireTableRowsDeleted(rowIndex, rowIndex);
                }
            }
        }

        public int getColumnCount() {
            return showTableNumber ? 4 : 3;
        }

        public int getRowCount() {
            return data.size();
        }

        public String getColumnName(int column) {
            switch (column) {
            case LAG_COLUMN:
                return "Lag";
            case 1:
                return "Type";
            case 2:
                return "Nickname";
            case 3:
                return "T#";
            default:
                return null;
            }
        }

        public Class getColumnClass(int columnIndex) {
            switch (columnIndex) {
            case LAG_COLUMN:
                return Integer.class;
            case 1:
                return PlayerType.class;
            case 2:
                return Player.class;
            case 3:
                return String.class;
            default:
                return super.getColumnClass(columnIndex);
            }
        }

        public Object getValueAt(int rowIndex, int columnIndex) {
            Player player = (Player) data.get(rowIndex);

            if (player == null)
                return null;

            switch (columnIndex) {
            case LAG_COLUMN:
                return new Integer(player.get_lag());
            case 1:
                return player.get_type();
            case 2:
                return player;
            case 3:
                return player.get_table() == null ? null : new Integer(player
                        .get_table().get_id());
            default:
                return null;
            }
        }

        public void fireLagCellUpdated(Player player) {
            int row = this.data.indexOf(player);
            if (row < 0) {
                log.warning("Lag not updated, could not find player in list: "
                        + player);
            } else {
                fireTableCellUpdated(row, LAG_COLUMN);
            }
        }

        public void fireStatsUpdated(Player player) {
            int row = this.data.indexOf(player);
            if (row < 0) {
                log
                .warning("Stats not updated, could not find player in list: "
                        + player);
            } else {
                fireTableRowsUpdated(row, row);
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
