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
import java.awt.Font;
import java.util.Arrays;
import java.util.Collection;
import java.util.Iterator;
import java.util.logging.Logger;
import java.util.prefs.PreferenceChangeListener;
import java.util.prefs.Preferences;

public class GGZPreferences {
    public static String CHAT_FONT = "Chat.Font";

    public static String MY_FONT_COLOR = "Chat.My.Color";

    public static String IGNORE_LIST = "Char.Ignore.List";

    public static String FRIENDS_LIST = "Char.Friends.List";

    public static final Logger log = Logger.getLogger(GGZPreferences.class
            .getName());

    private static Preferences prefs;

    static {
        try {
            prefs = Preferences.userNodeForPackage(GGZPreferences.class);
        } catch (SecurityException e) {
            // Running under a security manager so just use propterties.
            prefs = new PropertiesPreferences();
        }
    }

    public static Font getFont(String key, Font defaultValue) {
        return getFont(key, encodeFont(defaultValue));
    }

    public static Font getFont(String key, String defaultValue) {
        return Font.decode(get(key, defaultValue));
    }

    public static void putFont(String key, Font font) {
        String fontString = encodeFont(font);
        log.fine("Saving font preference " + key + ": " + fontString);
        put(key, fontString);
    }

    public static Color getColor(String key, Color defaultValue) {
        return getColor(key, encodeColor(defaultValue));
    }

    public static Color getColor(String key, String defaultValue) {
        return Color.decode(get(key, defaultValue));
    }

    public static void putColor(String key, Color color) {
        String colorString = encodeColor(color);
        log.fine("Saving color preference " + key + ": " + colorString);
        put(key, colorString);
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

    private static String get(String key, String defaultValue) {
        return prefs.get(key, defaultValue);
    }

    private static void put(String key, String value) {
        prefs.put(key, value);
    }

    private static String encodeFont(Font font) {
        return font.getFamily()
                + "-"
                + (font.isPlain() ? "PLAIN" : (font.isBold() ? "BOLD" : "")
                        + (font.isItalic() ? "ITALIC" : "")) + "-"
                + font.getSize();
    }

    private static String encodeColor(Color color) {
        return "0x" + Integer.toHexString(color.getRGB())
                + Integer.toHexString(color.getTransparency());
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
