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

#include <NetmapResourceFileBinary>

NetmapResourceFileBinary::NetmapResourceFileBinary()
: NetmapResourceFile()
{
	m_map_width = 1024;
	m_map_height = 1024;
	m_vtile_height = 16;
	m_vtile_width = 16;
}

NetmapResourceFileBinary::~NetmapResourceFileBinary()
{
}

void NetmapResourceFileBinary::openFile(char *filename)
{
	m_stream.open(filename, _IO_INPUT);
}

void NetmapResourceFileBinary::closeFile()
{
	if(!m_stream.is_open()) return;

	m_stream.close();
}

Ntile *NetmapResourceFileBinary::readTileAbsolute(Ncoord x, Ncoord y)
{
	Ncoord filepos;
	Ntile tilebuf;

	if(!m_stream.is_open()) return NULL;

	filepos = y * m_map_width + x;
	DEBUG("NetworkResource: read absolute %qi\n", filepos);
	m_stream.seekoff(filepos, ios::beg);
	//m_stream.xsgetn((char)&tile, sizeof(tile));
	tilebuf = m_stream.sgetc();

	tile = &tilebuf;
	return tile;
}

Ntile *NetmapResourceFileBinary::readTileRelative(Ncoord vx, Ncoord vy, Ncoord x, Ncoord y)
{
	Ntile tilebuf;

	if(!m_stream.is_open()) return NULL;

	m_stream.seekoff(vy * m_vtile_height * m_map_width + vx * m_vtile_width + y * m_map_width + x, ios::beg);
	//m_stream.xsgetn((char)&tile, sizeof(tile));
	tilebuf = m_stream.sgetc();

	tile = &tilebuf;
	return tile;
}

Nvtile *NetmapResourceFileBinary::readVtileAbsolute(Ncoord vx, Ncoord vy)
{
	Ncoord filepos;

	if(!m_stream.is_open()) return NULL;

	DEBUG("NetmapResource: read vtile at %qi/%qi\n", vx, vy);

	vtile = new Nvtile();
	vtile->setSize(16, 16);
	for(Ncoord j = 0; j < 16; j++)
	{
		filepos = vy * m_vtile_height * m_map_width + vx * m_vtile_width + j * m_map_width;
		m_stream.seekoff(filepos, ios::beg);
		for(Ncoord i = 0; i < 16; i++)
		{
			DEBUG("NetworkResource: read absolute %qi\n", filepos + i);
			vtile->setTile(i, j, m_stream.sgetc());
		}
	}

	return vtile;
}
