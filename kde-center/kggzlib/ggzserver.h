#ifndef KGGZLIB_GGZSERVER_HEADER
#define KGGZLIB_GGZSERVER_HEADER

#include <qstring.h>

class GGZServer
{
	public:
		GGZServer();
		~GGZServer();

		void setUri(const QString &uri);
		QString uri() const;

		void setApi(const QString &api);
		QString api() const;

		void setName(const QString &name);
		QString name() const;

		void setIcon(const QString &icon);
		QString icon() const;

	private:
		QString m_uri;
		QString m_api;
		QString m_name;
		QString m_icon;
};

#endif
