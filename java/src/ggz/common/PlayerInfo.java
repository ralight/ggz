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
package ggz.common;

/** Player information structure */
public class PlayerInfo {
    private int num;

    private String realname;

    private String photo;

    private String host;

    public PlayerInfo(int num, String realname, String photo, String host) {
        this.num = num;
        this.realname = realname;
        this.photo = photo;
        this.host = host;
    }

    public int getNum() {
        return num;
    }

    public String getRealName() {
        return realname;
    }

    public String getPhoto() {
        return photo;
    }

    public String getHost() {
        return host;
    }
}
