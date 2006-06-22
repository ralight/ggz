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

import ggz.common.ChatType;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.EventQueue;
import java.awt.Rectangle;
import java.awt.Toolkit;
import java.text.MessageFormat;
import java.util.HashSet;
import java.util.Iterator;
import java.util.ResourceBundle;

import javax.swing.BorderFactory;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.ScrollPaneConstants;
import javax.swing.text.AttributeSet;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.StyleConstants;

public class ChatPanel extends JPanel {
    private static final ResourceBundle messages = ResourceBundle
            .getBundle("ggz.ui.messages");

    private JScrollPane textScrollPane;

    private JPanel messageLayout;

    protected JTextPane chatArea;

    private JTextField textField;

    private JButton sendButton;

    private JLabel chatImage;

    protected SimpleAttributeSet senderText;

    protected SimpleAttributeSet friendlyText;

    protected SimpleAttributeSet infoText;

    protected SimpleAttributeSet commandText;

    protected SimpleAttributeSet announceText;

    protected boolean isAutoScroll = true;

    // Application global ignore list.
    static private HashSet ignoreList;

    // Application global friends list.
    static protected HashSet friendsList;

    /**
     * @param chatAction
     *            the action that sends the chat.
     */
    public ChatPanel(ChatAction chatAction) {
        super(new BorderLayout(0, 0));
        chatAction.setChatPanel(this);
        sendButton = new JButton(chatAction);
        textField = new JTextField();
        textField.setEnabled(false);
        chatArea = new JTextPane() {
            public void scrollRectToVisible(Rectangle rect) {
//                System.out.println("isAutoScroll="+isAutoScroll);
                if (isAutoScroll) {
                    super.scrollRectToVisible(rect);
                }
            }
        };
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
        add(messageLayout, BorderLayout.SOUTH);
        textField.setAction(sendButton.getAction());
        textField.setBorder(BorderFactory.createTitledBorder((String) null));
        messageLayout.setBorder(BorderFactory.createEmptyBorder(0, 4, 4, 4));

        setOpaque(false);
        sendButton.setOpaque(false);
        messageLayout.setOpaque(false);

        senderText = new SimpleAttributeSet();
        senderText.addAttribute(StyleConstants.Foreground, Color.BLUE);

        friendlyText = new SimpleAttributeSet();
        friendlyText.addAttribute(StyleConstants.Foreground, Color.GREEN
                .darker().darker());

        infoText = new SimpleAttributeSet();
        infoText.addAttribute(StyleConstants.Foreground, Color.MAGENTA);
        infoText.addAttribute(StyleConstants.Bold, Boolean.TRUE);

        commandText = new SimpleAttributeSet();
        commandText.addAttribute(StyleConstants.Foreground, Color.RED);
        commandText.addAttribute(StyleConstants.FontFamily, "Monospaced");

        announceText = new SimpleAttributeSet();
        announceText.addAttribute(StyleConstants.Foreground, Color.RED);
        announceText.addAttribute(StyleConstants.Bold, Boolean.TRUE);
    }

    public void appendInfo(final String message) {
        assertOnEventDispatchThread();
        Document doc = chatArea.getDocument();
        checkAutoScroll();
        try {
            doc.insertString(doc.getLength(), message, infoText);
            doc.insertString(doc.getLength(), "\n", null);
        } catch (BadLocationException e) {
            e.printStackTrace();
        }
    }

    public void appendCommandText(final String message) {
        assertOnEventDispatchThread();
        // All handlers are called from the socket thread so we need to do
        // this crazy stuff. This method is usually invoked from a handler.
        Document doc = chatArea.getDocument();
        checkAutoScroll();
        try {
            doc.insertString(doc.getLength(), message, commandText);
            doc.insertString(doc.getLength(), "\n", null);
        } catch (BadLocationException e) {
            e.printStackTrace();
        }
    }

    public void appendChat(final String sender, final String message) {
        appendChat(ChatType.GGZ_CHAT_NORMAL, sender, message);
    }

    public void appendChat(final ChatType type, final String sender,
            final String message) {
        if (ignoreList != null && ignoreList.contains(sender)) {
            // We are ignoring this person.
            return;
        }
        Document doc = chatArea.getDocument();
        String emote = null;
        AttributeSet textStyle = null;

        assertOnEventDispatchThread();

        // Handle the /me command and other emotes.
        if (message != null && message.length() >= 4
                && message.toLowerCase().startsWith("/me ")) {
            emote = "{0}" + message.substring(3);
        } else if (type == ChatType.GGZ_CHAT_BEEP) {
            emote = messages.getString("ChatPanel.Beep");
            Toolkit.getDefaultToolkit().beep();
        }

        if (type == ChatType.GGZ_CHAT_PERSONAL) {
            // TODO Open a private chat window instead.
            appendCommandText("You have received a private message. (below)");
        }

        if ("MegaGrub".equals(sender)) {
            textStyle = senderText;
        } else if (friendsList != null
                && friendsList.contains(sender.toLowerCase())) {
            textStyle = friendlyText;
        } else if (type == ChatType.GGZ_CHAT_ANNOUNCE) {
            textStyle = announceText;
            // TODO change the text to <nic> announces:
        }

        checkAutoScroll();
        try {
            if (emote == null) {
                doc.insertString(doc.getLength(), MessageFormat.format(messages
                        .getString("ChatPanel.Says"), new Object[] { sender }),
                        senderText);
                doc.insertString(doc.getLength(), message, textStyle);
            } else {
                doc.insertString(doc.getLength(), MessageFormat.format(emote,
                        new Object[] { sender }), senderText);
            }
            doc.insertString(doc.getLength(), "\n", null);
        } catch (BadLocationException e) {
            e.printStackTrace();
        }
    }

    protected void checkAutoScroll() {
        Rectangle visibleRect = chatArea.getVisibleRect();
        int pos = textScrollPane.getVerticalScrollBar().getValue();
        int max = textScrollPane.getVerticalScrollBar().getMaximum();
//        System.out.println("pos=" + pos + " max=" + max + " visibleRect.height=" + visibleRect.height);
        isAutoScroll = (pos >= (max - visibleRect.height) - 20);
        if (isAutoScroll) {
            chatArea.setCaretPosition(chatArea.getDocument().getLength());
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
        handle = handle.toLowerCase();
        if (ignoreList.remove(handle)) {
            appendCommandText(MessageFormat.format(messages
                    .getString("ChatPanel.Message.RemovedFromIngoreList"),
                    new Object[] { handle }));
        } else {
            ignoreList.add(handle);
            appendCommandText(MessageFormat.format(messages
                    .getString("ChatPanel.Message.AddedToIgnoreList"),
                    new Object[] { handle }));
        }
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

    public void toggleFriend(String handle) {
        if (friendsList == null) {
            friendsList = new HashSet();
        }
        // Convert to lowercase since user names are not case sensitive and we
        // can easier test for equality.
        handle = handle.toLowerCase();
        if (friendsList.remove(handle)) {
            appendCommandText(MessageFormat.format(messages
                    .getString("ChatPanel.Message.RemovedFromFriendsList"),
                    new Object[] { handle }));
        } else {
            friendsList.add(handle);
            appendCommandText(MessageFormat.format(messages
                    .getString("ChatPanel.Message.AddedToFriendsList"),
                    new Object[] { handle }));
        }
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
}
