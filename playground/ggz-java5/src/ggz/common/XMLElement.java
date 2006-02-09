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
package ggz.common;

import org.xml.sax.Attributes;

/**
 * Object representing a single XML element.
 * 
 * Except for "process", do not access these members directly. Instead use the
 * provided accessor functions. "process" is meant to be invoked as a method on
 * instances of GGZXMLElement.
 */
public class XMLElement {
    /** The name of the element. */
    private String tag;

    /** Text content of an element. */
    private StringBuffer text;

    /** List of attributes on the element. */
    private String[][] attributes;

    /** Extra data associated with tag (usually gleaned from children). */
    private Object data;

    public XMLElement(String tag, Attributes attrs) {
        this.tag = tag;
        if (attrs.getLength() > 0) {
            this.attributes = new String[attrs.getLength()][2];
            for (int i = 0; i < attrs.getLength(); i++) {
                this.attributes[i][0] = attrs.getQName(i);
                this.attributes[i][1] = attrs.getValue(i);
            }
        }
    }

    public void set_data(Object data) {
        this.data = data;
    }

    public String get_tag() {
        return this.tag;
    }

    public String get_attr(String attr) {
        if (this.attributes != null) {
            for (int i = 0; i < this.attributes.length; i++) {
                if (this.attributes[i][0].equals(attr)) {
                    return this.attributes[i][1];
                }
            }
        }
        return null;
    }

    public Object get_data() {
        return this.data;
    }

    public String get_text() {
        return this.text == null ? null : this.text.toString();
    }

    public void add_text(char[] ch, int start, int length) {
        if (text == null) {
            text = new StringBuffer();
        }
        text.append(ch, start, length);
    }
}
