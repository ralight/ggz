//import GGZChannel;
import java.nio.ByteBuffer;

class GameServer
{
	public GameServer()
	{
	}

	private void run()
	{
		GGZChannel channel = new GGZChannel(2);
		ByteBuffer buffer = ByteBuffer.allocate(100);
		buffer.put(new String("Hello World.\n").getBytes());
		channel.write(buffer);

		int clientfd = channel.readfd();
		GGZChannel client = new GGZChannel(clientfd);
		client.close();
	}

	public final static void main(String[] args)
	{
		GameServer g = new GameServer();
		g.run();
	}
}
