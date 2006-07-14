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

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.GraphicsEnvironment;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.HeadlessException;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.AbstractAction;
import javax.swing.BorderFactory;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JButton;
import javax.swing.JColorChooser;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.WindowConstants;
import javax.swing.border.Border;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.text.Style;
import javax.swing.text.StyleConstants;

public class StyleChooser extends JPanel implements ListSelectionListener,
        ActionListener {
    private Style style;

    private JList fontFamilyList;

    private JList fontSizeList;

    private JList fontStyleList;

    private JLabel sampleLabel;

    private JButton backgroundColorButton;

    private JButton foregroundColorButton;

    public StyleChooser(Style initialStyle) {
        super(new GridBagLayout());
        this.style = initialStyle;
        GridBagConstraints constraints = new GridBagConstraints();

        constraints.anchor = GridBagConstraints.NORTHWEST;
        constraints.insets.left = 5;
        constraints.gridx = 0;
        constraints.gridy = 0;
        add(new JLabel("Font:"), constraints);
        constraints.gridy++;
        add(createFontFamilyList(), constraints);

        constraints.gridx++;
        constraints.gridy = 0;
        add(new JLabel("Font style:"), constraints);
        constraints.gridy++;
        add(createFontStyleList(), constraints);

        constraints.gridx++;
        constraints.gridy = 0;
        add(new JLabel("Size:"), constraints);
        constraints.gridy++;
        add(createFontSizeList(), constraints);

        constraints.gridx = 0;
        constraints.gridy = 2;
        constraints.insets.top = 5;
        add(new JLabel("Foreground color:"), constraints);
        constraints.gridy++;
        add(createForegroundColorButton(), constraints);

        constraints.gridy++;
        add(new JLabel("Background color:"), constraints);
        constraints.gridy++;
        add(createBackgroundColorButton(), constraints);

        constraints.gridx = 1;
        constraints.gridy = 2;
        constraints.gridheight = 4;
        constraints.gridwidth = 2;
        constraints.fill = GridBagConstraints.BOTH;
        add(createSampleLabel(), constraints);
    }

    private JScrollPane createFontFamilyList() {
        GraphicsEnvironment gEnv = GraphicsEnvironment
                .getLocalGraphicsEnvironment();
        this.fontFamilyList = new JList(new DefaultComboBoxModel(gEnv
                .getAvailableFontFamilyNames()));
        JScrollPane scroller = new JScrollPane(this.fontFamilyList);
        setFontFamilyFromStyle();
        return scroller;
    }

    private void setFontFamilyFromStyle() {
        String fontFamily = StyleConstants.getFontFamily(this.style);
        this.fontFamilyList.setSelectedValue(fontFamily, true);
        this.fontFamilyList.addListSelectionListener(this);
    }

    private JScrollPane createFontStyleList() {
        this.fontStyleList = new JList(new String[] { "Regular", "Italic",
                "Bold", "Bold Italic" });
        this.fontStyleList.addListSelectionListener(this);
        JScrollPane scroller = new JScrollPane(this.fontStyleList);
        Dimension prefSize = scroller.getPreferredSize();
        prefSize.width *= 2;
        scroller.setPreferredSize(prefSize);
        setFontStyleFromStyle();
        return scroller;
    }

    private void setFontStyleFromStyle() {
        boolean isBold = StyleConstants.isBold(this.style);
        boolean isItalic = StyleConstants.isItalic(this.style);
        int styleIndex = (isBold ? 2 : 0) + (isItalic ? 1 : 0);

        this.fontStyleList.setSelectedIndex(styleIndex);
    }

    private JScrollPane createFontSizeList() {
        this.fontSizeList = new JList(new Integer[] { new Integer(8),
                new Integer(9), new Integer(10), new Integer(11),
                new Integer(12), new Integer(14), new Integer(16),
                new Integer(18), new Integer(20), new Integer(22),
                new Integer(24), new Integer(26), new Integer(28),
                new Integer(36), new Integer(48), new Integer(72) });
        JScrollPane scroller = new JScrollPane(this.fontSizeList);
        this.fontSizeList.addListSelectionListener(this);
        Dimension prefSize = scroller.getPreferredSize();
        prefSize.width *= 1.5;
        scroller.setPreferredSize(prefSize);
        setFontSizeFromStyle();
        return scroller;
    }

    private void setFontSizeFromStyle() {
        int fontSize = StyleConstants.getFontSize(this.style);
        if (fontSize < 8)
            fontSize = 8;
        if (fontSize > 72)
            fontSize = 72;
        this.fontSizeList.setSelectedValue(new Integer(fontSize), true);
        if (this.fontSizeList.getSelectedValue() == null) {
            // Couldn't find the value in the list so default to 12.
            this.fontSizeList.setSelectedIndex(4);
        }
    }

    private JComponent createSampleLabel() {
        this.sampleLabel = new JLabel(
                "<html>The quick brown fox jumped over the lazy dog.</html>");
        Border title = BorderFactory.createCompoundBorder(BorderFactory
                .createTitledBorder("Sample"), BorderFactory.createEmptyBorder(
                5, 5, 5, 5));
        JPanel border = new JPanel(new BorderLayout());
        border.setBorder(title);
        border.add(sampleLabel, BorderLayout.CENTER);
        this.sampleLabel.setOpaque(true);
        this.sampleLabel.setPreferredSize(new Dimension(20, 10));
        this.sampleLabel.setBorder(BorderFactory.createLoweredBevelBorder());
        applyStyleToLabel();
        return border;
    }

    private void applyStyleToLabel() {
        this.sampleLabel.setFont(getSelectedFont());
        this.sampleLabel
                .setForeground(StyleConstants.getForeground(this.style));
        this.sampleLabel
                .setBackground(StyleConstants.getBackground(this.style));
    }

    private JComponent createForegroundColorButton() {
        this.foregroundColorButton = new JButton();
        this.foregroundColorButton
                .setPreferredSize(new Dimension(/* 37 */fontFamilyList
                        .getPreferredScrollableViewportSize().width, 25));
        this.foregroundColorButton.addActionListener(this);
        this.foregroundColorButton.putClientProperty("dialogTitle",
                "Foreground color");
        setForegroundFromStyle();
        return this.foregroundColorButton;
    }

    private void setForegroundFromStyle() {
        this.foregroundColorButton.setBackground(StyleConstants
                .getForeground(this.style));
    }

    private JComponent createBackgroundColorButton() {
        this.backgroundColorButton = new JButton();
        this.backgroundColorButton
                .setPreferredSize(new Dimension(/* 37 */fontFamilyList
                        .getPreferredScrollableViewportSize().width, 25));
        this.backgroundColorButton.addActionListener(this);
        this.backgroundColorButton.putClientProperty("dialogTitle",
                "Background color");
        setBackgroundFromStyle();
        return this.backgroundColorButton;
    }

    private void setBackgroundFromStyle() {
        this.backgroundColorButton.setBackground(StyleConstants
                .getBackground(this.style));
    }

    public static Style showDialog(Component component, String title,
            Style initialStyle) throws HeadlessException {

        final StyleChooser pane = new StyleChooser(initialStyle);
        final JDialog dialog = new JDialog(JOptionPane
                .getFrameForComponent(component), title);
        dialog.getContentPane().add(pane, BorderLayout.CENTER);
        dialog.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
        dialog.setModal(true);

        JPanel buttonPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT, 5, 21));
        JPanel buttonSizePanel = new JPanel(new GridLayout(0, 1, 5, 5));
        JButton okButton = new JButton(new AbstractAction("OK") {
            public void actionPerformed(ActionEvent e) {
                dialog.dispose();
            }
        });
        JButton cancelButton = new JButton(new AbstractAction("Cancel") {
            public void actionPerformed(ActionEvent e) {
                dialog.dispose();
            }
        });
        buttonSizePanel.add(okButton);
        buttonSizePanel.add(cancelButton);
        buttonPanel.add(buttonSizePanel);
        dialog.getContentPane().add(buttonPanel, BorderLayout.EAST);
        pane.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

        dialog.pack();
        dialog.show(); // blocks until user brings dialog down...

        return pane.getStyle();
    }

    public void setStyle(Style style) {
        this.style = style;
        setFontFamilyFromStyle();
        setFontStyleFromStyle();
        setFontSizeFromStyle();
        setForegroundFromStyle();
        setBackgroundFromStyle();
        applyStyleToLabel();
    }

    public Style getStyle() {
        // String family = (String) fontFamilyList.getSelectedValue();
        // int styleIndex = fontStyleList.getSelectedIndex();
        // Integer size = (Integer) fontSizeList.getSelectedValue();
        // StyleConstants.setFontFamily(this.style, family);
        // StyleConstants.setBold(this.style, (styleIndex & 2) == 2);
        // StyleConstants.setItalic(this.style, (styleIndex & 1) == 1);
        // StyleConstants.setFontSize(this.style, size.intValue());
        // StyleConstants.setForeground(this.style,
        // sampleLabel.getForeground());
        // StyleConstants.setBackground(this.style,
        // sampleLabel.getBackground());
        return style;
    }

    public Font getSelectedFont() {
        // String family = (String) fontFamilyList.getSelectedValue();
        // int styleIndex = fontStyleList.getSelectedIndex();
        // int fontStyle = (((styleIndex & 2) == 2) ? Font.BOLD : Font.PLAIN)
        // | (((styleIndex & 1) == 1) ? Font.ITALIC : Font.PLAIN);
        // int size = ((Integer) fontSizeList.getSelectedValue()).intValue();
        String family = StyleConstants.getFontFamily(this.style);
        int fontStyle = (StyleConstants.isBold(this.style) ? Font.BOLD
                : Font.PLAIN)
                | (StyleConstants.isItalic(this.style) ? Font.ITALIC
                        : Font.PLAIN);
        int size = StyleConstants.getFontSize(this.style);
        return new Font(family, fontStyle, size);
    }

    public void valueChanged(ListSelectionEvent e) {
        if (e.getValueIsAdjusting())
            return;

        if (e.getSource() == fontFamilyList) {
            String fontFamily = (String) this.fontFamilyList.getSelectedValue();
            if (!fontFamily.equals(StyleConstants.getFontFamily(style)))
                StyleConstants.setFontFamily(this.style, fontFamily);
        } else if (e.getSource() == fontStyleList) {
            int styleIndex = fontStyleList.getSelectedIndex();
            boolean isBold = (styleIndex & 2) == 2;
            boolean isItalic = (styleIndex & 1) == 1;
            if (isBold != StyleConstants.isBold(style))
                StyleConstants.setBold(this.style, isBold);
            if (isItalic != StyleConstants.isItalic(style))
                StyleConstants.setItalic(this.style, isItalic);
        } else if (e.getSource() == fontSizeList) {
            int size = ((Integer) fontSizeList.getSelectedValue()).intValue();
            if (size != StyleConstants.getFontSize(style))
                StyleConstants.setFontSize(this.style, size);
        }

        if (this.sampleLabel != null)
            this.sampleLabel.setFont(getSelectedFont());
    }

    public void actionPerformed(ActionEvent e) {
        JComponent button = (JComponent) e.getSource();
        Color selection = JColorChooser.showDialog(button, (String) button
                .getClientProperty("dialogTitle"), button.getBackground());
        if (selection != null) {
            button.setBackground(selection);
            if (button == backgroundColorButton) {
                if (!selection.equals(StyleConstants.getBackground(style))) {
                    StyleConstants.setBackground(this.style, selection);
                    sampleLabel.setBackground(selection);
                }
            } else if (button == foregroundColorButton) {
                if (!selection.equals(StyleConstants.getForeground(style))) {
                    StyleConstants.setForeground(this.style, selection);
                    sampleLabel.setForeground(selection);
                }
            }
        }
    }
}
