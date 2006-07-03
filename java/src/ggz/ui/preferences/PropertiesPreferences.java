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
package ggz.ui.preferences;

import java.util.Properties;
import java.util.prefs.AbstractPreferences;
import java.util.prefs.BackingStoreException;

/**
 * Simple implementation of Preferences that is used when a security manager
 * prevents access to the platform default Preferences implementation.
 * 
 * @author Helg.Bredow
 * 
 */
public class PropertiesPreferences extends AbstractPreferences {
    private Properties props = new Properties();

    public PropertiesPreferences() {
        super(null, "");
    }

    protected String[] childrenNamesSpi() throws BackingStoreException {
        return new String[0];
    }

    protected AbstractPreferences childSpi(String name) {
        return null;
    }

    protected void flushSpi() throws BackingStoreException {
        // Nothing to do since we can't write.
    }

    protected String getSpi(String key) {
        return props.getProperty(key);
    }

    protected String[] keysSpi() throws BackingStoreException {
        return (String[]) props.keySet().toArray(new String[props.size()]);
    }

    protected void putSpi(String key, String value) {
        props.put(key, value);
    }

    protected void removeNodeSpi() throws BackingStoreException {
        // Nothing to do since we don't support children.
    }

    protected void removeSpi(String key) {
        props.remove(key);
    }

    protected void syncSpi() throws BackingStoreException {
        // Nothing to do since we have no backing store.
    }
};
