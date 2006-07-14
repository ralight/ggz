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
import java.awt.Toolkit;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.ClipboardOwner;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.event.ActionEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.ResourceBundle;

import javax.swing.AbstractAction;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.text.Document;
import javax.swing.text.JTextComponent;

/**
 * A popup menu that provides all the actions that are normally avialable in a
 * Windows text control such as cut, copy and paste. Why this popup is not
 * provided by Swing I have no idea...
 */
class TextPopupMenu extends MouseAdapter implements ClipboardOwner {
    protected static final ResourceBundle messages = ResourceBundle
            .getBundle("ggz.ui.messages");

    private JPopupMenu popup;

    protected JTextComponent editor;

    private TextPopupMenu(JTextComponent comp) {
        editor = comp;
        editor.addMouseListener(this);

        // Create the popup menu.
        popup = new JPopupMenu();
        popup.add(new JMenuItem(new CutAction()));
        popup.add(new JMenuItem(new CopyAction()));
        popup.add(new JMenuItem(new PasteAction()));
        popup.addSeparator();
        popup.add(new JMenuItem(new SelectAllAction()));
    }

    public static void enableFor(JTextComponent comp) {
        new TextPopupMenu(comp);
    }

    public void mousePressed(MouseEvent e) {
        maybeShowPopup(e);
    }

    public void mouseReleased(MouseEvent e) {
        maybeShowPopup(e);
    }

    private void maybeShowPopup(MouseEvent e) {
        if (e.isPopupTrigger()) {
            // Enable/disable menu items as appropriate.
            Component[] menuItems = popup.getComponents();
            for (int i = 0; i < menuItems.length; i++) {
                if (menuItems[i] instanceof JMenuItem) {
                    JMenuItem menuItem = (JMenuItem) menuItems[i];
                    menuItem.setEnabled(menuItem.getAction().isEnabled());
                }
            }
            popup.show(e.getComponent(), e.getX(), e.getY());
        }
    }

    public void actionPerformed(ActionEvent e) {
        System.out.println(e);
    }

    private class CutAction extends AbstractAction {
        private CutAction() {
            super(messages.getString("TextPopupMenu.Cut"));
        }

        public boolean isEnabled() {
            return editor.isEditable()
                    && (editor.getSelectionStart() != editor.getSelectionEnd());
        }

        public void actionPerformed(ActionEvent e) {
            editor.cut();
        }
    }

    private class CopyAction extends AbstractAction {
        private CopyAction() {
            super(messages.getString("TextPopupMenu.Copy"));
        }

        public boolean isEnabled() {
            return editor.getSelectionStart() != editor.getSelectionEnd();
        }

        public void actionPerformed(ActionEvent e) {
            // http://java.sun.com/docs/books/tutorial/uiswing/components/menu.html#popup
            // For Undo/redo
            // http://java.sun.com/docs/books/tutorial/uiswing/components/generaltext.html
            editor.copy();
        }
    }

    private class PasteAction extends AbstractAction {
        private Clipboard clipboard;

        private PasteAction() {
            super(messages.getString("TextPopupMenu.Paste"));
            try {
                clipboard = Toolkit.getDefaultToolkit().getSystemClipboard();
            } catch (SecurityException e) {
                // No permission so leave clipboard null. Most likely we are an
                // unsigned applet.
            }
        }

        /**
         * Determines whether text is available on the clipboard. If system
         * clipboard access is denied then this method simply returns true,
         * since in most cases text will be available. The clipboard used by the
         * TransferHandler when running in a sandbox isn't obtainable - the
         * method to retrieve it is private. If the component is not editable
         * then this method also returns false.
         * 
         * @return
         */
        public boolean isEnabled() {
            return editor.isEditable()
                    && (clipboard == null ? true
                            : isDataFlavorAvailable(DataFlavor.stringFlavor));
        }

        private boolean isDataFlavorAvailable(DataFlavor flavor) {
            if (flavor == null) {
                throw new NullPointerException("flavor");
            }

            try {
                Transferable cntnts = clipboard.getContents(null);
                if (cntnts == null) {
                    return false;
                }
                return cntnts.isDataFlavorSupported(flavor);
            } catch (IllegalStateException e) {
                // The clipboard is currently unavailable for some reason.
                return false;
            }
        }

        public void actionPerformed(ActionEvent e) {
            editor.paste();
            editor.requestFocus();
        }
    }

    private class SelectAllAction extends AbstractAction {
        private SelectAllAction() {
            super(messages.getString("TextPopupMenu.SelectAll"));
        }

        public boolean isEnabled() {
            boolean isAllSelected = false;
            Document doc = editor.getDocument();
            if (doc != null) {
                isAllSelected = editor.getSelectionStart() == 0
                        && editor.getSelectionEnd() == doc.getLength();
            }
            return editor.getText().length() > 0 && !isAllSelected;
        }

        public void actionPerformed(ActionEvent e) {
            editor.requestFocus();
            editor.selectAll();
        }
    }

    public void lostOwnership(Clipboard clipboard, Transferable contents) {
        // Nothing to do.
    }

}
