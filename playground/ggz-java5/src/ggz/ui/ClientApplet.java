package ggz.ui;

import ggz.client.core.ErrorEventData;
import ggz.client.core.MotdEventData;
import ggz.client.core.Room;
import ggz.client.core.Server;
import ggz.client.core.ServerListener;
import ggz.client.core.StateID;

import java.awt.AlphaComposite;
import java.awt.CardLayout;
import java.awt.Color;
import java.awt.Composite;
import java.awt.GradientPaint;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.io.IOException;
import java.net.URI;
import java.net.URL;

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
    protected Server server;

    protected LoginPanel loginPanel;

    protected LoungePanel loungePanel;

    protected JLabel busyPanel;

    protected RoomPanel roomPanel;

    private ErrorEventData loginFailData;
    
    private String uriPath;

    static {
        // Get Swing to use Antialiased text.
        System.setProperty("swing.aatext", "true");

        // Make tooltips appear without a delay, this is currently because the
        // options in force for a card game are shown by hovering over a label
        // and the delay makes the interface unintuitive.
        // ToolTipManager.sharedInstance().setInitialDelay(0);
    }

    public ClientApplet() throws IOException {
        // TODO Make watermark URL and background color applet parameters.
        //URL imageUrl = getClass().getResource("/ggz/ui/images/rose.gif");
        final Image watermark = null; //ImageIO.read(imageUrl);
        final Composite alphaComposite = AlphaComposite.getInstance(
                AlphaComposite.SRC_OVER, 0.3f);

        // Create a custom content pane that does fancy painting.
        setContentPane(new JPanel() {

            public void paintComponent(Graphics g) {
                if (isOpaque()) {
                    Graphics2D g2d = (Graphics2D) g;

                    // Fille the background with a gradient.
//                    g2d.setPaint(new GradientPaint(0, 0, Color.WHITE, 0,
//                            getHeight() / 2, getBackground(), true));
                    g2d.setPaint(new GradientPaint(0, 0, new Color(0xce, 0xfa, 0xdf), 0,
                            getHeight() / 2, new Color(0x7c, 0xaf, 0x68), true));
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
            // UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
            URI uri = new URI(getParameter("uri",
                    "ggz://live.ggzgamingzone.org:5688/Entry%20Room"));
            String host = uri.getHost();
            int port = uri.getPort();
            uriPath = uri.getPath();
            String sendLogFile = getParameter("sendLog");
            String receiveLogFile = getParameter("receiveLog");
            server = new Server(host, port, false);
            server.log_session(sendLogFile, receiveLogFile);
            server.add_event_hook(this);
            //getContentPane().setBackground(new Color(0, 128, 255));
            //getContentPane().setBackground(new Color(0x7c, 0xaf, 0x68));
            //getContentPane().setBackground(new Color(0xce, 0xfa, 0xdf));
            //getContentPane().setBackground(new Color(155, 203, 154));
            getContentPane().setLayout(new CardLayout());
            loginPanel = new LoginPanel(server);
            getContentPane().add(loginPanel, "login");
            loginPanel.init(uri.getUserInfo());
            loungePanel = new LoungePanel(server);
            getContentPane().add(loungePanel, "lounge");
            roomPanel = new RoomPanel(server);
            getContentPane().add(roomPanel, "room");
            busyPanel = new JLabel("Please wait...", SwingConstants.CENTER);
            getContentPane().add(busyPanel, "busy");
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
                CardLayout layout = (CardLayout) getContentPane().getLayout();
                try {
                    if (server.get_cur_room().get_gametype() == null) {
                        loungePanel.setRoom(server.get_cur_room());
                        layout.show(getContentPane(), "lounge");
                    } else {
                        roomPanel.setRoom(server.get_cur_room());
                        layout.show(getContentPane(), "room");
                    }
                } catch (Exception e) {
                    handleException(e);
                }
            }
        });
    }

    public void server_login_ok() {
        try {
            // server.list_gametypes(true);
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
                    "Message of the Day");
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
        // CardLayout layout = (CardLayout) getContentPane().getLayout();
        // layout.show(getContentPane(), "login");
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
        CardLayout layout = (CardLayout) getContentPane().getLayout();

        switch (server.get_state()) {
        case GGZ_STATE_OFFLINE:
            statusText = "Not logged in";
            break;
        /** In the process of connecting. */
        case GGZ_STATE_CONNECTING:
            loginFailData = null;
            layout.show(getContentPane(), "busy");
            statusText = "Connecting";
            break;
        /** Continuous reconnection attempts. */
        case GGZ_STATE_RECONNECTING:
            statusText = "Reconnecting";
            break;
        /** Connected, but not doing anything. */
        case GGZ_STATE_ONLINE:
            statusText = "Connected";
            break;
        /** In the process of logging in. */
        case GGZ_STATE_LOGGING_IN:
            statusText = "Logging in";
            break;
        /** Online and logged in! */
        case GGZ_STATE_LOGGED_IN:
            statusText = "Logged in";
            break;
        /** Moving into a room. */
        case GGZ_STATE_ENTERING_ROOM:
        /** Moving between rooms. */
        case GGZ_STATE_BETWEEN_ROOMS:
            layout.show(getContentPane(), "busy");
            statusText = "Loading";
            break;
        /** Online, logged in, and in a room. */
        case GGZ_STATE_IN_ROOM:
            statusText = "Logged in";
            break;
        /** Trying to launch a table. */
        case GGZ_STATE_LAUNCHING_TABLE:
            statusText = "Launching game";
            break;
        /** Trying to join a table. */
        case GGZ_STATE_JOINING_TABLE:
            statusText = "Joining game";
            break;
        /** Online, loggied in, in a room, at a table. */
        case GGZ_STATE_AT_TABLE:
            statusText = "Playing";
            break;
        /** Waiting to leave a table. */
        case GGZ_STATE_LEAVING_TABLE:
            statusText = "Leaving game";
            break;
        /** In the process of logging out. */
        case GGZ_STATE_LOGGING_OUT:
            statusText = "Disconnecting";
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
                // Move directly into the room specified by the URI.
                for (int room_num = 0; room_num < server.get_num_rooms(); room_num++) {
                    Room room = server.get_nth_room(room_num);
                    if (roomToEnter.equals(room.get_name())) {
                        server.join_room(room.get_id());
                        return;
                    }
                }
            }
            
            // If we got this far then either there was no room specified or no room with that name was found.
            server.join_room(0);
        } catch (Exception e) {
            handleException(e);
        }
    }

    protected void handleException(Throwable e) {
        e.printStackTrace();
        JOptionPane.showMessageDialog(this, e.getMessage());
    }

    protected void resetLogin() {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                CardLayout layout = (CardLayout) getContentPane().getLayout();
                try {
                    layout.show(getContentPane(), "login");
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
     * @param e
     */
    public void hyperlinkUpdate(HyperlinkEvent e) {
        getAppletContext().showDocument(e.getURL(), "ggz_rules");
    }

    public void invokeAndWait(Runnable doRun) {
        try {
            SwingUtilities.invokeAndWait(doRun);
        } catch (Exception e) {
            handleException(e);
        }
    }
}
