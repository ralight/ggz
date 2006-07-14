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
package ggz.client.mod;

/* Transactions between ggzmod-ggz and ggzmod-game */
public class ModTransaction {
    /*
     * Sit down (stop spectatin; join a seat) Data: seat number (int*)
     */
    public static final ModTransaction GGZMOD_TRANSACTION_SIT = new ModTransaction();

    /*
     * Stand up (leave your seat; become a spectator) Data: NULL
     */
    public static final ModTransaction GGZMOD_TRANSACTION_STAND = new ModTransaction();

    /*
     * Boot a player Data: player name (const char*)
     */
    public static final ModTransaction GGZMOD_TRANSACTION_BOOT = new ModTransaction();

    /*
     * Replace a bot/reserved seat with an open one. Data: seat number (int*)
     */
    public static final ModTransaction GGZMOD_TRANSACTION_OPEN = new ModTransaction();

    /*
     * Put a bot into an open seat Data: seat number (int*)
     */
    public static final ModTransaction GGZMOD_TRANSACTION_BOT = new ModTransaction();

    /*
     * Information about one or more players Data: seat number (int*)
     */
    public static final ModTransaction GGZMOD_TRANSACTION_INFO = new ModTransaction();

    /*
     * A chat originating from the game client. Data: message (const char*)
     */
    public static final ModTransaction GGZMOD_TRANSACTION_CHAT = new ModTransaction();

    private ModTransaction() {
        // Private constructor to prevent access.
    }
}
