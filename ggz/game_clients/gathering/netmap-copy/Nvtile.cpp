////////////////////////////////////////////////////////////////////////////
//                                                                        //
// The Netmap C++ Class Library                                           //
//                                                                        //
// Copyright (C) 2001 Josef Spillner                                      //
// dr_maux@users.sourceforge.net                                          //
// The MindX Open Source Project                                          //
// http://mindx.sourceforge.net/development/netmap                        //
//                                                                        //
// Published under GNU GPL conditions                                     //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#include <Nvtile>

#include <iostream>

Nvtile::Nvtile()
{
	m_vtile_width = 0;
	m_vtile_height = 0;
	m_vtile_x = -1;
	m_vtile_y = -1;
}

Nvtile::~Nvtile()
{
	tilevec.clear();
}

void Nvtile::setSize(Ncoord width, Ncoord height)
{
	tilevec.clear();
	m_vtile_width = width;
	m_vtile_height = height;

	for(Ncoord j = 0; j < height; j++)
		for(Ncoord i = 0; i < width; i++)
			tilevec.insert(tilevec.end(), 0);
}

Ncoord Nvtile::width()
{
	return m_vtile_width;
}

Ncoord Nvtile::height()
{
	return m_vtile_height;
}

Ncoord Nvtile::x()
{
	return m_vtile_x;
}

Ncoord Nvtile::y()
{
	return m_vtile_y;
}

void Nvtile::setTile(Ncoord x, Ncoord y, Ntile tile)
{
	vector<Ntile>::iterator it;
int foo;
foo = 0;

	it = tilevec.begin();
	for(Ncoord i = 0; i < y * m_vtile_width + x; i++)
{
foo++;
		it++;
}
cout << "foo is: " << foo << "(" << (int)tile << ")" << endl;

	tilevec.erase(it);
	tilevec.insert(it, tile);
}

Ntile *Nvtile::tile(Ncoord x, Ncoord y)
{
int foo;
foo = 0;
	vector<Ntile>::iterator it;

	if((x < 0) || (y < 0)) return NULL;

	it = tilevec.begin();
	for(Ncoord i = 0; i < y * m_vtile_width + x; i++)
{
foo++;
//printf("Ignore %i\n", (*it & 255));
		it++;
}
cout << "foo is: " << foo << " and has " << (int)(*it & 255) << endl;
	return it;
}

void Nvtile::setPosition(Ncoord vx, Ncoord vy)
{
	m_vtile_x = vx;
	m_vtile_y = vy;
}
