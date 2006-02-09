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

import ggz.client.core.ErrorEventData;
import ggz.client.core.MotdEventData;
import ggz.client.core.Room;
import ggz.client.core.Server;
import ggz.client.core.ServerListener;
import ggz.client.core.StateID;

import java.awt.AlphaComposite;
import java.awt.BorderLayout;
import java.awt.CardLayout;
import java.awt.Color;
import java.awt.Composite;
import java.awt.Cursor;
import java.awt.GradientPaint;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.IOException;
import java.net.URI;
import java.net.URL;
import java.util.ResourceBundle;

import javax.imageio.ImageIO;
import javax.swing.JApplet;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextArea;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;
import javax.swing.WindowConstants;
import javax.swing.event.HyperlinkEvent;
import javax.swing.event.HyperlinkListener;

public class ClientApplet extends JApplet implements ServerListener,
        HyperlinkListener {
    private static final ResourceBundle messages = ResourceBundle
            .getBundle("ggz.ui.messages");

    private static final int DEFAULT_PORT = 5688;

    protected Server server;

    protected JLabel aboutLabel;

    protected JPanel mainPanel;

    protected LoginPanel loginPanel;

    protected LoungePanel loungePanel;

    protected JLabel busyPanel;

    protected RoomPanel roomPanel;

    private ErrorEventData loginFailData;

    private String uriPath;

    static {
        try {
            // Try and get Swing to use Antialiased text.
            System.setProperty("swing.aatext", "true");
        } catch (Throwable ex) {
            // Ignore, we don't have permission so just don't anti-alias text.
        }
    }

    public ClientApplet() throws IOException {
        // TODO Make watermark URL and background color applet parameters.
        URL imageUrl = getClass().getResource("/ggz/ui/images/watermark.gif");
        final Image watermark = ImageIO.read(imageUrl);
        final Composite alphaComposite = AlphaComposite.getInstance(
                AlphaComposite.SRC_OVER, 0.3f);

        // Create a custom content pane that does fancy painting.
        setContentPane(new JPanel() {

            public void paintComponent(Graphics g) {
                if (isOpaque()) {
                    Graphics2D g2d = (Graphics2D) g;

                    // Fille the background with a gradient.
                    g2d.setPaint(new GradientPaint(0, 0, new Color(0xce, 0xfa,
                            0xdf), 0, getHeight() / 2, new Color(0x7c, 0xaf,
                            0x68), true));
                    g2d.fillRect(0, 0, getWidth(), getHeight());

                    if (watermark != null) {
                        // Paint the watermark.
                        Composite originalComposite = g2d.getComposite();
                        g2d.setComposite(alphaComposite);
                        g2d.drawImage(watermark, getWidth() - 60
                                - watermark.getWidth(this), getHeight() - 60
                                - watermark.getHeight(this), this);
                        g2d.setComposite(originalComposite);
                    }
                }
            }

        });
    }

    public void init() {
        try {
            // Parse URI parameter and set up server.
            URI uri = new URI(getParameter("uri",
                    "ggz://live.ggzgamingzone.org:5688/Entry%20Room"));
            String host = uri.getHost();
            int port = uri.getPort() == -1 ? DEFAULT_PORT : uri.getPort();
            uriPath = uri.getPath();
            String sendLogFile = getParameter("sendLog");
            String receiveLogFile = getParameter("receiveLog");
            server = new Server(host, port, false);
            server.log_session(sendLogFile, receiveLogFile);
            server.add_event_hook(this);

            // Init components
            getContentPane().setLayout(new BorderLayout());
            mainPanel = new JPanel(new CardLayout());
            mainPanel.setOpaque(false);
            getContentPane().add(mainPanel, BorderLayout.CENTER);
            aboutLabel = new JLabel(
                    "<HTML><A href='' style='font-weight:normal; font-size:smaller'>"
                            + messages.getString("ClientApplet.Label.About") + "</A>",
                    SwingConstants.RIGHT);
            aboutLabel
                    .setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
            aboutLabel.addMouseListener(new MouseAdapter() {
                public void mouseClicked(MouseEvent event) {
                    AboutDialog.showDialog(ClientApplet.this);
                }
            });
            getContentPane().add(aboutLabel, BorderLayout.SOUTH);
            loginPanel = new LoginPanel(server);
            mainPanel.add(loginPanel, "login");
            loginPanel.init(uri.getUserInfo());
            loungePanel = new LoungePanel(server);
            mainPanel.add(loungePanel, "lounge");
            roomPanel = new RoomPanel(server);
            mainPanel.add(roomPanel, "room");
            busyPanel = new JLabel(messages
                    .getString("ClientApplet.Label.PleaseWait"),
                    SwingConstants.CENTER);
            mainPanel.add(busyPanel, "busy");

            // Register ourselves as the global hyperlink handler for our
            // hyperlink label.
            HyperlinkLabel.setGlobalHyperlinkListener(this);
        } catch (Exception e) {
            handleException(e);
        }
    }

    public void start() {
        // Nothing to do since it's already done in init().
    }

    public void stop() {
        try {
            if (server != null
                    && server.get_state() != StateID.GGZ_STATE_OFFLINE) {
                server.logout();
            }
        } catch (Exception e) {
            handleException(e);
        }
    }

    /**
     * Utility method that wraps the Applet.getParameter() method supports
     * default values for parameters that don't have a value.
     * 
     * @param name
     * @param defaultValue
     * @return
     */
    protected String getParameter(String name, String defaultValue) {
        String value = getParameter(name);
        return (value == null) ? defaultValue : value;
    }

    public void server_channel_connected() {
        // Nothing to do here.
    }

    public void server_channel_fail(String error) {
        JOptionPane.showMessageDialog(this, error);
    }

    public void server_channel_ready() {
        try {
            server.get_cur_game().set_server_fd(server.get_channel());
        } catch (Exception e) {
            handleException(e);
        }
    }

    public void server_chat_fail(ErrorEventData data) {
        JOptionPane.showMessageDialog(this, data.message);
    }

    public void server_connect_fail(String error) {
        JOptionPane.showMessageDialog(this, error);
        resetLogin();
    }

    public void server_connected() {
        // Do nothing
    }

    public void server_enter_fail(ErrorEventData data) {
        JOptionPane.showMessageDialog(this, data.message);
        // Do the same logic as when enter succeeded so that we show the
        // appropriate panel.
        server_enter_ok();
    }

    public void server_enter_ok() {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                CardLayout layout = (CardLayout) mainPanel.getLayout();
                try {
                    if (server.get_cur_room().get_gametype() == null) {
                        loungePanel.setRoom(server.get_cur_room());
                        layout.show(mainPanel, "lounge");
                    } else {
                        roomPanel.setRoom(server.get_cur_room());
                        layout.show(mainPanel, "room");
                    }
                } catch (Exception e) {
                    handleException(e);
                }
            }
        });
    }

    public void server_login_ok() {
        try {
            server.list_rooms(true);
        } catch (Exception e) {
            handleException(e);
        }
    }

    public void server_login_fail(ErrorEventData data) {
        loginFailData = data;
        try {
            server.logout();
        } catch (Exception e) {
            handleException(e);
        }
    }

    public void server_logged_out() {
        resetLogin();
        if (loginFailData != null) {
            JOptionPane.showMessageDialog(this, loginFailData.message);
        }
    }

    public void server_motd_loaded(MotdEventData data) {
        if (!"normal".equals(data.priority)) {
            // Assume it's a higher priority.
            JDialog dialog = new JDialog(
                    JOptionPane.getFrameForComponent(this), messages
                            .getString("ClientApplet.DialogTitle.MessageOfTheDay"));
            JTextArea textArea = new JTextArea(data.motd);

            textArea.setEditable(false);
            dialog.add(textArea);
            dialog.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
            dialog.pack();
            dialog.setVisible(true);
        }
        loungePanel.setMotD(data.motd);
    }

    public void server_negotiate_fail(String error) {
        JOptionPane.showMessageDialog(this, error);
        resetLogin();
    }

    public void server_negotiated() {
        // Now that we have negotiated the connection successfully send the
        // login info.
        try {
            loginPanel.login();
        } catch (Exception e) {
            handleException(e);
        }
    }

    public void server_net_error(String error) {
        JOptionPane.showMessageDialog(this, error);
        resetLogin();
    }

    public void server_protocol_error(String error) {
        JOptionPane.showMessageDialog(this, error);
        resetLogin();
    }

    public void server_list_rooms() {
        try {
            server.list_gametypes(true);
        } catch (IOException e) {
            handleException(e);
        }

    }

    public void server_players_changed() {
        // Needed so that components that show player counts can refresh
        // themselves.
        invalidate();
        validate();
        repaint();
    }

    public void server_state_changed() {
        String statusText;
        CardLayout layout = (CardLayout) mainPanel.getLayout();

        switch (server.get_state()) {
        case GGZ_STATE_OFFLINE:
            statusText = messages.getString("ClientApplet.StateOffline");
            break;
        /** In the process of connecting. */
        case GGZ_STATE_CONNECTING:
            loginFailData = null;
            layout.show(mainPanel, "busy");
            statusText = messages.getString("ClientApplet.StateConnecting");
            break;
        /** Continuous reconnection attempts. */
        case GGZ_STATE_RECONNECTING:
            statusText = messages.getString("ClientApplet.StateReconnecting");
            break;
        /** Connected, but not doing anything. */
        case GGZ_STATE_ONLINE:
            statusText = messages.getString("ClientApplet.StateConnected");
            break;
        /** In the process of logging in. */
        case GGZ_STATE_LOGGING_IN:
            statusText = messages.getString("ClientApplet.StateLoggingIn");
            break;
        /** Online and logged in! */
        case GGZ_STATE_LOGGED_IN:
            statusText = messages.getString("ClientApplet.StateLoggedIn");
            break;
        /** Moving into a room. */
        case GGZ_STATE_ENTERING_ROOM:
            layout.show(mainPanel, "busy");
            statusText = messages.getString("ClientApplet.StateEnteringRoom");
            break;
        /** Moving between rooms. */
        case GGZ_STATE_BETWEEN_ROOMS:
            layout.show(mainPanel, "busy");
            statusText = messages.getString("ClientApplet.StateBetweenRooms");
            break;
        /** Online, logged in, and in a room. */
        case GGZ_STATE_IN_ROOM:
            statusText = messages.getString("ClientApplet.StateInRoom");
            break;
        /** Trying to launch a table. */
        case GGZ_STATE_LAUNCHING_TABLE:
            statusText = messages.getString("ClientApplet.StateLaunchingGame");
            break;
        /** Trying to join a table. */
        case GGZ_STATE_JOINING_TABLE:
            statusText = messages.getString("ClientApplet.StateJoiningGame");
            break;
        /** Online, loggied in, in a room, at a table. */
        case GGZ_STATE_AT_TABLE:
            statusText = messages.getString("ClientApplet.StatePlaying");
            break;
        /** Waiting to leave a table. */
        case GGZ_STATE_LEAVING_TABLE:
            statusText = messages.getString("ClientApplet.StateLeavingGame");
            break;
        /** In the process of logging out. */
        case GGZ_STATE_LOGGING_OUT:
            statusText = messages.getString("ClientApplet.StateLoggingOut");
            break;
        default:
            statusText = "";
            break;
        }
        getAppletContext().showStatus(statusText);
    }

    public void server_list_types() {
        // Game types are loaded after rooms (not sure why this is but I'm just
        // following the sequence in ggz-wrapper). In any case, we only refresh
        // the room list after both game types and rooms are loaded.
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                loungePanel.refreshRoomList();
            }
        });
        try {
            if (uriPath != null && uriPath.length() > 1) {
                String roomToEnter = uriPath.substring(1);
                // Strip any trailing slash.
                if (roomToEnter.endsWith("/")) {
                    roomToEnter = roomToEnter.substring(0,
                            roomToEnter.length() - 1);
                }
                // Move directly into the room specified by the URI.
                for (int room_num = 0; room_num < server.get_num_rooms(); room_num++) {
                    Room room = server.get_nth_room(room_num);
                    if (roomToEnter.equals(room.get_name())) {
                        server.join_room(room.get_id());
                        return;
                    }
                }
            }

            // If we got this far then either there was no room specified or no
            // room with that name was found.
            server.join_room(0);
        } catch (Exception e) {
            handleException(e);
        }
    }

    protected void handleException(Throwable e) {
        e.printStackTrace();
        JOptionPane.showMessageDialog(this, e.toString());
    }

    protected void resetLogin() {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                CardLayout layout = (CardLayout) mainPanel.getLayout();
                try {
                    layout.show(mainPanel, "login");
                    loginPanel.resetLogin();
                } catch (Exception e) {
                    handleException(e);
                }
            }
        });
    }

    /**
     * Called when a HyperlinkLabel in the application is clicked.
     * 
     * @param event
     */
    public void hyperlinkUpdate(HyperlinkEvent event) {
        getAppletContext().showDocument(event.getURL(), "ggz_url");
    }

    public void invokeAndWait(Runnable doRun) {
        try {
            SwingUtilities.invokeAndWait(doRun);
        } catch (Exception e) {
            handleException(e);
        }
    }
}
