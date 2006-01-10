package ggz.cards;

import java.awt.Dimension;
import java.awt.EventQueue;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GridLayout;
import java.awt.Point;
import java.awt.Rectangle;

import javax.swing.JButton;
import javax.swing.JPanel;

public class TablePanel extends JPanel {

    private JPanel buttonPanel;

    private int spriteWidth;

    private int spriteHeight;
    
    private GridLayout gridLayout;

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
                (getWidth() / 2) - (buttonPanel.getWidth() / 2),
                getHeight() - (buttonPanel.getHeight() + 110));
        invalidate();
        validate();
    }

    public void hideButtons() {
        remove(buttonPanel);
        invalidate();
        validate();
        repaint();
    }

    // public void update(Graphics g) {
    // paint(g);
    // }

    public void paintComponent(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;
        // Dimension d = getSize();
        //
        // // Create the offscreen graphics context
        // if ((offImage == null) || (d.width != offImage.getWidth())
        // || (d.height != offImage.getHeight())) {
        // offImage = getGraphicsConfiguration().createCompatibleImage(
        // d.width, d.height);
        // offGraphics = offImage.createGraphics();
        // }
        //
        Rectangle clip = g.getClipBounds();
        // offGraphics.setClip(clip);
        // offGraphics.setColor(getBackground());
        // offGraphics.fillRect(clip.x, clip.y, clip.width, clip.height);
        // super.paint(offGraphics);
        // g2d.drawImage(offImage, null, 0, 0);
        g2d.setColor(getBackground());
        g2d.fillRect(clip.x, clip.y, clip.width, clip.height);
        // try {
        // BufferedImage clippedImage = offImage.getSubimage(Math.max(0,
        // clip.x), clip.y, Math.min(d.width - clip.x, clip.x
        // + clip.width) - 1, Math.min(d.height - clip.y, clip.y
        // + clip.height) - 1);
        // g2d.drawImage(clippedImage, null, clip.x, clip.y);
        // } catch (RasterFormatException e) {
        // System.err.println(e.getMessage() + ": " + clip);
        // }
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
        for (int i = 1; i <= frames; i++) {
            // Display the next frame of animation.
            // if (x != endPos.x)
            x += xDelta;
            // if (y != endPos.y)
            y += yDelta;

            // Delay depending on how far we are behind.
            tm += millisPerFrame;
            long lagMillis = System.currentTimeMillis() - tm;
            if (lagMillis > 0) {
                // We're not drawing frames fast enough...we need to catch up.
                // Don't draw this frame.
                // System.err.println(lagMillis + "/"+millisPerFrame+
                // "="+Math.ceil(lagMillis/millisPerFrame));
                // droppedFrames += Math.ceil(lagMillis/millisPerFrame);
            } else {
                // Use passive rendering, let the AWT work out what needs to be
                // painted.
                // The paint events are placed on the event queue and handled
                // whenever
                // the system has time to process them.
                sprite.rotate(angle);
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
        sprite.repaint();
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
