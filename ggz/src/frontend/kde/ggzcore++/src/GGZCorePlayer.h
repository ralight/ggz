/////////////////////////////////////////////////////////////////////
//                                                                 ///
// GGZCore++ - C++ API wrapper for the ggzcore library              ///
// Copyright (C) 2001, 2002 Josef Spillner                           ///
// dr_maux@users.sourceforge.net                                      ///
// The MindX Open Source Project                                      ///
// http://mindx.sourceforge.net/                                     ///
//                                                                  ///
// This library may be distributed under the terms of the GNU GPL. ///
//                                                                ///
////////////////////////////////////////////////////////////////////

///// GGZ Player /////////////////////////////////////////////////

#ifndef GGZPLAYER_H
#define GGZPLAYER_H

#include <ggzcore.h>

/**
* Player representation.
*/

class GGZCorePlayer
{
	public:
		/** Constructor */
		GGZCorePlayer();
		/** Destructor */
		~GGZCorePlayer();

		/** Return the name of this player. */
		char* name();
		/** Return the player's current table. */
		GGZTable* table();

		/** Lag measurement */
		int lag();

	private:
		GGZPlayer *m_player;
};

#endif
