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

import java.awt.AWTEvent;
import java.awt.Cursor;
import java.awt.event.MouseEvent;
import java.net.MalformedURLException;
import java.net.URL;

import javax.swing.JLabel;
import javax.swing.event.EventListenerList;
import javax.swing.event.HyperlinkEvent;
import javax.swing.event.HyperlinkListener;

/**
 * A label that can be used to simulate hyperlinks. It uses the default JLabel
 * HTML rendering to render the hyperlink but fires hyperlinkUpdate() on any
 * HyperlinkListeners when the label is clicked.
 * 
 * @author Helg.Bredow
 * 
 */
public class HyperlinkLabel extends JLabel {
    private URL url;

    private static HyperlinkListener globalListener;

    public HyperlinkLabel() {
        super();
        if (globalListener != null) {
            addHyperlinkListener(globalListener);
        }
        setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
    }

    public HyperlinkLabel(String text, URL url) {
        this();
        setText(text, url);
    }

    public void setText(String text) {
        this.setText(text, url);
    }

    public void setText(String text, String url) throws MalformedURLException {
        this.setText(text, new URL(url));
    }

    public void setText(String text, URL url) {
        super.setText("<HTML><A href='" + url + "'>" + text + "</A></HTML>");
        this.url = url;
    }

    protected void processMouseEvent(MouseEvent event) {
        if (url != null) {
            if (event.getID() == MouseEvent.MOUSE_CLICKED) {
                fireHyperlinkUpdate(new HyperlinkEvent(this,
                        HyperlinkEvent.EventType.ACTIVATED, url));
            }
        }
        super.processMouseEvent(event);
    }

    /**
     * Adds a hyperlink listener for notification of any changes, for example
     * when a link is selected and entered.
     * 
     * @param listener
     *            the listener
     */
    public synchronized void addHyperlinkListener(HyperlinkListener listener) {
        listenerList.add(HyperlinkListener.class, listener);
        enableEvents(AWTEvent.MOUSE_EVENT_MASK);
    }

    /**
     * Removes a hyperlink listener.
     * 
     * @param listener
     *            the listener
     */
    public synchronized void removeHyperlinkListener(HyperlinkListener listener) {
        listenerList.remove(HyperlinkListener.class, listener);
    }

    /**
     * Returns an array of all the <code>HyperLinkListener</code>s added to
     * this JEditorPane with addHyperlinkListener().
     * 
     * @return all of the <code>HyperLinkListener</code>s added or an empty
     *         array if no listeners have been added
     * @since 1.4
     */
    public synchronized HyperlinkListener[] getHyperlinkListeners() {
        return (HyperlinkListener[]) listenerList.getListeners(HyperlinkListener.class);
    }

    /**
     * Notifies all listeners that have registered interest for notification on
     * this event type. This is normally called by the currently installed
     * <code>EditorKit</code> if a content type that supports hyperlinks is
     * currently active and there was activity with a link. The listener list is
     * processed last to first.
     * 
     * @param e
     *            the event
     * @see EventListenerList
     */
    public void fireHyperlinkUpdate(HyperlinkEvent e) {
        // Guaranteed to return a non-null array
        Object[] listeners = listenerList.getListenerList();
        // Process the listeners last to first, notifying
        // those that are interested in this event
        for (int i = listeners.length - 2; i >= 0; i -= 2) {
            if (listeners[i] == HyperlinkListener.class) {
                ((HyperlinkListener) listeners[i + 1]).hyperlinkUpdate(e);
            }
        }
    }

    /**
     * Set the global hyplink listener that will be notified when any
     * HyperlinkButton in the application is clicked. This allows all hyperlink
     * activations to be handled in one place, usually an Applet so that it can
     * invoke showDocument with the URL.
     * 
     * @param listener
     */
    public static void setGlobalHyperlinkListener(HyperlinkListener listener) {
        globalListener = listener;
    }
}
