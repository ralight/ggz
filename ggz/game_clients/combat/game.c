/*
 * File: game.c
 * Author: Ismael Orenstein
 * Project: GGZ Combat game module
 * Date: 09/17/2000
 * Desc: Game functions
 *
 * Copyright (C) 2000 Ismael Orenstein.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <easysock.h>
#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include "game.h"
#include "combat.h"

#include "callbacks.h"
#include "support.h"

GtkWidget *main_win;
GtkWidget **player_list = NULL;
struct game_info_t cbt_info;
combat_game cbt_game;

// Images
GtkPixmap *tiles[12];
GdkColor *player_colors;
GdkColor lake_color;
GdkColor open_color;
GdkGC *solid_gc;
GdkGC *tile_gc;


// TODO: Load all this information from a config file

#define PIXMAPS 12 // (May have more: Lakes, Unknown units, etc)

// Name of units
static char unitname[12][36] = {"Flag", "Bomb", "Spy", "Scout", "Miner", "Sergeant", "Lieutenant", "Captain", "Major", "Colonel", "General", "Marshall"};
// File names
static char filename[12][36] = {"flag.xpm", "bomb.xpm", "spy.xpm", "scout.xpm", "miner.xpm", "sergeant.xpm", "lieutenant.xpm", "captain.xpm", "major.xpm", "colonel.xpm", "general.xpm", "marshall.xpm"};
// Color names
static char colorname[2][36] = {"RGB:FF/00/00", "RGB:00/00/FF"};
static char lakename[] = {"RGB:00/55/00"};
static char openname[] = {"RGB:DC/DC/A0"};

// Graphics stuff
GdkPixmap* cbt_buf;

void game_handle_io(gpointer data, gint fd, GdkInputCondition cond) {
	int op = -1;

	// Read the fd
	if (es_read_int(fd, &op) < 0) {
		printf("Couldn't read the game fd\n");
		return;
	}

	// FIXME: Erase this
	printf("Got message from the server! (%d)\n", op);

	switch (op) {
		case CBT_MSG_SEAT:
			game_get_seat();
			game_init_board();
			game_draw_board();
			break;
		case CBT_MSG_PLAYERS:
			game_get_players();
			break;
		case CBT_REQ_OPTIONS:
			game_ask_options();
			break;
		case CBT_MSG_OPTIONS:
			game_get_options();
			game_draw_board();
			break;
		case CBT_REQ_SETUP:
			cbt_game.state = CBT_STATE_SETUP;
			callback_sendbutton_set_enabled(TRUE);
			break;
		case CBT_MSG_START:
			game_start();
			break;
		case CBT_MSG_MOVE:
			game_get_move();
			game_draw_board();
			break;
		case CBT_MSG_ATTACK:
			game_get_attack();
			game_draw_board();
			break;
		case CBT_MSG_GAMEOVER:
			game_get_gameover();
			break;
		default:
			game_status("Ops! Wrong message: %d", op);
			break;
	}
}

void game_start() {
	int a;

	// Updates the unit list
	for (a = 0; a < 12; a++)
		cbt_game.army[cbt_info.seat][a] = cbt_game.army[cbt_info.number][a];

	game_update_unit_list(cbt_info.seat);

	cbt_game.state = CBT_STATE_PLAYING;
	callback_sendbutton_set_enabled(FALSE);
	cbt_info.current = -1;
	game_draw_bg();
	game_draw_board();

	game_status("Game has started!");

	// Little hack to display the right turn
	cbt_game.turn--;
	game_change_turn();
}


int game_get_seat() {

	if (es_read_int(cbt_info.fd, &cbt_info.seat) < 0)
		return -1;

	if (es_read_int(cbt_info.fd, &cbt_info.number) < 0)
		return -1;

	if (es_read_int(cbt_info.fd, &cbt_info.version) < 0)
		return -1;

	// TODO: Compare the client and the server version
	game_status("Getting init information\nSeat: %d\tPlayers: %d\tVersion: %d\n", cbt_info.seat, cbt_info.number, cbt_info.version);

	cbt_game.state = CBT_STATE_WAIT;

	// Create the names
	cbt_info.names = (char **)calloc(cbt_info.number, sizeof(char *));

	// Create the seats
	cbt_info.seats = (int *)malloc(cbt_info.number * sizeof(int));

	game_add_player_info(cbt_info.number);

	return 0;
}

int game_ask_options() {
	combat_game _game;
	char *game_str = NULL;
	int a;
	// Default game
	game_status("Using mini game options\n");
	_game.width = 10;
	_game.height = 10;
	_game.map = (tile *)calloc(_game.width * _game.height, sizeof(tile));
	_game.army = (char **)calloc(1, sizeof(char *));
	_game.army[0] = (char *)calloc(12, sizeof(char));
	for (a = 0; a < _game.width*4; a++) {
		_game.map[a].type = OWNER(0) + T_OPEN;
		_game.map[a+60].type = OWNER(1) + T_OPEN;
	}

	for (a = 40; a < _game.width*6; a++) {
		_game.map[a].type = T_OPEN;
	}

	_game.map[CART(3,5,10)].type = T_LAKE;
	_game.map[CART(4,5,10)].type  = T_LAKE;
	_game.map[CART(3,6,10)].type  = T_LAKE;
	_game.map[CART(4,6,10)].type  = T_LAKE;

	_game.map[CART(7,5,10)].type  = T_LAKE;
	_game.map[CART(8,5,10)].type  = T_LAKE;
	_game.map[CART(7,6,10)].type  = T_LAKE;
	_game.map[CART(8,6,10)].type  = T_LAKE;

	// FIXME: Delete this
	printf("Writing army data\n");

	_game.army[0][U_FLAG] = 1;
	_game.army[0][U_BOMB] = 6;
	_game.army[0][U_SPY] = 1;
	_game.army[0][U_SCOUT] = 8;
	_game.army[0][U_MINER] = 5;
	_game.army[0][U_SERGEANT] = 4;
	_game.army[0][U_LIEUTENANT] = 4;
	_game.army[0][U_CAPTAIN] = 4;
	_game.army[0][U_MAJOR] = 3;
	_game.army[0][U_COLONEL] = 2;
	_game.army[0][U_GENERAL] = 1;
	_game.army[0][U_MARSHALL] = 1;
	
	game_status("Sending options string to server");

	game_str = combat_options_string_write(game_str, &_game);

	if (es_write_int(cbt_info.fd, CBT_MSG_OPTIONS) < 0 || es_write_string(cbt_info.fd, game_str) < 0)
		return -1;
	return 0;
}

int game_get_options() {
	char *optstr = NULL;
	int a;

	if (es_read_string_alloc(cbt_info.fd, &optstr) < 0)
		return -1;

	combat_options_string_read(optstr, &cbt_game, cbt_info.number);

	for (a = 0; a < cbt_info.number; a++)
		game_update_unit_list(a);

	// Put all the units in their initial positions
	for (a = 0; a < cbt_game.width*cbt_game.height; a++) {
		if (GET_OWNER(cbt_game.map[a].type) >= 0 && GET_OWNER(cbt_game.map[a].type) != cbt_info.seat)
			// Initial place
			cbt_game.map[a].unit = OWNER(GET_OWNER(cbt_game.map[a].type)) + U_UNKNOWN;
	}

	// TODO: Resize the mainarea widget to fit the map size


	return 0;
}

void game_init() {
	cbt_game.map = NULL;
	cbt_game.width = 10;
	cbt_game.height = 10;
	cbt_game.army = NULL;
	cbt_game.state = CBT_STATE_INIT;
	cbt_game.turn = 0;
	cbt_info.current = U_EMPTY;
}

void game_init_board() {
	int a;
	GdkColormap *sys_colormap;

	printf("Initializing board\n");

	// TODO: Get this information from a tileset file

	// Loads colors
	player_colors = (GdkColor *)calloc(cbt_info.number, sizeof(GdkColor));
	sys_colormap = gdk_colormap_get_system();
	for (a = 0; a < cbt_info.number; a++) {
		gdk_color_parse(colorname[a], &player_colors[a]);
		gdk_colormap_alloc_color(sys_colormap, &player_colors[a], FALSE, TRUE);
	}

	gdk_color_parse(lakename, &lake_color);
	gdk_colormap_alloc_color(sys_colormap, &lake_color, FALSE, TRUE);

	gdk_color_parse(openname, &open_color);
	gdk_colormap_alloc_color(sys_colormap, &open_color, FALSE, TRUE);
	
	// Loads pixmaps
	for (a = 0; a < PIXMAPS; a++) {
		//tiles[a] = gdk_pixmap_create_from_xpm(main_win->window, NULL,
							//&player_colors[cbt_info.seat], filename[a]);
		tiles[a] = (GtkPixmap *)create_pixmap(main_win, filename[a]);
	}
	// Loads GC
	solid_gc = gdk_gc_new(main_win->window);
	gdk_gc_ref(solid_gc);
	tile_gc = gdk_gc_new(main_win->window);
	gdk_gc_ref(tile_gc);
	gdk_gc_set_fill(tile_gc, GDK_TILED);

	printf("Board initialized\n");

}

void game_draw_bg() {
	int i;

	gdk_draw_rectangle( cbt_buf,
			main_win->style->mid_gc[GTK_WIDGET_STATE(main_win)], TRUE, 0, 0, (PIXSIZE+1)*cbt_game.width+1, (PIXSIZE+1)*cbt_game.height+1);

	for (i = 1; i < cbt_game.width; i++) {
		gdk_draw_line(cbt_buf, main_win->style->black_gc,
				i*(PIXSIZE+1), 0, i*(PIXSIZE+1), (PIXSIZE+1)*cbt_game.height+1);
	}
	for (i = 1; i < cbt_game.height; i++) {
		gdk_draw_line(cbt_buf, main_win->style->black_gc,
				0, i*(PIXSIZE+1), (PIXSIZE+1)*cbt_game.width+1, i*(PIXSIZE+1));;
	}
}

void game_draw_terrain(int x, int y, int type) {

	x = x*(PIXSIZE+1)+1;
	y = y*(PIXSIZE+1)+1;

	// TODO: A pixmap would look _MUCH_ better
	switch (type) {
		case (T_OPEN):
			// Draw the open square
			gdk_gc_set_foreground(solid_gc, &open_color);
			break;
		case (T_NULL):
			// Draw a black square
			gdk_gc_set_foreground(solid_gc, &main_win->style->black);
			break;
		case (T_LAKE):
			// Use lake color
			gdk_gc_set_foreground(solid_gc, &lake_color);
			break;
	}

	gdk_draw_rectangle( cbt_buf,
			solid_gc,
			TRUE,
			x, y,
			PIXSIZE, PIXSIZE);
}

void game_draw_unit(int x, int y, int tile, int player) {
		GdkBitmap *mask = NULL;
		GdkPixmap *pix = NULL;

		x = x*(PIXSIZE+1)+1;
		y = y*(PIXSIZE+1)+1;

		// If its no player, doesnt draw
		if (player < 0)
			return;

		// Draws owning player rectangle
		// TODO: A pixmap would look _MUCH_ better
		gdk_gc_set_foreground(solid_gc, &player_colors[player]);
		gdk_draw_rectangle( cbt_buf,
				solid_gc,
				TRUE,
				x, y,
				PIXSIZE, PIXSIZE);

		// Draws the image, if it is known
		if (tile >= 0 && tile < 12) {
			// Gets the image
			gtk_pixmap_get(tiles[tile], &pix, &mask);

			gdk_gc_set_tile(tile_gc, pix);
			gdk_gc_set_ts_origin(tile_gc, x, y);
			gdk_gc_set_clip_origin(tile_gc, x, y);
			gdk_gc_set_clip_mask(tile_gc, mask);
			gdk_draw_rectangle(cbt_buf, tile_gc, TRUE, x, y, PIXSIZE, PIXSIZE);
		}


}
	

void game_draw_board() {
	GtkWidget *tmp;
	int a;

	// Check what we know
	if (!cbt_game.map || !cbt_buf)
		return;

	for (a = 0; a < cbt_game.width * cbt_game.height; a++) {
		// Draw the terrain
		game_draw_terrain(a%cbt_game.width, a/cbt_game.width,
			 LAST(cbt_game.map[a].type));
		// If its setup phase, draw a bg tile in my initial positions
		if (cbt_game.state == CBT_STATE_WAIT || cbt_game.state == CBT_STATE_SETUP) {
				if (GET_OWNER(cbt_game.map[a].type) == cbt_info.seat)
					game_draw_unit(a%cbt_game.width, a/cbt_game.width,
												 U_UNKNOWN, cbt_info.seat);
		}
		// Draw the unit, if it isnt empty
		if (LAST(cbt_game.map[a].unit) != U_EMPTY)
			game_draw_unit(a%cbt_game.width, a/cbt_game.width,
				 LAST(cbt_game.map[a].unit), GET_OWNER(cbt_game.map[a].unit));
	}

	// Update the widget
	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "mainarea");
	gtk_widget_draw(tmp, NULL);

}

void game_add_player_info(int number) {
	int a, b;
	GtkWidget *box;
	GtkWidget **unit_info = NULL;
	char name_str[32];
	char **info;

	info = (char **)calloc(3, sizeof(char *));
	info[1] = (char *)malloc(1 * sizeof(char));
	info[2] = (char *)malloc(5 * sizeof(char));

	// Allocs memory
	player_list = (GtkWidget **)calloc(number, sizeof(GtkWidget *));
	unit_info = (GtkWidget **)calloc(number, sizeof(GtkWidget *));

	// Finds box
	box = gtk_object_get_data(GTK_OBJECT(main_win), "player_box");

	// Creates and puts on the right place
	for (a = 0; a < number; a++) {
		sprintf(name_str, "player_list[%d]", a);
		player_list[a] = (GtkWidget *)gtk_player_info_new(main_win, name_str);
		unit_info[a] = gtk_object_get_data(GTK_OBJECT(player_list[a]), "unit_list");
		gtk_box_pack_start (GTK_BOX(box), player_list[a], TRUE, TRUE, 0);
		gtk_widget_show(player_list[a]);
		gtk_widget_ref(player_list[a]);
	}

	// Now adds the unit names and power
	for (b = 0; b < 12; b++) {
		info[0] = unitname[b];
		strcpy(info[1], "");
		if (b <= U_BOMB)
			strcpy(info[2], "-");
		else
			sprintf(info[2], "%d", b);
		for (a = 0; a < number; a++)
			gtk_clist_insert(GTK_CLIST(unit_info[a]), b, info);
	}
}

void game_update_unit_list(int seat) {
	GtkWidget *unit_list;
	GtkWidget *player_info;
	char widget_name[32];
	char info[6];
	int a;

	sprintf(widget_name, "player_list[%d]", seat);
	player_info = gtk_object_get_data(GTK_OBJECT(main_win), widget_name);
	unit_list = gtk_object_get_data(GTK_OBJECT(player_info), "unit_list");

	for (a = 0; a < 12; a++) {
		sprintf(info, "%d/%d", cbt_game.army[seat][a],
				cbt_game.army[cbt_info.number][a]);
		gtk_clist_set_text(GTK_CLIST(unit_list), a, 1, info);
	}
}


void game_update_player_name(int seat) {
	GtkWidget *player_label;
	GtkWidget *player_info;
	char widget_name[32];

	sprintf(widget_name, "player_list[%d]", seat);
	player_info = gtk_object_get_data(GTK_OBJECT(main_win), widget_name);
	player_label = gtk_object_get_data(GTK_OBJECT(player_info), "player_name");

	if (cbt_info.seats[seat] == GGZ_SEAT_BOT)
		gtk_label_set_text(GTK_LABEL(player_label), "Bot");
	else if (cbt_info.seats[seat] == GGZ_SEAT_OPEN)
		gtk_label_set_text(GTK_LABEL(player_label), "Player Name");
	else
		gtk_label_set_text(GTK_LABEL(player_label), cbt_info.names[seat]);


}

int game_get_players() {
	  int i;
		
		game_status("Getting player list\n");

		for (i = 0; i < cbt_info.number; i++) {

			if (es_read_int(cbt_info.fd, &cbt_info.seats[i]) < 0)
				return -1;

			if (cbt_info.seats[i] != GGZ_SEAT_OPEN) {
				if (es_read_string_alloc(cbt_info.fd, &cbt_info.names[i]) < 0)
					return -1;
				game_status("Player %d named: %s", i, cbt_info.names[i]);
			}

			game_update_player_name(i);

		}

		return 0;

}

void game_status( const char* format, ... ) 
{
	int id;
	va_list ap;
	char* message;
	gpointer tmp;

	va_start( ap, format);
	message = g_strdup_vprintf(format, ap);
	va_end(ap);
	
	//printf(message);
	//printf("\n");
	
	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "statusbar");
	
	id = gtk_statusbar_get_context_id( GTK_STATUSBAR(tmp), "Main" );
	
	gtk_statusbar_pop( GTK_STATUSBAR(tmp), id );
	gtk_statusbar_push( GTK_STATUSBAR(tmp), id, message );

	
	g_free( message );
	
}

void game_change_turn() {
	gpointer tmp;
	int id;
	char msg[32];

	cbt_game.turn = NEXT(cbt_game.turn, cbt_info.number);

	tmp = gtk_object_get_data( GTK_OBJECT(main_win), "current_turn");
	id = gtk_statusbar_get_context_id( GTK_STATUSBAR(tmp), "Main" );
	sprintf(msg, "%s's turn", cbt_info.names[cbt_game.turn]);

	gtk_statusbar_pop( GTK_STATUSBAR(tmp), id );
	gtk_statusbar_push( GTK_STATUSBAR(tmp), id, msg );

}

void game_unit_list_handle (GtkCList *clist, gint row, gint column,
	 													GdkEventButton *event, gpointer user_data) {
	// If its not prep time, there is no need to do that!
	if (cbt_game.state != CBT_STATE_WAIT && cbt_game.state != CBT_STATE_SETUP)
		return;

	// Marks or unmarks the current row
	if (GPOINTER_TO_INT(user_data) == 1)
		cbt_info.current = row;
	else
		cbt_info.current = U_EMPTY;

	// If it hasn't enough units of that kind, then just mark it as empty
	if (cbt_game.army[cbt_info.seat][(int)cbt_info.current] <= 0)
		cbt_info.current = U_EMPTY;
}

void game_handle_move(int p) {
	int last_current, a;
	// Select unit or do move?
	if (cbt_info.current < 0) {
		// TODO: Check if this is valid!
		if (GET_OWNER(cbt_game.map[p].unit) != cbt_info.seat) {
			game_status("This is not yours...");
			return;
		}
		if (LAST(cbt_game.map[p].unit) <= U_BOMB) {
			game_status("This can't be moved!");
			return;
		}
		game_status("Selected %s", unitname[LAST(cbt_game.map[p].unit)]);
		cbt_info.current = p;
	} else {
		// Do move!
		// No more current move
		last_current = cbt_info.current;
		cbt_info.current = -1;
		a = combat_check_move(&cbt_game, last_current, p);
		if (a < 0)
			game_status("This move is invalid!");
		else {
			if (es_write_int(cbt_info.fd, CBT_REQ_MOVE) < 0 ||
					es_write_int(cbt_info.fd, last_current) < 0 ||
					es_write_int(cbt_info.fd, p) < 0) {
				game_status("Can't send message to server!");
				return;
			}
		}
	}
	return;
}

void game_handle_setup(int p) {
	
	// Only to reduce size of lines
	char unit;
	int seat = cbt_info.seat;

	// Check if you own this square
	if (GET_OWNER(cbt_game.map[p].type) != cbt_info.seat) {
		game_status("This is not yours!");
		return;
	}

	// Sanity check for the current value
	if (cbt_info.current < 0 || cbt_info.current > U_MARSHALL)
		cbt_info.current = U_EMPTY;

	// Gets the cliked unit
	unit = LAST(cbt_game.map[p].unit);

	// Checks if it is the same of the current
	if (unit == cbt_info.current) {
		// It is! Just remove it then
		game_setup_remove(p);
	} else if (cbt_info.current == U_EMPTY) {
		// None selected! Just remove it
		game_setup_remove(p);
	} else {
		// Adding from scratch or replacing... this suits both
		game_setup_remove(p);
		game_setup_add(p, cbt_info.current);
	}
	
	// Draws the board
	game_draw_board();
	// Update the player info
	game_update_unit_list(seat);


}

void game_setup_add(int p, int unit) {

	// If it's adding a empty unit, forget about it
	if (unit == U_EMPTY) {
		game_status("Adding a empty unit?");
		return;
	}

	// Sanity check
	if (cbt_game.army[cbt_info.seat][unit] <= 0) {
		game_status("You are out of %s", unitname[unit]);
		return;
	}

	// Updates the value
	cbt_game.map[p].unit = OWNER(cbt_info.seat) + unit;
	// Decreases the current one
	cbt_game.army[cbt_info.seat][unit]--;

}

void game_setup_remove(int p) {
	int unit = LAST(cbt_game.map[p].unit);

	if (unit == U_EMPTY) {
		return;
	}

	// Increases it 
	cbt_game.army[cbt_info.seat][unit]++;
	// Sanity check
	if (cbt_game.army[cbt_info.seat][unit] > cbt_game.army[cbt_info.number][unit]) {
		game_status("CHEATER! CHEATER! CHEATER!!");
		cbt_game.army[cbt_info.seat][unit]--;
		return;
	}

	// Erases it
	cbt_game.map[p].unit = U_EMPTY;

}


void game_send_setup() {
	char *setup;
	int len = 0;
	int a, b = 0;

	if (cbt_game.state != CBT_STATE_SETUP)
		return;

	// TODO: Check for validity
	
	printf("Sending setup...");

	// Gets the number of setup tiles
	for (a = 0; a < cbt_game.width*cbt_game.height; a++) {
		if (GET_OWNER(cbt_game.map[a].type) == cbt_info.seat)
			len++;
	}

	// Alloc memory
	setup = (char *)malloc((len+1) * sizeof(char));

	// Puts setup together
	// Adds one to everyone, so it can be sent without problems
	// (although the OWNER part of the message should handle it)
	for (a = 0; a < cbt_game.width*cbt_game.height; a++) {
		if (GET_OWNER(cbt_game.map[a].type) == cbt_info.seat) {
			setup[b] = cbt_game.map[a].unit + 1;
			b++;
		}
	}
	setup[b] = 0;

	// Sends the message
	if (es_write_int(cbt_info.fd, CBT_MSG_SETUP) < 0 || es_write_string(cbt_info.fd, setup) < 0)
		game_status("Couldn't send the setup!");

	// Free memory
	free(setup);

	// Hide the button
	callback_sendbutton_set_enabled(FALSE);
	// Update the state (NULL state)
	cbt_game.state = CBT_STATE_NULL;

	printf("done!\n");

}

void game_get_move() {
	int from, to;

	if (es_read_int(cbt_info.fd, &from) < 0 ||
			es_read_int(cbt_info.fd, &to) < 0) {
		game_status("Can't get move!");
		return;
	}

	if (from < 0 || to < 0) {
		game_status("Error! %d", from);
		return;
	}

	// do move!
	cbt_game.map[to].unit = cbt_game.map[from].unit;
	cbt_game.map[from].unit = U_EMPTY;

	// Change turn
	game_change_turn();
}

void game_get_attack() {
	int from, to, f_u, t_u, seat2;

	if (es_read_int(cbt_info.fd, &from) < 0 ||
			es_read_int(cbt_info.fd, &f_u) < 0 ||
			es_read_int(cbt_info.fd, &to) < 0 ||
			es_read_int(cbt_info.fd, &t_u) < 0) {
		game_status("Can't get attack data!");
		return;
	}

	seat2 = GET_OWNER(cbt_game.map[to].unit);

	// Do attack
	if (f_u < 0 && t_u >= 0) {
		// The from unit won!
		cbt_game.map[from].unit = U_EMPTY;
		cbt_game.map[to].unit = OWNER(cbt_game.turn) - f_u;
		cbt_game.army[seat2][t_u]--;
		game_update_unit_list(seat2);
	} else if (f_u >= 0 && t_u < 0) {
		// The to unit won!
		cbt_game.map[from].unit = U_EMPTY;
		cbt_game.map[to].unit = OWNER(seat2) - t_u;
		cbt_game.army[cbt_game.turn][f_u]--;
		game_update_unit_list(cbt_game.turn);
	} else if (f_u < 0 && t_u < 0) {
		// Both won (or lost, whatever!)
		cbt_game.map[from].unit = U_EMPTY;
		cbt_game.map[to].unit = U_EMPTY;
		cbt_game.army[cbt_game.turn][-f_u]--;
		cbt_game.army[seat2][-t_u]--;
		game_update_unit_list(cbt_game.turn);
		game_update_unit_list(seat2);
	} else {
		game_status("Problems in the attack logic!");
	}

	// Change turn
	game_change_turn();

}

void game_get_gameover() {
	int winner;

	if (es_read_int(cbt_info.fd, &winner) < 0) {
		game_status("Can't get gameover!");
		return;
	}

	game_status("Game is over! Winner: %s", cbt_info.names[winner]);
	cbt_game.state = CBT_STATE_DONE;
}
