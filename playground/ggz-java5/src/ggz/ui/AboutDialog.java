package ggz.ui;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Frame;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.InputStreamReader;

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
    private JPanel buttonPanel;

    private JButton closeButton;

    private JScrollPane licenseScrollPane;

    private JTextArea licenseTextArea;

    private JLabel copyrightLabel;

    public AboutDialog(Frame owner) {
        super(owner, "About GGZ Gaming Zone", true);
        setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);

        // Close button.
        buttonPanel = new JPanel(new FlowLayout(FlowLayout.CENTER));
        closeButton = new JButton("Close");
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
        licenseScrollPane.getViewport().add(licenseTextArea);
        licenseScrollPane.setBorder(BorderFactory.createCompoundBorder(spacer,
                licenseScrollPane.getBorder()));
        getContentPane().add(licenseScrollPane, BorderLayout.CENTER);

        readLicense();

        // Copyright
        copyrightLabel = new JLabel("Copyright (c) Helg Bredow 2006");
        copyrightLabel.setBorder(spacer);
        getContentPane().add(copyrightLabel, BorderLayout.NORTH);
    }

    private void readLicense() {
        try {
            licenseTextArea.read(new InputStreamReader(getClass()
                    .getResourceAsStream("LICENSE")),
                    "GNU LESSER GENERAL PUBLIC LICENSE");
        } catch (Throwable ex) {
            // Dump the stack trace for posterity but otherwise ignore the
            // error.
            ex.printStackTrace();
        }
    }

    private void close() {
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
