#include "level.h"

#include <ggz.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Level::Level()
{
	m_title = NULL;
	m_author = NULL;
	m_version = NULL;
	m_width = 0;
	m_height = 0;
	m_players = 0;
	m_cell = NULL;
	m_cellboard = NULL;
}

Level::~Level()
{
	if(m_title) ggz_free(m_title);
	if(m_author) ggz_free(m_author);
	if(m_version) ggz_free(m_version);
	for(int i = 0; i < m_height; i++)
	{
		ggz_free(m_cell[i]);
		ggz_free(m_cellboard[i]);
	}
	ggz_free(m_cellboard);
	ggz_free(m_cell);
}

void Level::loadFromNetwork(int fd)
{
	ggz_read_string_alloc(fd, &m_title);
	ggz_read_string_alloc(fd, &m_author);
	ggz_read_string_alloc(fd, &m_version);
	ggz_read_int(fd, &m_width);
	ggz_read_int(fd, &m_height);

	m_cell = (char**)ggz_malloc(m_height * sizeof(char*));
	m_cellboard = (char**)ggz_malloc(m_height * sizeof(char*));

	for(int j = 0; j < m_height; j++)
	{
		m_cell[j] = (char*)ggz_malloc(30);
		ggz_readn(fd, m_cell[j], 30);
	}
	for(int j = 0; j < m_height; j++)
	{
		m_cellboard[j] = (char*)ggz_malloc(30);
		ggz_readn(fd, m_cellboard[j], 30);
	}
}

void Level::loadFromFile(const char *filename)
{
	FILE *f;
	char buf[128];
	enum states {state_title, state_author, state_version, state_anywhere, state_knights, state_nowhere, state_map};
	enum qualities {quality_ok, quality_bad, quality_error};
	int state, quality, i, j;
	int x, y, mapx, mapy;

	const int verbose = 1;

	if(verbose) printf("Load map: %s\n", filename);
	f = fopen(filename, "r");
	if(!f) return;

	state = state_title;
	quality = quality_ok;
	i = 0;
	x = 0;
	y = 0;
	mapx = 0;
	mapy = 0;

	m_players = 0;

	m_cell = (char**)ggz_malloc(11 * sizeof(char*));
	m_cellboard = (char**)ggz_malloc(11 * sizeof(char*));

	while(fgets(buf, sizeof(buf), f))
	{
		i++;
		if(i > 100)
		{
			quality = quality_bad;
			return;
		}

		if((buf[0] != '\"') && (state == state_knights)) state = state_nowhere;
		if((buf[0] == '\"') && (state == state_nowhere)) state = state_map;
		if((buf[0] == '\"') && (state == state_anywhere)) state = state_knights;

		if((strlen(buf) > 3) && (buf[0] == '\"'))
		{
			buf[strlen(buf) - 2] = 0;
			switch(state)
			{
				case state_title:
					m_title = ggz_strdup(buf + 1);
					state = state_author;
					break;
				case state_author:
					m_author = ggz_strdup(buf + 1);
					state = state_version;
					break;
				case state_version:
					m_version = ggz_strdup(buf + 1);
					state = state_anywhere;
					break;
				case state_knights:
					if(y < 10)
					{
						m_cell[y] = (char*)ggz_malloc(30);
						bzero(m_cell[y], 30);
						if(strlen(buf + 1) > 30)
						{
							buf[31] = 0;
							quality = quality_bad;
						}
						memcpy(m_cell[y], buf + 1, strlen(buf + 1));
						if(strlen(buf + 1) > x) x = strlen(buf + 1);
						for(j = 0; j < 30; j++)
						{
							/* Humancode-to-computercode conversion */
							if(m_cell[y][j] == 32) m_cell[y][j] = -1;
							else m_cell[y][j] -= 48;
							if(m_cell[y][j] + 1> m_players) m_players = m_cell[y][j] + 1;
						}
						y++;
					}
					else quality = quality_bad;
					break;
				case state_map:
					if(mapy < 10)
					{
						m_cellboard[mapy] = (char*)ggz_malloc(30);
						bzero(m_cellboard[mapy], 30);
						if(strlen(buf + 1) > 30)
						{
							buf[31] = 0;
							quality = quality_bad;
						}
						memcpy(m_cellboard[mapy], buf + 1, strlen(buf + 1));
						if(strlen(buf + 1) > mapx) mapx = strlen(buf + 1);
						mapy++;
					}
					else quality = quality_bad;
					break;
			}
		}
	}
	if(verbose)
	{
		printf("Try map: [%s] [%s] [%s]\n", m_title, m_author, m_version);
		printf("State is %i, sizes are %i/%i, %i/%i\n", state, x, y, mapx, mapy);
		printf("Detected %i different players\n", m_players);
	}

	/* More sanity checks */
	if(state != state_map) quality = quality_error;
	if((x != mapx) || (y != mapy)) quality = quality_error;
	m_height = y;
	m_width = x;

	/* Consider map if quality is bad at maximum */
	if((quality == quality_ok) || (quality == quality_bad))
	{
		if(verbose) printf("Accepted map: [%s] [%s] [%s] (%i)\n", m_title, m_author, m_version, quality);
	}

}

const char *Level::title()
{
	return m_title;
}

const char *Level::version()
{
	return m_version;
}

const char *Level::author()
{
	return m_author;
}

int Level::width()
{
	return m_height;
}

int Level::height()
{
	return m_width;
}

int Level::players()
{
	return m_players;
}

char Level::cell(int i, int j)
{
	if((i >= 0) && (i < m_height) && (j >= 0) && (j < m_width))
		return m_cell[i][j];
	return -1;
}

char Level::cellboard(int i, int j)
{
	if((i >= 0) && (i < m_height) && (j >= 0) && (j < m_width))
		return m_cellboard[i][j];
	return -1;
}

