#ifndef KIO_GGZ_H
#define KIO_GGZ_H

#include <kio/slavebase.h>

class GGZProtocol : public KIO::SlaveBase
{
	public:
		GGZProtocol(const QCString& pool, const QCString& app);
		~GGZProtocol();
		void get(const KURL& url);
		void listDir(const KURL& url);

	private:
		void jobOperator(const KURL& url);
		void do_get(const KURL& url);
		void do_listServers(const KURL& url);
		void do_listRooms(const KURL& url);
};

#endif

