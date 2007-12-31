#include <stdio.h>
#include <string.h>

#include <ncurses.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>

#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "statsrt.h"

#define COL_RED 1
#define COL_WHITE 2
#define COL_YELLOW 3
#define COL_GREEN 4

static int stats_rt_shmid = 0;
static time_t lastupdate = 0;
static int lastuptime = 0;
static int lastchats[STATS_RT_MAX_ROOMS][10];

int ggztop_init(void)
{
	int i, j;

	stats_rt_shmid = shmget(STATS_RT_SHMID, sizeof(stats_rt), S_IRUSR);
	if(stats_rt_shmid < 0)
	{
		fprintf(stderr, "ggztop: shared memory setup failure (%s)\n",
			strerror(errno));
		return 0;
	}

	for(i = 0; i < STATS_RT_MAX_ROOMS; i++)
		for(j = 0; j < 10; j++)
			lastchats[i][j] = 0;

	return 1;
}

void swap(stats_rt *rt, int row1, int row2)
{
	char tmproom[STATS_RT_MAX_ROOMNAME_LEN];
	int tmpplayer, tmptable, tmpchat;

	strcpy(tmproom, rt->rooms[row1]);
	strcpy(rt->rooms[row1], rt->rooms[row2]);
	strcpy(rt->rooms[row2], tmproom);

	tmpplayer = rt->players[row1];
	rt->players[row1] = rt->players[row2];
	rt->players[row2] = tmpplayer;

	tmptable = rt->tables[row1];
	rt->tables[row1] = rt->tables[row2];
	rt->tables[row2] = tmptable;

	tmpchat = rt->chat[row1];
	rt->chat[row1] = rt->chat[row2];
	rt->chat[row2] = tmpchat;
}

void sort(stats_rt *rt, int sortcolumn)
{
	int i, j;
	int swapped;

	do
	{
		swapped = 0;
		for(i = 0; i < rt->num_rooms; i++)
		{
			for(j = i + 1; j < rt->num_rooms; j++)
			{
				if(i == j)
					continue;

				if(sortcolumn == 1)
				{
					if(strcmp(rt->rooms[i], rt->rooms[j]) > 0)
					{
						swap(rt, i, j);
						swapped = 1;
					}
				}
				else if(sortcolumn == 2)
				{
					if(rt->players[i] < rt->players[j])
					{
						swap(rt, i, j);
						swapped = 1;
					}
				}
				else if(sortcolumn == 3)
				{
					if(rt->tables[i] < rt->tables[j])
					{
						swap(rt, i, j);
						swapped = 1;
					}
				}
				else if(sortcolumn == 4)
				{
					if(rt->chat[i] < rt->chat[j])
					{
						swap(rt, i, j);
						swapped = 1;
					}
				}
			}
		}
	}
	while(swapped);
}

void ggztop_display_top_screen(stats_rt *rt, int showallrooms, int sortcolumn)
{
	int i, j;
	int line = 4;
	int col;
	stats_rt rtcopy;

	memcpy(&rtcopy, rt, sizeof(rtcopy));
	sort(&rtcopy, sortcolumn);
	rt = &rtcopy;

	clear();

	move(0, 0);
	attron(COLOR_PAIR(COL_WHITE) | A_BOLD);
	printw("ggztop - real-time statistics from ggzd");
	attroff(COLOR_PAIR(COL_WHITE) | A_BOLD);

	move(1, 0);
	printw("Uptime: %i secs", rt->uptime);
	printw(" (%i in arrears)", time(NULL) - lastupdate);
	printw(", Rooms: %i", rt->num_rooms);
	printw(", Players: %i", rt->num_players);
	printw(", Tables: %i", rt->num_tables);

	move(3, 0);
	if(sortcolumn == 1)
		attron(COLOR_PAIR(COL_GREEN));
	printw("Room");
	if(sortcolumn == 1)
		attroff(COLOR_PAIR(COL_GREEN));
	move(3, 30);
	if(sortcolumn == 2)
		attron(COLOR_PAIR(COL_GREEN));
	printw("Players");
	if(sortcolumn == 2)
		attroff(COLOR_PAIR(COL_GREEN));
	move(3, 40);
	if(sortcolumn == 3)
		attron(COLOR_PAIR(COL_GREEN));
	printw("Tables");
	if(sortcolumn == 3)
		attroff(COLOR_PAIR(COL_GREEN));
	move(3, 50);
	if(sortcolumn == 4)
		attron(COLOR_PAIR(COL_GREEN));
	printw("Chat%%");
	if(sortcolumn == 4)
		attroff(COLOR_PAIR(COL_GREEN));

	for(i = 0; i < rt->num_rooms; i++)
	{
		for(j = 1; j < 10; j++)
		{
			lastchats[i][j - 1] = lastchats[i][j];
		}
		lastchats[i][9] = rt->chat[i];

		if((rt->players[i] > 0) || (showallrooms))
		{
			move(line, 0);
			col = COL_RED;
			if((rt->players[i] == 0) && (showallrooms))
				col = COL_YELLOW;
			attron(COLOR_PAIR(col));
			printw(rt->rooms[i]);
			attroff(COLOR_PAIR(col));

			move(line, 30);
			printw("%i", rt->players[i]);
			move(line, 40);
			printw("%i", rt->tables[i]);
			move(line, 50);
			printw("%i", lastchats[i][9] - lastchats[i][0]);

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
	int showallrooms;
	int sortcolumn;

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
		init_pair(COL_YELLOW, COLOR_YELLOW, -1);
		init_pair(COL_GREEN, COLOR_GREEN, -1);
	}

	redraw = 1;
	lastuptime = rt->uptime;
	showallrooms = 0;
	sortcolumn = 0;

	while(1)
	{
		if(rt->uptime > lastuptime)
		{
			lastuptime = rt->uptime;
			lastupdate = time(NULL);
		}

		if(redraw)
		{
			if(!rawmode)
				ggztop_display_top_screen(rt, showallrooms, sortcolumn);
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
				else if(input == '\t')
				{
					showallrooms = !showallrooms;
					redraw = 1;
				}
				else if((input >= '0') && (input <= '4'))
				{
					sortcolumn = input - '0';
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

