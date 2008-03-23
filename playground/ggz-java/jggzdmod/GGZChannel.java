// GGZDMod wrapper for Java
// Copyright (C) 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under GNU GPL conditions

// Copy of GGZChannel for GGZDMod

package GGZDMod;

import java.nio.channels.ReadableByteChannel;
import java.nio.channels.WritableByteChannel;
import java.nio.ByteBuffer;

public class GGZChannel implements ReadableByteChannel, WritableByteChannel
{
	private int fd;

	native int readfd();
	native void sendfd(int fd);
	native int readbuffer(byte[] dst, int length);
	native int writebuffer(byte[] src, int length);

	public GGZChannel(int fd)
	{
		this.fd = fd;
	}

	public int getFd()
	{
		return fd;
	}

	public boolean isOpen()
	{
		return true;
	}

	public void close()
	{
	}

	public int read(ByteBuffer dst)
	{
		if(this.fd < 0)
			return -1;
		return readbuffer(dst.array(), dst.capacity());
	}

	public int write(ByteBuffer src)
	{
		if(this.fd < 0)
			return -1;
		return writebuffer(src.array(), src.position());
	}

	public int readInt()
	throws Exception
	{
		ByteBuffer bb = ByteBuffer.allocate(4);
		int ret = read(bb);
		if(ret < 0)
			throw new Exception("handler: cannot read int opcode");

		byte[] data = bb.array();
		int op = (data[0] << 24) + (data[1] << 16) + (data[2] << 8) + data[3];
		return op;
	}

	public String readString()
	throws Exception
	{
		int length = readInt();

		ByteBuffer bb = ByteBuffer.allocate(length);
		int ret = read(bb);
		if(ret < 0)
			throw new Exception("handler: cannot read int opcode");

		byte[] data = bb.array();
		return new String(data);
	}

	public void writeInt(int i)
	throws Exception
	{
		byte[] data = {(byte)((i >> 24) & 8), (byte)((i >> 16) & 8), (byte)((i >> 8) & 8), (byte)(i & 8)};

		ByteBuffer bb = ByteBuffer.allocate(4);
		bb.put(data);
		int ret = write(bb);
		if(ret < 0)
			throw new Exception("handler: cannot read int opcode");
	}

	public void writeString(String s)
	throws Exception
	{
		writeInt(s.length());

		ByteBuffer bb = ByteBuffer.allocate(s.getBytes().length);
		bb.put(s.getBytes());
		int ret = write(bb);
		if(ret < 0)
			throw new Exception("handler: cannot read int opcode");
	}

	static
	{
		System.loadLibrary("jancillary");
	}
}

