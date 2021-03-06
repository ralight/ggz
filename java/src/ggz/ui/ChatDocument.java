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

import java.awt.Image;
import java.awt.Toolkit;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.text.MessageFormat;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Properties;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.ImageIcon;
import javax.swing.text.BadLocationException;
import javax.swing.text.Element;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.html.HTML;
import javax.swing.text.html.HTMLDocument;
import javax.swing.text.html.StyleSheet;
import javax.swing.text.html.parser.ParserDelegator;

public class ChatDocument extends HTMLDocument {

    private static final Properties emoticons = new Properties();

    private static final Hashtable imageCache = new Hashtable();

    private static final Pattern emoticonRegexp;

    private static final Pattern urlRegexp = Pattern
            .compile("(?i)((http://|https://|www.|ftp://|mailto:)([-\\w\\.]+)+(:\\d+)?(/([\\w/_\\-\\.]*(\\?\\S+)?)?)?)");

    static {
        // Load emoticon icons.
        try {
            InputStream is = ChatDocument.class
                    .getResourceAsStream("/ggz/ui/emoticons.properties");
            emoticons.load(is);
        } catch (IOException e) {
            e.printStackTrace();
        }

        // Update each emoticon, replacing the resource with its URL.
        // Also cache the image and store the cache in the document.
        for (Enumeration i = emoticons.keys(); i.hasMoreElements();) {
            String emoticon = (String) i.nextElement();
            String resource = emoticons.getProperty(emoticon);
            URL url = ChatDocument.class.getResource(resource);

            // If we can't find the image for the emoticon then ignore it.
            if (url != null) {
                String imgTag = MessageFormat.format(
                        "<img src=\"{1}\" alt=\"{0}\">", new Object[] {
                                emoticon, url });
                emoticons.put(emoticon, imgTag);

                // Check if this emote is a duplicate to prevent reloading the
                // image.
                if (!imageCache.contains(url)) {
                    Image newImage = Toolkit.getDefaultToolkit().createImage(
                            url);
                    if (newImage != null) {
                        // Force the image to be loaded by using an ImageIcon.
                        ImageIcon ii = new ImageIcon();
                        ii.setImage(newImage);
                        imageCache.put(url, newImage);
                    }
                }
            }
        }

        emoticonRegexp = compileEmoticonsAsRegexp();
    }

    public ChatDocument(StyleSheet ss) {
        super(ss);
        setAsynchronousLoadPriority(4);
        setTokenThreshold(100);
        setParser(new ParserDelegator());

        // This is only done for performance but may affect JDK compatibility.
        putProperty("imageCache", imageCache);
    }

    public void insertString(int offs, String str, String styleName,
            boolean detectMarkup) throws BadLocationException {
        super.insertString(offs, str, null);

        if (styleName != null) {
            SimpleAttributeSet className = new SimpleAttributeSet();
            SimpleAttributeSet fontTag = new SimpleAttributeSet();
            className.addAttribute(HTML.Attribute.CLASS, styleName
                    .toLowerCase());
            fontTag.addAttribute(HTML.Tag.FONT, className);
            super.setCharacterAttributes(offs, str.length(), fontTag, false);
        }

        if (detectMarkup) {
            try {
                Element currentElement = getCharacterElement(offs);
                convertEmoticons(currentElement, offs);
                convertLinks(currentElement, offs);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    protected void convertLinks(Element e, int offset)
            throws BadLocationException {
        int length = e.getEndOffset() - offset;
        String text = getText(offset, length);
        Matcher matcher = urlRegexp.matcher(text);

        while (matcher.find()) {
            String url = matcher.group(1);
            String prefix = matcher.group(2);
            if ("www.".equals(prefix)) {
                url = "http://" + url;
            }
            SimpleAttributeSet src = new SimpleAttributeSet();
            SimpleAttributeSet tag = new SimpleAttributeSet();
            src.addAttribute(HTML.Attribute.HREF, url);
            tag.addAttribute(HTML.Tag.A, src);
            setCharacterAttributes(offset + matcher.start(), matcher.end()
                    - matcher.start(), tag, false);
        }
    }

    protected void convertEmoticons(Element e, int offset)
            throws BadLocationException, IOException {
        int startOffset = e.getStartOffset();
        int length = e.getEndOffset() - offset;
        String text = getText(offset, length);
        Matcher matcher = emoticonRegexp.matcher(text);
        StringBuffer sb = new StringBuffer();
        sb.append(getText(startOffset, offset - startOffset));
        HashMap newImageTags = new HashMap();
        while (matcher.find()) {
            String html = (String) emoticons.get(matcher.group(1));
            SimpleAttributeSet dummyTag = new SimpleAttributeSet();
            dummyTag.addAttribute(HTML.Tag.IMG, new SimpleAttributeSet());
            setCharacterAttributes(offset + matcher.start(), matcher.end()
                    - matcher.start(), dummyTag, false);
            Element elem = getCharacterElement(offset + matcher.start());
            newImageTags.put(elem, html);
        }

        // Replace all dummy <img> tags with real <img> tags so as to overwrite
        // the emoticon string.
        for (java.util.Iterator i = newImageTags.keySet().iterator(); i
                .hasNext();) {
            Element elem = (Element) i.next();
            String html = (String) newImageTags.get(elem);
            setOuterHTML(elem, html);
        }
    }

    private static Pattern compileEmoticonsAsRegexp() {
        StringBuffer sb = new StringBuffer();
        sb.append("(");
        for (java.util.Iterator i = emoticons.keySet().iterator(); i.hasNext();) {
            String emoticon = (String) i.next();
            escapeForRegexp(emoticon, sb);
            if (i.hasNext())
                sb.append("|");
        }
        sb.append(")");
        return Pattern.compile(sb.toString());
    }

    /** Escape regexp special characters. */
    private static void escapeForRegexp(String s, StringBuffer sb) {
        String specialChars = "[\\^$.|?*+()";
        for (int index = 0, n = s.length(); index < n; index++) {
            char ch = s.charAt(index);
            if (specialChars.indexOf(ch) >= 0)
                sb.append('\\');
            sb.append(ch);
        }
    }
}
