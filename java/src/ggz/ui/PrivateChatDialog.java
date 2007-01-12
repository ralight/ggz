package ggz.ui;

import ggz.client.core.ChatEventData;
import ggz.client.core.ErrorEventData;
import ggz.client.core.MotdEventData;
import ggz.client.core.Player;
import ggz.client.core.Room;
import ggz.client.core.RoomChangeEventData;
import ggz.client.core.RoomListener;
import ggz.client.core.Server;
import ggz.client.core.ServerListener;
import ggz.client.core.Table;
import ggz.client.core.TableLeaveEventData;
import ggz.common.AdminType;
import ggz.common.ChatType;

import java.awt.AWTEvent;
import java.awt.BorderLayout;
import java.awt.Image;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowEvent;
import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;

import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.SwingUtilities;
import javax.swing.Timer;
import javax.swing.WindowConstants;

public class PrivateChatDialog extends JFrame {
    protected static final HashMap dialogs = new HashMap();

    protected static Server server;

    protected static ChatDialogManager dialogManager = new ChatDialogManager();

    protected Timer iconBlinker;

    protected ChatPanel chatPanel;

    private String recipient;

    protected Image normalIcon;

    protected Image alertIcon;

    public static void setServer(Server s) {
        if (server != null) {
            server.remove_event_hook(dialogManager);
        }
        server = s;
        server.add_event_hook(dialogManager);
    }

    private PrivateChatDialog(String recipient) {
        super(recipient + " - Private chat");
        this.recipient = recipient;
        this.chatPanel = new ChatPanel(new PrivateChatAction(recipient));
        this.getContentPane().add(chatPanel, BorderLayout.CENTER);
        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);
        this.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
        this.normalIcon = new ImageIcon(this.getClass().getResource(
                "/ggz/ui/images/private_chat.png")).getImage();
        this.alertIcon = new ImageIcon(this.getClass().getResource(
                "/ggz/ui/images/comments_add.png")).getImage();
        this.setIconImage(this.normalIcon);
    }

    public void dispose() {
        this.chatPanel.dispose();
        setBlinking(false);
        super.dispose();
    }

    public static PrivateChatDialog getOrCreateDialog(String otherPlayer,
            int initialState) {
        PrivateChatDialog dialog = (PrivateChatDialog) dialogs.get(otherPlayer);
        if (dialog == null) {
            // No private chat with this player yet.
            dialog = new PrivateChatDialog(otherPlayer);
            dialogs.put(otherPlayer, dialog);
            dialog.setSize(400, 350);
            dialog.setExtendedState(initialState);
            dialog.setVisible(true);
            dialog.chatPanel.textField.requestFocus();
        }
        return dialog;
    }

    protected void processWindowEvent(WindowEvent event) {
        if (event.getID() == WindowEvent.WINDOW_CLOSED) {
            dialogs.remove(this.recipient);
        } else if (event.getID() == WindowEvent.WINDOW_ACTIVATED) {
            // If a message is received while this is not the active window then
            // the icon will have been changed to alert the user. Now that the
            // window has received focus the user is aware of the message so we
            // can change the icon back.
            if (isBlinking()) {
                setBlinking(false);
            }

            // Windows created as a result of new messages are initially
            // iconified and for some reason the layout isn't quite right. This
            // just forces the components to organise themsleves again. It's
            // harmless to do it if it's not needed.
            validate();
        }
        super.processWindowEvent(event);
    }

    public boolean isBlinking() {
        return this.iconBlinker != null;
    }

    public void setBlinking(boolean blink) {
        if (blink) {
            if (iconBlinker == null) {
                this.iconBlinker = new Timer(500, new ActionListener() {
                    public void actionPerformed(ActionEvent event) {
                        if (getIconImage() == normalIcon) {
                            setIconImage(alertIcon);
                        } else {
                            setIconImage(normalIcon);
                        }
                    }
                });
                this.setIconImage(this.alertIcon);
                this.iconBlinker.start();
            }
        } else {
            if (this.iconBlinker != null) {
                this.iconBlinker.stop();
                this.iconBlinker = null;
            }

            if (getIconImage() != normalIcon) {
                this.setIconImage(this.normalIcon);
            }
        }
    }

    protected class PrivateChatAction extends ChatAction {
        private String player;

        public PrivateChatAction(String recipient) {
            this.player = recipient;
        }

        public boolean sendChat(ChatType type, String target, String message)
                throws IOException {
            server.get_cur_room().chat(ChatType.GGZ_CHAT_PERSONAL, player,
                    message);
            return true;
        }

        protected boolean sendAdmin(AdminType type, String target, String reason)
                throws IOException {
            server.get_cur_room().admin(type, target, reason);
            return true;
        }

        protected void chat_display_local(ChatType type, String message) {
            String handle = server.get_handle();
            chatPanel.appendChat(type, handle, message, server.get_handle());
        }

        protected ChatType getDefaultChatType() {
            return ChatType.GGZ_CHAT_PERSONAL;
        }
    }

    protected static class ChatDialogManager implements RoomListener,
            ServerListener {
        protected Room currentRoom;

        public void server_channel_connected() {
            // Ignore.
        }

        public void server_channel_fail(String error) {
            // Ignore.
        }

        public void server_channel_ready() {
            // Ignore.
        }

        public void server_chat_fail(ErrorEventData data) {
            // Ignore.
        }

        public void server_connect_fail(String error) {
            // Ignore.
        }

        public void server_connected() {
            // Ignore.
        }

        public void server_enter_fail(ErrorEventData data) {
            // Ignore.
        }

        public void server_enter_ok() {
            // Stop listening for chat events in the old room and listen for
            // chat messages in the new room.
            if (this.currentRoom != null) {
                this.currentRoom.remove_event_hook(this);
            }
            this.currentRoom = server.get_cur_room();
            this.currentRoom.add_event_hook(this);
        }

        public void server_list_rooms() {
            // Ignore.
        }

        public void server_rooms_changed() {
            // Ignore.
        }

        public void server_list_types() {
            // Ignore.
        }

        public void server_logged_out() {
            // Close all open private chat dialogs.
            Iterator iter = dialogs.keySet().iterator();
            while (iter.hasNext()) {
                Object key = iter.next();
                PrivateChatDialog dialog = (PrivateChatDialog) dialogs.get(key);
                dialog.setVisible(false);
                dialog.dispose();
            }
            // This shouldn't be needed since the dialogs remove themselves on
            // close but do it just in case.
            dialogs.clear();
        }

        public void server_login_fail(ErrorEventData data) {
            // Ignore.
        }

        public void server_login_ok() {
            // Ignore.
        }

        public void server_motd_loaded(MotdEventData data) {
            // Ignore.
        }

        public void server_negotiate_fail(String error) {
            // Ignore.
        }

        public void server_negotiated() {
            // Ignore.
        }

        public void server_net_error(String error) {
            // Ignore.
        }

        public void server_players_changed() {
            // Ignore.
        }

        public void server_protocol_error(String error) {
            // Ignore.
        }

        public void server_state_changed() {
            // Ignore.
        }

        public void chat_event(final ChatEventData data) {
            // We are only interested in private chat.
            if (data.type != ChatType.GGZ_CHAT_PERSONAL) {
                return;
            }

            // Don't popup a dialog if we are ignoring the sender.
            if (ChatPanel.isIgnored(data.sender)) {
                return;
            }

            // All handlers are called from the socket thread so we need to do
            // this crazy stuff.
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    PrivateChatDialog dialog = getOrCreateDialog(data.sender,
                            ICONIFIED);
                    dialog.chatPanel.appendChat(data.type, data.sender,
                            data.message, server.get_handle());

                    // If the window is not the active window then let the user
                    // know that a message has arrived. We used to bring the
                    // window to the front but this was quite annoying if you
                    // were currently typing in another window.
                    if (!dialog.isActive()) {
                        dialog.setBlinking(true);
                    }
                }
            });
        }

        public void player_count(int count) {
            // Ignore.
        }

        public void player_lag(Player player) {
            // Ignore.
        }

        public void player_list(List players) {
            // Ignore.
        }

        public void player_stats(Player player) {
            // Ignore.
        }

        public void room_enter(RoomChangeEventData data) {
            // Ignore.
        }

        public void room_leave(RoomChangeEventData data) {
            // Ignore.
        }

        public void table_add(Table table) {
            // Ignore.
        }

        public void table_delete(Table table) {
            // Ignore.
        }

        public void table_join_fail(String error) {
            // Ignore.
        }

        public void table_joined(int table_index) {
            // Ignore.
        }

        public void table_launch_fail(ErrorEventData data) {
            // Ignore.
        }

        public void table_launched() {
            // Ignore.
        }

        public void table_leave_fail(String error) {
            // Ignore.
        }

        public void table_left(TableLeaveEventData data) {
            // Ignore.
        }

        public void table_list() {
            // Ignore.
        }

        public void table_update(Table table) {
            // Ignore.
        }
    }
}
