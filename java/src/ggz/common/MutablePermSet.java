/*
 * Copyright (C) 2007  Helg Bredow
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

/**
 * A PermSet that you can modify.
 * 
 * @author Helg.Bredow
 * 
 */
public class MutablePermSet extends PermSet {

    public MutablePermSet() {
        super();
    }

    public MutablePermSet(int bitfield) {
        super(bitfield);
    }

    public void set(Perm perm, boolean val) {
        super.set(perm, val);
    }
}
