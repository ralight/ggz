#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include <ggz.h>
#include <ggzmod.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include "wwwget.h"

#include "config.h"

#define ARRAY_WIDTH 20
#define ARRAY_HEIGHT 10

#define DATA_GLOBAL GGZDATADIR "/geekgame/"
#define DATA_LOCAL "/tmp/"

static GGZMod *mod = NULL;
static int modfd;
static int ggzmode = 0;

static void game_handle_io(void)
{
	int op, i;
	
	if (ggz_read_int(modfd, &op) < 0)
	{
		/* ... */
		return;
	}

	printf("Read opcode: %i\n", op);
}

static void handle_ggz()
{
	ggzmod_dispatch(mod);
}

static void handle_ggzmod_server(GGZMod *ggzmod, GGZModEvent e, void *data)
{
	int fd = *(int*)data;

	ggzmod_set_state(mod, GGZMOD_STATE_PLAYING);
	modfd = fd;
}

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

static void ggz_finish()
{
	ggzmod_disconnect(mod);
	ggzmod_free(mod);
}

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

void drawbox(int x, int y, int w, int h, SDL_Surface *screen, int green)
{
	SDL_Rect rect;

	rect.x = x;
	rect.y = y;
	rect.w = 1;
	rect.h = h + 3;

	SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, green, 0));

	rect.x = x + w + 2;

	SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, green, 0));

	rect.x = x;
	rect.w = w + 3;
	rect.h = 1;

	SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, green, 0));

	rect.y = y + h + 2;

	SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, green, 0));

	SDL_UpdateRect(screen, x, y, w + 3, h + 3);
}

void drawnumber(SDL_Surface *screen, int vx, int vy, int num)
{
	SDL_Surface *number;
	SDL_Rect rect;

	rect.x = vx * 32 + 20;
	rect.y = vy * 32 + 20;

	if(num) number = IMG_Load(DATA_GLOBAL "one.png");
	else number = IMG_Load(DATA_GLOBAL "zero.png");
	SDL_BlitSurface(number, NULL, screen, &rect);
	SDL_UpdateRect(screen, rect.x, rect.y, 32, 32);
}

void drawturn(SDL_Surface *screen, int players, int turn)
{
	SDL_Rect rect;
	int i;

	for(i = 0; i < players; i++)
	{
		rect.x = 20 + i * 150;
		rect.y = 450;
		rect.w = 128;
		rect.h = 128;

		drawbox(rect.x, rect.y, rect.w, rect.h, screen, (i == turn ? 255 : 120));
	}
}

int startgame(void);

int main(int argc, char *argv[])
{
	struct option op[] =
	{
		{"ggz", 0, 0, 'g'},
		{"help", 0, 0, 'h'},
		{0, 0, 0, 0}
	};
	int index = 0, c;
	int ret;

	while((c = getopt_long(argc, argv, "g", op, &index)) != -1)
	{
		switch(c)
		{
			case 'g':
				ggzmode = 1;
				break;
			case 'h':
				printf("The Geek Game\n");
				printf("\n");
				printf("Recognized options:\n");
				printf("[-g | --ggz ] Run game in GGZ mode\n");
				printf("[-h | --help] This help\n");
				return 0;
				break;
			default:
				fprintf(stderr, "Type '%s --help' to get a list of options.\n", argv[0]);
				return 1;
				break;
		}
	}

	if(ggzmode) ggz_init();

	ret = startgame();

	if(ggzmode) ggz_finish();

	return ret;
}

int startgame(void)
{
	SDL_Surface *screen, *image;
	SDL_Rect rect;
	SDL_Event event;
	int i, j;
	Uint8 *keystate;
	int escape;
	int x, y;
	char array[ARRAY_WIDTH][ARRAY_HEIGHT];
	int dimmer;
	int dimminc;
	int calc;
	char score[8];
	int sum;
	int players;
	int turn;
	char *playerimage;
	int conf;
	char conffile[1024];

	TTF_Font *font;
	SDL_Surface *text;

	SDL_Init(SDL_INIT_VIDEO);

	TTF_Init();

	font = TTF_OpenFont(DATA_GLOBAL "1979rg__.ttf", 12);
	/*font = TTF_OpenFont("dark2.ttf", 18);*/
	if(!font)
	{
		fprintf(stderr, "Could not open font file.\n");
		return -1;
	}
	TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

	SDL_WM_SetCaption("GGZ Geek Game", "GGZ Geek Game");

	screen = SDL_SetVideoMode(800, 600, 24, 0);

	players = 4;

	drawturn(screen, players, -1);

	snprintf(conffile, sizeof(conffile), "%s/.ggz/personalization", getenv("HOME"));
	conf = ggz_conf_parse(conffile, GGZ_CONF_RDONLY);
	if(conf != -1)
	{
		playerimage = ggz_conf_read_string(conf, "Personalization", "picture", NULL);
	}
	if((conf == -1) || (!playerimage)) playerimage = DATA_GLOBAL "bot.png";

	for(i = 0; i < players; i++)
	{
		sleep(1);

		if(!i)
		{
			wwwget("http://mindx.dyndns.org/homepage/photos/josef.png", DATA_LOCAL "tmp.png");
			image = IMG_Load(DATA_LOCAL "tmp.png");
		}
		else image = IMG_Load(DATA_GLOBAL "bot.png");

		rect.x = 20 + i * 150;
		rect.y = 450;
		SDL_BlitSurface(image, NULL, screen, &rect);
		SDL_UpdateRect(screen, rect.x, rect.y, image->w, image->h);

		drawbox(rect.x, rect.y, image->w, image->h, screen, 255);
	}

	turn = 0;
	drawturn(screen, players, 0);

	for(j = 0; j < ARRAY_HEIGHT; j++)
	{
		for(i = 0; i < ARRAY_WIDTH; i++)
		{
			x = rand() % 2;
			drawnumber(screen, i, j, x);
			array[i][j] = x;
		}
	}

	calc = 0;

	dimmer = 250;
	dimminc = -1;

	x = 50;
	y = 50;
	drawbox(x, y, 32, 32, screen, dimmer);

	escape = 0;
	while(1)
	{
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
					keystate = SDL_GetKeyState(NULL);
					if(keystate[SDLK_ESCAPE]) escape = 1;
					if(keystate[SDLK_RIGHT])
					{
						if(x < (ARRAY_WIDTH - 1) * 32)
						{
							drawbox(x, y, 32, 32, screen, 0);
							x += 32;
						}
					}
					if(keystate[SDLK_LEFT])
					{
						if(x >= 1 * 32)
						{
							drawbox(x, y, 32, 32, screen, 0);
							x -= 32;
						}
					}
					if(keystate[SDLK_DOWN])
					{
						if(y <= (ARRAY_HEIGHT - 1) * 32)
						{
							drawbox(x, y, 32, 32, screen, 0);
							y += 32;
						}
					}
					if(keystate[SDLK_UP])
					{
						if(y >= 1 * 32)
						{
							drawbox(x, y, 32, 32, screen, 0);
							y -= 32;
						}
					}
					if(keystate[SDLK_RETURN])
					{
						array[x / 32][y / 32] = !array[x / 32][y / 32];
						drawnumber(screen, x / 32, y / 32, array[x / 32][y / 32]);
						calc = 1;
					}
					break;
			}
		}

		drawbox(x, y, 32, 32, screen, dimmer);

		dimmer = dimmer + dimminc * 3;
		if((dimmer <= 100) || (dimmer >= 250)) dimminc = -dimminc;

		if(escape) break;

		if(calc)
		{
			calc = 0;

			SDL_Color black = {0xFF, 0xFF, 0xFF, 0};

			sum = 0;
			i = 0;
			for(j = 0; j < ARRAY_HEIGHT; j++)
				sum += array[i][j];

			snprintf(score, sizeof(score), "%i", sum);
			text = TTF_RenderText_Solid(font, score, black);
			if(text)
			{
				rect.x = 25;
				rect.y = 360;
				rect.w = text->w;
				rect.h = text->h;
				SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 0, 0));
				SDL_BlitSurface(text, NULL, screen, &rect);
				SDL_FreeSurface(text);
				SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);
			}

			turn = (turn + 1) % players;
			drawturn(screen, players, turn);
		}

		SDL_Delay(50);

		if(ggzmode) ggz_network();
	}

	SDL_Quit();

	return 0;
}

