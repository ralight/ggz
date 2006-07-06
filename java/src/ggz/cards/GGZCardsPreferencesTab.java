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

    private JSpinner cardGapSpinner;

    public GGZCardsPreferencesTab() {
        super(new GridBagLayout());
        spinCardsCheckBox = new JCheckBox(
                "Spin cards when they are played to the trick?", GGZPreferences
                        .getBoolean("GGZCards.SpinCards", true));
        closeGapCheckBox = new JCheckBox(
                "Close slot when a card is played to the trick?",
                GGZPreferences.getBoolean("GGZCards.PackCards", true));
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
    }

    public void apply() {
        GGZPreferences.putBoolean("GGZCards.SpinCards", spinCardsCheckBox
                .isSelected());
        GGZPreferences.putBoolean("GGZCards.PackCards", closeGapCheckBox
                .isSelected());
        GGZPreferences.putInt("GGZCards.CardGap", ((Integer) cardGapSpinner
                .getValue()).intValue());
    }

    public String getTitle() {
        return "GGZCards";
    }

}
