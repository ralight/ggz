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

import ggz.ui.GGZDialog;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.FlowLayout;
import java.awt.Frame;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Insets;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComponent;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JTabbedPane;
import javax.swing.JTextArea;

public class OptionDialog extends GGZDialog implements ItemListener {
    private JTabbedPane tabbedPane;

    private JPanel buttonSizePanel;

    private JPanel buttonFlowPanel;

    private JButton okButton;

    /** true if OK was click and false if Cancel was clicked. */
    private boolean result;

    private JComponent[][] checkboxes;

    private int[] values;

    public OptionDialog(Frame owner, String title, boolean modal) {
        super(owner, title, modal);
        // Setup the OK and cancel buttons
        buttonSizePanel = new JPanel(new GridLayout(1, 2, 4, 4));
        buttonFlowPanel = new JPanel(new FlowLayout(FlowLayout.CENTER));
        okButton = new JButton("OK");

        okButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                onOKClick();
            }
        });
        buttonSizePanel.add(okButton);
        buttonFlowPanel.add(buttonSizePanel);
        getContentPane().add(buttonFlowPanel, BorderLayout.SOUTH);
        // Add options container in center, options controls get added in
        // init().
        tabbedPane = new JTabbedPane();
        getContentPane().add(tabbedPane, BorderLayout.CENTER);
        addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {
                onOKClick();
            }
        });
        getRootPane().setDefaultButton(okButton);
    }

    private void init(String[] types, String[] descs, int[] defaults,
            String[][] option_choices) {
        checkboxes = new JComponent[descs.length][];
        values = defaults;

        Insets descInsets = new Insets(0, 4, 0, 0);
        Insets choiceInsets = new Insets(0, 24, 0, 0);
        GridBagConstraints constraints = new GridBagConstraints();
        constraints.anchor = GridBagConstraints.WEST;

        for (int i = 0; i < types.length; i++) {
            JPanel optionPanel;
            int indexOfTab = tabbedPane.indexOfTab(types[i]);
            if (indexOfTab < 0) {
                // Tab doesn't exist yet.
                optionPanel = new JPanel(new GridBagLayout());
                tabbedPane.addTab(types[i], optionPanel);
            } else {
                optionPanel = (JPanel) tabbedPane.getComponentAt(indexOfTab);
            }
            JTextArea label = new JTextArea();
            int numChoices = option_choices[i].length;
            ButtonGroup group = null;

            label.setEditable(false);
            label.setOpaque(false);
            label.setText(descs[i]);
            constraints.gridy++;
            constraints.insets = descInsets;
            optionPanel.add(label, constraints);

            if (numChoices > 1) {
                group = new ButtonGroup();
            }

            checkboxes[i] = new JComponent[numChoices];
            if (numChoices > 1) {
                for (int j = 0; j < numChoices; j++) {
                    boolean state = (j == defaults[i]);
                    JRadioButton radio = new JRadioButton(option_choices[i][j],
                            state);
                    checkboxes[i][j] = radio;
                    group.add(radio);
                    constraints.gridy++;
                    constraints.insets = choiceInsets;
                    optionPanel.add(radio, constraints);
                    radio.addItemListener(this);
                }
            } else {
                boolean state = defaults[i] > 0;
                JCheckBox checkBox = new JCheckBox(option_choices[i][0], state);
                checkboxes[i][0] = checkBox;
                constraints.gridy++;
                constraints.insets = choiceInsets;
                optionPanel.add(checkBox, constraints);
                checkBox.addItemListener(this);

            }
        }
    }

    protected void onOKClick() {
        result = true;
        dispose();
    }

    public void itemStateChanged(ItemEvent e) {
        boolean isCheckbox = e.getSource() instanceof JCheckBox;

        if (e.getStateChange() == ItemEvent.SELECTED) {
            // Find the checkbox and set the corresponding value.
            for (int i = 0; i < checkboxes.length; i++) {
                for (int j = 0; j < checkboxes[i].length; j++) {
                    if (checkboxes[i][j] == e.getItemSelectable()) {
                        values[i] = isCheckbox ? 1 : j;
                        return;
                    }
                }
            }
        } else {
            if (isCheckbox) {
                // It's a checkbox and not a radio so we unset the value.
                for (int i = 0; i < checkboxes.length; i++) {
                    if (checkboxes[i][0] == e.getItemSelectable()) {
                        values[i] = 0;
                        return;
                    }
                }
            }
        }
    }

    public static boolean show(Component parent, String[] types,
            String[] descs, int[] defaults, String[][] option_choices) {
        Frame frame = JOptionPane.getFrameForComponent(parent);
        OptionDialog dialog = new OptionDialog(frame, "Options", true);
        dialog.init(types, descs, defaults, option_choices);
        dialog.pack();

        // Set the location relative to the parent frame.
        Point location = frame.getLocation();
        location.x += 10;
        location.y += 10;

        dialog.setLocation(location);
        dialog.repositionIfNecessary();
        dialog.setVisible(true);
        dialog.okButton.requestFocus();
        return dialog.result;
    }
}
