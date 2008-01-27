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

#include <stdlib.h>

// Stackpath includes
#include "cwpathitem.h"

CWPathitem::CWPathitem(int width, int height, int** field)
: Pathitem(width, height, field)
{
}

CWPathitem::~CWPathitem()
{
}

int CWPathitem::available(int column, int mode)
{
	if((column < 0) || (column >= m_width)) return -1;

	// Select a random field
	for(int j = 0; j < m_height; j++)
		if((m_field[column][j] == 1) && (!(rand() % 4))) return j;
	for(int j = 0; j < m_height; j++)
		if((m_field[column][j] == 1) && (!(rand() % 4))) return j;

	// Set up field if none found
	if(mode == force)
	{
		for(int j = 0; j < m_height; j++)
			if(!m_field[column][j])
			{
				m_field[column][j] = 1;
				return j;
			}
	}

	return -1;
}

Pathitem* CWPathitem::result()
{
	list<Pathitem*> pathlist;
	list<Pathitem*>::iterator it;

	// Go threw the list of path items and search for a 'final' one
	pathlist = itemlist();
	for(it = pathlist.begin(); it != pathlist.end(); it++)
		if((*it)->status() == CWPathitem::final) return *it;

	return NULL;
}

void CWPathitem::setValue(int x, int y, int value)
{
	if((x < 0) || (x >= m_width) || (y < 0) || (y >= m_width)) return;

	m_field[x][y] = value;
}

int CWPathitem::value(int x, int y)
{
	if((x < 0) || (x >= m_width) || (y < 0) || (y >= m_width)) return -1;

	return m_field[x][y];
}

void CWPathitem::postprocess(int column)
{
	list<Pathitem*> pathlist;
	list<Pathitem*>::iterator it;

	// Update all fields in the specified column to 'final'
	pathlist = itemlist();
	for(it = pathlist.begin(); it != pathlist.end(); it++)
		if(((*it)->x() == column) && (m_field[(*it)->x()][(*it)->y()])) (*it)->setStatus(final);
}

