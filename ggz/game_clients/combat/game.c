/*
 * File: game.c
 * Author: Ismael Orenstein
 * Project: GGZ Combat game module
 * Date: 09/17/2000
 * Desc: Game functions
 * $Id: game.c 6225 2004-10-28 05:48:01Z jdorje $
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include <ggz.h>
#include <ggz_common.h>
#include <ggzmod.h>

#include "menus.h"

#include "combat.h"
#include "game.h"
#include "map.h"
#include "dlg_options.h"

#include "callbacks.h"
#include "support.h"

#define GAME_WEBPAGE "http://www.ggzgamingzone.org/games/combat/"

GtkWidget *main_win;
GtkWidget **player_list = NULL;
struct game_info_t cbt_info;
combat_game cbt_game;

// Images
GtkPixmap *tiles[12];
GdkColor *player_colors;
GdkColor last_color;
GdkColor current_color;
GdkColor lake_color;
GdkColor open_color;
GdkGC *solid_gc;
GdkGC *tile_gc;


// TODO: Load all this information from a config file

#define LAST_COLORNAME "RGB:55/77/55"
#define CURRENT_COLORNAME "RGB:00/00/00"

#define PIXMAPS 12 // (May have more: Lakes, Unknown units, etc)

// Name of units
char unitname[13][36] = {"Flag", "Bomb", "Spy", "Scout", "Miner", "Sergeant", "Lieutenant", "Captain", "Major", "Colonel", "General", "Marshall", "Unknown"};
// Number of units
int unitdefault[12] = {1, 6, 1, 8, 5, 4, 4, 4, 3, 2, 1, 1};
// File names
char filename[12][36] = {"flag.xpm", "bomb.xpm", "spy.xpm", "scout.xpm", "miner.xpm", "sergeant.xpm", "lieutenant.xpm", "captain.xpm", "major.xpm", "colonel.xpm", "general.xpm", "marshall.xpm"};
// Color names
char colorname[2][36] = {"red", "blue"};
char lakename[] = "dark green";
char openname[] = "beige";

// Graphics stuff
GdkPixmap* cbt_buf;

void game_handle_io(gpointer data, gint fd, GdkInputCondition cond) {
  int op = -1;

  // Read the fd
  if (ggz_read_int(fd, &op) < 0) {
    ggz_error_msg("Couldn't read the game fd");
    return;
  }

  ggz_debug("main", "Got message from the server! (%d)", op);

  switch (op) {
    case CBT_MSG_SEAT:
      game_get_seat();
      game_init_board();
      game_draw_board();
      set_menu_sensitive(TABLE_SYNC, FALSE);
      set_menu_sensitive(_("<main>/Game/Save current map"), FALSE);
      set_menu_sensitive(_("<main>/Game/Show game options"), FALSE);
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
      set_menu_sensitive("<main>/Game/Save current map", TRUE);
      set_menu_sensitive("<main>/Game/Show game options", TRUE);
      break;
    case CBT_REQ_SETUP:
      cbt_game.state = CBT_STATE_SETUP;
      callback_widget_set_enabled("send_setup", TRUE);
      break;
    case CBT_MSG_START:
      game_start();
      callback_widget_set_enabled("send_setup", FALSE);
      set_menu_sensitive(TABLE_SYNC, TRUE);
      break;
    case CBT_MSG_MOVE:
      game_get_move();
      game_draw_board();
      break;
    case CBT_MSG_ATTACK:
      game_get_attack();
      game_draw_board();
      break;
    case CBT_MSG_SYNC:
      game_get_sync();
      game_draw_board();
      // Little ugly hack to display the righ turn
      cbt_game.turn--;
      game_change_turn();
      break;
    case CBT_MSG_GAMEOVER:
      game_get_gameover();
      break;
    default:
      game_status("Ops! Wrong message: %d", op);
      break;
  }
}

void game_start(void) {
  int a;

  // Updates the unit list
  for (a = 0; a < 12; a++)
    cbt_game.army[cbt_info.seat][a] = cbt_game.army[cbt_game.number][a];

  game_update_unit_list(cbt_info.seat);

  cbt_game.state = CBT_STATE_PLAYING;
  cbt_info.current = -1;
  game_draw_board();

  game_status("Game has started!");

  // Little hack to display the right turn
  cbt_game.turn--;
  game_change_turn();
}


int game_get_seat(void) {

  if (ggz_read_int(cbt_info.fd, &cbt_info.seat) < 0)
    return -1;

  if (ggz_read_int(cbt_info.fd, &cbt_game.number) < 0)
    return -1;

  if (ggz_read_int(cbt_info.fd, &cbt_info.version) < 0)
    return -1;

  // TODO: Compare the client and the server version
  if (cbt_info.version != PROTOCOL_VERSION)
    game_message("Your client is using Combat protocol %d, while the server uses protocol %d. You may run into problems.\nYou should get a newer version on %s", PROTOCOL_VERSION, cbt_info.version, GAME_WEBPAGE);

  game_status("Getting init information\nSeat: %d\tPlayers: %d\tVersion: %d\n", cbt_info.seat, cbt_game.number, cbt_info.version);

  cbt_game.state = CBT_STATE_WAIT;

  // Create the names
  cbt_info.names = (char **)calloc(cbt_game.number, sizeof(char *));

  // Create the seats
  cbt_info.seats = (int *)ggz_malloc(cbt_game.number * sizeof(int));

  return 0;
}

void game_ask_options(void) {
  GtkWidget *options_dialog;
  GtkWidget *ok_button;
  options_dialog = create_dlg_options(cbt_game.number); 
  ok_button = lookup_widget(options_dialog, "ok_button"); 
  gtk_signal_connect_object (GTK_OBJECT (ok_button), "clicked",
                             GTK_SIGNAL_FUNC (game_send_options), 
                             GTK_OBJECT (options_dialog));
  gtk_widget_show_all(options_dialog);
}

int game_get_options(void) {
  char *optstr = NULL;
  int a;
  int old_width = cbt_game.width;
  int old_height = cbt_game.height;
  GtkWidget *checkmenuitem = get_menu_item("<main>/Game/Remember enemy units");
  char *title;
  GtkWidget *widget = lookup_widget(main_win, "mainarea");

  if (ggz_read_string_alloc(cbt_info.fd, &optstr) < 0)
    return -1;

  a = combat_options_string_read(optstr, &cbt_game);
  /* Free the String (thanks Jason!) */
  ggz_free(optstr);
  if (a > 0)
    game_message("Please note: \nThis client couldn't recognize %d options sent by the server.\nThe game may have unexpected behavior.\nYou should update your client at %s", a, GAME_WEBPAGE);

  if (old_width != cbt_game.width || old_height != cbt_game.height) {
    gdk_pixmap_unref(cbt_buf);
    cbt_buf = NULL;
    gtk_widget_set_usize(widget, cbt_game.width * (PIXSIZE+1)+1,
                                 cbt_game.height * (PIXSIZE+1)+1);
  }

  for (a = 0; a < cbt_game.number; a++)
    game_update_unit_list(a);

  // Put all the units in their initial positions
  for (a = 0; a < cbt_game.width*cbt_game.height; a++) {
    if (GET_OWNER(cbt_game.map[a].type) >= 0 && GET_OWNER(cbt_game.map[a].type) != cbt_info.seat)
      // Initial place
      cbt_game.map[a].unit = OWNER(GET_OWNER(cbt_game.map[a].type)) + U_UNKNOWN;
  }

  if (cbt_game.name) {
    title = ggz_malloc(strlen("Combat - ") + strlen(cbt_game.name) + 1);
    sprintf(title, "Combat - %s", cbt_game.name);
    gtk_window_set_title(GTK_WINDOW(main_win), title);
    ggz_free(title);
  }

  if (cbt_game.options & OPT_OPEN_MAP || cbt_game.options & OPT_SHOW_ENEMY_UNITS) {
    cbt_info.show_enemy = TRUE;
    gtk_object_set_data(GTK_OBJECT(checkmenuitem), "dirty", GINT_TO_POINTER(TRUE));
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(checkmenuitem), TRUE);
  }

  // Show we show the game optiosn?
  if (cbt_game.options)
    on_show_game_options_activate();

  return 0;
}

void game_init(void) {
  cbt_game.map = NULL;
  cbt_game.width = 10;
  cbt_game.height = 10;
  cbt_game.army = NULL;
  cbt_game.name = NULL;
  cbt_game.state = CBT_STATE_INIT;
  cbt_game.turn = 0;
  cbt_game.options = 0;
  cbt_info.current = U_EMPTY;
  cbt_info.show_enemy = FALSE;
  cbt_info.last_unit = U_UNKNOWN;
  cbt_info.last_from = -1;
  cbt_info.last_to = -1;
}

void game_init_board(void) {
  int a;
  GdkColormap *sys_colormap;

  // TODO: Get this information from a tileset file

  // Loads colors
  player_colors = (GdkColor *)calloc(cbt_game.number, sizeof(GdkColor));
  sys_colormap = gdk_colormap_get_system();
  for (a = 0; a < cbt_game.number; a++) {
    gdk_color_parse(colorname[a], &player_colors[a]);
    gdk_colormap_alloc_color(sys_colormap, &player_colors[a], FALSE, TRUE);
  }

  gdk_color_parse(LAST_COLORNAME, &last_color);
  gdk_colormap_alloc_color(sys_colormap, &last_color, FALSE, TRUE);

  gdk_color_parse(CURRENT_COLORNAME, &current_color);
  gdk_colormap_alloc_color(sys_colormap, &current_color, FALSE, TRUE);

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

	/* Add the player info */
  game_add_player_info(cbt_game.number);


}

void game_draw_bg(void) {
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

    // Draws owning player rectangle
    // TODO: A pixmap would look _MUCH_ better
    if (player >= 0) {
      gdk_gc_set_foreground(solid_gc, &player_colors[player]);
      gdk_draw_rectangle( cbt_buf,
          solid_gc,
          TRUE,
          x, y,
          PIXSIZE, PIXSIZE);
    }

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
  

void game_draw_board(void) {
  GtkWidget *tmp;
  int a;

  // Check what we know
  if (!cbt_game.map || !cbt_buf)
    return;

  // Draw bg
  game_draw_bg();

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

  game_draw_extra();

  // Update the widget
  tmp = gtk_object_get_data(GTK_OBJECT(main_win), "mainarea");
  gtk_widget_draw(tmp, NULL);

}

void game_draw_extra(void) {

  // Draw lasts
  if (cbt_info.last_unit >= 0)
    game_draw_unit(cbt_info.last_to%cbt_game.width, 
        cbt_info.last_to/cbt_game.width,
        cbt_info.last_unit, -1);

  // Draw borders
  gdk_gc_set_foreground(solid_gc, &last_color);
  if (cbt_info.last_from >= 0) {
    gdk_draw_rectangle( cbt_buf,
        solid_gc,
        FALSE,
        cbt_info.last_from%cbt_game.width*(PIXSIZE+1),
        cbt_info.last_from/cbt_game.width*(PIXSIZE+1),
        PIXSIZE+1, PIXSIZE+1);
    gdk_draw_rectangle( cbt_buf,
        solid_gc,
        FALSE,
        cbt_info.last_from%cbt_game.width*(PIXSIZE+1)+1,
        cbt_info.last_from/cbt_game.width*(PIXSIZE+1)+1,
        PIXSIZE-1, PIXSIZE-1);
  }
  if (cbt_info.last_to >= 0) {
    gdk_draw_rectangle( cbt_buf,
        solid_gc,
        FALSE,
        cbt_info.last_to%cbt_game.width*(PIXSIZE+1),
        cbt_info.last_to/cbt_game.width*(PIXSIZE+1),
        PIXSIZE+1, PIXSIZE+1);
    gdk_draw_rectangle( cbt_buf,
        solid_gc,
        FALSE,
        cbt_info.last_to%cbt_game.width*(PIXSIZE+1)+1,
        cbt_info.last_to/cbt_game.width*(PIXSIZE+1)+1,
        PIXSIZE-1, PIXSIZE-1);
  }
  if (cbt_info.current >= 0 && cbt_game.state == CBT_STATE_PLAYING) {
    gdk_gc_set_foreground(solid_gc, &current_color);
    gdk_draw_rectangle( cbt_buf,
        solid_gc,
        FALSE,
        cbt_info.current%cbt_game.width*(PIXSIZE+1),
        cbt_info.current/cbt_game.width*(PIXSIZE+1),
        PIXSIZE+1, PIXSIZE+1);
    gdk_draw_rectangle( cbt_buf,
        solid_gc,
        FALSE,
        cbt_info.current%cbt_game.width*(PIXSIZE+1)+1,
        cbt_info.current/cbt_game.width*(PIXSIZE+1)+1,
        PIXSIZE-1, PIXSIZE-1);
  }
    

}
  

void game_add_player_info(int number) {
  int a, b;
  GtkWidget *box;
  GtkWidget **unit_info = NULL;
  char name_str[32];
  char **info;

  info = (char **)calloc(3, sizeof(char *));
  info[1] = (char *)ggz_malloc(1 * sizeof(char));
  info[2] = (char *)ggz_malloc(5 * sizeof(char));

  // Allocs memory
  player_list = (GtkWidget **)calloc(number, sizeof(GtkWidget *));
  unit_info = (GtkWidget **)calloc(number, sizeof(GtkWidget *));

  // Finds box
  box = gtk_object_get_data(GTK_OBJECT(main_win), "player_box");

  // Creates and puts on the right place
  for (a = 0; a < number; a++) {
    sprintf(name_str, "player_list[%d]", a);
    player_list[a] = (GtkWidget *)gtk_player_info_new(main_win, name_str, a);
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
      sprintf(info[2], "%d", b-1);
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
    if (seat == cbt_info.seat || !(cbt_game.options & OPT_HIDE_UNIT_LIST)) {
      sprintf(info, "%d/%d", cbt_game.army[seat][a],
          cbt_game.army[cbt_game.number][a]);
    } else {
      sprintf(info, "?/%d", cbt_game.army[cbt_game.number][a]);
    }
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

int game_get_players(void) {
    int i;
    
    game_status("Getting player list\n");

    for (i = 0; i < cbt_game.number; i++) {

      if (ggz_read_int(cbt_info.fd, &cbt_info.seats[i]) < 0)
        return -1;

      if (cbt_info.seats[i] != GGZ_SEAT_OPEN) {
        /* Get new name */
        if (cbt_info.names[i]) {
          ggz_free(cbt_info.names[i]);
          cbt_info.names[i] = NULL;
        }
        if (ggz_read_string_alloc(cbt_info.fd, &cbt_info.names[i]) < 0)
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
  
  /* ggz_debug("main", "%s", message); */
  
  tmp = gtk_object_get_data(GTK_OBJECT(main_win), "statusbar");
  
  id = gtk_statusbar_get_context_id( GTK_STATUSBAR(tmp), "Main" );
  
  gtk_statusbar_pop( GTK_STATUSBAR(tmp), id );
  gtk_statusbar_push( GTK_STATUSBAR(tmp), id, message );

  
  g_free( message );
  
}

void game_change_turn(void) {
  gpointer tmp;
  int id;
  char msg[32];

  cbt_game.turn = NEXT(cbt_game.turn, cbt_game.number);

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

	// Check if at least we have a map
	if (!cbt_game.map)
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
    if ((LAST(cbt_game.map[p].unit) == U_BOMB && !(cbt_game.options & OPT_MOVING_BOMB)) || (LAST(cbt_game.map[p].unit) == U_FLAG && !(cbt_game.options & OPT_MOVING_FLAG))) {
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
    if (a < 0) {
      game_status("This move is invalid!");
      ggz_error_msg("Move error: %d", a);
    }
    else {
      if (ggz_write_int(cbt_info.fd, CBT_REQ_MOVE) < 0 ||
          ggz_write_int(cbt_info.fd, last_current) < 0 ||
          ggz_write_int(cbt_info.fd, p) < 0) {
        game_status("Can't send message to server!");
        return;
      }
    }
  }
  game_draw_board();
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
  if (cbt_game.army[cbt_info.seat][unit] > cbt_game.army[cbt_game.number][unit]) {
    game_status("CHEATER! CHEATER! CHEATER!!");
    cbt_game.army[cbt_info.seat][unit]--;
    return;
  }

  // Erases it
  cbt_game.map[p].unit = U_EMPTY;

}


void game_send_setup(void) {
  char *setup;
  int len = 0;
  int a, b = 0;

  if (cbt_game.state != CBT_STATE_SETUP)
    return;

  // TODO: Check for validity
  
  // Gets the number of setup tiles
  for (a = 0; a < cbt_game.width*cbt_game.height; a++) {
    if (GET_OWNER(cbt_game.map[a].type) == cbt_info.seat)
      len++;
  }

  // Alloc memory
  setup = (char *)ggz_malloc((len+1) * sizeof(char));

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
  if (ggz_write_int(cbt_info.fd, CBT_MSG_SETUP) < 0 || ggz_write_string(cbt_info.fd, setup) < 0)
    game_status("Couldn't send the setup!");

  // Free memory
  ggz_free(setup);

  // Hide the button
  callback_widget_set_enabled("send_setup", FALSE);
  // Update the state (NULL state)
  cbt_game.state = CBT_STATE_NULL;

}

void game_get_move(void) {
  int from, to;

  if (ggz_read_int(cbt_info.fd, &from) < 0 ||
      ggz_read_int(cbt_info.fd, &to) < 0) {
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

  // Update lasts
  cbt_info.last_unit = LAST(cbt_game.map[to].unit);
  cbt_info.last_from = from;
  cbt_info.last_to = to;

  // Change turn
  game_change_turn();
}

void game_get_attack(void) {
  int from, to, f_u, t_u, seat2;

  if (ggz_read_int(cbt_info.fd, &from) < 0 ||
      ggz_read_int(cbt_info.fd, &f_u) < 0 ||
      ggz_read_int(cbt_info.fd, &to) < 0 ||
      ggz_read_int(cbt_info.fd, &t_u) < 0) {
    game_status("Can't get attack data!");
    return;
  }

  seat2 = GET_OWNER(cbt_game.map[to].unit);

  // Do attack
  if (f_u < 0 && t_u >= 0) {
    // The from unit won!
    cbt_game.map[from].unit = U_EMPTY;
    if (cbt_info.show_enemy || cbt_game.turn == cbt_info.seat) {
      // It`s my turn or I'm cheating
      cbt_game.map[to].unit = OWNER(cbt_game.turn) - f_u;
    } else {
      // I'm not cheating
      cbt_game.map[to].unit = OWNER(cbt_game.turn) + U_UNKNOWN;
    }
    cbt_game.army[seat2][t_u]--;
    game_update_unit_list(seat2);
    game_status("A %s attacked a %s and won", unitname[-f_u], unitname[t_u]);
    // Update lasts
    cbt_info.last_unit = -f_u;
    cbt_info.last_from = from;
    cbt_info.last_to = to;
  } else if (f_u >= 0 && t_u < 0) {
    // The to unit won!
    cbt_game.map[from].unit = U_EMPTY;
    if (cbt_info.show_enemy || seat2 == cbt_info.seat) {
      // My turn or cheating
      cbt_game.map[to].unit = OWNER(seat2) - t_u;
    } else {
      cbt_game.map[to].unit = OWNER(seat2) + U_UNKNOWN;
    }
    cbt_game.army[cbt_game.turn][f_u]--;
    game_update_unit_list(cbt_game.turn);
    game_status("A %s tried to attack a %s and lost", unitname[f_u], unitname[-t_u]);
    // Update lasts
    cbt_info.last_unit = -t_u;
    cbt_info.last_from = from;
    cbt_info.last_to = to;
  } else if (f_u < 0 && t_u < 0) {
    // Both won (or lost, whatever!)
    cbt_game.map[from].unit = U_EMPTY;
    cbt_game.map[to].unit = U_EMPTY;
    cbt_game.army[cbt_game.turn][-f_u]--;
    cbt_game.army[seat2][-t_u]--;
    game_update_unit_list(cbt_game.turn);
    game_update_unit_list(seat2);
    if (f_u == t_u)
      game_status("Two %s died", unitname[-f_u]);
    else
      game_status("A %s tried to attack a %s. Both died", unitname[-f_u], unitname[-t_u]);
    // Update lasts
    cbt_info.last_unit = -1;
    cbt_info.last_from = from;
    cbt_info.last_to = to;
  } else {
    game_status("Problems in the attack logic!");
  }

  // Change turn
  game_change_turn();

}

void game_get_gameover(void) {
  int winner;

  if (ggz_read_int(cbt_info.fd, &winner) < 0) {
    game_status("Can't get gameover!");
    return;
  }

  game_status("Game is over! Winner: %s", cbt_info.names[winner]);
  cbt_game.state = CBT_STATE_DONE;

  // Now searches for the game and saves it
  if (map_search(&cbt_game) == 0)
    game_ask_save_map();
}

void game_ask_save_map(void) {
  GtkWidget *save_dlg = create_dlg_save();
  GtkWidget *yes = lookup_widget(save_dlg, "yes");
  GtkWidget *map_name = lookup_widget(save_dlg, "map_name");
  gtk_signal_connect(GTK_OBJECT(yes), "clicked",
                     GTK_SIGNAL_FUNC (game_confirm_save_map), save_dlg); 
  if (cbt_game.name)
    gtk_entry_set_text(GTK_ENTRY(map_name), cbt_game.name);
  gtk_widget_show_all(save_dlg);
}

void game_confirm_save_map(GtkButton *button, gpointer user_data) {
  GtkWidget *map_name = lookup_widget(user_data, "map_name");
  const char *name = gtk_entry_get_text(GTK_ENTRY(map_name));
  cbt_game.name = ggz_strdup(name);
  map_save(&cbt_game);
}


/* Function to open a dialog box displaying the message provided. */

void game_message( const char *format, ... ) {
  va_list ap;
  char* message;

  GtkWidget *dialog, *label, *ok_button;

  // Create the messages
  va_start( ap, format);
  message = g_strdup_vprintf(format, ap);
  va_end(ap);
              
  /* Create the widgets */
  
  dialog = gtk_dialog_new();
  label = gtk_label_new (message);
  ok_button = gtk_button_new_with_label("OK");

  gtk_label_set_line_wrap( GTK_LABEL(label), TRUE );
       
  /* Ensure that the dialog box is destroyed when
   * the user clicks ok. */
 
  gtk_signal_connect_object (GTK_OBJECT (ok_button), "clicked",
                             GTK_SIGNAL_FUNC (gtk_widget_destroy), GTK_OBJECT(dialog));

  gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->action_area),
                     ok_button);

  /* Add the label, and show everything
   * we've added to the dialog. */

	 gtk_container_set_border_width(GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), 5);
   gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox),
                      label);
   gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
   gtk_widget_show_all (dialog);

}

void game_get_sync(void) {
  char *syncstr;
  int a, len;

  if (ggz_read_string_alloc(cbt_info.fd, &syncstr) < 0)
    return;

  len = strlen(syncstr);

  for ( a = 0; a < len; a++)
    syncstr[a]--;

  // Gets current turn
  cbt_game.turn = syncstr[0];

  // Gets current state
  cbt_game.state = syncstr[1];

  // Gets map data
  for (a = 0; a < cbt_game.width*cbt_game.height; a++) {
    cbt_game.map[a].unit = syncstr[a+2];
  }

  // TODO: Recalculate the number of units

  if (syncstr[len] != 0)
    game_status("Error: Wrong sync string");

  ggz_free(syncstr);

  return;

}
  
void game_resync(void) {
  if (ggz_write_int(cbt_info.fd, CBT_REQ_SYNC) < 0)
    return;

  return;
}

int game_send_options(GtkWidget *options_dialog) {
  combat_game *_game;
  char *game_str = NULL;
  int a = 0;

  _game = gtk_object_get_data(GTK_OBJECT(options_dialog), "options");

  if (!_game) {
    game_message("No options?");
    return -1;
  }

  game_str = combat_options_string_write(_game, 0);

  if ((a = combat_options_check(_game)) != 0) {
    switch (a) {
      case CBT_ERROR_OPTIONS_FLAGS:
        game_message("Your map must have at least one moving unit");
        break;
      case CBT_ERROR_OPTIONS_MOVING:
        game_message("Your map must have at least one moving unit");
        break;
      case CBT_ERROR_OPTIONS_SIZE:
        game_message("Your map doesn't allow all the players to place their starting units on their starting positions");
        break;
    }
    return -1;
  }

  if (ggz_write_int(cbt_info.fd, CBT_MSG_OPTIONS) < 0 || ggz_write_string(cbt_info.fd, game_str) < 0)
    return -1;


  gtk_widget_destroy(options_dialog);

  return 0;
  
}
  
gboolean
game_refuse_options                     (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data) {

  ggz_write_int(cbt_info.fd, CBT_MSG_OPTIONS);
  ggz_write_string(cbt_info.fd, "\0");
  return FALSE;

}
