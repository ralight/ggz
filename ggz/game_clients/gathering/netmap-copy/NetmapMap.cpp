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

#include <NetmapMap>

/** Constructor */
NetmapMap::NetmapMap()
{
	DEBUG("NetmapMap: created\n");
	m_player_x = -1;
	m_player_y = -1;
	m_horizon_width = 2;
	m_horizon_height = 2;
	m_map_cache = 4096;
	m_resource = NULL;
	m_vtile_width = 16;
	m_vtile_height = 16;
	m_resource = NULL;
}

/** Destructor */
NetmapMap::~NetmapMap()
{
	DEBUG("NetmapMap: deleted\n");
	vtilelist.clear();
}

/** Specify how large a vtile should be. The default is 16 x 16. */
void NetmapMap::setVtileSize(Ncoord width, Ncoord height)
{
	m_vtile_width = width;
	m_vtile_height = height;
}

/** Set the local vtile buffer limit. The default is 4096, that is 1 MB of memory. */
void NetmapMap::setVtileCacheSize(Ncoord number)
{
	m_map_cache = number;
}

/** Receive a vtile from the resources. */
Nvtile *NetmapMap::vtile(Ncoord vx, Ncoord vy)
{
	Nvtile *vtile;

	DEBUG("NetmapMap: read vtile at %qi/%qi\n", vx, vy);

	if(!m_resource) return NULL;

	vtile = m_resource->readVtileAbsolute(vx, vy);

	DEBUG("Inserting vtile %qi/%qi\n", vx, vy);
	vtile->setPosition(vx, vy);
	vtilelist.push_back(*vtile);

	return vtile;
}

/** Receive a single tile from the resources. */
Ntile *NetmapMap::tile(Ncoord x, Ncoord y)
{
	list<Nvtile>::iterator it;
	Nvtile *vtile;

	DEBUG("NetmapMap: read tile at %qi/%qi\n", x, y);

	if((x < 0) || (y < 0)) return NULL;

	//DEBUG("NetmapMap: read tile at %qi/%qi because it is >= 0!!!\n", x, y);

	if((!mapWidth()) || (!mapHeight())) return NULL;

	//DEBUG("NetmapMap: read tile at %qi/%qi because resource is set!\n", x, y);

	for(it = vtilelist.begin(); it != vtilelist.end(); it++)
	{
		int tmpx = (int)(x % m_vtile_width);
		int tmpy = (int)(y % m_vtile_height);
		//cout << " * debug: " << tmpx << ", " << tmpy << endl;
		DEBUG("* debug: %i, %i\n", tmpx, tmpy);
		cout << " = " << (int)(it->tile(x % m_vtile_width, y % m_vtile_height)) << endl;
		if((it->x() == x / m_vtile_width) && (it->y() == y / m_vtile_height))
			return it->tile(x % m_vtile_width, y % m_vtile_height);
	}

	// Fallback: Fetch a vtile from the resources
	DEBUG("Fallback!\n");
	DEBUG("x / m_vtile_width, y / m_vtile_height: %qi, %qi\n", x / m_vtile_width, y / m_vtile_height);
	vtile = this->vtile(x / m_vtile_width, y / m_vtile_height);
	DEBUG("return vtile->tile(x %% m_vtile_width, y %% m_vtile_height);: %qi, %qi\n", x % m_vtile_width, y % m_vtile_height);
	if(vtile)
		return vtile->tile(x % m_vtile_width, y % m_vtile_height);

	return NULL;
}

/** Automatically retrieve all missing data. */
void NetmapMap::update()
{
	// Get data around position
	if(m_player_x != -1)
		vtile(m_player_x / m_vtile_width, m_player_y / m_vtile_height);

	// Compressing vtile cache
	for(Ncoord i = 0; i < vtilelist.size() - m_map_cache; i++)
		vtilelist.pop_front();
}

/** Set the player's position. */
void NetmapMap::setPosition(Ncoord x, Ncoord y)
{
	if(x < 0) x = 0;
	if(y < 0) y = 0;

	m_player_x = x;
	m_player_y = y;
}

/** Specify how far a player can see, both horizontally and vertically. The default is 3 x 3, specified by 1 x 1. */
void NetmapMap::setHorizon(Ncoord width, Ncoord height)
{
	m_horizon_width = width;
	m_horizon_height = height;
}

/** Return the width of the map. */
Ncoord NetmapMap::mapWidth()
{
	if(!m_resource) return 0;

	return m_resource->mapWidth();
}

/** Return the height of the map. */
Ncoord NetmapMap::mapHeight()
{
	if(!m_resource) return 0;

	return m_resource->mapHeight();
}

/** Set the resource object. */
void NetmapMap::setResource(NetmapResource *resource)
{
	DEBUG("NetmapMap: got resource at %qi\n", resource);
	m_resource = resource;
}

/** Get vertical position. */
Ncoord NetmapMap::x()
{
	return m_player_x;
}

/** Get vertical position. */
Ncoord NetmapMap::y()
{
	return m_player_y;
}
