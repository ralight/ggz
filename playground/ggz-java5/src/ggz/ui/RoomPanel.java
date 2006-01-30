package ggz.ui;

import ggz.client.core.ChatEventData;
import ggz.client.core.ErrorEventData;
import ggz.client.core.Game;
import ggz.client.core.GameEventListener;
import ggz.client.core.Module;
import ggz.client.core.Room;
import ggz.client.core.RoomChangeEventData;
import ggz.client.core.RoomListener;
import ggz.client.core.Server;
import ggz.client.core.StateID;
import ggz.client.core.Table;
import ggz.client.core.TableLeaveEventData;
import ggz.common.SeatType;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.io.IOException;
import java.net.URL;

import javax.swing.AbstractAction;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.ListSelectionModel;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.table.AbstractTableModel;

public class RoomPanel extends JPanel implements RoomListener,
        ListSelectionListener {
    protected Server server;

    protected Room room;

    private JScrollPane tableScrollPane;

    protected JTable tableTable;

    protected JPanel tablePanel;

    private JPanel tableButtonPanel;

    private JButton newTableButton;

    private JButton joinTableButton;

    private JPanel headerPanel;

    private JPanel headerButtonPanel;

    private JLabel titleLabel;

    private JButton logoutButton;

    private JButton lobbyButton;

    private RoomChatPanel chatPanel;

    private TablesTableModel tables;

    public RoomPanel(Server server) {
        super(new BorderLayout(4, 4));
        this.server = server;

        // Set up title with room name and back to lobby button.
        headerPanel = new JPanel(new BorderLayout());
        headerButtonPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
        titleLabel = new JLabel("Room");
        lobbyButton = new JButton(new BackToLobbyAction());
        logoutButton = new JButton(new LogoutAction());

        tablePanel = new JPanel(new BorderLayout());
        tableButtonPanel = new JPanel();
        newTableButton = new JButton(new NewTableAction());
        joinTableButton = new JButton(new JoinTableAction());
        joinTableButton.setEnabled(false);
        tables = new TablesTableModel();
        tableTable = new JTable(tables);
        tableTable.getTableHeader().setBackground(new Color(0xce, 0xfa, 0xdf));
        tableTable.setRowHeight(tableTable.getRowHeight() * 4);
        tableTable.getSelectionModel().setSelectionMode(
                ListSelectionModel.SINGLE_SELECTION);
        tableTable.getSelectionModel().addListSelectionListener(this);
        tableScrollPane = new JScrollPane();
        tableScrollPane.getViewport().add(tableTable);
        tableButtonPanel.add(newTableButton);
        tableButtonPanel.add(joinTableButton);

        tablePanel.add(tableScrollPane, BorderLayout.CENTER);
        tablePanel.add(tableButtonPanel, BorderLayout.SOUTH);
        add(tablePanel, BorderLayout.CENTER);
        chatPanel = new RoomChatPanel(true);
        // Set a preferred size to stop it from growing out of control,
        // we want it to stay one size.
        chatPanel.setPreferredSize(new Dimension(500, 200));
        add(chatPanel, BorderLayout.SOUTH);

        headerPanel.add(titleLabel, BorderLayout.WEST);
        headerPanel.add(headerButtonPanel, BorderLayout.EAST);
        headerButtonPanel.add(lobbyButton);
        headerButtonPanel.add(logoutButton);
        add(headerPanel, BorderLayout.NORTH);

        setOpaque(false);
        headerPanel.setOpaque(false);
        headerButtonPanel.setOpaque(false);
        tableButtonPanel.setOpaque(false);
        tablePanel.setOpaque(false);
        tableScrollPane.setOpaque(false);
        tableScrollPane.getViewport().setOpaque(false);
    }

    public void setRoom(Room room) throws IOException {
        this.room = room;
        this.room.add_event_hook(this);
        this.room.list_tables();
        chatPanel.setRoom(room);
        titleLabel.setText("<HTML><B>" + room.get_name()
                + "</B><BR><EM><SPAN style='font-weight:normal'>"
                + room.get_desc() +"</SPAN></EM></HTML>");

        Module module = Module.get_nth_by_type(room.get_gametype(), 0);
        if (module != null && module.get_icon_path() != null) {
            URL imageURL = getClass().getResource(module.get_icon_path());
            titleLabel.setIcon(new ImageIcon(imageURL));
        }

        joinTableButton.setEnabled(false);
        newTableButton.setEnabled(true);
    }

    public void chat_event(ChatEventData data) {
        // Handled by chat panel so do nothing.
    }

    public void player_count(int room_id) {
        // Ignore
    }

    public void player_lag(String player) {
        // Handled by chat panel so do nothing.
    }

    public void player_list(int room_id) {
        // Handled by chat panel so do nothing.
    }

    public void player_stats(String player) {
        // Ignore
    }

    public void room_enter(RoomChangeEventData data) {
        // Ignore
    }

    public void room_leave(RoomChangeEventData data) {
        // Ignore
    }

    public void table_join_fail(String error) {
        tables.fireTableDataChanged();
        JOptionPane.showMessageDialog(this, error);
    }

    public void table_joined(int table_index) {
        tables.fireTableDataChanged();
    }

    public void table_launch_fail(ErrorEventData data) {
        JOptionPane.showMessageDialog(this, data.message);
    }

    public void table_launched() {
        tables.fireTableDataChanged();
        joinTableButton.setEnabled(false);
        newTableButton.setEnabled(false);
    }

    public void table_leave_fail(String error) {
        JOptionPane.showMessageDialog(this, error);
    }

    public void table_left(TableLeaveEventData data) {
        switch (data.get_reason()) {
        case GGZ_LEAVE_BOOT:
            JOptionPane.showMessageDialog(this,
                    "You have been booted from the game by "
                            + data.get_booter());
            break;
        case GGZ_LEAVE_GAMEERROR:
            JOptionPane.showMessageDialog(this,
                    "Sorry, there has been an error in the game.");
            break;
        case GGZ_LEAVE_GAMEOVER:
            JOptionPane.showMessageDialog(this, "GAME OVER");
            break;
        }
        tables.fireTableDataChanged();
        newTableButton.setEnabled(true);
    }

    public void table_list() {
        tables.fireTableDataChanged();
    }

    public void table_update() {
        tables.fireTableDataChanged();
    }

    private class BackToLobbyAction extends AbstractAction {

        public Object getValue(String key) {
            if (NAME.equals(key)) {
                return "Back to Games";
            }
            return super.getValue(key);
        }

        public void actionPerformed(ActionEvent e) {
            try {
                server.join_room(0);
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        }

    }

    /**
     * Called when selection in table changes.
     * 
     * @param e
     */
    public void valueChanged(ListSelectionEvent e) {
        boolean canJoinTable = true;
        if (tableTable.getSelectedRowCount() > 0
                && server.get_state() == StateID.GGZ_STATE_IN_ROOM) {
            Table selectedTable = room.get_nth_table(tableTable
                    .getSelectedRow());
            /* Make sure table has open seats */
            canJoinTable = selectedTable.get_seat_count(SeatType.GGZ_SEAT_OPEN)
                    + selectedTable.get_seat_count(SeatType.GGZ_SEAT_RESERVED) > 0;

            // Temporary limitation because we don't support more than four
            // players in card games.
            canJoinTable = canJoinTable && (selectedTable.get_num_seats() <= 4);
        } else {
            canJoinTable = false;
        }
        joinTableButton.setEnabled(canJoinTable);
    }

    private abstract class PlayGameAction extends AbstractAction implements
            GameEventListener {

        public void actionPerformed(ActionEvent e) {
            if (Module.get_num_by_type(room.get_gametype()) == 0) {
                JOptionPane.showMessageDialog((Component) e.getSource(),
                        "Game is not supported yet");
            } else {
                Module module = Module.get_nth_by_type(room.get_gametype(), 0);
                Game game = new Game(server, module);
                game.add_event_hook(this);
                game.launch();
            }
        }

        public void game_launch_fail(Exception e) {
            e.printStackTrace();
            JOptionPane.showMessageDialog(RoomPanel.this, e.toString());
        }

        public void game_launched() {
            // Ignore
        }

        public void game_negotiate_fail() {
            JOptionPane
                    .showMessageDialog(RoomPanel.this, "game_negotiate_fail");
        }

        public void game_negotiated() {
            server.create_channel();
        }
    }

    private class NewTableAction extends PlayGameAction {
    	private Table table;

        public Object getValue(String key) {
            if (NAME.equals(key)) {
                return "New Game";
            }
            return super.getValue(key);
        }

        public void actionPerformed(ActionEvent e) {
            if (Module.get_num_by_type(room.get_gametype()) == 0) {
                JOptionPane.showMessageDialog((Component) e.getSource(),
                        "Game is not supported yet");
            } else {
            	table = SeatAllocationDialog.getTableSeatAllocation((Component) e.getSource(),room.get_gametype());
            	if (table != null) {
	                Module module = Module.get_nth_by_type(room.get_gametype(), 0);
	                Game game = new Game(server, module);
	                game.add_event_hook(this);
	                game.launch();
            	}
            }
        }

        public void game_playing() {
            try {
                room.launch_table(table);
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        }
    }

    private class JoinTableAction extends PlayGameAction {

        public Object getValue(String key) {
            if (NAME.equals(key)) {
                return "Join Game";
            }
            return super.getValue(key);
        }

        public void game_playing() {
            try {
                room.join_table(tableTable.getSelectedRow(), false);
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        }

    }

    private class LogoutAction extends AbstractAction {

        public Object getValue(String key) {
            if (NAME.equals(key)) {
                return "Logout";
            }
            return super.getValue(key);
        }

        public void actionPerformed(ActionEvent e) {
            try {
                server.logout();
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        }
    }

    private class TablesTableModel extends AbstractTableModel {

        public String getColumnName(int columnIndex) {
            switch (columnIndex) {
            case 0:
                return "Description";
            case 1:
                return "Players";
            case 2:
                return "Spectators";
            default:
                return null;
            }
        }

        public int getColumnCount() {
            return 3;
        }

        public int getRowCount() {
            return room == null ? 0 : room.get_num_tables();
        }

        public Object getValueAt(int rowIndex, int columnIndex) {
            try {
                Table table = room.get_nth_table(rowIndex);
                StringBuffer buffer;

                if (table == null)
                    return null;

                switch (columnIndex) {
                case 0:
                    buffer = new StringBuffer("<HTML><B>Table ");
                    buffer.append(table.get_id());
                    if (table.get_desc() != null) {
                        buffer.append("</B><BR><I>");
                        buffer.append(table.get_desc());
                    }
                    return buffer.toString();
                case 1:
                    buffer = new StringBuffer("<HTML><OL>");
                    for (int player_num = 0; player_num < table.get_num_seats(); player_num++) {
                        buffer.append("<LI>");
                        switch (table.get_nth_player_type(player_num)) {
                        case GGZ_SEAT_ABANDONED:
                            buffer.append("Abandoned");
                            break;
                        case GGZ_SEAT_BOT:
                            buffer.append("AI");
                            break;
                        case GGZ_SEAT_NONE:
                        case GGZ_SEAT_PLAYER:
                            buffer
                                    .append(table
                                            .get_nth_player_name(player_num));
                            break;
                        case GGZ_SEAT_OPEN:
                            buffer.append("Empty Seat");
                            break;
                        case GGZ_SEAT_RESERVED:
                            buffer.append("Reserved for ");
                            break;
                        }
                    }
                    buffer.append("</OL></HTML>");
                    return buffer.toString();
                case 2:
                    buffer = new StringBuffer("<HTML><OL>");
                    for (int spectator_num = 0; spectator_num < table
                            .get_num_spectator_seats(); spectator_num++) {
                        buffer.append("<LI>");
                        switch (table.get_nth_spectator_seat(spectator_num).type) {
                        case GGZ_SEAT_ABANDONED:
                            buffer.append("Abandoned");
                            break;
                        case GGZ_SEAT_BOT:
                            buffer.append("AI");
                            break;
                        case GGZ_SEAT_NONE:
                        case GGZ_SEAT_PLAYER:
                            buffer.append(table
                                    .get_nth_spectator_name(spectator_num));
                            break;
                        case GGZ_SEAT_OPEN:
                            buffer.append("Empty Seat");
                            break;
                        case GGZ_SEAT_RESERVED:
                            buffer.append("Reserved for ");
                            break;
                        }
                    }
                    buffer.append("</OL></HTML>");
                    return buffer.toString();
                default:
                    return null;
                }
            } catch (IndexOutOfBoundsException ex) {
                // This can sometimes happen due to a race condition but
                // it's OK.
                return null;
            }
        }
    }
}
