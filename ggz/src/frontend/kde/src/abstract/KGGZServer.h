#ifndef KGGZ_SERVER_H
#define KGGZ_SERVER_H

#include <GGZCoreServer.h>

// This _is_ crap !!!!!
#define GGZCoreServerEvent unsigned int

class KGGZServer : public GGZCoreServer
{
	public:
		KGGZServer();
		~KGGZServer();
		int countRoomsForce();
		static KGGZServer* instance();

	protected:
		static GGZHookReturn hookConnected(GGZCoreServerEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookConnectFail(GGZCoreServerEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookNegotiated(GGZCoreServerEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookNegotiateFail(GGZCoreServerEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookLoggedin(GGZCoreServerEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookLoginFail(GGZCoreServerEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookMotdLoaded(GGZCoreServerEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookRoomList(GGZCoreServerEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookTypeList(GGZCoreServerEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookEntered(GGZCoreServerEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookEnterFail(GGZCoreServerEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookLoggedout(GGZCoreServerEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookNetError(GGZCoreServerEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookProtoError(GGZCoreServerEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookChatFail(GGZCoreServerEvent id, void *event_data, void *user_data);
		static GGZHookReturn hookStateChange(GGZCoreServerEvent id, void *event_data, void *user_data);

	private:
		static KGGZServer* m_instance;
};

#endif
