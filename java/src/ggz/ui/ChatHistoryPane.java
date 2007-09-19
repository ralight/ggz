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

import ggz.ui.preferences.GGZPreferences;

import java.awt.Color;
import java.awt.Font;
import java.awt.Rectangle;
import java.util.prefs.PreferenceChangeEvent;
import java.util.prefs.PreferenceChangeListener;

import javax.swing.JEditorPane;
import javax.swing.JScrollBar;
import javax.swing.JScrollPane;
import javax.swing.SwingUtilities;
import javax.swing.ToolTipManager;
import javax.swing.text.Style;
import javax.swing.text.StyleConstants;
import javax.swing.text.html.HTMLEditorKit;
import javax.swing.text.html.StyleSheet;

public class ChatHistoryPane extends JEditorPane implements
        PreferenceChangeListener {

    protected boolean isAutoScroll = true;

    private boolean isDisposed = false;

    public ChatHistoryPane() {
        HTMLEditorKit htmlkit = new HTMLEditorKit();
        StyleSheet styles = htmlkit.getStyleSheet();
        StyleSheet ss = new StyleSheet();

        ss.addStyleSheet(styles);
        initialiseStyles(ss);
        this.setEditorKit(htmlkit);
        this.setDocument(new ChatDocument(ss));
        if (GlobalHyperlinkListener.get() != null) {
            this.addHyperlinkListener(GlobalHyperlinkListener.get());
        }

        GGZPreferences.addPreferenceChangeListener(this);
        ToolTipManager.sharedInstance().registerComponent(this);
    }

    private static void initialiseStyles(StyleSheet ss) {
        ss.addRule("p { margin: 0 }");
        ss.addRule("img { position: relative; top: 2px }");

        String[] styleNames = new String[] {
                GGZPreferences.CHAT_STYLE_ANNOUNCE,
                GGZPreferences.CHAT_STYLE_COMMAND,
                GGZPreferences.CHAT_STYLE_FRIEND,
                GGZPreferences.CHAT_STYLE_INFO, GGZPreferences.CHAT_STYLE_ME,
                GGZPreferences.CHAT_STYLE_SENDER };

        for (int i = 0, n = styleNames.length; i < n; i++) {
            String key = styleNames[i];
            String className = toCSSClassName(key);
            ss.addRule(className + " {}");
            Style style = ss.getStyle(className);
            GGZPreferences.copyAttributes(key, style);
        }
    }

    public ChatDocument getChatDocument() {
        return (ChatDocument) this.getDocument();
    }

    public void scrollRectToVisible(Rectangle rect) {
        // This is only needed when using setCaretPosition() to scroll
        // the chat.
        if (this.isAutoScroll) {
            super.scrollRectToVisible(rect);
        }
    }

    protected void checkAutoScroll() {
        JScrollPane scrollPane = (JScrollPane) SwingUtilities
                .getAncestorOfClass(JScrollPane.class, this);

        if (scrollPane == null)
            return;

        JScrollBar vbar = scrollPane.getVerticalScrollBar();
        this.isAutoScroll = vbar.getMaximum()
                - (vbar.getValue() + vbar.getVisibleAmount()) < 40;
        if (this.isAutoScroll) {
            this.setCaretPosition(this.getDocument().getLength());
        }
    }

    protected Style getBodyStyle() {
        ChatDocument doc = (ChatDocument) this.getDocument();

        if (doc == null)
            return null;

        StyleSheet styleSheet = doc.getStyleSheet();
        Style body = doc.getStyle("body");
        if (body == null) {
            styleSheet.addRule("body {}");
            body = doc.getStyle("body");
        }
        return body;
    }

    public void setFont(Font font) {
        super.setFont(font);
        Style body = getBodyStyle();
        if (body == null)
            return;
        StyleConstants.setFontFamily(body, font.getFontName());
        StyleConstants.setFontSize(body, font.getSize());
        StyleConstants.setBold(body, font.isBold());
        StyleConstants.setItalic(body, font.isItalic());
    }

    public void setForeground(Color fg) {
        super.setForeground(fg);
        Style body = getBodyStyle();
        if (body == null)
            return;
        StyleConstants.setForeground(body, fg);
    }

    public void setBackground(Color bg) {
        super.setBackground(bg);
        Style body = getBodyStyle();
        if (body == null)
            return;
        StyleConstants.setBackground(body, bg);
    }

    public void preferenceChange(PreferenceChangeEvent evt) {
        if (GGZPreferences.CHAT_STYLE_NORMAL.equals(evt.getKey())) {
            GGZPreferences.applyStyle(evt.getKey(), this);
        } else if (GGZPreferences.isChatStyle(evt.getKey())) {
            String className = toCSSClassName(evt.getKey());
            Style commandStyle = this.getChatDocument().getStyle(className);
            GGZPreferences.copyAttributes(evt.getKey(), commandStyle);
        }
    }

    protected static String toCSSClassName(String key) {
        if (key == null)
            return null;
        return "." + key.toLowerCase();
    }

    public void dispose() {
        if (!isDisposed) {
            GGZPreferences.removePreferenceChangeListener(this);
            ToolTipManager.sharedInstance().unregisterComponent(this);
        }
    }

}
