#ifndef MUEHLE_SERVER_H
#define MUEHLE_SERVER_H

#include "ggzgameserver.h"

class MuehleServer : public GGZGameServer {
	public:
		MuehleServer ();
		~MuehleServer ();
		void stateEvent ();
		void joinEvent ();
		void leaveEvent ();
		void dataEvent ();
		void errorEvent ();
};

#endif

