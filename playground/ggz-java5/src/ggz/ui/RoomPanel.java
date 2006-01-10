package ggz.ui;

import ggz.client.core.ChatEventData;
import ggz.client.core.ErrorEventData;
import ggz.client.core.Game;
import ggz.client.core.GameEventListener;
import ggz.client.core.GameType;
import ggz.client.core.Module;
import ggz.client.core.Room;
import ggz.client.core.RoomChangeEventData;
import ggz.client.core.RoomListener;
import ggz.client.core.Server;
import ggz.client.core.Table;
import ggz.client.core.TableLeaveEventData;
import ggz.common.SeatType;

import java.awt.BorderLayout;
import java.awt.Component;
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
import javax.swing.SwingUtilities;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.table.AbstractTableModel;

public class RoomPanel extends JPanel implements RoomListener, ListSelectionListener {
    protected Server server;

    protected Room room;

    private JScrollPane tableScrollPane;

    protected JTable tableTable;

    protected JPanel tablePanel;

    private JPanel tableButtonPanel;

    private JButton newTableButton;

    private JButton joinTableButton;

    private JButton playSoloButton;

    private JPanel headerPanel;

    private JPanel headerButtonPanel;

    private JLabel titleLabel;

    private JButton logoutButton;

    private JButton lobbyButton;

    private RoomChatPanel chatPanel;

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
        playSoloButton = new JButton(new PlaySoloAction());
        tableTable = new JTable();
        tableTable.getSelectionModel().setSelectionMode(
                ListSelectionModel.SINGLE_SELECTION);
        tableTable.getSelectionModel().addListSelectionListener(this);
        tableScrollPane = new JScrollPane();
        tableScrollPane.getViewport().add(tableTable);
        tableButtonPanel.add(newTableButton);
        tableButtonPanel.add(joinTableButton);
        tableButtonPanel.add(playSoloButton);
        playSoloButton.setVisible(false);

        tablePanel.add(tableScrollPane, BorderLayout.CENTER);
        tablePanel.add(tableButtonPanel, BorderLayout.SOUTH);
        add(tablePanel, BorderLayout.CENTER);
        chatPanel = new RoomChatPanel();
        add(chatPanel, BorderLayout.SOUTH);

        headerPanel.add(titleLabel, BorderLayout.WEST);
        headerPanel.add(headerButtonPanel, BorderLayout.EAST);
        headerButtonPanel.add(lobbyButton);
        headerButtonPanel.add(logoutButton);
        add(headerPanel, BorderLayout.NORTH);

        setOpaque(false);
        headerPanel.setOpaque(false);
        headerButtonPanel.setOpaque(false);
    }

    public void refreshTableList() {
        // Maintain our own list rather than use the server's so that we can
        // sort it according to our own needs.
        // if (tables == null) {
        // // First time we allocate an array of the appropriate size.
        // tables = new ArrayList<Table>(room.get_num_tables());
        // } else {
        // tables.clear();
        // }
        //
        // for (int i = 0; i < room.get_num_tables(); i++) {
        // tables.add(room.get_nth_table(i));
        // }

        // Sort on the game associated with the room.
        // Collections.sort(tables, new SortByGameType());
        // Collections.sort(tables, sortAlgorithm);

        // TODO register PropertyChangeListener for all rooms and then
        // get the model to fire model changed when a room's property changes.

        tableTable.setModel(new AbstractTableModel() {

            public String getColumnName(int columnIndex) {
                switch (columnIndex) {
                case 0:
                    return "Description";
                case 1:
                    return "Game";
                case 2:
                    return "Seats";
                case 3:
                    return "Spectator Seats";
                default:
                    return null;
                }
            }

            public int getColumnCount() {
                return 4;
            }

            public int getRowCount() {
                return room.get_num_tables();
                // return tables.size();
            }

            public Object getValueAt(int rowIndex, int columnIndex) {
                Table table = room.get_nth_table(rowIndex);
                // Table table = tables.get(rowIndex);

                if (table == null)
                    return null;

                switch (columnIndex) {
                case 0:
                    return table.get_desc();
                case 1:
                    GameType gameType = table.get_type();
                    return gameType == null ? null : table.get_type()
                            .get_name();
                case 2:
                    return table.get_num_seats();
                case 3:
                    return table.get_num_spectator_seats();
                default:
                    return null;
                }
            }
        });

    }

    public void setRoom(Room room) throws IOException {
        this.room = room;
        this.room.add_event_hook(this);
        this.room.list_tables();
        chatPanel.setRoom(room);
        titleLabel.setText("<HTML><B>" + room.get_name()
                + "</B><BR><EM><SPAN style='font-weight:normal'>"
                + room.get_desc() + "</SPAN></EM></HTML>");

        Module module = Module.get_nth_by_type(room.get_gametype(), 0);
        if (module.get_icon_path() != null) {
            URL imageURL = getClass().getResource(module.get_icon_path());
            titleLabel.setIcon(new ImageIcon(imageURL));
        }

        boolean isSoloPlayPossible = room.get_gametype().get_max_bots() > 0;
        playSoloButton.setVisible(isSoloPlayPossible);
    }

    public void chat_event(ChatEventData data) {
        // Handled by chat panel so do nothing.
    }

    public void player_count(int room_id) {

    }

    public void player_lag(String player) {
        // Handled by chat panel so do nothing.
    }

    public void player_list(int room_id) {
        // Handled by chat panel so do nothing.
    }

    public void player_stats(String player) {

    }

    public void room_enter(RoomChangeEventData data) {

    }

    public void room_leave(RoomChangeEventData data) {

    }

    public void table_join_fail(String error) {
        ((AbstractTableModel) tableTable.getModel()).fireTableDataChanged();
    }

    public void table_joined(int table_index) {
        ((AbstractTableModel) tableTable.getModel()).fireTableDataChanged();
    }

    public void table_launch_fail(ErrorEventData data) {
        JOptionPane.showMessageDialog(this, data.message);
    }

    public void table_launched() {
        ((AbstractTableModel) tableTable.getModel()).fireTableDataChanged();
        playSoloButton.setEnabled(false);
        joinTableButton.setEnabled(false);
        newTableButton.setEnabled(false);
    }

    public void table_leave_fail(String error) {
        JOptionPane.showMessageDialog(this, error);
    }

    public void table_left(TableLeaveEventData data) {
        ((AbstractTableModel) tableTable.getModel()).fireTableDataChanged();
        playSoloButton.setEnabled(true);
        newTableButton.setEnabled(true);
    }

    public void table_list() {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                refreshTableList();
            }
        });
    }

    public void table_update() {
        ((AbstractTableModel) tableTable.getModel()).fireTableDataChanged();
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
     * @param e
     */
    public void valueChanged(ListSelectionEvent e) {
        joinTableButton.setEnabled(tableTable.getSelectedRowCount() > 0);
    }
    
    private abstract class PlayGameAction extends AbstractAction implements
            GameEventListener {

        public void actionPerformed(ActionEvent e) {
            // Probably need to create an instance of our game here so that it
            // can be given the Mod.
            // game.getModGame()
            // Maybe use exec_path Module to include class name so that we can
            // "fork" it like the C version does
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
            JOptionPane.showMessageDialog(RoomPanel.this, e.getMessage());
        }

        public void game_launched() {
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

        public Object getValue(String key) {
            if (NAME.equals(key)) {
                return "New Game";
            }
            return super.getValue(key);
        }

        public void game_playing() {
            try {
                Table table = new Table(room.get_gametype(), "Join me!",
                        4);
                table.set_seat(0, SeatType.GGZ_SEAT_RESERVED, server
                        .get_handle());
                table.set_seat(1, SeatType.GGZ_SEAT_OPEN, null);
                table.set_seat(2, SeatType.GGZ_SEAT_OPEN, null);
                table.set_seat(3, SeatType.GGZ_SEAT_OPEN, null);
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

    private class PlaySoloAction extends PlayGameAction {

        public Object getValue(String key) {
            if (NAME.equals(key)) {
                return "Play against the computer";
            }
            return super.getValue(key);
        }

        public void game_playing() {
            try {
                Table table = new Table(room.get_gametype(), "I play alone...",
                        4);
                table.set_seat(0, SeatType.GGZ_SEAT_RESERVED, server
                        .get_handle());
                table.set_seat(1, SeatType.GGZ_SEAT_BOT, null);
                table.set_seat(2, SeatType.GGZ_SEAT_BOT, null);
                table.set_seat(3, SeatType.GGZ_SEAT_BOT, null);
                room.launch_table(table);
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

}
