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

#ifndef DOTS_H
#define DOTS_H

#include <qvaluelist.h>

struct move_t
{
	int x;
	int y;
	int side;
	int direction;
};
typedef struct move_t Move;

typedef int* dot_t;

class Dots
{
	public:
		Dots();
		~Dots();
		void resizeBoard(int cols, int rows);
		int setBorderValue(int x, int y, int direction, int side, int action);
		int count(int side);
		void setOwnership(int x, int y, int owner);
		enum Directions
		{
			left = 0,
			right = 1,
			up = 2,
			down = 3,
			fieldrightbelow = 4
		};
		enum Actions
		{
			check = 0,
			move = 1,
			replay = 2
		};
	protected:
		int borders(int x, int y, int side);
		int border(int x, int y, int direction);
		int content(int x, int y);
		void cleanup();

		int m_cols, m_rows;
		QValueList<Move> m_moves;
	private:
		void assignBorder(int x, int y, int direction, int side);

		dot_t **m_field;
};

#endif

