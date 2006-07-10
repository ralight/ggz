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
import ggz.ui.preferences.GGZPreferences;

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.io.IOException;
import java.util.Arrays;
import java.util.ResourceBundle;

import javax.swing.AbstractAction;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPasswordField;
import javax.swing.JRadioButton;
import javax.swing.JTextField;
import javax.swing.text.Document;
import javax.swing.text.PlainDocument;

public class LoginPanel extends JPanel {
    private static final ResourceBundle messages = ResourceBundle
            .getBundle("ggz.ui.messages");

    private Server server;

    protected JTextField handleTextField;

    protected JPasswordField passwordField;

    protected JPasswordField confirmPasswordField;

    protected JTextField emailTextField;

    protected JLabel handleLabel;

    protected JLabel passwordLabel;

    protected JLabel confirmPasswordLabel;

    protected JLabel emailLabel;

    private JRadioButton normalLoginRadio;

    private JRadioButton guestLoginRadio;

    private JRadioButton newLoginRadio;

    private JButton loginButton;

    private JCheckBox rememberMeCheckBox;

    public LoginPanel(Server server) {
        super(new GridBagLayout());
        this.server = server;

        // Create all the components
        handleLabel = new JLabel("<HTML>"
                + messages.getString("LoginPanel.Label.Nickname")
                + ": <FONT color=red>*</FONT>");
        passwordLabel = new JLabel("<HTML>"
                + messages.getString("LoginPanel.Label.Password")
                + ": <FONT color=red>*</FONT>");
        confirmPasswordLabel = new JLabel("<HTML>"
                + messages.getString("LoginPanel.Label.ConfirmPassword")
                + ": <FONT color=red>*</FONT>");
        emailLabel = new JLabel(messages.getString("LoginPanel.Label.Email")
                + ":");
        handleTextField = new JTextField(20);
        // Nicknames are only allowed to be 16 characters.
        Document doc = handleTextField.getDocument();
        if (doc instanceof PlainDocument) {
            ((PlainDocument) doc).setDocumentFilter(new DocumentSizeFilter(16));
        }
        passwordField = new JPasswordField(20);
        confirmPasswordField = new JPasswordField(20);
        emailTextField = new JTextField(null, 20);
        normalLoginRadio = new JRadioButton(new ChooseNormalLoginAction());
        guestLoginRadio = new JRadioButton(new ChooseGuestLoginAction());
        newLoginRadio = new JRadioButton(new ChooseNewLoginAction());
        loginButton = new JButton(new ConnectToServerAction());
        rememberMeCheckBox = new JCheckBox(messages
                .getString("LoginPanel.CheckBox.RememberMe"));

        TextPopupMenu.enableFor(handleTextField);
        TextPopupMenu.enableFor(emailTextField);

        // Set up the components
        setOpaque(false);
        loginButton.setOpaque(false);
        normalLoginRadio.setOpaque(false);
        guestLoginRadio.setOpaque(false);
        newLoginRadio.setOpaque(false);
        rememberMeCheckBox.setOpaque(false);
        passwordField.setFont(handleTextField.getFont());
        confirmPasswordField.setFont(passwordField.getFont());

        // Group the radio buttons.
        ButtonGroup group = new ButtonGroup();
        group.add(guestLoginRadio);
        group.add(normalLoginRadio);
        group.add(newLoginRadio);

        // Simulate radion selection to initialise radio buttons' hidden state.
        normalLoginRadio.setSelected(true);
        normalLoginRadio.getAction().actionPerformed(null);

        // Add all the components
        GridBagConstraints constraints = new GridBagConstraints();
        constraints.anchor = GridBagConstraints.WEST;
        constraints.insets.top = 4;
        constraints.insets.left = 4;
        constraints.insets.left = 0;

        constraints.gridx = 1;
        constraints.gridy++;
        add(guestLoginRadio, constraints);
        constraints.insets.top = 0;
        constraints.gridy++;
        add(newLoginRadio, constraints);
        constraints.gridy++;
        add(normalLoginRadio, constraints);

        constraints.insets.top = 4;

        constraints.gridx = 0;
        constraints.gridy++;
        add(handleLabel, constraints);
        constraints.gridx = 1;
        add(handleTextField, constraints);
        constraints.gridx = 0;
        constraints.gridy++;
        add(passwordLabel, constraints);
        constraints.gridx = 1;
        add(passwordField, constraints);
        constraints.gridx = 0;
        constraints.gridy++;
        add(confirmPasswordLabel, constraints);
        constraints.gridx = 1;
        add(confirmPasswordField, constraints);
        constraints.gridx = 0;
        constraints.gridy++;
        add(emailLabel, constraints);
        constraints.gridx = 1;
        add(emailTextField, constraints);

        constraints.gridx = 1;
        constraints.gridy++;
        add(rememberMeCheckBox, constraints);

        constraints.gridx = 1;
        constraints.gridy++;
        add(loginButton, constraints);

        // Set up focus so that we use the order that components were added to
        // the container rather than the order that they appear on screen.
        // This method doesn't exist for Java 1.4.
        // setFocusTraversalPolicyProvider(true);
        // And this one seems to be broken...
        // setFocusCycleRoot(true);
        // setFocusTraversalPolicy(new ContainerOrderFocusTraversalPolicy());

        // By default, Swing panels and labels are focusable and we don't want
        // that.
        setFocusable(false);
        handleLabel.setFocusable(false);
        passwordLabel.setFocusable(false);
        confirmPasswordLabel.setFocusable(false);
        emailLabel.setFocusable(false);
    }

    /**
     * We need an init() method since we can't get the root pane in the
     * constructor, this panel needs to be added to a container first.
     */
    public void init(String userInfo) {
        getRootPane().setDefaultButton(loginButton);
        // handleTextField.requestFocus();

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
                // Simulate radion selection to initialise radio buttons' hidden
                // state.
                guestLoginRadio.setSelected(true);
                guestLoginRadio.getAction().actionPerformed(null);
            }
            connect();
        } else {
            String savedNickname = GGZPreferences.get(
                    GGZPreferences.LAST_LOGIN_NICKNAME, null);
            if (savedNickname != null) {
                handleTextField.setText(savedNickname);
                rememberMeCheckBox.setSelected(true);
            }

            String lastLoginType = GGZPreferences.get(
                    GGZPreferences.LAST_LOGIN_TYPE, null);
            if ("normal".equals(lastLoginType)) {
                normalLoginRadio.setSelected(true);
                normalLoginRadio.getAction().actionPerformed(null);
            } else if ("guest".equals(lastLoginType)) {
                guestLoginRadio.setSelected(true);
                guestLoginRadio.getAction().actionPerformed(null);
            }
        }
    }

    protected boolean validateUserInput() {
        if ("".equals(handleTextField.getText().trim())) {
            JOptionPane.showMessageDialog(LoginPanel.this, messages
                    .getString("LoginPanel.Message.BlankNickname"));
            return false;
        }

        if (!guestLoginRadio.isSelected()) {
            if (passwordField.getPassword().length == 0) {
                JOptionPane.showMessageDialog(LoginPanel.this, messages
                        .getString("LoginPanel.Message.BlankPassword"));
                return false;
            }
        }

        if (newLoginRadio.isSelected()) {
            // Check if both password fields contain a value and they match.
            if (!Arrays.equals(passwordField.getPassword(),
                    confirmPasswordField.getPassword())) {
                JOptionPane.showMessageDialog(LoginPanel.this, messages
                        .getString("LoginPanel.Message.PasswordMismatch"));
                return false;
            }
        }
        return true;
    }

    protected void connect() {
        if (!validateUserInput())
            return;
        setAllEnabled(false);
        String handle = handleTextField.getText();
        String password = new String(passwordField.getPassword());
        String loginType;

        try {
            if (normalLoginRadio.isSelected()) {
                loginType = "normal";
                server.set_logininfo(LoginType.GGZ_LOGIN, handle, password,
                        null);
            } else if (guestLoginRadio.isSelected()) {
                loginType = "guest";
                server.set_logininfo(LoginType.GGZ_LOGIN_GUEST, handle, null,
                        null);
            } else {
                loginType = "normal";
                server.set_logininfo(LoginType.GGZ_LOGIN_NEW, handle, password,
                        emailTextField.getText());
            }

            if (rememberMeCheckBox.isSelected()) {
                // Remember login details.
                GGZPreferences.put(GGZPreferences.LAST_LOGIN_NICKNAME, handle);
                GGZPreferences.put(GGZPreferences.LAST_LOGIN_TYPE, loginType);
            } else {
                // Clear the login details.
                GGZPreferences.put(GGZPreferences.LAST_LOGIN_NICKNAME, null);
                GGZPreferences.put(GGZPreferences.LAST_LOGIN_TYPE, null);
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
        confirmPasswordField.setText(null);
    }

    private void setAllEnabled(boolean enabled) {
        for (int i = getComponentCount() - 1; i >= 0; i--) {
            getComponent(i).setEnabled(enabled);
        }
    }

    private class ConnectToServerAction extends AbstractAction {

        public ConnectToServerAction() {
            super(messages.getString("LoginPanel.Button.Login"));
        }

        public void actionPerformed(ActionEvent e) {
            connect();
        }

    }

    private class ChooseNormalLoginAction extends AbstractAction {

        public ChooseNormalLoginAction() {
            super(messages.getString("LoginPanel.Radio.MemberLogin"));
        }

        public void actionPerformed(ActionEvent e) {
            passwordField.setVisible(true);
            passwordLabel.setVisible(true);
            confirmPasswordField.setVisible(false);
            confirmPasswordLabel.setVisible(false);
            emailTextField.setVisible(false);
            emailLabel.setVisible(false);
        }

    }

    private class ChooseGuestLoginAction extends AbstractAction {

        public ChooseGuestLoginAction() {
            super(messages.getString("LoginPanel.Radio.GuestLogin"));
        }

        public void actionPerformed(ActionEvent e) {
            passwordField.setVisible(false);
            passwordLabel.setVisible(false);
            confirmPasswordField.setVisible(false);
            confirmPasswordLabel.setVisible(false);
            emailTextField.setVisible(false);
            emailLabel.setVisible(false);
        }

    }

    private class ChooseNewLoginAction extends AbstractAction {

        public ChooseNewLoginAction() {
            super(messages.getString("LoginPanel.Radio.NewUser"));
        }

        public void actionPerformed(ActionEvent e) {
            passwordField.setVisible(true);
            passwordLabel.setVisible(true);
            confirmPasswordField.setVisible(true);
            confirmPasswordLabel.setVisible(true);
            emailTextField.setVisible(true);
            emailLabel.setVisible(true);
        }

    }
}
