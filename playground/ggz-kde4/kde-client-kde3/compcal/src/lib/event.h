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

#ifndef COMPCAL_EVENT_H
#define COMPCAL_EVENT_H

#include <qstring.h>
#include <qptrlist.h>

class Participant;
class Eventgroup;

class Event
{
	public:
		Event(int date, QString location, QString title, QString description, QString status, QString image);
		~Event();
		void addParticipant(Participant *participant);

		QString date();
		QString title();
		QString description();
		QString location();
		QString status();
		QString image();
		QPtrList<Participant> participants();

		void setParent(Eventgroup *parent);
		Eventgroup *parent();

		void setEventgroup(QString eventgroup);
		QString eventgroup();

	private:
		int m_date;
		QString m_title;
		QString m_description;
		QPtrList<Participant> m_participants;
		QString m_location;
		QString m_status;
		QString m_image;
		QString m_eventgroup;
		Eventgroup *m_parent;
};

#endif

