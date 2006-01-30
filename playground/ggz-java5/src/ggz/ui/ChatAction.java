package ggz.ui;

import ggz.common.ChatType;

import java.awt.Component;
import java.awt.event.ActionEvent;
import java.io.IOException;

import javax.swing.AbstractAction;
import javax.swing.JOptionPane;

public abstract class ChatAction extends AbstractAction {
    protected ChatPanel chatPanel;

    private final String commands[][] = new String[][] {
            { "/msg",
                    "/msg <username> <message> . Private message a player" },
            { "/table", "/table <message> .......... Message to your table" },
            { "/wall", "/wall <message> ........... Admin command" },
            { "/beep", "/beep <username> .......... Beep a player" },
            { "/help", "/help ..................... This help message" },
            { "/friends",
                    "/friends <username>........ Add player to your friends list" },
            { "/ignore", "/ignore <username>......... Ignore a player" } };

    void setChatPanel(ChatPanel chatPanel) {
        this.chatPanel = chatPanel;
    }

    public Object getValue(String key) {
        if (NAME.equals(key)) {
            return "Chat";
        }
        return super.getValue(key);
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
     * Sends a chat message to the server, also parses commads within the
     * chat message.
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
        sendChat(getDefaultChatType(), null, message);
        chat_display_local(getDefaultChatType(), message);
    }

    /*
     * Sends a chat to a user as private
     * 
     * @param message The text to send as a private message
     * 
     */
    private void chat_send_prvmsg(String message) throws IOException {
        try {
            // Remove /msg command
            message = message.substring(commands[0][0].length()).trim();
            // Assume that spaces are not allowed in names.
            int nameEnd = message.indexOf(' ');
            String target = message.substring(0, nameEnd);
            sendChat(ChatType.GGZ_CHAT_PERSONAL, target, message);
            chatPanel.appendCommandText("Sent private message to " + target);
        } catch (IndexOutOfBoundsException ex) {
            /* Could not parse it. */
            chatPanel.appendCommandText("Usage: /msg <username> <message>");
            chatPanel
                    .appendCommandText("    Sends a private message to a user on the network.");
        }
    }

    /*
     * Sends a chat to all players at the table.
     * 
     * @param message The text to send as a private message
     * 
     */
    private void chat_send_tablemsg(String message) throws IOException {
        try {
            // Remove /table command
            message = message.substring(commands[1][0].length()).trim();
            if (message.length() > 0) {
                sendChat(ChatType.GGZ_CHAT_TABLE, null, message);
            }
        } catch (IndexOutOfBoundsException ex) {
            /* Could not parse it. */
            chatPanel.appendCommandText("Usage: /table <message>");
            chatPanel
                    .appendCommandText("    Sends a message to the game you are currently playing.");
        }
    }

    protected abstract void chat_display_local(ChatType type, String message);

    /*
     * Displays help on all the chat commands
     * 
     */
    private void chat_help() {

        chatPanel.appendCommandText("Chat Commands");
        chatPanel.appendCommandText("-------------");

        /* This one is hard-coded at the server end. */
        chatPanel
                .appendCommandText("/me <action> .............. Send an action");

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
    private void chat_toggle_friends(String message) {
        // Remove /ignore command
        String name = message.substring(commands[5][0].length()).trim();
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
    private void chat_toggle_ignore(String message) {
        // Remove /ignore command
        String name = message.substring(commands[6][0].length()).trim();
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
    private void chat_send_beep(String message) throws IOException {
        try {
            // Remove /beep command
            String target = message.substring(commands[3][0].length())
                    .trim();
            sendChat(ChatType.GGZ_CHAT_BEEP, target, null);
            chatPanel.appendCommandText("Beep sent to " + target + ".");
        } catch (IndexOutOfBoundsException ex) {
            /* Could not parse it. */
            chatPanel.appendCommandText("Usage: /beep <username>");
            chatPanel
                    .appendCommandText("    Sends a beep to a user on the network.");
        }
    }

    /**
     * Sends a message to all rooms
     * 
     * @para message The text to send as a beep message
     * 
     * Returns:
     */
    private void chat_send_wall(String message) throws IOException {
        try {
            // Remove /wall command
            message = message.substring(commands[2][0].length()).trim();
            // Assume that spaces are not allowed in names.
            sendChat(ChatType.GGZ_CHAT_ANNOUNCE, null, message);
        } catch (IndexOutOfBoundsException ex) {
            /* Could not parse it. */
            chatPanel.appendCommandText("Usage: /wall <message>");
            chatPanel
                    .appendCommandText("    Sends a message to all users on the network.");
        }
    }

    protected abstract void sendChat(ChatType chatType, String target,
            String message) throws IOException;
    
    protected abstract ChatType getDefaultChatType();

    private void doCommand(int commandIndex, String commandString)
            throws IOException {
        switch (commandIndex) {
        case 0:
            chat_send_prvmsg(commandString);
            break;
        case 1:
            chat_send_tablemsg(commandString);
            break;
        case 2:
            chat_send_wall(commandString);
            break;
        case 3:
            chat_send_beep(commandString);
            break;
        case 5:
            chat_toggle_friends(commandString);
            break;
        case 6:
            chat_toggle_ignore(commandString);
            break;
        default:
            chat_help();
            break;
        }
    }
}
