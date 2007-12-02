#include <gloox/client.h>
#include <gloox/tag.h>
#include <gloox/connectionbase.h>
#include <gloox/connectionlistener.h>
#include <gloox/presencehandler.h>
#include <gloox/messagehandler.h>
#include <gloox/stanzaextension.h>

#include <iostream>

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

class GGZJabber : public gloox::ConnectionListener, gloox::PresenceHandler, gloox::MessageHandler
{
	public:
		GGZJabber()
		{
			m_client = NULL;
		}

		~GGZJabber()
		{
			delete m_client;
		}

		void config_jabber(std::string jid, std::string password, std::string proxyjid)
		{
			m_jid = jid;
			m_password = password;
			m_proxyjid = proxyjid;
		}

		bool run()
		{

			GGZOptions opt;
			opt.flags = (GGZOptionFlags)(GGZ_OPT_PARSER | GGZ_OPT_MODULES);
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
		void run_ggz(std::string server)
		{
			std::cout << "Connect to GGZ server: " << server << std::endl;
		}

		void onConnect()
		{
			std::cout << "-client- connected" << std::endl;

			gloox::Stanza *stanza = gloox::Stanza::createMessageStanza(
				m_proxyjid, "play");
			m_client->send(stanza);
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

			if(stanza->body().substr(0, 6) == "ggz on")
				run_ggz(stanza->body().substr(7));
		}

		gloox::Client *m_client;
		std::string m_jid, m_password, m_proxyjid;
};

int main(int argc, char **argv)
{
	std::string jid, password, proxyjid;

	for(int i = 1; i < argc; i++)
	{
		std::string arg = argv[i];

		if((arg == "-h") || (arg == "--help"))
		{
			std::cout << "ggz-jabber [options...] -j <jid>" << std::endl;
			std::cout << std::endl;
			std::cout << "Options:" << std::endl;
			std::cout << "[-h | --help     ] Display help" << std::endl;
			std::cout << "[-j | --jid      ] Jabber ID to use for the connection" << std::endl;
			std::cout << "[-p | --password ] Password to use for the connection" << std::endl;
			std::cout << "[-P | --proxyjid ] Jabber ID of the ggzd-jabberproxy" << std::endl;
			return 0;
		}
		else if((arg == "-j") || (arg == "--jid"))
		{
			if(i < argc - 1)
			{
				jid = argv[i + 1];
				i++;
			}
			else
			{
				std::cerr << "Error: option jid needs an argument." << std::endl;
				return -1;
			}
		}
		else if((arg == "-P") || (arg == "--proxyjid"))
		{
			if(i < argc - 1)
			{
				proxyjid = argv[i + 1];
				i++;
			}
			else
			{
				std::cerr << "Error: option proxyjid needs an argument." << std::endl;
				return -1;
			}
		}
		else if((arg == "-p") || (arg == "--password"))
		{
			if(i < argc - 1)
			{
				password = argv[i + 1];
				i++;
			}
			else
			{
				std::cerr << "Error: option password needs an argument." << std::endl;
				return -1;
			}
		}
		else
		{
			std::cerr << "Error: unknown command line option " << arg << std::endl;
			return -1;
		}
	}

	if(jid.empty())
	{
		std::cerr << "Error: no Jabber ID has been set." << std::endl;
		return -1;
	}

	std::cout << "Parameters assigned." << std::endl;
	std::cout << "|| jid=" << jid << std::endl;
	std::cout << "|| password=(hidden)" << std::endl;

	GGZJabber jabberclient;
	jabberclient.config_jabber(jid, password, proxyjid);
	bool ret = jabberclient.run();
	if(!ret)
	{
		std::cerr << "Error: the client was aborted unexpectedly." << std::endl;
		return -1;
	}

	return 0;
}

