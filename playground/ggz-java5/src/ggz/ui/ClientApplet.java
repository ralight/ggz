package ggz.ui;

import ggz.client.core.ErrorEventData;
import ggz.client.core.MotdEventData;
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
import java.net.URL;

import javax.imageio.ImageIO;
import javax.swing.JApplet;
import javax.swing.JDialog;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextArea;
import javax.swing.SwingUtilities;
import javax.swing.WindowConstants;

public class ClientApplet extends JApplet implements ServerListener {
    protected Server server;

    protected LoginPanel loginPanel;

    protected LoungePanel loungePanel;

    protected RoomPanel roomPanel;

    static {
        // Get Swing to use Antialiased text.
        System.setProperty("swing.aatext", "true");
    }

    public ClientApplet() throws IOException {
        // TODO Make watermark URL and background color applet parameters.
        URL imageUrl = getClass().getResource("/ggz/cards/images/cards/j.gif");
        final Image watermark = ImageIO.read(imageUrl);
        final Composite alphaComposite = AlphaComposite.getInstance(
                AlphaComposite.SRC_OVER, 0.3f);

        setContentPane(new JPanel() {

            public void paintComponent(Graphics g) {
                if (isOpaque()) {
                    Graphics2D g2d = (Graphics2D) g;

                    // Fille the background with a gradient.
                    g2d.setPaint(new GradientPaint(0, 0, Color.WHITE, 0,
                            getHeight() / 2, getBackground(), true));
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
            String host = getParameter("host", "live.ggzgamingzone.org");
            int port = Integer.parseInt(getParameter("port", "5688"));
            server = new Server();
            server.log_session("stderr");
            server.set_hostinfo(host, port, false);
            server.add_event_hook(this);
             getContentPane().setBackground(new Color(0, 128, 255));
            // getContentPane().setBackground(new Color(204, 153, 153));
//            getContentPane().setBackground(new Color(155, 203, 154));
            getContentPane().setLayout(new CardLayout());
            loginPanel = new LoginPanel(server);
            getContentPane().add(loginPanel, "login");
            loginPanel.init();
            loungePanel = new LoungePanel(server);
            getContentPane().add(loungePanel, "lounge");
            roomPanel = new RoomPanel(server);
            getContentPane().add(roomPanel, "room");
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
        System.out.println("channel_connected");
    }

    public void server_channel_fail(String error) {
        JOptionPane.showMessageDialog(this, error);
    }

    public void server_channel_ready() {
        System.out.println("channel_ready");
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
        loginPanel.resetLogin();
    }

    public void server_connected() {
        // Do nothing
    }

    public void server_enter_fail(ErrorEventData data) {
        JOptionPane.showMessageDialog(this, data.message);
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
        JOptionPane.showMessageDialog(this, data.message);
        loginPanel.resetLogin();
        try {
            server.logout();
        } catch (Exception e) {
            handleException(e);
        }
    }

    public void server_logged_out() {
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
    }

    public void server_negotiate_fail(String error) {
        JOptionPane.showMessageDialog(this, error);
        loginPanel.resetLogin();
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
        loginPanel.resetLogin();
    }

    public void server_protocol_error(String error) {
        JOptionPane.showMessageDialog(this, error);
        loginPanel.resetLogin();
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
        switch (server.get_state()) {
        case GGZ_STATE_OFFLINE:
            statusText = "Not logged in";
            break;
        /** In the process of connecting. */
        case GGZ_STATE_CONNECTING:
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
            statusText = "Loading";
            break;
        /** Online, logged in, and in a room. */
        case GGZ_STATE_IN_ROOM:
            statusText = "Logged in";
            break;
        /** Moving between rooms. */
        case GGZ_STATE_BETWEEN_ROOMS:
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
            // Move directly into the lounge.
            server.join_room(0);
        } catch (Exception e) {
            handleException(e);
        }
    }

    protected void handleException(Throwable e) {
        e.printStackTrace();
        JOptionPane.showMessageDialog(this, e.getMessage());
    }
}
