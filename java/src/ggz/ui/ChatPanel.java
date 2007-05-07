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
import ggz.ui.preferences.GGZPreferences;

import java.awt.BorderLayout;
import java.awt.EventQueue;
import java.awt.Rectangle;
import java.awt.Toolkit;
import java.io.IOException;
import java.text.MessageFormat;
import java.util.HashSet;
import java.util.Iterator;
import java.util.ResourceBundle;
import java.util.StringTokenizer;
import java.util.prefs.PreferenceChangeEvent;
import java.util.prefs.PreferenceChangeListener;

import javax.swing.BorderFactory;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollBar;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.ScrollPaneConstants;
import javax.swing.text.AttributeSet;
import javax.swing.text.BadLocationException;
import javax.swing.text.DefaultStyledDocument;
import javax.swing.text.Document;
import javax.swing.text.Style;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyleContext;
import javax.swing.text.StyledDocument;

public class ChatPanel extends JPanel implements PreferenceChangeListener {
    private static final ResourceBundle messages = ResourceBundle
            .getBundle("ggz.ui.messages");

    private JScrollPane textScrollPane;

    private JPanel messageLayout;

    protected JTextPane chatArea;

    protected JTextField textField;

    private JButton sendButton;

    private JLabel chatImage;

    protected ChatAction chatAction;

    protected boolean isAutoScroll = true;

    private boolean isDisposed;

    // Application global ignore list.
    static private HashSet ignoreList;

    // Application global friends list.
    static protected HashSet friendsList;

    static protected StyleContext emoticons;

    static {
        ignoreList = (HashSet) GGZPreferences.addAll(
                GGZPreferences.IGNORE_LIST, new HashSet());
        friendsList = (HashSet) GGZPreferences.addAll(
                GGZPreferences.FRIENDS_LIST, new HashSet());

        // Initialise icons for emoticons.
        emoticons = new StyleContext();
        Style emoticon;

        emoticon = emoticons.addStyle("emoticon-smile", null);
        StyleConstants.setIcon(emoticon, new ImageIcon(ChatPanel.class
                .getResource("/ggz/ui/images/emoticon_smile.png")));

        emoticon = emoticons.addStyle("emoticon-unhappy", null);
        StyleConstants.setIcon(emoticon, new ImageIcon(ChatPanel.class
                .getResource("/ggz/ui/images/emoticon_unhappy.png")));

        emoticon = emoticons.addStyle("emoticon-tongue", null);
        StyleConstants.setIcon(emoticon, new ImageIcon(ChatPanel.class
                .getResource("/ggz/ui/images/emoticon_tongue.png")));

        emoticon = emoticons.addStyle("emoticon-wink", null);
        StyleConstants.setIcon(emoticon, new ImageIcon(ChatPanel.class
                .getResource("/ggz/ui/images/emoticon_wink.png")));

        emoticon = emoticons.addStyle("emoticon-evilgrin", null);
        StyleConstants.setIcon(emoticon, new ImageIcon(ChatPanel.class
                .getResource("/ggz/ui/images/emoticon_evilgrin.png")));

        emoticon = emoticons.addStyle("emoticon-grin", null);
        StyleConstants.setIcon(emoticon, new ImageIcon(ChatPanel.class
                .getResource("/ggz/ui/images/emoticon_grin.png")));

        emoticon = emoticons.addStyle("emoticon-surprised", null);
        StyleConstants.setIcon(emoticon, new ImageIcon(ChatPanel.class
                .getResource("/ggz/ui/images/emoticon_surprised.png")));

        emoticon = emoticons.addStyle("emoticon-kiss", null);
        StyleConstants.setIcon(emoticon, new ImageIcon(ChatPanel.class
                .getResource("/ggz/ui/images/emoticon_kiss.png")));

        emoticon = emoticons.addStyle("emoticon-star", null);
        StyleConstants.setIcon(emoticon, new ImageIcon(ChatPanel.class
                .getResource("/ggz/ui/images/emoticon_star.png")));

        emoticon = emoticons.addStyle("emoticon-rainbow", null);
        StyleConstants.setIcon(emoticon, new ImageIcon(ChatPanel.class
                .getResource("/ggz/ui/images/emoticon_rainbow.png")));

        emoticon = emoticons.addStyle("emoticon-lightbulb", null);
        StyleConstants.setIcon(emoticon, new ImageIcon(ChatPanel.class
                .getResource("/ggz/ui/images/emoticon_lightbulb.png")));

        emoticon = emoticons.addStyle("emoticon-bomb", null);
        StyleConstants.setIcon(emoticon, new ImageIcon(ChatPanel.class
                .getResource("/ggz/ui/images/emoticon_bomb.png")));

        emoticon = emoticons.addStyle("emoticon-flag", null);
        StyleConstants.setIcon(emoticon, new ImageIcon(ChatPanel.class
                .getResource("/ggz/ui/images/emoticon_flag.png")));

        emoticon = emoticons.addStyle("emoticon-cup", null);
        StyleConstants.setIcon(emoticon, new ImageIcon(ChatPanel.class
                .getResource("/ggz/ui/images/emoticon_cup.png")));

        emoticon = emoticons.addStyle("emoticon-drink", null);
        StyleConstants.setIcon(emoticon, new ImageIcon(ChatPanel.class
                .getResource("/ggz/ui/images/emoticon_drink.png")));

        emoticon = emoticons.addStyle("emoticon-cake", null);
        StyleConstants.setIcon(emoticon, new ImageIcon(ChatPanel.class
                .getResource("/ggz/ui/images/player_host.png")));

        emoticon = emoticons.addStyle("emoticon-rosette", null);
        StyleConstants.setIcon(emoticon, new ImageIcon(ChatPanel.class
                .getResource("/ggz/ui/images/emoticon_rosette.png")));
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
        textField = new JTextField();
        textField.setEnabled(false);
        chatArea = new JTextPane(new DefaultStyledDocument(emoticons)) {
            public void scrollRectToVisible(Rectangle rect) {
                // This is only needed when using setCaretPosition() to scroll
                // the chat.
                if (isAutoScroll) {
                    super.scrollRectToVisible(rect);
                }
            }
        };
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
        add(messageLayout, BorderLayout.SOUTH);
        textField.setAction(sendButton.getAction());
        textField.setBorder(BorderFactory.createTitledBorder((String) null));
        textField.setFont(chatArea.getFont());
        textField.setForeground(chatArea.getForeground());
        textField.setBackground(chatArea.getBackground());
        messageLayout.setBorder(BorderFactory.createEmptyBorder(0, 4, 4, 4));

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
            // This is needed to prevent memory leaks due to us using our own
            // static StyleContext, it might be a bug in JDK1.4.
            this.chatArea.setDocument(new DefaultStyledDocument());
            isDisposed = true;
        }
    }

    public void appendInfo(final String message) {
        assertOnEventDispatchThread();
        StyledDocument doc = chatArea.getStyledDocument();
        checkAutoScroll();
        try {
            doc.insertString(doc.getLength(), message, GGZPreferences
                    .getChatStyleInfo());
            doc.insertString(doc.getLength(), "\n", null);
        } catch (BadLocationException e) {
            e.printStackTrace();
        }
    }

    public void appendCommandText(final String message) {
        assertOnEventDispatchThread();
        // All handlers are called from the socket thread so we need to do
        // this crazy stuff. This method is usually invoked from a handler.
        StyledDocument doc = chatArea.getStyledDocument();
        checkAutoScroll();
        try {
            doc.insertString(doc.getLength(), message, GGZPreferences
                    .getChatStyleCommand());
            doc.insertString(doc.getLength(), "\n", null);
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
        StyledDocument doc = chatArea.getStyledDocument();
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

        if ("MegaGrub".equals(sender)) {
            textStyle = GGZPreferences.getChatStyleSender();
        } else if (type == ChatType.GGZ_CHAT_ANNOUNCE) {
            textStyle = GGZPreferences.getChatStyleAnnounce();
            // TODO change the text to <nic> announces:
        } else if (sender.equals(me)) {
            textStyle = GGZPreferences.getChatStyleMe();
        } else if (isFriend(sender)) {
            textStyle = GGZPreferences.getChatStyleFriend();
        }

        doc.getStyle("");
        checkAutoScroll();
        try {
            if (emote == null) {
                doc.insertString(doc.getLength(), MessageFormat.format(messages
                        .getString("ChatPanel.Says"), new Object[] { sender }),
                        GGZPreferences.getChatStyleSender());
                insertWithEmotes(doc, message, textStyle);
            } else {
                insertWithEmotes(doc, MessageFormat.format(emote,
                        new Object[] { sender }), GGZPreferences
                        .getChatStyleSender());
            }
            doc.insertString(doc.getLength(), "\n", null);
        } catch (BadLocationException e) {
            e.printStackTrace();
        }
    }

    protected void insertWithEmotes(StyledDocument doc, String text,
            AttributeSet style) throws BadLocationException {
        // Look for the start of a smiley or special "emote".
        StringTokenizer parser = new StringTokenizer(text, ":;(", true);
        if (!parser.hasMoreElements())
            return;

        String tok = parser.nextToken();

        do {
            Style emoticon = null;
            String nextTok = null;
            if (parser.hasMoreTokens()) {
                nextTok = parser.nextToken();
                char ch = nextTok.charAt(0);
                if (":".equals(tok)) {
                    // Possible emoticon.
                    switch (ch) {
                    case ')':
                        // Smile
                        emoticon = doc.getStyle("emoticon-smile");
                        tok += ch;
                        break;
                    case '(':
                        // Unhappy.
                        emoticon = doc.getStyle("emoticon-unhappy");
                        tok += ch;
                        break;
                    case 'D':
                        // Grin.
                        emoticon = doc.getStyle("emoticon-grin");
                        tok += ch;
                        break;
                    case '@':
                        // Evil Grin.
                        emoticon = doc.getStyle("emoticon-evilgrin");
                        tok += ch;
                        break;
                    case 'p':
                    case 'P':
                        // Tongue.
                        emoticon = doc.getStyle("emoticon-tongue");
                        tok += ch;
                        break;
                    case 'O': // Surprised.
                        emoticon = doc.getStyle("emoticon-surprised");
                        tok += ch;
                        break;
                    case 'x':
                    case 'X':
                        // Kiss.
                        emoticon = doc.getStyle("emoticon-kiss");
                        tok += ch;
                        break;
                    }
                } else if (";".equals(tok)) {
                    switch (ch) {
                    case ')':
                        // Wink.
                        emoticon = doc.getStyle("emoticon-wink");
                        tok += ch;
                        break;
                    }
                } else if ("(".equals(tok)) {
                    if (nextTok.startsWith("*)")) {
                        emoticon = doc.getStyle("emoticon-star");
                        tok = "(*)";
					} else if (nextTok.startsWith("R)") || nextTok.startsWith("r)")) {
						emoticon = doc.getStyle("emoticon-rainbow");
						tok = "(R)";
					} else if (nextTok.startsWith("I)") || nextTok.startsWith("i)")) {
						emoticon = doc.getStyle("emoticon-lightbulb");
						tok = "(I)";
					} else if (nextTok.startsWith("!)")) {
						emoticon = doc.getStyle("emoticon-bomb");
						tok = "(!)";
					} else if (nextTok.startsWith("/)")) {
						emoticon = doc.getStyle("emoticon-flag");
						tok = "(/)";
					} else if (nextTok.startsWith("C)")) {
						emoticon = doc.getStyle("emoticon-cup");
						tok = "(C)";
					} else if (nextTok.startsWith("D)")) {
						emoticon = doc.getStyle("emoticon-drink");
						tok = "(D)";
					} else if (nextTok.startsWith("^)")) {
						emoticon = doc.getStyle("emoticon-cake");
						tok = "(^)";
					} else if (nextTok.startsWith("%)")) {
						emoticon = doc.getStyle("emoticon-rosette");
						tok = "(%)";
                    }/* else if (nextTok.startsWith("my emote)")) {
                        emoticon = doc.getStyle("my emote");
                        tok = "(my emote)";
                    }*/
                }
            }

            if (emoticon == null) {
                doc.insertString(doc.getLength(), tok, style);
                if (nextTok != null) {
                    tok = nextTok;
                } else if (parser.hasMoreTokens()) {
                    tok = parser.nextToken();
                } else {
                    tok = null;
                }
            } else {
                doc.insertString(doc.getLength(), tok, emoticon);
                if (nextTok != null && nextTok.length() > tok.length() - 1) {
                    tok = nextTok.substring(tok.length() - 1);
                } else if (parser.hasMoreTokens()) {
                    tok = parser.nextToken();
                } else {
                    tok = null;
                }
            }
        } while (tok != null);
    }

    protected void checkAutoScroll() {
        JScrollBar vbar = textScrollPane.getVerticalScrollBar();
        isAutoScroll = vbar.getMaximum()
                - (vbar.getValue() + vbar.getVisibleAmount()) < 40;
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
            GGZPreferences.applyStyle(evt.getKey(), chatArea);
            GGZPreferences.applyStyle(evt.getKey(), textField);
            revalidate();
        }
    }
}
