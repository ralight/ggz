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

#ifndef COMPCAL_EVENTGROUP_H
#define COMPCAL_EVENTGROUP_H

#include <qstring.h>
#include <qptrlist.h>

class Event;

class Eventgroup
{
	public:
		Eventgroup(QString title, QString status, QString image, QString policy);
		~Eventgroup();
		void addEvent(Event *event);

		QString title();
		QString image();
		QString status();
		QString policy();
		QPtrList<Event> events();

		void setId(QString id);
		QString id();

	private:
		QString m_image;
		QString m_title;
		QString m_status;
		QPtrList<Event> m_events;
		QString m_policy;
		QString m_id;
};

#endif

