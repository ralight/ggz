/////////////////////////////////////////////
//                                         //
// Stackpath 0.1                           //
//                                         //
// Copyright (C) 2001, 2002 Josef Spillner //
// dr_maux@users.sourceforge.net           //
// The MindX Open Source Project           //
// http://mindx.sourceforge.net            //
//                                         //
// Published under GNU GPL conditions.     //
//                                         //
/////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#  include <config.h>			/* Site-specific config */
#endif

// CWPathitem include
#include "cwpathitem.h"

// System includes
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>

// Gives a console output of the map
void printpath(int width, int height, int **field)
{
	for(int j = 0; j < height; j++)
	{
		for(int i = 0; i < width; i++)
		{
			if(field[i][j]) cout << "\e[1m\e[39m";
			else cout << "\e[0m\e[39m";
			cout << field[i][j];
		}
		cout << endl;
	}
	cout << "\e[0m\e[39m";
}

// The main function: Build a map and find a way
int main(int argc, char **argv)
{
	CWPathitem *path;
	int **field;
	int yl, yr;
	Pathitem* backtrace;

	cout << "Creating field..." << endl;
	field = (int**)malloc(10 * sizeof(int));
	for(int i = 0; i < 10; i++)
		field[i] = (int*)malloc(10 * sizeof(int));

	cout << "Filling field..." << endl;
	srandom(time(NULL));
	for(int j = 0; j < 10; j++)
		for(int i = 0; i < 10; i++)
			field[i][j] = rand() % 3;

	cout << "Creating pathitem" << endl;
	path = new CWPathitem(10, 10, (int**)field);

	cout << "Checking availability..." << endl;
	yl = path->available(0, CWPathitem::noforce);
	yr = path->available(9, CWPathitem::noforce);

	cout << "yl: " << yl << endl;
	cout << "yr: " << yr << endl;
	if((yl < 0) || (yr < 0))
	{
		cout << "No space available!" << endl;
		exit(-1);
	}

	cout << "Printing path" << endl;
	printpath(10, 10, field);

	cout << "Processing..." << endl;
	path->process(0, yl, 9, yr, Pathitem::extended);

	cout << "Evaluating..." << endl;
	backtrace = path->result();
	if(backtrace)
	{
		cout << "## Found the path (" << backtrace->x() << ", " << backtrace->y() << ") !!! Doing backtrace..." << endl;
		do
		{
			cout << backtrace->x() << "," << backtrace->y() << endl;
			backtrace = backtrace->parent();
		}
		while(backtrace->parent() != NULL);
	}
	else cout << "## Ooops, no way found :(" << endl;

	cout << "Cleanup" << endl;
	delete path;

	cout << "Done" << endl;

	return 0;
}

