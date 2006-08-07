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

import ggz.common.AdminType;
import ggz.common.ChatType;

import java.awt.Component;
import java.awt.event.ActionEvent;
import java.io.IOException;
import java.text.MessageFormat;
import java.util.ResourceBundle;

import javax.swing.AbstractAction;
import javax.swing.JOptionPane;

public abstract class ChatAction extends AbstractAction {
    private static final ResourceBundle messages = ResourceBundle
            .getBundle("ggz.ui.messages");

    protected ChatPanel chatPanel;

    private final String commands[][] = new String[][] {
            { "/beep", messages.getString("ChatAction.Help.beep") },
            { "/friends", messages.getString("ChatAction.Help.friends") },
            { "/help", messages.getString("ChatAction.Help.help") },
            { "/ignore", messages.getString("ChatAction.Help.ignore") },
            { "/msg", messages.getString("ChatAction.Help.msg") },
            { "/wall", messages.getString("ChatAction.Help.wall") },
            { "/kick", messages.getString("ChatAction.Help.kick") },
            { "/gag", messages.getString("ChatAction.Help.gag") },
            { "/ungag", messages.getString("ChatAction.Help.ungag") },
            { "/ban", messages.getString("ChatAction.Help.ban") },
            { "/clear", messages.getString("ChatAction.Help.clear") } };

    public ChatAction() {
        super(messages.getString("ChatAction.Chat"));
    }

    void setChatPanel(ChatPanel chatPanel) {
        this.chatPanel = chatPanel;
    }

    public void actionPerformed(ActionEvent e) {
        try {
            chat_send(chatPanel.getMessage());
        } catch (IOException ex) {
            ex.printStackTrace();
            JOptionPane.showMessageDialog((Component) e.getSource(), ex
                    .toString());
        }
    }

    /**
     * Sends a chat message to the server, also parses commads within the chat
     * message.
     * 
     * @param message
     *            The text to send to the server as a chat message
     */
    private void chat_send(String message) throws IOException {
        chatPanel.clearMessage();
        if (message != null && !"".equals(message.trim())) {
            String commandString = message.toLowerCase();
            for (int i = 0; i < commands.length; i++) {
                if (commandString.startsWith(commands[i][0])) {
                    // It's a command.
                    doCommand(i, message);
                    return;
                }
            }
            chat_send_msg(message);
        }
    }

    /*
     * Sends a chat message to the server as normal
     * 
     * @param message The text to send as a normal message
     * 
     */
    private void chat_send_msg(String message) throws IOException {
        if (sendChat(getDefaultChatType(), null, message)) {
            chat_display_local(getDefaultChatType(), message);
        }
    }

    /*
     * Sends a chat to a user as private
     * 
     * @param message The text to send as a private message
     * 
     */
    private void chat_send_prvmsg(int commandIndex, String message)
            throws IOException {
        try {
            // Remove /msg command
            message = message.substring(commands[commandIndex][0].length())
                    .trim();
            // Assume that spaces are not allowed in names.
            int nameEnd = message.indexOf(' ');
            String target = message.substring(0, nameEnd);
            message = message.substring(nameEnd + 1);
            if (sendChat(ChatType.GGZ_CHAT_PERSONAL, target, message)) {
                chatPanel.appendCommandText(MessageFormat.format(messages
                        .getString("ChatAction.Result.msg"),
                        new Object[] { target }));
            }
        } catch (IndexOutOfBoundsException ex) {
            /* Could not parse it. */
            chatPanel.appendCommandText(messages
                    .getString("ChatAction.Usage.msg"));
        }
    }

    protected abstract void chat_display_local(ChatType type, String message);

    /*
     * Displays help on all the chat commands
     * 
     */
    private void chat_help() {

        chatPanel.appendCommandText(messages
                .getString("ChatAction.Help.Heading"));

        /* This one is hard-coded into clients. */
        chatPanel.appendCommandText(messages.getString("ChatAction.Help.me"));

        for (int i = 0; i < commands.length; i++) {
            chatPanel.appendCommandText(commands[i][1]);
        }
    }

    /**
     * Adds or removes a name from your friends list.
     * 
     * @param name
     *            name to add
     */
    private void chat_toggle_friends(int commandIndex, String message) {
        // Remove /ignore command
        String name = message.substring(commands[commandIndex][0].length())
                .trim();
        if (name.length() == 0) {
            chatPanel.appendFriendsList();
        } else {
            chatPanel.toggleFriend(name);
        }
    }

    /**
     * Adds or removes a name from your ignore list.
     * 
     * @param name
     *            name to add
     */
    private void chat_toggle_ignore(int commandIndex, String message) {
        // Remove /ignore command
        String name = message.substring(commands[commandIndex][0].length())
                .trim();
        if (name.length() == 0) {
            chatPanel.appendIgnoreList();
        } else {
            chatPanel.toggleIgnore(name);
        }
    }

    /**
     * Sends a beep to a user
     * 
     * @param message
     *            The text to send as a beep message
     */
    private void chat_send_beep(int commandIndex, String message)
            throws IOException {
        try {
            // Remove /beep command
            String target = message.substring(
                    commands[commandIndex][0].length()).trim();
            sendBeep(target);
        } catch (IndexOutOfBoundsException ex) {
            /* Could not parse it. */
            chatPanel.appendCommandText(messages
                    .getString("ChatAction.Usage.beep"));
        }
    }

    public void sendBeep(String target) throws IOException {
        if (sendChat(ChatType.GGZ_CHAT_BEEP, target, null)) {
            chatPanel.appendCommandText(MessageFormat.format(messages
                    .getString("ChatAction.Result.beep"),
                    new Object[] { target }));
        }
    }

    /**
     * Sends a message to all rooms
     * 
     * @para message The text to send as a beep message
     * 
     * Returns:
     */
    private void admin_send_wall(int commandIndex, String message)
            throws IOException {
        try {
            // Remove /wall command
            message = message.substring(commands[commandIndex][0].length())
                    .trim();
            // Assume that spaces are not allowed in names.
            sendChat(ChatType.GGZ_CHAT_ANNOUNCE, null, message);
        } catch (IndexOutOfBoundsException ex) {
            /* Could not parse it. */
            chatPanel.appendCommandText(messages
                    .getString("ChatAction.Usage.wall"));
        }
    }

    /**
     * Boots a player
     * 
     * @param commandIndex
     * @param cmd
     *            /kick <nickname> <reason>
     */
    private void admin_kick(int commandIndex, String cmd) throws IOException {
        try {
            // Remove /kick string
            cmd = cmd.substring(commands[commandIndex][0].length()).trim();
            // Assume that spaces are not allowed in names.
            int nameEnd = cmd.indexOf(' ');
            String nickname = cmd.substring(0, nameEnd);
            String reason = cmd.substring(nameEnd + 1);
            sendAdmin(AdminType.GGZ_ADMIN_KICK, nickname, reason);
        } catch (IndexOutOfBoundsException ex) {
            /* Could not parse it. */
            chatPanel.appendCommandText(messages
                    .getString("ChatAction.Usage.kick"));
        }
    }

    private void admin_gag(int commandIndex, String cmd) throws IOException {
        try {
            // Remove /gag string
            cmd = cmd.substring(commands[commandIndex][0].length()).trim();
            // Assume that spaces are not allowed in names.
            String nickname = cmd.substring(0);
            sendAdmin(AdminType.GGZ_ADMIN_GAG, nickname, null);
        } catch (IndexOutOfBoundsException ex) {
            /* Could not parse it. */
            chatPanel.appendCommandText(messages
                    .getString("ChatAction.Usage.gag"));
        }
    }

    private void admin_ungag(int commandIndex, String cmd) throws IOException {
        try {
            // Remove /ungag string
            cmd = cmd.substring(commands[commandIndex][0].length()).trim();
            // Assume that spaces are not allowed in names.
            String nickname = cmd.substring(0);
            sendAdmin(AdminType.GGZ_ADMIN_UNGAG, nickname, null);
        } catch (IndexOutOfBoundsException ex) {
            /* Could not parse it. */
            chatPanel.appendCommandText(messages
                    .getString("ChatAction.Usage.ungag"));
        }
    }

    private void admin_ban(int commandIndex, String cmd) throws IOException {
        try {
            // Remove /ban string
            cmd = cmd.substring(commands[commandIndex][0].length()).trim();
            // Assume that spaces are not allowed in names.
            String nickname = cmd.substring(0);
            sendAdmin(AdminType.GGZ_ADMIN_BAN, nickname, null);
        } catch (IndexOutOfBoundsException ex) {
            /* Could not parse it. */
            chatPanel.appendCommandText(messages
                    .getString("ChatAction.Usage.ban"));
        }
    }

    /**
     * Performs the requested chat operation. If a particular chat operation is
     * not allowed this method should return false.
     * 
     * @param chatType
     * @param target
     * @param message
     * @return true if the operation was performed, false otherwise.
     * @throws IOException
     */
    protected abstract boolean sendChat(ChatType chatType, String target,
            String message) throws IOException;

    protected abstract ChatType getDefaultChatType();

    protected abstract boolean sendAdmin(AdminType type, String target,
            String reason) throws IOException;

    private void doCommand(int commandIndex, String commandString)
            throws IOException {
        switch (commandIndex) {
        case 0:
            chat_send_beep(commandIndex, commandString);
            break;
        case 1:
            chat_toggle_friends(commandIndex, commandString);
            break;
        case 3:
            chat_toggle_ignore(commandIndex, commandString);
            break;
        case 4:
            chat_send_prvmsg(commandIndex, commandString);
            break;
        case 5:
            admin_send_wall(commandIndex, commandString);
            break;
        case 6:
            admin_kick(commandIndex, commandString);
            break;
        case 7:
            admin_gag(commandIndex, commandString);
            break;
        case 8:
            admin_ungag(commandIndex, commandString);
            break;
        case 9:
            admin_ban(commandIndex, commandString);
            break;
        case 10:
            chatPanel.clearChat();
            break;
        default:
            chat_help();
            break;
        }
    }
}
