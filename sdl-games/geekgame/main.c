#define HAVE_SOUND /* automate later on */

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#ifdef HAVE_SOUND
#include <SDL/SDL_mixer.h>
#endif

#include <ggz.h>
#include <ggzmod.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

#include "wwwget.h"
#include "proto.h"

#include "config.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define ARRAY_WIDTH 20
#define ARRAY_HEIGHT 10

#define DATA_GLOBAL GGZDATADIR "/geekgame/"
#define DATA_LOCAL "/tmp/"

#define MATCH_SERVER "geekgame"
#define MATCH_VERSION 1

#define MAX_PLAYERS 5

/* Global variables */
static GGZMod *mod = NULL;
static char *playerimage = NULL;
static int modfd;
static int ggzmode = 0;
static SDL_Surface *screen, *image;
static TTF_Font *font = NULL;
static int playmode = -1;
static int players = 0;
static char scores[MAX_PLAYERS];
static char array[ARRAY_WIDTH][ARRAY_HEIGHT];
static int winner = -1;
static int usesound = 1;
#ifdef HAVE_SOUND
Mix_Music *music = NULL;
Mix_Chunk *chunk = NULL;
int chunkchannel = -1;
#endif

/* Prototypes */
int startgame(void);
void loadsettings(void);
void addplayer(const char *picture);
void musicdone(void);

static void game_handle_io(void)
{
	char op;
	int version;
	char greeting[64];
	char player[64], pic[64];
	
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
			break;
		case OP_NEWPLAYER:
			ggz_read_string(modfd, player, sizeof(player));
			ggz_read_string(modfd, pic, sizeof(pic));
			wwwget(pic, DATA_LOCAL "tmp.png");
			addplayer(NULL);
			break;
		case OP_GAMESTART:
			break;
		case OP_GAMESTOP:
			break;
		case OP_GAMEEND:
			break;
		case OP_PRESENTATION:
			break;
		default:
			/* discard */
			break;
	}
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

void drawbox(int x, int y, int w, int h, SDL_Surface *screen, int green, int autocrop)
{
	SDL_Rect rect;

	if(autocrop)
	{
		if(x < 20) x = 20;
		if(x + w > ARRAY_WIDTH * 32 + 20) w = ARRAY_WIDTH * 32 + 20 - x;
		if(y < 20) y = 20;
		if(y + h > ARRAY_HEIGHT * 32 + 20) h = ARRAY_HEIGHT * 32 + 20 - y;
	}
	else
	{
		if((x < 0) || (x + w > SCREEN_WIDTH - 5)) return;
		if((y < 0) || (y + h > SCREEN_HEIGHT - 5)) return;
	}

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

		drawbox(rect.x, rect.y, rect.w, rect.h, screen, (i == turn ? 255 : 120), 0);
	}
}

void drawlevel(int player, int level)
{
	int i;
	for(i = 0; i < 10; i++)
	{
		drawbox(20 + player * 150 + i * 10, 420, 5, 20, screen, (i < level ? 220 : 100), 0);
	}
}

int main(int argc, char *argv[])
{
	struct option op[] =
	{
		{"ggz", 0, 0, 'g'},
		{"help", 0, 0, 'h'},
		{"nosound", 0, 0, 'n'},
		{0, 0, 0, 0}
	};
	int index = 0, c;
	int ret;

	while((c = getopt_long(argc, argv, "ghn", op, &index)) != -1)
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
				printf("[-n | --nosound] Don't use sound\n");
				return 0;
				break;
			case 'n':
				usesound = 0;
				break;
			default:
				fprintf(stderr, "Type '%s --help' to get a list of options.\n", argv[0]);
				return 1;
				break;
		}
	}

	printf("<<<< Geekgame >>>>\n");

	loadsettings();

	if(ggzmode) ggz_init();

	ret = startgame();

	if(ggzmode) ggz_finish();

	return ret;
}

void loadsettings(void)
{
	int conf;
	char conffile[1024];

	snprintf(conffile, sizeof(conffile), "%s/.ggz/personalization", getenv("HOME"));
	conf = ggz_conf_parse(conffile, GGZ_CONF_RDONLY);
	if(conf != -1)
	{
		playerimage = ggz_conf_read_string(conf, "Personalization", "picture", NULL);
	}
	if((conf == -1) || (!playerimage)) playerimage = DATA_GLOBAL "bot.png";
}

void addplayer(const char *picture)
{
	static int counter = 0;
	SDL_Rect rect;

	image = IMG_Load(DATA_LOCAL "tmp.png");

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
		SDL_BlitSurface(text, NULL, screen, &rect);
		SDL_FreeSurface(text);
		SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);
	}
}

void renderscore(int x, int y, int sum)
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

void renderdesc(int x, int y, const char *desc, int active)
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

void playnoise()
{
#ifdef HAVE_SOUND
	if(chunk)
	{
		Mix_Volume(-1, 50);
		chunkchannel = Mix_PlayChannel(-1, chunk, 0);
	}
#endif
}

void screen_intro()
{
	int escape;
	SDL_Event event;
	Uint8 *keystate;
	int x, y, i, oldy;
	int dimmer, dimminc;
	SDL_Rect rect;
	char *desc1, *desc2, *desc3, *desc4;

	x = 20;
	y = 20;
	oldy = y;

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

	rendermode(22, 24, "easy");
	rendermode(22, 54, "matrix");
	rendermode(22, 84, "havoc");
	rendermode(22, 114, "hax0r");

	if(!ggzmode)
	{
		for(i = 0; i < players; i++)
		{
			SDL_Delay(100);
	
			if(!i)
			{
				wwwget(playerimage, DATA_LOCAL "tmp.png");
				image = IMG_Load(DATA_LOCAL "tmp.png");
				if(!image) image = IMG_Load(DATA_GLOBAL "default.png");
			} else image = NULL;

			if(!image) image = IMG_Load(DATA_GLOBAL "bot.png");

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
						if(y <= 15 + 3 * 30)
						{
							drawbox(x, y, 150, 20, screen, 0, 1);
							oldy = y;
							y += 30;
						}
					}
					if(keystate[SDLK_UP])
					{
						if(y >= 15 + 1 * 30)
						{
							drawbox(x, y, 150, 20, screen, 0, 1);
							oldy = y;
							y -= 30;
						}
					}
					if(keystate[SDLK_RETURN])
					{
						escape = 1;
						playmode = (y - 20) / 30 + MODE_RESERVED + 1;
						playnoise();
					}
					break;
			}
		}

		if(oldy)
		{
			desc1 = "The sum of any row or column\nmust be zero.";
			desc2 = "The sum of any row or column,\nmultiplied by 2, must be 42.";
			desc3 = "Numbers encompassed by cursor\nmust divide by 4\nin binary coded format.";
			desc4 = "Both cursor bars\nmust contain the same number\nof 1s";

			if(oldy == 20)
			{
				renderdesc(150, 200, desc1, 0);
			}
			else if(oldy == 50)
			{
				renderdesc(150, 200, desc2, 0);
			}
			else if(oldy == 80)
			{
				renderdesc(150, 200, desc3, 0);
			}
			else if(oldy == 110)
			{
				renderdesc(150, 200, desc4, 0);
			}

			if(y == 20)
			{
				renderdesc(150, 200, desc1, 1);
			}
			else if(y == 50)
			{
				renderdesc(150, 200, desc2, 1);
			}
			else if(y == 80)
			{
				renderdesc(150, 200, desc3, 1);
			}
			else if(y == 110)
			{
				renderdesc(150, 200, desc4, 1);
			}

			oldy = 0;
		}

		drawbox(x, y, 150, 20, screen, dimmer, 1);

		dimmer = dimmer + dimminc * 3;
		if((dimmer <= 150) || (dimmer >= 250)) dimminc = -dimminc;

		SDL_Delay(50);
	}

}

void screen_game()
{
	int dimmer;
	int dimminc;
	Uint8 *keystate;
	int escape;
	SDL_Event event;
	int i, j;
	int x, y;
	int turn;
	int sum, sum2;
	int calc;
	int makescore;

	dimmer = 250;
	dimminc = -1;

	calc = 0;
	winner = -1;
	turn = 0;

	escape = 0;

	x = 50;
	y = 50;
	drawbox(x, y, 32, 32, screen, dimmer, 0);

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
							drawbox(x, y, 32, 32, screen, 0, 1);
							drawbox(x - 96, y, 224, 32, screen, 0, 1);
							drawbox(x, y - 96, 32, 224, screen, 0, 1);
							x += 32;
						}
					}
					if(keystate[SDLK_LEFT])
					{
						if(x >= 1 * 32)
						{
							drawbox(x, y, 32, 32, screen, 0, 1);
							drawbox(x - 96, y, 224, 32, screen, 0, 1);
							drawbox(x, y - 96, 32, 224, screen, 0, 1);
							x -= 32;
						}
					}
					if(keystate[SDLK_DOWN])
					{
						if(y <= (ARRAY_HEIGHT - 1) * 32)
						{
							drawbox(x, y, 32, 32, screen, 0, 1);
							drawbox(x - 96, y, 224, 32, screen, 0, 1);
							drawbox(x, y - 96, 32, 224, screen, 0, 1);
							y += 32;
						}
					}
					if(keystate[SDLK_UP])
					{
						if(y >= 1 * 32)
						{
							drawbox(x, y, 32, 32, screen, 0, 1);
							drawbox(x - 96, y, 224, 32, screen, 0, 1);
							drawbox(x, y - 96, 32, 224, screen, 0, 1);
							y -= 32;
						}
					}
					if(keystate[SDLK_RETURN])
					{
						array[x / 32][y / 32] = !array[x / 32][y / 32];
						drawnumber(screen, x / 32, y / 32, array[x / 32][y / 32]);
						calc = 1;
						playnoise();
					}
					break;
			}
		}

		drawbox(x - 96, y, 224, 32, screen, dimmer / 2, 1);
		drawbox(x, y - 96, 32, 224, screen, dimmer / 2, 1);
		drawbox(x, y, 32, 32, screen, dimmer, 1);

		dimmer = dimmer + dimminc * 3;
		if((dimmer <= 150) || (dimmer >= 250)) dimminc = -dimminc;

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
					for(j = 0; j < ARRAY_HEIGHT; j++)
						sum += array[i][j];
					renderscore(x + 10, 360, sum);
					if(!sum) makescore = 1;

					sum = 0;
					j = y / 32;
					for(i = 0; i < ARRAY_WIDTH; i++)
						sum += array[i][j];
					renderscore(680, y + 10, sum);
					if(!sum) makescore = 1;
					break;
				case MODE_MATRIX:
					sum = 0;
					i = x / 32;
					for(j = 0; j < ARRAY_HEIGHT; j++)
						sum += array[i][j];
					renderscore(x + 10, 360, sum * 2);

					j = y / 32;
					for(i = 0; i < ARRAY_WIDTH; i++)
						sum += array[i][j];
					renderscore(680, y + 10, sum * 2);

					if(sum * 2 == 42) makescore = 1;
					break;
				case MODE_HAVOC:
					sum = 0;
					for(j = y / 32 - 3; j <= y / 32 + 3; j++)
						for(i = x / 32 - 3; i <= x / 32 + 3; i++)
						{
							if((i < 0) || (i >= ARRAY_WIDTH)) continue;
							if((j < 0) || (j >= ARRAY_HEIGHT)) continue;
							if((i == x / 32) || (j == y / 32))
							{
								sum += array[i][j];
							}
						}
					if(sum == (sum & ~0x03)) makescore = 1;
					break;
				case MODE_HAX0R:
					sum = 0;
					sum2 = 0;
					for(j = y / 32 - 3; j <= y / 32 + 3; j++)
					{
						if((j < 0) || (j >= ARRAY_HEIGHT)) continue;
						sum += array[x / 32][j];
					}
					for(i = x / 32 - 3; i <= x / 32 + 3; i++)
					{
						if((i < 0) || (i >= ARRAY_WIDTH)) continue;
						sum2 += array[i][y / 32];
					}
					if(sum == sum2) makescore = 1;
					break;
			}

			if(makescore) scores[turn]++;
			if(scores[turn] == 10)
			{
				escape = 1;
				winner = turn;
			}
			drawlevel(turn, scores[turn]);

			turn = (turn + 1) % players;
			drawturn(screen, players, turn);
		}

		SDL_Delay(50);

		if(ggzmode) ggz_network();
	}
}

void screen_outtro()
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

int startgame(void)
{
	int i, j, x;
	SDL_Rect rect;
	SDL_Surface *icon;
	Uint32 init;
	int ret;

	init = SDL_INIT_VIDEO;
#ifdef HAVE_SOUND
	if(usesound) init |= SDL_INIT_AUDIO;
#endif
	SDL_Init(init);

	TTF_Init();

#ifdef HAVE_SOUND
	if(usesound)
	{
		ret = Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048);
		if(ret < 0)
		{
			fprintf(stderr, "Could not open sound device.\n");
			/*return -1;*/
		}
		else
		{
			music = Mix_LoadMUS(DATA_GLOBAL "music/song32.ogg");
			Mix_PlayMusic(music, -1);
			Mix_HookMusicFinished(musicdone);

			chunk = Mix_LoadWAV(DATA_GLOBAL "sound/phaser.wav");
		}
	}
#endif

	font = TTF_OpenFont(DATA_GLOBAL "1979rg__.ttf", 12);
	/*font = TTF_OpenFont("dark2.ttf", 18);*/
	if(!font)
	{
		fprintf(stderr, "Could not open font file.\n");
		return -1;
	}
	TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

	SDL_WM_SetCaption("GGZ Geek Game", "GGZ Geek Game");
	icon = IMG_Load(DATA_GLOBAL "icon.png");
	if(icon) SDL_WM_SetIcon(icon, 0);

	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 24, 0);

	players = MAX_PLAYERS;
	for(i = 0; i < players; i++)
		scores[i] = 0;

	drawturn(screen, players, -1);

	while(1)
	{
		screen_intro();

		if(playmode < 0)
		{
			SDL_Quit();
			return 0;
		}

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

