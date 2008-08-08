#ifndef KGGZLIB_GGZPROFILE_HEADER
#define KGGZLIB_GGZPROFILE_HEADER

#include <qstring.h>

#include "ggzserver.h"

class GGZProfile
{
	public:
		GGZProfile();
		~GGZProfile();

		enum LoginType
		{
			unconfigured,
			guest,
			firsttime,
			registered
		};

		void setLoginType(const LoginType type);
		LoginType loginType() const;

		void setUsername(const QString &username);
		QString username() const;

		void setPassword(const QString &password);
		QString password() const;

		void setRoomname(const QString &roomname);
		QString roomname() const;

		void setRealname(const QString &realname);
		QString realname() const;

		void setEmail(const QString &email);
		QString email() const;

		void setGGZServer(const GGZServer &server);
		GGZServer ggzServer() const;

		void assign(const GGZProfile profile);

		bool configured();

	private:
		GGZServer m_server;
		QString m_username;
		QString m_password;
		QString m_roomname;
		QString m_realname;
		QString m_email;
		LoginType m_logintype;
};

#endif
