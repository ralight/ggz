#include "tuxmanserv.h"
#include "map.h"

int main(int argc, char **argv)
{
	TuxmanServer serv;

	Map map;
	map.load(TUXMANDATA "/level9.pac");
	map.dump();

	serv.connect();

	return 0;
}

