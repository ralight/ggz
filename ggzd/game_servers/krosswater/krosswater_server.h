// Zone includes
#include <ZoneGGZModServer.h>
#include <ZoneGGZ.h>

// Stackpath
#include <cwpathitem.h>

struct player_t
{
	int x, y;
};

class KrosswaterServer : public ZoneGGZModServer
{
	public:
		KrosswaterServer();
		~KrosswaterServer();
		int slotZoneInput(int fd, int i);
		void slotZoneAI();

		enum KrosswaterProto
		{
			proto_helloworld = 101,
			proto_move = 102,
			proto_map = 103,
			proto_map_respond = 104,
			proto_map_backtrace = 105,
			proto_move_broadcast = 106
		};
	private:
		void getMove();
		void sendMap();
		int doMove(int fromx, int fromy, int tox, int toy);

		int m_fd;
		int map_x, map_y;
		int **map;
		CWPathitem *path;
		player_t *players;
};
