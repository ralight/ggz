//
//    Competition Calendar
//
//    Copyright (C) 2002, 2003 Josef Spillner <josef@ggzgamingzone.org>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef COMPCAL_PARSER_H
#define COMPCAL_PARSER_H

#include <qptrlist.h>

class Series;
class Eventgroup;
class Event;
class Participant;
class QDomNode;

class Parser
{
	public:
		Parser(const char *file);
		~Parser();
		Series *series();

		enum ReturnCodes
		{
			normal,
			unknown,
			odd,
			invalid
		};

	private:
		int parse(const char *file);
		int parseSeries(QDomNode *pnode);
		int parseEventgroup(QDomNode *pnode);
		int parseEvent(QDomNode *pnode);
		int parseParticipants(QDomNode *pnode);

		void buildTree();

		QPtrList<Series> m_serieslist;
		QPtrList<Eventgroup> m_eventgrouplist;
		QPtrList<Event> m_eventlist;
		QPtrList<Participant> m_participantlist;

		Series *m_series;
};

#endif

