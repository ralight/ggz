/*
 * File: connect.c
 * Author: Brent Hendricks
 * Project: GGZ Client
 * Date: 1/23/99
 *
 * This fils contains functions for connecting with the server
 *
 * Copyright (C) 1998 Brent Hendricks.
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


#include <config.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <gtk/gtk.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "connect.h"
#include "datatypes.h"
#include "dlg_error.h"
#include "dlg_main.h"
#include "dlg_msgbox.h"
#include "dlg_motd.h"
#include "easysock.h"
#include "err_func.h"
#include "game.h"
#include "protocols.h"
#include "seats.h"


/* Global state of game variable */
extern struct ConnectInfo connection;
extern struct Game game;
extern struct Users users;
extern struct GameTypes game_types;
extern GtkWidget *detail_window;
extern GtkWidget *main_win;
extern GtkWidget *dlg_motd;
extern int selected_table;
extern int selected_type;
extern GdkColor colors[];

GtkWidget *detail_window = NULL;

/* Various local handles */
static guint sock_handle;
static void server_sync();
static void connect_msg(const char *, ...);
static void add_user_list(gchar * name, gint table);
static void add_table_list(TableInfo table);
static void handle_server_fd(gpointer, gint, GdkInputCondition);
static void handle_list_tables(int op, int fd);
static void motd_print_line(char *line);


char *opcode_str[] = { 	"MSG_SERVER_ID",
			"MSG_SERVER_FULL",
			"MSG_MOTD",
			"MSG_CHAT",
			"MSG_UPDATE_PLAYERS",
			"MSG_UPDATE_TYPES",
			"MSG_UPDATE_TABLES",
			"MSG_ERROR",
			"RSP_LOGIN_NEW",
			"RSP_LOGIN",
			"RSP_LOGIN_ANON",
			"RSP_LOGOUT",
			"RSP_PREF_CHANGE",
			"RSP_REMOVE_USER",
			"RSP_LIST_PLAYERS",
			"RSP_LIST_TYPES",
			"RSP_LIST_TABLES",
			"RSP_TABLE_OPTIONS",
			"RSP_USER_STAT",
			"RSP_TABLE_LAUNCH",
			"RSP_TABLE_JOIN",
			"RSP_TABLE_LEAVE",
			"RSP_GAME",
			"RSP_CHAT"
};


/**
 * Begin connection to server.  
 */
int connect_to_server(void)
{
	if (FAIL(connection.sock = es_make_socket(ES_CLIENT, connection.port,
						  connection.server))) 
		return -1;

	sock_handle = gdk_input_add_full(connection.sock, GDK_INPUT_READ,
					 handle_server_fd, NULL, NULL);

	return 0;
}


/*
 * disconnect from server, close sockets, etc.
 */
void disconnect(GtkWidget * widget, gpointer data)
{
	if (sock_handle)
		gdk_input_remove(sock_handle);
	sock_handle = 0;

	close(connection.sock);
	connection.connected = FALSE;
}


/**
 * Read data from the Spades server 
 */
void handle_server_fd(gpointer data, gint source, GdkInputCondition cond)
{
	gpointer tmp;
	char *message;
	char name[9];
	char status;
	int num, op, size, checksum, count, i;
	char buf[4096];
	static int color_index=0;

        if (FAIL(es_read_int(source, &op))) {
	        disconnect(NULL, NULL);
	        return;
	}
	
	switch (op) {
	case MSG_SERVER_ID:
		es_read_string_alloc(source, &message);
		connect_msg("[%s] %s\n", opcode_str[op], message);
		switch (connection.login_type) {
		case 0:	/*Normal login */
		case 2:	/*First time login */
		case 1:	/*Anonymous login */
			anon_login();
		}
		break;

	case MSG_SERVER_FULL:
		break;

	case RSP_LOGIN_ANON:
		es_read_char(source, &status);
		connect_msg("[%s] %d\n", opcode_str[op], status);
		if (status < 0) {
			login_bad_name();
			warn_dlg("That username is already in usage,\n or not permitted on this server.\n\nPlease choose a different name");

			return;
		}
		es_read_int(source, &checksum);
		login_ok();
		connect_msg("[%s] Checksum = %d\n", opcode_str[op], checksum);
		/*server_sync();*/
		break;

	case RSP_TABLE_LAUNCH:
		es_read_char(source, &status);
		connect_msg("[%s] %d\n", opcode_str[op], status);
		if (status < 0) {
			/* FIXME: Don't really need to disconnect */
			disconnect(NULL, NULL);
			return;
		}
		connection.playing = TRUE;
		break;

	case RSP_TABLE_JOIN:
		es_read_char(source, &status);
		connect_msg("[%s] %d\n", opcode_str[op], status);
		if (status >= 0) {
			connection.playing = TRUE;
			launch_game(selected_type,0);
		} else {
			switch (status){
			case -5: 
				warn_dlg("No table selected to join.");
				break;
			case -4:
				warn_dlg("Sorry, The table is full.");
				break;
			}
		}
		break;

	case RSP_LOGOUT:
		es_read_char(source, &status);
		connect_msg("[%s] %d\n", opcode_str[op], status);
		disconnect(NULL, NULL);
		msg(0,"Bye-Bye Now","Thanks for trying out GNU-Gaming Zone");
		break;

	case RSP_LIST_TYPES:
		es_read_int(source, &count);
		game_types.count = count;
		connect_msg("[%s] List Count %d\n", opcode_str[op], count);
		for (i = 0; i < count; i++) {

			es_read_int(source, &game_types.info[i].index);
			es_read_string(source, game_types.info[i].name);
			es_read_string(source, game_types.info[i].version);
			es_read_char(source, &game_types.info[i].num_play_allow);
			es_read_string(source, game_types.info[i].desc);
			es_read_string(source, game_types.info[i].author);
			es_read_string(source, game_types.info[i].web);

			connect_msg("[%s] Game type %d\n", opcode_str[op], 
				    game_types.info[i].index);
			connect_msg("[%s] %s\n", opcode_str[op], 
				    game_types.info[i].name);
			connect_msg("[%s] %s\n", opcode_str[op], 
				    game_types.info[i].version);
			connect_msg("[%s] %s\n", opcode_str[op], 
				    game_types.info[i].desc);
			connect_msg("[%s] %s\n", opcode_str[op], 
				    game_types.info[i].author);
			connect_msg("[%s] %s\n", opcode_str[op], 
				    game_types.info[i].web);
		}
		break;

	case RSP_LIST_TABLES:
		handle_list_tables(op, source);
		break;

	case RSP_LIST_PLAYERS:
		tmp = gtk_object_get_data(GTK_OBJECT(main_win), "player_list");
		gtk_clist_clear(GTK_CLIST(tmp));
		es_read_int(source, &count);
		connect_msg("[%s] User List Count %d\n", opcode_str[op], count);
		for (i = 0; i < count; i++) {
			color_index++;
			if ((color_index > 9) || (color_index<0))
				color_index = 0;
			es_read_string(source, name);
			connect_msg("[%s] User %s\n", opcode_str[op], name);
			es_read_int(source, &num);
			connect_msg("[%s] Table %d\n", opcode_str[op], num);
			add_user_list(name, num);
			users.count++;
			strcpy(users.info[i].name, name);
			users.info[i].chat_color=color_index;
		}
		users.count=count;
		break;

	case RSP_GAME:
		es_read_int(source, &size);
		connect_msg("[%s] %d bytes\n", opcode_str[op], size);
		es_readn(source, buf, size);
		status = es_writen(game.fd, buf, size);
		if (status <= 0) {	/* Game over */
			dbg_msg("Game is over");
			connection.playing = 0;
			close(game.fd);
		}
		break;

	case RSP_CHAT:
		es_read_char(source, &status);
		connect_msg("[%s] %d\n", opcode_str[op], status);
		break;

	case MSG_CHAT:
		es_read_string(source, name);
		connect_msg("[%s] msg from %s\n", opcode_str[op], name);
		es_read_string_alloc(source, &message);
		connect_msg("[%s] %s\n", opcode_str[op], message);
		display_chat(name, message);
		g_free(message);
		break;

	case RSP_LOGIN:
		es_read_char(source, &status);
		connect_msg("[%s] %d\n", opcode_str[op], status);
		break;

	case MSG_UPDATE_PLAYERS:
		connect_msg("[%s]\n", opcode_str[op]);
		ggz_get_players(NULL, NULL);
		break;

	case MSG_UPDATE_TYPES:
		connect_msg("[%s]\n", opcode_str[op]);
		ggz_get_types(NULL, NULL);
		break;

	case MSG_UPDATE_TABLES:
		connect_msg("[%s]\n", opcode_str[op]);
		ggz_get_tables(NULL, NULL);
		break;

	case RSP_LOGIN_NEW:
	case MSG_MOTD:
		dlg_motd = create_dlgMOTD();
		es_read_int(source, &count);
		connect_msg("[%s] MOTD line count %d\n", opcode_str[op], count);
		for (i = 0; i < count; i++) {
			es_read_string_alloc(source, &message);
			connect_msg("[%s] %s",opcode_str[op], message);
			motd_print_line(message);
		}
		gtk_widget_show(dlg_motd);
		break;
	case RSP_PREF_CHANGE:
	case RSP_REMOVE_USER:
	case RSP_TABLE_OPTIONS:
	case RSP_USER_STAT:
		break;

	}
}


void connect_msg(const char *format, ...)
{
	va_list ap;
	char *message;
	gpointer tmp;

	/* If we're not displaying details then don't bother */
	if (detail_window == NULL)
		return;
	
	va_start(ap, format);
	message = g_strdup_vprintf(format, ap);
	va_end(ap);


	tmp = gtk_object_get_data(GTK_OBJECT(detail_window), "text");
	gtk_text_insert(GTK_TEXT(tmp), NULL, NULL, NULL, message, -1);
	
	g_free(message);
}


void add_user_list(gchar * name, gint table)
{
	gpointer tmp;
	gchar* entry[2];
	
	if (main_win == NULL)
		return;
	
	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "player_list");

	entry[0] = name;
	if (table == -1)
		entry[1] = "none";
	else 
		entry[1] = g_strdup_printf("%d", table);

	gtk_clist_append(GTK_CLIST(tmp), entry);

	if (table != -1)
		g_free(entry[1]);
}


void add_table_list(TableInfo table)
{
	gpointer tmp;
	gchar *entry[7];

	if (main_win == NULL)
		return;

	entry[0] = "";
	entry[1] = g_strdup_printf("%d", table.table_index);
	entry[2] = g_strdup_printf("%s", 
				   game_types.info[table.type_index].name);
	entry[3] = g_strdup_printf("%d", seats_num(table));
	entry[4] = g_strdup_printf("%d", seats_open(table));
	entry[5] = g_strdup_printf("%d", seats_human(table));
	entry[6] = g_strdup_printf("%s", table.desc);

	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "table_tree");

	gtk_clist_append(GTK_CLIST(tmp), entry);
	g_free(entry[1]);
	g_free(entry[2]);
	g_free(entry[3]);
	g_free(entry[4]);
	g_free(entry[5]);
	g_free(entry[6]);

}


int anon_login(void)
{

	es_write_int(connection.sock, REQ_LOGIN_ANON);
	es_write_string(connection.sock, connection.username);

	return 0;
}


/* Complete sync with server */ 
static void server_sync()
{
	es_write_int(connection.sock, REQ_LIST_PLAYERS);
	es_write_int(connection.sock, REQ_LIST_TYPES);
	es_write_char(connection.sock, 1);
	es_write_int(connection.sock, REQ_LIST_TABLES);
	es_write_int(connection.sock, -1);
} 


void display_chat(char *name, char *msg)
{
	char *buf;			/* incomeing text */
	gpointer tmp;			/* chat widget */
	GdkColormap *cmap;		/* system colormap */
	GdkFont *fixed_font;		/* font for chat */
	int color_index;		/* color for chat */
	char cmd[1024];			/* command used in chat */
	char out[1024];			/* text following command */
	char *line;			/* line to parse */
	int cmd_index=0, out_index=0;	/* indexes */

	line=g_strdup(msg);
	strcpy(line,msg);

	/* Get color for user */
	color_index=1;
	if (!strcmp(connection.username, name))
		color_index=0;

	/* Load a fixed-size font for chat, */
	/* if not fixed size, things don't look good */
	fixed_font = gdk_font_load ("-misc-fixed-medium-r-normal--10-100-75-75-c-60-iso8859-1");
		
	/* Get the system color map and allocate the color. */
	cmap = gdk_colormap_get_system();
	if (!gdk_color_alloc(cmap, &colors[color_index])) {
		g_error("couldn't allocate color");
	}

        
        /* Skip until we find the end of the command, converting */
        /* everything (for convenience) to lowercase as we go */
        while(*line != ' ' && *line != '\n' && *line != '\0'){
                *line = tolower(*line);
		cmd[cmd_index]=*line;
                line++;
		cmd_index++;
        }
	cmd[cmd_index]='\0';
        line++;

	/* get all remaining text */
        while(*line != '\0'){
		out[out_index]=*line;
                line++;
		out_index++;
        }
	out[out_index]='\0';

	if (!strcmp(cmd,"/me")){		/* Action, like in IRC */
		tmp = gtk_object_get_data(GTK_OBJECT(main_win), "chat_text");
		buf = g_strdup_printf("%*s** %s", MAX_USER_NAME_LEN+1-strlen(name), " ", name);
		gtk_text_insert(GTK_TEXT(tmp), fixed_font, &colors[color_index], NULL, buf, -1);
		g_free(buf);
		buf = g_strdup_printf(" %s\n", out);
		gtk_text_insert(GTK_TEXT(tmp), fixed_font, NULL, NULL, buf, -1);
		g_free(buf);
	}else if (!strcmp(cmd,"/yell")){	/* Yelling in chat */
		tmp = gtk_object_get_data(GTK_OBJECT(main_win), "chat_text");
		buf = g_strdup_printf("%*s** %s", MAX_USER_NAME_LEN+1-strlen(name), " ", name);
		gtk_text_insert(GTK_TEXT(tmp), fixed_font, &colors[color_index], NULL, buf, -1);
		g_free(buf);
		buf = g_strdup_printf(" yells from across the room \"%s\".\n", out);
		gtk_text_insert(GTK_TEXT(tmp), fixed_font, NULL, NULL, buf, -1);
		g_free(buf);
	}else if (!strcmp(cmd,"/beep")){	/* Beep a person threw chat */
		if (!strcmp(out,connection.username)){

			/* Big old hack, got to be a better way */
			printf("\a\n");
			/* End big old hack */

			tmp = gtk_object_get_data(GTK_OBJECT(main_win), "chat_text");
			buf = g_strdup_printf("%*s** %s", MAX_USER_NAME_LEN+1-strlen(name), " ", name);
			gtk_text_insert(GTK_TEXT(tmp), fixed_font, &colors[color_index], NULL, buf, -1);
			g_free(buf);
			buf = g_strdup_printf(" is beeping you!\n");
			gtk_text_insert(GTK_TEXT(tmp), fixed_font, NULL, NULL, buf, -1);
			g_free(buf);
		}else{
			tmp = gtk_object_get_data(GTK_OBJECT(main_win), "chat_text");
			buf = g_strdup_printf("%*s** %s", MAX_USER_NAME_LEN+1-strlen(name), " ", name);
			gtk_text_insert(GTK_TEXT(tmp), fixed_font, &colors[color_index], NULL, buf, -1);
			g_free(buf);
			buf = g_strdup_printf(" beeped %s.\n", out);
			gtk_text_insert(GTK_TEXT(tmp), fixed_font, NULL, NULL, buf, -1);
			g_free(buf);
		}
	}else{		/* No command given, display it all */
		tmp = gtk_object_get_data(GTK_OBJECT(main_win), "chat_text");
		buf = g_strdup_printf("< %s >%*s", name, MAX_USER_NAME_LEN+1-strlen(name), " ");
		gtk_text_insert(GTK_TEXT(tmp), fixed_font, &colors[color_index], NULL, buf, -1);
		g_free(buf);
		buf = g_strdup_printf("%s\n", msg);
		gtk_text_insert(GTK_TEXT(tmp), fixed_font, NULL, NULL, buf, -1);
		g_free(buf);
	}
}


void handle_list_tables(int op, int fd)
{
	int i, j,  count, num;
	GtkObject* tmp;
	TableInfo table;
	
	selected_table = -1;
	tmp = gtk_object_get_data(GTK_OBJECT(main_win), "table_tree");
	gtk_clist_clear(GTK_CLIST(tmp));
	es_read_int(fd, &count);
	connect_msg("[%s] Table List Count %d\n", opcode_str[op], count);
	for (i = 0; i < count; i++) {
		es_read_int(fd, &table.table_index);
		es_read_int(fd, &table.type_index);
		es_read_string(fd, table.desc);
		es_read_char(fd, &table.playing);
		es_read_int(fd, &num);
		
		for (j = 0; j < num; j++) {
			es_read_int(fd, &table.seats[j]);
			if (table.seats[j] >= 0
			    || table.seats[j] == GGZ_SEAT_RESV) 
				es_read_string(fd, &table.names[j]);
		}
		for (j = num; j < MAX_TABLE_SIZE; j++)
			table.seats[j] = GGZ_SEAT_NONE;
		

		connect_msg("[%s] Type: %d\n", opcode_str[op],table.type_index);
		connect_msg("[%s] Playing: %d\n",opcode_str[op], table.playing);
		connect_msg("[%s] Seats: %d\n", opcode_str[op], num);
		connect_msg("[%s] Desc: %s\n", opcode_str[op], table.desc);
		
		add_table_list(table);
	}

}

void motd_print_line(char *line)
{
        char out[1024];
        int lindex=0;
        int oindex=0;
	GtkWidget *temp_widget;
	GdkColormap *cmap;
	GdkFont *fixed_font;
	int color_index=9; /* Black */


	cmap = gdk_colormap_get_system();
	if (!gdk_color_alloc(cmap, &colors[color_index])) {
		g_error("couldn't allocate color");
	}

	temp_widget = gtk_object_get_data(GTK_OBJECT(dlg_motd), "txtMOTD");
	fixed_font = gdk_font_load ("-misc-fixed-medium-r-normal--10-100-75-75-c-60-iso8859-1");

        while(line[lindex] != '\0')
        {
                if (line[lindex] == '%')
                {
                        lindex++;
                        if (line[lindex] == 'c')
                        {
                                lindex++;
                                switch (line[lindex]) {
                                        case '0':
                                                out[oindex]='\0';
						gtk_text_insert (GTK_TEXT (temp_widget), fixed_font,
								&colors[color_index], NULL, out, -1);
						color_index=0;
						cmap = gdk_colormap_get_system();
						if (!gdk_color_alloc(cmap, &colors[color_index])) {
							g_error("couldn't allocate color");
						}
                                                oindex=0;
                                                lindex++;
                                                break;
                                        case '1':
                                                out[oindex]='\0';
						gtk_text_insert (GTK_TEXT (temp_widget), fixed_font,
								&colors[color_index], NULL, out, -1);
						color_index=1;
						cmap = gdk_colormap_get_system();
						if (!gdk_color_alloc(cmap, &colors[color_index])) {
							g_error("couldn't allocate color");
						}
                                                oindex=0;
                                                lindex++;
                                                break;
                                        case '2':
                                                out[oindex]='\0';
						gtk_text_insert (GTK_TEXT (temp_widget), fixed_font,
								&colors[color_index], NULL, out, -1);
						color_index=2;
						cmap = gdk_colormap_get_system();
						if (!gdk_color_alloc(cmap, &colors[color_index])) {
							g_error("couldn't allocate color");
						}
                                                oindex=0;
                                                lindex++;
                                                break;
                                        case '3':
                                                out[oindex]='\0';
						gtk_text_insert (GTK_TEXT (temp_widget), fixed_font,
								&colors[color_index], NULL, out, -1);
						color_index=3;
						cmap = gdk_colormap_get_system();
						if (!gdk_color_alloc(cmap, &colors[color_index])) {
							g_error("couldn't allocate color");
						}
                                                oindex=0;
                                                lindex++;
                                                break;
                                        case '4':
                                                out[oindex]='\0';
						gtk_text_insert (GTK_TEXT (temp_widget), fixed_font,
								&colors[color_index], NULL, out, -1);
						color_index=4;
						cmap = gdk_colormap_get_system();
						if (!gdk_color_alloc(cmap, &colors[color_index])) {
							g_error("couldn't allocate color");
						}
                                                oindex=0;
                                                lindex++;
                                                break;
                                        case '5':
                                                out[oindex]='\0';
						gtk_text_insert (GTK_TEXT (temp_widget), fixed_font,
								&colors[color_index], NULL, out, -1);
						color_index=5;
						cmap = gdk_colormap_get_system();
						if (!gdk_color_alloc(cmap, &colors[color_index])) {
							g_error("couldn't allocate color");
						}
                                                oindex=0;
                                                lindex++;
                                                break;
                                        case '6':
                                                out[oindex]='\0';
						gtk_text_insert (GTK_TEXT (temp_widget), fixed_font,
								&colors[color_index], NULL, out, -1);
						color_index=6;
						cmap = gdk_colormap_get_system();
						if (!gdk_color_alloc(cmap, &colors[color_index])) {
							g_error("couldn't allocate color");
						}
                                                oindex=0;
                                                lindex++;
                                                break;
                                        case '7':
                                                out[oindex]='\0';
						gtk_text_insert (GTK_TEXT (temp_widget), fixed_font,
								&colors[color_index], NULL, out, -1);
						color_index=7;
						cmap = gdk_colormap_get_system();
						if (!gdk_color_alloc(cmap, &colors[color_index])) {
							g_error("couldn't allocate color");
						}
                                                oindex=0;
                                                lindex++;
                                                break;
                                        case '8':
                                                out[oindex]='\0';
						gtk_text_insert (GTK_TEXT (temp_widget), fixed_font,
								&colors[color_index], NULL, out, -1);
						color_index=8;
						cmap = gdk_colormap_get_system();
						if (!gdk_color_alloc(cmap, &colors[color_index])) {
							g_error("couldn't allocate color");
						}
                                                oindex=0;
                                                lindex++;
                                                break;
                                        case '9':
                                                out[oindex]='\0';
						gtk_text_insert (GTK_TEXT (temp_widget), fixed_font,
								&colors[color_index], NULL, out, -1);
						color_index=9;
						cmap = gdk_colormap_get_system();
						if (!gdk_color_alloc(cmap, &colors[color_index])) {
							g_error("couldn't allocate color");
						}
                                                oindex=0;
                                                lindex++;
                                                break;
                                        default:
                                                lindex--;
                                                lindex--;
                                }
                        }else{
                                lindex--;
                        }
                }
                out[oindex]=line[lindex];
                lindex++;
                oindex++;
        }
        out[oindex]='\0';
	gtk_text_insert (GTK_TEXT (temp_widget), fixed_font,
			&colors[color_index], NULL, out, -1);
}



