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
package ggz.games;

import ggz.client.mod.ModGame;
import ggz.client.mod.SpectatorSeat;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.IOException;

import javax.swing.AbstractAction;
import javax.swing.BorderFactory;
import javax.swing.DefaultListModel;
import javax.swing.JList;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;

public class SpectatorListPanel extends JPanel {
    protected JList spectatorList;

    protected JPopupMenu popup;

    protected ModGame ggzMod;

    protected SpectatorBootAction bootAction;

    public SpectatorListPanel() {
        super(new BorderLayout());
        spectatorList = new JList(new DefaultListModel());
        spectatorList.setOpaque(false);
        spectatorList.setBorder(BorderFactory.createCompoundBorder(BorderFactory
                .createEmptyBorder(4, 4, 4, 4), BorderFactory
                .createTitledBorder("Spectators")));
        spectatorList.addMouseListener(new PopupListener());
        bootAction = new SpectatorBootAction();
        popup = new JPopupMenu("Spectator");
        popup.add(new JMenuItem(bootAction));
        add(spectatorList, BorderLayout.CENTER);
    }

    public void setMod(ModGame mod) {
        this.ggzMod = mod;
    }

    public void addSpectator(SpectatorSeat seat) {
        ((DefaultListModel) spectatorList.getModel()).addElement(seat);
    }

    public void removeSpectator(SpectatorSeat seat) {
        ((DefaultListModel) spectatorList.getModel()).removeElement(seat);
    }

    private class PopupListener extends MouseAdapter {
        public void mousePressed(MouseEvent event) {
            maybeShowPopup(event);
        }

        public void mouseReleased(MouseEvent event) {
            maybeShowPopup(event);
        }

        public void maybeShowPopup(MouseEvent event) {
            Component component = (Component) event.getSource();
            if (popup.isPopupTrigger(event)) {
                bootAction.setEnabled(spectatorList.getSelectedValue() != null);
                popup.show(component, event.getX(), event.getY());
            }
        }
    }

    protected class SpectatorBootAction extends AbstractAction {

        public SpectatorBootAction() {
            super("Boot this spectator from the game");
        }

        public void actionPerformed(ActionEvent event) {
            SpectatorSeat spectator = (SpectatorSeat) spectatorList
                    .getSelectedValue();
            if (JOptionPane.showConfirmDialog(SpectatorListPanel.this,
                    "Are you sure you want to boot " + spectator.get_name()
                            + " from the game?", "Boot",
                    JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION) {
                try {
                    ggzMod.request_boot(spectator.get_name());
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
