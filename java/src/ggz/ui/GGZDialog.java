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

import java.awt.Component;
import java.awt.Dimension;
import java.awt.Frame;
import java.awt.Insets;
import java.awt.Point;

import javax.swing.JDialog;
import javax.swing.JOptionPane;

public class GGZDialog extends JDialog {
    public GGZDialog(Frame owner, String title) {
        super(owner, title);
    }

    public GGZDialog(Frame owner, String title, boolean modal) {
        super(owner, title, modal);
    }

    public GGZDialog(Component owner, String title) {
        super(JOptionPane.getFrameForComponent(owner), title);
    }

    /**
     * Ensures that the dialog is not positioned off the screen.
     */
    public void repositionIfNecessary() {
        // Make sure that the dialog is not positioned off the screen.
        Point location = this.getLocation();
        Dimension screen = this.getToolkit().getScreenSize();
        Insets insets = this.getToolkit().getScreenInsets(
                this.getGraphicsConfiguration());
        if (location.x + this.getWidth() > screen.width - insets.right) {
            location.x = screen.width - (this.getWidth() + insets.right);
        }
        if (location.y + this.getHeight() > screen.height - insets.bottom) {
            location.y = screen.height - (this.getHeight() + insets.bottom);
        }
        this.setLocation(location);
    }
}
