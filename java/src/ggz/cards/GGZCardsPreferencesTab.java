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
package ggz.cards;

import ggz.ui.preferences.GGZPreferences;
import ggz.ui.preferences.PreferencesTab;

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;

import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSpinner;
import javax.swing.SpinnerNumberModel;

public class GGZCardsPreferencesTab extends JPanel implements PreferencesTab {
    private JCheckBox spinCardsCheckBox;

    private JCheckBox closeGapCheckBox;

    private JCheckBox beepOnTurnCheckBox;

    private JSpinner cardGapSpinner;

    public GGZCardsPreferencesTab() {
        super(new GridBagLayout());
        spinCardsCheckBox = new JCheckBox(
                "Spin cards when they are played to the trick", GGZPreferences
                        .getBoolean("GGZCards.SpinCards", true));
        closeGapCheckBox = new JCheckBox(
                "Close slot when a card is played to the trick",
                GGZPreferences.getBoolean("GGZCards.PackCards", true));
        beepOnTurnCheckBox = new JCheckBox("Beep on my turn", GGZPreferences
                .getBoolean("GGZCards.BeepOnTurn", true));
        cardGapSpinner = new JSpinner(new SpinnerNumberModel(GGZPreferences
                .getInt("GGZCards.CardGap", 17), 10, 50, 1));
        GridBagConstraints constraints = new GridBagConstraints();
        constraints.gridx = 0;
        constraints.gridy = 0;
        add(new JLabel("Gap between cards:"), constraints);
        constraints.gridx = 1;
        constraints.anchor = GridBagConstraints.WEST;
        constraints.insets.left = 4;
        add(cardGapSpinner, constraints);
        constraints.insets.left = 0;
        constraints.gridx = 0;
        constraints.gridy++;
        constraints.gridwidth = 2;
        constraints.gridy++;
        add(closeGapCheckBox, constraints);
        constraints.gridy++;
        add(spinCardsCheckBox, constraints);
        constraints.gridy++;
        add(beepOnTurnCheckBox, constraints);
    }

    public void apply() {
        GGZPreferences.putBoolean("GGZCards.SpinCards", spinCardsCheckBox
                .isSelected());
        GGZPreferences.putBoolean("GGZCards.PackCards", closeGapCheckBox
                .isSelected());
        GGZPreferences.putBoolean("GGZCards.BeepOnTurn", beepOnTurnCheckBox
                .isSelected());
        GGZPreferences.putInt("GGZCards.CardGap", ((Integer) cardGapSpinner
                .getValue()).intValue());
    }

    public void dispose() {
        // Nothing to dispose.
    }

    public String getTitle() {
        return "GGZCards";
    }

}
