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

#include "series.h"

#include "eventgroup.h"

Series::Series(QString organizer, QString registration, QString result, QString title, QString homepage)
{
	m_organizer = organizer;
	m_registration = registration;
	m_title = title;
	m_result = result;
	m_homepage = homepage;
}

Series::~Series()
{
}

void Series::addEventgroup(Eventgroup *eventgroup)
{
	m_eventgroups.append(eventgroup);
}

QString Series::result()
{
	return m_result;
}

QString Series::homepage()
{
	return m_homepage;
}

QString Series::title()
{
	return m_title;
}

QString Series::organizer()
{
	return m_organizer;
}

QString Series::registration()
{
	return m_registration;
}

QPtrList<Eventgroup> Series::eventgroups()
{
	return m_eventgroups;
}

