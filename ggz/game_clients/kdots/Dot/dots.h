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

#ifndef DOTS_H
#define DOTS_H

typedef int* dot_t;

class Dots
{
	public:
		Dots();
		~Dots();
		void resizeBoard(int cols, int rows);
		int setBorderValue(int x, int y, int direction, int side);
		int count(int side);
		enum Directions
		{
			left = 0,
			right = 1,
			up = 2,
			down = 3
		};
	protected:
		int borders(int x, int y, int side);
		int border(int x, int y, int direction);

		int m_cols, m_rows;
	private:
		void cleanup();
		void assignBorder(int x, int y, int direction, int side);

		dot_t **m_field;
};

#endif
