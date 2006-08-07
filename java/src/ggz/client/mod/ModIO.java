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

import ggz.common.SeatType;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

public class ModIO {
    /** Messages sent from the game client to the ggz core client. */
    protected static final int MSG_GAME_STATE = 0;

    protected static final int REQ_STAND = 1;

    protected static final int REQ_SIT = 2;

    protected static final int REQ_BOOT = 3;

    protected static final int REQ_BOT = 4;

    protected static final int REQ_OPEN = 5;

    protected static final int REQ_CHAT = 6;

    protected static final int REQ_INFO = 7;

    /** Messages sent from the ggz core client to the game client. */
    protected static final int MSG_GAME_LAUNCH = 0;

    protected static final int MSG_GAME_SERVER = 1;

    protected static final int MSG_GAME_SERVER_FD = 2;

    /** Info about this player (us). */
    protected static final int MSG_GAME_PLAYER = 3;

    /** Sent from GGZ to game to tell of a seat change. No response is necessary. */
    protected static final int MSG_GAME_SEAT = 4;

    protected static final int MSG_GAME_SPECTATOR_SEAT = 5;

    protected static final int MSG_GAME_CHAT = 6;

    protected static final int MSG_GAME_STATS = 7;

    protected static final int MSG_GAME_INFO = 8;

    protected DataInputStream in;

    protected DataOutputStream out;

    protected int readInt() throws IOException {
        return this.in.readInt();
    }

    protected void writeInt(int v) throws IOException {
        this.out.writeInt(v);
    }

    protected void writeString(String s) throws IOException {
        byte[] bytes = s.getBytes("UTF-8");
        int size = bytes.length + 1;

        this.out.writeInt(size);
        this.out.write(bytes);
        this.out.write(0); // Write the null terminator.
    }

    protected String readString() throws IOException {
        int size = this.in.readInt();
        byte[] chars = new byte[size];
        String message;

        this.in.readFully(chars);

        if (size > 1) {
            // Don't include the null terminator.
            message = new String(chars, 0, chars.length - 1, "UTF-8");
        } else {
            // This should never happen but we handle it just in case to prevent
            // a StringIndexOutOfBoundsException above.
            message = null;
        }
        return message;
    }

    protected boolean readBoolean() throws IOException {
        return this.in.readInt() != 0;
    }

    protected void writeBoolean(boolean b) throws IOException {
        this.out.writeInt(b ? 1 : 0);
    }

    protected SeatType readSeatType() throws IOException {
        int ordinal = this.in.readInt();
        return SeatType.valueOf(ordinal);
    }

    protected void writeSeatType(SeatType type) throws IOException {
        this.out.writeInt(type.ordinal());
    }

    protected ModState readState() throws IOException {
        int ordinal = this.in.read();
        return ModState.valueOf(ordinal);
    }

    protected void writeState(ModState state) throws IOException {
        this.out.writeByte(state.ordinal());
    }
}
