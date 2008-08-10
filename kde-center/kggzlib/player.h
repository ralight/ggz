#ifndef PLAYER_H
#define PLAYER_H

#include <qstring.h>

#include "kggzlib_export.h"

class KGGZLIB_EXPORT Player
{
	public:
		Player(QString name);

		enum Relation
		{
			Unknown,
			Buddy,
			Ignored
		};

		enum Role
		{
			Guest,
			Registered,
			Host,
			Admin,
			Bot
		};

		void setStatistics(QString statistics);
		// percent 0..100
		void setLag(int lag);
		void setRelation(Relation relation);
		void setRole(Role role);

		QString name();
		QString statistics();
		int lag();
		Relation relation();
		Role role();

	private:
		QString m_name;
		QString m_statistics;
		int m_lag;
		Relation m_relation;
		Role m_role;
};

#endif
