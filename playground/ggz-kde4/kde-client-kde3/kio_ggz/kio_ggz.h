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
		void stat(const KURL& url);

	protected:
		static GGZHookReturn hook_server_connect(unsigned int id, const void *event, const void *data);
		static GGZHookReturn hook_server_negotiated(unsigned int id, const void *event, const void *data);
		static GGZHookReturn hook_server_login(unsigned int id, const void *event, const void *data);
		static GGZHookReturn hook_server_roomlist(unsigned int id, const void *event, const void *data);
		static GGZHookReturn hook_server_motd(unsigned int id, const void *event, const void *data);
		static GGZHookReturn hook_server_error(unsigned int id, const void *event, const void *data);

	private:
		void jobOperator(const KURL& url);
		void init(const KURL& url);
		void debug(QString s);
		void errormessage(QString s);

		void showMotd();
		GGZCoreServer *server() {return m_server;}

		GGZCore *m_core;
		GGZCoreServer *m_server;
		KIO::UDSEntry entry;
		QString savemotd;
		bool m_finished;
};

#endif

