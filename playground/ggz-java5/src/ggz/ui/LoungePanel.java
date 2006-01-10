package ggz.ui;

import ggz.client.core.GameType;
import ggz.client.core.Module;
import ggz.client.core.Room;
import ggz.client.core.Server;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.GridLayout;
import java.awt.SystemColor;
import java.awt.event.ActionEvent;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

import javax.swing.AbstractAction;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.ListCellRenderer;
import javax.swing.SwingConstants;

public class LoungePanel extends JPanel {
    protected Server server;

    private JPanel roomPanel;

    private RoomChatPanel chatPanel;

    private ArrayList<Room> rooms;

    private JPanel contentPanel;

    private JPanel headerPanel;

    private JButton logoutButton;

    private Comparator sortAlgorithm = new SortByRoomName();

    public LoungePanel(Server server) {
        super(new BorderLayout(4, 4));
        this.server = server;
        contentPanel = new JPanel(new BorderLayout(4, 4));
        chatPanel = new RoomChatPanel();
        contentPanel.add(chatPanel, BorderLayout.CENTER);

        roomPanel = new JPanel(new GridLayout());
        contentPanel.add(roomPanel, BorderLayout.NORTH);

        logoutButton = new JButton(new LogoutAction());
        headerPanel = new JPanel(new BorderLayout());
        //headerPanel.add(new JLabel("Games"), BorderLayout.WEST);
        headerPanel.add(logoutButton, BorderLayout.EAST);
        // Make everything transparent.
        headerPanel.setOpaque(false);
        logoutButton.setOpaque(false);
        contentPanel.setOpaque(false);
        roomPanel.setOpaque(false);
        setOpaque(false);

        add(headerPanel, BorderLayout.NORTH);
        add(contentPanel, BorderLayout.CENTER);
    }

    public void refreshRoomList() {
        if (rooms == null) {
            // First time we allocate an array of the appropriate size.
            rooms = new ArrayList<Room>(server.get_num_rooms());
        }
        rooms.clear();
        // Maintain our own list rather than use the server's so that we can
        // sort it according to our own needs.
        for (int i = 0; i < server.get_num_rooms(); i++) {
            Room room = server.get_nth_room(i);
            if (room.get_gametype() != null
                    && Module.get_num_by_type(room.get_gametype()) > 0) {
                rooms.add(room);
            }
        }

        // Sort on the game associated with the room.
        // Collections.sort(rooms, new SortByGameType());
        Collections.sort(rooms, sortAlgorithm);

        // Add a button for each room, the button handles click events and joins
        // the associated room automatically.
        roomPanel.removeAll();
        for (int i = 0; i < rooms.size(); i++) {
            Room room = rooms.get(i);
            roomPanel.add(new RoomButton(room));
        }

    }

    public void setRoom(Room room) throws IOException {
        chatPanel.setRoom(room);
    }

    private static class SortByGameType implements Comparator {
        public int compare(Object o1, Object o2) {
            GameType g1 = ((Room) o1).get_gametype();
            GameType g2 = ((Room) o2).get_gametype();
            if (g1 == null && g2 == null) {
                return 0;
            } else if (g1 == null && g2 != null) {
                return -1;
            } else if (g1 == null && g2 != null) {
                return 1;
            } else {
                return g1.get_name().compareTo(g2.get_name());
            }
        }
    }

    private static class SortByRoomName implements Comparator {
        public int compare(Object o1, Object o2) {
            Room r1 = (Room) o1;
            Room r2 = (Room) o2;
            GameType g1 = r1.get_gametype();
            GameType g2 = r2.get_gametype();
            if (g1 == null && g2 == null) {
                return 0;
            } else if (g1 == null && g2 != null) {
                return -1;
            } else if (g1 == null && g2 != null) {
                return 1;
            } else {
                return r1.get_name().compareTo(r2.get_name());
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

    private class RoomCellRenderer extends JPanel implements ListCellRenderer {
        private JLabel nameLabel = new JLabel();

        private JLabel playerCountLabel = new JLabel((String) null,
                SwingConstants.RIGHT);

        private Room room;

        RoomCellRenderer() {
            this.setLayout(new BorderLayout(5, 5));
            this.add(nameLabel, BorderLayout.CENTER);
            this.add(playerCountLabel, BorderLayout.EAST);
            this.setBorder(BorderFactory.createEmptyBorder(0, 10, 0, 10));
        }

        public Component getListCellRendererComponent(JList list, Object value,
                int index, boolean isSelected, boolean cellHasFocus) {
            this.room = (Room) value;
            if (isSelected && list.hasFocus()) {
                setBackground(SystemColor.textHighlight);
                nameLabel.setForeground(SystemColor.textHighlightText);
                playerCountLabel.setForeground(SystemColor.textHighlightText);
            } else {
                setBackground(SystemColor.window);
                nameLabel.setForeground(Color.BLUE);
                playerCountLabel.setForeground(SystemColor.textText);
            }
            nameLabel.setText(room.get_name());
            playerCountLabel.setText(String.valueOf(room.get_num_players()));
            return this;
        }

        public String getToolTipText() {
            return room.get_gametype().get_desc();

        }
    }
}
