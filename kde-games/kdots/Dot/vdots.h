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

#ifndef VDOTS_H
#define VDOTS_H

#include "dots.h"

class VDots : public Dots
{
	public:
		VDots();
		~VDots();
	protected:
		void vResizeBoard(int width, int height);
		int vSetBorderValue(int x, int y, int side);

		int m_lastx, m_lasty, m_lastdirection;
	private:
		int absi(int i);

		int m_width, m_height;
};

#endif
