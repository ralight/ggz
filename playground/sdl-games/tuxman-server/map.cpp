// Tuxman Server
// Copyright (C) 2003 Josef Spillner <josef@ggzgamingzone.org>
// Tuxman Copyright (C) 2003 Robert Strobl <badwolf@acoderz.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "map.h"
#include <ios>
#include <fstream>
#include <iostream>
#include <string>

Map::Map()
{
	field = NULL;
	m_width = 0;
	m_height = 0;
}

Map::~Map()
{
	for(int j = 0; j < m_height; j++)
		delete field[j];
	delete field;
}

void Map::load(const char *file)
{
	char c;
	std::fstream f;
	int hx = 0, hy = -1;

	m_width = 0;
	m_height = 0;

	f.open(file, std::ios::in);
	if(f.is_open())
	{
		field = new int*[1024];
		while(f.get(c))
		{
			if(!hx)
			{
				hy++;
				m_height++;
				field[hy] = new int[1024];
				for(int i = 0; i < 1024; i++)
					field[hy][i] = tile_none;
			}
			if(c == '#') field[hy][hx] = tile_wall;
			else if(c == 'M') field[hy][hx] = tile_monster;
			else if(c == 'B') field[hy][hx] = tile_bomb;
			else if(c == 'S') field[hy][hx] = tile_stop;
			else if(c == 'X') field[hy][hx] = tile_cross;
			else if(c == 'L') field[hy][hx] = tile_life;
			else if(c == '\n') hx = -1;
			hx++;
			if(hx > m_width) m_width = hx;
		}
		f.close();
	}
}

void Map::setTile(int x, int y, int type)
{
	field[x][y] = type;
}

int Map::tile(int x, int y)
{
	return field[x][y];
}

int Map::width()
{
	return m_width;
}

int Map::height()
{
	return m_height;
}

void Map::dump()
{
	for(int j = 0; j < m_height; j++)
	{
		for(int i = 0; i < m_width; i++)
			std::cout << (int)field[j][i];
		std::cout << std::endl << std::flush;
	}
}

