/////////////////////////////////////////////////////////////////////
//                                                                 ///
// GGZCore++ - C++ API wrapper for the ggzcore library              ///
// (C) 2001 Josef Spillner                                           ///
// dr_maux@users.sourceforge.net                                      ///
// The MindX Open Source Project                                      ///
// http://mindx.sourceforge.net/                                     ///
//                                                                  ///
// This library may be distributed under the terms of the GNU GPL. ///
//                                                                ///
////////////////////////////////////////////////////////////////////

///// GGZ Game ///////////////////////////////////////////////////

#ifndef GGZGAME_H
#define GGZGAME_H

#include <ggzcore.h>

/**
*
* Every game is represented by an object of this type.
*
*/

class GGZCoreGame
{
	public:
		enum GGZCoreGameEvent
		{
			launched,
			launchfail,
			negotiated,
			negotiatefail,
			data,
			over,
			ioerror,
			protoerror
		};

		/** Constructor */
		GGZCoreGame();
		/** Destructor */
		~GGZCoreGame();

		/** Add a simple callback to the game. */
		int addHook(const GGZCoreGameEvent event, const GGZHookFunc func);
		/** Add a callback with arguments. */
		int addHook(const GGZCoreGameEvent event, const GGZHookFunc func, void* data);

		/** Remove a callback from the game. */
		int removeHook(const GGZCoreGameEvent event, const GGZHookFunc func);
		/** Overloaded: Remove a callback on its id. */
		int removeHook(const GGZCoreGameEvent event, const unsigned int id);

		/** Initialize this game. */
		int init(GGZModule* module);

		/** Return the specific file descriptor. */
		int fd();
		/** Return the associated module. */
		GGZModule* module();

		/** Launch this game. */
		int launch();
		/** Join this game. */
		int join();

		/** Send other data. */
		int dataSend(char* buffer);

		/** Control data flow. */
		int dataPending();
		/** Returns whether data is read. */
		int dataRead();
		/** Return whether data is written. */
		int dataWrite();

	private:
		GGZGame* m_game;
};

#endif
