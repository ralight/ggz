import java.nio.channels.ReadableByteChannel;
import java.nio.channels.WritableByteChannel;
import java.nio.ByteBuffer;

class GGZChannel implements ReadableByteChannel, WritableByteChannel
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

	static
	{
		System.loadLibrary("jancillary");
	}
}

