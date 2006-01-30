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

import ggz.client.core.LoginType;
import ggz.client.core.Server;

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.io.IOException;

import javax.swing.AbstractAction;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JPasswordField;
import javax.swing.JRadioButton;
import javax.swing.JTextField;
import javax.swing.SwingConstants;

public class LoginPanel extends JPanel {
    private Server server;

    protected JTextField handleTextField;

    protected JPasswordField passwordField;

    protected JTextField emailTextField;

    protected JLabel handleLabel;

    protected JLabel passwordLabel;

    protected JLabel emailLabel;

    private JRadioButton normalLoginRadio;

    private JRadioButton guestLoginRadio;

    private JRadioButton newLoginRadio;

    private JButton loginButton;

    public LoginPanel(Server server) {
        super(new GridBagLayout());
        this.server = server;

        // Create all the components
        handleLabel = new JLabel(
                "<HTML>Nickname<FONT color=red>*</FONT></HTML>",
                SwingConstants.RIGHT);
        passwordLabel = new JLabel(
                "<HTML>Password<FONT color=red>*</FONT></HTML>",
                SwingConstants.RIGHT);
        emailLabel = new JLabel("Email", SwingConstants.RIGHT);
        handleTextField = new JTextField(20);
        passwordField = new JPasswordField(20);
        emailTextField = new JTextField(null, 20);
        normalLoginRadio = new JRadioButton(new ChooseNormalLoginAction());
        guestLoginRadio = new JRadioButton(new ChooseGuestLoginAction());
        newLoginRadio = new JRadioButton(new ChooseNewLoginAction());
        loginButton = new JButton(new ConnectToServerAction());

        // Set up the components
        setOpaque(false);
        loginButton.setOpaque(false);
        normalLoginRadio.setOpaque(false);
        guestLoginRadio.setOpaque(false);
        newLoginRadio.setOpaque(false);
        guestLoginRadio.setSelected(true);
        passwordField.setFont(handleTextField.getFont());

        // Simulate radion selection to initialise radio buttons' hidden state.
        guestLoginRadio.getAction().actionPerformed(null);

        // Group the radio buttons.
        ButtonGroup group = new ButtonGroup();
        group.add(normalLoginRadio);
        group.add(guestLoginRadio);
        group.add(newLoginRadio);

        // Add all the components
        GridBagConstraints constraints = new GridBagConstraints();
        constraints.anchor = GridBagConstraints.WEST;
        constraints.insets.top = 4;
        constraints.insets.left = 4;
        constraints.gridx = 0;
        constraints.gridy = 0;

        add(handleLabel, constraints);
        constraints.gridx = 1;
        add(handleTextField, constraints);
        constraints.gridx = 2;
        add(loginButton, constraints);
        constraints.gridx = 0;
        constraints.gridy = 1;
        add(passwordLabel, constraints);
        constraints.gridx = 1;
        add(passwordField, constraints);
        constraints.gridx = 0;
        constraints.gridy = 2;
        add(emailLabel, constraints);
        constraints.gridx = 1;
        add(emailTextField, constraints);

        constraints.insets.top = 0;
        constraints.insets.left = 0;
        constraints.gridy = 3;
        add(normalLoginRadio, constraints);
        constraints.gridy = 4;
        add(guestLoginRadio, constraints);
        constraints.gridy = 5;
        add(newLoginRadio, constraints);
    }

    /**
     * We need an init() method since we can't get the root pane in the
     * constructor, this panel needs to added to a container first.
     */
    public void init(String userInfo) {
        getRootPane().setDefaultButton(loginButton);
        handleTextField.requestFocus();
        
        // If we have userInfo then log in automatically.
        if (userInfo != null) {
            int indexOfColon = userInfo.indexOf(':');
            if (indexOfColon > -1 && indexOfColon < userInfo.length()) {
                // We have a password.
                handleTextField.setText(userInfo.substring(0, indexOfColon));
                passwordField.setText(userInfo.substring(indexOfColon + 1));
                // Simulate radion selection to initialise radio buttons' hidden
                // state.
                normalLoginRadio.setSelected(true);
                normalLoginRadio.getAction().actionPerformed(null);
            } else {
                handleTextField.setText(userInfo);
            }
            connect();
        }
    }

    protected void connect() {
        setAllEnabled(false);
        String password = new String(passwordField.getPassword());
        try {
            if (normalLoginRadio.isSelected()) {
                server.set_logininfo(LoginType.GGZ_LOGIN, handleTextField
                        .getText(), password, null);
            } else if (guestLoginRadio.isSelected()) {
                server.set_logininfo(LoginType.GGZ_LOGIN_GUEST, handleTextField
                        .getText(), null, null);
            } else {
                server.set_logininfo(LoginType.GGZ_LOGIN_NEW, handleTextField
                        .getText(), password, emailTextField.getText());
            }
            server.connect();
        } catch (RuntimeException e) {
            setAllEnabled(true);
            throw e;
        }
    }

    public void login() throws IOException {
        server.login();
    }

    public void resetLogin() {
        setAllEnabled(true);
        handleTextField.selectAll();
        handleTextField.requestFocus();
        passwordField.setText(null);
    }

    private void setAllEnabled(boolean enabled) {
        for (int i = getComponentCount() - 1; i >= 0; i--) {
            getComponent(i).setEnabled(enabled);
        }
    }

    private class ConnectToServerAction extends AbstractAction {

        public Object getValue(String key) {
            if (NAME.equals(key)) {
                return "Login";
            }
            return super.getValue(key);
        }

        public void actionPerformed(ActionEvent e) {
            connect();
        }

    }

    private class ChooseNormalLoginAction extends AbstractAction {

        public Object getValue(String key) {
            if (NAME.equals(key)) {
                return "Member login";
            }
            return super.getValue(key);
        }

        public void actionPerformed(ActionEvent e) {
            passwordField.setVisible(true);
            passwordLabel.setVisible(true);
            emailTextField.setVisible(false);
            emailLabel.setVisible(false);
        }

    }

    private class ChooseGuestLoginAction extends AbstractAction {

        public Object getValue(String key) {
            if (NAME.equals(key)) {
                return "Guest login";
            }
            return super.getValue(key);
        }

        public void actionPerformed(ActionEvent e) {
            passwordField.setVisible(false);
            passwordLabel.setVisible(false);
            emailTextField.setVisible(false);
            emailLabel.setVisible(false);
        }

    }

    private class ChooseNewLoginAction extends AbstractAction {

        public Object getValue(String key) {
            if (NAME.equals(key)) {
                return "New user";
            }
            return super.getValue(key);
        }

        public void actionPerformed(ActionEvent e) {
            passwordField.setVisible(true);
            passwordLabel.setVisible(true);
            emailTextField.setVisible(true);
            emailLabel.setVisible(true);
        }

    }
}
