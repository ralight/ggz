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

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.Frame;
import java.awt.SystemColor;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;

public class ScoresDialog extends JDialog implements ActionListener {
    private JLabel scoresLabel;

    public ScoresDialog(Frame owner) {
        super(owner, "Scores");
        JPanel scoresPanel = new JPanel(new FlowLayout());
        JPanel buttonPanel = new JPanel(new FlowLayout(FlowLayout.CENTER));
        JButton closeButton = new JButton("Close");
        scoresLabel = new JLabel();
        scoresLabel.setFont(new Font("Monospaced", Font.PLAIN, 12));
        scoresPanel.setOpaque(false);
        scoresPanel.add(scoresLabel);
        buttonPanel.setOpaque(false);
        buttonPanel.setBorder(BorderFactory.createMatteBorder(1, 0, 0, 0,
                SystemColor.textText));
        buttonPanel.add(closeButton);
        closeButton.addActionListener(this);
        getContentPane().add(scoresPanel, BorderLayout.NORTH);
        getContentPane().add(buttonPanel, BorderLayout.SOUTH);
        getContentPane().setBackground(SystemColor.text);
        setMinimumSize(new Dimension(200, 200));
    }

    public void setScores(String scores) {
        if (scores == null) {
            scores = "No Scores";
        }
        scoresLabel.setText("<HTML><PRE>" + scores + "</PRE></HTML");
        pack();
        Dimension preferredSize = getSize();
        Dimension minimumSize = getMinimumSize();
        setSize(Math.max(minimumSize.width, preferredSize.width), Math.max(
                minimumSize.height, preferredSize.height));
        invalidate();
        validate();
        repaint();
    }

    public void actionPerformed(ActionEvent event) {
        dispose();
    }
}
