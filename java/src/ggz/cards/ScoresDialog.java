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

import javax.swing.JDialog;
import javax.swing.JLabel;

public class ScoresDialog extends JDialog {
    private JLabel scoresLabel;

    public ScoresDialog(Frame owner) {
        super(owner, "Scores");
        scoresLabel = new JLabel();
        scoresLabel.setFont(new Font("Monospaced", Font.PLAIN, 12));
        getContentPane().setLayout(new FlowLayout());
        getContentPane().add(scoresLabel, BorderLayout.NORTH);
        getContentPane().setBackground(SystemColor.text);
        setMinimumSize(new Dimension(200, 200));
    }

    public void setScores(String scores) {
        if (scores == null) {
            scores = "No Scores";
        }
        scoresLabel.setText("<HTML><PRE>" + scores + "</PRE></HTML");
        Dimension preferredSize = getPreferredSize();
        Dimension minimumSize = getMinimumSize();
        setSize(Math.max(minimumSize.width, preferredSize.width), Math.max(
                minimumSize.height, preferredSize.height));
        invalidate();
        validate();
        repaint();
    }
}
