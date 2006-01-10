package ggz.ui;

import ggz.client.core.ChatEventData;
import ggz.client.core.ErrorEventData;
import ggz.client.core.Player;
import ggz.client.core.Room;
import ggz.client.core.RoomChangeEventData;
import ggz.client.core.RoomListener;
import ggz.client.core.TableLeaveEventData;
import ggz.common.ChatType;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.event.ActionEvent;
import java.io.IOException;

import javax.swing.AbstractAction;
import javax.swing.JComponent;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
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

    public RoomChatPanel() {
        super(new BorderLayout());

        chatPanel = new ChatPanel(new ChatAction());
        add(chatPanel, BorderLayout.CENTER);
        
        // The list of players.
        players = new PlayersTableModel();
        playerList = new JTable(players);
        // playerList.setRowHeight(20);
        playerList.getColumn("Lag").setMaxWidth(20);
        playerList.getColumn("Lag").setHeaderValue("");
        playerList.getColumn("Type").setMaxWidth(40);
        playerList.setRowSelectionAllowed(false);
        playerList.setGridColor(playerList.getBackground());
        // playerList.getSelectionModel().setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        setOpaque(false);
        playerList.setOpaque(false);
        playerList.setPreferredScrollableViewportSize(new Dimension(150, 100));
        playerList.setDefaultRenderer(Integer.class, new LagCellRenderer());
        playerScrollPane = new JScrollPane();
        playerScrollPane.setOpaque(false);
        playerScrollPane.getViewport().add(playerList);
        playerScrollPane.getViewport().setOpaque(false);
        add(playerScrollPane, BorderLayout.WEST);
    }

    public void setRoom(Room r) throws IOException {
        room = r;
        handle = room.get_server().get_handle();
        room.add_event_hook(this);
        room.list_players();
        chatPanel.setEnabled(true);
    }

    public void chat_event(final ChatEventData data) {
        // Ignore chat messages from ourselves since we append the text on send
        // without waiting for the server to make the app feel more responsive.
        if (!handle.equals(data.sender)) {
            chatPanel.handle_chat(data.sender, data.message);
        }
    }

    public void player_count(int room_id) {
        // TODO will this fix the count display errors I've noticed?
        // textArea.append("player_count: "+room_id);
    }

    public void player_lag(String player) {
        // TODO only notify row for player.
        players.fireTableDataChanged();
    }

    public void player_list(int room_id) {
        players.fireTableDataChanged();
        // All handlers are called from the socket thread so we need to do
        // this crazy stuff.
        // SwingUtilities.invokeLater(new Runnable() {
        // public void run() {
        // DefaultListModel model = (DefaultListModel) playerList
        // .getModel();
        // model.removeAllElements();
        // for (int i = 0; i < room.get_num_players(); i++) {
        // model.addElement(room.get_nth_player(i).get_name());
        // }
        // }
        // });
    }

    public void player_stats(String player) {
    }

    public void room_enter(RoomChangeEventData data) {
        player_list(0);
    }

    public void room_leave(RoomChangeEventData data) {
        player_list(0);
    }

    public void table_join_fail(String error) {
    }

    public void table_joined(int table_index) {
    }

    public void table_launch_fail(ErrorEventData data) {
    }

    public void table_launched() {
    }

    public void table_leave_fail(String error) {
    }

    public void table_left(TableLeaveEventData data) {
    }

    public void table_list() {
    }

    public void table_update() {
    }

    private class PlayersTableModel extends AbstractTableModel {

        public int getColumnCount() {
            return 3;
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
                return "Lag";
            default:
                return null;
            }
        }

        public Class<?> getColumnClass(int columnIndex) {
            if (columnIndex == 2) {
                return Integer.class;
            }
            return super.getColumnClass(columnIndex);
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
                return player.get_lag(); // seems to range from 1-5
            default:
                return null;
            }
        }

    }

    private class ChatAction extends AbstractAction {
        public Object getValue(String key) {
            if (NAME.equals(key)) {
                return "Send";
            }
            return super.getValue(key);
        }

        public void actionPerformed(ActionEvent e) {
            try {
                String message = chatPanel.getMessage();
                if (message != null && !"".equals(message.trim())) {
                    room.chat(ChatType.GGZ_CHAT_NORMAL, null, message);
                    chatPanel.clearMessage();
                    chatPanel.handle_chat(handle, message);
                }
            } catch (IOException ex) {
                ex.printStackTrace();
                JOptionPane.showMessageDialog((Component) e.getSource(), ex
                        .getMessage());
            }
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
