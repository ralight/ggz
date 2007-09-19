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
package ggz.ui;

import java.util.logging.Logger;

import javax.swing.event.HyperlinkListener;

public class GlobalHyperlinkListener {
    private static final Logger log = Logger
            .getLogger(GlobalHyperlinkListener.class.getName());

    private static HyperlinkListener instance;

    private GlobalHyperlinkListener() {
        // Private so as to ensure Singleton.
    }

    /**
     * Set the global hyplink listener that will be notified when any
     * HyperlinkButton in the application is clicked. This allows all hyperlink
     * activations to be handled in one place, usually an Applet so that it can
     * invoke showDocument with the URL.
     * 
     * @param listener
     */
    public static void set(HyperlinkListener newInstance) {
        if (instance != null) {
            log.warning("Global hyperlink listener has already been set.");
        }
        instance = newInstance;
    }

    public static HyperlinkListener get() {
        return instance;
    }
}
