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

#include "vdots.h"

#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>

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

void VDots::save(QString filename)
{
	QValueList<Move>::Iterator it;

	QFile f(filename);
	if(!f.open(IO_WriteOnly)) return;

	QTextStream t(&f);
	t << "format" << endl;
	t << 1 << endl;
	t << "boardsize" << endl;
	t << m_cols - 1 << endl;
	t << m_rows - 1 << endl;
	for(it = m_moves.begin(); it != m_moves.end(); it++)
	{
		t << (*it).x << "," << (*it).y << "," << (*it).side << "," << (*it).direction;
		t << endl;
	}

	f.close();
}

void VDots::load(QString filename)
{
	QString s;
	QStringList sl;
	int format, w, h;

	m_moves.clear();

	QFile f(filename);
	if(!f.open(IO_ReadOnly)) return;

	QTextStream t(&f);
	t >> s;
	if(s != "format") return;
	t >> format;
	if(format != 1) return;
	t >> s;
	if(s != "boardsize") return;
	t >> w;
	t >> h;

	resizeBoard(w, h);

	while(!t.eof())
	{
		Move m;
		t >> s;
		sl = sl.split(",", s);
		if(sl.count() != 4) return;
		s = *(sl.at(0));
		m.x = s.toInt();
		s = *(sl.at(1));
		m.y = s.toInt();
		s = *(sl.at(2));
		m.side = s.toInt();
		s = *(sl.at(3));
		m.direction = s.toInt();
		m_moves.append(m);
	}

	f.close();
}

int VDots::positions()
{
	return m_moves.count();
}

void VDots::step(int position)
{
	QValueList<Move>::Iterator it;
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
