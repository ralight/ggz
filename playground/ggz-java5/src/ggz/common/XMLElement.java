package ggz.common;

import org.xml.sax.Attributes;

/** @brief Object representing a single XML element.
*
*  Except for "process", do not access these members directly.
*  Instead use the provided accessor functions.  "process" is meant
*  to be invoked as a method on instances of GGZXMLElement.
*/
public class XMLElement {
    /** < The name of the element */
    private String tag;

    /** < Text content of an element */
    private StringBuffer text;

    /** < List of attributes on the element */
    private String[][] attributes;

    /** < Extra data associated with tag (usually gleaned from children) */
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
