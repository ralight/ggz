// Header file
#include "krosswater_server.h"

// System includes
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Zone includes
#include <ZoneProtocols.h>
#include <easysock.h>
#include <ZoneGGZModGGZ.h>

// Gives a console output of the map
void printpath(int width, int height, int **field)
{
	for(int j = 0; j < height; j++)
	{
		for(int i = 0; i < width; i++)
		{
			if(field[i][j]) cout << "\e[1m\e[39m";
			else cout << "\e[0m\e[39m";
			cout << field[i][j];
		}
		cout << endl;
	}
	cout << "\e[0m\e[39m";
}

KrosswaterServer::KrosswaterServer()
: ZoneGGZModServer()
{
	ZONEDEBUG(">> Krosswater: server launched\n");
	ZoneRegister("Krosswater", ZoneGGZ::turnbased, 4);

	map = NULL;
	path = NULL;
	players = NULL;

	zoneMainLoop();
}

KrosswaterServer::~KrosswaterServer()
{
	if(map)
	{
		for(int i = 0; i < map_x; i++)
			free(map[i]);
		free(map);
	}
	if(path) delete path;
	if(players) free(players);
}

int KrosswaterServer::slotZoneInput(int fd, int i)
{
	char string[256];
	int op;
	int status;

	if(es_read_int(fd, &op) < 0)
	{
		ZONEERROR("error in protocol (3)\n");
		return -1;
	}

	m_fd = fd;

	status = -1;
	if(op == proto_helloworld)
	{
		ZONEDEBUG("Hiya!\n");
		es_read_string(fd, (char*)&string, 256);
		ZONEDEBUG(":: %s ::\n", string);
		status = 0;
	}

	if(op == proto_move)
	{
		ZONEDEBUG("MOVE!!\n");
		getMove();
		status = 0;
	}

	if(op == proto_map)
	{
		ZONEDEBUG("MAP!!\n");
		sendMap();
		status = 0;
	}

	return status;
}

void KrosswaterServer::getMove()
{
	int fromx, fromy, tox, toy;
	int valid;
	int ret;

	// Read move coordinates
	if((es_read_int(m_fd, &fromx) < 0)
	|| (es_read_int(m_fd, &fromy) < 0)
	|| (es_read_int(m_fd, &tox) < 0)
	|| (es_read_int(m_fd, &toy) < 0))
	{
		ZONEERROR("Move corrupted!\n");
		return;
	}
	ZONEDEBUG("Client moved from %i/%i to %i/%i\n", fromx, fromy, tox, toy);
	ZONEDEBUG("That is: %i -> %i\n", map[fromx][fromy], map[tox][toy]);

	// Evaluate move
	valid = 1;
	if((fromx <= 0) || (fromy < 0) || (fromx >= map_x) || (fromy >= map_y)) valid = 0;
	if((tox < 0) || (toy < 0) || (tox >= map_x) || (toy >= map_y)) valid = 0;
	if((valid) && (map[fromx][fromy] == 0)) valid = 0;
	if((valid) && (map[tox][toy] != 0)) valid = 0;
	if((valid) && (map[fromx][fromy] == 2)) valid = 0;

	// Process new situation
	if(valid)
	{
		if(!path)
		{
			ZONEERROR("Pathitem is NULL!\n");
			return;
		}
		ret = doMove(fromx, fromy, tox, toy);
		if(ret)
		{
			ZONEDEBUG("Player %i won the game!\n", zoneTurn());
		}
		else zoneNextTurn();
	}
	else
	{
  		if(es_write_int(m_fd, ZoneGGZ::invalid) < 0)
		{
			ZONEERROR("couldn't send ZoneGGZ::invalid\n");
			return;
		}
	}
}

void KrosswaterServer::sendMap()
{
	// Create the map and pass it to Stackpath
	if(!map)
	{
		map_x = 40;
		map_y = 20;

		map = (int**)malloc(map_x * sizeof(int));
		for(int i = 0; i < map_x; i++)
			map[i] = (int*)malloc(map_y * sizeof(int));

		srandom(time(NULL));
		for(int j = 0; j < map_y; j++)
			for(int i = 0; i < map_x; i++)
				map[i][j] = rand() % 2;

		ZONEDEBUG("path >>> create\n");
		path = new CWPathitem(map_x, map_y, map);
	}

	// Place all players on the map
	if((!players) && (path))
	{
		players = (struct player_t*)malloc(sizeof(struct player_t) * m_maxplayers);
		for(int i = 0; i < m_maxplayers; i++)
		{
			players[i].x = 0;
			players[i].y = path->available(0, CWPathitem::force);
			map[players[i].x][players[i].y] = 2;
		}
		// Force map to have at least one goal
		path->available(map_x - 1, CWPathitem::force);
	}

	// Send map information to the client who joined
	if((es_write_int(m_fd, proto_map_respond) < 0)
	|| (es_write_int(m_fd, map_x) < 0)
	|| (es_write_int(m_fd, map_y) < 0))
	{
		ZONEERROR("Map control transmission corrupted!\n");
		return;
	}

	// Send map data
	for(int j = 0; j < map_y; j++)
		for(int i = 0; i < map_x; i++)
			if(es_write_int(m_fd, map[i][j]) < 0)
			{
				ZONEERROR("Map transmission corrupted!\n");
				return;
			}

	// Send player data
	if(es_write_int(m_fd, m_maxplayers) < 0)
	{
		ZONEERROR("Player control transmission corrupted!\n");
		return;
	}
	for(int i = 0; i < m_maxplayers; i++)
		if((es_write_int(m_fd, players[i].x) < 0)
		|| (es_write_int(m_fd, players[i].y) < 0))
		{
			ZONEERROR("Player transmission corrupted!\n");
			return;
		}
}

void KrosswaterServer::slotZoneAI()
{
	int fromx, fromy, tox, toy;
	int ret;

	do
	{
		fromx = rand() % map_x;
		fromy = rand() % map_y;
	}
	while(map[fromx][fromy] != 1);
	do
	{
		tox = rand() % map_x;
		toy = rand() % map_y;
	}
	while(map[tox][toy] != 0);

	ZONEDEBUG("AI moved from: %i/%i to: %i/%i\n", fromx, fromy, tox, toy);
	ret = doMove(fromx, fromy, tox, toy);
	if(ret)
	{
		ZONEDEBUG("AI (%i) won the game!\n", zoneTurn());
	}
	else zoneNextTurn();
}

int KrosswaterServer::doMove(int fromx, int fromy, int tox, int toy)
{
	int yl, yr;
	Pathitem *backtrace;
	int ret;

	map[fromx][fromy] = 0;
	map[tox][toy] = 1;

	// broadcast changes here!
	for(int i = 0; i < m_numplayers; i++)
		if((ZoneGGZModGGZ::ggz_seats[i].assign == -5) && (i != zoneTurn()))
	  		if((es_write_int(ZoneGGZModGGZ::ggz_seats[i].fd, proto_move_broadcast) < 0)
			|| (es_write_int(ZoneGGZModGGZ::ggz_seats[i].fd, fromx) < 0)
			|| (es_write_int(ZoneGGZModGGZ::ggz_seats[i].fd, fromy) < 0)
			|| (es_write_int(ZoneGGZModGGZ::ggz_seats[i].fd, tox) < 0)
			|| (es_write_int(ZoneGGZModGGZ::ggz_seats[i].fd, toy) < 0))
				ZONEERROR("couldn't send move broadcast!\n");


	ZONEDEBUG("path >>> setValue\n");
	path->setValue(fromx, fromy, 0);
	path->setValue(tox, toy, 1);
	//yl = path->available(0, CWPathitem::force);
	yl = players[zoneTurn()].y;
	yr = path->available(map_x - 1, CWPathitem::noforce);
	ZONEDEBUG("path >>> process\n");
	path->process(0, yl, map_x - 1, yr, Pathitem::normal | Pathitem::nostop);
	path->postprocess(map_x - 1);
	ZONEDEBUG("path >>> evaluating\n");
	ZONEDEBUG("from: %i/%i to: %i/*\n", 0, yl, map_x - 1);
	backtrace = path->result();

	if(backtrace)
	{
		cout << "## Found the path (" << backtrace->x() << ", " << backtrace->y() << ") !!! Doing backtrace..." << endl;

		// prepare for backtrace broadcast
		for(int i = 0; i < m_numplayers; i++)
			if(ZoneGGZModGGZ::ggz_seats[i].assign == -5)
	  			if((es_write_int(ZoneGGZModGGZ::ggz_seats[i].fd, proto_map_backtrace) < 0)
				|| (es_write_int(ZoneGGZModGGZ::ggz_seats[i].fd, zoneTurn()) < 0))
					ZONEERROR("couldn't send backtrace control\n");
		do
		{
			// send everyone the good message
			cout << backtrace->x() << "," << backtrace->y() << endl;
			for(int i = 0; i < m_numplayers; i++)
				if(ZoneGGZModGGZ::ggz_seats[i].assign == -5)
				{
	  				if((es_write_int(ZoneGGZModGGZ::ggz_seats[i].fd, backtrace->x()) < 0)
	  				|| (es_write_int(ZoneGGZModGGZ::ggz_seats[i].fd, backtrace->y()) < 0))
						ZONEERROR("couldn't send part of backtrace\n");
				}
			backtrace = backtrace->parent();
		}
		while(backtrace->parent() != NULL);

		// mark the end
		for(int i = 0; i < m_numplayers; i++)
			if(ZoneGGZModGGZ::ggz_seats[i].assign == -5)
	  			if(es_write_int(ZoneGGZModGGZ::ggz_seats[i].fd, -1) < 0)
					ZONEERROR("couldn't send backtrace control\n");
		ret = 1;
	}
	else
	{
		cout << "## Ooops, no way found :(" << endl;
		ret = 0;
	}

	printpath(map_x, map_y, map);
	path->clear();

	if(ret)
	{
		for(int i = 0; i < m_numplayers; i++)
			if(ZoneGGZModGGZ::ggz_seats[i].assign == -5)
	  			if(es_write_int(ZoneGGZModGGZ::ggz_seats[i].fd, ZoneGGZ::gameover) < 0)
					ZONEERROR("couldn't send ZoneGGZ::invalid\n");
	}

	return ret;
}
