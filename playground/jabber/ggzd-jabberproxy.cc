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
		ConnInfo(gloox::Client *client, std::string server, std::string jid)
		{
			this->client = client;
			this->server = server;
			this->jid = jid;
		}

		gloox::Client *client;
		std::string server;
		std::string jid;
};

// This class does not take ownership of the server objects
class GGZHandler
{
	public:
		GGZHandler(GGZServer *server)
		{
			m_active = true;
			m_server = server;
		}

		bool active()
		{
			return m_active;
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
			}
		}

	private:
		bool m_active;
		GGZServer *m_server;
};

static GGZHookReturn hook_server(unsigned int id, const void *event_data, const void *user_data)
{
	const GGZHandler *handler_const = static_cast<const GGZHandler*>(user_data);
	GGZHandler *handler = const_cast<GGZHandler*>(handler_const);
	handler->serverevent(id, event_data);
}

// This function takes ownership of its argument 'info'
// ... but not of the contents of info!
static void *ggzthread(void *arg)
{
	std::cout << "-thread- started #" << pthread_self() << std::endl;

	ConnInfo *info = static_cast<ConnInfo*>(arg);

	GGZServer *server = ggzcore_server_new();
	GGZHandler *handler = new GGZHandler(server);

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

	ggzcore_server_set_hostinfo(server, info->server.c_str(), 5688, 0);
	ggzcore_server_set_logininfo(server, GGZ_LOGIN_GUEST, "jabberproxy", NULL, NULL);

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
	gloox::Stanza *stanza = gloox::Stanza::createMessageStanza(
		info->jid, "ggz on " + info->server);
	info->client->send(stanza);

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

		void config_ggz(std::string server)
		{
			m_server = server;
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

			// FIXME: set to false once ggzd connection works?
			bool ret = m_client->connect(/*false*/);
			std::cout << "Connection: " << ret << std::endl;
		}

	private:
		void run_ggz(std::string jid)
		{
			std::cout << "Connection thread for " << jid << std::endl;

			ConnInfo *info = new ConnInfo(m_client, m_server, jid);

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

			if(stanza->body() == "play")
				run_ggz(stanza->from().bare());
		}

		gloox::Client *m_client;
		std::string m_jid, m_password, m_server;
};

int main(int argc, char **argv)
{
	std::string jid, password, server;
	char *jidstr, *passwordstr, *serverstr;

	int rc = ggz_conf_parse("ggzd-jabberproxy.rc", GGZ_CONF_RDONLY);
	if(rc == -1)
	{
		std::cerr << "Error: couldn't load config file ggzd-jabberproxy.rc." << std::endl;
		return -1;
	}
	jidstr = ggz_conf_read_string(rc, "Jabber", "JID", NULL);
	passwordstr = ggz_conf_read_string(rc, "Jabber", "Password", NULL);
	serverstr = ggz_conf_read_string(rc, "GGZ", "Server", NULL);
	ggz_conf_close(rc);

	jid = jidstr;
	password = passwordstr;
	server = serverstr;
	ggz_free(jidstr);
	ggz_free(passwordstr);
	ggz_free(serverstr);

	std::cout << "Configuration loaded." << std::endl;
	std::cout << "|| jid=" << jid << std::endl;
	std::cout << "|| password=(hidden)" << std::endl;
	std::cout << "|| ggzserver=" << server << std::endl;

	GGZDJabberProxy proxy;
	proxy.config_jabber(jid, password);
	proxy.config_ggz(server);
	bool ret = proxy.run();
	if(!ret)
	{
		std::cerr << "Error: the proxy was aborted unexpectedly." << std::endl;
		return -1;
	}

	return 0;
}

