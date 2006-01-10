/*
 * Copyright (C) Jerry Huxtable 1998
 */
package ggz.cards;

import java.awt.image.RGBImageFilter;

public class GrayFilter extends RGBImageFilter {

    public GrayFilter() {
        canFilterIndexColorModel = true;
    }

    /**
     * Averages the RGB values with another color to make it look greyed out.
     */
    public int filterRGB(int x, int y, int rgb) {
        int a = rgb & 0xff000000;
        int r = (rgb >> 16) & 0xff;
        int g = (rgb >> 8) & 0xff;
        int b = rgb & 0xff;
        r = (r + 210) / 2;
        g = (g + 210) / 2;
        b = (b + 210) / 2;
        return a | (r << 16) | (g << 8) | b;
    }

    public String toString() {
        return "Colors/Gray Out";
    }

}
