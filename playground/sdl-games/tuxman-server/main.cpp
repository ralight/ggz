#include "tuxmanserv.h"
#include "map.h"

int main(int argc, char **argv)
{
	TuxmanServer serv;

	Map map;
	map.load("level9.pac");
	map.dump();

	return 0;
}

