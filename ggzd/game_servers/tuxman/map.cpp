// Tuxman Server
// Copyright (C) 2003, 2004 Josef Spillner <josef@ggzgamingzone.org>
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
	directions = NULL;
	flags = NULL;
	m_width = 0;
	m_height = 0;
	m_pacmans = 0;
	m_monsters = 0;
}

Map::~Map()
{
	for(int j = 0; j < m_height; j++)
		delete field[j];
	delete field;
	for(int j = 0; j < m_height; j++)
		delete directions[j];
	delete directions;
	for(int j = 0; j < m_height; j++)
		delete flags[j];
	delete flags;
}

bool Map::load(const char *file)
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
		directions = new int*[1024];
		flags = new int*[1024];
		while(f.get(c))
		{
			if(!hx)
			{
				hy++;
				m_height++;
				field[hy] = new int[1024];
				directions[hy] = new int[1024];
				flags[hy] = new int[1024];
				for(int i = 0; i < 1024; i++)
				{
					field[hy][i] = tile_none;
					directions[hy][i] = dir_unknown;
					flags[hy][i] = flag_normal;
				}
			}
			if(c == '#') field[hy][hx] = tile_wall;
			else if(c == 'O')
			{
				field[hy][hx] = tile_pacman;
				m_pacmans++;
			}
			else if(c == 'M')
			{
				field[hy][hx] = tile_monster;
				m_monsters++;
			}
			else if(c == 'B') field[hy][hx] = tile_bonus;
			else if(c == 'S') field[hy][hx] = tile_stop;
			else if(c == 'X') field[hy][hx] = tile_cross;
			else if(c == 'L') field[hy][hx] = tile_life;
			else if(c == '.') field[hy][hx] = tile_food;
			else if(c == '\n') hx = -1;
			hx++;
			if(hx > m_width) m_width = hx;
		}
		f.close();
	}
	else return false;
	return true;
}

void Map::setTile(int x, int y, int type)
{
	field[y][x] = type;
}

int Map::tile(int x, int y)
{
	return field[y][x];
}

void Map::setDirection(int x, int y, int dir)
{
	directions[y][x] = dir;
}

int Map::direction(int x, int y)
{
	return directions[y][x];
}

void Map::setFlag(int x, int y, int state)
{
	flags[y][x] = state;
}

int Map::flag(int x, int y)
{
	return flags[y][x];
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
	std::cout << "Map has " << monsters() << " monsters and "
		<< pacmans() << " pacmans" << std::endl;
	for(int j = 0; j < m_height; j++)
	{
		for(int i = 0; i < m_width; i++)
			std::cout << (int)field[j][i];
		std::cout << std::endl << std::flush;
	}
}

int Map::monsters()
{
	return m_monsters;
}

int Map::pacmans()
{
	return m_pacmans;
}

