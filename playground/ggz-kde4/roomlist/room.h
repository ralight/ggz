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

		void setDescription(QString description);
		void setLogo(QString logo);
		void setAccess(Access access);
		void setModule(bool module);
		void setFavourite(bool favourite);
		void setPlayers(int players);

		QString name();
		QString description();
		QString logo();
		Access access();
		bool module();
		bool favourite();
		int players();

	private:
		QString m_name;
		QString m_description;
		QString m_logo;
		Access m_access;
		bool m_module;
		bool m_favourite;
		int m_players;
};

#endif
