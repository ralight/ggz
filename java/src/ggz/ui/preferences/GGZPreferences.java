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
package ggz.ui.preferences;

import ggz.common.CSVParser;

import java.awt.Color;
import java.awt.Component;
import java.awt.Font;
import java.util.Arrays;
import java.util.Collection;
import java.util.Iterator;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.prefs.PreferenceChangeListener;
import java.util.prefs.Preferences;

import javax.swing.JTextArea;
import javax.swing.text.AttributeSet;
import javax.swing.text.Style;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyleContext;

public class GGZPreferences {

    public static String CHAT_STYLE_NORMAL = "Chat.Style.Normal";

    public static String CHAT_STYLE_ANNOUNCE = "Chat.Style.Announce";

    public static String CHAT_STYLE_SENDER = "Chat.Style.Sender";

    public static String CHAT_STYLE_ME = "Chat.Style.Me";

    public static String CHAT_STYLE_INFO = "Chat.Style.Info";

    public static String CHAT_STYLE_COMMAND = "Chat.Style.Command";

    public static String CHAT_STYLE_FRIEND = "Chat.Style.Friend";

    public static String IGNORE_LIST = "Chat.Ignore.List";

    public static String FRIENDS_LIST = "Chat.Friends.List";

    public static String LAST_LOGIN_NICKNAME = "Login.Nickname";

    public static String LAST_LOGIN_TYPE = "Login.Type";

    public static final Logger log = Logger.getLogger(GGZPreferences.class
            .getName());

    private static Preferences prefs;

    static protected StyleContext styles;

    static {
        try {
            prefs = Preferences.userNodeForPackage(GGZPreferences.class);
        } catch (SecurityException e) {
            // Running under a security manager so just use propterties.
            prefs = new PropertiesPreferences();
        }

        // Set up styles.
        styles = new StyleContext();
    }

    public static Font getFont(String key, Font defaultValue) {
        Font value = getFont(key, (String) null);
        return value == null ? defaultValue : value;
    }

    public static Font getFont(String key, String defaultValue) {
        String value = get(key, defaultValue);
        return value == null ? null : Font.decode(value);
    }

    public static void putFont(String key, Font font) {
        String fontString = encodeFont(font);
        log.fine("Saving font preference " + key + ": " + fontString);
        put(key, fontString);
    }

    public static Color getColor(String key, Color defaultValue) {
        String colorString = get(key, null);
        // if (defaultValue == null) {
        // defaultValue = (Color) defaults.get(key);
        // }
        return colorString == null ? defaultValue : Color.decode(colorString);
    }

    public static void putColor(String key, Color color) {
        String colorString = encodeColor(color);
        log.fine("Saving color preference " + key + ": " + colorString);
        put(key, colorString);
    }

    public static Style getStyle(String key, String fontFamily, int fontStyle,
            Color foreground) {
        // Get the style context to manage this style for us for
        // performance.
        Style style = styles.getStyle(key);
        String styleString = get(key, null);
        if (style == null) {
            if (styleString == null) {
                if (CHAT_STYLE_NORMAL.equals(key)) {
                    style = styles.addStyle(key, null);
                    JTextArea defs = new JTextArea();
                    Font defaultFont = defs.getFont();
                    if (fontStyle == -1) {
                        fontStyle = defaultFont.getStyle();
                    }
                    if (fontFamily == null) {
                        fontFamily = defaultFont.getFamily();
                    }
                    StyleConstants.setFontFamily(style, fontFamily);
                    StyleConstants.setFontSize(style, defaultFont.getSize());
                    StyleConstants.setBold(style, (fontStyle & Font.BOLD) != 0);
                    StyleConstants.setItalic(style,
                            (fontStyle & Font.ITALIC) != 0);
                    StyleConstants.setBackground(style, defs.getBackground());
                } else {
                    style = styles.addStyle(key, getChatStyleNormal());
                    if (fontFamily != null)
                        StyleConstants.setFontFamily(style, fontFamily);
                    if (fontStyle != -1) {
                        StyleConstants.setBold(style,
                                (fontStyle & Font.BOLD) != 0);
                        StyleConstants.setItalic(style,
                                (fontStyle & Font.ITALIC) != 0);
                    }
                }
                StyleConstants.setForeground(style, foreground);
            } else {
                if (CHAT_STYLE_NORMAL.equals(key)) {
                    style = styles.addStyle(key, null);
                } else {
                    style = styles.addStyle(key, getChatStyleNormal());
                }
                try {
                    String[] values = CSVParser.parseLine(styleString);
                    if (!"".equals(values[0]))
                        StyleConstants.setFontFamily(style, values[0]);
                    if (!"".equals(values[1]))
                        StyleConstants.setFontSize(style, Integer
                                .parseInt(values[1]));
                    if (!"".equals(values[2]))
                        StyleConstants.setBold(style, Boolean
                                .valueOf(values[2]).booleanValue());
                    if (!"".equals(values[3]))
                        StyleConstants.setItalic(style, Boolean.valueOf(
                                values[3]).booleanValue());
                    if (!"".equals(values[4]))
                        StyleConstants.setForeground(style, Color
                                .decode(values[4]));
                    if (!"".equals(values[5]))
                        StyleConstants.setBackground(style, Color
                                .decode(values[5]));
                } catch (Throwable e) {
                    // Ignore but log it.
                    log.log(Level.WARNING, "Cannot parse preference " + key, e);
                }
            }
        }
        return style;
    }

    public static void putStyle(String key, Style style) {
        Style oldStyle = styles.getStyle(key);
        if (oldStyle == null) {
            Style newStyle = styles.addStyle(key, null);
            newStyle.addAttributes(style.copyAttributes());
        } else if (style != oldStyle) {
            // Copy just the attributes but keep the parent.
            AttributeSet oldParent = oldStyle.getResolveParent();
            oldStyle.addAttributes(style.copyAttributes());
            oldStyle.setResolveParent(oldParent);
        }
        put(key, encodeStyle(style));
    }

    public static Style getChatStyleAnnounce() {
        return getStyle(CHAT_STYLE_ANNOUNCE, null, Font.BOLD, Color.RED);
    }

    public static Style getChatStyleCommand() {
        return getStyle(CHAT_STYLE_COMMAND, "Monospaced", -1, Color.RED);
    }

    public static Style getChatStyleFriend() {
        return getStyle(CHAT_STYLE_FRIEND, null, -1, Color.GREEN.darker());
    }

    public static Style getChatStyleInfo() {
        return getStyle(CHAT_STYLE_INFO, null, Font.PLAIN, Color.BLUE);
    }

    public static Style getChatStyleMe() {
        return getStyle(CHAT_STYLE_ME, null, -1, Color.GRAY);
    }

    public static Style getChatStyleNormal() {
        return getStyle(CHAT_STYLE_NORMAL, null, -1, Color.BLACK);
    }

    public static Style getChatStyleSender() {
        return getStyle(CHAT_STYLE_SENDER, null, -1, Color.BLUE);
    }

    public static void applyStyle(String key, Component component) {
        if (component == null || key == null) {
            return;
        }
        Font font = component.getFont();
        Style style = getStyle(key, font.getFamily(), font.getStyle(),
                component.getForeground());
        applyStyle(component, style);
    }

    public static void applyStyle(Component component, Style style) {
        int fontStyle = (StyleConstants.isBold(style) ? Font.BOLD : Font.PLAIN)
                | (StyleConstants.isItalic(style) ? Font.ITALIC : Font.PLAIN);
        component.setForeground(StyleConstants.getForeground(style));
        component.setBackground(StyleConstants.getBackground(style));
        component.setFont(new Font(StyleConstants.getFontFamily(style),
                fontStyle, StyleConstants.getFontSize(style)));
    }

    public static void putBoolean(String key, boolean value) {
        prefs.putBoolean(key, value);
    }

    public static boolean getBoolean(String key, boolean defaultValue) {
        return prefs.getBoolean(key, defaultValue);
    }

    public static void putInt(String key, int value) {
        prefs.putInt(key, value);
    }

    public static int getInt(String key, int defaultValue) {
        return prefs.getInt(key, defaultValue);
    }

    /**
     * Adds all Strings read from this key to the collection.
     * 
     * @param key
     * @param collection
     * @return
     */
    public static Collection addAll(String key, Collection collection) {
        String csvString = get(key, null);
        if (csvString != null) {
            String[] values = CSVParser.parseLine(csvString);
            collection.addAll(Arrays.asList(values));
        }
        return collection;
    }

    public static void putStringCollection(String key, Collection collection) {
        String csvString = encodeStringCollection(collection);
        log.fine("Saving collection preferences " + key + ": " + csvString);
        put(key, csvString);
    }

    public static void addPreferenceChangeListener(PreferenceChangeListener pcl) {
        prefs.addPreferenceChangeListener(pcl);
    }

    public static void removePreferenceChangeListener(
            PreferenceChangeListener pcl) {
        prefs.removePreferenceChangeListener(pcl);
    }

    public static String get(String key, String defaultValue) {
        return prefs.get(key, defaultValue);
    }

    public static void put(String key, String value) {
        if (value == null) {
            prefs.remove(key);
        } else {
            prefs.put(key, value);
        }
    }

    private static String encodeFont(Font font) {
        return font.getFamily()
                + "-"
                + (font.isPlain() ? "PLAIN" : (font.isBold() ? "BOLD" : "")
                        + (font.isItalic() ? "ITALIC" : "")) + "-"
                + font.getSize();
    }

    private static String encodeColor(Color color) {
        if (color == null)
            return null;
        return "0x" + toTwoDigitHexString(color.getRed())
                + toTwoDigitHexString(color.getGreen())
                + toTwoDigitHexString(color.getBlue());
    }

    private static String encodeStyle(Style style) {
        if (style == null)
            return null;

        return CSVParser.escape(getDefinedAttribute(style,
                StyleConstants.FontFamily))
                + ","
                + getDefinedAttribute(style, StyleConstants.FontSize)
                + ","
                + getDefinedAttribute(style, StyleConstants.Bold)
                + ","
                + getDefinedAttribute(style, StyleConstants.Italic)
                + ","
                + getDefinedAttribute(style, StyleConstants.Foreground)
                + ","
                + getDefinedAttribute(style, StyleConstants.Background);
    }

    private static String getDefinedAttribute(Style style, Object attribute) {
        if (style.isDefined(attribute)) {
            Object value = style.getAttribute(attribute);
            if (value instanceof Color)
                return encodeColor((Color) value);
            return value.toString();
        }
        return "";
    }

    private static String toTwoDigitHexString(int n) {
        String s = Integer.toHexString(n);
        return s.length() == 1 ? "0" + s : s;
    }

    private static String encodeStringCollection(Collection collection) {
        Iterator iter = collection.iterator();
        StringBuffer result = new StringBuffer();
        while (iter.hasNext()) {
            result.append(CSVParser.escape((String) iter.next()));
            if (iter.hasNext()) {
                result.append(",");
            }
        }
        return result.toString();
    }
}
