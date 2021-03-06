/*
 * File: map.c
 * Author: Ismael Orenstein
 * Project: GGZ Combat game module
 * Date: 09/17/2000
 * Desc: Game functions
 * $Id: map.c 9953 2008-04-12 22:56:10Z oojah $
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

#include <assert.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <ggz.h>

#include "combat.h"
#include "map.h"

#ifdef MKDIR_TAKES_ONE_ARG
#define mkdir(a, b) (mkdir) (a)
#endif

#define GLOBAL_MAPS GGZDATADIR "/combat/maps"

void game_message(const char *format, ...);

static unsigned int _generate_hash(char *);

static int _maps_only(const struct dirent *entry);

static char file_unit_name[12][36] =
    { "flag", "bomb", "spy", "scout", "miner", "sergeant", "lieutenant",
	"captain", "major", "colonel", "general", "marshall"
};

static unsigned int _generate_hash(char *p)
{
	unsigned int sum = 0;
	unsigned char bitsave;
	// Use Rich's ROL algorithm
	while (*p != 0) {
		bitsave = (sum & (int)0x80000000L);
		sum *= 2;
		sum = sum ^ *p;
		sum = sum + bitsave;
		p++;
	}
	return sum;
}


int map_save(combat_game * map)
{
	unsigned int hash;
	char filename[128];
	char options[20];
	char *map_data;
	int handle, a;
	hash = _generate_hash((char*)combat_options_string_write(map, 1));
	snprintf(filename, sizeof(filename), "%s/%s.%u", GLOBAL_MAPS, map->name, hash);
	handle = ggz_conf_parse(filename, GGZ_CONF_RDWR | GGZ_CONF_CREATE);
	if (handle < 0) {
		snprintf(filename, sizeof(filename), "%s/.ggz/combat/maps/%s.%u",
			getenv("HOME"), map->name, hash);
		handle =
		    ggz_conf_parse(filename,
				   GGZ_CONF_RDWR | GGZ_CONF_CREATE);
		if (handle < 0) {
			snprintf(filename, sizeof(filename), "%s/.ggz", getenv("HOME"));
			mkdir(filename, S_IRWXU | S_IRGRP | S_IXGRP);

			snprintf(filename, sizeof(filename), "%s/.ggz/combat",
				getenv("HOME"));
			mkdir(filename, S_IRWXU | S_IRGRP | S_IXGRP);

			snprintf(filename, sizeof(filename), "%s/.ggz/combat/maps",
				getenv("HOME"));
			mkdir(filename, S_IRWXU | S_IRGRP | S_IXGRP);

			snprintf(filename, sizeof(filename), "%s/.ggz/combat/maps/%s.%u",
				getenv("HOME"), map->name, hash);
			handle =
			    ggz_conf_parse(filename,
					   GGZ_CONF_RDWR |
					   GGZ_CONF_CREATE);
			if (handle < 0) {
				// Give up
				game_message
				    ("I couldn't write the map to disk");
				return -1;
			}
		}
	}
	// Width/height
	ggz_conf_write_int(handle, "map", "width", map->width);
	ggz_conf_write_int(handle, "map", "height", map->height);
	// Army data
	for (a = 0; a < 12; a++)
		ggz_conf_write_int(handle, "army", file_unit_name[a],
				   map->army[map->number][a]);
	// Map data
	map_data = (char *)ggz_malloc(map->width * map->height + 1);
	for (a = 0; a < map->width * map->height; a++) {
		if (GET_OWNER(map->map[a].type) >= 0) {
			// Intial position!
			map_data[a] = GET_OWNER(map->map[a].type) + 49;
		} else {
			// Get's terrain
			switch (LAST(map->map[a].type)) {
			case T_OPEN:
				map_data[a] = 'O';
				break;
			case T_NULL:
				map_data[a] = 'N';
				break;
			case T_LAKE:
				map_data[a] = 'L';
				break;
			}
		}
	}
	map_data[map->width * map->height] = 0;
	ggz_conf_write_string(handle, "map", "data", map_data);
	// Options
	snprintf(options, sizeof(filename), "%lX", map->options);
	ggz_conf_write_string(handle, "options", "bin1", options);
	ggz_conf_commit(handle);
	return 0;
}

static int _maps_only(const struct dirent *entry)
{
	if (*entry->d_name == '.')
		return 0;
	if (strcmp(entry->d_name, "CVS") == 0)
		return 0;
	return 1;
}

int map_search(combat_game * map)
{
	unsigned int hash;
	char hash_str[32];
	int a;
	struct dirent *entry;
	char dir_name[2][50];
	DIR* dir;

	hash = _generate_hash((char*)combat_options_string_write(map, 1));
	snprintf(hash_str, sizeof(hash_str), ".%u", hash);
	snprintf(dir_name[0], sizeof(dir_name[0]), GLOBAL_MAPS);
	snprintf(dir_name[1], sizeof(dir_name[1]), "%s/.ggz/combat/maps", getenv("HOME"));
	for (a = 0; a < 2; a++) {
		dir = opendir(dir_name[a]);
		if (!dir) {
			/* ??? */
			continue;
		}
		while ((entry = readdir(dir))) {
			if (!_maps_only(entry)) continue;
			if (strstr(entry->d_name, hash_str) != NULL) {
				closedir(dir);
				return 1;
			}
		}
		closedir(dir);
	}
	return 0;
}

void map_load(combat_game * _game, char *filename, gboolean *changed)
{
	int handle;
	unsigned int hash;
	char hash_str[32];
	char *new_filename;
	char *options;
	int a, b;
	char *terrain_data;

	if (changed) *changed = FALSE;

	handle = ggz_conf_parse(filename, GGZ_CONF_RDONLY);
	if (handle < 0) {
		fprintf(stderr, "Couldn't load map %s.\n", filename);
		assert(0);
		return;
	}
	/* Get the data from the file */
	// Width / Height
	_game->width = ggz_conf_read_int(handle, "map", "width", 10);
	_game->height = ggz_conf_read_int(handle, "map", "height", 10);
	for (a = 0; a < 12; a++) {
		ARMY(_game, a) =
		    ggz_conf_read_int(handle, "army", file_unit_name[a],
				      0);
	}
	// Terrain data
	terrain_data = ggz_conf_read_string(handle, "map", "data", NULL);
	_game->map =
	    (tile *) ggz_malloc(_game->width * _game->height *
				sizeof(tile));
	if (terrain_data) {
		a = strlen(terrain_data);
		while (--a >= 0) {
			switch (terrain_data[a]) {
			case 'O':
				_game->map[a].type = T_OPEN;
				break;
			case 'L':
				_game->map[a].type = T_LAKE;
				break;
			case 'N':
				_game->map[a].type = T_NULL;
				break;
			case '1':
				_game->map[a].type = OWNER(0) + T_OPEN;
				break;
			case '2':
				_game->map[a].type = OWNER(1) + T_OPEN;
				break;
			default:
				_game->map[a].type = T_OPEN;
				break;
			}
		}
	}
	// Options
	options = ggz_conf_read_string(handle, "options", "bin1", NULL);
	if (options)
		sscanf(options, "%lx", &_game->options);
	a = strlen(filename) - 1;
	while (a >= 0 && filename[a] != '/')
		a--;
	b = a + 1;
	while (b < strlen(filename) && filename[b] != '.')
		b++;
	_game->name = (char *)ggz_malloc(b - a);
	strncpy(_game->name, filename + a + 1, b - a - 1);
	_game->name[b - a - 1] = 0;

	// Well, now that we have loaded the map, let's check it's hash!
	hash = _generate_hash((char*)combat_options_string_write(_game, 1));
	snprintf(hash_str, sizeof(hash_str), ".%u", hash);
	if (strstr(filename, hash_str) == NULL) {
		// Hash don't match!!
		ggz_error_msg
		    ("Filename for map %s should be %s.%u, and not %s",
		     _game->name, _game->name, hash, filename);
		// Let's rename it!
		new_filename =
		    (char *)ggz_malloc(strlen(filename) +
				       strlen(hash_str) + 14);
		for (a = strlen(filename); a >= 0; a--) {
			if (filename[a] == '.')
				break;
		}
		strncpy(new_filename, filename, a);
		new_filename[a] = 0;
		strcat(new_filename, hash_str);
		if (rename(filename, new_filename) != 0) {
			/* I have no idea why != 0 is used as the comparison
			   here.  It seems quite wrong. */
			if (changed) *changed = TRUE;
		}
	};
}

char **map_list(void)
{
	char dir_name[2][1024];
	struct dirent *entry;
	char **names = NULL;
	int map_number = 0, a;
	DIR *dir;

	/* This used to use scandir, but that isn't portable. */
	snprintf(dir_name[0], sizeof(dir_name[0]), "%s", GLOBAL_MAPS);
	snprintf(dir_name[1], sizeof(dir_name[1]), "%s/.ggz/combat/maps", getenv("HOME"));
	for (a = 0; a < 2; a++) {
		dir = opendir(dir_name[a]);
		if (!dir) continue; /* FIXME: what else? */
		while ((entry = readdir(dir))) {
			if (!_maps_only(entry)) continue;
			map_number++;
			names = realloc(names, map_number * sizeof(*names));
			names[map_number - 1]
			  = ggz_malloc(strlen(dir_name[a])
				       + strlen(entry->d_name) + 3);
			sprintf(names[map_number - 1], "%s/%s",
				dir_name[a], entry->d_name);
		}
		closedir(dir);
	}
	names = realloc(names, (map_number + 1) * sizeof(*names));
	names[map_number] = NULL; /* Terminator */
	return names;

}
