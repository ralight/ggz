/*
 * Copyright (C) 2007  Helg Bredow
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
package ggz.common.dio;

import java.io.BufferedInputStream;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;

public class DIOInputStream extends DataInputStream {
    private DataInputStream realIn;

    private DynamicByteArrayInputStream packet;

    public DIOInputStream(InputStream in) {
        super(new DynamicByteArrayInputStream());
        this.packet = (DynamicByteArrayInputStream) this.in;
        this.realIn = new DataInputStream(new BufferedInputStream(in));
    }

    /**
     * Tells us how many bytes to expect from the server, including the bytes in
     * this header. So if the packet contains two bytes of data the packet size
     * will be 4; 2 bytes for this header and 2 for the data.
     * 
     * @return
     * @throws IOException
     */
    public void startPacket() throws IOException {
        short packetSize = this.realIn.readShort();
        int dataSize = packetSize - 2;
        this.packet.reset(dataSize);
        this.realIn.readFully(packet.buffer(), 0, dataSize);
    }

    public String readString() throws IOException {
        int size = readInt();
        byte[] chars = new byte[size];
        String message;

        readFully(chars);

        if (size > 0) {
            // Don't include the null terminator.
            message = new String(chars, 0, chars.length - 1, "UTF-8");
        } else {
            // This should never happen but we handle it just in case to prevent
            // a StringIndexOutOfBoundsException above.
            message = "";
        }
        return message;
    }
}

class DynamicByteArrayInputStream extends ByteArrayInputStream {
    public DynamicByteArrayInputStream() {
        super(new byte[1024]);
    }

    public byte[] buffer() {
        return this.buf;
    }

    public void reset(int size) {
        if (this.buf.length < size) {
            // Need a bigger buffer.
            this.buf = new byte[size];
        }
        this.count = size;
        this.pos = 0;
    }
}
