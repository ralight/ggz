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

#ifndef TUXMAN_MAP_H
#define TUXMAN_MAP_H

// Map object
class Map
{
	public:
		Map();
		~Map();
		bool load(const char *file);

		void setTile(int x, int y, int type);
		int tile(int x, int y);

		int width();
		int height();
		int monsters();
		int pacmans();

		void dump();

		enum Tiles
		{
			tile_none,
			tile_wall,
			tile_monster,
			tile_bonus,
			tile_stop,
			tile_kill,
			tile_cross,
			tile_life,
			tile_food,
			tile_pacman
		};

	private:
		int m_width, m_height;
		int m_monsters, m_pacmans;
		int **field;
};

#endif

