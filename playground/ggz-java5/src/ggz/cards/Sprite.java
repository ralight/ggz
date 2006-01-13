package ggz.cards;

import ggz.cards.common.Card;
import ggz.cards.common.Face;
import ggz.cards.common.Suit;

import java.awt.AWTEvent;
import java.awt.AWTEventMulticaster;
import java.awt.AlphaComposite;
import java.awt.Color;
import java.awt.Component;
import java.awt.Composite;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.geom.AffineTransform;
import java.awt.image.AffineTransformOp;
import java.awt.image.BufferedImage;
import java.awt.image.FilteredImageSource;
import java.io.IOException;
import java.net.URL;

import javax.imageio.ImageIO;

public class Sprite extends Component {
    private static String basePath = "/ggz/cards/images/cards/";

    private static int dropShadowWidth = 2;

    private Card card;

    private BufferedImage originalImage;

    private BufferedImage rotatedImage;

    private BufferedImage unselectableImage;

    private boolean isSelected = false;

    private boolean isSelectable = true;

    private double angle;

//    private Shape originalHitRect;
//
//    private Shape rotatedHitRect;

    private Point unselectedLocation;

    transient private ActionListener actionListener;

    public Sprite(Card card) throws IOException {
        this(card, true);
    }

    public Sprite(Card c, boolean isFaceUp) throws IOException {
        card = c;
        enableEvents(AWTEvent.MOUSE_EVENT_MASK);
        URL url = getClass().getResource(getImageName(card, isFaceUp));
        originalImage = ImageIO.read(url);
        createRotatedImage();
//        createHitRect();
        resetSize();
        setEnabled(false);
        fanAtAngle(0);
    }

    private void resetSize() {
        // Set the size to take into consideration the drop shadow.
        Dimension size = getPreferredSize();
        size.width += dropShadowWidth;
        size.height += dropShadowWidth;
        setSize(size);
    }

    public void fanAtAngle(double radians) {
        // int rw = rotatedImage.getWidth();
        // int rh = rotatedImage.getHeight();
        // int ow = originalImage.getWidth();
        // int oh = originalImage.getHeight();
        // AffineTransform xform = new AffineTransform();
        // // xform.translate((rw / 2) - (ow / 2), (rh / 2) - (oh / 2));
        // xform.translate((rw / 2) - (ow / 2), (rh - (oh + ow / 2)));
        // // xform.rotate(radians, ow / 2, oh / 2);
        // xform.rotate(radians, ow / 2, oh);
        // AffineTransformOp op = new AffineTransformOp(xform,
        // AffineTransformOp.TYPE_BILINEAR);
        //
        // // Clear the background;
        // Graphics2D g2d = rotatedImage.createGraphics();
        // g2d.setComposite(AlphaComposite.Clear);
        // g2d.fillRect(0, 0, rw, rh);
        // g2d.dispose();
        //
        // if (isSelectable) {
        // op.filter(originalImage, rotatedImage);
        // }else {
        // op.filter(unselectableImage, rotatedImage);
        // }
        // rotatedHitRect = xform.createTransformedShape(originalHitRect);
        angle = radians;
        if (angle == 0) {
            resetSize();
        }
    }
    public void startRotation() {
        int rw = rotatedImage.getWidth();
        int rh = rotatedImage.getHeight();
        int ow = originalImage.getWidth();
        int oh = originalImage.getHeight();
        Point location = getLocation();
        setSize(rw, rh);
        setLocation(location.x - ((rw - ow) / 2), location.y - ((rh - oh) / 2));
    }
    public void endRotation() {
        int rw = rotatedImage.getWidth();
        int rh = rotatedImage.getHeight();
        int ow = originalImage.getWidth();
        int oh = originalImage.getHeight();
        Point location = getLocation();
        resetSize();
        setLocation(location.x + ((rw - ow) / 2), location.y + ((rh - oh) / 2));
    }

    // TODO optimise by having the TablePanel create the image and handle
    // the rotation
    public synchronized void rotate(double radians, boolean updateImage) {
        angle += radians;
        if (updateImage) {
        int rw = rotatedImage.getWidth();
        int rh = rotatedImage.getHeight();
        int ow = originalImage.getWidth();
        int oh = originalImage.getHeight();
        AffineTransform xform = new AffineTransform();
         xform.translate((rw / 2) - (ow / 2), (rh / 2) - (oh / 2));
        // xform.translate((rw / 2) - (ow / 2), (rh - (oh + ow / 2)));
        // // xform.rotate(radians, ow / 2, oh / 2);
        xform.rotate(angle, ow / 2, oh / 2);
        AffineTransformOp op = new AffineTransformOp(xform,
                AffineTransformOp.TYPE_BILINEAR);

        // Clear the background;
        Graphics2D g2d = rotatedImage.createGraphics();
        g2d.setComposite(AlphaComposite.Clear);
        g2d.fillRect(0, 0, rw, rh);
        g2d.dispose();

        op.filter(originalImage, rotatedImage);
//        rotatedHitRect = xform.createTransformedShape(originalHitRect);
        }
    }

    /**
     * Creates an image that is big enough to fully rotate the original image.
     */
    private void createRotatedImage() {
        // Only useful for rotating around center
         int hypotenuse = (int)
         Math.round(Math.hypot(originalImage.getWidth(),
         originalImage.getHeight()));
         int w = hypotenuse;
         int h = hypotenuse;
        // rotatedImage = new BufferedImage(w, h, BufferedImage.TYPE_INT_ARGB);
//        int hypotenuse = (int) Math.round(Math.hypot(
//                originalImage.getWidth() / 2, originalImage.getHeight()));
//        int w = hypotenuse * 2;
//        int h = hypotenuse + originalImage.getWidth() / 2;
        rotatedImage = new BufferedImage(w, h, BufferedImage.TYPE_INT_ARGB);
    }

//    private void createHitRect() {
//        originalHitRect = new Rectangle(originalImage.getWidth(), originalImage
//                .getHeight());
//        rotatedHitRect = originalHitRect;
//    }

    public Card card() {
        return card;
    }

    public synchronized void paint(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;

        if (angle == 0) {
            // Don't draw a drop shadow when rotating
            Composite oldComposite = g2d.getComposite();
            Composite alphaComposite = AlphaComposite.getInstance(
                    AlphaComposite.SRC_OVER, 0.3f);
            g2d.setComposite(alphaComposite);
            g2d.setColor(Color.BLACK);
            g2d.fillRoundRect(0, dropShadowWidth, getWidth() - 1,
                    getHeight() - 1, 2, 2);
            g2d.setComposite(oldComposite);
        }

        if (isSelectable) {
            // g2d.drawImage(rotatedImage, 0, 0, this);
            // g2d.drawImage(rotatedImage, null, 0, 0);
            // Apparently getSubImage() can cause performance problems
            // <http://www.jhlabs.com/ip/managed_images.html>
            // Rectangle clip = g.getClipBounds();
            // BufferedImage clippedImage = rotatedImage.getSubimage(clip.x,
            // clip.y, clip.width, clip.height);
            // g2d.drawImage(clippedImage, null, clip.x, clip.y);
            if (angle == 0) {
                g2d.drawImage(originalImage, null, dropShadowWidth, 0);
            } else {
                g2d.drawImage(rotatedImage, null, dropShadowWidth, 0);
            }
            // g2d.drawRect(clip.x, clip.y, clip.width-1, clip.height-1);
        } else {
            if (unselectableImage == null)
                throw new NullPointerException("greyed out image is null");
            g2d.drawImage(unselectableImage, dropShadowWidth, 0, this);
        }
    }

//    public boolean contains(int x, int y) {
//        return rotatedHitRect.contains(x, y);
//    }

    public Dimension getPreferredSize() {
        // return new Dimension(rotatedImage.getWidth(this), rotatedImage
        // .getHeight(this));
        return new Dimension(originalImage.getWidth(this), originalImage
                .getHeight(this));
    }

    private void createNonSelectableImage() {
        // Filter the image to grey it out.
        // ColorSpace cs = ColorSpace.getInstance(ColorSpace.CS_GRAY);
        // ColorConvertOp op = new ColorConvertOp(cs, null);
        // unselectableImage = op.filter(rotatedImage, null);
        // TODO don't use the rotated image, instead use the original image and
        // create a new rotated image.
        // Check that this doesn't cause an ugly lag though.
        // Filter the image to grey it out.
        Image gray = createImage(new FilteredImageSource(originalImage
                .getSource(), new GrayFilter()));
        int w = originalImage.getWidth();
        int h = originalImage.getHeight();
        unselectableImage = new BufferedImage(w, h, BufferedImage.TYPE_INT_ARGB);
        Graphics2D g2d = unselectableImage.createGraphics();
        g2d.drawImage(gray, 0, 0, null);
        g2d.dispose();
    }

    public void setSelectable(boolean b) {
        if (b != isSelectable) {
            if (!b && (unselectableImage == null)) {
                createNonSelectableImage();
            }
            isSelectable = b;
            // TODO check if this is really inefficient.
            fanAtAngle(angle);
            repaint();
        }
    }

    public boolean isSelectable() {
        return isSelectable;
    }

    public void setSelected(boolean b) {
        if (b != isSelected) {
            Point loc = getLocation();

            isSelected = b;
            if (isSelected) {
                unselectedLocation = (Point) loc.clone();
                loc.y += (int) Math.round(10 * -Math.cos(angle));
                loc.x += (int) Math.round(10 * Math.sin(angle));
            } else {
                loc = unselectedLocation;
            }
            setLocation(loc);
            getParent().repaint();
        }
    }

    public boolean isSelected() {
        return isSelected;
    }

    public Object[] getSelectedObjects() {
        if (isSelected) {
            return new Object[] { this };
        }
        return null;
    }

    public static void setBasePath(String path) {
        basePath = path;
    }

    public static String getImageName(Card card, boolean isFaceUp) {
        final String[] suits = new String[] { "c", "d", "h", "s" };
        final String[] ranks = new String[] { "a", "2", "3", "4", "5", "6",
                "7", "8", "9", "t", "j", "q", "k", "a" };
        String path = basePath;

        if (card == null || !isFaceUp || card.get_suit() == Suit.UNKNOWN_SUIT
                || card.get_face() == Face.UNKNOWN_FACE) {
            path += "b.gif";
        } else if (card.get_face() == Face.JOKER1
                || card.get_face() == Face.JOKER2) {
            path += "j.gif";
        } else {
            path += ranks[card.get_face().ordinal() - 1]
                    + suits[card.get_suit().ordinal() - 1] + ".gif";
        }
        return path;
    }

    public void processMouseEvent(MouseEvent e) {
        // when event occurs which causes "action" semantic
        if (isEnabled() && actionListener != null) {
            if (e.getID() == MouseEvent.MOUSE_PRESSED) {
                actionListener.actionPerformed(new ActionEvent(this,
                        ActionEvent.ACTION_PERFORMED, "Double Click"));
            }
        }
        super.processMouseEvent(e);
    }

    public void addActionListener(ActionListener l) {
        actionListener = AWTEventMulticaster.add(actionListener, l);
    }

    public void removeActionListener(ActionListener l) {
        actionListener = AWTEventMulticaster.remove(actionListener, l);
    }
}
