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

#ifndef COMPCAL_SERIES_H
#define COMPCAL_SERIES_H

#include <qstring.h>
#include <qptrlist.h>

class Eventgroup;

class Series
{
	public:
		Series(QString organizer, QString registration, QString result, QString title, QString homepage);
		~Series();
		void addEventgroup(Eventgroup *eventgroup);

		QString title();
		QString registration();
		QString organizer();
		QString result();
		QString homepage();
		QPtrList<Eventgroup> eventgroups();

	private:
		QString m_organizer;
		QString m_title;
		QString m_registration;
		QPtrList<Eventgroup> m_eventgroups;
		QString m_homepage;
		QString m_result;
};

#endif

