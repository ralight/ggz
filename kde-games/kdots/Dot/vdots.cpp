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
	return setBorderValue(dx, dy, direction, side, 0);
}

