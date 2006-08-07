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
import ggz.client.core.Player;
import ggz.client.core.Room;
import ggz.client.core.Table;
import ggz.common.PlayerType;
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
import java.awt.event.KeyEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.IOException;
import java.text.MessageFormat;
import java.util.Arrays;
import java.util.ResourceBundle;

import javax.swing.BorderFactory;
import javax.swing.ButtonGroup;
import javax.swing.DefaultListCellRenderer;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JTextField;
import javax.swing.KeyStroke;
import javax.swing.ListCellRenderer;

// TODO Localise this
public class SeatAllocationDialog extends JDialog implements ItemListener {
    protected static final ResourceBundle messages = ResourceBundle
            .getBundle("ggz.ui.messages");

    private static final String JOIN_ME = messages
            .getString("SeatAllocationDialog.DefaultTableDescription.JoinMe");

    private static final String I_PLAY_ALONE = messages
            .getString("SeatAllocationDialog.DefaultTableDescription.IPlayAlone");

    private GameType gameType;

    private Table table;

    protected Room room;

    private JPanel buttonSizePanel;

    private JPanel buttonFlowPanel;

    private JButton okButton;

    private JButton cancelButton;

    private JPanel centerPanel;

    protected JRadioButton soloPlayButton;

    private JRadioButton multiPlayButton;

    protected JRadioButton advancedPlayButton;

    private JLabel tableDescriptionLabel;

    private JTextField tableDescriptionTextField;

    private JLabel numberOfPlayersLabel;

    private JComboBox numberOfPlayersComboBox;

    private JPanel advancedPanel;

    private JPanel reserveSeatsPanel;

    protected static ListCellRenderer playerRenderer = new PlayerListCellRenderer();

    public SeatAllocationDialog(Frame frame, String title, Room room) {
        super(frame, title, true);
        this.room = room;
        this.gameType = room.get_gametype();
        this.setResizable(false);

        // Closing the dialog is the same as clicking cancel.
        addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent event) {
                onCancelClick();
            }
        });

        // Set up the OK and Cancel buttons.
        buttonSizePanel = new JPanel(new GridLayout(1, 2, 4, 4));
        buttonFlowPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
        okButton = new JButton(messages
                .getString("SeatAllocationDialog.Button.OK"));
        okButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
                onOKClick();
            }
        });
        ActionListener cancelAction = new ActionListener() {
            public void actionPerformed(ActionEvent event) {
                onCancelClick();
            }
        };
        cancelButton = new JButton(messages
                .getString("SeatAllocationDialog.Button.Cancel"));
        cancelButton.addActionListener(cancelAction);
        buttonSizePanel.add(okButton);
        buttonSizePanel.add(cancelButton);
        buttonFlowPanel.add(buttonSizePanel);
        getContentPane().add(buttonFlowPanel, BorderLayout.SOUTH);

        // Let escape key close dialog.
        this.getRootPane().registerKeyboardAction(cancelAction,
                KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0),
                JComponent.WHEN_IN_FOCUSED_WINDOW);

        centerPanel = new JPanel(new GridBagLayout());
        centerPanel.setBorder(BorderFactory.createEmptyBorder(6, 6, 6, 6));

        boolean isSoloPlayPossible = gameType.get_max_bots() > 0;
        ButtonGroup soloOrMultiGroup = new ButtonGroup();
        GridBagConstraints constraints = new GridBagConstraints();
        getContentPane().add(centerPanel, BorderLayout.CENTER);
        constraints.gridx = 0;
        constraints.gridy = 0;
        constraints.anchor = GridBagConstraints.WEST;
        constraints.gridwidth = 2;
        if (isSoloPlayPossible) {
            soloPlayButton = new JRadioButton(
                    messages
                            .getString("SeatAllocationDialog.Radio.PlayAgainstComputer"));
            soloPlayButton.addItemListener(this);
            soloOrMultiGroup.add(soloPlayButton);
            centerPanel.add(soloPlayButton, constraints);
        }

        multiPlayButton = new JRadioButton(messages
                .getString("SeatAllocationDialog.Radio.StartMultiplayerGame"));
        multiPlayButton.addItemListener(this);
        soloOrMultiGroup.add(multiPlayButton);
        constraints.gridy += 1;
        centerPanel.add(multiPlayButton, constraints);

        advancedPlayButton = new JRadioButton(messages
                .getString("SeatAllocationDialog.Radio.StartAdvancedGame"));
        advancedPlayButton.addItemListener(this);
        soloOrMultiGroup.add(advancedPlayButton);
        constraints.gridy += 1;
        centerPanel.add(advancedPlayButton, constraints);

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
            numberOfPlayersComboBox.addItemListener(this);
            constraints.insets.left = 5;
            constraints.gridx = 0;
            constraints.gridy += 1;
            centerPanel.add(numberOfPlayersLabel, constraints);
            constraints.gridx = 1;
            centerPanel.add(numberOfPlayersComboBox, constraints);
        }

        // Panel for reserving seats.
        reserveSeatsPanel = new JPanel(new GridLayout(getNumSeats(), 0));
        for (int seatNum = 0; seatNum < getNumSeats(); seatNum++) {
            reserveSeatsPanel.add(new SeatPanel(seatNum));
        }
        advancedPanel = new JPanel(new BorderLayout());
        // JLabel label = new JLabel(
        // "You can only reserve seats for registered players.");
        // label.setFont(label.getFont().deriveFont(Font.PLAIN).deriveFont(
        // Font.ITALIC));
        // advancedPanel.add(label, BorderLayout.NORTH);
        advancedPanel.add(reserveSeatsPanel, BorderLayout.CENTER);
        constraints.gridx = 0;
        constraints.gridy += 1;
        constraints.gridwidth = 2;
        constraints.insets.left = 5;
        centerPanel.add(advancedPanel, constraints);

        // Table description, label and text field.
        tableDescriptionLabel = new JLabel(messages
                .getString("SeatAllocationDialog.Label.TableDescription"));
        tableDescriptionTextField = new JTextField(I_PLAY_ALONE, 20);
        TextPopupMenu.enableFor(tableDescriptionTextField);
        constraints.insets.left = 5;
        constraints.insets.top = 5;
        // constraints.gridx = 0;
        constraints.gridy += 1;
        // constraints.gridwidth = 2;
        centerPanel.add(tableDescriptionLabel, constraints);
        constraints.insets.top = 5;
        constraints.gridy += 1;
        constraints.weightx = 1.0;
        constraints.fill = GridBagConstraints.HORIZONTAL;
        centerPanel.add(tableDescriptionTextField, constraints);

        if (isSoloPlayPossible) {
            soloPlayButton.setSelected(true);
        } else {
            multiPlayButton.setSelected(true);
        }
        getRootPane().setDefaultButton(okButton);
    }

    protected void onOKClick() {
        String description = tableDescriptionTextField.getText();
        int numSeats = getNumSeats();

        table = new Table(gameType, description, numSeats);
        try {
            for (int seatNum = 0; seatNum < reserveSeatsPanel
                    .getComponentCount(); seatNum++) {
                SeatPanel seat = (SeatPanel) reserveSeatsPanel
                        .getComponent(seatNum);
                if (seat.isValidInput()) {
                    table.set_seat(seatNum, seat.getSeatType(), seat
                            .getSeatName());
                } else {
                    JOptionPane
                            .showMessageDialog(
                                    this,
                                    messages
                                            .getString("SeatAllocationDialog.Message.BlankNickname"));
                    return;
                }
            }
        } catch (IOException ex) {
            // Ignore it since this should never happen
            ex.printStackTrace();
        }
        dispose();
    }

    protected void onCancelClick() {
        table = null;
        dispose();
    }

    public void itemStateChanged(ItemEvent e) {
        // We only care about selections.
        if (e.getStateChange() == ItemEvent.DESELECTED) {
            return;
        }

        if (e.getSource() == numberOfPlayersComboBox) {
            int numSeats = getNumSeats();
            int actualNumSeats = reserveSeatsPanel.getComponentCount();
            if (numSeats > actualNumSeats) {
                // We to add more seats.
                for (int seatNum = actualNumSeats; seatNum < numSeats; seatNum++) {
                    JPanel seatPanel = new SeatPanel(seatNum);
                    reserveSeatsPanel.add(seatPanel);
                }
            } else if (numSeats < actualNumSeats) {
                // We need to remove seats.
                for (int componentIndex = actualNumSeats - 1; componentIndex >= numSeats; componentIndex--) {
                    reserveSeatsPanel.remove(componentIndex);
                }
            }
            ((GridLayout) reserveSeatsPanel.getLayout()).setRows(numSeats);
        } else if (e.getSource() == advancedPlayButton) {
            advancedPanel.setVisible(true);
        } else {
            advancedPanel.setVisible(false);
        }

        for (int seatNum = 1; seatNum < reserveSeatsPanel.getComponentCount(); seatNum++) {
            SeatPanel seat = (SeatPanel) reserveSeatsPanel
                    .getComponent(seatNum);
            if (e.getSource() == soloPlayButton) {
                seat.setSeatType(SeatType.GGZ_SEAT_BOT);
            } else if (e.getSource() == multiPlayButton) {
                seat.setSeatType(SeatType.GGZ_SEAT_OPEN);
            } else if (e.getSource() == advancedPlayButton) {
                seat.setSeatType(SeatType.GGZ_SEAT_RESERVED);
            }
        }
        // Only change the text if it hasn't changed.
        String text = tableDescriptionTextField.getText();
        if (JOIN_ME.equals(text) || I_PLAY_ALONE.equals(text)) {
            if (soloPlayButton != null && soloPlayButton.isSelected()) {
                tableDescriptionTextField.setText(I_PLAY_ALONE);
            } else {
                tableDescriptionTextField.setText(JOIN_ME);
            }
        }
        pack();
    }

    public static Table getTableSeatAllocation(Component parent, Room room) {
        Frame owner = JOptionPane.getFrameForComponent(parent);
        SeatAllocationDialog dialog = new SeatAllocationDialog(owner, messages
                .getString("SeatAllocationDialog.Title"), room);
        dialog.pack();
        dialog.setLocationRelativeTo(owner);
        dialog.setVisible(true);
        return dialog.table;
    }

    public int getNumSeats() {
        int numSeats = gameType.get_max_players();

        if (numberOfPlayersComboBox != null) {
            numSeats = ((Integer) numberOfPlayersComboBox.getSelectedItem())
                    .intValue();
        }
        return numSeats;
    }

    private class SeatPanel extends JPanel implements ItemListener {
        private JRadioButton computerButton;

        private JRadioButton anyoneButton;

        private JRadioButton reservedButton;

        private JComboBox reservedCombo;

        SeatPanel(int seatNum) {
            super(new FlowLayout(FlowLayout.LEFT, 0, 0));
            ButtonGroup buttonGroup = new ButtonGroup();

            add(new JLabel(MessageFormat.format(messages
                    .getString("SeatAllocationDialog.Label.Seat"),
                    new Object[] { String.valueOf(seatNum + 1) })));

            if (soloPlayButton != null) {
                computerButton = new JRadioButton(messages
                        .getString("SeatAllocationDialog.Radio.Computer"));
                computerButton.addItemListener(this);
                add(computerButton);
                buttonGroup.add(computerButton);
            }

            anyoneButton = new JRadioButton(messages
                    .getString("SeatAllocationDialog.Radio.Anyone"));
            anyoneButton.addItemListener(this);
            add(anyoneButton);
            buttonGroup.add(anyoneButton);

            reservedButton = new JRadioButton(messages
                    .getString("SeatAllocationDialog.Radio.ReservedFor"));
            reservedButton.addItemListener(this);
            add(reservedButton);
            buttonGroup.add(reservedButton);

            // Fill the combo with players.
            Player[] players = room.get_players();
            Arrays.sort(players, Player.SORT_BY_NAME);
            reservedCombo = new JComboBox(players);
            reservedCombo.setEditable(true);
            reservedCombo.setEnabled(false);
            reservedCombo.setRenderer(playerRenderer);
            add(reservedCombo);

            if (seatNum == 0) {
                reservedCombo.setSelectedItem(room.get_server().get_handle());
                reservedButton.setSelected(true);
            } else if (soloPlayButton != null && soloPlayButton.isSelected()) {
                computerButton.setSelected(true);
            } else if (advancedPlayButton.isSelected()) {
                reservedButton.setSelected(true);
            } else {
                anyoneButton.setSelected(true);
            }
        }

        public void setSeatType(SeatType seatType) {
            if (seatType == SeatType.GGZ_SEAT_BOT) {
                computerButton.setSelected(true);
                setSeatName(null);
            } else if (seatType == SeatType.GGZ_SEAT_OPEN) {
                anyoneButton.setSelected(true);
                setSeatName(null);
            } else if (seatType == SeatType.GGZ_SEAT_RESERVED) {
                reservedButton.setSelected(true);
            } else {
                throw new IllegalArgumentException("Seat type not supported.");
            }
        }

        public SeatType getSeatType() {
            if (computerButton != null && computerButton.isSelected()) {
                return SeatType.GGZ_SEAT_BOT;
            } else if (anyoneButton.isSelected()) {
                return SeatType.GGZ_SEAT_OPEN;
            } else if (reservedButton.isSelected()) {
                return SeatType.GGZ_SEAT_RESERVED;
            } else {
                throw new IllegalStateException("No radio button is selected!");
            }
        }

        public void setSeatName(String name) {
            reservedCombo.setSelectedItem(name);
        }

        public String getSeatName() {
            Object player = reservedCombo.getSelectedItem();
            if (player == null) {
                return null;
            }
            return String.valueOf(player);
        }

        public boolean isValidInput() {
            if (getSeatType() == SeatType.GGZ_SEAT_RESERVED) {
                return getSeatName() != null;
            }
            return true;
        }

        public void itemStateChanged(ItemEvent e) {
            if (e.getSource() == reservedButton) {
                reservedCombo.setEnabled(true);
            } else {
                reservedCombo.setEnabled(false);
                setSeatName(null);
            }
        }
    }

    protected static class PlayerListCellRenderer extends DefaultListCellRenderer {
        public Component getListCellRendererComponent(JList list, Object value,
                int index, boolean isSelected, boolean cellHasFocus) {
            super.getListCellRendererComponent(list, value, index, isSelected,
                    cellHasFocus);
            Player p = (Player) value;
            setText(p.get_name());
            PlayerType type = p.get_type();
            setIcon(IconFactory.getPlayerTypeIcon(type));
            return this;
        }

    }
}
