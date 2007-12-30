#include <stdio.h>
#include <string.h>

#include <ncurses.h>
#include <sys/time.h>
#include <sys/types.h>

#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "statsrt.h"

#define COL_RED 1
#define COL_WHITE 2

static int stats_rt_shmid = 0;

int ggztop_init(void)
{
	stats_rt_shmid = shmget(STATS_RT_SHMID, sizeof(stats_rt), S_IRUSR | S_IWUSR);
	if(stats_rt_shmid < 0)
	{
		fprintf(stderr, "ggztop: shared memory setup failure (%s)\n",
			strerror(errno));
		return 0;
	}

	return 1;
}

void ggztop_display_top_screen(stats_rt *rt)
{
	int i;
	int line = 4;

	clear();

	move(0, 0);
	attron(COLOR_PAIR(COL_WHITE) | A_BOLD);
	printw("ggztop - real-time statistics from ggzd");
	attroff(COLOR_PAIR(COL_WHITE) | A_BOLD);

	move(1, 0);
	printw("Uptime: %i secs", rt->uptime);
	printw(", Rooms: %i", rt->num_rooms);
	printw(", Players: %i", rt->num_players);
	printw(", Tables: %i", rt->num_tables);

	move(3, 0);
	printw("Room");
	move(3, 30);
	printw("Players");
	move(3, 40);
	printw("Tables");

	for(i = 0; i < rt->num_rooms; i++)
	{
		if(rt->players[i] > 0)
		{
			move(line, 0);
			attron(COLOR_PAIR(COL_RED));
			printw(rt->rooms[i]);
			attroff(COLOR_PAIR(COL_RED));

			move(line, 30);
			printw("%i", rt->players[i]);
			move(line, 40);
			printw("%i", rt->tables[i]);

			line++;
		}
	}

	wrefresh(stdscr);
}

void ggztop_display_top(stats_rt *rt, int rawmode)
{
	fd_set fds;
	struct timeval timeout;
	int ret;
	int redraw;

	if(!rawmode)
	{
		printf("\033[2J");
		initscr();
		cbreak();
		noecho();
		nodelay(stdscr, TRUE);
		start_color();
		use_default_colors();
		init_pair(COL_RED, COLOR_RED, -1);
		init_pair(COL_WHITE, COLOR_WHITE, -1);
	}

	redraw = 1;

	while(1)
	{
		if(redraw)
		{
			if(!rawmode)
				ggztop_display_top_screen(rt);
			redraw = 0;
		}

		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		FD_ZERO(&fds);
		FD_SET(STDIN_FILENO, &fds);

		ret = select(STDIN_FILENO + 1, &fds, NULL, NULL, &timeout);
		if(ret > 0)
		{
			int input = getch();
			if(input != -1)
			{
				if(input == 'q')
				{
					break;
				}
				else if(input == ' ')
				{
					redraw = 1;
				}
			}
		}
		else if(ret < 0)
		{
			/* FIXME: error? */
			break;
		}
		else
			redraw = 1;
	}

	if(!rawmode)
	{
		curs_set(1);
		endwin();
	}
}

void ggztop_display_text(stats_rt *rt)
{
	int i;

	for(i = 0; i < rt->num_rooms; i++)
	{
		printf("ROOM %i: %s\n", i, rt->rooms[i]);
		printf(" - %i players, %i tables\n", rt->players[i], rt->tables[i]);
	}
}

void ggztop_read(int textonly)
{
	stats_rt *rt;

	rt = (stats_rt*)shmat(stats_rt_shmid, 0, 0);

	if(rt->version != STATS_RT_VERSION)
	{
		fprintf(stderr, "ggztop: wrong stats_rt version\n");
		return;
	}
	if(rt->magicversion != sizeof(stats_rt))
	{
		fprintf(stderr, "ggztop: wrong stats_rt size\n");
		return;
	}

	if(textonly)
		ggztop_display_text(rt);
	else
		ggztop_display_top(rt, 0);

	shmdt(rt);
}

int main(int argc, char *argv[])
{
	int ret;
	int textonly = 0;

	if((argc == 2) && (!strcmp(argv[1], "-h")))
	{
		printf("ggztop [-t]\n");
		printf(" -t: bare text output, no loop\n");
		return 0;
	}

	if((argc == 2) && (!strcmp(argv[1], "-t")))
		textonly = 1;

	ret = ggztop_init();
	if(!ret)
		return -1;

	ggztop_read(textonly);

	return 0;
}

