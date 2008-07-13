#ifndef KIO_GGZMETA_HELPER_H
#define KIO_GGZMETA_HELPER_H

#include <kio/slavebase.h>

class GGZMetaProtocolHelper
{
	public:
		static void app_entry(KIO::UDSEntry & e, unsigned int uds, const QString& str);
		static void app_entry(KIO::UDSEntry & e, unsigned int uds, long l);
		static void app_dir(KIO::UDSEntry & e, const QString & n, size_t s);
		static void app_file(KIO::UDSEntry & e, const QString & n, size_t s, const QString & m);
};

#endif
