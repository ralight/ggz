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
public enum ModTransaction {
    /*
     * Sit down (stop spectatin; join a seat) Data: seat number (int*)
     */
    GGZMOD_TRANSACTION_SIT,

    /*
     * Stand up (leave your seat; become a spectator) Data: NULL
     */
    GGZMOD_TRANSACTION_STAND,

    /*
     * Boot a player Data: player name (const char*)
     */
    GGZMOD_TRANSACTION_BOOT,

    /*
     * Replace a bot/reserved seat with an open one. Data: seat number (int*)
     */
    GGZMOD_TRANSACTION_OPEN,

    /*
     * Put a bot into an open seat Data: seat number (int*)
     */
    GGZMOD_TRANSACTION_BOT,

    /*
     * Information about one or more players Data: seat number (int*)
     */
    GGZMOD_TRANSACTION_INFO,

    /*
     * A chat originating from the game client. Data: message (const char*)
     */
    GGZMOD_TRANSACTION_CHAT;
}
