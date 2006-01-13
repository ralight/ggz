package ggz.cards;

import ggz.ui.HyperlinkLabel;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.EventQueue;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GridLayout;
import java.awt.Point;
import java.awt.Rectangle;
import java.net.MalformedURLException;

import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.SwingConstants;

public class TablePanel extends JPanel {

    private JPanel buttonPanel;

    private int spriteWidth;

    private int spriteHeight;

    private GridLayout gridLayout;

    private JLabel optionsSummaryLabel;

    private HyperlinkLabel rulesLabel;

    protected String rulesURL;

    protected JLabel statusLabel;

    public TablePanel() {
        super(null);
        gridLayout = new GridLayout(1, 0, 2, 2);
        buttonPanel = new JPanel(gridLayout);
        buttonPanel.setOpaque(false);
        setPreferredSize(new Dimension(640, 400));
        setDoubleBuffered(true);
    }

    public void setSpriteDimensions(int width, int height) {
        spriteWidth = width;
        spriteHeight = height;
    }

    public void setOptionsSummary(String options) {
        if (optionsSummaryLabel == null) {
            optionsSummaryLabel = new JLabel("Options");
            optionsSummaryLabel.setForeground(Color.WHITE);
            add(optionsSummaryLabel);
            // Make sure the font is not bold.
            optionsSummaryLabel.setFont(optionsSummaryLabel.getFont()
                    .deriveFont(Font.PLAIN).deriveFont(Font.ITALIC));
        }
        options = options.replace("\n", "<BR>");
        optionsSummaryLabel.setToolTipText("<HTML>" + options + "</HTML>");
        optionsSummaryLabel.setSize(optionsSummaryLabel.getPreferredSize());
        if (rulesLabel == null) {
            optionsSummaryLabel.setLocation(0, 0);
        } else {
            optionsSummaryLabel.setLocation(0, rulesLabel.getHeight());
        }
    }

    /**
     * Adds a label in the top left corner that contains a hyperlink to the
     * rules of the game. It strips any leading text and removes any trailing
     * dot since that's the format the message currently comes from the server.
     * 
     * @param url
     * @throws MalformedURLException
     */
    public void setRulesURL(String url) throws MalformedURLException {
        int beginURLSubstring = url.indexOf("http");
        int endURLSubstring = url.lastIndexOf('.');

        if (beginURLSubstring > -1) {
            if (rulesLabel == null) {
                rulesLabel = new HyperlinkLabel();
                rulesLabel.setForeground(Color.WHITE);
                add(rulesLabel);
            }
            if (endURLSubstring > -1) {
                rulesURL = url.substring(beginURLSubstring, endURLSubstring);
            } else {
                rulesURL = url.substring(beginURLSubstring);
            }
            rulesLabel.setText("How to play", rulesURL);
            rulesLabel.setSize(rulesLabel.getPreferredSize());
            if (optionsSummaryLabel == null) {
                rulesLabel.setLocation(0, 0);
            } else {
                rulesLabel.setLocation(0, optionsSummaryLabel.getHeight());
            }
        }
    }

    public void addButton(JButton button) {
        buttonPanel.add(button);
    }

    public void removeAllButtons() {
        buttonPanel.removeAll();
        gridLayout.setRows(1);
    }

    public void showButtons() {
        add(buttonPanel);
        buttonPanel.setSize(buttonPanel.getPreferredSize());
        while (buttonPanel.getWidth() > getWidth()) {
            gridLayout.setRows(gridLayout.getRows() + 1);
            buttonPanel.setSize(buttonPanel.getPreferredSize());
        }
        buttonPanel.setLocation(
                (getWidth() / 2) - (buttonPanel.getWidth() / 2), getHeight()
                        - (buttonPanel.getHeight() + 110));
        invalidate();
        validate();
    }

    public void hideButtons() {
        remove(buttonPanel);
        invalidate();
        validate();
        repaint();
    }

    public void setStatus(String message) {
        if (message != null) {
            if (statusLabel == null) {
                statusLabel = new JLabel();
                statusLabel.setHorizontalAlignment(SwingConstants.CENTER);
                statusLabel.setForeground(Color.white);
                add(statusLabel);
            }
            statusLabel.setText(message);
            statusLabel.setSize(statusLabel.getPreferredSize());
            statusLabel.setLocation(
                    getWidth() / 2 - statusLabel.getWidth() / 2, getHeight()
                            / 2 - statusLabel.getHeight() / 2);
        } else if (statusLabel != null) {
            remove(statusLabel);
        }
        validate();
        repaint();
    }

    public void paintComponent(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;
        // Only fill the background where we need to.
        Rectangle clip = g.getClipBounds();
        g2d.setColor(Color.BLACK);
        g2d.fillRect(clip.x, clip.y, clip.width, clip.height);
        // Don't know how to only draw part of a round rect so draw the lot
        // regardless of clip.
        g2d.setColor(getBackground());
        g2d.fillRoundRect(0, 0, getWidth(), getHeight(), 250, 250);
    }

    /**
     * Uses passive rendering since the AWT handles clipping and other house
     * keeping for us. As long as this is called from a thread other than the
     * AWT even queue thread (EventQueue.isDispatchThread()) paint() should be
     * called by repaint() as quickly as is possible anyway.
     * 
     * @param sprite
     * @param endPos
     * @param time
     */
    public void animate(Sprite sprite, Point endPos, double time) {
        long start = System.currentTimeMillis();
        long tm = start;
        Point startPos = sprite.getLocation();
        int fps = 200;
        int millisPerFrame = (1000 / fps);
        double frames = fps * time;
        double xDelta = (endPos.x - startPos.x) / frames;
        double yDelta = (endPos.y - startPos.y) / frames;
        double x = startPos.x;
        double y = startPos.y;
        double angle = (Math.PI * 2) / frames;
        boolean useActiveRendering = EventQueue.isDispatchThread();
        Graphics graphics = null;

        if (useActiveRendering) {
            graphics = getGraphics();
        }
        sprite.startRotation();
        for (int i = 1; i <= frames; i++) {
            // Display the next frame of animation.
            x += xDelta;
            y += yDelta;

            // Delay depending on how far we are behind.
            tm += millisPerFrame;
            long lagMillis = System.currentTimeMillis() - tm;
            if (lagMillis > 0) {
                // We're not drawing frames fast enough...we need to catch up.
                // Don't draw this frame.
                sprite.rotate(angle, false);
            } else {
                // Use passive rendering, let the AWT work out what needs to be
                // painted.
                // The paint events are placed on the event queue and handled
                // whenever
                // the system has time to process them.
                sprite.rotate(angle, true);
                sprite.setLocation((int) Math.round(x), (int) Math.round(y));
                if (useActiveRendering) {
                    // We can simply set the clip to the bounds of the sprite
                    // because they
                    // have so much transparent space around the card graphic
                    // and we are
                    // moving in small increments.
                    // graphics.setClip(sprite.getBounds());
                    paint(graphics);
                }
                try {
                    Thread.sleep(Math.max(0, tm - System.currentTimeMillis()));
                } catch (InterruptedException e) {
                    // Ignore, it just means we got woken up early.
                }
            }
        }
        sprite.fanAtAngle(0);
        sprite.setLocation(endPos);
        sprite.endRotation();
        sprite.repaint();
        if (graphics != null) {
            graphics.dispose();
        }
    }

    public void animate(int numSprites, Sprite[] sprite, Point[] endPos,
            double time) {
        long start = System.currentTimeMillis();
        long tm = start;
        int fps = 200;
        int millisPerFrame = (1000 / fps);
        double frames = fps * time;
        double xDelta[] = new double[numSprites];
        double yDelta[] = new double[numSprites];
        double[] x = new double[numSprites];
        double[] y = new double[numSprites];
        boolean useActiveRendering = EventQueue.isDispatchThread();
        Graphics graphics = null;

        // Set up.
        for (int s = 0; s < numSprites; s++) {
            Point startPos = sprite[s].getLocation();
            xDelta[s] = (endPos[s].x - startPos.x) / frames;
            yDelta[s] = (endPos[s].y - startPos.y) / frames;
            x[s] = startPos.x;
            y[s] = startPos.y;
        }

        if (useActiveRendering) {
            graphics = getGraphics();
        }

        for (int i = 1; i <= frames; i++) {
            // Display the next frame of animation.
            for (int s = 0; s < numSprites; s++) {
                x[s] += xDelta[s];
                y[s] += yDelta[s];
            }
            // Delay depending on how far we are behind.
            tm += millisPerFrame;
            long lagMillis = System.currentTimeMillis() - tm;
            if (lagMillis > 0) {
                // We're not drawing frames fast enough...we need to catch up.
                // Don't draw this frame.
            } else {
                // Use passive rendering, let the AWT work out what needs to be
                // painted. The paint events are placed on the event queue and
                // handled whenever the system has time to process them.
                for (int s = 0; s < numSprites; s++) {
                    // sprite[s].rotate(angle, true);
                    sprite[s].setLocation((int) Math.round(x[s]), (int) Math
                            .round(y[s]));
                }

                if (useActiveRendering) {
                    // We can simply set the clip to the bounds of the sprite
                    // because they
                    // have so much transparent space around the card graphic
                    // and we are
                    // moving in small increments.
                    // graphics.setClip(sprite.getBounds());
                    paint(graphics);
                }

                try {
                    Thread.sleep(Math.max(0, tm - System.currentTimeMillis()));
                } catch (InterruptedException e) {
                    // Ignore, it just means we got woken up early.
                }
            }
        }

        for (int s = 0; s < numSprites; s++) {
            sprite[s].fanAtAngle(0);
            sprite[s].setLocation(endPos[s]);
            sprite[s].repaint();
        }

        if (graphics != null) {
            graphics.dispose();
        }
    }

    /**
     * Gets the location where cards are animated from and to for the purpose of
     * playing a card and winning trick.
     * 
     * @param player
     * @return
     */
    public Point getPlayerCardPos(int player) {

        switch (player) {
        case 0: // South
            return new Point(getWidth() / 2, getHeight());
        case 1: // West
            return new Point(-spriteWidth, getHeight() / 2);
        case 2: // North
            return new Point(getWidth() / 2, -spriteHeight);
        case 3: // East
            return new Point(getWidth(), getHeight() / 2);
        default:
            throw new UnsupportedOperationException(
                    "More than 4 players not supported yet.");
        }
    }
}
