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

import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.Iterator;
import java.util.LinkedList;

public class SortedList extends LinkedList {
    private Comparator comparator;

    public SortedList(Comparator c) {
        if (c == null) {
            throw new NullPointerException("comparator cannot be null");
        }
        comparator = c;
    }

    public boolean add(Object e) {
        int index = Collections.binarySearch(this, e, comparator);
        if (index < 0) {
            super.add(-index - 1, e);
        } else {
            super.add(index, e);
        }
        return true;
    }

    public boolean addAll(Collection c) {
        int numNew = c.size();
        if (numNew == 0)
            return false;
        modCount++;

        Iterator it = c.iterator();
        while (it.hasNext())
            add(it.next());
        return true;
    }

    public void add(int arg0, Object arg1) {
        throw new UnsupportedOperationException();
    }

    public boolean addAll(int arg0, Collection arg1) {
        throw new UnsupportedOperationException();
    }

    public void addFirst(Object arg0) {
        throw new UnsupportedOperationException();
    }

    public void addLast(Object arg0) {
        throw new UnsupportedOperationException();
    }

    public Object set(int arg0, Object arg1) {
        throw new UnsupportedOperationException();
    }
}
