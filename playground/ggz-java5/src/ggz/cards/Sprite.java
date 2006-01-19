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
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.image.BufferedImage;
import java.awt.image.FilteredImageSource;
import java.io.IOException;
import java.net.URL;

import javax.imageio.ImageIO;

public class Sprite extends Component {
    private static String basePath = "/ggz/cards/images/cards/";

    protected static int DROP_SHADOW_WIDTH = 2;

    private static BufferedImage cardBacks;

    private static BufferedImage cardFronts;

    private Card card;

    private BufferedImage originalImage;

    private BufferedImage dropShadowedImage;

    private boolean isSelected = false;

    private boolean isSelectable = true;

    private Point unselectedLocation;

    transient private ActionListener actionListener;

    public Sprite(Card c) throws IOException {
        card = c;
        originalImage = loadImage();
        createDropShadowedImage();
        resetSize();
        setEnabled(false);
        enableEvents(AWTEvent.MOUSE_EVENT_MASK);
    }

    private void resetSize() {
        setSize(dropShadowedImage.getWidth(), dropShadowedImage.getHeight());
    }

    BufferedImage getImage() {
        return originalImage;
    }

    public Card card() {
        return card;
    }

    public synchronized void paint(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;
        Rectangle clip = g2d.getClipBounds();

        if (clip == null) {
            g2d.drawImage(dropShadowedImage, 0, 0,
                    dropShadowedImage.getWidth(),
                    dropShadowedImage.getHeight(), this);
        } else {
            g2d.drawImage(dropShadowedImage, clip.x, clip.y, clip.x
                    + clip.width, clip.y + clip.height, clip.x, clip.y, clip.x
                    + clip.width, clip.y + clip.height, this);
        }
    }

    public Dimension getPreferredSize() {
        return new Dimension(dropShadowedImage.getWidth(), dropShadowedImage
                .getHeight());
    }

    /**
     * Creates a buffered image that has a drop shadow with the card image. The
     * card image is greyed out if the card is not selectable.
     */
    private void createDropShadowedImage() {
        int w = originalImage.getWidth();
        int h = originalImage.getHeight();
        if (dropShadowedImage == null) {
            dropShadowedImage = new BufferedImage(w + DROP_SHADOW_WIDTH, h
                    + DROP_SHADOW_WIDTH, BufferedImage.TYPE_INT_ARGB);
        }
        Graphics2D g2d = dropShadowedImage.createGraphics();

        // Fill with transparency to create a transparent background.
        Composite oldComposite = g2d.getComposite();
        g2d.setComposite(AlphaComposite.Clear);
        g2d.fillRect(0, 0, dropShadowedImage.getWidth(), dropShadowedImage
                .getHeight());
        g2d.setComposite(oldComposite);

        // Draw the drop shadow
        Color shadowColor = new Color(0, 0, 0, 77);
        g2d.setColor(shadowColor);
        g2d.fillRoundRect(0, DROP_SHADOW_WIDTH, dropShadowedImage.getWidth()
                - DROP_SHADOW_WIDTH, dropShadowedImage.getHeight()
                - DROP_SHADOW_WIDTH, 4, 4);
        if (isSelectable) {
            g2d.drawImage(originalImage, DROP_SHADOW_WIDTH, 0, null);
        } else {
            // Filter the image to grey it out.
            Image gray = createImage(new FilteredImageSource(originalImage
                    .getSource(), new GrayFilter()));
            g2d.drawImage(gray, DROP_SHADOW_WIDTH, 0, null);
            // This is faster but doesn't look as good.
            // g2d.drawImage(originalImage, DROP_SHADOW_WIDTH, 0, null);
            // g2d.setColor(new Color(0, 0, 0, 22));
            // g2d.fillRoundRect(DROP_SHADOW_WIDTH, 0, originalImage.getWidth(),
            // originalImage.getHeight(), 4, 4);
        }
        g2d.dispose();
    }

    public void setSelectable(boolean b) {
        if (b != isSelectable) {
            isSelectable = b;
            createDropShadowedImage();
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
                loc.y -= 10;
                // loc.y += (int) Math.round(10 * -Math.cos(angle));
                // loc.x += (int) Math.round(10 * Math.sin(angle));
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

    private BufferedImage loadImage() throws IOException {
        if (card.get_suit() == Suit.UNKNOWN_SUIT
                || card.get_face() == Face.UNKNOWN_FACE) {
            if (cardBacks == null) {
                URL url = getClass().getResource(basePath + "cards-b1.png");
                cardBacks = ImageIO.read(url);
            }
            int cardWidth = cardBacks.getWidth() / 4;
            int cardHeight = cardBacks.getHeight();
            int xIndex = 0;
            int yIndex = 0;
            BufferedImage image = new BufferedImage(cardWidth, cardHeight,
                    BufferedImage.TYPE_INT_ARGB);
            Graphics2D g2d = image.createGraphics();
            g2d.drawImage(cardBacks, 0, 0, cardWidth, cardHeight, xIndex
                    * cardWidth, yIndex * cardHeight, (xIndex * cardWidth)
                    + cardWidth, (yIndex * cardHeight) + cardHeight, this);
            g2d.dispose();
            return image;
        }
        if (cardFronts == null) {
            URL url = getClass().getResource("images/cards-1.png");
            cardFronts = ImageIO.read(url);
        }
        int cardWidth = cardFronts.getWidth() / 4;
        int cardHeight = cardFronts.getHeight() / 13;
        int xIndex = card.get_suit().ordinal() - 1;
        int yIndex = card.get_face() == Face.ACE_HIGH ? 0 : card.get_face()
                .ordinal() - 1;
        BufferedImage image = new BufferedImage(cardWidth, cardHeight,
                BufferedImage.TYPE_INT_ARGB);
        Graphics2D g2d = image.createGraphics();
        g2d.drawImage(cardFronts, 0, 0, cardWidth, cardHeight, xIndex
                * cardWidth, yIndex * cardHeight, (xIndex * cardWidth)
                + cardWidth, (yIndex * cardHeight) + cardHeight, this);
        g2d.dispose();
        return image;
    }

    // public static String getImageName(Card card) {
    // final String[] suits = new String[] { "c", "d", "h", "s" };
    // final String[] ranks = new String[] { "a", "2", "3", "4", "5", "6",
    // "7", "8", "9", "t", "j", "q", "k", "a" };
    // String path = basePath;
    //
    // if (card == null || card.get_suit() == Suit.UNKNOWN_SUIT
    // || card.get_face() == Face.UNKNOWN_FACE) {
    // path += "b.gif";
    // } else if (card.get_face() == Face.JOKER1
    // || card.get_face() == Face.JOKER2) {
    // path += "j.gif";
    // } else {
    // path += ranks[card.get_face().ordinal() - 1]
    // + suits[card.get_suit().ordinal() - 1] + ".gif";
    // }
    // return path;
    // }

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
