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

#include <NetmapResourceFileText>
#include <StringExt>
#include <stdlib.h>

NetmapResourceFileText::NetmapResourceFileText()
: NetmapResourceFile()
{
	m_map_width = 1024;
	m_map_height = 1024;
	m_vtile_height = 16;
	m_vtile_width = 16;
}

NetmapResourceFileText::~NetmapResourceFileText()
{
}

void NetmapResourceFileText::openFile(char *filename)
{
	m_stream.open(filename, _IO_INPUT);
	m_filename = filename;
}

void NetmapResourceFileText::closeFile()
{
	if(!m_stream.is_open()) return;

	m_stream.close();
}

Ntile *NetmapResourceFileText::readTileAbsolute(Ncoord x, Ncoord y)
{
	Ncoord filepos;
	Ntile tilebuf;

	if(!m_stream.is_open()) return NULL;

	filepos = y * m_map_width + x;
	DEBUG("NetworkResource: read absolute %qi\n", filepos);
	//m_stream.seekoff(filepos, ios::beg);
	//tilebuf = m_stream.sgetc();

	tile = &tilebuf;
	return tile;
}

Ntile *NetmapResourceFileText::readTileRelative(Ncoord vx, Ncoord vy, Ncoord x, Ncoord y)
{
	Ntile tilebuf;

	if(!m_stream.is_open()) return NULL;

	//m_stream.seekoff(vy * m_vtile_height * m_map_width + vx * m_vtile_width + y * m_map_width + x, ios::beg);
	//tilebuf = m_stream.sgetc();

	tile = &tilebuf;
	return tile;
}

Nvtile *NetmapResourceFileText::readVtileAbsolute(Ncoord vx, Ncoord vy)
{
	list<string> csv;
	list<string>::iterator it;
	StringExt mystring;
	string text;
	Ncoord i;
	int fd;

	if(!m_stream.is_open()) return NULL;

	vtile = new Nvtile();
	vtile->setSize(16, 16);

	//m_stream.seekoff(0, ios::beg);
	//m_stream.seekp(0);
	// quick hack - who cares?
	m_stream.close();
	m_stream.open(m_filename.c_str(), _IO_INPUT);

	for(Ncoord j = 0; j < vy; j++)
		m_stream >> text;

	for(Ncoord j = 0; j < 16; j++)
	{
		mystring.erase();
		m_stream >> text;
		mystring.append(text);
		csv = mystring.explode(",");

		i = 0;
		it = csv.begin();
		for(; it != csv.end(); it++)
		{
			//cout << it->c_str() << flush;
			if((i >= vx) && (i < vx + 16))
				vtile->setTile(i, j, atoi(it->c_str()));
			i++;
		}

		csv.clear();
	}

	return vtile;
}
