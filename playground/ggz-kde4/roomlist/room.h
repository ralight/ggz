#ifndef ROOM_H
#define ROOM_H

#include <qstring.h>

class Room
{
	public:
		Room(QString name);

		enum Access
		{
			Open,
			Locked
		};

		void setLogo(QString logo);
		void setAccess(Access access);
		void setModule(bool module);
		void setFavourite(bool favourite);

		QString name();
		QString logo();
		Access access();
		bool module();
		bool favourite();

	private:
		QString m_name;
		QString m_logo;
		Access m_access;
		bool m_module;
		bool m_favourite;
};

#endif
