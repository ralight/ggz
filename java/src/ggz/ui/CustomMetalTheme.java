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

import java.applet.Applet;
import java.awt.Font;
import java.util.logging.Logger;

import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import javax.swing.plaf.ColorUIResource;
import javax.swing.plaf.FontUIResource;
import javax.swing.plaf.metal.DefaultMetalTheme;
import javax.swing.plaf.metal.MetalLookAndFeel;

/**
 * Class used to provide a custom theme that reads it's values from applet
 * parameters.
 * 
 * The supported parameters are as follows.
 * 
 * Black White Primary1 Primary2 Primary3 Secondary Secondary2 Secondary3
 * ControlTextFont MenuTextFont SubTextFont SystemTextFont UserTextFont
 * WindowTitleFont
 * 
 * See the Integer.decode() and Font.decode() method to see what Color and Font
 * strings are supported.
 * 
 * @author Helg.Bredow
 * 
 */
public class CustomMetalTheme extends DefaultMetalTheme {
    protected static final Logger log = Logger.getLogger(CustomMetalTheme.class
            .getName());

    private ColorUIResource black;

    private ColorUIResource white;

    private ColorUIResource primary1;

    private ColorUIResource primary2;

    private ColorUIResource primary3;

    private ColorUIResource secondary1;

    private ColorUIResource secondary2;

    private ColorUIResource secondary3;

    private FontUIResource controlTextFont;

    private FontUIResource menuTextFont;

    private FontUIResource subTextFont;

    private FontUIResource systemTextFont;

    private FontUIResource userTextFont;

    private FontUIResource windowTitleFont;

    private Applet applet;

    public CustomMetalTheme(Applet applet) {
        this.applet = applet;
    }

    public String getName() {
        return "Custom";
    }

    protected ColorUIResource getBlack() {
        if (black == null) {
            black = getColorUIResource("theme.black");
            if (black == null) {
                black = super.getBlack();
            }
        }
        return black;
    }

    protected ColorUIResource getWhite() {
        if (white == null) {
            white = getColorUIResource("theme.white");
            if (white == null) {
                white = super.getWhite();
            }
        }
        return white;
    }

    protected ColorUIResource getPrimary1() {
        if (primary1 == null) {
            primary1 = getColorUIResource("theme.primary1");
            if (primary1 == null) {
                primary1 = super.getPrimary1();
            }
        }
        return primary1;
    }

    protected ColorUIResource getPrimary2() {
        if (primary2 == null) {
            primary2 = getColorUIResource("theme.primary2");
            if (primary2 == null) {
                primary2 = super.getPrimary2();
            }
        }
        return primary2;
    }

    protected ColorUIResource getPrimary3() {
        if (primary3 == null) {
            primary3 = getColorUIResource("theme.primary3");
            if (primary3 == null) {
                primary3 = super.getPrimary3();
            }
        }
        return primary3;
    }

    protected ColorUIResource getSecondary1() {
        if (secondary1 == null) {
            secondary1 = getColorUIResource("theme.secondary1");
            if (secondary1 == null) {
                secondary1 = super.getSecondary1();
            }
        }
        return secondary1;
    }

    protected ColorUIResource getSecondary2() {
        if (secondary2 == null) {
            secondary2 = getColorUIResource("theme.secondary2");
            if (secondary2 == null) {
                secondary2 = super.getSecondary2();
            }
        }
        return secondary2;
    }

    protected ColorUIResource getSecondary3() {
        if (secondary3 == null) {
            secondary3 = getColorUIResource("theme.secondary3");
            if (secondary3 == null) {
                secondary3 = super.getSecondary3();
            }
        }
        return secondary3;
    }

    public FontUIResource getMenuTextFont() {
        if (menuTextFont == null) {
            menuTextFont = getFontUIResource("theme.menuTextFont");
            if (menuTextFont == null) {
                menuTextFont = super.getMenuTextFont();
            }
        }
        return menuTextFont;
    }

    public FontUIResource getControlTextFont() {
        if (controlTextFont == null) {
            controlTextFont = getFontUIResource("theme.controlTextFont");
            if (controlTextFont == null) {
                controlTextFont = super.getControlTextFont();
            }
        }
        return controlTextFont;
    }

    public FontUIResource getSubTextFont() {
        if (subTextFont == null) {
            subTextFont = getFontUIResource("theme.subTextFont");
            if (subTextFont == null) {
                subTextFont = super.getSubTextFont();
            }
        }
        return subTextFont;
    }

    public FontUIResource getSystemTextFont() {
        if (systemTextFont == null) {
            systemTextFont = getFontUIResource("theme.systemTextFont");
            if (systemTextFont == null) {
                systemTextFont = super.getSystemTextFont();
            }
        }
        return systemTextFont;
    }

    public FontUIResource getUserTextFont() {
        if (userTextFont == null) {
            userTextFont = getFontUIResource("theme.userTextFont");
            if (userTextFont == null) {
                userTextFont = super.getUserTextFont();
            }
        }
        return userTextFont;
    }

    public FontUIResource getWindowTitleFont() {
        if (windowTitleFont == null) {
            windowTitleFont = getFontUIResource("theme.windowTitleFont");
            if (windowTitleFont == null) {
                windowTitleFont = super.getWindowTitleFont();
            }
        }
        return windowTitleFont;
    }

    /**
     * Gets the font resource based on the applet parameter. If the parameter is
     * null or cannot be parsed then this method returns null.
     * 
     * @param name
     * @return
     */
    protected FontUIResource getFontUIResource(String name) {
        String fontString = applet.getParameter(name);
        if (fontString != null) {
            return new FontUIResource(Font.decode(fontString));
        }
        return null;
    }

    /**
     * Gets the color resource based on the applet parameter. If the parameter
     * is null or cannot be parsed then this method returns null.
     * 
     * @param name
     * @return
     */
    protected ColorUIResource getColorUIResource(String name) {
        String rgbString = applet.getParameter(name);
        if (rgbString != null) {
            try {
                return new ColorUIResource(Integer.decode(rgbString).intValue());
            } catch (Throwable e) {
                log.warning(e.getMessage());
            }
        }
        return null;
    }

    /**
     * Sets the look and feel to Metal and installs this theme.
     * 
     * @param applet
     */
    public static void install(Applet applet) {
        MetalLookAndFeel.setCurrentTheme(new CustomMetalTheme(applet));
        try {
            // re-install the Metal Look and Feel
            UIManager.setLookAndFeel(new MetalLookAndFeel());
        } catch (UnsupportedLookAndFeelException e) {
            e.printStackTrace();
        }
        // only needed to update existing widgets
        SwingUtilities.updateComponentTreeUI(applet);
    }
}
