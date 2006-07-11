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

import java.awt.Dimension;
import java.awt.Font;
import java.awt.GraphicsEnvironment;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JColorChooser;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSpinner;
import javax.swing.JTextPane;
import javax.swing.SpinnerNumberModel;

public class ChatPreferencesTab extends JPanel implements PreferencesTab {
    private JComboBox fontFamilyCombo;

    private JSpinner fontSizeSpinner;

    private JCheckBox boldCheckBox;

    private JCheckBox italicCheckBox;

    private JButton backgroundColorButton;

    private JButton foregroundColorButton;

    private JButton myColorButton;

    public ChatPreferencesTab() {
        super(new GridBagLayout());
        JTextPane defaults = new JTextPane();
        Font font = GGZPreferences.getFont(GGZPreferences.CHAT_FONT, defaults
                .getFont());
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

        GridBagConstraints constraints = new GridBagConstraints();
        constraints.gridx = 0;
        constraints.gridy = 0;
        constraints.anchor = GridBagConstraints.WEST;
        constraints.gridwidth = 2;
        add(
                new JLabel(
                        "<html>These font settings only effect what you see, "
                                + "they do not change<br>the font that other players see.</html>"),
                constraints);
        constraints.gridwidth = 1;
        constraints.gridy++;
        add(new JLabel("Font:"), constraints);
        constraints.gridx++;
        JPanel fontPanel = new JPanel();
        add(fontPanel, constraints);
        fontPanel.add(fontFamilyCombo);
        fontPanel.add(fontSizeSpinner);
        fontPanel.add(boldCheckBox);
        fontPanel.add(italicCheckBox);

        ActionListener colorSelector = new ColorSelector();
        constraints.gridx = 0;
        constraints.gridy++;
        add(new JLabel("Foreground:"), constraints);
        constraints.gridx++;
        constraints.insets.left = 5;
        foregroundColorButton = new JButton("");
        foregroundColorButton.setPreferredSize(new Dimension(37, 25));
        foregroundColorButton.setBackground(GGZPreferences.getColor(
                GGZPreferences.CHAT_FOREGROUND, defaults.getForeground()));
        foregroundColorButton.putClientProperty("dialogTitle",
                "Foreground Colour");
        foregroundColorButton.addActionListener(colorSelector);
        add(foregroundColorButton, constraints);

        constraints.gridx = 0;
        constraints.gridy++;
        constraints.insets.left = 0;
        add(new JLabel("Background:"), constraints);
        constraints.gridx++;
        constraints.insets.left = 5;
        backgroundColorButton = new JButton("");
        backgroundColorButton.setPreferredSize(new Dimension(37, 25));
        backgroundColorButton.setBackground(GGZPreferences.getColor(
                GGZPreferences.CHAT_BACKGROUND, defaults.getBackground()));
        backgroundColorButton.putClientProperty("dialogTitle",
                "Background Colour");
        backgroundColorButton.addActionListener(colorSelector);
        add(backgroundColorButton, constraints);

        constraints.gridx = 0;
        constraints.gridy++;
        constraints.gridwidth = 2;
        constraints.insets.left = 0;
        add(new JLabel(
                "<html>The following colour settings will only take effect on new text,"
                        + "<br>existing text will not be affected.</html>"),
                constraints);
        constraints.gridwidth = 1;
        constraints.gridy++;
        add(new JLabel("My Text:"), constraints);
        constraints.gridx++;
        constraints.insets.left = 5;
        myColorButton = new JButton("");
        myColorButton.setPreferredSize(new Dimension(37, 25));
        myColorButton.setBackground(GGZPreferences.getColor(
                GGZPreferences.MY_FONT_COLOR, null));
        myColorButton.putClientProperty("dialogTitle", "My Text");
        myColorButton.addActionListener(colorSelector);
        add(myColorButton, constraints);

        setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));
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
        GGZPreferences.putColor(GGZPreferences.CHAT_BACKGROUND,
                backgroundColorButton.getBackground());
        GGZPreferences.putColor(GGZPreferences.CHAT_FOREGROUND,
                foregroundColorButton.getBackground());
        GGZPreferences.putColor(GGZPreferences.MY_FONT_COLOR, myColorButton
                .getBackground());
    }

    private static void getFontFamilyNames(JComboBox combo) {
        GraphicsEnvironment gEnv = GraphicsEnvironment
                .getLocalGraphicsEnvironment();
        combo.setModel(new DefaultComboBoxModel(gEnv
                .getAvailableFontFamilyNames()));
    }

    private class ColorSelector implements ActionListener {
        public void actionPerformed(ActionEvent e) {
            JComponent button = (JComponent) e.getSource();
            String title = (String) button.getClientProperty("dialogTitle");
            button.setBackground(JColorChooser.showDialog(button, title, button
                    .getBackground()));
        }
    }
}
