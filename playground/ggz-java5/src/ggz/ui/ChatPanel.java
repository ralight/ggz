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
import java.awt.Rectangle;
import java.util.HashSet;
import java.util.Iterator;

import javax.swing.BorderFactory;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.ScrollPaneConstants;
import javax.swing.SwingUtilities;
import javax.swing.text.AttributeSet;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.StyleConstants;

public class ChatPanel extends JPanel {
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

    // Application global ignore list.
    static private HashSet<String> ignoreList;

    // Application global friends list.
    static protected HashSet<String> friendsList;

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
        chatArea = new JTextPane();
        chatArea.setEditable(false);

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
        chatImage = new JLabel(new ImageIcon(getClass().getResource("images/chat.gif")));
        chatImage.setToolTipText("Type in the text box to the right to chat with other players.");
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
                .darker());

        infoText = new SimpleAttributeSet();
        infoText.addAttribute(StyleConstants.Foreground, Color.BLUE);
        infoText.addAttribute(StyleConstants.Bold, Boolean.TRUE);

        commandText = new SimpleAttributeSet();
        commandText.addAttribute(StyleConstants.Foreground, Color.RED);
        commandText.addAttribute(StyleConstants.Family, "Monospaced");
    }

    public void appendInfo(final String message) {
        // All handlers are called from the socket thread so we need to do
        // this crazy stuff. This method is usually invoked from a handler.
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                Document doc = chatArea.getDocument();
                try {
                    doc.insertString(doc.getLength(), message, infoText);
                    doc.insertString(doc.getLength(), "\n", null);
                } catch (BadLocationException e) {
                    e.printStackTrace();
                }
                chatArea.scrollRectToVisible(new Rectangle(0, chatArea
                        .getHeight() - 2, 1, 1));
            }
        });
    }

    public void appendCommandText(final String message) {
        // All handlers are called from the socket thread so we need to do
        // this crazy stuff. This method is usually invoked from a handler.
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                Document doc = chatArea.getDocument();
                try {
                    doc.insertString(doc.getLength(), message, commandText);
                    doc.insertString(doc.getLength(), "\n", null);
                } catch (BadLocationException e) {
                    e.printStackTrace();
                }
                chatArea.scrollRectToVisible(new Rectangle(0, chatArea
                        .getHeight() - 2, 1, 1));
                chatArea.scrollRectToVisible(new Rectangle(0, chatArea
                        .getHeight() - 2, 1, 1));
            }
        });
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
        // All handlers are called from the socket thread so we need to do
        // this crazy stuff. This method is usually invoked from a handler.
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                Document doc = chatArea.getDocument();
                String emote = null;
                AttributeSet textStyle = null;

                // Handle the /me command.
                if (message != null && message.length() >= 4
                        && message.toLowerCase().startsWith("/me ")) {
                    emote = message.substring(3);
                }

                switch (type) {
                case GGZ_CHAT_BEEP:
                    emote = " BEEPS!";
                    break;
                }

                if ("MegaGrub".equals(sender)) {
                    textStyle = senderText;
                } else if (friendsList != null
                        && friendsList.contains(sender.toLowerCase())) {
                    textStyle = friendlyText;
                }

                try {
                    if (emote == null) {
                        doc.insertString(doc.getLength(), sender + " says: ",
                                senderText);
                        doc.insertString(doc.getLength(), message, textStyle);
                    } else {
                        doc.insertString(doc.getLength(), sender + emote,
                                senderText);
                    }
                    doc.insertString(doc.getLength(), "\n", null);
                } catch (BadLocationException e) {
                    e.printStackTrace();
                }
                chatArea.scrollRectToVisible(new Rectangle(0, chatArea
                        .getHeight() - 2, 1, 1));
            }
        });
    }

    public String getMessage() {
        return textField.getText();
    }

    public void clearMessage() {
        textField.setText(null);
    }

    public void clearChat() {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                Document doc = chatArea.getDocument();
                try {
                    doc.remove(doc.getStartPosition().getOffset(), doc
                            .getLength());
                } catch (BadLocationException ex) {
                    ex.printStackTrace();
                }
            }
        });
    }

    public void toggleIgnore(String handle) {
        if (ignoreList == null) {
            ignoreList = new HashSet<String>();
        }
        // Convert to lowercase since user names are not case sensitive and we
        // can easier test for equality.
        handle = handle.toLowerCase();
        if (ignoreList.remove(handle)) {
            appendCommandText("You have removed "
                    + handle
                    + " from your ignore list. You will now see messages that he or she types.");
        } else {
            ignoreList.add(handle);
            appendCommandText("You have added "
                    + handle
                    + " to your ignore list. You will no longer see messages that he or she types.");
        }
    }

    public void appendIgnoreList() {
        if (ignoreList == null || ignoreList.size() == 0) {
            appendCommandText("You are not ignoring anyone.");
        } else {
            StringBuffer list = new StringBuffer();
            list.append("Your are ignoring the following people.");
            for (Iterator<String> iter = ignoreList.iterator(); iter.hasNext();) {
                list.append("\n    ");
                list.append(iter.next());
            }
            appendCommandText(list.toString());
        }
    }

    public void toggleFriend(String handle) {
        if (friendsList == null) {
            friendsList = new HashSet<String>();
        }
        // Convert to lowercase since user names are not case sensitive and we
        // can easier test for equality.
        handle = handle.toLowerCase();
        if (friendsList.remove(handle)) {
            appendCommandText("You have removed " + handle
                    + " from your friends list.");
        } else {
            friendsList.add(handle);
            appendCommandText("You have added "
                    + handle
                    + " to your friends list. You will now see messages that he or she types in green.");
        }
    }

    public void appendFriendsList() {
        if (friendsList == null || friendsList.size() == 0) {
            appendCommandText("You have no friends.");
        } else {
            StringBuffer list = new StringBuffer();
            list.append("You consider the following players your friends.");
            for (Iterator<String> iter = friendsList.iterator(); iter.hasNext();) {
                list.append("\n    ");
                list.append(iter.next());
            }
            appendCommandText(list.toString());
        }
    }

}
