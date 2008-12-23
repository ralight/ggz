#include <gloox/client.h>
#include <gloox/tag.h>
#include <gloox/connectionbase.h>
#include <gloox/connectionlistener.h>
#include <gloox/presencehandler.h>
#include <gloox/messagehandler.h>
#include <gloox/stanzaextension.h>

#include <iostream>

#include <pthread.h>

#include <ggz.h>
#include <ggzcore.h>

class Roomlist;
static Roomlist *roomlistobj = NULL;

class Roomlist
{
	public:
		Roomlist()
		{
			std::cout << "[mutex] init" << std::endl;
			pthread_mutex_init(&roomlist_lock, NULL);
		}

		~Roomlist()
		{
			std::cout << "[mutex] deinit" << std::endl;
			pthread_mutex_destroy(&roomlist_lock);
			roomlistobj = NULL;
		}

		static Roomlist *self()
		{
			std::cout << "[mutex] self()" << std::endl;
			if(!roomlistobj)
				roomlistobj = new Roomlist();
			return roomlistobj;
		}

		void fill(GGZServer *server)
		{
			std::cout << "[mutex] use" << std::endl;
			pthread_mutex_lock(&roomlist_lock);
			int num = ggzcore_server_get_num_rooms(server);
			for(int i = 0; i < num; i++)
			{
				GGZRoom *room = ggzcore_server_get_nth_room(server, i);
				std::cout << "* room: " << ggzcore_room_get_name(room) << std::endl;
				roomlist.push_back(ggzcore_room_get_name(room));
			}
			pthread_mutex_unlock(&roomlist_lock);
		}

		int findpos(std::string roomname)
		{
			std::cout << "[mutex] use" << std::endl;
			pthread_mutex_lock(&roomlist_lock);
			int num = -1;
			std::list<std::string>::const_iterator it;
			for(it = roomlist.begin(); it != roomlist.end(); it++)
			{
				std::string room = (*it);
				num++;
				if(roomname == room)
				{
					break;
				}
			}
			pthread_mutex_unlock(&roomlist_lock);
			return num;
		}

		bool contains(std::string roomname)
		{
			std::cout << "[mutex] use" << std::endl;
			bool found = false;
			std::list<std::string>::const_iterator it;
			pthread_mutex_lock(&roomlist_lock);
			for(it = roomlist.begin(); it != roomlist.end(); it++)
			{
				std::string room = (*it);
				if(roomname == room)
				{
					found = true;
					break;
				}
			}
			pthread_mutex_unlock(&roomlist_lock);
			return found;
		}

	private:
		std::list<std::string> roomlist;
		pthread_mutex_t roomlist_lock;
};

std::string extensionname(gloox::StanzaExtensionType type)
{
	switch(type)
	{
		case gloox::ExtNone:
			return "none";
		case gloox::ExtVCardUpdate:
			return "vcard-update";
		case gloox::ExtOOB:
			return "oob";
		case gloox::ExtGPGSigned:
			return "gpg-signed";
		case gloox::ExtGPGEncrypted:
			return "gpg-encrypted";
		case gloox::ExtXDelay:
			return "xdelay";
		case gloox::ExtDelay:
			return "delay";
	}

	return std::string();
}

std::string statename(gloox::ConnectionState state)
{
	switch(state)
	{
		case gloox::StateDisconnected:
			return "disconnected";
		case gloox::StateConnecting:
			return "connecting";
		case gloox::StateConnected:
			return "connected";
	}

	return std::string();
}

std::string presencename(gloox::Presence presence)
{
	switch(presence)
	{
		case gloox::PresenceUnknown:
			return "unknown";
		case gloox::PresenceAvailable:
			return "available";
		case gloox::PresenceChat:
			return "available for chat";
		case gloox::PresenceAway:
			return "away";
		case gloox::PresenceDnd:
			return "busy";
		case gloox::PresenceXa:
			return "extended away";
		case gloox::PresenceUnavailable:
			return "offline";
	}

	return std::string();
}

std::string reasonname(gloox::ConnectionError reason)
{
	switch(reason)
	{
		case gloox::ConnNoError:
			return "no error";
		case gloox::ConnStreamError:
			return "stream error";
		case gloox::ConnStreamVersionError:
			return "stream version error";
		case gloox::ConnStreamClosed:
			return "stream closed";
		case gloox::ConnProxyAuthRequired:
			return "proxy auth required";
		case gloox::ConnProxyAuthFailed:
			return "proxy auth failed";
		case gloox::ConnProxyNoSupportedAuth:
			return "proxy no supported auth";
		case gloox::ConnIoError:
			return "io error";
		case gloox::ConnParseError:
			return "parse error";
		case gloox::ConnConnectionRefused:
			return "connection refused";
		case gloox::ConnDnsError:
			return "dns error";
		case gloox::ConnOutOfMemory:
			return "out of memory";
		case gloox::ConnNoSupportedAuth:
			return "no supported auth";
		case gloox::ConnTlsFailed:
			return "tls failed";
		case gloox::ConnTlsNotAvailable:
			return "tls not available";
		case gloox::ConnCompressionFailed:
			return "compression failed";
		case gloox::ConnAuthenticationFailed:
			return "authentication failed";
		case gloox::ConnUserDisconnected:
			return "user disconnected";
		case gloox::ConnNotConnected:
			return "not connected";
	}

	return std::string();
}

std::string statusname(int status)
{
	std::string res = "";

	if(status & gloox::CertOk)
		res += "+ok";
	if(status & gloox::CertInvalid)
		res += "+invalid";
	if(status & gloox::CertSignerUnknown)
		res += "+signer_unknown";
	if(status & gloox::CertRevoked)
		res += "+revoked";
	if(status & gloox::CertExpired)
		res += "+expired";
	if(status & gloox::CertNotActive)
		res += "+not_active";
	if(status & gloox::CertWrongPeer)
		res += "+wrong_peer";
	if(status & gloox::CertSignerNotCa)
		res += "+signer_not_ca";

	return res;
}

class ConnInfo
{
	public:
		ConnInfo(gloox::Client *client, std::string server, std::string jid, std::string roomname)
		{
			this->client = client;
			this->server = server;
			this->jid = jid;
			this->roomname = roomname;
		}

		void setcontrolinfo(std::string password)
		{
			this->password = password;
		}

		gloox::Client *client;
		std::string server;
		std::string jid;
		std::string password;
		std::string roomname;
};

// This class does not take ownership of the server objects
class GGZHandler
{
	public:
		GGZHandler(GGZServer *server, std::string roomname)
		{
			m_active = true;
			m_server = server;
			m_roomname = roomname;
			m_control = false;
		}

		bool active()
		{
			return m_active;
		}

		void setcontrol(bool control)
		{
			m_control = control;
		}

		bool iscontrol()
		{
			return m_control;
		}

		void serverevent(unsigned int id, const void *event_data)
		{
			GGZServerEvent event = (GGZServerEvent)id;
			std::string error;
			const GGZErrorEventData *data;

			switch(event)
			{
				case GGZ_CONNECTED:
					std::cout << "ggz: connected" << std::endl;
					break;
				case GGZ_CONNECT_FAIL:
					error = (char*)event_data;
					std::cerr << "ggz: connection failed: " << error << std::endl;
					m_active = false;
					break;
				case GGZ_NEGOTIATED:
					std::cout << "ggz: negotiated" << std::endl;
					ggzcore_server_login(m_server);
					break;
				case GGZ_NEGOTIATE_FAIL:
					error = (char*)event_data;
					std::cerr << "ggz: negotiation failed: " << error << std::endl;
					m_active = false;
					break;
				case GGZ_LOGGED_IN:
					std::cout << "ggz: logged in" << std::endl;
					// FIXME: we don't need the whole room list
					// unless we're the control connection
					// but the ggzcore API forces us to load all rooms
					ggzcore_server_list_rooms(m_server, -1, 0);
					break;
				case GGZ_LOGIN_FAIL:
					data = (const GGZErrorEventData*)event_data;
					error = data->message;
					std::cerr << "ggz: login failed: " << error << std::endl;
					m_active = false;
					break;
				case GGZ_MOTD_LOADED:
					// event_data: complex/motdeventdata
					// this event is not of interest
					break;
				case GGZ_ROOM_LIST:
					std::cout << "ggz: room list arrived" << std::endl;
					if(m_control)
					{
						Roomlist::self()->fill(m_server);
					}
					else
					{
						int num = Roomlist::self()->findpos(m_roomname);
						GGZRoom *room = ggzcore_server_get_nth_room(m_server, num);
						std::cout << "* going to room number " << num << std::endl;
						ggzcore_server_join_room(m_server, room);
					}
					break;
				case GGZ_TYPE_LIST:
					std::cout << "ggz: type list arrived" << std::endl;
					break;
				case GGZ_SERVER_PLAYERS_CHANGED:
					// event_data is unused
					// this event is not of interest
					break;
				case GGZ_ENTERED:
					std::cout << "ggz: entered a room" << std::endl;
					break;
				case GGZ_ENTER_FAIL:
					data = (const GGZErrorEventData*)event_data;
					error = data->message;
					std::cerr << "ggz: entering room failed: " << error << std::endl;
					m_active = false;
					break;
				case GGZ_LOGOUT:
					std::cout << "ggz: logged out" << std::endl;
					break;
				case GGZ_NET_ERROR:
					error = (char*)event_data;
					std::cerr << "ggz: network error: " << error << std::endl;
					m_active = false;
					break;
				case GGZ_PROTOCOL_ERROR:
					data = (const GGZErrorEventData*)event_data;
					error = data->message;
					std::cerr << "ggz: protocol error: " << error << std::endl;
					m_active = false;
					break;
				case GGZ_CHAT_FAIL:
					// event_data: complex/erroreventdata
					// this event is not of interest
					break;
				case GGZ_STATE_CHANGE:
					// event_data is unused
					break;
				case GGZ_CHANNEL_CONNECTED:
					// event_data is unused
					break;
				case GGZ_CHANNEL_READY:
					// event_data is unused
					break;
				case GGZ_CHANNEL_FAIL:
					// event_data: error string/const char*
					break;
				case GGZ_SERVER_ROOMS_CHANGED:
					// event_data is unused
					break;
				case GGZ_NUM_SERVER_EVENTS:
					// internal
					break;
			}
		}

	private:
		bool m_active;
		bool m_control;
		GGZServer *m_server;
		std::string m_roomname;
};

static GGZHookReturn hook_server(unsigned int id, const void *event_data, const void *user_data)
{
	const GGZHandler *handler_const = static_cast<const GGZHandler*>(user_data);
	GGZHandler *handler = const_cast<GGZHandler*>(handler_const);
	handler->serverevent(id, event_data);
	return GGZ_HOOK_OK;
}

// This function takes ownership of its argument 'info'
// ... but not of the contents of info!
static void *ggzthread(void *arg)
{
	std::cout << "-thread- started #" << pthread_self() << std::endl;

	ConnInfo *info = static_cast<ConnInfo*>(arg);

	GGZServer *server = ggzcore_server_new();
	GGZHandler *handler = new GGZHandler(server, info->roomname);

	ggzcore_server_add_event_hook_full(server, GGZ_CONNECTED, hook_server, handler);
	ggzcore_server_add_event_hook_full(server, GGZ_CONNECT_FAIL, hook_server, handler);
	ggzcore_server_add_event_hook_full(server, GGZ_NEGOTIATED, hook_server, handler);
	ggzcore_server_add_event_hook_full(server, GGZ_NEGOTIATE_FAIL, hook_server, handler);
	ggzcore_server_add_event_hook_full(server, GGZ_LOGGED_IN, hook_server, handler);
	ggzcore_server_add_event_hook_full(server, GGZ_LOGIN_FAIL, hook_server, handler);
	ggzcore_server_add_event_hook_full(server, GGZ_ROOM_LIST, hook_server, handler);
	ggzcore_server_add_event_hook_full(server, GGZ_TYPE_LIST, hook_server, handler);
	ggzcore_server_add_event_hook_full(server, GGZ_ENTERED, hook_server, handler);
	ggzcore_server_add_event_hook_full(server, GGZ_ENTER_FAIL, hook_server, handler);
	ggzcore_server_add_event_hook_full(server, GGZ_LOGOUT, hook_server, handler);
	ggzcore_server_add_event_hook_full(server, GGZ_NET_ERROR, hook_server, handler);
	ggzcore_server_add_event_hook_full(server, GGZ_PROTOCOL_ERROR, hook_server, handler);

	GGZLoginType mode;
	if(!info->password.empty())
	{
		mode = GGZ_LOGIN;
		handler->setcontrol(true);
	}
	else
	{
		mode = GGZ_LOGIN_GUEST;
	}

	// FIXME: jid might not always be valid as GGZ username
	ggzcore_server_set_hostinfo(server, info->server.c_str(), 5688, GGZ_CONNECTION_SECURE_OPTIONAL);
	ggzcore_server_set_logininfo(server, mode, info->jid.c_str(), info->password.c_str(), NULL);

	int success = ggzcore_server_connect(server);
	if(success == -1)
	{
		std::cerr << "Error: couldn't connect to GGZ server" << std::endl;
		return NULL;
	}

	// FIXME: main loop goes here
	while(handler->active())
		if(ggzcore_server_data_is_pending(server))
			ggzcore_server_read_data(server, ggzcore_server_get_fd(server));

	ggzcore_server_free(server);

	// Now send GGZ location to the user
	// FIXME: this requires 'client' to be thread-safe, but we could make it so easily
	if(!handler->iscontrol())
	{
		gloox::Stanza *stanza = gloox::Stanza::createMessageStanza(
			info->jid, "ggz on " + info->server);
		info->client->send(stanza);
	}

	delete info;

	std::cout << "-thread- ended #" << pthread_self() << std::endl;

	return NULL;
}

class GGZDJabberProxy : public gloox::ConnectionListener, gloox::PresenceHandler, gloox::MessageHandler
{
	public:
		GGZDJabberProxy()
		{
			m_client = NULL;
		}

		~GGZDJabberProxy()
		{
			delete m_client;
		}

		void config_jabber(std::string jid, std::string password)
		{
			m_jid = jid;
			m_password = password;
		}

		void config_ggz(std::string server, std::string login, std::string password)
		{
			m_server = server;
			m_ggzlogin = login;
			m_ggzpassword = password;
		}

		bool run()
		{

			GGZOptions opt;
			opt.flags = GGZ_OPT_PARSER;
			int success = ggzcore_init(opt);
			if(success == -1)
			{
				std::cerr << "Error: couldn't initialize ggzcore." << std::endl;
				return false;
			}

			gloox::JID jid(m_jid);

			m_client = new gloox::Client(jid, m_password);
			m_client->registerConnectionListener(this);
			m_client->registerPresenceHandler(this);
			m_client->registerMessageHandler(this);

			// FIXME: gloox should extend CertInfo to inform about CAs
			//gloox::StringList calist;
			//calist.push_back("/etc/ssl/certs/ca.pem");
			//m_client->setCACerts(calist);

			run_ggz(m_ggzlogin, std::string());

			// FIXME: set to false once ggzd connection works?
			bool ret = m_client->connect(/*false*/);
			std::cout << "Connection: " << ret << std::endl;

			delete Roomlist::self();

			return true;
		}

	private:
		void run_ggz(std::string jid, std::string roomname)
		{
			std::cout << "Connection thread for " << jid << std::endl;

			// FIXME: HACK to not run against ggzd's username length limit
			if(jid != m_ggzlogin)
			{
				jid = "masked-jid";
			}

			ConnInfo *info = new ConnInfo(m_client, m_server, jid, roomname);

			// if JID is the GGZ login, this becomes main connection
			if(jid == m_ggzlogin)
			{
				std::cout << "- is control thread!" << std::endl;
				info->setcontrolinfo(m_ggzpassword);
			}

			pthread_t thread;
			int success = pthread_create(&thread, NULL, ggzthread, info);
			if(success != 0)
			{
				std::cerr << "Error: couldn't spawn a thread." << std::endl;
				return;
			}
		}

		void onConnect()
		{
			std::cout << "-client- connected" << std::endl;
		}

		bool onTLSConnect(const gloox::CertInfo& info)
		{
			std::cout << "-client- tls connect" << std::endl;

			std::cout << "|| tls status:   " << statusname(info.status) << std::endl;
			std::cout << "|| tls chain:    " << info.chain << std::endl;
			std::cout << "|| tls issuer:   " << info.issuer << std::endl;
			std::cout << "|| tls server:   " << info.server << std::endl;
			std::cout << "|| tls protocol: " << info.protocol << std::endl;
			std::cout << "|| tls cipher:   " << info.cipher << std::endl;

			return true;
			// FIXME: This is very evil!
			//return false;
		}

		void onDisconnect(gloox::ConnectionError error)
		{
			std::cout << "-client- disconnected: " << reasonname(error) << std::endl;
		}

		void handlePresence(gloox::Stanza *stanza)
		{
			std::cout << "-client- presence: " << std::endl;
			std::cout << "|| sender:   " << stanza->from().full() << std::endl;
			std::cout << "|| presence: " << presencename(stanza->presence()) << std::endl;
		}

		void handleMessage(gloox::Stanza *stanza, gloox::MessageSession *session)
		{
			std::cout << "-client- message: " << std::endl;
			std::cout << "|| sender:    " << stanza->from().full() << std::endl;
			std::cout << "|| header:    " << stanza->subject() << std::endl;
			std::cout << "|| body:      " << stanza->body() << std::endl;

			gloox::StanzaExtensionList exts = stanza->extensions();
			gloox::StanzaExtensionList::const_iterator it;
			for(it = exts.begin(); it != exts.end(); it++)
			{
				gloox::StanzaExtension *ext = (*it);
				std::cout << "|| extension: " << extensionname(ext->type()) << std::endl;
			}

			if(stanza->body().substr(0, 4) == "play")
			{
				if(stanza->body().length() < 6)
				{
					gloox::Stanza *stanzareply = gloox::Stanza::createMessageStanza(
						stanza->from().bare(), "room must be given");
					m_client->send(stanzareply);
					return;
				}

				std::string roomname = stanza->body().substr(5);

				bool found = Roomlist::self()->contains(roomname);
				if(found)
				{
					run_ggz(stanza->from().bare(), roomname);
				}
				else
				{
					gloox::Stanza *stanzareply = gloox::Stanza::createMessageStanza(
						stanza->from().bare(), "room not found");
					m_client->send(stanzareply);
				}
			}
		}

		gloox::Client *m_client;
		std::string m_jid, m_password, m_server;
		std::string m_ggzlogin, m_ggzpassword;
};

class Config
{
	public:
		Config(){}

		bool load(std::string configfile)
		{
			char *jidstr, *passwordstr, *serverstr, *ggzloginstr, *ggzpasswordstr;

			int rc = ggz_conf_parse(configfile.c_str(), GGZ_CONF_RDONLY);
			if(rc == -1)
			{
				return false;
			}

			jidstr = ggz_conf_read_string(rc, "Jabber", "JID", NULL);
			passwordstr = ggz_conf_read_string(rc, "Jabber", "Password", NULL);
			serverstr = ggz_conf_read_string(rc, "GGZ", "Server", NULL);
			ggzloginstr = ggz_conf_read_string(rc, "GGZ", "Username", NULL);
			ggzpasswordstr = ggz_conf_read_string(rc, "GGZ", "Password", NULL);
			ggz_conf_close(rc);

			jid = jidstr;
			password = passwordstr;
			server = serverstr;
			ggzlogin = ggzloginstr;
			ggzpassword = ggzpasswordstr;
			ggz_free(jidstr);
			ggz_free(passwordstr);
			ggz_free(serverstr);
			ggz_free(ggzloginstr);
			ggz_free(ggzpasswordstr);

			return true;
		}

		std::string jid, password, server, ggzlogin, ggzpassword;
};

int main(int argc, char **argv)
{
	Config conf;
	if(!conf.load("ggzd-jabberproxy.rc"))
	{
		std::cerr << "Error: couldn't load config file ggzd-jabberproxy.rc." << std::endl;
		return -1;
	}

	std::cout << "Configuration loaded." << std::endl;
	std::cout << "|| jid=" << conf.jid << std::endl;
	std::cout << "|| password=(hidden)" << std::endl;
	std::cout << "|| ggzserver=" << conf.server << std::endl;
	std::cout << "|| ggzlogin=(hidden)" << conf.ggzlogin << std::endl;
	std::cout << "|| ggzpassword=(hidden)" << std::endl;

	GGZDJabberProxy proxy;
	proxy.config_jabber(conf.jid, conf.password);
	proxy.config_ggz(conf.server, conf.ggzlogin, conf.ggzpassword);
	bool ret = proxy.run();
	if(!ret)
	{
		std::cerr << "Error: the proxy was aborted unexpectedly." << std::endl;
		return -1;
	}

	return 0;
}

