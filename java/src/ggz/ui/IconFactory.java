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

import ggz.common.PlayerType;

import javax.swing.Icon;
import javax.swing.ImageIcon;

public class IconFactory {

    private static ImageIcon[] icons = new ImageIcon[PlayerType.values().length];

    private static ImageIcon friendIcon;

    private static ImageIcon ignoreIcon;

    private static ImageIcon adminIcon;

    private IconFactory() {
        // Private constructor to prevent access.
    }

    public static Icon getPlayerTypeIcon(PlayerType type) {
        if (type == null) {
            return null;
        }
        ImageIcon icon = icons[type.ordinal()];
        if (icon == null) {
            if (type == PlayerType.GGZ_PLAYER_NORMAL) {
                icon = new ImageIcon(IconFactory.class
                        .getResource("images/player_normal.png"));
            } else if (type == PlayerType.GGZ_PLAYER_GUEST) {
                icon = new ImageIcon(IconFactory.class
                        .getResource("images/player_guest.png"));
            } else if (type == PlayerType.GGZ_PLAYER_ADMIN) {
                icon = new ImageIcon(IconFactory.class
                        .getResource("images/player_admin.gif"));
            } else if (type == PlayerType.GGZ_PLAYER_HOST) {
                icon = new ImageIcon(IconFactory.class
                        .getResource("images/player_host.png"));
            } else if (type == PlayerType.GGZ_PLAYER_BOT) {
                icon = new ImageIcon(IconFactory.class
                        .getResource("images/player_bot.gif"));
            } else {
                icon = new ImageIcon(IconFactory.class
                        .getResource("images/player_unknown.gif"));
            }
            icons[type.ordinal()] = icon;
        }
        return icon;
    }

    public static Icon getFriendIcon() {
        if (friendIcon == null) {
            friendIcon = new ImageIcon(IconFactory.class
                    .getResource("images/friend.png"));
        }
        return friendIcon;
    }

    public static Icon getIgnoreIcon() {
        if (ignoreIcon == null) {
            ignoreIcon = new ImageIcon(IconFactory.class
                    .getResource("images/ignore.png"));
        }
        return ignoreIcon;
    }

    public static Icon getAdminIcon() {
        if (adminIcon == null) {
            adminIcon = new ImageIcon(IconFactory.class
                    .getResource("images/edit_permissions.png"));
        }
        return adminIcon;
    }
}
