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
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.ResourceBundle;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.WindowConstants;
import javax.swing.border.Border;

public class AboutDialog extends JDialog {
    private static final ResourceBundle messages = ResourceBundle
            .getBundle("ggz.ui.messages");

    private JPanel buttonPanel;

    private JButton closeButton;

    private JScrollPane licenseScrollPane;

    private JTextArea licenseTextArea;

    private JLabel copyrightLabel;

    public AboutDialog(Frame owner) {
        super(owner, true);
        setTitle(messages.getString("About.Title"));
        setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);

        // Close button.
        buttonPanel = new JPanel(new FlowLayout(FlowLayout.CENTER));
        closeButton = new JButton(messages.getString("About.Button.Close"));
        closeButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
                close();
            }
        });
        buttonPanel.add(closeButton);
        getContentPane().add(buttonPanel, BorderLayout.SOUTH);

        // License.
        Border spacer = BorderFactory.createEmptyBorder(10, 10, 10, 10);
        licenseScrollPane = new JScrollPane();
        licenseTextArea = new JTextArea() {
            public Dimension getPreferredScrollableViewportSize() {
                return new Dimension(500, 300);
            }
        };
        licenseTextArea.setEditable(false);
        licenseTextArea.setFont(new Font("Monospaced", Font.PLAIN, 12));
        licenseScrollPane.getViewport().add(licenseTextArea);
        licenseScrollPane.setBorder(BorderFactory.createCompoundBorder(spacer,
                licenseScrollPane.getBorder()));
        getContentPane().add(licenseScrollPane, BorderLayout.CENTER);

        readLicense();

        // Copyright
        // Note: Extra version information is also available in /version.properties.
        Package pkg = getClass().getPackage();
        String version = pkg.getSpecificationVersion() + " " + pkg.getImplementationVersion();
        copyrightLabel = new JLabel(
                "<HTML>"
                        + "ggz-java version " + version + "<BR>Copyright (C) 2006 Helg Bredow<BR>"
                        + " GGZ Gaming Zone comes with ABSOLUTELY NO WARRANTY.<BR>"
                        + "  This is free software, and you are welcome to"
                        + " redistribute it<BR>under certain conditions as"
                        + " described in the license below.");
        copyrightLabel.setBorder(spacer);
        getContentPane().add(copyrightLabel, BorderLayout.NORTH);
    }

    private void readLicense() {
        try {
        	BufferedReader reader = new BufferedReader(new InputStreamReader(getClass()
                  .getResourceAsStream("LICENSE"), "ASCII"));
        	char[] buf = new char[1024];
        	int numCharsRead;
        	StringBuffer text = new StringBuffer();
        	while ((numCharsRead = reader.read(buf)) != -1) {
        		text.append(buf, 0, numCharsRead);
        	}
        	licenseTextArea.setText(text.toString());
        	licenseTextArea.setCaretPosition(0);
        } catch (Throwable ex) {
            // Dump the stack trace for posterity but otherwise ignore the
            // error.
            ex.printStackTrace();
        }
    }

    protected void close() {
        dispose();
    }

    public static void showDialog(Component parentComponent) {
        Frame frame = JOptionPane.getFrameForComponent(parentComponent);
        AboutDialog dialog = new AboutDialog(frame);
        dialog.pack();
        dialog.setLocationRelativeTo(frame);
        dialog.setVisible(true);
    }
}
