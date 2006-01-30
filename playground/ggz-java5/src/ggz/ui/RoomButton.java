package ggz.ui;

import ggz.client.core.ChatEventData;
import ggz.client.core.ErrorEventData;
import ggz.client.core.GameType;
import ggz.client.core.Module;
import ggz.client.core.Room;
import ggz.client.core.RoomChangeEventData;
import ggz.client.core.RoomListener;
import ggz.client.core.Server;
import ggz.client.core.TableLeaveEventData;

import java.awt.AWTEvent;
import java.awt.SystemColor;
import java.awt.event.MouseEvent;
import java.io.IOException;

import javax.swing.ImageIcon;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.SwingConstants;

public class RoomButton extends JLabel implements RoomListener {
    private Room room;

    public RoomButton(Room room) {
        GameType gameType = room.get_gametype();
        Module module = Module.get_nth_by_type(gameType, 0);
        this.room = room;
        room.add_event_hook(this);
        updateText();
        setIcon(new ImageIcon(getClass().getResource(module.get_icon_path())));
        setHorizontalAlignment(SwingConstants.CENTER);
        setBackground(SystemColor.text);
        setVerticalAlignment(SwingConstants.TOP);
        setVerticalTextPosition(SwingConstants.BOTTOM);
        setHorizontalTextPosition(SwingConstants.CENTER);
        setOpaque(false);
        enableEvents(AWTEvent.MOUSE_EVENT_MASK);
    }

    private void updateText() {
        setText("<HTML><B>" + room.get_name()
                + "</B><BR><span style='font-weight:normal'>"
                + room.get_num_players() + " players</span>");
    }

    protected void processMouseEvent(MouseEvent e) {
        switch (e.getID()) {
        case MouseEvent.MOUSE_CLICKED:
            // Find the index of the room and then join the room
            try {
                Server server = room.get_server();
                for (int i = 0; i < server.get_num_rooms(); i++) {
                    if (server.get_nth_room(i) == room) {
                        server.join_room(i);
                    }
                }
            } catch (IOException ex) {
                ex.printStackTrace();
                JOptionPane.showMessageDialog(this, ex.toString());
            }
            break;
        case MouseEvent.MOUSE_ENTERED:
            setBackground(SystemColor.textHighlight);
            setForeground(SystemColor.textHighlightText);
            break;
        case MouseEvent.MOUSE_EXITED:
            setBackground(SystemColor.text);
            setForeground(SystemColor.textText);
            break;
        }
        super.processMouseMotionEvent(e);
    }

    public void chat_event(ChatEventData data) {
        // Not needed
    }

    public void player_count(int room_id) {
        updateText();
    }

    public void player_lag(String player) {
        // Not needed
    }

    public void player_list(int room_id) {
        // Not needed
    }

    public void player_stats(String player) {
        // Not needed
    }

    public void room_enter(RoomChangeEventData data) {
        // Not needed
    }

    public void room_leave(RoomChangeEventData data) {
        // Not needed
    }

    public void table_join_fail(String error) {
        // Not needed
    }

    public void table_joined(int table_index) {
        // Not needed
    }

    public void table_launch_fail(ErrorEventData data) {
        // Not needed
    }

    public void table_launched() {
        // Not needed
    }

    public void table_leave_fail(String error) {
        // Not needed
    }

    public void table_left(TableLeaveEventData data) {
        // Not needed
    }

    public void table_list() {
        // Not needed
    }

    public void table_update() {
        // Not needed
    }
}
