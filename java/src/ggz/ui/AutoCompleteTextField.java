/*
 * Copyright (C) 2007  Helg Bredow
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

import ggz.client.core.Player;
import ggz.client.core.Room;

import java.awt.KeyboardFocusManager;
import java.awt.Toolkit;
import java.awt.event.KeyEvent;
import java.util.ArrayList;
import java.util.Collections;

import javax.swing.JTextField;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;
import javax.swing.text.JTextComponent;

public class AutoCompleteTextField extends JTextField {
    private Room room;

    public AutoCompleteTextField() {
        this.setFocusTraversalKeys(KeyboardFocusManager.FORWARD_TRAVERSAL_KEYS,
                Collections.EMPTY_SET);
        this.setFocusTraversalKeys(
                KeyboardFocusManager.BACKWARD_TRAVERSAL_KEYS,
                Collections.EMPTY_SET);
        this.enableEvents(KeyEvent.KEY_PRESSED);
    }

    public void setRoom(Room room) {
        this.room = room;
    }

    protected void processKeyEvent(KeyEvent event) {
        switch (event.getID()) {
        case KeyEvent.KEY_PRESSED:
            keyPressed(event);
            break;
        }
        super.processKeyEvent(event);
    }

    public void keyPressed(KeyEvent event) {
        JTextComponent c = (JTextComponent) event.getComponent();
        switch (event.getKeyCode()) {
        case KeyEvent.VK_TAB:
            try {
                Document doc = c.getDocument();
                String wordAtCaret = getWordAtCursor(c);
                Match match = findClosestMatch(wordAtCaret);
                if (match == null) {
                    Toolkit.getDefaultToolkit().beep();
                } else {
                    int len = wordAtCaret.length();
                    int offs = c.getCaretPosition() - len;
                    doc.remove(offs, len);
                    doc.insertString(c.getCaretPosition(), match.match, null);
                    if (match.isExact) {
                        // Append colon if at start of line.
                        if (offs == doc.getStartPosition().getOffset()) {
                            doc.insertString(c.getCaretPosition(), ":", null);
                        }
                        doc.insertString(c.getCaretPosition(), " ", null);
                    }
                }
                event.consume();
            } catch (BadLocationException e) {
                e.printStackTrace();
            }
            break;
        }
    }

    private static String getWordAtCursor(JTextComponent c) {
        try {
            Document doc = c.getDocument();
            int endIndex = c.getCaretPosition();
            int beginIndex = endIndex - 1;
            String textBeforeCaret = doc.getText(0, endIndex);
            for (; beginIndex >= 0; beginIndex--) {
                char ch = textBeforeCaret.charAt(beginIndex);
                if (Character.isWhitespace(ch)) {
                    break;
                }
            }
            return textBeforeCaret.substring(beginIndex + 1, endIndex);
        } catch (BadLocationException e) {
            e.printStackTrace();
            return "";
        }
    }

    private Match findClosestMatch(String s) {
        s = s.toLowerCase();
        ArrayList candidates = new ArrayList();
        // Take a copy of the player list to prevent race conditions if a player
        // leaves the room while we are iterating over the list.
        Player[] players = room.get_players();
        for (int i = 0, n = players.length; i < n; i++) {
            String name = players[i].get_name();
            if (name.toLowerCase().startsWith(s)) {
                candidates.add(name);
            }
        }

        switch (candidates.size()) {
        case 0: // No match.
            return null;
        case 1: // Only one match.
            return new Match((String) candidates.get(0), true);
        default:
            // Find common starting chars.
            String sample = (String) candidates.get(0);
            while (s.length() < sample.length()) {
                s += Character.toLowerCase(sample.charAt(s.length()));
                for (int i = 1, n = candidates.size(); i < n; i++) {
                    if (!((String) candidates.get(i)).toLowerCase().startsWith(
                            s)) {
                        return new Match(s.substring(0, s.length() - 1), false);
                    }
                }
            }
            return new Match(s, false);
        }
    }

    protected static class Match {
        public String match;

        public boolean isExact;

        public Match(String match, boolean isExact) {
            this.match = match;
            this.isExact = isExact;
        }
    }
}
