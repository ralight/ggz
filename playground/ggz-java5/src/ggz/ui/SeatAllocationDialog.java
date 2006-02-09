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
package ggz.ui;

import ggz.client.core.GameType;
import ggz.client.core.Table;
import ggz.common.SeatType;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.FlowLayout;
import java.awt.Frame;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.IOException;
import java.util.ResourceBundle;

import javax.swing.BorderFactory;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JTextField;

public class SeatAllocationDialog extends JDialog implements ItemListener {
    private static final ResourceBundle messages = ResourceBundle
            .getBundle("ggz.ui.messages");

    private static final String JOIN_ME = messages
            .getString("SeatAllocationDialog.DefaultTableDescription.JoinMe");

    private static final String I_PLAY_ALONE = messages
            .getString("SeatAllocationDialog.DefaultTableDescription.IPlayAlone");

    private GameType gameType;

    private Table table;

    private JPanel buttonSizePanel;

    private JPanel buttonFlowPanel;

    private JButton okButton;

    private JButton cancelButton;

    private JPanel centerPanel;

    private JRadioButton soloPlayButton;

    private JRadioButton multiPlayButton;

    private JLabel tableDescriptionLabel;

    private JTextField tableDescriptionTextField;

    private JLabel numberOfPlayersLabel;

    private JComboBox numberOfPlayersComboBox;

    public SeatAllocationDialog(Frame frame, String title, GameType gameType) {
        super(frame, title, true);
        this.gameType = gameType;

        // Closing the dialog is the same as clicking cancel.
        addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent event) {
                onCancelClick();
            }
        });

        // Set up the OK and Cancel buttons.
        buttonSizePanel = new JPanel(new GridLayout(1, 2, 4, 4));
        buttonFlowPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
        okButton = new JButton(messages.getString("SeatAllocationDialog.Button.OK"));
        okButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
                onOKClick();
            }
        });
        cancelButton = new JButton(messages
                .getString("SeatAllocationDialog.Button.Cancel"));
        cancelButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
                onCancelClick();
            }
        });
        buttonSizePanel.add(okButton);
        buttonSizePanel.add(cancelButton);
        buttonFlowPanel.add(buttonSizePanel);
        getContentPane().add(buttonFlowPanel, BorderLayout.SOUTH);

        centerPanel = new JPanel(new GridBagLayout());
        centerPanel.setBorder(BorderFactory.createEmptyBorder(6, 6, 6, 6));

        boolean isSoloPlayPossible = gameType.get_max_bots() > 0;
        GridBagConstraints constraints = new GridBagConstraints();
        add(centerPanel, BorderLayout.CENTER);
        constraints.gridx = 0;
        constraints.gridy = 0;
        constraints.anchor = GridBagConstraints.WEST;
        if (isSoloPlayPossible) {
            ButtonGroup soloOrMultiGroup = new ButtonGroup();
            soloPlayButton = new JRadioButton(messages
                    .getString("SeatAllocationDialog.Radio.PlayAgainstComputer"));
            soloPlayButton.addItemListener(this);
            multiPlayButton = new JRadioButton(messages
                    .getString("SeatAllocationDialog.Radio.StartMultiplayerGame"));
            multiPlayButton.addItemListener(this);
            soloOrMultiGroup.add(soloPlayButton);
            soloOrMultiGroup.add(multiPlayButton);
            constraints.gridwidth = 2;
            centerPanel.add(soloPlayButton, constraints);
            constraints.gridy = 1;
            centerPanel.add(multiPlayButton, constraints);
        }
        constraints.gridwidth = 1;
        // Card games only support a max of 4 players at the moment.
        int maxPlayers = Math.min(4, gameType.get_max_players());
        if (maxPlayers > gameType.get_min_players()) {
            numberOfPlayersLabel = new JLabel(messages
                    .getString("SeatAllocationDialog.Label.NumberOfPlayers"));
            numberOfPlayersComboBox = new JComboBox();
            for (int i = gameType.get_min_players(); i <= maxPlayers; i++) {
                Object item = new Integer(i);
                numberOfPlayersComboBox.addItem(item);
                // Default to 4 players...a bit dumb since it assumes we are
                // playing cards but it works for now.
                if (i == 4) {
                    numberOfPlayersComboBox.setSelectedItem(item);
                }
            }
            constraints.insets.left = 5;
            constraints.gridx = 0;
            constraints.gridy = 2;
            centerPanel.add(numberOfPlayersLabel, constraints);
            constraints.gridx = 1;
            centerPanel.add(numberOfPlayersComboBox, constraints);
        }

        tableDescriptionLabel = new JLabel(messages
                .getString("SeatAllocationDialog.Label.TableDescription"));
        tableDescriptionTextField = new JTextField(I_PLAY_ALONE, 20);
        constraints.insets.top = 5;
        constraints.gridx = 0;
        constraints.gridy = 3;
        constraints.gridwidth = 2;
        centerPanel.add(tableDescriptionLabel, constraints);
        constraints.insets.top = 5;
        constraints.gridy = 4;
        constraints.weightx = 1.0;
        constraints.fill = GridBagConstraints.HORIZONTAL;
        centerPanel.add(tableDescriptionTextField, constraints);

        if (isSoloPlayPossible) {
            soloPlayButton.setSelected(true);
        }
    }

    private void onOKClick() {
        String description = tableDescriptionTextField.getText();
        int numSeats = gameType.get_max_players();

        if (numberOfPlayersComboBox != null) {
            numSeats = ((Integer) numberOfPlayersComboBox.getSelectedItem())
                    .intValue();
        }
        table = new Table(gameType, description, numSeats);
        try {
            if (soloPlayButton.isSelected()) {
                table.set_seat(0, SeatType.GGZ_SEAT_OPEN, null);
                for (int seat_num = 1; seat_num < numSeats; seat_num++) {
                    table.set_seat(seat_num, SeatType.GGZ_SEAT_BOT, null);
                }
            } else {
                for (int seat_num = 0; seat_num < numSeats; seat_num++) {
                    table.set_seat(seat_num, SeatType.GGZ_SEAT_OPEN, null);
                }
            }
        } catch (IOException ex) {
            // Ignore it since this should never happen
            ex.printStackTrace();
        }
        dispose();
    }

    private void onCancelClick() {
        table = null;
        dispose();
    }

    public void itemStateChanged(ItemEvent e) {
        // Only change the text if it hasn't changed.
        String text = tableDescriptionTextField.getText();
        if (JOIN_ME.equals(text) || I_PLAY_ALONE.equals(text)) {
            if (multiPlayButton.isSelected()) {
                tableDescriptionTextField.setText(JOIN_ME);
            } else if (soloPlayButton.isSelected()) {
                tableDescriptionTextField.setText(I_PLAY_ALONE);
            }
        }
    }

    public static Table getTableSeatAllocation(Component parent,
            GameType gameType) {
        Frame owner = JOptionPane.getFrameForComponent(parent);
        SeatAllocationDialog dialog = new SeatAllocationDialog(owner, messages
                .getString("SeatAllocationDialog.Title"), gameType);
        dialog.pack();
        dialog.setLocationRelativeTo(owner);
        dialog.setVisible(true);
        return dialog.table;
    }
}
