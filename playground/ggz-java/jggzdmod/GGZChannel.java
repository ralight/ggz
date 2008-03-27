// GGZDMod wrapper for Java
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions

// Copy of GGZChannel for GGZDMod

package GGZDMod;

import java.nio.channels.ReadableByteChannel;
import java.nio.channels.WritableByteChannel;
import java.nio.ByteBuffer;

/** @brief Networking base class for GGZ-Java
 *
 * This class implements Java nio channels with the advantage of being able to
 * work on already open sockets. In addition to some convenience methods for
 * reading and writing values, this class can also transfer other file
 * descriptors over any channel, which might service as input for new channels.
 *
 * Attention: This class will only work on systems which have libggz
 * installed, as it implements some native methods.
 */
public class GGZChannel implements ReadableByteChannel, WritableByteChannel
{
	private int fd;

	/** @brief Reads a file descriptor with ancillary data
	 *
	 * The value is read as one byte from the channel, which however is being
	 * accompanied with ancillary data. Therefore, this special method must be
	 * used instead of readByte.
	 *
	 * @return File descriptor
	 */
	native int readfd();

	/** @brief Sends a file descriptor with ancillary data
	 *
	 * @param fd File descriptor
	 *
	 * @see readfd
	 */
	native void sendfd(int fd);

	/** @internal */
	native int readbuffer(byte[] dst, int length);

	/** @internal */
	native int writebuffer(byte[] src, int length);

	/** @brief Constructor
	 *
	 * Creates a new channel for the given file descriptor.
	 *
	 * @param fd Already opened file descriptor
	 */
	public GGZChannel(int fd)
	{
		this.fd = fd;
	}

	/** @brief Return the wrapped file descriptor
	 *
	 * @return File descriptor of this channel
	 */
	public int getFd()
	{
		return fd;
	}

	/** @brief Tell if this channel is open
	 *
	 * @return Always \b true for GGZ
	 */
	public boolean isOpen()
	{
		return true;
	}

	/** Attempt to close this channel
	 *
	 * This method does nothing, since GGZ channels cannot be closed.
	 */
	public void close()
	{
	}

	/** @internal */
	public int read(ByteBuffer dst)
	{
		if(this.fd < 0)
			return -1;
		return readbuffer(dst.array(), dst.capacity());
	}

	/** @internal */
	public int write(ByteBuffer src)
	{
		if(this.fd < 0)
			return -1;
		return writebuffer(src.array(), src.position());
	}

	/** @brief Read a single byte from the channel
	 *
	 * @return One byte
	 */
	public byte readByte()
	throws Exception
	{
		ByteBuffer bb = ByteBuffer.allocate(1);
		int ret = read(bb);
		if(ret < 0)
			throw new Exception("handler: cannot read byte opcode");

		byte[] data = bb.array();
		byte op = data[0];
		return op;
	}

	/** @brief Read an integer (four bytes) from the channel
	 *
	 * The integer will arrive in network-byte order but will be returned
	 * in host-byte order.
	 *
	 * @return One integer
	 */
	public int readInt()
	throws Exception
	{
		ByteBuffer bb = ByteBuffer.allocate(4);
		int ret = read(bb);
		if(ret < 0)
			throw new Exception("handler: cannot read int opcode");

		int op = bb.getInt();
		return op;
	}

	/** @brief Read a string from the channel
	 *
	 * The string will arrive in easysock formatting, which means four bytes
	 * determining the length of the string, the string itself and a terminating
	 * null byte. This method will however return a native Java string.
	 *
	 * @return One string
	 */
	public String readString()
	throws Exception
	{
		int length = readInt();

		ByteBuffer bb = ByteBuffer.allocate(length);
		int ret = read(bb);
		if(ret < 0)
			throw new Exception("handler: cannot read string opcode");

		byte[] data = bb.array();
		return new String(data);
	}

	/** @brief Write a single byte into the channel
	 *
	 * @param c One byte
	 */
	public void writeByte(byte c)
	throws Exception
	{
		byte[] data = {c};

		ByteBuffer bb = ByteBuffer.allocate(1);
		bb.put(data);
		int ret = write(bb);
		if(ret < 0)
			throw new Exception("handler: cannot write byte opcode");
	}

	/** @brief Write a single integer into the channel
	 *
	 * @param i One integer
	 */
	public void writeInt(int i)
	throws Exception
	{
		ByteBuffer bb = ByteBuffer.allocate(4);
		bb.putInt(i);
		int ret = write(bb);
		if(ret < 0)
			throw new Exception("handler: cannot write int opcode");
	}

	/** @brief Write a single string into the channel
	 *
	 * @param s One string
	 */
	public void writeString(String s)
	throws Exception
	{
		byte[] zero = {0};

		ByteBuffer bb = ByteBuffer.allocate(s.getBytes().length + 1);
		bb.put(s.getBytes());
		bb.put(zero);
		writeInt(s.length() + 1);
		int ret = write(bb);
		if(ret < 0)
			throw new Exception("handler: cannot write string opcode");
	}

	static
	{
		System.loadLibrary("jancillary");
	}
}

