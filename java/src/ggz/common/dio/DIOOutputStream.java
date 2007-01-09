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

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;

/**
 * This class does not write anything to the output stream until the flush()
 * method is called. All writes first go into an internal buffer so that the
 * size of a packet can be determined. When the flush() method is called, a
 * "packet header" is written that contains the size of the packet (including
 * the header) in bytes.
 * 
 * @author Helg.Bredow
 * 
 */
public class DIOOutputStream extends DataOutputStream {
    private ByteArrayOutputStream buffer;

    private DataOutputStream output;

    public DIOOutputStream(OutputStream os) {
        super(new ByteArrayOutputStream());
        // The actual stream that will eventually be writtin to when the flush()
        // method is called.
        output = new DataOutputStream(os);
        // Cast our decorated stream back to what we know it is for convenience.
        // We have to do it this way because we can't create the buffer before
        // calling the super constructor.
        buffer = (ByteArrayOutputStream) out;
    }

    /**
     * Writes a char string to the given fd preceeded by its size. The string is
     * encoded using UTF-8.
     */
    public void writeString(String message) throws IOException {
        byte[] bytes = message.getBytes("UTF-8");
        int size = bytes.length + 1;

        writeInt(size);
        write(bytes);
        // I think we need to write the null terminator, if not then adjust size
        // above.
        write(0);
    }

    /**
     * Writes out the bytes that we have accumulated in previous write
     * operations, preceded by a packet header that specifies the size of the
     * packet.
     */
    public void endPacket() throws IOException {
        // Write the size of the packet that is being sent, this size must
        // include the two bytes for this header.
        output.writeShort(buffer.size() + 2);
        buffer.writeTo(output);
        output.flush();
        // Get ready for the next packet.
        buffer.reset();
    }
}
