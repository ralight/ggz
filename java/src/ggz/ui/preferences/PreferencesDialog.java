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
import java.awt.Component;
import java.awt.FlowLayout;
import java.awt.Frame;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.util.logging.Level;
import java.util.logging.Logger;

import javax.swing.*;

public class PreferencesDialog extends JDialog {
    private static final Logger log = Logger.getLogger(PreferencesDialog.class
            .getName());

    private JTabbedPane tabbedPane;

    private JPanel buttonPanel;

    public PreferencesDialog(Frame owner) {
        super(owner, "Preferences");
        this.tabbedPane = new JTabbedPane();
        this.getContentPane().add(this.tabbedPane, BorderLayout.CENTER);
        JButton okButton = new JButton(new OKAction());
        JButton cancelButton = new JButton(new CancelAction());
        JButton applyButton = new JButton(new ApplyAction());
        JPanel buttonSizePanel = new JPanel(new GridLayout(1, 0, 4, 4));
        buttonSizePanel.add(okButton);
        buttonSizePanel.add(cancelButton);
        buttonSizePanel.add(applyButton);
        this.buttonPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT, 5, 5));
        this.buttonPanel.add(buttonSizePanel);
        this.getContentPane().add(this.buttonPanel, BorderLayout.SOUTH);

        // This is effectively the same as clicking cancel.
        this.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
        
        // OK is the default button.
        this.getRootPane().setDefaultButton(okButton);

        // Let escape key close dialog.
        this.getRootPane().registerKeyboardAction(cancelButton.getAction(),
                KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0),
                JComponent.WHEN_IN_FOCUSED_WINDOW);
    }

    protected void apply() {
        int tabCount = tabbedPane.getTabCount();
        for (int tabIndex = 0; tabIndex < tabCount; tabIndex++) {
            PreferencesTab tab = (PreferencesTab) tabbedPane.getComponentAt(tabIndex);
            tab.apply();
        }
    }

    protected void close() {
        setVisible(false);
        dispose();
    }

    public static void showPreferences(Component owner, String[] panels) {
        PreferencesDialog dialog = new PreferencesDialog(JOptionPane.getFrameForComponent(owner));
        for (int i = 0; i < panels.length; i++) {
            try {
                Class c = Class.forName(panels[i]);
                PreferencesTab panel = (PreferencesTab) c.newInstance();
                dialog.tabbedPane.addTab(panel.getTitle(), (Component) panel);
            } catch (Throwable e) {
                // Ignore but dump for debugging.
                log.log(Level.WARNING, "Could not create preferences panel", e);
            }
        }
        dialog.pack();
        dialog.setModal(true);
        dialog.setLocationRelativeTo(owner);
        dialog.setVisible(true);
    }

    private class OKAction extends AbstractAction {
        public OKAction() {
            super("OK");
        }

        public void actionPerformed(ActionEvent event) {
            apply();
            close();
        }
    }

    private class ApplyAction extends AbstractAction {
        public ApplyAction() {
            super("Apply");
        }

        public void actionPerformed(ActionEvent event) {
            apply();
        }
    }

    private class CancelAction extends AbstractAction {
        public CancelAction() {
            super("Cancel");
        }

        public void actionPerformed(ActionEvent event) {
            close();
        }
    }
}
