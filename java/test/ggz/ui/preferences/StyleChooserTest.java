package ggz.ui.preferences;

import java.awt.Color;

import javax.swing.text.Style;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyleContext;

import junit.framework.TestCase;

public class StyleChooserTest extends TestCase {
    private StyleContext context = new StyleContext();

    public void testAPI() {
        Style style = context.addStyle("testStyle", null);
        StyleChooser chooser = new StyleChooser(style);

        assertEquals(style, chooser.getStyle());

//        chooser.setFontStyle(3);
//        assertEquals(true, StyleConstants.isBold(style));
//        assertEquals(true, StyleConstants.isItalic(style));
//        chooser.setFontStyle(2);
//        assertEquals(true, StyleConstants.isBold(style));
//        assertEquals(false, StyleConstants.isItalic(style));
//        chooser.setFontStyle(1);
//        assertEquals(false, StyleConstants.isBold(style));
//        assertEquals(true, StyleConstants.isItalic(style));
//        chooser.setFontStyle(0);
//        assertEquals(false, StyleConstants.isBold(style));
//        assertEquals(false, StyleConstants.isItalic(style));
//
//        chooser.setFontFamily("Dialog");
//        assertEquals("Dialog", StyleConstants.getFontFamily(style));
//
//        chooser.setFontSize(new Integer(72));
//        assertEquals(72, StyleConstants.getFontSize(style));
    }

    public void testUI() throws Exception {
        Style style = context.addStyle("testStyle", null);
//        StyleConstants.setFontFamily(style, "Dialog");
        StyleConstants.setForeground(style, Color.BLUE);
        StyleConstants.setBackground(style, Color.WHITE);
        StyleChooser.showDialog(null, "JUnit Test", style);
    }
}
