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

import ggz.client.core.Module;
import ggz.client.core.MotdEventData;
import ggz.client.core.Room;
import ggz.client.core.Server;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Cursor;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.GridLayout;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.ResourceBundle;

import javax.swing.AbstractAction;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSplitPane;
import javax.swing.JTextArea;
import javax.swing.JTextPane;
import javax.swing.SwingConstants;
import javax.swing.event.HyperlinkListener;
import javax.swing.text.JTextComponent;

public class LoungePanel extends JPanel {
    protected static final ResourceBundle messages = ResourceBundle
            .getBundle("ggz.ui.messages");

    protected Server server;

    private JPanel roomPanel;

    private JScrollPane motdScroll;

    private JTextComponent motdText;

    private CategoryPanel cardGamesPanel;

    private RoomChatPanel chatPanel;

    private ArrayList rooms;

    private JSplitPane contentPanel;

    private JPanel headerPanel;

    private JButton logoutButton;

    public LoungePanel(Server server) {
        super(new BorderLayout(4, 4));
        this.server = server;
        contentPanel = new JSplitPane(JSplitPane.VERTICAL_SPLIT);
        chatPanel = new RoomChatPanel(false);
        contentPanel.setBottomComponent(chatPanel);

        roomPanel = new JPanel(new GridLayout());
        contentPanel.setTopComponent(roomPanel);

        contentPanel.setResizeWeight(0);
        contentPanel.setDividerLocation(300);
        contentPanel.setBorder(null);

        logoutButton = new JButton(new LogoutAction());
        headerPanel = new JPanel(new BorderLayout());
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
            rooms = new ArrayList(server.get_num_rooms());
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
        Collections.sort(rooms, Room.SORT_BY_NAME);

        // Add a button for each room, the button handles click events and joins
        // the associated room automatically.
        // roomPanel.removeAll();
        if (cardGamesPanel != null) {
            roomPanel.remove(cardGamesPanel);
        }
        cardGamesPanel = new CategoryPanel(messages
                .getString("LoungePanel.GroupHeader.CardGames"));
        for (int i = 0; i < rooms.size(); i++) {
            Room room = (Room) rooms.get(i);
            // roomPanel.add(new RoomButton(room));
            cardGamesPanel.addRoom(room);
        }
        roomPanel.add(cardGamesPanel);
        // CategoryPanel boardGamesPanel = new CategoryPanel("Board Games");
        // boardGamesPanel.addRoom(server.get_nth_room(2));
        // roomPanel.add(boardGamesPanel);
    }

    public void setMotD(MotdEventData motd, HyperlinkListener hll) {
        try {
            motdText = new JTextPane();
            ((JTextPane) motdText).setPage(motd.url);
            ((JTextPane) motdText).addHyperlinkListener(hll);
        } catch (Throwable ex) {
            motdText = new JTextArea(motd.motd);
            motdText.setFont(new Font("Monospaced", Font.PLAIN, 12));
        }
        motdText.setEditable(false);
        motdText.setOpaque(false);
        TextPopupMenu.enableFor(motdText);
        if (motdScroll == null) {
            motdScroll = new JScrollPane();
            motdScroll.setOpaque(false);
            motdScroll.getViewport().setOpaque(false);
            motdScroll.setBorder(null);
            motdScroll.setBorder(BorderFactory.createTitledBorder(messages
                    .getString("LoungePanel.GroupHeader.MessageOfTheDay")));
            roomPanel.add(motdScroll);
        }
        motdScroll.getViewport().add(motdText);
    }

    public void setRoom(Room room) throws IOException {
        chatPanel.setRoom(room);
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

    private class CategoryPanel extends JScrollPane {
        JPanel listPanel;

        protected CategoryPanel(String categoryName) {
            setBorder(BorderFactory.createTitledBorder(categoryName));
            JPanel keepAtTop = new JPanel(new BorderLayout(0, 0));
            listPanel = new JPanel(new GridLayout(0, 1));
            listPanel.setBorder(BorderFactory.createEmptyBorder(4, 0, 4, 0));
            listPanel.setOpaque(false);
            keepAtTop.setOpaque(false);
            keepAtTop.add(listPanel, BorderLayout.NORTH);
            getViewport().add(keepAtTop);
            getViewport().setOpaque(false);
            setOpaque(false);
        }

        protected void addRoom(final Room room) {
            JPanel listCellPanel = new JPanel(new BorderLayout()) {
                protected void paintComponent(Graphics g) {
                    // Do our own painting so that we handle alpha properly.
                    // It's got to do with isOpaque();
                    Rectangle clip = g.getClipBounds();
                    g.setColor(getBackground());
                    g.fillRect(clip.x, clip.y, clip.width, clip.height);
                }
            };
            JLabel nameLabel = new JLabel("<HTML><U>" + room.get_name()
                    + "</U></HTML>");
            final JLabel populationLabel = new JLabel((String) null,
                    SwingConstants.RIGHT) {
                public String getText() {
                    // Room can be null during JLabel construction...not sure
                    // why...
                    return room == null ? null : String.valueOf(room
                            .get_num_players());
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
            // Totally transparent.
            listCellPanel.setBackground(new Color(0xff, 0xff, 0xff, 0x00));
            listCellPanel.setBorder(BorderFactory.createEmptyBorder(0, 10, 0,
                    10));
            listPanel.add(listCellPanel);
            getVerticalScrollBar().setUnitIncrement(
                    Math.max(listCellPanel.getPreferredSize().height,
                            getVerticalScrollBar().getUnitIncrement()));
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

                public void mouseEntered(MouseEvent event) {
                    JComponent cell = (JComponent) event.getComponent()
                            .getParent();
                    // Translucent.
                    cell.setBackground(new Color(0xff, 0xff, 0xff, 0x66));
                    cell.repaint();
                }

                public void mouseExited(MouseEvent event) {
                    JComponent cell = (JComponent) event.getComponent()
                            .getParent();
                    // Totally transparent.
                    cell.setBackground(new Color(0xff, 0xff, 0xff, 0x00));
                    cell.repaint();
                }
            });
        }
    }
}
