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
package ggz.cards.common;

import java.util.ArrayList;

/* Global message types */
/*
 * Each MESSAGE_GAME will be followed by one of these opcodes. This represents
 * game-specific data being transmitted to the frontend.
 */
public class GameMessage {

	public static final ArrayList values = new ArrayList();

	/*
     * A simple text message, containing two strings: a "marker" and the
     * "message".
     */
    public static final GameMessage GAME_MESSAGE_TEXT = new GameMessage();

    /*
     * A text message for the player, consisting of a seat # followed by a
     * message string.
     */
    public static final GameMessage GAME_MESSAGE_PLAYER = new GameMessage();

    /*
     * A list of cards for each player. First comes a "marker" string, then (for
     * each player) an integer n plus n cards for that player.
     */
    public static final GameMessage GAME_MESSAGE_CARDLIST = new GameMessage();

    /*
     * Block data that may be game-specific. It is followed by the (string) name
     * of the game, followed by an integer n followed by n bytes of data. It is
     * up to the client frontend to determine what (if anything) to do with this
     * data; it'll be build at the server end by the game module.
     */
    public static final GameMessage GAME_MESSAGE_GAME = new GameMessage();

	private GameMessage() {
		values.add(this);
	}

	public static GameMessage valueOf(int ordinal) {
		return (GameMessage) values.get(ordinal);
	}
 }
