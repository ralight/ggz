/*
 * Copyright (C) 2007  Helg Bredow
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

import ggz.client.core.Player;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.FlowLayout;
import java.awt.Frame;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.security.AccessControlException;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.WindowConstants;

public class PlayerAdminDialog extends JDialog {
    protected JButton closeButton;

    protected JButton applyButton;

    protected PlayerPermissionsPanel permissionsPanel;

    protected PlayerAdminDialog(Frame owner, Player player) {
        super(owner);

        closeButton = new JButton("Close");
        closeButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
                setVisible(false);
                dispose();
            }
        });

        applyButton = new JButton("Apply");
        applyButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
                try {
                    permissionsPanel.applyPermissions();
                } catch (AccessControlException e) {
                    handleException(e);
                } catch (IOException e) {
                    handleException(e);
                }
            }
        });

        JPanel buttonPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
        buttonPanel.setBorder(BorderFactory.createMatteBorder(1, 0, 0, 0,
                Color.BLACK));
        buttonPanel.add(applyButton);
        buttonPanel.add(closeButton);

        permissionsPanel = new PlayerPermissionsPanel(player);

        // Only admins can set permissions.
        applyButton.setVisible(permissionsPanel.isEnabled());

        getContentPane().add(permissionsPanel, BorderLayout.CENTER);
        getContentPane().add(buttonPanel, BorderLayout.SOUTH);
    }

    protected void handleException(Throwable e) {
        e.printStackTrace();
        JOptionPane.showMessageDialog(this, e.getMessage());
    }

    public static PlayerAdminDialog showDialog(Component parentComponent,
            Player player) {

        if (player == null)
            throw new NullPointerException("player cannot be null");

        Frame parentFrame = JOptionPane.getFrameForComponent(parentComponent);
        PlayerAdminDialog dialog = new PlayerAdminDialog(parentFrame, player);
        dialog.setModal(false);
        dialog.setTitle("Player Admin");
        dialog.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
        dialog.pack();
        dialog.setVisible(true);
        return dialog;
    }
}
