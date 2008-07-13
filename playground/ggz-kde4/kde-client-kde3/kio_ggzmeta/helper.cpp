#include "helper.h"

// These functions are stolen from the AudioCD slave
#include <sys/stat.h>

void GGZMetaProtocolHelper::app_entry(KIO::UDSEntry& e, unsigned int uds, const QString& str)
{
	KIO::UDSAtom a;
	a.m_uds = uds;
	a.m_str = str;
	e.append(a);
}
 
void GGZMetaProtocolHelper::app_entry(KIO::UDSEntry& e, unsigned int uds, long l)
{
	KIO::UDSAtom a;
	a.m_uds = uds;
	a.m_long = l;
	e.append(a);
}
 
void GGZMetaProtocolHelper::app_dir(KIO::UDSEntry& e, const QString & n, size_t s)
{
	e.clear();
	app_entry(e, KIO::UDS_NAME, n);
	app_entry(e, KIO::UDS_FILE_TYPE, S_IFDIR);
	app_entry(e, KIO::UDS_ACCESS, 0400);
	app_entry(e, KIO::UDS_SIZE, s);
}
 
void GGZMetaProtocolHelper::app_file(KIO::UDSEntry& e, const QString & n, size_t s, const QString & m)
{
	e.clear();
	app_entry(e, KIO::UDS_NAME, n);
	app_entry(e, KIO::UDS_FILE_TYPE, S_IFREG);
	app_entry(e, KIO::UDS_ACCESS, 0400);
	app_entry(e, KIO::UDS_SIZE, s);
	if(!m.isNull()) app_entry(e, KIO::UDS_MIME_TYPE, m);
}

// End of stolen code. I'm a thief :)
// But hey, if you're annoyed by that, simply believe in that these 4 little
// nice functions are needed quite often and should be part of kdelibs
// actually. Vote for this! app_entry for president!

