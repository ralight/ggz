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
package ggz.tictactoe;

import java.awt.AWTEvent;
import java.awt.Color;
import java.awt.Component;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.GridLayout;
import java.awt.event.ComponentEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.font.FontRenderContext;
import java.awt.font.LineMetrics;

import javax.swing.BorderFactory;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.SwingConstants;

/**
 * Class that represents a tic tac toe board. It uses a JLabel for each cell.
 * Ideally it would draw it's own board rather than using JLabels because then
 * it could draw a line through the winning row. Currently it simply displays
 * the symbol in each cell and doesn't highlight the winning row.
 * 
 * @author Helg.Bredow
 * 
 */
public class BoardPanel extends JPanel {
    protected BoardListener listener;

    /** The symbol representing the player that can interact with the board. */
    protected String userSymbol;

    public BoardPanel() {
        super(new GridLayout(3, 3));

        // Create a label for each cell.
        for (int x = 0; x < 3; x++) {
            for (int y = 0; y < 3; y++) {
                JLabel cell = new JLabel(" ");
                cell.setForeground(Color.BLACK);
                cell.setBackground(Color.WHITE);
                cell.setHorizontalAlignment(SwingConstants.CENTER);
                cell.setOpaque(true);
                cell.setBorder(BorderFactory.createLineBorder(Color.BLACK));
                cell.addMouseListener(this.mouseListener);
                this.add(cell);
            }
        }

        // We need to be notified of resize events so that we can adjust the
        // font size.
        enableEvents(AWTEvent.COMPONENT_EVENT_MASK);
    }

    public void setUserSymbol(char symbol) {
        this.userSymbol = String.valueOf(symbol);
    }

    public void refresh(char[] boardData) {
        int cell = 0;

        for (int x = 0; x < 3; x++) {
            for (int y = 0; y < 3; y++) {
                JLabel label = (JLabel) this.getComponent(cell);
                label.setForeground(Color.BLACK);
                label.setText(String.valueOf(boardData[cell]));
                cell++;
            }
        }
    }

    protected void processComponentEvent(ComponentEvent e) {
        super.processComponentEvent(e);
        if (e.getID() == ComponentEvent.COMPONENT_RESIZED) {
            resetFontSize();
        }
    }

    protected void resetFontSize() {
        int cell = 0;
        Font font = getOptimumFont();

        for (int x = 0; x < 3; x++) {
            for (int y = 0; y < 3; y++) {
                JLabel label = (JLabel) this.getComponent(cell);
                label.setFont(font);
                cell++;
            }
        }
    }

    /**
     * Calculates the largest font size that can be used to display an X or O in
     * a cell.
     * 
     * @return
     */
    private Font getOptimumFont() {
        Component cell = this.getComponent(0);
        Font font = cell.getFont();
        Graphics2D graphics = (Graphics2D) cell.getGraphics();
        FontRenderContext context = graphics.getFontRenderContext();
        int direction;
        int previousDirection = 0;
        while (true) {
            LineMetrics metrics = font.getLineMetrics("X", context);
            float y = metrics.getHeight();
            if (y > cell.getHeight()) {
                direction = -1;
            } else {
                direction = 1;
            }
            font = font.deriveFont(font.getSize2D() + direction);

            if (previousDirection == 0) {
                // First time.
            } else if (direction != previousDirection) {
                break;
            }

            previousDirection = direction;
        }
        return font;
    }

    /**
     * Gets the ordinal cell index of the given component. This method is useful
     * for converting a mouse click on a cell component to an index in the
     * board.
     * 
     * @param comp
     *            the component to test.
     * @return the index of the cell or -1 if the component is not a cell on the
     *         board.
     */
    protected int getCellIndex(Component comp) {
        int cell = 0;
        for (int x = 0; x < 3; x++) {
            for (int y = 0; y < 3; y++) {
                if (comp == this.getComponent(cell)) {
                    return cell;
                }
                cell++;
            }
        }
        return -1;
    }

    public void setEnabled(boolean enabled) {
        super.setEnabled(enabled);

        if (userSymbol == null)
            return;

        // Just in case we are in the middel of accepting user input.
        for (int cell = 0, n = getComponentCount(); cell < n; cell++) {
            JLabel label = (JLabel) getComponent(cell);
            if (userSymbol.equals(label.getText())
                    && Color.GRAY.equals(label.getForeground())) {
                label.setForeground(Color.BLACK);
                label.setText(" ");
            }
        }
    }

    public void addBoardListener(BoardListener l) {
        if (this.listener != null && l != this.listener) {
            throw new UnsupportedOperationException(
                    "Multiple listeners not supported yet.");
        }
        this.listener = l;
    }

    private MouseListener mouseListener = new MouseAdapter() {
        public void mouseClicked(MouseEvent event) {
            if (BoardPanel.this.isEnabled()) {
                JLabel label = (JLabel) event.getComponent();
                int cellIndex = getCellIndex(label);

                // Put the symbol in the cell. The server will later send an
                // update and overwrite this value but that's OK. This prevents
                // the cell being blanked out when the board is disabled between
                // the time the mouse is clicked and the server responds with
                // the board update.
                label.setForeground(Color.BLACK);

                BoardPanel.this.listener.cellClicked(cellIndex);
            }
        }

        public void mouseEntered(MouseEvent event) {
            if (BoardPanel.this.isEnabled()) {
                JLabel label = (JLabel) event.getComponent();
                if (" ".equals(label.getText())) {
                    label.setForeground(Color.GRAY);
                    label.setText(userSymbol);
                }
            }
        }

        public void mouseExited(MouseEvent event) {
            if (BoardPanel.this.isEnabled()) {
                JLabel label = (JLabel) event.getComponent();
                if (userSymbol.equals(label.getText())
                        && Color.GRAY.equals(label.getForeground())) {
                    label.setForeground(Color.BLACK);
                    label.setText(" ");
                }
            }
        }
    };
}
