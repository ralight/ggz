#include "ZoneGGZModServer.h"

#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include "ZoneGGZModGGZ.h"

#include <stdio.h>

#include "easysock.h"

ZoneGGZModServer::ZoneGGZModServer()
{
	m_gamemode = ZoneGGZ::unitialized;
	m_turn = -1;
	m_state = ZoneGGZ::preinit;
	m_numplayers = 0;
	m_players = NULL;
	m_gamename = NULL;
	m_ready = 0;
}

ZoneGGZModServer::~ZoneGGZModServer()
{
	if(m_players) free(m_players);
}

int ZoneGGZModServer::game_input(int zone_fd, int *p_fd)
{
	int op, seat, status = -1;

	if (es_read_int(zone_fd, &op) < 0)
	{
		ZONEERROR("couldn't read from descriptor\n");
		return -1;
	}

	ZONEDEBUG("server input\n");

	switch(op)
	{
		case ZoneGGZ::gamelaunch:
			if(ZoneGGZModGGZ::ggz_game_launch() == 0)
			{
				status = game_update(ZoneGGZ::launch, NULL);
			}
			break;
		case ZoneGGZ::gamejoin:
			if(ZoneGGZModGGZ::ggz_player_join(&seat, p_fd) == 0)
			{
				game_update(ZoneGGZ::join, &seat);
				status = 1;
			}
			break;
		case ZoneGGZ::gameleave:
			if((status = ZoneGGZModGGZ::ggz_player_leave(&seat, p_fd)) == 0)
			{
				game_update(ZoneGGZ::leave, &seat);
				status = 2;
			}
			break;
		case ZoneGGZ::gameover:
			status = 3;
			break;
		default:
			ZONEERROR("Unknown op: %i\n", op);
			status = -1;
			break;
	}

	return status;
}

int ZoneGGZModServer::slotZoneInput(int fd, int i)
{
//printf("moo.\n");
}

void ZoneGGZModServer::slotZoneAI()
{
//printf("moo.\n");
}

int ZoneGGZModServer::zoneMainLoop()
{
	int game_over;
	int i, fd, fd_max, status;
	fd_set active_fd_set, read_fd_set;

	if(!m_gamename)
	{
		ZONEERROR("server module not registered yet!\n");
		return -1;
	}

	game_over = 0;

	if(ZoneGGZModGGZ::ggz_init(m_gamename) < 0)
	{
		ZONEERROR("ggz_init\n");
		return -1;
	}

	if((ggz_sock = ZoneGGZModGGZ::ggz_connect()) < 0)
	{
		ZONEERROR("ggz_connect\n");
		return -1;
	}

	FD_ZERO(&active_fd_set);
	FD_SET(ggz_sock, &active_fd_set);

	ZONEDEBUG("entering main loop\n");

	while(!game_over)
	{
		read_fd_set = active_fd_set;
		fd_max = ZoneGGZModGGZ::ggz_fd_max();

		status = select((fd_max + 1), &read_fd_set, NULL, NULL, NULL);

		if(status <= 0)
		{
			if(errno == EINTR) continue;
			else
			{
				ZONEERROR("select() returned errno %i\n", errno);
				return -1;
			}
		}

		if(FD_ISSET(ggz_sock, &read_fd_set))
		{
			status = game_input(ggz_sock, &fd);

			switch(status)
			{
				case -1: /*KONQUEST_ERR_DUMMY*/
					ZONEERROR("dummy error\n");
					return -1;
				case 0: /*KONQUEST_STATE_INIT*/
					break;
				case 1: /*KONQUEST_STATE_PLAYING*/
					FD_SET(fd, &active_fd_set);
					break;
				case 2: /*KONQUEST_STATE_WAIT*/
					FD_CLR(fd, &active_fd_set);
					break;
				case 3: /*KONQUEST_STATE_DONE*/
					game_over = 1;
					break;
			}
		}

		for(i = 0; i < ZoneGGZModGGZ::ggz_seats_num(); i++)
		{
			fd = ZoneGGZModGGZ::ggz_seats[i].fd;
			if (fd != -1 && FD_ISSET(fd, &read_fd_set))
			{
				status = slotZoneInput(fd, i);
				if(status < 0) FD_CLR(fd, &active_fd_set);
			}
		}
	}

	ZoneGGZModGGZ::ggz_quit();

	return 0;
}

int ZoneGGZModServer::game_update(int event, void* data)
{
	int seat;

	switch(event)
	{
		case ZoneGGZ::launch:
			if((m_state != ZoneGGZ::init) && (m_state != ZoneGGZ::preinit))
			{
				ZONEERROR("launching game when state not init/preinit\n");
				return -1;
			}
			m_state = ZoneGGZ::wait;
			break;
		case ZoneGGZ::join:
			ZONEDEBUG("## pre-join\n");
			if (m_state != ZoneGGZ::wait) return -1;
			ZONEDEBUG("## post-join\n");
			seat = *(int*)data;
			game_send_seat(seat);
			game_send_players();
			if(m_gamemode != ZoneGGZ::unitialized) game_send_rules();
			if(m_ready == 1)
			{
				m_ready = 2;
				zoneNextTurn();
			}
			break;
		case ZoneGGZ::leave:
			if (m_state == ZoneGGZ::playing)
			{
				m_state = ZoneGGZ::wait;
				game_send_players();
			}
			break;
	}

	return 0;
}


/* Send out seat assignment */
int ZoneGGZModServer::game_send_seat(int seat)
{
	int fd = ZoneGGZModGGZ::ggz_seats[seat].fd;

	ZONEDEBUG("Sending player %d's seat num.\n", seat);

	if (es_write_int(fd, ZoneGGZ::seat) < 0 || es_write_int(fd, seat) < 0) return -1;

	ZONEDEBUG("RAW OUTPUT: %i (seat)\n", ZoneGGZ::seat);
	ZONEDEBUG("RAW OUTPUT: %i (seatnumber)\n", seat);

	return 0;
}

/* Send out player list to everybody */
int ZoneGGZModServer::game_send_players()
{
	int i, j, fd;

	m_numplayers = 0;

	for(i = 0; i < m_maxplayers; i++)
	{
		fd = ZoneGGZModGGZ::ggz_seats[i].fd;
		if((fd != -2) && (fd != 0))
		{
			m_numplayers++;
			ZONEDEBUG("seat number: %i (step %i) => %i\n", ZoneGGZModGGZ::ggz_seats[i].fd, i, m_numplayers);
		}
	}
	ZONEDEBUG("%i players found; sending number to clients...\n", m_numplayers);

	if(m_players) free(m_players);
	m_players = (int*)malloc(m_numplayers * sizeof(int));

	for (j = 0; j < m_numplayers; j++)
	{
		m_players[j] = 1;

		if((fd = ZoneGGZModGGZ::ggz_seats[j].fd) == -1)
		{
			ZONEDEBUG("Player %i is a un/bot-assigned (%i).\n", j, ZoneGGZModGGZ::ggz_seats[j].assign);
			continue;
		}

		ZONEDEBUG("Sending player list to player %d (%i)\n", j, fd);

		if(es_write_int(fd, ZoneGGZ::players) < 0)
		{
			ZONEERROR("couldn't send ZoneGGZ::players\n");
			return -1;
		}
		if(es_write_int(fd, m_numplayers) < 0)
		{
			ZONEERROR("couldn't send player number\n");
			return -1;
		}

		m_ready = 1;
		for(i = 0; i < m_numplayers; i++)
		{
			if(es_write_int(fd, ZoneGGZModGGZ::ggz_seats[i].assign) < 0)
			{
				ZONEERROR("couldn't send seat assignments\n");
				return -1;
			}
			ZONEDEBUG("RAW OUTPUT: %i (ggz_seats[i].assign)\n", ZoneGGZModGGZ::ggz_seats[i].assign);

			if(ZoneGGZModGGZ::ggz_seats[i].assign == GGZ_SEAT_OPEN) m_ready = 0;
			else
			{
				if(es_write_string(fd, ZoneGGZModGGZ::ggz_seats[i].name) < 0)
				{
					ZONEERROR("couldn't send player names\n");
					return -1;
				}
			}
		}
	}
	return 0;
}

void ZoneGGZModServer::game_send_rules()
{
	int i, fd;

	ZONEDEBUG("-- sending rules now ;)\n");

	for(i = 0; i < m_numplayers; i++)
	{
		fd = ZoneGGZModGGZ::ggz_seats[i].fd;
		if(fd == -1)
		{
			ZONEDEBUG("Skipping %i\n", i);
			continue;
		}
		if(es_write_int(fd, ZoneGGZ::rules) < 0)
		{
			ZONEERROR("couldn't send ZoneGGZ::rules\n");
			return;
		}
		if((es_write_int(fd, m_gamemode) < 0) || (es_write_int(fd, m_maxplayers) < 0))
		{
			ZONEERROR("couldn't send gamemode, maxplayers\n");
			return;
		}
	}

	return;
}

void ZoneGGZModServer::ZoneRegister(char* gamename, int gamemode, int maxplayers)
{
	ZONEDEBUG("registering...\n");
	if(m_gamename)
	{
		ZONEERROR("cannot register twice!\n");
		return;
	}
	m_gamename = (char*)malloc(strlen(gamename) + 1);
	strcpy(m_gamename, gamename);
	m_gamemode = gamemode;
	m_maxplayers = maxplayers;
}

void ZoneGGZModServer::zoneNextTurn()
{
	int fd;
	int counter;

	if((m_gamemode == ZoneGGZ::turnbased) && (m_turn != -1))
	{
		if(ZoneGGZModGGZ::ggz_seats[m_turn].assign == -5)
		{
			fd = ZoneGGZModGGZ::ggz_seats[m_turn].fd;
			if(es_write_int(fd, ZoneGGZ::turnover) < 0)
			{
				ZONEERROR("Couldn't send ZoneGGZ::turnover\n");
				return;
			}
		}
	}

	counter = 0;
	do
	{
		m_turn++;
		if(m_turn == m_numplayers) m_turn = 0;
		ZONEDEBUG("Next player: %i (%i)\n", m_turn, ZoneGGZModGGZ::ggz_seats[m_turn].assign);
		if(ZoneGGZModGGZ::ggz_seats[m_turn].assign == -2)
		{
			ZONEDEBUG("== AI!\n");
			slotZoneAI();
		}
		counter++;
		if(counter == m_numplayers + 1)
		{
			ZONEERROR("Loop detected!!!\n");
			return;
		}
	}
	while(ZoneGGZModGGZ::ggz_seats[m_turn].assign != -5);

	if((m_gamemode == ZoneGGZ::turnbased) && (ZoneGGZModGGZ::ggz_seats[m_turn].assign == -5))
	{
		fd = ZoneGGZModGGZ::ggz_seats[m_turn].fd;
		if(es_write_int(fd, ZoneGGZ::turn) < 0)
		{
			ZONEERROR("Couldn't send ZoneGGZ::turn\n");
			return;
		}
	}
}

int ZoneGGZModServer::zoneTurn()
{
	return m_turn;
}
