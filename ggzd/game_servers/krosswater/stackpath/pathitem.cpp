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

// Stackpath includes
#include "pathitem.h"

// System includes
#include <unistd.h>

Pathitem::Pathitem(int width, int height, int** field)
{
	// Cleanly initializing object
	m_field = field;
	m_width = width;
	m_height = height;
	m_status = queued;
	m_itemlist.clear();
	m_itemlist.insert(m_itemlist.end(), this);
	m_x = -1;
	m_y = -1;
	m_parent = NULL;
}

Pathitem::~Pathitem()
{
	// Go cleanup
	clear();
}

void Pathitem::process(int fromx, int fromy, int tox, int toy, int mode)
{
	Pathitem* ancestor;
	list<Pathitem*>::iterator it;
	int inside;

	// Am I the parent?
	if((m_x == -1) && (m_y == -1))
	{
		m_x = fromx;
		m_y = fromy;
	}

	// Check for goal
	m_status = processed;
	if((fromx == tox) && (fromy == toy))
	{
		//cout << "FOUND PATH!" << endl;
		m_status = final;
		if(!(mode & nostop)) return;
	}

	// Find possible childs
	for(int j = fromy - 1; j <= fromy + 1; j++)
		for(int i = fromx - 1; i <= fromx + 1; i++)
		{
			// Do not process if field is outside of borders
			if((i < 0) || (j < 0) || (i >= m_width) || (j >= m_width)) continue;
			// Do not let item process itself (this check is just for performance reasons)
			if((i == fromx) && (j == fromy)) continue;
			// Do only process fields with content
			if(!m_field[i][j]) continue;
			// Extended mode?
			if(mode & normal)
			{
				if(((j == fromy - 1) && (i == fromx - 1))
				|| ((j == fromy + 1) && (i == fromx - 1))
				|| ((j == fromy - 1) && (i == fromx + 1))
				|| ((j == fromy + 1) && (i == fromx + 1))) continue;
			}

			// Refuse to process fields which are already processed
			inside = 0;
			for(it = m_itemlist.begin(); it != m_itemlist.end(); it++)
				if(((*it)->m_x == i) && ((*it)->m_y == j)) inside = 1;
			if(inside) continue;

			// Set up new item
			ancestor = new Pathitem(m_width, m_height, m_field);
			ancestor->setParent(this);
			ancestor->setOrigin(i, j);
			ancestor->setTarget(tox, toy);

			// Mark item as preprocessed
			m_itemlist.insert(m_itemlist.end(), ancestor);
		}

	// Process queued childs
	for(it = m_itemlist.begin(); it != m_itemlist.end(); it++)
		if((*it)->m_status == queued)
		{
			(*it)->setItemList(m_itemlist);
			(*it)->process((*it)->m_x, (*it)->m_y, tox, toy, mode);
			setItemList((*it)->itemlist());
		}
}

void Pathitem::setOrigin(int x, int y)
{
	m_x = x;
	m_y = y;
}

void Pathitem::setTarget(int x, int y)
{
	m_tox = x;
	m_tox = y;
}

void Pathitem::setItemList(list<Pathitem*> list)
{
	m_itemlist = list;
}

list<Pathitem*> Pathitem::itemlist()
{
	return m_itemlist;
}

int Pathitem::x()
{
	return m_x;
}

int Pathitem::y()
{
	return m_y;
}

int Pathitem::status()
{
	return m_status;
}

void Pathitem::setParent(Pathitem* parent)
{
	m_parent = parent;
}

Pathitem* Pathitem::parent()
{
	return m_parent;
}

void Pathitem::clear()
{
	list<Pathitem*>::iterator it;

	// Clean the list
	for(it = m_itemlist.begin(); it != m_itemlist.end(); it++)
		if((*it) != this)
		{
			(*it)->m_itemlist.clear();
			delete (*it);
		}
	m_itemlist.clear();
}

void Pathitem::setStatus(int status)
{
	m_status = status;
}


