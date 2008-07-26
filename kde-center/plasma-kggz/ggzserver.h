#ifndef PLASMA_KGGZ_GGZSERVER_HEADER
#define PLASMA_KGGZ_GGZSERVER_HEADER

#include <qstring.h>

class GGZServer
{
	public:
		GGZServer();
		~GGZServer();

		void setUri(const QString &uri);
		QString uri() const;

	private:
		QString m_uri;
};

#endif
