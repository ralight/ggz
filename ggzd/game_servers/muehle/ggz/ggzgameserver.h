#ifndef GGZ_GAMESERVER_H
#define GGZ_GAMESERVER_H

#include <ggzdmod.h>

class GGZGameServer {
	public:
		GGZGameServer ();
		virtual ~GGZGameServer ();
		void connect ();

	protected:
		virtual void stateEvent ();
		virtual void joinEvent ();
		virtual void leaveEvent ();
		virtual void dataEvent ();
		virtual void errorEvent ();

	private:
		static void handle_state ( GGZdMod* ggzdmod, GGZdModEvent event, void* data );
		static void handle_join  ( GGZdMod* ggzdmod, GGZdModEvent event, void* data );
		static void handle_leave ( GGZdMod* ggzdmod, GGZdModEvent event, void* data );
		static void handle_data  ( GGZdMod* ggzdmod, GGZdModEvent event, void* data );
		static void handle_error ( GGZdMod* ggzdmod, GGZdModEvent event, void* data );
};

#endif

