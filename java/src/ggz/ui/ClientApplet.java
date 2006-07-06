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
import ggz.client.core.Module;
import ggz.client.core.MotdEventData;
import ggz.client.core.Room;
import ggz.client.core.Server;
import ggz.client.core.ServerListener;
import ggz.client.core.StateID;
import ggz.ui.preferences.PreferencesDialog;

import java.applet.Applet;
import java.awt.AlphaComposite;
import java.awt.BorderLayout;
import java.awt.CardLayout;
import java.awt.Color;
import java.awt.Composite;
import java.awt.Cursor;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.GradientPaint;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.IOException;
import java.lang.reflect.Method;
import java.net.MalformedURLException;
import java.net.SocketException;
import java.net.URI;
import java.net.URL;
import java.util.ResourceBundle;
import java.util.logging.Logger;

import javax.swing.ImageIcon;
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
    protected static final Logger log = Logger.getLogger(ClientApplet.class
            .getName());

    protected static final ResourceBundle messages = ResourceBundle
            .getBundle("ggz.ui.messages");

    private static final int DEFAULT_PORT = 5688;

    protected Server server;

    protected JLabel aboutLabel;

    protected JLabel totalPlayerCountLabel;

    protected JPanel mainPanel;

    protected JPanel footerPanel;

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

    public ClientApplet() {
    }

    public void init() {
        // Register ourselves as the global hyperlink handler for our
        // hyperlink label.
        HyperlinkLabel.setGlobalHyperlinkListener(this);

        if (getBooleanParameter("theme.enabled", false)) {
            installCustomTheme();
        }

        try {
            // Parse URI parameter and set up server.
            URI uri = new URI(getParameter("uri",
                    "ggz://oojah.dyndns.org:5688/Entry%20Room"));
            String host = uri.getHost();
            int port = uri.getPort() == -1 ? DEFAULT_PORT : uri.getPort();
            uriPath = uri.getPath();
            String sendLogFile = getParameter("xmlout");
            String receiveLogFile = getParameter("xmlin");
            server = new Server(host, port, false);
            server.log_session(sendLogFile, receiveLogFile);
            server.add_event_hook(this);

            initFancyBackground();

            // Init components
            getContentPane().setLayout(new BorderLayout());
            mainPanel = new JPanel(new CardLayout());
            mainPanel.setOpaque(false);
            getContentPane().add(mainPanel, BorderLayout.CENTER);

            // Footer.
            JPanel footerLayoutPanel = new JPanel(new BorderLayout(0, 0));
            footerLayoutPanel.setOpaque(false);
            getContentPane().add(footerLayoutPanel, BorderLayout.SOUTH);
            totalPlayerCountLabel = new JLabel();
            totalPlayerCountLabel.setFont(totalPlayerCountLabel.getFont().deriveFont(Font.PLAIN));
            footerLayoutPanel.add(totalPlayerCountLabel, BorderLayout.WEST);
            footerPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT, 5, 0));
            footerPanel.setOpaque(false);
            footerLayoutPanel.add(footerPanel, BorderLayout.EAST);
            JLabel preferencesLabel = new JLabel(
                    "<HTML><A href='' style='font-weight:normal'>"
                            + "Preferences..." + "</A>", SwingConstants.RIGHT);
            preferencesLabel.setCursor(Cursor
                    .getPredefinedCursor(Cursor.HAND_CURSOR));
            preferencesLabel.addMouseListener(new MouseAdapter() {
                public void mouseClicked(MouseEvent event) {
                    PreferencesDialog
                            .showPreferences(
                                    ClientApplet.this,
                                    new String[] { "ggz.ui.preferences.ChatPreferencesTab" });
                }
            });
            footerPanel.add(preferencesLabel);
            footerPanel.add(new HyperlinkLabel("Help", new URL(getCodeBase(),
                    "help/help.html"), "font-weight:normal"),
                    BorderLayout.SOUTH);
            aboutLabel = new JLabel(
                    "<HTML><A href='' style='font-weight:normal'>"
                            + messages.getString("ClientApplet.Label.About")
                            + "</A>", SwingConstants.RIGHT);
            aboutLabel
                    .setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
            aboutLabel.addMouseListener(new MouseAdapter() {
                public void mouseClicked(MouseEvent event) {
                    AboutDialog.showDialog(ClientApplet.this);
                }
            });
            footerPanel.add(aboutLabel);

            loginPanel = new LoginPanel(server);
            mainPanel.add(loginPanel, "login");
            loungePanel = new LoungePanel(server);
            mainPanel.add(loungePanel, "lounge");
            roomPanel = new RoomPanel(server);
            mainPanel.add(roomPanel, "room");
            busyPanel = new JLabel(messages
                    .getString("ClientApplet.Label.PleaseWait"),
                    SwingConstants.CENTER);
            mainPanel.add(busyPanel, "busy");
            loginPanel.init(uri.getUserInfo());
        } catch (Exception e) {
            handleException(e);
        }

        // Set the server so that we can be notified of room enter/leave.
        PrivateChatDialog.setServer(server);
    }

    public void start() {
        // Nothing to do since it's already done in init().
    }

    public void stop() {
        try {
            if (server != null
                    && server.get_state() != StateID.GGZ_STATE_OFFLINE
                    && server.get_state() != StateID.GGZ_STATE_LOGGING_OUT
                    && server.get_state() != StateID.GGZ_STATE_RECONNECTING) {
                server.logout();
            }
        } catch (SocketException e) {
            // Trying to track this bugger down...
            System.err.println(server.get_state());
            handleException(e);
        } catch (Exception e) {
            handleException(e);
        }
    }

    public String[][] getParameterInfo() {
        return new String[][] {
                { "uri", "URI",
                        "e.g. ggz://live.ggzgamingzone.org:5688/A%20Room" },
                { "xmlin", "stdout|stderr",
                        "Where to log XML recieved from server." },
                { "xmlout", "stdout|stderr",
                        "Where to log XML sent to the server." },
                { "background.image.enabled", "boolean",
                        "Whether to draw the watermark image." },
                { "background.image.url", "URL",
                        "Location of image to use as the watermark." },
                { "background.gradient.enabled", "boolean",
                        "Whether to enable gradient painting for the background." },
                { "background.gradient.color1", "int",
                        "RGB string to use for the gradient." },
                { "background.gradient.color2", "int",
                        "RGB string to use for the gradient." },
                { "theme.enabled", "boolean",
                        "Whether to enable theme support." },
                { "theme.black", "Color",
                        "The color for things that are generally black like text." },
                { "theme.white", "Color",
                        "The color for things that are generally white like text fields." },
                { "theme.primary1", "Color", "" },
                { "theme.primary2", "Color", "" },
                { "theme.primary3", "Color", "" },
                { "theme.secondary", "Color", "" },
                { "theme.secondary2", "Color", "" },
                { "theme.secondary3", "Color", "" },
                { "theme.controlTextFont", "Font",
                        "The font for controls; like buttons and labels." },
                { "theme.menuTextFont", "Font", "The font for menus." },
                { "theme.subTextFont", "Font", "" },
                { "theme.systemTextFont", "Font", "" },
                { "theme.userTextFont", "Font",
                        "The font for controls that allow user input; like text fields." },
                { "theme.windowTitleFont", "Font", "" }, };
    }

    private void installCustomTheme() {
        CustomMetalTheme.install(this);
    }

    private void initFancyBackground() {
        // Init backgound painting stuff.
        final boolean isGradientEnabled = getBooleanParameter(
                "background.gradient.enabled", true);
        final Image watermark = getBooleanParameter("background.image.enabled",
                true) ? getWatermark() : null;
        final Color gradientColor1 = getGradientColor1();
        final Color gradientColor2 = getGradientColor2();
        final Composite alphaComposite = AlphaComposite.getInstance(
                AlphaComposite.SRC_OVER, 0.3f);

        // Create a custom content pane that does fancy painting.
        setContentPane(new JPanel() {

            public void paintComponent(Graphics g) {
                if (isOpaque()) {
                    Graphics2D g2d = (Graphics2D) g;

                    if (isGradientEnabled) {
                        // Fill the background with a gradient.
                        g2d.setPaint(new GradientPaint(0, 0, gradientColor1, 0,
                                getHeight() / 2, gradientColor2, true));
                    } else {
                        g2d.setPaintMode();
                        g2d.setColor(getBackground());
                    }
                    g2d.fillRect(0, 0, getWidth(), getHeight());

                    if (watermark != null) {
                        // Paint the watermark.
                        Composite originalComposite = g2d.getComposite();
                        g2d.setComposite(alphaComposite);
                        g2d.drawImage(watermark, getWidth() - 60
                                - watermark.getWidth(this), 60, this);
                        g2d.setComposite(originalComposite);
                    }
                }
            }

        });
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

    /**
     * Reads integer string from applet parameter. See the Integer.decode()
     * method for supported string formats.
     * 
     * @param name
     * @param defaultValue
     * @return
     * @see Integer#decode(java.lang.String)
     */
    protected int getIntParameter(String name, int defaultValue) {
        String rgbString = getParameter(name);

        if (rgbString != null) {
            try {
                return Integer.decode(rgbString).intValue();
            } catch (NumberFormatException e) {
                log.warning(e.getMessage());
            }
        }

        // Applet parameter was not present or invalid.
        return defaultValue;
    }

    protected boolean getBooleanParameter(String name, boolean defaultValue) {
        String booleanString = getParameter(name);
        if (booleanString == null) {
            return defaultValue;
        }
        return Boolean.valueOf(booleanString).booleanValue();
    }

    protected Color getGradientColor1() {
        return new Color(
                getIntParameter("background.gradient.color1", 0xCEFADF));
    }

    protected Color getGradientColor2() {
        return new Color(
                getIntParameter("background.gradient.color2", 0x7CAF68));
    }

    protected Image getWatermark() {
        String customImageUrl = getParameter("background.image.url");

        if (customImageUrl != null) {
            if ("".equals(customImageUrl.trim())) {
                return null;
            }

            try {
                return new ImageIcon(new URL(customImageUrl)).getImage();
            } catch (MalformedURLException e) {
                log.warning(e.getMessage());
                return null;
            }
        }

        return new ImageIcon(getClass().getResource(
                "/ggz/ui/images/watermark.png")).getImage();
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
                    JOptionPane.getFrameForComponent(this),
                    messages
                            .getString("ClientApplet.DialogTitle.MessageOfTheDay"));
            JTextArea textArea = new JTextArea(data.motd);

            textArea.setEditable(false);
            dialog.add(textArea);
            dialog.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
            dialog.pack();
            dialog.setVisible(true);
        }
        loungePanel.setMotD(data, this);
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
        // Count the number of players in all the rooms that we have a module
        // for. We can't use Server.get_num_players() since we aren't showing
        // all rooms. This method is called on the socket read thread so room
        // player counts should not change while we are adding it all up. Also
        // include cound of players in the entry room.
        int count = server.get_room_by_id(0).get_num_players();
        for (int i = 0; i < server.get_num_rooms(); i++) {
            Room room = server.get_nth_room(i);
            if (room.get_gametype() != null
                    && Module.get_num_by_type(room.get_gametype()) > 0) {
                count += room.get_num_players();
            }
        }

        final int totalCount = count;
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                totalPlayerCountLabel.setText(totalCount
                        + (totalCount == 1 ? " player" : " players")
                        + " online");
            }
        });

        // Needed so that components that show player counts can refresh
        // themselves.
        repaint();
    }

    public void server_state_changed() {
        String statusText;
        CardLayout layout = (CardLayout) mainPanel.getLayout();

        if (server.get_state() == StateID.GGZ_STATE_OFFLINE) {
            statusText = messages.getString("ClientApplet.StateOffline");
            /** In the process of connecting. */
        } else if (server.get_state() == StateID.GGZ_STATE_CONNECTING) {
            loginFailData = null;
            layout.show(mainPanel, "busy");
            statusText = messages.getString("ClientApplet.StateConnecting");
            /** Continuous reconnection attempts. */
        } else if (server.get_state() == StateID.GGZ_STATE_RECONNECTING) {
            statusText = messages.getString("ClientApplet.StateReconnecting");
            /** Connected, but not doing anything. */
        } else if (server.get_state() == StateID.GGZ_STATE_ONLINE) {
            statusText = messages.getString("ClientApplet.StateConnected");
            /** In the process of logging in. */
        } else if (server.get_state() == StateID.GGZ_STATE_LOGGING_IN) {
            statusText = messages.getString("ClientApplet.StateLoggingIn");
            /** Online and logged in! */
        } else if (server.get_state() == StateID.GGZ_STATE_LOGGED_IN) {
            statusText = messages.getString("ClientApplet.StateLoggedIn");
            /** Moving into a room. */
        } else if (server.get_state() == StateID.GGZ_STATE_ENTERING_ROOM) {
            layout.show(mainPanel, "busy");
            statusText = messages.getString("ClientApplet.StateEnteringRoom");
            /** Moving between rooms. */
        } else if (server.get_state() == StateID.GGZ_STATE_BETWEEN_ROOMS) {
            layout.show(mainPanel, "busy");
            statusText = messages.getString("ClientApplet.StateBetweenRooms");
            /** Online, logged in, and in a room. */
        } else if (server.get_state() == StateID.GGZ_STATE_IN_ROOM) {
            statusText = messages.getString("ClientApplet.StateInRoom");
            /** Trying to launch a table. */
        } else if (server.get_state() == StateID.GGZ_STATE_LAUNCHING_TABLE) {
            statusText = messages.getString("ClientApplet.StateLaunchingGame");
            /** Trying to join a table. */
        } else if (server.get_state() == StateID.GGZ_STATE_JOINING_TABLE) {
            statusText = messages.getString("ClientApplet.StateJoiningGame");
            /** Online, loggied in, in a room, at a table. */
        } else if (server.get_state() == StateID.GGZ_STATE_AT_TABLE) {
            statusText = messages.getString("ClientApplet.StatePlaying");
            /** Waiting to leave a table. */
        } else if (server.get_state() == StateID.GGZ_STATE_LEAVING_TABLE) {
            statusText = messages.getString("ClientApplet.StateLeavingGame");
            /** In the process of logging out. */
        } else if (server.get_state() == StateID.GGZ_STATE_LOGGING_OUT) {
            statusText = messages.getString("ClientApplet.StateLoggingOut");
        } else {
            statusText = "";
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
                    totalPlayerCountLabel.setText(null);
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
        if (event.getEventType() == HyperlinkEvent.EventType.ACTIVATED) {
            try {
                Class jsObjectClass = Class
                        .forName("netscape.javascript.JSObject");
                Method getWindow = jsObjectClass.getMethod("getWindow",
                        new Class[] { Applet.class });
                Method eval = jsObjectClass.getMethod("eval",
                        new Class[] { String.class });
                Object window = getWindow.invoke(jsObjectClass,
                        new Object[] { this });
                eval
                        .invoke(
                                window,
                                new Object[] { "var popup = window.open(\""
                                        + event.getURL()
                                        + "\", \"ggz\", \"menubar=no,resizable=yes,scrollbars=yes,status=no,titlebar=no,height=500,width=640\");"
                                        + "if (popup == null) {"
                                        + "alert(\"It seems popups are being blocked, either disable your popup blocker for this site or try holding down the CTRL key when you click the link.\");"
                                        + "} else {popup.focus();}" });
            } catch (Exception ex) {
                // Ignore, just use showDocument() instead.
                getAppletContext().showDocument(event.getURL(), "ggz_url");
            }
        }
    }

    public void invokeAndWait(Runnable doRun) {
        try {
            SwingUtilities.invokeAndWait(doRun);
        } catch (Exception e) {
            handleException(e);
        }
    }
}
