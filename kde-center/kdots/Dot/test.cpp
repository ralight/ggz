/////////////////////////////////////////////////////////////////////
//
// Dots: C++ classes for Connect the Dots games
// http://www.ggzgamingzone.org/gameclients/kdots/
//
// Copyright (C) 2001 - 2008 Josef Spillner <josef@ggzgamingzone.org>
// Published under the conditions of the GNU GPL, see COPYING
//
/////////////////////////////////////////////////////////////////////

#include "dots.h"

#include <iostream>

using namespace std;

Dots *dots;

int main(int argc, char **argv)
{
	int val;

	cout << "Creating dots" << endl;
	dots = new Dots();

	cout << "Resizing" << endl;
	dots->resizeBoard(10, 10);

	cout << "Influencing" << endl;
	dots->setBorderValue(0, 0, Dots::right, 1, Dots::move);
	dots->setBorderValue(0, 0, Dots::down, 1, Dots::move);
	dots->setBorderValue(1, 1, Dots::up, 1, Dots::move);
	dots->setBorderValue(0, 1, Dots::right, 1, Dots::move);

	cout << "Counting fields" << endl;
	val = dots->count(1);
	cout << val << endl;

	cout << "Destroying dots" << endl;
	delete dots;

	cout << "Ready" << endl;
}

