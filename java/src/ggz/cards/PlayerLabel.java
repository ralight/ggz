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
import java.awt.Color;
import java.awt.Font;

import javax.swing.Icon;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.SwingConstants;

public class PlayerLabel extends JPanel {
    private JLabel nameLabel;

    private JLabel iconLabel;

    private JLabel textLabel;

    public PlayerLabel(String playerName) {
        super(new BorderLayout());
        nameLabel = new JLabel(playerName);
        iconLabel = new JLabel();
        textLabel = new JLabel("", SwingConstants.LEFT);

        Font f = nameLabel.getFont();
        // nameLabel.setFont(f.deriveFont(f.getStyle() ^ Font.BOLD));
        nameLabel.setFont(f.deriveFont(f.getStyle() & Font.BOLD));
        nameLabel.setForeground(Color.WHITE);
        textLabel.setFont(f.deriveFont(f.getStyle() & Font.BOLD));
        textLabel.setVerticalAlignment(SwingConstants.TOP);
        textLabel.setHorizontalAlignment(SwingConstants.LEFT);
        textLabel.setForeground(Color.WHITE);

        add(nameLabel, BorderLayout.NORTH);
        add(iconLabel, BorderLayout.WEST);
        add(textLabel, BorderLayout.CENTER);
        setOpaque(false);
    }

    public void setPlayerName(String name) {
        nameLabel.setText(name);
        revalidate();
        repaint();
    }

    public void setText(String text) {
        textLabel.setText(text);
        revalidate();
        repaint();
    }

    public void setIcon(Icon icon) {
        iconLabel.setIcon(icon);
        revalidate();
        repaint();
    }
}
