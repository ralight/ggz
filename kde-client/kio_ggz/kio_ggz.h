#ifndef KIO_GGZ_H
#define KIO_GGZ_H

#include <kio/slavebase.h>
#include "GGZCoreServer.h"

class GGZCore;
class GGZCoreServer;

class GGZProtocol : public KIO::SlaveBase
{
	public:
		GGZProtocol(const QCString& pool, const QCString& app);
		~GGZProtocol();
		void get(const KURL& url);
		void listDir(const KURL& url);

		GGZCoreServer *server() {return m_server;}

	protected:
		static GGZHookReturn hook_server_connect(unsigned int id, const void *event, const void *data);
		static GGZHookReturn hook_server_negotiated(unsigned int id, const void *event, const void *data);
		static GGZHookReturn hook_server_login(unsigned int id, const void *event, const void *data);
		static GGZHookReturn hook_server_roomlist(unsigned int id, const void *event, const void *data);
		static GGZHookReturn hook_server_motd(unsigned int id, const void *event, const void *data);
		static GGZHookReturn hook_server_error(unsigned int id, const void *event, const void *data);

	private:
		void jobOperator(const KURL& url);
		/*void do_get(const KURL& url);
		void do_listServers(const KURL& url);
		void do_listRooms(const KURL& url);*/
		void init(const KURL& url);
		void debug(QString s);
		void error(QString s);

		void showMotd();

		GGZCore *m_core;
		GGZCoreServer *m_server;
		KIO::UDSEntry entry;
		QString savemotd;
};

#endif

