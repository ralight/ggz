#include "../config.h"
#include "game.h"
#include "ships.h"

#define MSGC	5

void gameloop ();
void ProcessEvent (SDL_Event event);
int handle_movement (Ship *shipx);
int sound_new (char *filename);
void *sound_function (void *arg);
void *bgm_function (void *arg);
int random_pos (int h);
void process_shooting (int en_num);
void process_movement (int en_num);
void get_highscores ();
void process_highscore ();
void highscore_func ();
void enter_highscore (char *temp);
void collision_control (Ship *Ship1, Ship *Ship2, int twoships);
void collision (Ship *ship1, Ship *ship2, int twoships);
void explode (int x1, int y1, Ship *ship1, int x2, int y2, Ship *ship2, 
		int dep);	
void yes_or_no ();
void yor_ok_func ();
void yor_nok_func ();


SDL_Surface *buffer, *menu, *space;
SDL_Joystick *joy;
#ifndef NOGTK
GtkWidget *gwin;
GtkWidget *name_entry;
#endif
pthread_t sound_thread[5], bgm_thread;
int intro, quit, game_state, num_sounds, bgm;
int intro_quit, no_bgm, enemydef[MAX_EN];
int score, lives, hp, ehp[MAX_EN];
int exp_x1, exp_x2, exp_y1, exp_y2;
int dexp, exp, e, pos, rockets, end;
int enemies[] = {TIE, ADVANCED, BOMBER, INTERCEPT};
char *spaceimg;
char *introbgm[] = {"fanfare.wav", "imperial.wav"};
char *introimg[] = {"title.jpg", "dockingbay1.jpg", "xwingfleet.jpg",
	"emperor3.jpg", "emperor1.jpg", "imvader.jpg", "tie1.jpg",
	"xwingcockpit.jpg", "tie2.jpg", NULL};
char *expimg[] = {"01.gif", "02.gif", "03.gif", "04.gif", "05.gif", "06.gif",
	NULL};
char *r2sounds[] = {"r2danger.wav", "r2happy.wav", "r2hit.wav", "r2warn.wav"};
char *chatmsg[] = {"Die, cowboy.", "Oh no, this is a Jedi.", 
	"My Emperor I failed you !", "You don't know what you're doing...",
	"Kill him !!!!!!"};
Button startb = {NULL, NULL, NULL, 540, 120, 222, 60};
Button readb = {NULL, NULL, NULL, 570, 200, 176, 60}; 
Button optsb = {NULL, NULL, NULL, 573, 280, 170, 60};
Button exitb = {NULL, NULL, NULL, 560, 360, 186, 58};
Ship pshoot, prock, enemy[MAX_EN], enshoot[MAX_EN];
Ship *exp_ship1, *exp_ship2;


void gameloop ()
{
	int i, i2;
	char *image = (char *) malloc (300);
	char *status = (char *) malloc(255);
	
	switch (game_state)
	{
		case INIT:
			sdl_init ();
			
			get_highscores ();
			
			#ifdef USE_JOYSTICK
			if (SDL_NumJoysticks() > 0)
				joy = SDL_JoystickOpen (0);
		
			fprintf (stderr, "Found a joystick: %s\nAxes: %d Buttons: %d Hats: %d Balls: %d\n", SDL_JoystickName (0), SDL_JoystickNumAxes (joy), SDL_JoystickNumButtons (joy), SDL_JoystickNumHats (joy), SDL_JoystickNumBalls (joy));
			#endif
			
			window = sdl_createwin (800, 600, 16, stralloc
					(TITLE, VERSION));
			buffer = sdl_surface (window, 800, 600, 16);
			sdl_blit (buffer, window);
			space = sdl_surface (window, 800, 600, 16);
			sdl_drawimg (stralloc (BGDIR, spaceimg), space, 0, 0);

			menu = sdl_surface (window, 800, 600, 16);
			startb.std = sdl_surface (window, startb.w, startb.h,
					16);
			startb.mov = sdl_surface (window, startb.w, startb.h,
					16);
			readb.std = sdl_surface (window, readb.w, readb.h, 16);
			readb.mov = sdl_surface (window, readb.w, readb.h, 16);
			optsb.std = sdl_surface (window, optsb.w, optsb.h, 16);
			optsb.mov = sdl_surface (window, optsb.w, optsb.h, 16);
			exitb.std = sdl_surface (window, exitb.w, exitb.h, 16);
			exitb.mov = sdl_surface (window, exitb.w, exitb.h, 16);
			
			sdl_drawimg (stralloc (BGDIR, "menu.jpg"), menu, 0, 0);
			sdl_drawimg (stralloc (BTNDIR, "start_std.jpg"), 
					startb.std, 0, 0);
			sdl_drawimg (stralloc (BTNDIR, "start_mov.jpg"),
					startb.mov, 0, 0);
			sdl_drawimg (stralloc (BTNDIR, "read_std.jpg"),
					readb.std, 0, 0);
			sdl_drawimg (stralloc (BTNDIR, "read_mov.jpg"),
					readb.mov, 0, 0);
			sdl_drawimg (stralloc (BTNDIR, "opts_std.jpg"),
					optsb.std, 0, 0);
			sdl_drawimg (stralloc (BTNDIR, "opts_mov.jpg"),
					optsb.mov, 0, 0);
			sdl_drawimg (stralloc (BTNDIR, "exit_std.jpg"),
					exitb.std, 0, 0);
			sdl_drawimg (stralloc (BTNDIR, "exit_mov.jpg"),
					exitb.mov, 0, 0);
			
			startb.bmp = &startb.std;
			readb.bmp = &readb.std;
			optsb.bmp = &optsb.std;
			exitb.bmp = &exitb.std;
		
			#ifndef NOINTRO
			intro = 0;
			intro_quit = FALSE; 
			sound_new (introbgm[rand() % 2]);
			#endif
			game_state = INTRO;
			break;
		case INTRO:
			#ifdef NOINTRO
			game_state = MENU;
			break;
			#else
			if (introimg[intro] == NULL || intro_quit)
			{
				pthread_cancel (sound_thread[0]);
			        game_state = MENU;
			}
			else
			{
				sdl_introimg (stralloc (INTRODIR,
					introimg[intro]), window);
				intro++;
			}
			#endif
			break;
		case MENU:
			#ifndef NOMENU
			sdl_blit (menu, buffer);
			sdl_blitxy (*startb.bmp, buffer, startb.x, startb.y);
			sdl_blitxy (*exitb.bmp, buffer, exitb.x, exitb.y);
			sdl_blitxy (*readb.bmp, buffer, readb.x, readb.y);
			sdl_blitxy (*optsb.bmp, buffer, optsb.x, optsb.y);
			sdl_blit (buffer, window);
			#else
			set_ship (XWING);
			game_state = GAME;
			i = sound_new ("tiescomingin.wav");
			pthread_join (sound_thread[i], NULL);
			pthread_create (&bgm_thread, NULL, bgm_function, NULL);
			#endif
			break;
		case GAME:
			handle_movement (&player);
			pshoot.visible = handle_movement (&pshoot);
			prock.visible = handle_movement (&prock);
			
			sdl_blit (space, buffer);	
			if (player.visible)
				sdl_drawimg (stralloc (SPRITEDIR, player.file),
						buffer, player.x, player.y);
			if (pshoot.visible)
			{
				if (littlestar)
					image = stralloc (SPRITEDIR, def_file
							(DSHOOT));
				else
					image = stralloc (SPRITEDIR, def_file
							(SHOOT));
							
				sdl_drawimg (image, buffer, pshoot.x, 
					pshoot.y);
			}
			if (prock.visible)
				sdl_drawimg (stralloc (SPRITEDIR, def_file
					(ROCKET)), buffer, prock.x, prock.y);
			
			for (i = 0; i < MAX_EN; i++)
			{
				enshoot[i].visible = handle_movement (
						&enshoot[i]);
				process_shooting (i);

				enemy[i].visible = handle_movement (&enemy[i]);
				process_movement (i);
				
				collision_control (&player, &enemy[i],
							TRUE);
				collision_control (&enshoot[i], &player,
							FALSE);
				collision_control (&pshoot, &enemy[i],
							FALSE);
				collision_control (&prock, &enemy[i],
							FALSE);
				
				for (i2 = 0; i2 < MAX_EN; i2++)
					collision_control (&enemy[i],
							&enemy[i2], ENEMY);
				
				if (enemy[i].visible)
					sdl_drawimg (stralloc (SPRITEDIR,
						enemy[i].file), buffer, 
						enemy[i].x, enemy[i].y);
				if (enshoot[i].visible)
					sdl_drawimg (stralloc (SPRITEDIR,
						def_file (ENSHOOT)), buffer,
							enshoot[i].x, 
							enshoot[i].y);
			}

			if (dexp)
			{
				if (littlestar && exp_ship1 == &player)
					image = stralloc (DEXPDIR, expimg[e]);
				else	
					image = stralloc (EXPDIR, expimg[e]);
				
				sdl_drawimg (image, buffer, exp_x1, exp_y1);
				sdl_drawimg (stralloc (EXPDIR, expimg[e]), 
						buffer, exp_x2, exp_y2);
				e++;
				if (expimg[e] == NULL)
				{
					exp_ship1->visible = TRUE;
					exp_ship2->visible = TRUE;
					dexp = FALSE;
				}
			}

			if (exp)
			{
				if (littlestar && exp_ship1 == &player)
					image = stralloc (DEXPDIR, expimg[e]);
				else
					image = stralloc (EXPDIR, expimg[e]);
				
				sdl_drawimg (image, buffer, exp_x1, exp_y1);
				e++;
				if (expimg[e] == NULL)
				{
					exp_ship1->visible = TRUE;
					exp = FALSE;
				}
			}
			
			snprintf (status, 255, 
			"Energy: %d    Rockets: %d    Lives: %d    Score: %d",
				hp, rockets, lives, score);
			sdl_textout (buffer, status, 0, 0);
			
			sdl_blit (buffer, window);
		        if (lives == 0)
				game_state = GAME_OVER;
			break;
		case CREDITS:
			sdl_blit (space, window);
			break;
		case GAME_OVER:
			no_bgm = TRUE;
			pthread_cancel (sound_thread[bgm]);
			SDL_Delay (500);
			sound_new ("laugh.wav");
			#ifndef NOGOSCREEN
			SDL_Delay (1000);
			sound_new ("gameover_bgm.wav");
			sdl_drawimg (stralloc (BGDIR, "gameover.jpg"), buffer,
					0, 0);
			sprintf (status, "Your Score: %d", score);
			sdl_textout (buffer, status, 400, 200);
			sdl_blit (buffer, window);
			SDL_Delay (1000);
			sound_new ("gotupperhand.wav");
			SDL_Delay (7000);
			#endif
			process_highscore ();
			game_state = HIGHSCORE;
			break;
		case HIGHSCORE:
			sdl_drawimg (stralloc (BGDIR, "cockpit.jpg"), 
					buffer, 0, 0);
			for (i = 0; i < 10; i++)
			{
				sprintf (status, 
				"%d.       %s        %d", i + 1, 
						scorelist[i].name, 
						scorelist[i].score);
				sdl_textout (buffer, status, 300, 100 + 50 * i);
			}
			sdl_textout (buffer, "Highscores", 300, 50);
			sdl_blit (buffer, window);
			break;
		case SHUTDOWN:
			quit = TRUE;
			break;
		case PAUSE:
			sdl_textout (window, "!!! Game paused !!!", 350, 300);
			break;
		default:
			fprintf (stderr, "Bad game state !\n");
			exit(23);
			break;
	}
}

void get_highscores ()
{
	FILE *hai;
	int n;
	char *score = (char *) malloc (10);
	
	hai = fopen (HIGHFILE, "r");
	for (n = 1; n < 11; n++)
	{
		fgets (scorelist[n-1].name, 100, hai);
		scorelist[n-1].name[strlen (scorelist[n-1].name)-1] = '\0';
		fgets (score, 10, hai);
		score[strlen (score)-1] = '\0';
		scorelist[n-1].score = atoi (score);
	}
	fclose (hai);
}

void process_highscore ()
{
	int n = 9;
	int temp1 = FALSE;
	char temp2[100];
	#ifndef NOGTK
	GtkWidget *label;
	GtkWidget *box;
	#endif
	
	while (!temp1)
		if (score > scorelist[n].score && n > -1)
			n--;
		else
			temp1 = TRUE;

	pos = n + 1;

	if (n < 9)
	{
		sound_new ("win.wav");
		sprintf (temp2, "!!!! Very Good !!!!\nYou are now no. %d on the highscore-list !\nPlease enter your name:", n + 2);
		
		#ifndef NOGTK
		gwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		label = gtk_label_new (temp2);
		name_entry = gtk_entry_new ();
		box = gtk_vbox_new (FALSE, 0);
		
		gtk_signal_connect (GTK_OBJECT(name_entry), "activate", 
				GTK_SIGNAL_FUNC(highscore_func), NULL);
		gtk_signal_connect (GTK_OBJECT(gwin), "delete_event",
				GTK_SIGNAL_FUNC(gtk_main_quit), NULL);
		
		gtk_widget_set_uposition (gwin, 215, 335);
		
		gtk_box_pack_start (GTK_BOX(box), label, TRUE, FALSE, 0);
		gtk_box_pack_start (GTK_BOX(box), name_entry, TRUE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER(gwin), box);

		gtk_widget_show (label);
		gtk_widget_show (name_entry);
		gtk_widget_show (box);
		gtk_widget_show (gwin);
		
		gtk_main ();
		#else
		enter_highscore ("Nameless One");
		#endif
	}
	else
	{
		sound_new ("lost.wav");
		SDL_Delay (1000);
	}
}

void highscore_func ()
{
	#ifndef NOGTK
	char *temp = (char *) malloc (100);
	
	temp =	gtk_entry_get_text (GTK_ENTRY(name_entry));
	enter_highscore (temp);

	gtk_widget_destroy (gwin);
	gtk_main_quit ();
	#endif
}

void enter_highscore (char *name)
{
	HaiScore save1, save2;
	FILE *hai;
	int n;
	char *temp = (char *) malloc (100);
	
	strncpy (save2.name, name, 100);
	save2.score = score;

	for (n = pos; n < 10; n++)
	{
		save1 = scorelist[n];
		scorelist[n] = save2;
		save2 = save1;
	}

	unlink (HIGHFILE);
	hai = fopen (HIGHFILE, "w");
	
	for (n = 0; n < 10; n++)
	{
		snprintf (temp, 101, "%s\n", scorelist[n].name);
		fputs (temp, hai);
		snprintf (temp, 11, "%d\n", scorelist[n].score);
		fputs (temp, hai);
	}
	
	fclose (hai);
}

void ProcessEvent (SDL_Event event)
{
	SDL_MouseMotionEvent motion = event.motion;
	SDL_MouseButtonEvent button = event.button;	
	int i;
	
	switch (event.type)
	{
		case SDL_QUIT:
			game_state = SHUTDOWN;
			break;
		case SDL_MOUSEMOTION:
			if (game_state != MENU)
				break;
			#ifdef PRINTMOUSE
			fprintf (stderr, "Mouse motion: x = %d | y = %d\n"
					, motion.x, motion.y);
			#endif
			if (motion.x > exitb.x && motion.x < exitb.x + exitb.w
					&& motion.y > exitb.y && motion.y
					< exitb.y + exitb.h)
				exitb.bmp = &exitb.mov;
			else
				exitb.bmp = &exitb.std;
			if (motion.x > startb.x && motion.x < startb.x + 
					startb.w && motion.y > startb.y &&
					motion.y < startb.y + startb.h)
				startb.bmp = &startb.mov;
			else
				startb.bmp = &startb.std;
			if (motion.x > readb.x && motion.x < readb.x + readb.w
					&& motion.y > readb.y && motion.y <
					readb.y + readb.h)
				readb.bmp = &readb.mov;
			else
				readb.bmp = &readb.std;
			if (motion.x > optsb.x && motion.x < optsb.x + optsb.w
					&& motion.y > optsb.y && motion.y <
					optsb.y + optsb.h)
				optsb.bmp = &optsb.mov;
			else
				optsb.bmp = &optsb.std;
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (game_state != MENU)
				break;
			#ifdef PRINTMOUSE
			fprintf (stderr, "Mouse button %d pressed", 
					button.button);
			#endif
			if (button.button == 1 && startb.bmp == &startb.mov)
			{
				sound_new (r2sounds[rand() % 4]);
				if (pdef == 23023)
					set_ship (XWING);
				if (littlestar)
				{
					set_ship (DEATH);
					pshoot.w = def_width (DSHOOT);
					pshoot.h = def_height (DSHOOT);
					pshoot.speed = def_speed (DSHOOT);
				}
				game_state = GAME;
				i = sound_new ("tiescomingin.wav");
				pthread_join (sound_thread[i], NULL);
				pthread_create (&bgm_thread, NULL, bgm_function
						, NULL);
			}
			if (button.button == 1 && exitb.bmp == &exitb.mov)
			{
				sound_new (r2sounds[rand() % 4]);
				i = sound_new ("atze.wav");
				sdl_drawimg (stralloc (BGDIR, "atze.jpg"),
						window, 530, 270);
				pthread_join (sound_thread[i], NULL);
				game_state = SHUTDOWN;
			}
			if (button.button == 1 && readb.bmp == &readb.mov)
			{
				#ifndef NOGTK
				sound_new (r2sounds[rand() % 4]);
				show_text ("history");
				sound_new (r2sounds[rand() % 4]);
				show_text ("readme");
				#endif
			}
			if (button.button == 1 && optsb.bmp == &optsb.mov)
			{
				#ifndef NOGTK
				sound_new (r2sounds[rand() % 4]);
				optionscreen ();
				if (show_credits)
				{
					sdl_drawimg (stralloc (BGDIR, 
						"credits.jpg"), space, 0, 0);
					game_state = CREDITS;
					pthread_create (&bgm_thread, NULL,
							bgm_function, NULL);
				}
				if (outride)
					set_ship (OUTRIDER);
				#endif
			}
			break;
		#ifdef USE_JOYSTICK
		case SDL_JOYAXISMOTION:
			switch (event.jaxis.axis)
			{
				case 0:
				case 2:
				case 4:
					if (event.jaxis.value < 0 && game_state
							== GAME)
						player.movex = -player.speed;
					else
					if (event.jaxis.value > 0 && game_state
							== GAME)
						player.movex = player.speed;
					else
						player.movex = 0;
					break;
				case 1:
				case 3:
				case 5:
					if (event.jaxis.value < 0 && game_state
							== GAME)
						player.movey = -player.speed;
					else
					if (event.jaxis.value > 0 && game_state
							== GAME)
						player.movey = player.speed;
					else
						player.movey = 0;
					break;
				default:
					break;
			}
			break;
		case SDL_JOYBUTTONDOWN:
			switch (event.jbutton.button)
			{
				case 0:
					if (game_state == GAME && 
							!pshoot.visible)
					{
						pshoot.x = player.x + 
							player.w + 2;
						pshoot.y = player.y + 
							player.h / 2;
						pshoot.movey = 0;
						pshoot.movex = pshoot.speed;
						pshoot.visible = TRUE;
						sound_new ("fire.wav");
					}
					if (game_state == INTRO)
						intro_quit = TRUE;
					break;
				case 1:
					if (game_state == GAME && 
						!prock.visible && rockets > 0)
					{
						prock.x = player.x + 
							player.w + 2;
						prock.y = player.y + 
							player.h / 2;
						prock.movey = 0;
						prock.movex = prock.speed;
						prock.visible = TRUE;
						sound_new ("rocket.wav");
						rockets--;
					}
				default:
					break;
			}
			break;
		#endif
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
				case SDLK_ESCAPE:
					game_state = SHUTDOWN;
					break;
				case KEY_PAUSE:
					if (game_state == PAUSE)
						game_state = GAME;
					else
						game_state = PAUSE;
					break;
				case KEY_QUIT:
					game_state = SHUTDOWN;
					break;
				case KEY_UP:
					if (game_state == GAME)
					player.movey = -player.speed;
					break;
				case KEY_DOWN:
					if (game_state == GAME)
					player.movey = player.speed;
					break;
				case KEY_LEFT:
					if (game_state == GAME)
					player.movex = -player.speed;
					break;
				case KEY_RIGHT:
					if (game_state == GAME)
					player.movex = player.speed;
					break;
				case KEY_FIRE:
					if (game_state == GAME && 
							!pshoot.visible)
					{
						pshoot.x = player.x + 
							player.w + 2;
						pshoot.y = player.y + 
							player.h / 2;
						pshoot.movey = 0;
						pshoot.movex = pshoot.speed;
						pshoot.visible = TRUE;
						sound_new ("fire.wav");
					}
					if (game_state == INTRO)
						intro_quit = TRUE;
					break;
				case KEY_ROCKET:
					if (game_state == GAME && 
						!prock.visible && rockets > 0)
	                                {
						prock.x = player.x + 
							player.w + 2;
						prock.y = player.y +
							player.h / 2;
						prock.movey = 0;
						prock.movex = prock.speed;
						prock.visible = TRUE;
						sound_new ("rocket.wav");
						rockets--;
					}
				default:
					break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
				case KEY_UP:
					if (game_state == GAME)
					player.movey = 0;
					break;
				case KEY_DOWN:
					if (game_state == GAME)
					player.movey = 0;
					break;
				case KEY_LEFT:
					if (game_state == GAME)
					player.movex = 0;
					break;
				case KEY_RIGHT:
					if (game_state == GAME)
					player.movex = 0;
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}

int handle_movement (Ship *shipx)
{
	Ship xship = *shipx;
	
	int result = xship.visible;
	
	xship.x = xship.x + xship.movex;
	xship.y = xship.y + xship.movey;

	if (xship.x < 0)
	{
		xship.x = 0;
		result = FALSE;
	}
	if (xship.x > 800 - xship.w)
	{
		xship.x = 800 - xship.w;
		result = FALSE;
	}
	if (xship.y < 0)
	{
		xship.y = 0;
		result = FALSE;
	}
	if (xship.y > 600 - xship.h)
	{	
		xship.y = 600 - xship.h;
		result = FALSE;
	}

	*shipx = xship; 
	return result;
}

void process_shooting (int en_num)
{
	if (!enshoot[en_num].visible && rand() % 2 == 0)
	{
		enshoot[en_num].x = enemy[en_num].x - 2;
		enshoot[en_num].y = enemy[en_num].y + enemy[en_num].h / 2;
		enshoot[en_num].movey = 0;
		enshoot[en_num].movex = - enshoot[en_num].speed;
		enshoot[en_num].visible = TRUE;
		sound_new ("fire.wav");
	}
}	

void process_movement (int en_num)
{
	if (!enemy[en_num].visible)
	{
		enemy[en_num].visible = TRUE;
		enemy[en_num].x = 800 - enemy[en_num].w;
		enemy[en_num].y = random_pos (enemy[en_num].h);
		enemy[en_num].movex = 0;
		enemy[en_num].movey = 0;
		ehp[en_num] = def_hp (enemydef[en_num]);
	}
	else
	{
		enemy[en_num].movex = - enemy[en_num].speed + rand() % 2;
		if (pshoot.visible && pshoot.x > enemy[en_num].x - FOV && 
			pshoot.x < enemy[en_num].x)
		{
			if (pshoot.y > enemy[en_num].y && pshoot.y <= 
				enemy[en_num].y + (enemy[en_num].h / 2) &&
				enemy[en_num].movey == 0)
				enemy[en_num].movey = -enemy[en_num].speed;
			else
			if (pshoot.y > enemy[en_num].y + (enemy[en_num].h / 2)
				&& pshoot.y < enemy[en_num].y + 
				enemy[en_num].h && enemy[en_num].movey == 0)
				enemy[en_num].movey = enemy[en_num].speed;
		}
		else
			enemy[en_num].movey = 0;
	}
}

int sound_new (char *filename)
{	
	int res = num_sounds;
	
	#ifdef VERBOSE
		fprintf (stderr, "sound: New thread: %s\t%d\n", filename, num_sounds);
	#endif
	if (num_sounds < 5)
	{
		pthread_create (&sound_thread[num_sounds], NULL, sound_function, (void *) stralloc (WAVDIR, filename));
		num_sounds++;
	}
	else
		res = 23;
	return res;
}

void *sound_function (void *arg)
{
	#ifdef VERBOSE
	int i = num_sounds;
	#endif
	
	#ifndef NOSOUND
	esd_playwav ((char *)arg);
	#ifdef VERBOSE
		fprintf (stderr, "sound: Thread %d finished !\n", i);
	#endif
	#endif
	num_sounds--;
	pthread_exit (NULL);
}

void *bgm_function (void *arg)
{
	#ifndef NOSOUND
	int i;
	void *result;
	
	while (!quit && !no_bgm)
	{
		i = sound_new ("bgm.wav");
		bgm = i;
		#ifdef VERBOSE
			fprintf (stderr, "bgm: Thread %d is alive !\n", i);
		#endif
		pthread_join (sound_thread[i], &result);
		#ifdef VERBOSE
			fprintf (stderr, "bgm: Thread %d finished !\n", i);
		#endif
	}
	#endif
	pthread_exit (NULL);
}

void set_ship (int type)
{
	pdef = type;
	player.w = def_width (pdef);
	player.h = def_height (pdef);
	player.speed = def_speed (pdef);
	player.file = def_file (pdef);
	hp = def_hp (pdef);
}

int random_pos (int h)
{
	int i;
	int res = 23023;
	
	while (res == 23023)
	{
		res = rand () % 500;
		for (i = 0; i < MAX_EN; i++)
		{
			if (res > enemy[i].y && res < enemy[i].y + enemy[i].h)
				res = 23023;
			if (res + h > enemy[i].y && res < enemy[i].y +
					enemy[i].h)
				res = 23023;
		}
	}
	
	return res;
}

void collision_control (Ship *Ship1, Ship *Ship2, int twoships)
{
	Ship ship1 = *Ship1;
	Ship ship2 = *Ship2;

	if (!ship1.visible || !ship2.visible)
		return;
	
	if (ship1.x > ship2.x && ship1.x < ship2.x + ship2.w && ship1.y >
			ship2.y && ship1.y < ship2.y + ship2.h)
	{
		collision (Ship1, Ship2, twoships);
		return;
	}
	
	if (ship1.x + ship1.w > ship2.x && ship1.x < ship2.x + ship2.w &&
			ship1.y > ship2.y && ship1.y < ship2.y + ship2.h)
	{	
		collision (Ship1, Ship2, twoships);
		return;
	}

	if (ship2.x > ship1.x && ship2.x < ship1.x + ship1.w && ship2.y >
			ship1.y && ship2.y < ship1.y + ship1.h)
	{	
		collision (Ship1, Ship2, twoships);
		return;
	}
	
	if (ship2.x + ship2.w > ship1.x && ship2.x < ship1.x + ship1.w &&
			ship2.y > ship1.y && ship2.y < ship1.y + ship1.h)
	{
		collision (Ship1, Ship2, twoships);
		return;
	}
}

void collision (Ship *ship1, Ship *ship2, int twoships)
{		
	int i, n, n2;
	
	ship1->visible = FALSE;
	
	if (twoships == TRUE)
	{
		for (i = 0; i < MAX_EN; i++)
			if (ship2 == &enemy[i])
				n = i;
		ship2->visible = FALSE;
		score += 500;
		if (!littlestar)
		{
			explode (ship1->x, ship1->y, ship1, ship2->x, 
					ship2->y, ship2, TRUE);
			lives--;
			ship1->x = 0;
			ship1->y = 100;
			ship1->movex = 0;
			ship1->movey = 0;
			hp = def_hp (pdef);
		}
		else
		{
			ship1->visible = TRUE;
			explode (ship2->x, ship2->y, ship2, 0, 0 , NULL, 
					FALSE);
		}

		ship2->x = 800 - ship2->w;
		ship2->y = random_pos (ship2->h);
		ship2->movex = 0;
		ship2->movey = 0;
		ehp[n] = def_hp (enemydef[n]);
	}

	if (twoships == ENEMY)
	{
		for (i = 0; i < MAX_EN; i++)
			if (ship1 == &enemy[i])
				n = i;
		for (i = 0; i < MAX_EN; i++)
			if (ship2 == &enemy[i])
				n2 = i;

		ship2->visible = FALSE;
		explode (ship1->x, ship1->y, ship1, ship2->x, ship2->y, ship2,
				TRUE);
		ship1->x = 800 - ship1->w;
		ship1->y = random_pos (ship1->h);
		ship1->movex = 0;
		ship1->movey = 0;
		ehp[n] = def_hp (enemydef[n]);
		ship2->x = 800 - ship2->w;
		ship2->y = random_pos (ship2->h);
		ship2->movex = 0;
		ship2->movey = 0;
		ehp[n2] = def_hp (enemydef[n2]);
	}	
	
	if (ship1 == &pshoot || ship1 == &prock)
	{
		for (i = 0; i < MAX_EN; i++)
			if (ship2 == &enemy[i])
				n = i;
		#ifdef VERBOSE
		fprintf (stderr, "Enemy: %d\tHP: %d\n", n, ehp[n]);
		#endif
		ehp[n]--;
		if (ehp[n] == 0 || ship1 == &prock)
		{
			ship2->visible = FALSE;
			explode (ship2->x, ship2->y, ship2, 0, 0, NULL,
					FALSE);
			score += 500;
			ship2->x = 800 - ship2->w;
			ship2->y = random_pos (ship2->h);
			ship2->movex = 0;
			ship2->movey = 0;
			ehp[n] = def_hp (enemydef[n]);
		}
	}	

	for (i = 0; i < MAX_EN; i++)
		if (ship1 == &enshoot[i])
		{
			hp--;
			if (hp == 0)
			{
				ship2->visible = FALSE;
				explode (ship2->x, ship2->y, ship2, 0, 0, NULL,
						FALSE);
				lives--;
				ship2->x = 0;
				ship2->y = 100;
				ship2->movex = 0;
				ship2->movey = 0;
				hp = def_hp (pdef);
			}
		}
}

void explode (int x1, int y1, Ship *ship1, int x2, int y2, Ship *ship2, int dep)
{
	sound_new ("explode.wav");
	SDL_Delay (100);

	if (dep)
		dexp = TRUE;
	else
		exp = TRUE;

	exp_x1 = x1;
	exp_y1 = y1;
	exp_ship1 = ship1;
	exp_x2 = x2;
	exp_y2 = y2;
	exp_ship2 = ship2;
	e = 0;
}

void yes_or_no ()
{
	#ifndef NOGTK
	GtkWidget *label;
	GtkWidget *ok;
	GtkWidget *cancel;
	GtkWidget *box1;
	GtkWidget *box2;
	
	gwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	label = gtk_label_new ("Yet\nAnother\nSenseless\nTry ?\n\n");
	ok = gtk_button_new_with_label ("Of Course");
	cancel = gtk_button_new_with_label ("No !!! Please, nooooooooo!");
	box1 = gtk_vbox_new (FALSE, 0);
	box2 = gtk_hbox_new (FALSE, 0);
	
	gtk_widget_set_uposition (gwin, 400, 300);
	gtk_widget_set_usize (gwin, 300, 200);
	
	gtk_signal_connect (GTK_OBJECT(ok), "clicked", GTK_SIGNAL_FUNC
			(yor_ok_func), NULL);
	gtk_signal_connect (GTK_OBJECT(cancel), "clicked", GTK_SIGNAL_FUNC
			(yor_nok_func), NULL);
	gtk_signal_connect (GTK_OBJECT(gwin), "delete_event", GTK_SIGNAL_FUNC
			(gtk_main_quit), NULL);

	gtk_box_pack_start (GTK_BOX(box2), ok, TRUE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(box2), cancel, TRUE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(box1), label, TRUE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(box1), box2, TRUE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER(gwin), box1);

	gtk_widget_show (ok);
	gtk_widget_show (cancel);
	gtk_widget_show (box2);
	gtk_widget_show (label);
	gtk_widget_show (box1);
	gtk_widget_show (gwin);

	gtk_main ();
	#else
	end = TRUE;
	#endif
}

#ifndef NOGTK
void yor_ok_func ()
{
	gtk_widget_destroy (gwin);
	gtk_main_quit ();
}

void yor_nok_func ()
{
	end = TRUE;
	yor_ok_func ();
}
#endif

int main (int argc, char **argv)
{
	int i;
	char z[] = {'\0', '\0'};
	SDL_Event event;
	
	srand (time(0));
	#ifndef NOGTK
	gtk_init (&argc, &argv);
	#endif

	end = FALSE;

	while (!end)
	{
		show_credits = FALSE;
		outride = FALSE;
		invul = FALSE;
		littlestar = FALSE;
	
		quit = FALSE;
		no_bgm = FALSE;
		dexp = FALSE;
		exp = FALSE;
		game_state = INIT;
		num_sounds = 0;
		pdef = 23023;
		score = 0;
		lives = 3;
		rockets = 10;
	
		pshoot.w = def_width (SHOOT);
		pshoot.h = def_height (SHOOT);
		pshoot.speed = def_speed (SHOOT);
		pshoot.visible = FALSE;
		prock.w = def_width (ROCKET);
		prock.h = def_width (ROCKET);
		prock.speed = def_speed (ROCKET);
		prock.visible = FALSE;
		player.x = 0;
		player.y = 100;
		player.movex = 0;
		player.movey = 0;
		player.visible = TRUE;
	
		z[0] = 49 + (rand() % 4);
		spaceimg = stralloc (stralloc ("space", z), ".jpg");
	
		for (i = 0; i < MAX_EN; i++)
		{
			enemydef[i] = enemies[rand () % 4];
			enemy[i].x = 700;
			enemy[i].movex = 0;
			enemy[i].movey = 0;
			ehp[i] = def_hp (enemydef[i]);
			enemy[i].visible = TRUE;
			enemy[i].w = def_width (enemydef[i]);
			enemy[i].h = def_height (enemydef[i]);
			enemy[i].y = random_pos (enemy[i].h);
			enemy[i].speed = def_speed (enemydef[i]);
			enemy[i].file = def_file (enemydef[i]);

			enshoot[i].w = def_width (ENSHOOT);
			enshoot[i].h = def_height (ENSHOOT);
			enshoot[i].speed = def_speed (ENSHOOT);
			enshoot[i].visible = FALSE;
		}

		while (!quit)
		{
			while (SDL_PollEvent(&event))
				ProcessEvent (event);
			gameloop ();
		}
		yes_or_no ();
	}

	SDL_JoystickClose (joy);
	return 0;
}
