/*
 * Geekgame - a game which only real geeks understand
 * Copyright (C) 2002 - 2004 Josef Spillner, josef@ggzgamingzone.org

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* Configuration */
#include "config.h"

/* SDL include files */
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#ifdef HAVE_SOUND
#include <SDL/SDL_mixer.h>
#endif
#ifdef HAVE_XCURSOR
#include <X11/Xlib.h>
#include <X11/Xcursor/Xcursor.h>
#include <SDL/SDL_syswm.h>
#endif

/* GGZ include files */
#include <ggz.h>
#include <ggzmod.h>

/* System include files */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fnmatch.h>

/* Geekgame includes */
#include "wwwget.h"
#include "proto.h"

#include "config.h"

/* Hard-coded definitions */
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define ARRAY_WIDTH 20
#define ARRAY_HEIGHT 10

#define MATCH_SERVER "geekgame"
#define MATCH_VERSION 2

#define MAX_PLAYERS 5

#define STRING_LENGTH 1024

/* Global variables */
static GGZMod *mod = NULL;
static char *playerimage = NULL;
static int modfd;
static int ggzmode = 0;
static SDL_Surface *screen, *image;
static TTF_Font *font = NULL;
static char *fontpath = NULL;
static char *musicpath = NULL;
static char *soundpath = NULL;
static int playmode = -1;
static int players = 0;
static char scores[MAX_PLAYERS];
static char array[ARRAY_WIDTH][ARRAY_HEIGHT];
static int winner = -1;
static int usesound = 1;
static int usefullscreen = 0;
static int gamerunning = 0;
static int modemenu = 0;
static int arraywidth, arrayheight;
#ifdef HAVE_SOUND
Mix_Music *music = NULL;
Mix_Chunk *chunk = NULL;
int chunkchannel = -1;
#endif

/* Prototypes */
int startgame(void);
void loadsettings(void);
void savesettings(void);
void addplayer(const char *picture);
void musicdone(void);
void rendermode(int x, int y, const char *mode);

/* Return global read-only data directory */
static const char *data_global()
{
	return GGZDATADIR "/geekgame";
}

/* Return local read-write data directory */
static const char *data_local()
{
	static char *dl = NULL;
	char *home, *path;

	if(!dl)
	{
		home = getenv("HOME");
		path = "/.ggz/games/geekgame";
		dl = (char*)malloc(strlen(home) + strlen(path) + 1);
		sprintf(dl, "%s/%s", home, path);
	}

	return dl;
}

/* Protocol handler */
static void game_handle_io(void)
{
	unsigned char op;
	int version;
	char greeting[64];
	char player[64], pic[64];
	char path[STRING_LENGTH];
	int result;
	int i, j, field;
	
	if (ggz_read_char(modfd, &op) < 0)
	{
		/* ... */
		return;
	}

	printf("Read opcode: %i\n", op);
	switch(op)
	{
		case OP_GREETING:
			ggz_read_string(modfd, greeting, sizeof(greeting));
			ggz_read_int(modfd, &version);
			printf("Server is %s, version %i\n", greeting, version);
			if((!strcmp(greeting, MATCH_SERVER)) && (version == MATCH_VERSION))
			{
				ggz_write_char(modfd, OP_PRESENTATION);
				ggz_write_string(modfd, "dummy player");
				ggz_write_string(modfd, playerimage);
			}
			else rendermode(150, 170, "Version mismatch!       ");
			break;
		case OP_NUMPLAYERS:
			ggz_read_int(modfd, &players);
			break;
		case OP_NEWPLAYER:
			ggz_read_string(modfd, player, sizeof(player));
			ggz_read_string(modfd, pic, sizeof(pic));
			snprintf(path, sizeof(path), "%s/tmp.png", data_local());
			wwwget(pic, path);
			addplayer(NULL);
			break;
		case OP_GAMESTART:
			rendermode(150, 170, "Game has started!       ");
			gamerunning = 1;
			ggz_read_int(modfd, &arrayheight);
			ggz_read_int(modfd, &arraywidth);
			if((arrayheight > ARRAY_HEIGHT) || (arrayheight < 1))
				arrayheight = ARRAY_HEIGHT;
			if((arraywidth > ARRAY_WIDTH) || (arraywidth < 1))
				arraywidth = ARRAY_WIDTH;
			for(j = 0; j < arrayheight; j++)
				for(i = 0; i < arraywidth; i++)
				{
					ggz_read_int(modfd, &field);
					array[i][j] = field;
				}
			break;
		case OP_GAMESTOP:
			rendermode(150, 170, "Game stopped. Try again.");
			gamerunning = 0;
			break;
		case OP_GAMEEND:
			break;
		case OP_PRESENTATION:
			break;
		case OP_MOVERESULT:
			ggz_read_int(modfd, &result);
			break;
		case OP_MODERESULT:
			ggz_read_int(modfd, &result);
			break;
		default:
			/* discard */
			break;
	}
}

/* GGZMod event dispatcher */
static void handle_ggz()
{
	ggzmod_dispatch(mod);
}

/* GGZMod server event dispatcher */
static void handle_ggzmod_server(GGZMod *ggzmod, GGZModEvent e, void *data)
{
	int fd = *(int*)data;

	ggzmod_set_state(mod, GGZMOD_STATE_PLAYING);
	modfd = fd;

	/*players = ggzmod_get_num_seats(mod);*/ /* Doesn't work yet! */
}

/* Initialize GGZMod */
static void ggz_init()
{
	int ret;

	mod = ggzmod_new(GGZMOD_GAME);
	ggzmod_set_handler(mod, GGZMOD_EVENT_SERVER, &handle_ggzmod_server);

	ret = ggzmod_connect(mod);
	if(ret < 0)
	{
		fprintf(stderr, "Couldn't initialize GGZ.\n");
		exit(-1);
	}
}

/* Deinitialize GGZMod */
static void ggz_finish()
{
	ggzmod_disconnect(mod);
	ggzmod_free(mod);
}

/* Check for new GGZMod events */
static void ggz_network()
{
	int maxfd;
	fd_set set;
	struct timeval tv;
	int ret;
	int serverfd;

	FD_ZERO(&set);

	serverfd = ggzmod_get_fd(mod);

	FD_SET(serverfd, &set);
	maxfd = serverfd;
	if(modfd >= 0)
	{
		FD_SET(modfd, &set);
		if(modfd > maxfd) maxfd = modfd;
	}

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	ret = select(maxfd + 1, &set, NULL, NULL, &tv);

	if(ret)
	{
		if(modfd >= 0)
		{
			if(FD_ISSET(modfd, &set)) game_handle_io();
		}
		if(FD_ISSET(serverfd, &set)) handle_ggz();
	}
}

/* Draw a filled green or black box */
static void drawbox(int x, int y, int w, int h, SDL_Surface *screen,
		    int green, int autocrop)
{
	SDL_Rect rect;

	if(autocrop)
	{
		if(x < 20) x = 20;
		if(x + w > arraywidth * 32 + 18)
			x = arraywidth * 32 + 18 - w;
		if(y < 20) y = 20;
		if(y + h > arrayheight * 32 + 18)
			y = arrayheight * 32 + 18 - h;
	}
	else
	{
		if((x < 0) || (x + w > SCREEN_WIDTH - 5)) return;
		if((y < 0) || (y + h > SCREEN_HEIGHT - 5)) return;
	}

	rect.x = x;
	rect.y = y;
	rect.w = 2;
	rect.h = h + 3;

	SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, green, 0));

	rect.x = x + w + 2;

	SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, green, 0));

	rect.x = x;
	rect.w = w + 3;
	rect.h = 2;

	SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, green, 0));

	rect.y = y + h + 2;

	SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, green, 0));

	SDL_UpdateRect(screen, x, y, w + 3 + 1, h + 3 + 1);
}

/* Draw an image representing a number */
static void drawnumber(SDL_Surface *screen, int vx, int vy, int num)
{
	SDL_Surface *number;
	SDL_Rect rect;
	char path[STRING_LENGTH];
	char *image;

	rect.x = vx * 32 + 20;
	rect.y = vy * 32 + 20;

	if(num) image = "one.png";
	else image = "zero.png";
	snprintf(path, sizeof(path), "%s/%s", data_global(), image);
	number = IMG_Load(path);
	SDL_BlitSurface(number, NULL, screen, &rect);
	SDL_UpdateRect(screen, rect.x, rect.y, 32, 32);
}

/* Draw frame around player whose turn it is */
static void drawturn(SDL_Surface *screen, int players, int turn)
{
	SDL_Rect rect;
	int i;

	for(i = 0; i < players; i++)
	{
		rect.x = 20 + i * 150;
		rect.y = 450;
		rect.w = 128;
		rect.h = 128;

		drawbox(rect.x, rect.y, rect.w, rect.h, screen, (i == turn ? 255 : 120), 0);
	}
}

/* Draw box around selected level */
static void drawlevel(int player, int level)
{
	int i;
	for(i = 0; i < 10; i++)
	{
		drawbox(20 + player * 150 + i * 10, 420, 5, 20, screen, (i < level ? 220 : 100), 0);
	}
}

/* Main function, including commandline argument handling */
int main(int argc, char *argv[])
{
	struct option op[] =
	{
		{"fullscreen", no_argument, 0, 'f'},
		{"ggz", no_argument, 0, 'g'},
		{"help", no_argument, 0, 'h'},
		{"nosound", no_argument, 0, 'n'},
		{"players", required_argument, 0, 'p'},
		{0, 0, 0, 0}
	};
	int index = 0, c;
	int ret;

	while((c = getopt_long(argc, argv, "fghnp:", op, &index)) != -1)
	{
		switch(c)
		{
			case 'f':
				usefullscreen = 1;
				break;
			case 'g':
				ggzmode = 1;
				break;
			case 'h':
				printf("The Geek Game\n");
				printf("\n");
				printf("Recognized options:\n");
				printf("[-f | --fullscreen] Use full screen mode\n");
				printf("[-g | --ggz       ] Run game in GGZ mode\n");
				printf("[-h | --help      ] This help\n");
				printf("[-n | --nosound   ] Don't use sound or music\n");
				printf("[-p | --players=x ] Number of players for single player mode ");
				printf("(default: %i)\n", MAX_PLAYERS);
				return 0;
				break;
			case 'n':
				usesound = 0;
				break;
			case 'p':
				players = atoi(optarg);
				if((players <= 0) || (players > MAX_PLAYERS))
				{
					printf("Invalid player count %s, using %i\n", optarg, MAX_PLAYERS);
					players = MAX_PLAYERS;
				}
				break;
			default:
				fprintf(stderr, "Type '%s --help' to get a list of options.\n", argv[0]);
				return 1;
				break;
		}
	}

	printf("<<<< Geekgame >>>>\n");

	srand(time(NULL));

	loadsettings();

	if(ggzmode) ggz_init();

	ret = startgame();

	if(ggzmode) ggz_finish();

	return ret;
}

/* Load individual user settings */
void loadsettings(void)
{
	int conf;
	char conffile[STRING_LENGTH];

	snprintf(conffile, sizeof(conffile), "%s/config", data_local());
	conf = ggz_conf_parse(conffile, GGZ_CONF_RDONLY);
	if(conf != -1)
	{
		fontpath = ggz_conf_read_string(conf, "Configuration", "fontpath", NULL);
		musicpath = ggz_conf_read_string(conf, "Configuration", "musicpath", NULL);
		soundpath = ggz_conf_read_string(conf, "Configuration", "soundpath", NULL);

		ggz_conf_close(conf);
	}

	snprintf(conffile, sizeof(conffile), "%s/.ggz/personalization", getenv("HOME"));
	conf = ggz_conf_parse(conffile, GGZ_CONF_RDONLY);
	if(conf != -1)
	{
		playerimage = ggz_conf_read_string(conf, "Personalization", "picture", NULL);

		ggz_conf_close(conf);
	}

	if((conf == -1) || (!playerimage))
	{
		playerimage = (char*)malloc(STRING_LENGTH);
		snprintf(playerimage, STRING_LENGTH, "%s/default.png", data_global());
	}
}

/* Save settings into user preference file */
void savesettings(void)
{
	int conf;
	char conffile[STRING_LENGTH];

	snprintf(conffile, sizeof(conffile), "%s/config", data_local());
	conf = ggz_conf_parse(conffile, GGZ_CONF_RDWR | GGZ_CONF_CREATE);
	if(conf != -1)
	{
		ggz_conf_write_string(conf, "Configuration", "fontpath", fontpath);
		ggz_conf_write_string(conf, "Configuration", "musicpath", musicpath);
		ggz_conf_write_string(conf, "Configuration", "soundpath", soundpath);

		ggz_conf_commit(conf);
		ggz_conf_close(conf);
	}
}

/* Add a player to the visual player gallery */
void addplayer(const char *picture)
{
	static int counter = 0;
	SDL_Rect rect;
	char path[STRING_LENGTH];

	snprintf(path, sizeof(path), "%s/tmp.png", data_local());
	image = IMG_Load(path);

	if(image)
	{
		rect.x = 20 + counter * 150;
		rect.y = 450;
		SDL_BlitSurface(image, NULL, screen, &rect);
		SDL_UpdateRect(screen, rect.x, rect.y, image->w, image->h);

		drawbox(rect.x, rect.y, image->w, image->h, screen, 255, 0);
	}

	counter++;
}

/* Simple textual messages */
void rendermode(int x, int y, const char *mode)
{
	SDL_Surface *text;
	SDL_Rect rect;

	SDL_Color green = {0x3F, 0xFF, 0x5F, 0};

	text = TTF_RenderText_Solid(font, mode, green);
	if(text)
	{
		rect.x = x;
		rect.y = y;
		rect.h = text->h;
		rect.w = text->w;
		SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 0, 0));
		SDL_BlitSurface(text, NULL, screen, &rect);
		SDL_FreeSurface(text);
		SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);
	}
}

/* Draw scores of players */
static void renderscore(int x, int y, int sum)
{
	char score[8];
	SDL_Surface *text;
	SDL_Rect rect;

	SDL_Color black = {0xFF, 0xFF, 0xFF, 0};

	snprintf(score, sizeof(score), "%i", sum);
	text = TTF_RenderText_Solid(font, score, black);
	if(text)
	{
		rect.x = x;
		rect.y = y;
		rect.w = 32;
		rect.h = text->h;
		SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 0, 0));
		rect.w = text->w;
		SDL_BlitSurface(text, NULL, screen, &rect);
		rect.w = 32;
		SDL_FreeSurface(text);
		SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);
	}
}

/* Draw multi-lines game mode descriptions */
static void renderdesc(int x, int y, const char *desc, int active)
{
	SDL_Surface *text;
	SDL_Rect rect;
	char *token;
	char *tmp;

	SDL_Color white = {0xFF, 0xFF, 0xFF, 0};
	SDL_Color black = {0x00, 0x00, 0x00, 0};

	tmp = strdup(desc);
	token = strtok(tmp, "\n");
	while(token)
	{
		text = TTF_RenderText_Solid(font, token, (active ? white : black));
		if(text)
		{
			rect.x = x;
			rect.y = y;
			rect.w = text->w/*300*/;
			rect.h = text->h/*100*/;
			SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 0, 0));
			rect.w = text->w/*300*/;
			SDL_BlitSurface(text, NULL, screen, &rect);
			rect.w = text->w/*300*/;
			SDL_FreeSurface(text);
			SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);
		}
		token = strtok(NULL, "\n");
		y += 30;
	}
	free(tmp);
}

/* Play a sound effect */
static void playnoise(void)
{
#ifdef HAVE_SOUND
	if(chunk)
	{
		Mix_Volume(-1, 50);
		chunkchannel = Mix_PlayChannel(-1, chunk, 0);
	}
#endif
}

/* Scan directory for files matching pattern */
static char *scan_dir(const char *dir, const char *pattern)
{
	DIR *dp;
	struct dirent *ep;

	dp = opendir(dir);
	if(dp)
	{
		while((ep = readdir(dp)) != 0)
		{
			if(!fnmatch(pattern, ep->d_name, 0))
			{
				closedir(dp);
				return ep->d_name;
			}
		}
		closedir(dp);
	}
	return NULL;
}

/* Return random file matching a pattern from a directory */
static char *scan_file(const char *dir, const char *pattern)
{
	char *tmpfile;
	char *ret;
	char path[STRING_LENGTH];

	tmpfile = scan_dir(dir, pattern);
	ret = NULL;
	if(tmpfile)
	{
		snprintf(path, sizeof(path), "%s/%s", dir, tmpfile);
		ret = strdup(path);
	}

	return ret;
}

/* Display notice while scanning for files */
static void screen_scanning(int display)
{
	SDL_Rect rect;
	char path[STRING_LENGTH];

	if(display)
	{
		rect.x = 0;
		rect.y = 0;
		rect.w = SCREEN_WIDTH;
		rect.h = SCREEN_HEIGHT;
		SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 0, 0));

		renderdesc(50, 50, "Scanning...", 1);

		SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);

		/* Scanning for other stuff */
		/*sleep(1);*/
	}
	else
	{
		/* Blindly create local directory structure */
		mkdir(data_local(), S_IRWXU);
		snprintf(path, sizeof(path), "%s/music", data_local());
		mkdir(path, S_IRWXU);
		snprintf(path, sizeof(path), "%s/sound", data_local());
		mkdir(path, S_IRWXU);
		snprintf(path, sizeof(path), "%s/fonts", data_local());
		mkdir(path, S_IRWXU);

		/* Scanning for fonts */
		if(!fontpath)
		{
			snprintf(path, sizeof(path), "%s/fonts", data_local());
			fontpath = scan_file(path, "*.ttf");
		}
		if(!fontpath)
		{
			snprintf(path, sizeof(path), "%s/fonts", data_global());
			fontpath = scan_file(path, "*.ttf");
		}
		if(!fontpath) fontpath = scan_file("/usr/share/fonts/truetype", "*.ttf");
		if(!fontpath) fontpath = scan_file("/usr/share/fonts/truetype/freefont", "*.ttf");
		if(!fontpath) fontpath = scan_file("/usr/X11R6/lib/X11/fonts/TrueType", "*.ttf");

		/* Scanning for music */
		if(!musicpath)
		{
			snprintf(path, sizeof(path), "%s/music", data_local());
			musicpath = scan_file(path, "*.ogg");
			if(!musicpath) musicpath = scan_file(path, "*.mp3");
			if(!musicpath) musicpath = scan_file(path, "*.wav");
		}
		if(!musicpath)
		{
			snprintf(path, sizeof(path), "%s/music", data_global());
			musicpath = scan_file(path, "*.ogg");
			if(!musicpath) musicpath = scan_file(path, "*.mp3");
			if(!musicpath) musicpath = scan_file(path, "*.wav");
		}

		/* Scanning for sound effects */
		if(!soundpath)
		{
			snprintf(path, sizeof(path), "%s/sound", data_local());
			soundpath = scan_file(path, "*.ogg");
			if(!soundpath) musicpath = scan_file(path, "*.mp3");
			if(!soundpath) musicpath = scan_file(path, "*.wav");
		}
		if(!musicpath)
		{
			snprintf(path, sizeof(path), "%s/sound", data_global());
			soundpath = scan_file(path, "*.ogg");
			if(!soundpath) musicpath = scan_file(path, "*.mp3");
			if(!soundpath) musicpath = scan_file(path, "*.wav");
		}
	}
}

static void showcursor(int mode)
{
#ifdef HAVE_XCURSOR
	Cursor cursor;
	SDL_SysWMinfo info;
	int ret;
#endif

	SDL_ShowCursor(mode);

#ifdef HAVE_XCURSOR
	if(mode == SDL_ENABLE)
	{
		info.version.major = SDL_MAJOR_VERSION;
		ret = SDL_GetWMInfo(&info);
		if(ret == 1)
		{
			info.info.x11.lock_func();
			cursor = XcursorLibraryLoadCursor(info.info.x11.display, "redglass");
			XDefineCursor(info.info.x11.display, info.info.x11.window, cursor);
			info.info.x11.unlock_func();
		}
	}
#endif
}

/* Intro menu with game mode selection and player gallery */
static void screen_intro(int firsttime)
{
	int escape;
	SDL_Event event;
	Uint8 *keystate;
	int x, y, i, oldy, oldmodemenu;
	int dimmer, dimminc;
	SDL_Rect rect;
	char *desc1, *desc2, *desc3, *desc4;
	char path[STRING_LENGTH];
	char *rule[] = {"easy", "matrix", "havoc", "hax0r"};

	x = 20;
	y = modemenu * 30 + 20;
	oldy = y;
	oldmodemenu = modemenu;

	escape = 0;
	playmode = -1;
	dimmer = 250;
	dimminc = -1;

	rect.x = 0;
	rect.y = 0;
	rect.w = SCREEN_WIDTH;
	rect.h = SCREEN_HEIGHT;
	SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 0, 0));
	SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);

	showcursor(SDL_ENABLE);

	rendermode(22, 24, rule[0]);
	rendermode(22, 54, rule[1]);
	rendermode(22, 84, rule[2]);
	rendermode(22, 114, rule[3]);

	if(!ggzmode)
	{
		for(i = 0; i < players; i++)
		{
			if(firsttime == 1) SDL_Delay(200);
	
			if(!i)
			{
				snprintf(path, sizeof(path), "%s/tmp.png", data_local());
				wwwget(playerimage, path);
				image = IMG_Load(path);
				if(!image)
				{
					snprintf(path, sizeof(path), "%s/default.png", data_global());
					image = IMG_Load(path);
				}
			} else image = NULL;

			if(!image)
			{
				snprintf(path, sizeof(path), "%s/bot.png", data_global());
				image = IMG_Load(path);
			}

			rect.x = 20 + i * 150;
			rect.y = 450;
			if(image)
			{
				SDL_BlitSurface(image, NULL, screen, &rect);
				SDL_UpdateRect(screen, rect.x, rect.y, image->w, image->h);

				drawbox(rect.x, rect.y, image->w, image->h, screen, 255, 0);
			}
		}
	}
	else rendermode(150, 170, "Waiting for players...  ");

	while(!escape)
	{
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
					keystate = SDL_GetKeyState(NULL);
					if(keystate[SDLK_ESCAPE]) escape = 1;
					if(keystate[SDLK_DOWN])
					{
						if(modemenu < 3)
						{
							drawbox(x, y, 150, 20, screen, 0, 1);
							oldy = y;
							y += 30;
							oldmodemenu = modemenu;
							modemenu++;
						}
					}
					if(keystate[SDLK_UP])
					{
						if(modemenu > 0)
						{
							drawbox(x, y, 150, 20, screen, 0, 1);
							oldy = y;
							y -= 30;
							oldmodemenu = modemenu;
							modemenu--;
						}
					}
					if(keystate[SDLK_RETURN])
					{
						if((!ggzmode) || (gamerunning))
						{
							escape = 1;
							playmode = modemenu + MODE_RESERVED + 1;
							if(ggzmode)
							{
								ggz_write_char(modfd, OP_RULESET);
								ggz_write_string(modfd, rule[playmode - MODE_RESERVED - 1]);
							}
						}
						playnoise();
					}
					if(keystate[SDLK_f])
					{
						SDL_WM_ToggleFullScreen(screen);
						showcursor(SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE ? SDL_DISABLE : SDL_ENABLE);
					}
					break;
			}
		}

		if(oldy)
		{
			desc1 = "The sum of any row or column\nmust be zero.";
			desc2 = "The sum of any row and column,\nmultiplied by 2, must be 42.";
			desc3 = "Numbers encompassed by cursor\nmust divide by 4\nin binary coded format.";
			desc4 = "One cursor bar must\ncontain the same number\nof ones as the other one zeroes.";

			if(oldmodemenu == 0)
			{
				renderdesc(150, 200, desc1, 0);
			}
			else if(oldmodemenu == 1)
			{
				renderdesc(150, 200, desc2, 0);
			}
			else if(oldmodemenu == 2)
			{
				renderdesc(150, 200, desc3, 0);
			}
			else if(oldmodemenu == 3)
			{
				renderdesc(150, 200, desc4, 0);
			}

			if(modemenu == 0)
			{
				renderdesc(150, 200, desc1, 1);
			}
			else if(modemenu == 1)
			{
				renderdesc(150, 200, desc2, 1);
			}
			else if(modemenu == 2)
			{
				renderdesc(150, 200, desc3, 1);
			}
			else if(modemenu == 3)
			{
				renderdesc(150, 200, desc4, 1);
			}

			oldy = 0;
		}

		drawbox(x, y, 150, 20, screen, dimmer, 1);

		dimmer = dimmer + dimminc * 3;
		if((dimmer <= 150) || (dimmer >= 250)) dimminc = -dimminc;

		SDL_Delay(50);

		if(ggzmode) ggz_network();
	}
}

/* Handle input */
static int gameinput(int *ox, int *oy, int userinput)
{
	SDL_Event event;
	Uint8 *keystate;
	int x, y, wantx, wanty, found;
	int dimmer;
	int dimminc;
	enum Directions {right, left, up, down, none, done};
	int march;
	int tries, i, j, k, l, zeroes, minzeroes;
	int sum, sum2, bestsum;
	int sx, sy;

	x = *ox;
	y = *oy;

	dimmer = 250;
	dimminc = -1;
	found = 0;
	wantx = 0;
	wanty = 0;

	march = none;

	while(1)
	{
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
					keystate = SDL_GetKeyState(NULL);
					if(keystate[SDLK_ESCAPE]) return 0;
					if(userinput)
					{
						if(keystate[SDLK_RIGHT])
							march = right;
						if(keystate[SDLK_LEFT])
							march = left;
						if(keystate[SDLK_DOWN])
							march = down;
						if(keystate[SDLK_UP])
							march = up;
						if(keystate[SDLK_RETURN])
							march = done;
					}
					if(keystate[SDLK_f])
					{
						SDL_WM_ToggleFullScreen(screen);
						showcursor(SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE ? SDL_DISABLE : SDL_ENABLE);
					}
					break;
				case SDL_KEYUP:
					march = none;
					break;
			}
		}

		if((march == right) && (x >= (arraywidth - 1) * 32)) march = none;
		else if((march == left) && (x < 1 * 32)) march = none;
		else if((march == down) && (y >= (arrayheight - 1) * 32)) march = none;
		else if((march == up) && (y < 1 * 32)) march = none;

		if(!userinput)
		{
			if(!found)
			{
				wantx = -1;
				wanty = -1;
				if(playmode == MODE_EASY)
				{
					minzeroes = arraywidth * arrayheight;
					for(j = 0; j < arrayheight; j++)
					{
						zeroes = 0;
						for(i = 0; i < arraywidth; i++) zeroes += array[i][j];
						if((zeroes < minzeroes) && (zeroes > 0))
						{
							minzeroes = zeroes;
							wantx = -1;
							wanty = j;
							for(tries = 0; ((tries < arraywidth) && (wantx == -1)); tries++)
							{
								i = rand() % arraywidth;
								if(array[i][j]) wantx = i;
							}
							for(i = 0; i < arraywidth; i++)
								if(array[i][j]) wantx = i;
						}
					}
					for(i = 0; i < arraywidth; i++)
					{
						zeroes = 0;
						for(j = 0; j < arrayheight; j++) zeroes += array[i][j];
						if((zeroes < minzeroes) && (zeroes > 0))
						{
							minzeroes = zeroes;
							wantx = i;
							wanty = -1;
							for(tries = 0; ((tries < arrayheight) && (wanty == -1)); tries++)
							{
								j = rand() % arrayheight;
								if(array[i][j]) wanty = j;
							}
							for(j = 0; j < arrayheight; j++)
								if(array[i][j]) wanty = j;
						}
					}
				}
				else if(playmode == MODE_MATRIX)
				{
					bestsum = 0;
					for(j = 0; j < arrayheight; j++)
					{
						for(i = 0; i < arraywidth; i++)
						{
							sum = 0;
							for(l = 0; l < arrayheight; l++)
								sum += array[i][l];
							for(k = 0; k < arraywidth; k++)
								sum += array[k][j];
							sum = sum - array[i][j] * 2 + (!array[i][j]) * 2;
							sum *= 2;
							if(abs(sum - 42) < abs(bestsum - 42))
							{
								if(!((sum + 2) % 4))
								{
									printf("bestsum is %i\n", sum);
									bestsum = sum;
									wantx = i;
									wanty = j;
								}
							}
						}
					}
				}
				else if(playmode == MODE_HAVOC)
				{
					for(j = 0; j < arrayheight; j++)
					{
						for(i = 0; i < arraywidth; i++)
						{
							sum = 0;
							sx = i - 3;
							sy = j - 3;
							if(sx < 0) sx = 0;
							if(sy < 0) sy = 0;
							if(sx + 6 >= arraywidth) sx = arraywidth - 6 - 1;
							if(sy + 6 >= arrayheight) sy = arrayheight - 6 - 1;
							for(l = sy; l <= sy + 6; l++)
								sum += array[i][l];
							for(k = sx; k <= sx + 6; k++)
								sum += array[k][j];
							sum = sum - array[i][j] * 2 + (!array[i][j]);
							if(sum == (sum & ~0x03))
							{
								wantx = i;
								wanty = j;
							}
						}
					}
				}
				else if(playmode == MODE_HAX0R)
				{
					for(j = 0; j < arrayheight; j++)
					{
						if((wantx != -1) && (wanty != -1) && (!(rand() % 6)))
							break;
						for(i = 0; i < arraywidth; i++)
						{
							sum = 0;
							sum2 = 0;
							sx = i - 3;
							sy = j - 3;
							if(sx < 0) sx = 0;
							if(sy < 0) sy = 0;
							if(sx + 6 >= arraywidth) sx = arraywidth - 6 - 1;
							if(sy + 6 >= arrayheight) sy = arrayheight - 6 - 1;
							for(l = sy; l <= sy + 6; l++)
								sum += array[i][l];
							for(k = sx; k <= sx + 6; k++)
								sum2 += !array[k][j];
							sum = sum - array[i][j] + !array[i][j];
							sum2 = sum2 - !array[i][j] + array[i][j];
							if(sum == sum2)
							{
								wantx = i;
								wanty = j;
								if(!(rand() % 3)) break;
							}
						}
					}
				}

				if((wantx == -1) || (wanty == -1))
				{
					wantx = (rand() % arraywidth) * 32 + 18;
					wanty = (rand() % arrayheight) * 32 + 18;
				}
				else
				{
					wantx = wantx * 32 + 18;
					wanty = wanty * 32 + 18;
				}
				found = 1;
			}

			if(wantx < x) march = left;
			else if(wantx > x) march = right;
			else if(wanty < y) march = up;
			else if(wanty > y) march = down;
			else march = done;

			SDL_Delay(100);
		}

		if(march == right)
		{
			drawbox(x, y, 32, 32, screen, 0, 1);
			drawbox(x - 96, y, 224, 32, screen, 0, 1);
			drawbox(x, y - 96, 32, 224, screen, 0, 1);
			x += 32;
		}
		if(march == left)
		{
			drawbox(x, y, 32, 32, screen, 0, 1);
			drawbox(x - 96, y, 224, 32, screen, 0, 1);
			drawbox(x, y - 96, 32, 224, screen, 0, 1);
			x -= 32;
		}
		if(march == down)
		{
			drawbox(x, y, 32, 32, screen, 0, 1);
			drawbox(x - 96, y, 224, 32, screen, 0, 1);
			drawbox(x, y - 96, 32, 224, screen, 0, 1);
			y += 32;
		}
		if(march == up)
		{
			drawbox(x, y, 32, 32, screen, 0, 1);
			drawbox(x - 96, y, 224, 32, screen, 0, 1);
			drawbox(x, y - 96, 32, 224, screen, 0, 1);
			y -= 32;
		}
		if(march == done)
		{
			*ox = x;
			*oy = y;
			return 1;
		}

		drawbox(x - 96, y, 224, 32, screen, dimmer / 2, 1);
		drawbox(x, y - 96, 32, 224, screen, dimmer / 2, 1);
		drawbox(x, y, 32, 32, screen, dimmer, 1);

		dimmer = dimmer + dimminc * 3;
		if((dimmer <= 150) || (dimmer >= 250)) dimminc = -dimminc;

		SDL_Delay(50);

		if(ggzmode) ggz_network();
	}
	return 0;
}

/* Game screen with player gallery and game board */
static void screen_game(void)
{
	int escape;
	int i, j;
	int x, y, sx, sy;
	int turn;
	int sum, sum2;
	int calc;
	int makescore;
	int ret;

	calc = 0;
	winner = -1;
	turn = 0;

	escape = 0;

	x = 50;
	y = 50;
	drawbox(x, y, 32, 32, screen, 250, 0);

	while(1)
	{
		if(turn == 0)
		{
			ret = gameinput(&x, &y, 1);
		}
		else
		{
			ret = gameinput(&x, &y, 0);
		}

		if(ret)
		{
			array[x / 32][y / 32] = !array[x / 32][y / 32];
			drawnumber(screen, x / 32, y / 32, array[x / 32][y / 32]);
			calc = 1;
			playnoise();
			if(ggzmode)
			{
				ggz_write_char(modfd, OP_MOVE);
				ggz_write_int(modfd, x / 32);
				ggz_write_int(modfd, y / 32);
			}
		}
		else escape = 1;

		if(escape) break;

		if(calc)
		{
			calc = 0;

			makescore = 0;
			switch(playmode)
			{
				case MODE_EASY:
					sum = 0;
					i = x / 32;
					for(j = 0; j < arrayheight; j++)
						sum += array[i][j];
					renderscore(x + 10, 360, sum);
					if(!sum) makescore = 1;

					sum = 0;
					j = y / 32;
					for(i = 0; i < arraywidth; i++)
						sum += array[i][j];
					renderscore(680, y + 10, sum);
					if(!sum) makescore = 1;
					break;
				case MODE_MATRIX:
					sum = 0;
					i = x / 32;
					for(j = 0; j < arrayheight; j++)
						sum += array[i][j];
					renderscore(x + 10, 360, sum * 2);

					sum2 = 0;
					j = y / 32;
					for(i = 0; i < arraywidth; i++)
						sum2 += array[i][j];
					renderscore(680, y + 10, sum2 * 2);

					if((sum + sum2) * 2 == 42) makescore = 1;
					break;
				case MODE_HAVOC:
					sum = 0;
					sx = x / 32 - 3;
					sy = y / 32 - 3;
					if(sx < 0) sx = 0;
					if(sy < 0) sy = 0;
					if(sx + 6 >= arraywidth) sx = arraywidth - 6 - 1;
					if(sy + 6 >= arrayheight) sy = arrayheight - 6 - 1;
					for(j = sy; j <= sy + 6; j++)
						sum += array[x / 32][j];
					for(i = sx; i <= sx + 6; i++)
						sum += array[i][y / 32];
					sum -= array[x / 32][y / 32];
					if(sum == (sum & ~0x03)) makescore = 1;
					break;
				case MODE_HAX0R:
					sum = 0;
					sum2 = 0;
					sx = x / 32 - 3;
					sy = y / 32 - 3;
					if(sx < 0) sx = 0;
					if(sy < 0) sy = 0;
					if(sx + 6 >= arraywidth) sx = arraywidth - 6 - 1;
					if(sy + 6 >= arrayheight) sy = arrayheight - 6 - 1;

					for(j = sy; j <= sy + 6; j++)
						sum += array[x / 32][j];
					for(i = sx; i <= sx + 6; i++)
						sum2 += !array[i][y / 32];
					if(sum == sum2) makescore = 1;
					break;
			}

			if(!ggzmode)
			{
				if(makescore) scores[turn]++;
				if(scores[turn] == 10)
				{
					escape = 1;
					winner = turn;
				}
				drawlevel(turn, scores[turn]);
			}

			turn = (turn + 1) % players;
			drawturn(screen, players, turn);
		}
	}
}

/* Game over menu */
static void screen_outtro(void)
{
	int escape;
	SDL_Rect rect;
	SDL_Event event;
	Uint8 *keystate;

	rect.x = 0;
	rect.y = 0;
	rect.w = SCREEN_WIDTH;
	rect.h = SCREEN_HEIGHT;
	SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 0, 0));
	rendermode(22, 24, "The game is over.");
	if(!ggzmode)
	{
		if(!winner)
			rendermode(22, 54, "You defeated the bots.");
		else
			rendermode(22, 54, "You lost against the bots.");
	}
	renderdesc(22, 84, "Press escape to return to the main menu.", 1);
	SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);

	escape = 0;
	while(!escape)
	{
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
					keystate = SDL_GetKeyState(NULL);
					if(keystate[SDLK_ESCAPE]) escape = 1;
			}
		}
	}
}

/* Program initialization */
int startgame(void)
{
	int i, j, x;
	SDL_Surface *icon;
	Uint32 init;
	char path[STRING_LENGTH];
	int step;

	init = SDL_INIT_VIDEO;
#ifdef HAVE_SOUND
	if(usesound) init |= SDL_INIT_AUDIO;
#endif
	SDL_Init(init);

	TTF_Init();

#ifdef HAVE_SOUND
	if(usesound)
	{
		int ret = Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048);
		if(ret < 0)
		{
			fprintf(stderr, "Could not open sound device.\n");
		}
		else
		{
			if(!musicpath)
			{
				snprintf(path, sizeof(path), "%s/music/song32.ogg", data_global());
				musicpath = strdup(path);
			}
			music = Mix_LoadMUS(musicpath);
			Mix_PlayMusic(music, -1);
			Mix_HookMusicFinished(musicdone);

			if(!soundpath)
			{
				snprintf(path, sizeof(path), "%s/sound/phaser.wav", data_global());
				soundpath = strdup(path);
			}
			chunk = Mix_LoadWAV(soundpath);
		}
	}
#endif

	screen_scanning(0);

	font = TTF_OpenFont(fontpath, 18);
	if(!font)
	{
		fprintf(stderr, "Could not open font file.\n");
		return -1;
	}
	TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

	SDL_WM_SetCaption("GGZ Geek Game", "GGZ Geek Game");
	snprintf(path, sizeof(path), "%s/icon.png", data_global());
	icon = IMG_Load(path);
	if(icon) SDL_WM_SetIcon(icon, 0);

	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 24, (usefullscreen ? SDL_FULLSCREEN : 0));

	if(!players) players = MAX_PLAYERS;

	arraywidth = ARRAY_WIDTH;
	arrayheight = ARRAY_HEIGHT;

	drawturn(screen, players, -1);

	screen_scanning(1);

	savesettings();

	step = 0;
	while(1)
	{
		step++;
		screen_intro(step);

		if(playmode < 0)
		{
			SDL_Quit();
			return 0;
		}

		drawturn(screen, players, 0);

		for(j = 0; j < arrayheight; j++)
		{
			for(i = 0; i < arraywidth; i++)
			{
				x = rand() % 2;
				drawnumber(screen, i, j, x);
				array[i][j] = x;
			}
		}

		for(i = 0; i < players; i++)
			scores[i] = 0;

		screen_game();

		if(winner >= 0) screen_outtro();
	}

#ifdef HAVE_SOUND
	if(usesound)
	{
		if(chunkchannel >= 0) Mix_HaltChannel(chunkchannel);
		chunkchannel = -1;
		chunk = NULL;
		Mix_CloseAudio();
		music = NULL;
	}
#endif
	SDL_Quit();

	return 0;
}

/* Shutdown music system */
void musicdone(void)
{
#ifdef HAVE_SOUND
	if(chunkchannel >= 0) Mix_HaltChannel(chunkchannel);
	chunkchannel = -1;
	chunk = NULL;
	Mix_HaltMusic();
	Mix_FreeMusic(music);
	music = NULL;
#endif
}

