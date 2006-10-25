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
package ggz.client.core;

import java.util.ArrayList;

public class Module {

    private String name;

    private String protocolEngine;

    private String protocolVersion;

    private String className;

    private String iconResourcePath;

    private static ArrayList module_list = new ArrayList(1);

    static {
        // We only support GGZCards modules at the moment.
        add(new Module("GGZCards-Spades", "GGZCards", "17",
                "ggz.cards.CardGamePanel", "/ggz/ui/images/spades.gif"));
        add(new Module("GGZCards-LaPocha", "GGZCards", "17",
                "ggz.cards.CardGamePanel", "/ggz/ui/images/pocha.gif"));
        add(new Module("GGZCards-Bridge", "GGZCards", "17",
                "ggz.cards.CardGamePanel", "/ggz/ui/images/bridge.gif"));
        add(new Module("GGZCards-Sueca", "GGZCards", "17",
                "ggz.cards.CardGamePanel", "/ggz/ui/images/sueca.gif"));
        add(new Module("GGZCards-Suaro", "GGZCards", "17",
                "ggz.cards.CardGamePanel", "/ggz/ui/images/suaro.gif"));
        add(new Module("GGZCards-Whist", "GGZCards", "17",
                "ggz.cards.CardGamePanel", "/ggz/ui/images/whist.gif"));
        add(new Module("GGZCards-Hearts", "GGZCards", "17",
                "ggz.cards.CardGamePanel", "/ggz/ui/images/hearts.gif"));
        add(new Module("GGZCards-Euchre", "GGZCards", "17",
                "ggz.cards.CardGamePanel", "/ggz/ui/images/euchre.gif"));
    }

    private static void add(Module module) {
        module_list.add(module);
    }

    /** This returns the number of registered modules */
    public static int getNumModules() {
        return module_list == null ? 0 : module_list.size();
    }

    /** Checks if there is a module registered for the game type. */
    public static boolean isGameSupported(GameType game) {
        return getForGame(game) != null;
    }

    /**
     * Gets the module that provides support for the game. The protocol and
     * version must be an exact match for the game type but only the start of
     * the module name must match the game name. This allows us to define
     * different types of rooms for the various games.
     * 
     * @return the module that supports this game and protocol
     */
    public static Module getForGame(GameType type) {
        return getForGame(type.get_name(), type.get_prot_engine(), type
                .get_prot_version());

    }

    public static Module getForGame(String game, String engine, String version) {
        if (game == null || engine == null || version == null)
            return null;

        for (int i = 0, n = module_list.size(); i < n; i++) {
            Module module = (Module) module_list.get(i);
            if (engine.equals(module.getProtocolEngine())
                    && version.equals(module.getProtocolVersion())
                    && game.startsWith(module.getName())) {
                return module;
            }
        }
        return null;
    }

    public String getName() {
        return this.name;
    }

    public String getProtocolEngine() {
        return this.protocolEngine;
    }

    public String getProtocolVersion() {
        return this.protocolVersion;
    }

    /**
     * Gets the patch to the icon as is needed by Class.getResource().
     * 
     * @return the path to the icon.
     */
    public String getIconResourcePath() {
        return this.iconResourcePath;
    }

    public String getClassName() {
        return isEmbedded() ? this.className : null;
    }

    public String getPWD() {
        throw new UnsupportedOperationException(
                "Non-embedded modules support is not finished yet.");
    }

    public String getCommandLine() {
        throw new UnsupportedOperationException(
                "Non-embedded modules support is not finished yet.");
    }

    /**
     * Always returns true. We don't currently support non-embedded game clients
     * although suppport is mostly already implemented in the Mod.
     */
    public boolean isEmbedded() {
        return true;
    }

    public Module(String name, String protocolEngine, String protocolVersion,
            String className, String iconPath) {
        if (name == null)
            throw new NullPointerException("name cannot be null");
        if (protocolEngine == null)
            throw new NullPointerException("protocolEngine cannot be null");
        if (protocolVersion == null)
            throw new NullPointerException("protocolVersion cannot be null");
        if (className == null)
            throw new NullPointerException("className cannot be null");
        if (iconPath == null)
            throw new NullPointerException("iconPath cannot be null");
        this.name = name;
        this.protocolEngine = protocolEngine;
        this.protocolVersion = protocolVersion;
        this.className = className;
        this.iconResourcePath = iconPath;
    }
}
