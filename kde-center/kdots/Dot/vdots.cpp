/////////////////////////////////////////////////////////////////////
//
// Dots: C++ classes for Connect the Dots games
// http://www.ggzgamingzone.org/gameclients/kdots/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

#include "vdots.h"

#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <qdebug.h>

VDots::VDots()
: Dots()
{
}

VDots::~VDots()
{
}

void VDots::vResizeBoard(int width, int height)
{
	m_width = width;
	m_height = height;
}

int VDots::absi(int i)
{
	if(i < 0) return -i;
	return i;
}

int VDots::vSetBorderValue(int x, int y, int side)
{
	int nx, ny, dx, dy, direction;
	int rh, rv;

	if((m_cols < 0) || (m_rows < 0)) return 0;

	nx = m_width / (m_cols - 1);
	ny = m_height / (m_rows - 1);

	dx = (x + nx / 2) / nx;
	dy = (y + ny / 2) / ny;

	rh = nx * dx - x;
	rv = ny * dy - y;

	if(absi(rh) >= absi(rv))
		if(rh <= 0)
			direction = right;
		else
			direction = left;
	else
		if(rv <= 0)
			direction = down;
		else
			direction = up;

	m_lastx = dx;
	m_lasty = dy;
	m_lastdirection = direction;
	return setBorderValue(dx, dy, direction, side, Dots::check);
}

bool VDots::save(QString filename)
{
	QList<Move>::Iterator it;

	QFile f(filename);
	if(!f.open(QIODevice::WriteOnly))
		return false;

	QTextStream t(&f);
	t << "width " << m_cols << endl;
	t << "height " << m_rows << endl;
	t << "player1 join Human" << endl;
	t << "player2 join AI" << endl;
	for(it = m_moves.begin(); it != m_moves.end(); it++)
	{
		int dir = (*it).direction;
		int x = (*it).x;
		int y = (*it).y;
		if(dir == 0)
		{
			dir = 1;
			x -= 1;
		}
		if(dir == 2)
		{
			dir = 3;
			y -= 1;
		}

		QString dirstr;
		if(dir == 1)
			dirstr = "horizontal";
		else
			dirstr = "vertical";

		t << "player" << ((*it).side + 1) << " move " << x << " " << y << " " << dirstr;
		t << endl;
	}

	f.close();

	return true;
}

bool VDots::load(QString filename)
{
	QString s;
	QStringList sl;
	int w = -1;
	int h = -1;
	QString game;
	QString playeraction;
	QString playername;
	QString dirstr;
	int x, y;
	int side, dir;
	bool setup = false;

	m_moves.clear();

	QFile f(filename);
	if(!f.open(QIODevice::ReadOnly))
		return false;

	QTextStream t(&f);

	while(!t.atEnd())
	{
		t >> s;
		if(s.isEmpty())
			break;

		if((s == "player1") || (s == "player2"))
		{
			t >> playeraction;
			if(playeraction == "join")
			{
				t >> playername;
			}
			else if(playeraction == "leave")
			{
				t >> playername;
			}
			else if((playeraction == "move") && (setup))
			{
				t >> x;
				t >> y;
				t >> dirstr;

				if(dirstr == "vertical")
					dir = 3;
				else if(dirstr == "horizontal")
					dir = 1;
				else
					return false;

				if(s == "player1")
					side = 0;
				else
					side = 1;

				Move m;
				m.x = x;
				m.y = y;
				m.side = side;
				m.direction = dir;
				m_moves.append(m);
			}
			else if((playeraction == "winner") && (setup))
			{
			}
			else
			{
				return false;
			}
		}
		else if(s == "width")
		{
			t >> w;
		}
		else if(s == "height")
		{
			t >> h;
		}
		else if((s == "tie") && (setup))
		{
			t >> game;
			if(game != "game")
				return false;
		}
		else
		{
			return false;
		}

		if((w != -1) && (h != -1) && (!setup))
		{
			resizeBoard(w, h);
			setup = true;
		}
	}

	f.close();

	return true;
}

int VDots::positions()
{
	return m_moves.count();
}

void VDots::step(int position)
{
	QList<Move>::Iterator it;
	int i;

	for(it = m_moves.begin(); it != m_moves.end(); it++)
	{
		setBorderValue((*it).x, (*it).y, (*it).direction, -1, Dots::replay);
	}

	i = 0;
	for(it = m_moves.begin(); it != m_moves.end(); it++)
	{
		if(i == position) return;
		setBorderValue((*it).x, (*it).y, (*it).direction, (*it).side, Dots::replay);
		i++;
	}
}
