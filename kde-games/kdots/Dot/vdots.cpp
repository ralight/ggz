///////////////////////////////////////////////////////////////
//
// Dots
// C++ Class Set for Connect the Dots games
// Copyright (C) 2001 Josef Spillner
// dr_maux@users.sourceforge.net
// The MindX Open Source Project
// http://mindx.sourceforge.net/games/kdots/
//
///////////////////////////////////////////////////////////////

#include "vdots.h"

#include <iostream>

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

	cout << "Tile size: " << nx << "x" << ny << " pixels." << endl;
	cout << "Tiles: " << m_cols - 1 << ", " << m_rows - 1 << endl;
	cout << "Size: " << m_width << ", " << m_height << endl;

	dx = (x + nx / 2) / nx;
	dy = (y + ny / 2) / ny;

	cout << "* dx=" << dx << ", dy=" << dy << endl;

	rh = nx * dx - x;
	rv = ny * dy - y;

	cout << "XH=" << rh << ", YH=" << rv << endl;

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

	cout << "Direction: " << direction << endl;

switch(direction)
{
	case up:
		cout << "up" << endl;
		break;
	case down:
		cout << "down" << endl;
		break;
	case left:
		cout << "left" << endl;
		break;
	case right:
		cout << "right" << endl;
		break;
}

	m_lastx = dx;
	m_lasty = dy;
	m_lastdirection = direction;
	return setBorderValue(dx, dy, direction, side);
}
