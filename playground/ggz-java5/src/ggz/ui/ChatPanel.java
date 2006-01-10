package ggz.ui;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Rectangle;

import javax.swing.Action;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.ScrollPaneConstants;
import javax.swing.SwingUtilities;
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

    /**
     * @param chatAction
     *            the action that sends the chat.
     */
    public ChatPanel(Action chatAction) {
        super(new BorderLayout(0, 0));
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
        messageLayout.add(textField, BorderLayout.CENTER);
        messageLayout.add(sendButton, BorderLayout.EAST);
        add(messageLayout, BorderLayout.SOUTH);
        textField.setAction(sendButton.getAction());
        textField.setBorder(BorderFactory.createTitledBorder((String) null));
        messageLayout.setBorder(BorderFactory.createEmptyBorder(0, 4, 4, 4));

        setOpaque(false);
        sendButton.setOpaque(false);
        messageLayout.setOpaque(false);
    }

    public void handle_chat(final String sender, final String message) {
        // All handlers are called from the socket thread so we need to do
        // this crazy stuff. This method is usually invoked from a handler.
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                Document doc = chatArea.getDocument();
                SimpleAttributeSet blueText = new SimpleAttributeSet();
                blueText.addAttribute(StyleConstants.Foreground, Color.BLUE);

                try {
                    // doc.insertString(doc.getLength(), "[" + sender + "] ", blueText);
                    doc.insertString(doc.getLength(), sender + " says: ", blueText);
                    doc.insertString(doc.getLength(), message, "MegaGrub"
                            .equals(sender) ? blueText : null);
                    doc.insertString(doc.getLength(), "\n", null);
                } catch (BadLocationException e) {
                    e.printStackTrace();
                }
                chatArea.scrollRectToVisible(new Rectangle(0, chatArea.getHeight() - 2,
                        1, 1));
            }
        });
    }

    public String getMessage() {
        return textField.getText();
    }

    public void clearMessage() {
        textField.setText(null);
    }
}
