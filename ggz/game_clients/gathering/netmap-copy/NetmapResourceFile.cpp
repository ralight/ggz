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

#include <NetmapResourceFile>

/** Constructor */
NetmapResourceFile::NetmapResourceFile()
: NetmapResource()
{
}

/** Destructor */
NetmapResourceFile::~NetmapResourceFile()
{
}

/** Open a map file locally. Such files do only contain raw data. */
void NetmapResourceFile::openFile(char *filename)
{
	// virtual
}

/** Close an open file. */
void NetmapResourceFile::closeFile()
{
	// virtual
}

/** Specify the x and y coordinates to read one tile. */
Ntile *NetmapResourceFile::readTileAbsolute(Ncoord x, Ncoord y)
{
	// virtual
}

/** Read a tile relative to the specified Vtile. */
Ntile *NetmapResourceFile::readTileRelative(Ncoord vx, Ncoord vy, Ncoord x, Ncoord y)
{
	// virtual
}

/** Specify the vx and vy coordinates to read one vtile. */
Nvtile *NetmapResourceFile::readVtileAbsolute(Ncoord vx, Ncoord vy)
{
	// virtual
}
