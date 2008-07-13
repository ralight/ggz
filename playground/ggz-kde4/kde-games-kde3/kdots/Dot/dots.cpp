///////////////////////////////////////////////////////////////
//
// Dots
// C++ Class Set for Connect the Dots games
// Copyright (C) 2001, 2002 Josef Spillner
// dr_maux@users.sourceforge.net
// The MindX Open Source Project
// http://mindx.sourceforge.net/games/kdots/
//
///////////////////////////////////////////////////////////////

#include "dots.h"

#include <cstdlib>
#include <iostream>

using namespace std;

Dots::Dots()
{
	m_field = NULL;
	cleanup();
}

Dots::~Dots()
{
	cleanup();
}

void Dots::resizeBoard(int cols, int rows)
{
	cleanup();
	m_moves.clear();

	m_rows = rows + 1;
	m_cols = cols + 1;

	m_field = (dot_t**)malloc(sizeof(dot_t) * m_cols);
	for(int i = 0; i < m_cols; i++)
	{
		m_field[i] = (dot_t*)malloc(sizeof(dot_t) * m_rows);
		for(int j = 0; j < m_rows; j++)
		{
   			m_field[i][j] = (dot_t)malloc(sizeof(dot_t) * 5);
			m_field[i][j][right] = -1;
			m_field[i][j][left] = -1;
			m_field[i][j][up] = -1;
			m_field[i][j][down] = -1;
			m_field[i][j][fieldrightbelow] = -1;
		}
	}
}

void Dots::assignBorder(int x, int y, int direction, int side)
{
	if(!m_field) return;
	if((x < 0) || (y < 0) || (x >= m_cols) || (y >= m_rows)) return;

	m_field[x][y][direction] = side;

	// check if field is now enclosed
	if((borders(x, y, -1) == 0) && (m_field[x][y][fieldrightbelow] == -1))
		m_field[x][y][fieldrightbelow] = side;
	if((x > 0) && (borders(x - 1, y, -1) == 0) && (m_field[x - 1][y][fieldrightbelow] == -1))
		m_field[x - 1][y][fieldrightbelow] = side;
	if((y > 0) && (borders(x, y - 1, -1) == 0) && (m_field[x][y - 1][fieldrightbelow] == -1))
		m_field[x][y - 1][fieldrightbelow] = side;
	if((x > 0) && (y > 0) && (borders(x - 1, y - 1, -1) == 0) && (m_field[x - 1][y - 1][fieldrightbelow] == -1))
		m_field[x - 1][y - 1][fieldrightbelow] = side;

	// likewise for the other way around
	if((borders(x, y, -1) > 0) && (m_field[x][y][fieldrightbelow] != -1))
		m_field[x][y][fieldrightbelow] = -1;
	if((x > 0) && (borders(x - 1, y, -1) > 0) && (m_field[x - 1][y][fieldrightbelow] != -1))
		m_field[x - 1][y][fieldrightbelow] = -1;
	if((y > 0) && (borders(x, y - 1, -1) > 0) && (m_field[x][y - 1][fieldrightbelow] != -1))
		m_field[x][y - 1][fieldrightbelow] = -1;
	if((x > 0) && (y > 0) && (borders(x - 1, y - 1, -1) > 0) && (m_field[x - 1][y - 1][fieldrightbelow] != -1))
		m_field[x - 1][y - 1][fieldrightbelow] = -1;
}

int Dots::setBorderValue(int x, int y, int direction, int side, int action)
{
	if(!m_field) return 0;
	if((x < 0) || (y < 0) || (x >= m_cols) || (y >= m_rows)) return 0;
	if((direction < 0) || (direction > 3)) return 0;
	if((x == 0) && (direction == left)) return 0;
	if((y == 0) && (direction == up)) return 0;
	if((x == m_cols - 1) && (direction == right)) return 0;
	if((y == m_rows - 1) && (direction == down)) return 0;
	if((m_field[x][y][direction] >= 0) && (side != -1)) return 0;
	if(action == check) return 1;

	assignBorder(x, y, direction, side);
	switch(direction)
	{
		case right:
			assignBorder(x + 1, y, left, side);
			break;
		case left:
			assignBorder(x - 1, y, right, side);
			break;
		case up:
			assignBorder(x, y - 1, down, side);
			break;
		case down:
			assignBorder(x, y + 1, up, side);
			break;
		default:
			cout << "CRITICAL DOT ERROR!" << endl;
	}

	Move m;
	m.x = x;
	m.y = y;
	m.side = side;
	m.direction = direction;
	if(action != replay) m_moves.append(m);

	return 1;
}

int Dots::count(int side)
{
	int ret;

	if(!m_field) return -1;

	ret = 0;
	for(int j = 0; j < m_rows - 1; j++)
		for(int i = 0; i < m_cols - 1; i++)
			if(content(i, j) == side) ret++;

	return ret;
}

void Dots::cleanup()
{
	if(m_field)
	{
		for(int i = 0; i < m_cols; i++)
		{
			for(int j = 0; j < m_rows; j++)
				free(m_field[i][j]);
			free(m_field[i]);
		}
		free(m_field);
	}
	m_rows = -1;
	m_cols = -1;
	m_field = NULL;
}

int Dots::borders(int x, int y, int side)
{
	int ret;

	if(!m_field) return -1;
	if((x < 0) || (y < 0) || (x >= m_cols - 1) || (y >= m_rows - 1)) return -1;

	ret = 0;
	if(m_field[x][y][right] == side) ret++;
	if(m_field[x][y][down] == side) ret++;
	if(m_field[x + 1][y + 1][left] == side) ret++;
	if(m_field[x + 1][y + 1][up] == side) ret++;
	return ret;
}

int Dots::border(int x, int y, int direction)
{
	if(!m_field) return -1;
	if((x < 0) || (y < 0) || (x >= m_cols) || (y >= m_rows)) return -1;

	return m_field[x][y][direction];
}

int Dots::content(int x, int y)
{
	if(!m_field) return -1;
	if((x < 0) || (y < 0) || (x >= m_cols - 1) || (y >= m_rows - 1)) return -1;

	return m_field[x][y][fieldrightbelow];
}

void Dots::setOwnership(int x, int y, int owner)
{
	if(!m_field) return;
	if((x < 0) || (y < 0) || (x >= m_cols - 1) || (y >= m_rows - 1)) return;

	m_field[x][y][fieldrightbelow] = owner;
}

