package ggz.ui;

import ggz.client.core.ChatEventData;
import ggz.client.core.ErrorEventData;
import ggz.client.core.Player;
import ggz.client.core.Room;
import ggz.client.core.RoomChangeEventData;
import ggz.client.core.RoomListener;
import ggz.client.core.TableLeaveEventData;
import ggz.common.ChatType;
import ggz.common.PlayerType;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Graphics;
import java.io.IOException;

import javax.swing.ImageIcon;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.SwingConstants;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.TableCellRenderer;

public class RoomChatPanel extends JPanel implements RoomListener {
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
        playerList.getTableHeader().setBackground(new Color(0xce, 0xfa, 0xdf));
        playerList.getColumn("Lag").setMaxWidth(20);
        playerList.getColumn("Lag").setHeaderValue("");
        playerList.getColumn("Type").setMaxWidth(40);
        if (showTableNumber) {
            playerList.getColumn("T#").setMaxWidth(20);
        }
        playerList.setRowSelectionAllowed(false);
        playerList.setGridColor(playerList.getBackground());
        // playerList.getSelectionModel().setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        setOpaque(false);
        playerList.setOpaque(false);
        playerList.setPreferredScrollableViewportSize(new Dimension(150, 100));
        playerList.setDefaultRenderer(PlayerType.class,
                new PlayerTypeCellRenderer());
        playerList.setDefaultRenderer(Integer.class, new LagCellRenderer());
        playerList.setRowHeight(16);
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
        // Ignore chat messages from ourselves since we append the text on send
        // without waiting for the server to make the app feel more responsive.
        if (!handle.equals(data.sender) && data.type != ChatType.GGZ_CHAT_TABLE) {
            chatPanel.appendChat(data.type, data.sender, data.message);
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

    public void table_update() {
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

        public Class<?> getColumnClass(int columnIndex) {
            switch (columnIndex) {
            case 0:
                return PlayerType.class;
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
                return player.get_name();
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
        protected void sendChat(ChatType chatType, String target, String message)
                throws IOException {
            room.chat(chatType, target, message);
        }

        protected void chat_display_local(ChatType type, String message) {
            chatPanel.appendChat(type, handle, message);
        }

        protected ChatType getDefaultChatType() {
            return ChatType.GGZ_CHAT_NORMAL;
        }
    }

    private class PlayerTypeCellRenderer extends JLabel implements
            TableCellRenderer {
        private PlayerType type;

        private ImageIcon[] icons;

        private PlayerTypeCellRenderer() {
            setBackground(Color.WHITE);
            setOpaque(true);
            setHorizontalAlignment(SwingConstants.CENTER);
            icons = new ImageIcon[PlayerType.values().length];
        }

        public Component getTableCellRendererComponent(JTable table,
                Object value, boolean isSelected, boolean hasFocus, int row,
                int column) {

            type = (PlayerType) value;
            if (type == null) {
                setIcon(null);
            } else {
                ImageIcon icon = icons[type.ordinal()];
                if (icon == null) {
                    switch (type) {
                    case GGZ_PLAYER_ADMIN:
                        icon = new ImageIcon(getClass().getResource(
                                "images/p21.gif"));
                        break;
                    case GGZ_PLAYER_BOT:
                        icon = new ImageIcon(getClass().getResource(
                                "images/p17.gif"));
                        break;
                    case GGZ_PLAYER_GUEST:
                        icon = new ImageIcon(getClass().getResource(
                                "images/p29.gif"));
                        break;
                    case GGZ_PLAYER_NORMAL:
                        icon = new ImageIcon(getClass().getResource(
                                "images/p25.gif"));
                        break;
                    case GGZ_PLAYER_UNKNOWN:
                    default:
                        icon = new ImageIcon(getClass().getResource(
                                "images/p13.gif"));
                        break;
                    }
                    icons[type.ordinal()] = icon;
                }
                setIcon(icon);
            }
            return this;
        }

        public String getToolTipText() {
            if (type == null) {
                return null;
            }
            return type.toString();
        }
    }

    private class LagCellRenderer extends JComponent implements
            TableCellRenderer {
        private int lag;

        private LagCellRenderer() {
            setBackground(Color.WHITE);
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
                return "This player has an excellent connection speed.";
            case 2:
                return "This player has a good connection speed.";
            case 3:
                return "This player has an acceptable connection speed.";
            case 4:
                return "This player has a poor connection speed.";
            default:
                return "This player may take a long time to respond or take his/her turn.";
            }
        }
    }
}
