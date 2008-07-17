/////////////////////////////////////////////////////////////////////
//
// Dots: C++ classes for Connect the Dots games
// http://www.ggzgamingzone.org/gameclients/kdots/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

#ifndef VDOTS_H
#define VDOTS_H

#include "dots.h"

class VDots : public Dots
{
	public:
		VDots();
		~VDots();

		bool save(QString filename);
		bool load(QString filename);
		int positions();
		void step(int position);
	protected:
		void vResizeBoard(int width, int height);
		int vSetBorderValue(int x, int y, int side);

		int m_lastx, m_lasty, m_lastdirection;
	private:
		int absi(int i);

		int m_width, m_height;
};

#endif

