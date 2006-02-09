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

import ggz.client.core.GameType;
import ggz.client.core.Module;
import ggz.client.core.Room;
import ggz.client.core.Server;

import java.awt.BorderLayout;
import java.awt.Cursor;
import java.awt.Font;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.ResourceBundle;

import javax.swing.AbstractAction;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.SwingConstants;

public class LoungePanel extends JPanel {
    private static final ResourceBundle messages = ResourceBundle
            .getBundle("ggz.ui.messages");

    protected Server server;

    private JPanel roomPanel;

    private RoomChatPanel chatPanel;

    private ArrayList<Room> rooms;

    private JPanel contentPanel;

    private JPanel headerPanel;

    private JButton logoutButton;

    private String motd;

    private Comparator sortAlgorithm = new SortByRoomName();

    public LoungePanel(Server server) {
        super(new BorderLayout(4, 4));
        this.server = server;
        contentPanel = new JPanel(new BorderLayout(4, 4));
        chatPanel = new RoomChatPanel(false);
        contentPanel.add(chatPanel, BorderLayout.CENTER);

        roomPanel = new JPanel(new GridLayout());
        contentPanel.add(roomPanel, BorderLayout.NORTH);

        logoutButton = new JButton(new LogoutAction());
        headerPanel = new JPanel(new BorderLayout());
        // headerPanel.add(new JLabel("Games"), BorderLayout.WEST);
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
        Collections.sort(rooms, sortAlgorithm);

        // Add a button for each room, the button handles click events and joins
        // the associated room automatically.
        roomPanel.removeAll();
        CategoryPanel cardGamesPanel = new CategoryPanel(messages
                .getString("LoungePanel.GroupHeader.CardGames"));
        for (int i = 0; i < rooms.size(); i++) {
            Room room = rooms.get(i);
            // roomPanel.add(new RoomButton(room));
            cardGamesPanel.addRoom(room);
        }
        roomPanel.add(cardGamesPanel);
        // CategoryPanel boardGamesPanel = new CategoryPanel("Board Games");
        // boardGamesPanel.addRoom(server.get_nth_room(2));
        // roomPanel.add(boardGamesPanel);
        JScrollPane motdScroll = new JScrollPane();
        JTextArea motdText = new JTextArea(motd);
        motdText.setFont(new Font("Monospaced", Font.PLAIN, 12));
        motdText.setEditable(false);
        motdScroll.setPreferredSize(cardGamesPanel.getPreferredSize());
        motdText.setOpaque(false);
        motdScroll.setOpaque(false);
        motdScroll.getViewport().setOpaque(false);
        motdScroll.setBorder(null);
        motdScroll.setBorder(BorderFactory.createTitledBorder(messages
                .getString("LoungePanel.GroupHeader.MessageOfTheDay")));
        motdScroll.getViewport().add(motdText);
        roomPanel.add(motdScroll);
    }

    public void setMotD(String motd) {
        this.motd = motd;
    }

    public void setRoom(Room room) throws IOException {
        chatPanel.setRoom(room);
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
                return messages.getString("LoungePanel.Button.Logout");
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

    private class CategoryPanel extends JPanel {
        JPanel listPanel;

        protected CategoryPanel(String categoryName) {
            setBorder(BorderFactory.createTitledBorder(categoryName));
            listPanel = new JPanel(new GridLayout(0, 1));
            listPanel.setOpaque(false);
            setLayout(new BorderLayout());
            add(listPanel, BorderLayout.NORTH);
            setOpaque(false);
        }

        protected void addRoom(final Room room) {
            JPanel listCellPanel = new JPanel(new BorderLayout());
            JLabel nameLabel = new JLabel("<HTML><U>" + room.get_name()
                    + "</U></HTML>");
            final JLabel populationLabel = new JLabel((String) null,
                    SwingConstants.RIGHT) {
                public String getText() {
                    return String.valueOf(room.get_num_players());
                }
            };
            listCellPanel.setOpaque(false);
            nameLabel.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
            nameLabel.setToolTipText(room.get_desc());
            nameLabel.setFont(nameLabel.getFont().deriveFont(Font.PLAIN));
            populationLabel.setFont(populationLabel.getFont().deriveFont(
                    Font.PLAIN));
            listCellPanel.add(nameLabel, BorderLayout.WEST);
            listCellPanel.add(populationLabel, BorderLayout.CENTER);
            listPanel.add(listCellPanel);
            nameLabel.addMouseListener(new MouseAdapter() {
                public void mouseClicked(MouseEvent event) {
                    // Find the index of the room and then join the room
                    try {
                        for (int i = 0; i < server.get_num_rooms(); i++) {
                            if (server.get_nth_room(i) == room) {
                                server.join_room(i);
                            }
                        }
                    } catch (IOException ex) {
                        ex.printStackTrace();
                        JOptionPane.showMessageDialog(CategoryPanel.this, ex
                                .toString());
                    }
                }
            });
        }
    }
}
