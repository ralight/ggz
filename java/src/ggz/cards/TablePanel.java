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
package ggz.cards;

import ggz.ui.HyperlinkLabel;

import java.awt.AlphaComposite;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.EventQueue;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GridLayout;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.geom.AffineTransform;
import java.awt.image.AffineTransformOp;
import java.awt.image.BufferedImage;
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

    private int paintCount;

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
        optionsSummaryLabel.setToolTipText("<HTML>" + options);
        Dimension preferredSize = optionsSummaryLabel.getPreferredSize();
        // Sometimes the label is really, really big for some reason so
        // resize it to a sane size if we have to.
        // preferredSize.width = Math.min(100, preferredSize.width);
        // preferredSize.height = Math.min(20, preferredSize.height);
        optionsSummaryLabel.setSize(preferredSize);
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
            Dimension preferredSize = rulesLabel.getPreferredSize();
            // Sometimes the label is really, really big for some reason so
            // resize it to a sane size if we have to.
            // preferredSize.width = Math.min(100, preferredSize.width);
            // preferredSize.height = Math.min(20, preferredSize.height);
            rulesLabel.setSize(preferredSize);
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
        add(buttonPanel, new TableConstraints(TableConstraints.BUTTON_PANEL));
        revalidate();
    }

    public void hideButtons() {
        remove(buttonPanel);
        revalidate();
        repaint();
    }

    public void setStatus(String message) {
        if (message != null) {
            if (statusLabel == null) {
                statusLabel = new JLabel();
                statusLabel.setHorizontalAlignment(SwingConstants.CENTER);
                statusLabel.setForeground(Color.white);
                add(statusLabel, new TableConstraints(
                        TableConstraints.STATUS_LABEL));
            }
            statusLabel.setText(message);
        } else if (statusLabel != null) {
            remove(statusLabel);
        }
        revalidate();
        repaint();
    }

    public void paintComponent(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;
        Rectangle clip = g.getClipBounds();

        // Only fill the background where and if we need to.
        g2d.setColor(Color.BLACK);
        g2d.fillRect(clip.x, clip.y, clip.width, clip.height);

        // Don't know how to only draw part of a round rect so draw the lot
        // regardless of clip.
        g2d.setColor(getBackground());
        g2d.fillRoundRect(0, 0, getWidth(), getHeight(), 250, 250);
        paintCount++;
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
    public void animate(Sprite originalSprite, Point endPos, double time) {
        long start = System.currentTimeMillis();
        long tm = start;
        Point startPos = originalSprite.getLocation();
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
        int spriteZOrder = getComponentZOrderJDK14(originalSprite);
        BufferedImage originalImage = originalSprite.getImage();
        RotatingSprite sprite = new RotatingSprite(originalImage.getWidth(),
                originalImage.getHeight());

        sprite.setImage(originalImage);
        sprite.setLocation(startPos);
        remove(spriteZOrder);
        add(sprite, spriteZOrder);
        // revalidate();
        if (useActiveRendering) {
            graphics = getGraphics();
            paint(graphics);
        } else {
            repaint();
        }
        paintCount = 0;
        int droppedFrameCount = 0;
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
                droppedFrameCount++;
            } else {
                int oldX = sprite.getX();
                int oldY = sprite.getY();
                // Use passive rendering, let the AWT work out what needs to be
                // painted. The paint events are placed on the event queue and
                // handled whenever the system has time to process them.
                sprite.rotate(angle, true);
                sprite.setLocation((int) Math.round(x), (int) Math.round(y));
                if (useActiveRendering) {
                    // Paint where the sprite used to be.
                    // graphics.setClip(oldX, oldY, sprite.getWidth(), sprite
                    // .getHeight());
                    // paint(graphics);
                    // // Paint where the sprite now is.
                    // graphics.setClip(sprite.getBounds());
                    // paint(graphics);

                    // Paint a rectangle that encompasses the old and new
                    // location. One larger paint() seems to be faster than two
                    // smaller ones.
                    graphics.setClip(sprite.getBounds().union(
                            new Rectangle(oldX, oldY, sprite.getWidth(), sprite
                                    .getHeight())));
                    paint(graphics);
                }
                try {
                    Thread.sleep(Math.max(0, tm - System.currentTimeMillis()));
                } catch (InterruptedException e) {
                    // Ignore, it just means we got woken up early.
                }
            }
        }
        originalSprite.setLocation(endPos);
        remove(spriteZOrder);
        add(originalSprite, spriteZOrder);
        revalidate();
        repaint();

        // double totalSeconds = ((System.currentTimeMillis() - start) /
        // 1000.0);
        // System.out.println(paintCount / totalSeconds + " paint() calls/s ("
        // + totalSeconds + "s), dropped " + droppedFrameCount + " of "
        // + frames + " frames");

        if (graphics != null) {
            graphics.dispose();
        }
    }

    /**
     * Gets the components index. Can be removed when we move to JDK 1.5.
     */
    public int getComponentZOrderJDK14(Component comp) {
        Component[] components = getComponents();
        for (int i = 0; i < components.length; i++) {
            if (components[i] == comp) {
                return i;
            }
        }
        return -1;
    }

    public void animate(int numSprites, Sprite[] sprite, Point[] endPos,
            double time) {
        if (numSprites == 0)
            return;
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
        Rectangle[] oldBounds = new Rectangle[numSprites];

        // Set up.
        for (int s = 0; s < numSprites; s++) {
            Point startPos = sprite[s].getLocation();
            xDelta[s] = (endPos[s].x - startPos.x) / frames;
            yDelta[s] = (endPos[s].y - startPos.y) / frames;
            x[s] = startPos.x;
            y[s] = startPos.y;
            oldBounds[s] = sprite[s].getBounds();
        }

        if (useActiveRendering) {
            graphics = getGraphics();
        }

        paintCount = 0;
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
                    sprite[s].setLocation((int) Math.round(x[s]), (int) Math
                            .round(y[s]));
                }

                if (useActiveRendering) {
                    // Create a union of all the rectangles to create one big
                    // clip area.
                    Rectangle clip = sprite[0].getBounds().union(oldBounds[0]);
                    sprite[0].getBounds(oldBounds[0]);
                    for (int s = 1; s < numSprites; s++) {
                        Rectangle.union(clip, oldBounds[s], clip);
                        // Now that we have used oldBounds we can update
                        // oldBounds with the new location.
                        sprite[s].getBounds(oldBounds[s]);
                        Rectangle.union(clip, oldBounds[s], clip);
                    }
                    graphics.setClip(clip);
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
            sprite[s].setLocation(endPos[s]);
            sprite[s].repaint();
        }

        // double totalSeconds = ((System.currentTimeMillis() - start) /
        // 1000.0);
        // System.out.println(paintCount / totalSeconds + " fps");

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

    private class RotatingSprite extends Component {
        private BufferedImage rotatedImage;

        private BufferedImage originalImage;

        private int xOffset;

        private int yOffset;

        private double angle;

        public RotatingSprite(int width, int height) {
            createRotateBuffer(width, height);
            setSize(rotatedImage.getWidth(), rotatedImage.getHeight());
        }

        public void setImage(BufferedImage imageToRotate) {
            originalImage = imageToRotate;
            int rw = rotatedImage.getWidth();
            int rh = rotatedImage.getHeight();
            int ow = originalImage.getWidth();
            int oh = originalImage.getHeight();
            xOffset = -((rw - ow) / 2) + Sprite.DROP_SHADOW_WIDTH;
            yOffset = -((rh - oh) / 2);
            rotate(angle, true);
        }

        /**
         * Sets the location of the sprite. The location is offset automatically
         * so that when the image is in the same orientation as the original
         * image then the sprite is in the correct location.
         */
        public void setLocation(int x, int y) {
            super.setLocation(x + xOffset, y + yOffset);
        }

        public synchronized void rotate(double radians, boolean updateImage) {
            angle += radians;
            if (updateImage) {
                int rw = rotatedImage.getWidth();
                int rh = rotatedImage.getHeight();
                int ow = originalImage.getWidth();
                int oh = originalImage.getHeight();
                AffineTransform xform = new AffineTransform();
                xform.translate((rw / 2) - (ow / 2), (rh / 2) - (oh / 2));
                xform.rotate(angle, ow / 2, oh / 2);
                AffineTransformOp op = new AffineTransformOp(xform,
                        AffineTransformOp.TYPE_BILINEAR);

                // Clear the background;
                Graphics2D g2d = rotatedImage.createGraphics();
                g2d.setComposite(AlphaComposite.Clear);
                g2d.fillRect(0, 0, rw, rh);
                g2d.dispose();

                op.filter(originalImage, rotatedImage);
            }
        }

        /**
         * Creates an image that is big enough to fully rotate the original
         * image around its center.
         */
        private void createRotateBuffer(int width, int height) {
            // int hypotenuse = (int) Math.round(Math.hypot(width, height));
            int hypotenuse = hypot(width, height);
            int w = hypotenuse;
            int h = hypotenuse;
            rotatedImage = new BufferedImage(w, h, BufferedImage.TYPE_INT_ARGB);
        }

        public synchronized void paint(Graphics g) {
            Graphics2D g2d = (Graphics2D) g;
            g2d.drawImage(rotatedImage, 0, 0, rotatedImage.getWidth(),
                    rotatedImage.getHeight(), this);
        }

        private int hypot(int width, int height) {
            return (int) Math.round(Math.sqrt((width * width)
                    + (height * height)));
        }
    }
}
