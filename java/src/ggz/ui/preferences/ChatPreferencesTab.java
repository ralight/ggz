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

import java.awt.Component;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;

import javax.swing.BorderFactory;
import javax.swing.DefaultListCellRenderer;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.text.Style;
import javax.swing.text.StyleContext;

public class ChatPreferencesTab extends JPanel implements PreferencesTab,
        ListSelectionListener {
    private StyleChooser styleChooser;

    protected StyleContext styleContext = new StyleContext();

    protected StylePreference[] stylePreferences;

    public ChatPreferencesTab() {
        super(new GridBagLayout());
        setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));
        GridBagConstraints constraints = new GridBagConstraints();
        constraints.gridx = 0;
        constraints.gridy = 0;
        constraints.anchor = GridBagConstraints.WEST;
        constraints.gridwidth = 2;
        constraints.insets.bottom = 15;
        add(
                new JLabel(
                        "<html>These font settings only effect what you see, "
                                + "they do not change the font that other players see.<br>"
                                + " Only changes to Normal Chat will take effect immediately,"
                                + " other changes will only be applied to<br>new text.</html>"),
                constraints);

        stylePreferences = getChatStyles();
        JList stylesList = new JList(stylePreferences);
        stylesList.setSelectedIndex(0);
        styleChooser = new StyleChooser(((StylePreference) stylesList
                .getSelectedValue()).style);

        constraints.gridx = 0;
        constraints.gridy++;
        constraints.gridwidth = 1;
        constraints.insets.bottom = 0;
        constraints.fill = GridBagConstraints.BOTH;
        add(new JScrollPane(stylesList), constraints);

        constraints.gridx++;
        constraints.insets.left = 5;
        add(styleChooser, constraints);

        stylesList.addListSelectionListener(this);
        stylesList.setFixedCellHeight(-1);
        stylesList.setCellRenderer(new DefaultListCellRenderer() {

            public Component getListCellRendererComponent(JList list,
                    Object value, int index, boolean isSelected,
                    boolean cellHasFocus) {
                StylePreference pref = (StylePreference) value;
                Component comp = super.getListCellRendererComponent(list,
                        pref.description, index, isSelected, cellHasFocus);
                GGZPreferences.applyStyle(comp, pref.style);
                return comp;
            }

        });
    }

    public String getTitle() {
        return "Chat";
    }

    public void apply() {
        if (stylePreferences[0].hasChanged)
            GGZPreferences.putStyle(GGZPreferences.CHAT_STYLE_NORMAL,
                    stylePreferences[0].style);
        if (stylePreferences[1].hasChanged)
            GGZPreferences.putStyle(GGZPreferences.CHAT_STYLE_ME,
                    stylePreferences[1].style);
        if (stylePreferences[2].hasChanged)
            GGZPreferences.putStyle(GGZPreferences.CHAT_STYLE_FRIEND,
                    stylePreferences[2].style);
        if (stylePreferences[3].hasChanged)
            GGZPreferences.putStyle(GGZPreferences.CHAT_STYLE_INFO,
                    stylePreferences[3].style);
        if (stylePreferences[4].hasChanged)
            GGZPreferences.putStyle(GGZPreferences.CHAT_STYLE_SENDER,
                    stylePreferences[4].style);
        if (stylePreferences[5].hasChanged)
            GGZPreferences.putStyle(GGZPreferences.CHAT_STYLE_ANNOUNCE,
                    stylePreferences[5].style);
        if (stylePreferences[6].hasChanged)
            GGZPreferences.putStyle(GGZPreferences.CHAT_STYLE_COMMAND,
                    stylePreferences[6].style);
    }

    public void dispose() {
        for (int i = 0; i < this.stylePreferences.length; i++) {
            this.stylePreferences[i].dispose();
        }
    }

    private StylePreference[] getChatStyles() {
        // We need to make sure the copy of the normal style is used as the
        // parent for all other styles so that they update correctly.
        StylePreference normalPreference = new StylePreference("Normal Chat",
                GGZPreferences.getChatStyleNormal(), null);
        Style normal = normalPreference.style;
        return new StylePreference[] {
                normalPreference,
                new StylePreference("My Chat", GGZPreferences.getChatStyleMe(),
                        normal),
                new StylePreference("Friend Chat", GGZPreferences
                        .getChatStyleFriend(), normal),
                new StylePreference("Information Text", GGZPreferences
                        .getChatStyleInfo(), normal),
                new StylePreference("Nickname:", GGZPreferences
                        .getChatStyleSender(), normal),
                new StylePreference("Administrator Announcement",
                        GGZPreferences.getChatStyleAnnounce(), normal),
                new StylePreference("Command Text", GGZPreferences
                        .getChatStyleCommand(), normal) };
    }

    public void valueChanged(ListSelectionEvent e) {
        // User selected a style in the list.
        JList list = (JList) e.getSource();
        styleChooser
                .setStyle(((StylePreference) list.getSelectedValue()).style);
    }

    private class StylePreference implements ChangeListener {
        Style style;

        String description;

        boolean hasChanged = false;

        public StylePreference(String description, Style style,
                Style normalStyle) {
            this.description = description;
            this.style = styleContext.addStyle(style.getName(), null);
            this.style = (Style) style.copyAttributes();
            this.style.setResolveParent(normalStyle);
            this.style.addChangeListener(this);
        }

        public void stateChanged(ChangeEvent e) {
            hasChanged = true;
            ChatPreferencesTab.this.repaint();
        }

        public void dispose() {
            this.style.removeChangeListener(this);
        }

    }
}
