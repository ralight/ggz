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

import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.LayoutManager;

public class SmartChatLayout implements LayoutManager {
    private Component table;

    private Component chat;

    private Component spectators;

    public static final String CHAT = "chat";

    public static final String TABLE = "table";

    public static final String SPECTATOR_LIST = "spectators";

    public void addLayoutComponent(String name, Component comp) {
        if (TABLE.equals(name)) {
            table = comp;
        } else if (CHAT.equals(name)) {
            chat = comp;
        } else if (SPECTATOR_LIST.equals(name)) {
            spectators = comp;
        } else {
            throw new IllegalArgumentException(
                    "Unsupported layout constraint name: " + name);
        }
    }

    /**
     * Lays out the table so that it's always square, chat gets the rest.
     */
    public void layoutContainer(Container parent) {
        if (parent.getWidth() > parent.getHeight()) {
            // The container is horizontal so put chat to the side.
            table.setLocation(0, 0);
            table.setSize(parent.getHeight(), parent.getHeight());
            spectators.setLocation(parent.getHeight(), 0);
            spectators.setSize(parent.getWidth() - parent.getHeight(),
                    spectators.getPreferredSize().height);
            chat.setLocation(parent.getHeight(), spectators.getHeight());
            chat.setSize(parent.getWidth() - parent.getHeight(), parent
                    .getHeight()
                    - spectators.getHeight());
        } else {
            // Put chat to the bottom.
            table.setLocation(0, 0);
            table.setSize(parent.getWidth(), parent.getWidth());
            spectators.setLocation(0, parent.getWidth());
            spectators.setSize(spectators.getPreferredSize().width, parent
                    .getHeight()
                    - parent.getWidth());
            chat.setLocation(spectators.getWidth(), parent.getWidth());
            chat.setSize(parent.getWidth() - spectators.getWidth(), parent
                    .getHeight()
                    - parent.getWidth());
        }
    }

    public Dimension minimumLayoutSize(Container parent) {
        return preferredLayoutSize(parent);
    }

    public Dimension preferredLayoutSize(Container parent) {
        int width = 0;
        int height = Math.max(
                chat == null ? 0 : chat.getPreferredSize().height,
                table == null ? 0 : table.getPreferredSize().height);

        width += chat == null ? 0 : chat.getPreferredSize().width;
        width += table == null ? 0 : table.getPreferredSize().width;

        return new Dimension(width, height);
    }

    public void removeLayoutComponent(Component comp) {
        if (table == comp) {
            table = null;
        } else if (chat == comp) {
            chat = null;
        } else if (spectators == comp) {
            spectators = null;
        }
    }

}
