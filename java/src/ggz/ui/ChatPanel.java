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

import ggz.client.core.Room;
import ggz.common.ChatType;
import ggz.ui.preferences.GGZPreferences;

import java.awt.BorderLayout;
import java.awt.EventQueue;
import java.awt.Toolkit;
import java.io.IOException;
import java.text.MessageFormat;
import java.util.HashSet;
import java.util.Iterator;
import java.util.ResourceBundle;
import java.util.prefs.PreferenceChangeEvent;
import java.util.prefs.PreferenceChangeListener;

import javax.swing.BorderFactory;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.ScrollPaneConstants;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;
import javax.swing.text.Style;

public class ChatPanel extends JPanel implements PreferenceChangeListener {
    private static final ResourceBundle messages = ResourceBundle
            .getBundle("ggz.ui.messages");

    private JScrollPane textScrollPane;

    private JPanel messageLayout;

    protected ChatHistoryPane chatArea;

    protected AutoCompleteTextField textField;

    private JButton sendButton;

    private JLabel chatImage;

    protected ChatAction chatAction;

    private boolean isDisposed;

    // Application global ignore list.
    static private HashSet ignoreList;

    // Application global friends list.
    static protected HashSet friendsList;

    static {
        ignoreList = (HashSet) GGZPreferences.addAll(
                GGZPreferences.IGNORE_LIST, new HashSet());
        friendsList = (HashSet) GGZPreferences.addAll(
                GGZPreferences.FRIENDS_LIST, new HashSet());
    }

    /**
     * Any code that creates an instance of this class must call dispose() so
     * that this instance can be made ready for garbage collection.
     * 
     * @param chatAction
     *            the action that sends the chat.
     */
    public ChatPanel(ChatAction chatAction) {
        super(new BorderLayout(0, 0));
        this.chatAction = chatAction;
        chatAction.setChatPanel(this);
        sendButton = new JButton(chatAction);
        textField = new AutoCompleteTextField();
        textField.setEnabled(false);
        chatArea = new ChatHistoryPane();
        GGZPreferences.addPreferenceChangeListener(this);
        GGZPreferences.applyStyle(GGZPreferences.CHAT_STYLE_NORMAL, chatArea);
        chatArea.setEditable(false);
        TextPopupMenu.enableFor(chatArea);
        TextPopupMenu.enableFor(textField);

        textScrollPane = new JScrollPane();
        textScrollPane.getViewport().add(chatArea);
        textScrollPane
                .setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
        textScrollPane.setBorder(BorderFactory.createCompoundBorder(
                BorderFactory.createEmptyBorder(4, 4, 4, 4), BorderFactory
                        .createTitledBorder((String) null)));
        textScrollPane.setOpaque(false);
        add(textScrollPane, BorderLayout.CENTER);
        messageLayout = new JPanel(new BorderLayout(4, 4));
        chatImage = new JLabel(new ImageIcon(getClass().getResource(
                "images/chat.gif")));
        chatImage.setToolTipText(messages
                .getString("ChatPanel.ToolTip.ChatImage"));
        messageLayout.add(chatImage, BorderLayout.WEST);
        messageLayout.add(textField, BorderLayout.CENTER);
        messageLayout.add(sendButton, BorderLayout.EAST);
        messageLayout.setBorder(BorderFactory.createEmptyBorder(0, 4, 4, 4));
        add(messageLayout, BorderLayout.SOUTH);
        textField.setAction(sendButton.getAction());
        textField.setBorder(BorderFactory.createTitledBorder((String) null));
        textField.setFont(chatArea.getFont());
        textField.setForeground(chatArea.getForeground());
        textField.setBackground(chatArea.getBackground());
        setOpaque(false);
        sendButton.setOpaque(false);
        messageLayout.setOpaque(false);
    }

    /**
     * Stops listening for preference change events so that this instance can be
     * garbage collected.
     */
    public void dispose() {
        // Prevent "memory leaks". Anyone who creates us should also dispose()
        // us.
        if (!isDisposed) {
            GGZPreferences.removePreferenceChangeListener(this);
            this.chatArea.dispose();
            isDisposed = true;
        }
    }

    public void setRoom(Room room) {
        textField.setRoom(room);
    }

    public void appendInfo(final String message) {
        assertOnEventDispatchThread();
        ChatDocument doc = chatArea.getChatDocument();
        chatArea.checkAutoScroll();
        try {
            doc.insertString(doc.getLength(), message,
                    GGZPreferences.CHAT_STYLE_INFO, false);
            doc.insertString(doc.getLength(), "\n", (Style) null);
        } catch (BadLocationException e) {
            e.printStackTrace();
        }
    }

    public void appendCommandText(final String message) {
        assertOnEventDispatchThread();
        // All handlers are called from the socket thread so we need to do
        // this crazy stuff. This method is usually invoked from a handler.
        ChatDocument doc = chatArea.getChatDocument();
        chatArea.checkAutoScroll();
        try {
            doc.insertString(doc.getLength(), message,
                    GGZPreferences.CHAT_STYLE_COMMAND, false);
            doc.insertString(doc.getLength(), "\n", (Style) null);
        } catch (BadLocationException e) {
            e.printStackTrace();
        }
    }

    public void appendChat(String sender, String message, String me) {
        appendChat(ChatType.GGZ_CHAT_NORMAL, sender, message, me);
    }

    public void appendChat(ChatType type, String sender, String message,
            String me) {
        if (isIgnored(sender)) {
            // We are ignoring this person.
            return;
        }
        ChatDocument doc = chatArea.getChatDocument();
        String emote = null;
        String textStyle = null;

        assertOnEventDispatchThread();

        // Handle the /me command and other emotes.
        if (message != null && message.length() >= 4
                && message.toLowerCase().startsWith("/me ")) {
            emote = "{0}" + message.substring(3);
        } else if (type == ChatType.GGZ_CHAT_BEEP) {
            emote = messages.getString("ChatPanel.Beep");
            Toolkit.getDefaultToolkit().beep();
        }

        if ("MegaGrub".equals(sender)) {
            textStyle = GGZPreferences.CHAT_STYLE_SENDER;
        } else if (type == ChatType.GGZ_CHAT_ANNOUNCE) {
            textStyle = GGZPreferences.CHAT_STYLE_ANNOUNCE;
            // TODO change the text to <nic> announces:
        } else if (sender.equals(me)) {
            textStyle = GGZPreferences.CHAT_STYLE_ME;
        } else if (isFriend(sender)) {
            textStyle = GGZPreferences.CHAT_STYLE_FRIEND;
        }

        chatArea.checkAutoScroll();
        try {
            if (emote == null) {
                doc.insertString(doc.getLength(), MessageFormat.format(messages
                        .getString("ChatPanel.Says"), new Object[] { sender }),
                        GGZPreferences.CHAT_STYLE_SENDER, false);
                doc.insertString(doc.getLength(), message, textStyle, true);
            } else {
                doc.insertString(doc.getLength(), MessageFormat.format(emote,
                        new Object[] { sender }),
                        GGZPreferences.CHAT_STYLE_SENDER, true);
            }
            doc.insertString(doc.getLength(), "\n", (Style) null);
        } catch (BadLocationException e) {
            e.printStackTrace();
        }
    }

    public String getMessage() {
        return textField.getText();
    }

    public void clearMessage() {
        textField.setText(null);
    }

    public void clearChat() {
        assertOnEventDispatchThread();
        Document doc = chatArea.getDocument();
        try {
            doc.remove(doc.getStartPosition().getOffset(), doc.getLength());
        } catch (BadLocationException ex) {
            ex.printStackTrace();
        }
    }

    public void toggleIgnore(String handle) {
        if (ignoreList == null) {
            ignoreList = new HashSet();
        }
        // Convert to lowercase since user names are not case sensitive and we
        // can easier test for equality.
        String lowerCaseHandle = handle.toLowerCase();
        if (ignoreList.remove(lowerCaseHandle)) {
            appendCommandText(MessageFormat.format(messages
                    .getString("ChatPanel.Message.RemovedFromIngoreList"),
                    new Object[] { handle }));
        } else {
            ignoreList.add(lowerCaseHandle);
            appendCommandText(MessageFormat.format(messages
                    .getString("ChatPanel.Message.AddedToIgnoreList"),
                    new Object[] { handle }));
        }
        GGZPreferences.putStringCollection(GGZPreferences.IGNORE_LIST,
                ignoreList);
    }

    public static boolean isIgnored(String handle) {
        if (ignoreList == null) {
            return false;
        }
        // Convert to lowercase since user names are not case sensitive and we
        // can easier test for equality.
        handle = handle.toLowerCase();
        return ignoreList.contains(handle);
    }

    public void appendIgnoreList() {
        if (ignoreList == null || ignoreList.size() == 0) {
            appendCommandText(messages
                    .getString("ChatPanel.Message.IgnoreListIsEmpty"));
        } else {
            StringBuffer list = new StringBuffer();
            list.append(messages
                    .getString("ChatPanel.Message.IgnoreListHeader"));
            for (Iterator iter = ignoreList.iterator(); iter.hasNext();) {
                list.append("\n    ");
                list.append(iter.next());
            }
            appendCommandText(list.toString());
        }
    }

    public void sendBeep(String handle) throws IOException {
        chatAction.sendBeep(handle);
    }

    public void toggleFriend(String handle) {
        if (friendsList == null) {
            friendsList = new HashSet();
        }
        // Convert to lowercase since user names are not case sensitive and we
        // can easier test for equality.
        String lowerCaseHandle = handle.toLowerCase();
        if (friendsList.remove(lowerCaseHandle)) {
            appendCommandText(MessageFormat.format(messages
                    .getString("ChatPanel.Message.RemovedFromFriendsList"),
                    new Object[] { handle }));
        } else {
            friendsList.add(lowerCaseHandle);
            appendCommandText(MessageFormat.format(messages
                    .getString("ChatPanel.Message.AddedToFriendsList"),
                    new Object[] { handle }));
        }
        GGZPreferences.putStringCollection(GGZPreferences.FRIENDS_LIST,
                friendsList);
    }

    public static boolean isFriend(String handle) {
        if (friendsList == null) {
            return false;
        }
        // Convert to lowercase since user names are not case sensitive and we
        // can easier test for equality.
        handle = handle.toLowerCase();
        return friendsList.contains(handle);
    }

    public void appendFriendsList() {
        if (friendsList == null || friendsList.size() == 0) {
            appendCommandText(messages
                    .getString("ChatPanel.Message.FriendsListIsEmpty"));
        } else {
            StringBuffer list = new StringBuffer();
            list.append(messages
                    .getString("ChatPanel.Message.FriendsListHeader"));
            for (Iterator iter = friendsList.iterator(); iter.hasNext();) {
                list.append("\n    ");
                list.append(iter.next());
            }
            appendCommandText(list.toString());
        }
    }

    private void assertOnEventDispatchThread() {
        if (!EventQueue.isDispatchThread()) {
            throw new IllegalStateException(
                    "This method must be called using SwingUtilities.invokeLater() or SwingUtilities.invokeAndWait().");
        }
    }

    public void preferenceChange(PreferenceChangeEvent evt) {
        if (GGZPreferences.CHAT_STYLE_NORMAL.equals(evt.getKey())) {
            GGZPreferences.applyStyle(evt.getKey(), textField);
            revalidate();
        }
    }
}
