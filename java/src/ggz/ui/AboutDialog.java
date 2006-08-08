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

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.Frame;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ResourceBundle;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.JTextArea;
import javax.swing.KeyStroke;
import javax.swing.WindowConstants;
import javax.swing.border.Border;

public class AboutDialog extends JDialog {
    private static final ResourceBundle messages = ResourceBundle
            .getBundle("ggz.ui.messages");

    public AboutDialog(Frame owner) {
        super(owner, true);
        this.setTitle(messages.getString("About.Title"));
        this.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);

        // Close button.
        JPanel buttonPanel = new JPanel(new FlowLayout(FlowLayout.CENTER));
        JButton closeButton = new JButton(messages
                .getString("About.Button.Close"));
        ActionListener closeAction = new ActionListener() {
            public void actionPerformed(ActionEvent event) {
                AboutDialog.this.dispose();
            }
        };
        closeButton.addActionListener(closeAction);
        buttonPanel.add(closeButton);
        this.getContentPane().add(buttonPanel, BorderLayout.SOUTH);
        this.getRootPane().setDefaultButton(closeButton);

        // Tabbed pane.
        JTabbedPane tabbedPane = new JTabbedPane();
        this.getContentPane().add(tabbedPane, BorderLayout.CENTER);
        tabbedPane.addTab("License", createLicenseTab());
        tabbedPane.addTab("Credits", createCreditsTab());

        // Let escape key close dialog.
        this.getRootPane().registerKeyboardAction(closeAction,
                KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0),
                JComponent.WHEN_IN_FOCUSED_WINDOW);
    }

    private JPanel createLicenseTab() {
        // License.
        JPanel licensePanel = new JPanel(new BorderLayout());
        Border spacer = BorderFactory.createEmptyBorder(10, 10, 10, 10);
        JScrollPane licenseScrollPane = new JScrollPane();
        JTextArea licenseTextArea = new JTextArea() {
            public Dimension getPreferredScrollableViewportSize() {
                return new Dimension(500, 300);
            }
        };
        licenseTextArea.setEditable(false);
        licenseTextArea.setFont(new Font("Monospaced", Font.PLAIN, 12));
        licenseScrollPane.getViewport().add(licenseTextArea);
        licenseScrollPane.setBorder(BorderFactory.createCompoundBorder(spacer,
                licenseScrollPane.getBorder()));
        licensePanel.add(licenseScrollPane, BorderLayout.CENTER);

        readLicense(licenseTextArea);

        // Copyright
        // Note: Extra version information is also available in
        // /version.properties.
        Package pkg = getClass().getPackage();
        String version = pkg.getSpecificationVersion() + " "
                + pkg.getImplementationVersion();
        JLabel copyrightLabel = new JLabel("<HTML>" + "ggz-java version "
                + version + "<BR>Copyright (C) 2006 Helg Bredow<BR>"
                + " GGZ Gaming Zone comes with ABSOLUTELY NO WARRANTY.<BR>"
                + "  This is free software, and you are welcome to"
                + " redistribute it<BR>under certain conditions as"
                + " described in the license below.");
        copyrightLabel.setBorder(spacer);
        licensePanel.add(copyrightLabel, BorderLayout.NORTH);
        return licensePanel;
    }

    private JPanel createCreditsTab() {
        JPanel creditsPanel = new JPanel(new GridBagLayout());
        GridBagConstraints constraints = new GridBagConstraints();
        JLabel label = new JLabel("Java Programming");
        Font plain = label.getFont().deriveFont(Font.PLAIN);
        Font bold = label.getFont().deriveFont(Font.BOLD);

        constraints.gridx = 0;
        constraints.gridy = 0;
        label.setFont(plain);
        creditsPanel.add(label, constraints);
        constraints.gridy++;
        label = new JLabel("Helg Bredow");
        label.setFont(bold);
        creditsPanel.add(label, constraints);
        constraints.gridy++;
        constraints.insets.top = 20;

        label = new JLabel("Icons");
        label.setFont(plain);
        creditsPanel.add(label, constraints);
        constraints.gridy++;
        constraints.insets.top = 0;
        label = new JLabel("Mark James");
        label.setFont(bold);
        creditsPanel.add(label, constraints);
        constraints.gridy++;
        try {
            creditsPanel.add(new HyperlinkLabel(
                    "http://www.famfamfam.com/lab/icons/silk/", new URL(
                            "http://www.famfamfam.com/lab/icons/silk/")),
                    constraints);
        } catch (MalformedURLException e) {
            // ignore.
        }

        return creditsPanel;
    }

    private void readLicense(JTextArea textArea) {
        try {
            BufferedReader reader = new BufferedReader(new InputStreamReader(
                    getClass().getResourceAsStream("LICENSE"), "ASCII"));
            char[] buf = new char[1024];
            int numCharsRead;
            StringBuffer text = new StringBuffer();
            while ((numCharsRead = reader.read(buf)) != -1) {
                text.append(buf, 0, numCharsRead);
            }
            textArea.setText(text.toString());
            textArea.setCaretPosition(0);
        } catch (Throwable ex) {
            // Dump the stack trace for posterity but otherwise ignore the
            // error.
            ex.printStackTrace();
        }
    }

    public static void showDialog(Component parentComponent) {
        Frame frame = JOptionPane.getFrameForComponent(parentComponent);
        AboutDialog dialog = new AboutDialog(frame);
        dialog.pack();
        dialog.setLocationRelativeTo(frame);
        dialog.setVisible(true);
    }
}
