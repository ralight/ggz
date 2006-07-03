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

import java.awt.Font;
import java.awt.GraphicsEnvironment;

import javax.swing.DefaultComboBoxModel;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSpinner;
import javax.swing.SpinnerNumberModel;

public class ChatPreferencesTab extends JPanel implements PreferencesTab {
    private JComboBox fontFamilyCombo;

    private JSpinner fontSizeSpinner;

    private JCheckBox boldCheckBox;

    private JCheckBox italicCheckBox;

    // Font size that the cell renderer uses to display fonts.
    private static final float fontSize = 14.0f;

    public ChatPreferencesTab() {
        Font font = GGZPreferences.getFont(GGZPreferences.CHAT_FONT,
                "Dialog-NORMAL-" + (int) fontSize);
        fontFamilyCombo = new JComboBox();
        getFontFamilyNames(fontFamilyCombo);
        fontFamilyCombo.setSelectedItem(font.getName());
        double fontSize = font.getSize();
        if (fontSize < 8.0)
            fontSize = 8.0;
        if (fontSize > 72.0)
            fontSize = 72.0;
        fontSizeSpinner = new JSpinner(new SpinnerNumberModel(fontSize, 8.0,
                72.0, 1.0));
        boldCheckBox = new JCheckBox("Bold", font.isBold());
        italicCheckBox = new JCheckBox("Italic", font.isItalic());
        add(new JLabel("Font:"));
        add(fontFamilyCombo);
        add(fontSizeSpinner);
        add(boldCheckBox);
        add(italicCheckBox);
    }

    public String getTitle() {
        return "Chat";
    }

    public void apply() {
        String name = (String) fontFamilyCombo.getSelectedItem();
        int bold = boldCheckBox.isSelected() ? Font.BOLD : Font.PLAIN;
        int italic = italicCheckBox.isSelected() ? Font.ITALIC : Font.PLAIN;
        int style = italic | bold;
        int size = ((Double) fontSizeSpinner.getValue()).intValue();
        Font font = new Font(name, style, size);
        font = font.deriveFont(size);
        font = font.deriveFont(style);
        GGZPreferences.putFont(GGZPreferences.CHAT_FONT, font);
    }

    private static void getFontFamilyNames(JComboBox combo) {
        GraphicsEnvironment gEnv = GraphicsEnvironment
                .getLocalGraphicsEnvironment();
        combo.setModel(new DefaultComboBoxModel(gEnv
                .getAvailableFontFamilyNames()));
    }

}
