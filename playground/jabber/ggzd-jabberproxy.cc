#include <gloox/client.h>
#include <gloox/tag.h>
#include <gloox/connectionbase.h>
#include <gloox/connectionlistener.h>
#include <gloox/presencehandler.h>
#include <gloox/messagehandler.h>

#include <iostream>

#include <ggz.h>
#include <ggzcore.h>

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

		void config(std::string jid, std::string password)
		{
			m_jid = jid;
			m_password = password;
		}

		bool run()
		{

			GGZOptions opt;
			opt.flags = GGZ_OPT_PARSER;
			int success = ggzcore_init(opt);
			if(success == -1)
				return false;

			gloox::JID jid(m_jid);

			m_client = new gloox::Client(jid, m_password);
			m_client->registerConnectionListener(this);
			m_client->registerPresenceHandler(this);
			m_client->registerMessageHandler(this);

			// FIXME: set to false once ggzd connection works?
			bool ret = m_client->connect(/*false*/);
			std::cout << "Connection: " << ret << std::endl;
		}

		void runggz()
		{
			GGZServer *server = ggzcore_server_new();
			//ggzcore_server_add_event_hook(server, ...);
			ggzcore_server_set_hostinfo(server, "localhost", 5688, 0);
			ggzcore_server_set_logininfo(server, GGZ_LOGIN_GUEST, "jabberproxy", NULL, NULL);
			ggzcore_server_connect(server);

			ggzcore_server_free(server);
		}

		void onConnect()
		{
			std::cout << "-client- connect" << std::endl;
		}

		bool onTLSConnect(const gloox::CertInfo& info)
		{
			std::cout << "-client- tls connect" << std::endl;
			return true;
			// FIXME: This is very evil!
			//return false;
		}

		void onDisconnect(gloox::ConnectionError error)
		{
			std::cout << "-client- disconnect: " << reasonname(error) << std::endl;
		}

		void handlePresence(gloox::Stanza *stanza)
		{
			std::cout << "-client- presence: " << presencename(stanza->presence()) << std::endl;
		}

		void handleMessage(gloox::Stanza *stanza, gloox::MessageSession *session)
		{
			std::cout << "-client- message: " << stanza->body() << std::endl;
		}

	private:
		gloox::Client *m_client;
		std::string m_jid, m_password;
};

int main(int argc, char **argv)
{
	std::string jid, password;
	char *jidstr, *passwordstr;

	int rc = ggz_conf_parse("ggzd-jabberproxy.rc", GGZ_CONF_RDONLY);
	if(rc == -1)
		return -1;
	jidstr = ggz_conf_read_string(rc, "Jabber", "JID", NULL);
	passwordstr = ggz_conf_read_string(rc, "Jabber", "Password", NULL);
	ggz_conf_close(rc);

	jid = jidstr;
	password = passwordstr;
	ggz_free(jidstr);
	ggz_free(passwordstr);

	std::cout << "Configuration: " << jid << ":" << password << std::endl;

	GGZDJabberProxy proxy;
	proxy.config(jid, password);
	bool ret = proxy.run();
	if(!ret)
		return -1;

	return 0;
}

