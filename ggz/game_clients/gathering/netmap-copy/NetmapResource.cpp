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

#include <NetmapResource>

NetmapResource::NetmapResource()
{
	DEBUG("NetmapResource: created\n");
}

NetmapResource::~NetmapResource()
{
	DEBUG("NetmapResource: deleted.\n");
}

Ntile *NetmapResource::readTileAbsolute(Ncoord x, Ncoord y)
{
	// virtual
}

Ntile *NetmapResource::readTileRelative(Ncoord vx, Ncoord vy, Ncoord x, Ncoord y)
{
	// virtual
}

Nvtile *NetmapResource::readVtileAbsolute(Ncoord vx, Ncoord vy)
{
	// virtual
}

Ncoord NetmapResource::mapWidth()
{
	return m_map_width;
}

Ncoord NetmapResource::mapHeight()
{
	return m_map_height;
}
