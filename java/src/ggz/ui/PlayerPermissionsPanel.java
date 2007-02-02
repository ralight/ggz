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
import ggz.client.core.Server;
import ggz.common.AdminType;
import ggz.common.MutablePermSet;
import ggz.common.Perm;
import ggz.common.PermSet;
import ggz.common.PlayerType;

import java.awt.Component;
import java.awt.FlowLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;
import java.util.ArrayList;
import java.util.ResourceBundle;

import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.BorderFactory;
import javax.swing.DefaultListCellRenderer;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JPanel;

public class PlayerPermissionsPanel extends JPanel {

    protected static final ResourceBundle messages = ResourceBundle
            .getBundle("ggz.ui.messages");

    protected Player player;

    protected Server server = null;

    private JLabel handleTextLabel;

    private JComboBox playerTypeComboBox;

    private ArrayList permCheckBoxes = new ArrayList();

    private Action kickAction = new AbstractAction("Kick") {
        public void actionPerformed(ActionEvent event) {
            String reason = JOptionPane.showInputDialog((Component) event
                    .getSource(), "Enter the reason for kicking "
                    + player.get_name() + ":");
            if (reason != null) {
                try {
                    server.admin(AdminType.GGZ_ADMIN_KICK, player.get_name(),
                            reason);
                } catch (IOException e) {
                    handleException(e);
                }
            }
        }
    };

    private Action gagAction = new AbstractAction("Gag") {
        public void actionPerformed(ActionEvent event) {
            try {
                server.admin(AdminType.GGZ_ADMIN_GAG, player.get_name(), null);
            } catch (IOException e) {
                handleException(e);
            }
        }
    };

    private Action ungagAction = new AbstractAction("Ungag") {
        public void actionPerformed(ActionEvent event) {
            try {
                server
                        .admin(AdminType.GGZ_ADMIN_UNGAG, player.get_name(),
                                null);
            } catch (IOException e) {
                handleException(e);
            }
        }
    };

    private static String[] PERM_LABELS = new String[] { "Can join tables",
            "Can launch tables", "Can join rooms", "Room administrator",
            "Can make server announcements", "Is a bot",
            "No stats for this player", "Can administer tables",
            "Can send private messages at a table" };

    public PlayerPermissionsPanel(Player player) {
        super(new GridBagLayout());
        this.player = player;
        this.server = player.get_room().get_server();
        setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));
        JLabel handleLabel = new JLabel("Nickname:");
        JLabel playerTypeLabel = new JLabel("Player Type:");
        JLabel permissionsLabel = new JLabel("Permissions:");

        String handle = server.get_handle();
        Player me = player.get_room().get_player_by_name(handle);
        boolean iAmAdmin = me != null
                && me.get_type() == PlayerType.GGZ_PLAYER_ADMIN;
        setEnabled(iAmAdmin && player.get_type() != PlayerType.GGZ_PLAYER_GUEST);

        // Create player nickname label.
        handleTextLabel = new JLabel(player.get_name());

        // Create player type combo box.
        Object[] options;
        if (player.get_type() == PlayerType.GGZ_PLAYER_GUEST) {
            options = new Object[] { PlayerType.GGZ_PLAYER_GUEST };
        } else {
            options = new Object[] { PlayerType.GGZ_PLAYER_NORMAL,
                    PlayerType.GGZ_PLAYER_HOST, PlayerType.GGZ_PLAYER_ADMIN,
                    PlayerType.GGZ_PLAYER_BOT };
        }
        playerTypeComboBox = new JComboBox(options);
        playerTypeComboBox.setRenderer(new PlayerTypeCellRenderer());
        playerTypeComboBox.setEnabled(isEnabled());
        playerTypeComboBox.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                JComboBox cb = (JComboBox) e.getSource();
                PlayerType type = (PlayerType) cb.getSelectedItem();
                initCheckBoxes(PermSet.forPlayerType(type));
            }
        });

        // Create permission check boxes.
        ActionListener checkBoxListener = new ActionListener() {
            public void actionPerformed(ActionEvent event) {
                PermSet perms = createPermSetFromCheckBoxes();
                initPlayerTypeFromPermissions(perms);
                initCheckBoxes(perms);
            }
        };
        Perm[] perms = Perm.values();
        for (int i = 0, n = perms.length; i < n; i++) {
            Perm perm = perms[i];
            JCheckBox checkBox = new JCheckBox(getPermissionLabel(perm));
            checkBox.putClientProperty(Perm.class, perm);
            checkBox.addActionListener(checkBoxListener);
            checkBox.setEnabled(isEnabled());
            permCheckBoxes.add(checkBox);
        }

        GridBagConstraints constraints = new GridBagConstraints();
        constraints.anchor = GridBagConstraints.WEST;
        constraints.fill = GridBagConstraints.HORIZONTAL;
        constraints.gridx = 0;
        constraints.gridy = 0;
        constraints.insets.bottom = 4;
        add(handleLabel, constraints);
        constraints.gridx++;
        constraints.insets.left = 4;
        add(handleTextLabel, constraints);

        constraints.insets.left = 0;
        constraints.insets.bottom = 0;
        constraints.gridx = 0;
        constraints.gridy++;
        add(playerTypeLabel, constraints);
        constraints.gridx++;
        constraints.weightx = 1.0;
        constraints.insets.left = 4;
        add(playerTypeComboBox, constraints);

        constraints.insets.left = 0;
        constraints.weightx = 0.0;
        constraints.gridx = 0;
        constraints.gridy++;
        add(permissionsLabel, constraints);
        constraints.gridx++;
        for (int i = 0, n = permCheckBoxes.size(); i < n; i++) {
            add((JCheckBox) permCheckBoxes.get(i), constraints);
            constraints.gridy++;
        }

        constraints.gridx = 0;
        constraints.gridy++;
        add(new JLabel("Actions:"), constraints);
        constraints.gridx++;
        JPanel actionsPanel = new JPanel(new FlowLayout(FlowLayout.LEFT, 4, 0));
        JPanel actionsSizePanel = new JPanel(new GridLayout(1, 0, 4, 0));
        add(actionsPanel, constraints);
        actionsPanel.add(actionsSizePanel);
        actionsSizePanel.add(new JButton(kickAction));
        actionsSizePanel.add(new JButton(gagAction));
        actionsSizePanel.add(new JButton(ungagAction));

        // Sync UI with permissions.
        init();
    }

    protected void init() {
        PermSet perms;

        if (player != null) {
            perms = player.get_perms();
            initPlayerTypeFromPermissions(perms);
            initCheckBoxes(perms);
        }
    }

    protected void initCheckBoxes(PermSet perms) {
        for (int i = 0, n = permCheckBoxes.size(); i < n; i++) {
            JCheckBox permCheckBox = (JCheckBox) permCheckBoxes.get(i);
            Perm perm = (Perm) permCheckBox.getClientProperty(Perm.class);
            permCheckBox.setSelected(perms.isSet(perm));
        }
    }

    protected void initPlayerTypeFromPermissions(PermSet perms) {
        if (perms.isBot()) {
            playerTypeComboBox.setSelectedItem(PlayerType.GGZ_PLAYER_BOT);
        } else if (perms.isAdmin()) {
            playerTypeComboBox.setSelectedItem(PlayerType.GGZ_PLAYER_ADMIN);
        } else if (perms.isHost()) {
            playerTypeComboBox.setSelectedItem(PlayerType.GGZ_PLAYER_HOST);
        } else {
            playerTypeComboBox.setSelectedItem(PlayerType.GGZ_PLAYER_NORMAL);
        }
    }

    protected PermSet createPermSetFromCheckBoxes() {
        MutablePermSet perms = new MutablePermSet();
        for (int i = 0, n = permCheckBoxes.size(); i < n; i++) {
            JCheckBox permCheckBox = (JCheckBox) permCheckBoxes.get(i);
            Perm perm = (Perm) permCheckBox.getClientProperty(Perm.class);
            perms.set(perm, permCheckBox.isSelected());
        }
        return perms;
    }

    /**
     * Sends permissions to the server.
     */
    public void applyPermissions() throws IOException {
        if (server == null)
            return;

        String handle = handleTextLabel.getText();

        for (int i = 0, n = permCheckBoxes.size(); i < n; i++) {
            JCheckBox permCheckBox = (JCheckBox) permCheckBoxes.get(i);
            Perm perm = (Perm) permCheckBox.getClientProperty(Perm.class);
            server.set_perm(handle, perm, permCheckBox.isSelected());
        }
    }

    protected static String getPermissionLabel(Perm perm) {
        return PERM_LABELS[perm.ordinal()];
    }

    protected void handleException(Throwable e) {
        e.printStackTrace();
        JOptionPane.showMessageDialog(this, e.getMessage());
    }

    protected static class PlayerTypeCellRenderer extends
            DefaultListCellRenderer {

        public Component getListCellRendererComponent(JList list, Object value,
                int index, boolean isSelected, boolean cellHasFocus) {

            super.getListCellRendererComponent(list, value, index, isSelected,
                    cellHasFocus);

            PlayerType type = (PlayerType) value;
            setText(playerTypeToString(type));
            setIcon(IconFactory.getPlayerTypeIcon(type));
            return this;
        }

        private static String playerTypeToString(PlayerType type) {
            if (type == null) {
                return null;
            } else if (type == PlayerType.GGZ_PLAYER_NORMAL) {
                return messages.getString("PlayerType.Normal");
            } else if (type == PlayerType.GGZ_PLAYER_GUEST) {
                return messages.getString("PlayerType.Guest");
            } else if (type == PlayerType.GGZ_PLAYER_ADMIN) {
                return messages.getString("PlayerType.Admin");
            } else if (type == PlayerType.GGZ_PLAYER_HOST) {
                return messages.getString("PlayerType.Host");
            } else if (type == PlayerType.GGZ_PLAYER_BOT) {
                return messages.getString("PlayerType.Bot");
            }
            return type.toString();
        }

    }
}
